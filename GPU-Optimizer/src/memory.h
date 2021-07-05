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

	template<typename T>
	T Read(uint64_t address) const
	{
		T buf;
		ReadProcessMemory(m_process, (LPCVOID)address, &buf, sizeof(T), nullptr);
		return buf;
	}

	decltype(auto) ReadPtr(uint64_t address) const
	{
		return Read<uint64_t>(address);
	}

	template<typename T>
	void Write(uint64_t address, const T& value) const
	{
		WriteProcessMemory(m_process, (LPVOID)address, &value, sizeof(T), nullptr);
	}

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
