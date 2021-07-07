#pragma once

#include "memory.h"
#include "init.h"
#include "objectwithaddr.h"
#include "natives.h"

class MainLoop
{
public:
	MainLoop(const Memory* mem, const Init* init);

	void Run();

private:
	void ProcessLocalPlayer(Ped& localPlayer, PlayerInfo& localPlayerInfo);
	void Aimbot(const Ped& localPlayer, const PlayerInfo& localPlayerInfo, const std::vector<Ped>& peds, const viewport_t& viewport);

private:
	const Memory* m_mem;
	const Init* m_init;
};
