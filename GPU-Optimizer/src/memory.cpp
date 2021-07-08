#include "memory.h"

#include <TlHelp32.h>
#include <stdexcept>
#include <memory>

Memory::Memory(std::wstring_view processName, const std::vector<std::wstring_view>& windowNameContains,
	std::wstring_view moduleName, DWORD accessRights)
{
	auto pidOptional = GetPid(processName);
	if (!pidOptional)
	{
		throw std::runtime_error("Pid not found.");
	}
	auto pid = pidOptional.value();

	auto targetWindowOptional = ScanForWindow(pid, windowNameContains);
	if (!targetWindowOptional)
	{
		throw std::runtime_error("Target window not found.");
	}
	m_targetWindow = targetWindowOptional.value();

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

std::optional<HWND> Memory::ScanForWindow(DWORD pid, const std::vector<std::wstring_view>& windowNameContains)
{
	HWND currentWindow = NULL;
	do
	{
		currentWindow = FindWindowEx(NULL, currentWindow, NULL, NULL);
		DWORD currentWindowPid;
		GetWindowThreadProcessId(currentWindow, &currentWindowPid);
		if (currentWindowPid == pid)
		{
			for (auto windowNameSubstring : windowNameContains)
			{
				wchar_t currentWindowName[MAX_PATH];
				GetWindowText(currentWindow, currentWindowName, _countof(currentWindowName));
				if (std::wstring_view(currentWindowName).find(windowNameSubstring) != std::wstring_view::npos)
				{
					return currentWindow;
				}
			}
		}
	} while (currentWindow != NULL);
	return {};
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

std::optional<DWORD> Memory::GetPid(std::wstring_view processName) const
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
