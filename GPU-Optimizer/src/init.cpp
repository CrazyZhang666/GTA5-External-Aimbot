#include "init.h"

#include <stdexcept>

Init::Init(const Memory* mem)
{
	constexpr BYTE pattern[] = "\x48\x8D\x0D\x00\x00\x00\x00\x48\x8B\xD7\xE8\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\x8A\xD8\xE8\x00\x00\x00\x00\x84\xDB\x75\x13\x48\x8D\x0D\x00\x00\x00\x00";
	auto replayInterfaceRef = mem->PatternScan(mem->mainModule.base, mem->mainModule.size,
		pattern, "xxx????xxxx????xxx????xxx????xxxxxxx????", sizeof(pattern) - 1);
	if (!replayInterfaceRef)
	{
		throw std::runtime_error("Replay interface pattern not found.");
	}
	
	auto replayInterfaceInt = mem->Read<int32_t>(replayInterfaceRef.value() + 0x3);
	this->replayInterface = mem->ReadPtr(replayInterfaceRef.value() + replayInterfaceInt + 0x7);
}
