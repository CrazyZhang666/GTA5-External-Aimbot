#pragma once

#include <windows.h>
#include <string_view>
#include <optional>

class Memory
{
public:
	Memory(std::wstring_view processName, std::wstring_view moduleName);

private:
	struct Module
	{
		uint64_t base;
		DWORD size;
	};

	std::optional<DWORD> GetPid(std::wstring_view processName);
	std::optional<Module> GetModule(DWORD pid, std::wstring_view moduleName);

private:
	Module m_module;
};
