#pragma once

#include "natives.h"
#include "memory.h"

template<typename T>
struct ObjectWithAddr
{
	T obj;
	uint64_t addr;

	ObjectWithAddr()
	{
	}

	ObjectWithAddr(const Memory* mem, uint64_t addr)
	{
		InitWithAddr(mem, addr);
	}

	void InitWithAddr(const Memory* mem, uint64_t addr)
	{
		this->addr = addr;
		if (addr)
		{
			this->obj = mem->Read<T>(addr);
		}
	}
};

using Ped = ObjectWithAddr<ped_t>;
using Entity = ObjectWithAddr<entity_t>;
using PlayerInfo = ObjectWithAddr<player_info_t>;
using Weapon = ObjectWithAddr<weapon_t>;
