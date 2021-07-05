#include "mainloop.h"
#include "natives.h"

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
		auto pedInterface = m_mem->ReadPtr(m_init->replayInterface + 0x18);
		auto pedList = m_mem->ReadPtr(pedInterface + 0x100);
		auto pedCount = m_mem->Read<int32_t>(pedInterface + 0x110);

		system("cls");
		std::cout << pedCount << std::endl;
		for (int32_t i = 0; i < pedCount; i++)
		{
			auto pedPtr = m_mem->ReadPtr(pedList + 0x10ull * i);
			auto ped = m_mem->Read<ped_t>(pedPtr);

			if (ped.entity_type != 4)
			{
				continue;
			}

			std::cout << GetCorrectPedType(ped.ped_type) << ' ' << ped.health;
			if (ped.player_info)
			{
				auto player_info = m_mem->Read<player_info_t>(ped.player_info);
				std::cout << ' ' << player_info.name;
			}
			std::cout << std::endl;
		}


		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}
