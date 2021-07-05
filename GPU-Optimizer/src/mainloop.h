#pragma once

#include "memory.h"
#include "init.h"

class MainLoop
{
public:
	MainLoop(const Memory* mem, const Init* init);

	void Run();

private:
	const Memory* m_mem;
	const Init* m_init;
};
