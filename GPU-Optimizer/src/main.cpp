#include "memory.h"

#include <iostream>
#include <assert.h>

int main()
{
	try
	{
		auto mem = new Memory(L"GTA5.exe", L"GTA5.exe");
	}
	catch (const std::runtime_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}
