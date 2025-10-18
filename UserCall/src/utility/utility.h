#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>

class UtilityClass {
public:
    UtilityClass();                    
    ~UtilityClass() = default;

    BOOL SetUpConsole();

    BOOL FindTextSection(HMODULE Module, BYTE** TextStart, uint32_t* Size);
    BOOL GetProcessModules();

    HANDLE GetProcessHandle() const { return g_ProcessHandle; }
    DWORD  GetPid() const { return g_Pid; }
    const std::vector<MODULEENTRY32>& GetModuleList() const { return g_ModuleList; }
    BOOL isInitialized() const { return g_UtilityInitialized; }

private:
    HANDLE g_ProcessHandle = nullptr;
    DWORD  g_Pid = 0;
    std::vector<MODULEENTRY32> g_ModuleList;
    BOOL g_UtilityInitialized;
};

inline UtilityClass m_Utility;