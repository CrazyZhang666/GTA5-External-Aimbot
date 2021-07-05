#pragma once

#include <windows.h>
#include <string_view>
#include <optional>

class Memory
{
public:
	Memory(std::wstring_view processName, std::wstring_view moduleName, DWORD accessRights = PROCESS_ALL_ACCESS);

	~Memory();

	std::optional<uint64_t> PatternScan(uint64_t start, DWORD size, const BYTE* pattern, std::string_view mask, DWORD patternSize) const;

private:
	struct Module
	{
		uint64_t base;
		DWORD size;
	};

	std::optional<DWORD> GetPid(std::wstring_view processName) const;
	std::optional<Module> GetModule(DWORD pid, std::wstring_view moduleName) const;

public:
	Module mainModule;

private:
	HANDLE m_process;
};
