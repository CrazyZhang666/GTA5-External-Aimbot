#include "memory.h"

#include <TlHelp32.h>
#include <stdexcept>

Memory::Memory(std::wstring_view processName, std::wstring_view moduleName)
{
	auto pid = GetPid(processName);
	if (!pid)
	{
		throw std::runtime_error("Pid not found.");
	}

	auto module = GetModule(pid.value(), moduleName);
	if (!module)
	{
		throw std::runtime_error("Module not found.");
	}
}

std::optional<DWORD> Memory::GetPid(std::wstring_view processName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	std::optional<DWORD> pid;
	if (Process32First(snapshot, &entry))
	{
		do
		{
			if (_wcsicmp(entry.szExeFile, processName.data()) == 0)
			{
				pid = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &entry));
	}
	CloseHandle(snapshot);
	return pid;
}

std::optional<Memory::Module> Memory::GetModule(DWORD pid, std::wstring_view moduleName)
{
	std::optional<Module> module;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, moduleName.data()))
				{
					module = { (uint64_t)modEntry.modBaseAddr, modEntry.modBaseSize };
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return module;
}
