#include "mainloop.h"
#include "natives.h"
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
		std::this_thread::sleep_for(100us);

		if (!m_mem->IsTargetWindowValid())
		{
			throw std::runtime_error("Lost target window.");
		}

		if (!m_mem->IsTargetWindowMaximized())
		{
			continue;
		}

		auto viewport = m_mem->Read<viewport_t>(m_init->viewport);

		auto localPlayerPtr = m_mem->ReadPtr(m_init->world + 0x8);
		auto localPlayer = m_mem->Read<ped_t>(localPlayerPtr);
		ProcessLocalPlayer(localPlayer, localPlayerPtr);
		m_mem->Write(localPlayerPtr, localPlayer);

		auto pedInterface = m_mem->ReadPtr(m_init->replayInterface + 0x18);
		auto pedList = m_mem->ReadPtr(pedInterface + 0x100);
		auto pedCount = m_mem->Read<int32_t>(pedInterface + 0x110);

		std::optional<D3DXVECTOR2> closestFov;
		//system("cls");
		//std::cout << pedCount << std::endl;
		for (int32_t i = 0; i < pedCount; i++)
		{
			auto pedPtr = m_mem->ReadPtr(pedList + 0x10ull * i);
			auto ped = m_mem->Read<ped_t>(pedPtr);

			if (ped.entity_type != 4)
			{
				continue;
			}

			//std::cout << GetCorrectPedType(ped.ped_type) << ' ' << ped.health << ' ' << ped.armor;
			//std::cout << std::hex << ' ' << pedPtr << std::dec;

			auto screenPos = Utils::WorldToScreen(ped.pos, viewport);
			if (screenPos)
			{
				//std::cout << ' ' << screenPos->x << ' ' << screenPos->y;

				if (!closestFov || Utils::Distance(screenPos.value(), Utils::screenCenter) < Utils::Distance(closestFov.value(), Utils::screenCenter))
				{
					closestFov = screenPos;
				}
			}

			if (ped.player_info)
			{
				auto player_info = m_mem->Read<player_info_t>(ped.player_info);
				//std::cout << ' ' << "player" << ' ' << player_info.name;
			}
			if (pedPtr == localPlayerPtr)
			{
				//ped.bike_seatbelt = 0xC9;
				//ped.health = 200.0f;
				//m_mem->Write(pedPtr, ped);
				//std::cout << ' ' << "localPlayer";
			}
			//std::cout << std::endl;
		}

		if (closestFov && GetAsyncKeyState('X') & 0x8000)
		{
			auto delta = closestFov.value() - Utils::screenCenter;
			mouse_event(MOUSEEVENTF_MOVE, delta.x, delta.y, 0, 0);
		}
	}
}

void MainLoop::ProcessLocalPlayer(ped_t& localPlayer, uint64_t localPlayerPtr)
{
	// Set full HP (shift + alt + f5).
	//if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_LMENU) & 0x8000)
	if (GetAsyncKeyState(VK_LSHIFT) & 0x1)
	{
		localPlayer.health = 150.0f + rand() % 50;
		//m_mem->Write(localPlayerPtr, localPlayer);
	}
}
