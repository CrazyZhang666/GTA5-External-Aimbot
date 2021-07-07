#include "mainloop.h"
#include "natives.h"
#include "objectwithaddr.h"
#include "utils.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <array>

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
		
		Ped localPlayer(m_mem, m_mem->ReadPtr(m_init->world + 0x8));
		if (!localPlayer.addr)
		{
			continue;
		}
		PlayerInfo localPlayerInfo(m_mem, localPlayer.obj.player_info);
		ProcessLocalPlayer(localPlayer, localPlayerInfo);

		auto viewport = m_mem->Read<viewport_t>(m_init->viewport);
		auto pedInterface = m_mem->ReadPtr(m_init->replayInterface + 0x18);
		auto pedList = m_mem->ReadPtr(pedInterface + 0x100);
		auto pedCount = m_mem->Read<int32_t>(pedInterface + 0x110);

		std::vector<Ped> peds;
		for (int32_t i = 0; i < pedCount; i++)
		{
			Ped ped(m_mem, m_mem->ReadPtr(pedList + 0x10ull * i));
			if (ped.obj.entity_type == 4)
			{
				peds.push_back(ped);
			}
		}

		Aimbot(localPlayer, localPlayerInfo, peds, viewport);
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

void MainLoop::Aimbot(const Ped& localPlayer, const PlayerInfo& localPlayerInfo, const std::vector<Ped>& peds, const viewport_t& viewport)
{
	constexpr float maxDistance = 150.0f;
	constexpr float maxFov = 100.0f;
	constexpr std::array bonesToAim = { bone_types::HEAD, bone_types::STOMACH };

	static std::optional<std::pair<uint64_t, bone_types>> rememberedTarget; // pair (pedAddr, boneId)

	if (GetAsyncKeyState('C') & 0x8000 && localPlayerInfo.obj.is_aiming)
	{
		std::optional<std::tuple<uint64_t, bone_types, float, D3DXVECTOR2>> bestTarget;
		for (const auto& ped : peds)
		{
			if (rememberedTarget && ped.addr != rememberedTarget->first)
			{
				continue;
			}

			if (ped.addr == localPlayer.addr || Utils::Distance(ped.obj.pos, localPlayer.obj.pos) > maxDistance)
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
				pedAddr = ped.addr;
				boneId = bone;
				auto screenPosOptional = Utils::WorldToScreen(Utils::GetBonePosition(ped, (int)boneId), viewport);
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
			delta = delta / 2.0f + Utils::ClampVector2Length(delta, 2.5f);
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
