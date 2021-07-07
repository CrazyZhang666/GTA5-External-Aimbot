#include "memory.h"
#include "init.h"
#include "mainloop.h"

#include <iostream>
#include <assert.h>
#include <chrono>
#include <thread>

int main()
{
	while (true)
	{
		try
		{
			system("cls");
			auto mem = std::make_unique<Memory>(std::vector<std::wstring_view>{ L"Grand Theft Auto V", L"RAGE Multiplayer" }, L"GTA5.exe", PROCESS_ALL_ACCESS);
			auto init = std::make_unique<Init>(mem.get());
			auto mainLoop = std::make_unique<MainLoop>(mem.get(), init.get());
			mainLoop->Run();
		}
		catch (const std::runtime_error& err)
		{
			std::cout << err.what() << std::endl;
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
	}
}
