#pragma once

#include <windows.h>
#include <cstdint>
#include <d3dx9math.h>

#pragma pack(push, 1)
struct entity_t
{
	char pad_0x0000[0x28]; // 0x0000
	BYTE entity_type; // 0x0028
	char pad_0x0029[0x3]; // 0x0029
	BYTE invisible; // 0x002C
	char pad_0x002D[0x3]; // 0x002D
	uint64_t pos_base; // 0x0030
	char pad_0x0038[0x28]; // 0x0038
	char matrix[0x30]; // 0x0060
	D3DXVECTOR3 pos; // 0x0090
	char pad_0x009C[0xED]; // 0x009C
	BYTE god; // 0x0189
	char pad_0x018A[0xF6]; // 0x018A
	float health; // 0x0280
	char pad_0x0284[0x1C]; // 0x0284
	float max_health; // 0x02A0
};

struct ped_t : entity_t
{
	char pad_0x02A4[0x18C]; // 0x02A4
	D3DXVECTOR4 bones[12]; // 0x0430
	char pad_0x04F0[0x840]; // 0x04F0
	uint64_t vehicle; // 0x0D30
	char pad_0x0D38[0x380]; // 0x0D38
	uint32_t ped_type; // 0x10B8
	char pad_0x10BC[0xC]; // 0x10BC
	uint64_t player_info; // 0x10C8
	char pad_0x10D0[0x33C]; // 0x10D0
	BYTE bike_seatbelt; // 0x140C (0xC8 = off; 0xC9 = on)
	char pad_0x140D[0xB]; // 0x140D
	BYTE vehicle_seatbelt; // 0x1418 (0x01 = off; 0x00 = on)
	char pad_0x1419[0x5E]; // 0x1419
	BYTE in_vehicle; // 0x1477
	char pad_0x1478[0x68]; // 0x1478
	float armor; // 0x14E0
};

struct player_info_t
{
	char pad_0x0000[0xA4]; // 0x0000
	char name[0x14]; // 0x00A4
	char pad_0x00B8[0xD9B]; // 0x00B8
	BYTE is_aiming; // 0x0E53
};

struct viewport_t
{
	char pad_0x0000[0x24C]; // 0x0000
	D3DXMATRIX viewmatrix; // 0x024C
};

enum class ped_types
{
	PLAYER_0, // michael
	PLAYER_1, // franklin
	NETWORK_PLAYER, // mp character
	PLAYER_2, // trevor
	CIVMALE,
	CIVFEMALE,
	COP,
	GANG_ALBANIAN,
	GANG_BIKER_1,
	GANG_BIKER_2,
	GANG_ITALIAN,
	GANG_RUSSIAN,
	GANG_RUSSIAN_2,
	GANG_IRISH,
	GANG_JAMAICAN,
	GANG_AFRICAN_AMERICAN,
	GANG_KOREAN,
	GANG_CHINESE_JAPANESE,
	GANG_PUERTO_RICAN,
	DEALER,
	MEDIC,
	FIREMAN,
	CRIMINAL,
	BUM,
	PROSTITUTE,
	SPECIAL,
	MISSION,
	SWAT,
	ANIMAL,
	ARMY
};

enum class bone_types
{
	HEAD = 0,
	NECK = 7,
	STOMACH = 8,
};
#pragma pack(pop)
