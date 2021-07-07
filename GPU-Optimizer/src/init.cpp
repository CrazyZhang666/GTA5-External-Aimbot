#include "init.h"

#include <stdexcept>

Init::Init(const Memory* mem)
{
	InitWorld(mem);
	InitReplayInterface(mem);
	InitViewport(mem);
}

void Init::InitWorld(const Memory* mem)
{
	constexpr BYTE pattern[] = "\x48\x8B\x05\x00\x00\x00\x00\x45\x00\x00\x00\x00\x48\x8B\x48\x08\x48\x85\xC9\x74\x07\x0F\x28";
	auto worldRef = mem->PatternScan(mem->mainModule.base, mem->mainModule.size,
		pattern, "xxx????x????xxxxxxxxxxx", sizeof(pattern) - 1);
	if (!worldRef)
	{
		throw std::runtime_error("World pattern not found.");
	}

	auto worldInt = mem->Read<int32_t>(worldRef.value() + 0x3);
	world = mem->ReadPtr(worldRef.value() + worldInt + 0x7);
	if (!world)
	{
		throw std::runtime_error("World not found.");
	}
}

void Init::InitReplayInterface(const Memory* mem)
{
	constexpr BYTE pattern[] = "\x48\x8D\x0D\x00\x00\x00\x00\x48\x8B\xD7\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\x8A\xD8\xE8\x00\x00\x00\x00\x84\xDB\x75\x13\x48\x8D\x0D\x00\x00\x00\x00";
	auto replayInterfaceRef = mem->PatternScan(mem->mainModule.base, mem->mainModule.size,
		pattern, "xxx????xxxx????xxx????xxx????xxxxxxx????", sizeof(pattern) - 1);
	if (!replayInterfaceRef)
	{
		throw std::runtime_error("Replay interface pattern not found.");
	}

	auto replayInterfaceInt = mem->Read<int32_t>(replayInterfaceRef.value() + 0x3);
	replayInterface = mem->ReadPtr(replayInterfaceRef.value() + replayInterfaceInt + 0x7);
	if (!replayInterface)
	{
		throw std::runtime_error("Replay interface not found.");
	}
}

void Init::InitViewport(const Memory* mem)
{
	constexpr BYTE pattern[] = "\x48\x8B\x15\x00\x00\x00\x00\x48\x8D\x2D\x00\x00\x00\x00\x48\x8B\xCD";
	auto viewportRef = mem->PatternScan(mem->mainModule.base, mem->mainModule.size,
		pattern, "xxx????xxx????xxx", sizeof(pattern) - 1);
	if (!viewportRef)
	{
		throw std::runtime_error("Viewport pattern not found.");
	}

	auto viewportInt = mem->Read<int32_t>(viewportRef.value() + 0x3);
	viewport = mem->ReadPtr(viewportRef.value() + viewportInt + 0x7);
	if (!viewport)
	{
		throw std::runtime_error("Viewport not found.");
	}
}
