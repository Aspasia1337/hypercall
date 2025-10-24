#include "process_manager.h"
#include "logger.h"

ProcessManager::ProcessManager() {
    g_Initialized = SetUpConsole();

    g_ProcessHandle = GetModuleHandle(NULL);
    g_Pid = GetCurrentProcessId();

    if (!GetProcessModules()) {
        Logger::Log(LogLevel::LOG_ERROR, "[ProcessManager] Failed to enumerate modules");
        g_Initialized = FALSE;
    }
}

BOOL ProcessManager::SetUpConsole() {
    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);
        SetConsoleTitleW(L"[+] USER LOGGER [+]");
        return TRUE;
    }
    return FALSE;
}

BOOL ProcessManager::GetProcessModules() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, g_Pid);
    MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };

    if (snapshot == INVALID_HANDLE_VALUE)
        return FALSE;

    g_ModuleList.clear();
    if (Module32First(snapshot, &me32)) {
        do {
            g_ModuleList.push_back(me32);
        } while (Module32Next(snapshot, &me32));
    }
    CloseHandle(snapshot);

    return !g_ModuleList.empty();
}

BOOL ProcessManager::FindTextSection(HMODULE Module, BYTE** TextStart, uint32_t* Size) {
    if (!Module || !TextStart || !Size)
        return FALSE;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)Module;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;

    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE*)Module + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return FALSE;

    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        if (memcmp(section->Name, ".text", 5) == 0) {
            *TextStart = (BYTE*)Module + section->VirtualAddress;
            *Size = section->Misc.VirtualSize;
            return TRUE;
        }
    }

    return FALSE;
}

HMODULE ProcessManager::GetHandle() const { return g_ProcessHandle; }
DWORD ProcessManager::GetPid() const { return g_Pid; }
const std::vector<MODULEENTRY32>& ProcessManager::GetModuleList() const { return g_ModuleList; }