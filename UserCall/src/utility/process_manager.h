#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

class ProcessManager {
public:
    ProcessManager();
    BOOL SetUpConsole();
    BOOL GetProcessModules();
    BOOL FindTextSection(HMODULE Module, BYTE** TextStart, uint32_t* Size);

    HMODULE GetHandle() const;
    DWORD GetPid() const;
    const std::vector<MODULEENTRY32>& GetModuleList() const;

private:
    HMODULE g_ProcessHandle = nullptr;
    DWORD g_Pid = 0;
    std::vector<MODULEENTRY32> g_ModuleList;
    BOOL g_Initialized = TRUE;
};