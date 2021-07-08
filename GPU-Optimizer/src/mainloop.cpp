#include "mainloop.h"
#include "natives.h"
#include "objectwithaddr.h"
#include "utils.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <array>
#include <unordered_map>

MainLoop::MainLoop(const Memory* mem, const Init* init)
	: m_mem(mem), m_init(init)
{
}

uint32_t GetCorrectPedType(uint32_t pedType)
{
	return pedType << 11 >> 25;
}

void MainLoop::Run()
{
	while (true)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1000ms / 60);

		if (!m_mem->IsTargetWindowValid())
		{
			throw std::runtime_error("Lost target window.");
		}

		if (!m_mem->IsTargetWindowMaximized())
		{
			continue;
		}
		
		auto localPlayer = std::make_unique<Ped>(m_mem, m_mem->ReadPtr(m_init->world + 0x8));
		if (!localPlayer->addr)
		{
			continue;
		}
		auto localPlayerInfo = std::make_unique<PlayerInfo>(m_mem, localPlayer->obj.player_info);
		ProcessLocalPlayer(*localPlayer, *localPlayerInfo);
		WeaponHacks(*localPlayer);

		auto viewport = m_mem->Read<viewport_t>(m_init->viewport);
		auto pedInterface = m_mem->ReadPtr(m_init->replayInterface + 0x18);
		auto pedList = m_mem->ReadPtr(pedInterface + 0x100);
		auto pedCount = m_mem->Read<int32_t>(pedInterface + 0x110);

		std::vector<std::unique_ptr<Ped>> peds;
		for (int32_t i = 0; i < pedCount; i++)
		{
			auto ped = std::make_unique<Ped>(m_mem, m_mem->ReadPtr(pedList + 0x10ull * i));
			if (ped->obj.entity_type == 4)
			{
				peds.push_back(std::move(ped));
			}
		}

		Aimbot(*localPlayer, *localPlayerInfo, peds, viewport);
	}
}

void MainLoop::WeaponHacks(Ped& localPlayer)
{
	constexpr float spreadModifier = 0.4f;
	constexpr float recoilModifier = 0.4f;

	auto weaponManager = m_mem->Read<weapon_manager_t>(localPlayer.obj.weapon_manager);
	if (!weaponManager.current_weapon)
	{
		return;
	}
	auto currentWeapon = std::make_unique<Weapon>(m_mem, weaponManager.current_weapon);

	struct WeaponModifiable
	{
		float spread;
		float recoil;
	};

	static std::unordered_map<decltype(weapon_t::name_hash), WeaponModifiable> originalValues;
	if (!originalValues.count(currentWeapon->obj.name_hash))
	{
		originalValues[currentWeapon->obj.name_hash] = { currentWeapon->obj.spread, currentWeapon->obj.recoil };
	}

	const auto& currentOriginal = originalValues[currentWeapon->obj.name_hash];
	WeaponModifiable currentModified = { currentOriginal.spread * spreadModifier, currentOriginal.recoil * recoilModifier };
	if (currentWeapon->obj.spread != currentModified.spread)
	{
		m_mem->Write<float>(currentWeapon->addr + offsetof(weapon_t, spread), currentModified.spread);
	}
	if (currentWeapon->obj.recoil != currentModified.recoil)
	{
		m_mem->Write<float>(currentWeapon->addr + offsetof(weapon_t, recoil), currentModified.recoil);
	}
}

void MainLoop::ProcessLocalPlayer(Ped& localPlayer, PlayerInfo& localPlayerInfo)
{
	// Set full HP (shift + alt + f5).
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_LMENU) & 0x8000 && GetAsyncKeyState(VK_F5) & 0x1)
	{
		localPlayer.obj.health = localPlayer.obj.max_health;
		m_mem->Write(localPlayer.addr + offsetof(ped_t, health), localPlayer.obj.health);
	}
}

void MainLoop::Aimbot(const Ped& localPlayer, const PlayerInfo& localPlayerInfo, const std::vector<std::unique_ptr<Ped>>& peds, const viewport_t& viewport)
{
	constexpr float maxDistance = 200.0f;
	constexpr float maxFov = 75.0f;
	constexpr float velocityMultiplier = 0.05f;
	constexpr std::array bonesToAim = { bone_types::HEAD, bone_types::STOMACH };

	static std::optional<std::pair<uint64_t, bone_types>> rememberedTarget; // pair (pedAddr, boneId)

	if (GetAsyncKeyState('C') & 0x8000 && localPlayerInfo.obj.is_aiming)
	{
		std::optional<std::tuple<uint64_t, bone_types, float, D3DXVECTOR2>> bestTarget;
		for (const auto& ped : peds)
		{
			if (rememberedTarget && ped->addr != rememberedTarget->first)
			{
				continue;
			}

			if (ped->addr == localPlayer.addr || ped->obj.health <= 0.0f || Utils::Distance(ped->obj.pos, localPlayer.obj.pos) > maxDistance)
			{
				continue;
			}

			for (auto bone : bonesToAim)
			{
				//if (rememberedTarget && bone != rememberedTarget->second)
				//{
				//	continue;
				//}

				decltype(bestTarget)::value_type current;
				auto& [pedAddr, boneId, fov, delta] = current;
				pedAddr = ped->addr;
				boneId = bone;
				auto velocityOffset = ped->obj.velocity * velocityMultiplier;
				auto screenPosOptional = Utils::WorldToScreen(Utils::GetBonePosition(*ped, (int)boneId) + velocityOffset, viewport);
				if (!screenPosOptional)
				{
					continue;
				}
				delta = screenPosOptional.value() - Utils::screenCenter;
				fov = D3DXVec2Length(&delta);

				if (fov > maxFov)
				{
					continue;
				}

				if (!bestTarget || fov < std::get<2>(bestTarget.value()))
				{
					bestTarget = current;
				}
			}
		}

		if (bestTarget)
		{
			auto [pedAddr, boneId, fov, delta] = bestTarget.value();
			auto deltaLength = D3DXVec2Length(&delta);
			delta = delta / 3.0f + Utils::ClampVector2Length(delta, 2.5f);
			delta = Utils::ClampVector2Length(delta, deltaLength);
			mouse_event(MOUSEEVENTF_MOVE, (DWORD)delta.x, (DWORD)delta.y, 0, 0);
			rememberedTarget = std::make_pair(pedAddr, boneId);
		}
	}
	else
	{
		rememberedTarget.reset();
	}
}
