#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip> 

#include "../Integrity/integrity.h"


enum LogLevel : int{
    LOG_ERROR = 0,
    LOG_SUCCESS,
    LOG_INFO,
};

class UtilityClass {
public:

    UtilityClass();                    
    ~UtilityClass() = default;

    BOOL SetUpConsole();

    BOOL FindTextSection(HMODULE Module, BYTE** TextStart, uint32_t* Size);
    BOOL GetProcessModules();

    void Log(LogLevel level, const char* message, ...);

    HANDLE GetProcessHandle() const { return g_ProcessHandle; }
    DWORD  GetPid() const { return g_Pid; }
    const std::vector<MODULEENTRY32>& GetModuleList() const { return g_ModuleList; }
    BOOL isInitialized() const { return g_UtilityInitialized; }
    BOOL HashModules();

private:
    HANDLE g_ProcessHandle = nullptr;
    DWORD  g_Pid = 0;
    std::vector<MODULEENTRY32> g_ModuleList;
    std::vector<std::array<BYTE, 32>> g_Hashes;
    BOOL g_UtilityInitialized = 1;
};

inline UtilityClass m_Utility;