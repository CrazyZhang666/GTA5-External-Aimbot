#include "memory.h"

#include <TlHelp32.h>
#include <stdexcept>
#include <memory>

Memory::Memory(std::vector<std::wstring_view>&& windowNames, std::wstring_view moduleName, DWORD accessRights)
{
	m_targetWindow = 0;
	for (auto windowName : windowNames)
	{
		if (m_targetWindow = FindWindow(nullptr, windowName.data()))
		{
			break;
		}
	}
	if (!m_targetWindow)
	{
		throw std::runtime_error("Target window not found.");
	}

	DWORD pid;
	GetWindowThreadProcessId(m_targetWindow, &pid);
	if (!pid)
	{
		throw std::runtime_error("Pid not found.");
	}

	auto module = GetModule(pid, moduleName);
	if (!module)
	{
		throw std::runtime_error("Module not found.");
	}
	mainModule = std::move(module.value());

	m_process = OpenProcess(accessRights, false, pid);
	if (!m_process)
	{
		throw std::runtime_error("Failed to obtain handle.");
	}
}

Memory::~Memory()
{
	CloseHandle(m_process);
}

bool CheckPattern(const BYTE* data, const BYTE* pattern, std::string_view mask, DWORD patternSize)
{
	for (DWORD i = 0; i < patternSize; i++)
	{
		if (mask[i] != '?' && pattern[i] != *(data + i))
		{
			return false;
		}
	}
	return true;
}

std::optional<uint64_t> Memory::PatternScan(uint64_t start, DWORD size, const BYTE* pattern, std::string_view mask, DWORD patternSize) const
{
	auto data = std::make_unique<BYTE[]>(size);
	ReadProcessMemory(m_process, (LPCVOID)start, data.get(), size, nullptr);
	for (uint64_t i = 0; i <= size - patternSize; i++)
	{
		if (CheckPattern(data.get() + i, pattern, mask, patternSize))
		{
			return start + i;
		}
	}
	return {};
}

bool Memory::IsTargetWindowMaximized() const
{
	return GetForegroundWindow() == m_targetWindow;
}

bool Memory::IsTargetWindowValid() const
{
	return IsWindow(m_targetWindow);
}

std::optional<Memory::Module> Memory::GetModule(DWORD pid, std::wstring_view moduleName) const
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
