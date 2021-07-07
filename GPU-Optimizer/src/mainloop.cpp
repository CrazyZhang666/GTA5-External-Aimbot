#include "mainloop.h"
#include "natives.h"
#include "objectwithaddr.h"
#include "utils.h"

#include <iostream>
#include <chrono>
#include <thread>

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
		ProcessLocalPlayer(localPlayer);

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

		Aimbot(localPlayer, peds, viewport);
	}
}

void MainLoop::ProcessLocalPlayer(Ped& localPlayer)
{
	// Set full HP (shift + alt + f5).
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_LMENU) & 0x8000 && GetAsyncKeyState(VK_F5) & 0x1)
	{
		localPlayer.obj.health = localPlayer.obj.max_health;
		m_mem->Write(localPlayer.addr + offsetof(ped_t, health), localPlayer.obj.health);
	}
}

void MainLoop::Aimbot(const Ped& localPlayer, const std::vector<Ped>& peds, const viewport_t& viewport)
{
	constexpr float maxDistance = 150.0f;
	constexpr float maxFov = 100.0f;

	if (!(GetAsyncKeyState('X') & 0x8000))
	{
		return;
	}

	std::optional<std::tuple<const Ped*, float, D3DXVECTOR2>> bestTarget;
	for (const auto& ped : peds)
	{
		if (ped.addr == localPlayer.addr || Utils::Distance(ped.obj.pos, localPlayer.obj.pos) > maxDistance)
		{
			continue;
		}

		decltype(bestTarget)::value_type current;
		auto& [pedPtr, fov, delta] = current;
		pedPtr = &ped;
		auto screenPosOptional = Utils::WorldToScreen(ped.obj.pos, viewport);
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

		if (!bestTarget || fov < std::get<1>(bestTarget.value()))
		{
			bestTarget = current;
		}
	}

	if (bestTarget)
	{
		auto [pedPtr, fov, delta] = bestTarget.value();
		auto deltaLength = D3DXVec2Length(&delta);
		delta = delta / 2.5f + Utils::ClampVector2Length(delta, 2.5f);
		delta = Utils::ClampVector2Length(delta, deltaLength);
		mouse_event(MOUSEEVENTF_MOVE, (DWORD)delta.x, (DWORD)delta.y, 0, 0);
	}
}
