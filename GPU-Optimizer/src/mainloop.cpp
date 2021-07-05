#include "mainloop.h"

#include <iostream>
#include <chrono>
#include <thread>

MainLoop::MainLoop(const Memory* mem, const Init* init)
	: m_mem(mem), m_init(init)
{
}

void MainLoop::Run()
{
	while (true)
	{
		auto pedInterface = m_mem->ReadPtr(m_init->replayInterface + 0x18);
		auto pedCount = m_mem->Read<int32_t>(pedInterface + 0x110);
		system("cls");
		std::cout << pedCount << std::endl;

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}
