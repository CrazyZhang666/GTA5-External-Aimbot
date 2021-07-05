#include "memory.h"
#include "init.h"

#include <iostream>
#include <assert.h>

int main()
{
	try
	{
		auto mem = std::make_unique<Memory>(L"GTA5.exe", L"GTA5.exe", PROCESS_VM_READ);
		auto init = std::make_unique<Init>(mem.get());

		std::cout << std::hex << init->replayInterface << std::endl;
	}
	catch (const std::runtime_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}
