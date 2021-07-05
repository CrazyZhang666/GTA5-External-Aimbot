#include "memory.h"
#include "init.h"
#include "mainloop.h"

#include <iostream>
#include <assert.h>

int main()
{
	try
	{
		auto mem = std::make_unique<Memory>(L"GTA5.exe", L"GTA5.exe", PROCESS_VM_READ);
		auto init = std::make_unique<Init>(mem.get());
		auto mainLoop = std::make_unique<MainLoop>(mem.get(), init.get());
		mainLoop->Run();
	}
	catch (const std::runtime_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}
