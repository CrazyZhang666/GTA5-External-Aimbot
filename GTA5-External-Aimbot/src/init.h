#pragma once

#include "memory.h"

class Init
{
public:
	Init(const Memory* mem);

private:
	void InitWorld(const Memory* mem);
	void InitReplayInterface(const Memory* mem);
	void InitViewport(const Memory* mem);

public:
	uint64_t world;
	uint64_t replayInterface;
	uint64_t viewport;
};
