#pragma once

#include "memory.h"
#include "init.h"
#include "natives.h"

class MainLoop
{
public:
	MainLoop(const Memory* mem, const Init* init);

	void Run();

private:
	void ProcessLocalPlayer(ped_t& localPlayer, uint64_t localPlayerPtr);

private:
	const Memory* m_mem;
	const Init* m_init;
};
