#include "utility.h"

BOOL g_UtilityInitialized = TRUE;
HMODULE g_ProcessHandle;
DWORD g_Pid;
std::vector<MODULEENTRY32> g_ModuleList;



void UtilityClass::Log(LogLevel level, const char* message, ...)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (level) {
	case LOG_ERROR:
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case LOG_SUCCESS:
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case LOG_INFO:
	default:
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	}

	const char* tag =
		(level == LOG_ERROR) ? "[-]" :
		(level == LOG_SUCCESS) ? "[+]" :
		"[INFO]";

	printf("%s ", tag);

	va_list args;
	va_start(args, message);
	vprintf(message, args);
	va_end(args);
	printf("\n");

	SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

UtilityClass::UtilityClass()
{
	g_ModuleList.clear();
	

	if (!SetUpConsole()) {
		UtilityClass::Log(LOG_ERROR, "[UTILITY] Error setting up console");
		g_UtilityInitialized = FALSE;
	}
	else {
		UtilityClass::Log(LOG_SUCCESS, "[UTILITY] Console initialized");
	}

	g_ProcessHandle = GetModuleHandle(NULL);
	if (g_ProcessHandle == INVALID_HANDLE_VALUE) {
		UtilityClass::Log(LOG_ERROR, "[UTILITY] Error initializing Userland - ProcessHandle");
		g_UtilityInitialized = FALSE;
	}
	else {
		UtilityClass::Log(LOG_SUCCESS, "[UTILITY] Process handle OK (%p)", g_ProcessHandle);
	}

	g_Pid = GetCurrentProcessId();
	if (g_Pid == 0) {
		UtilityClass::Log(LOG_ERROR, "[UTILITY] Error initializing Userland - ProcessId");
		g_UtilityInitialized = FALSE;
	}
	else {
		UtilityClass::Log(LOG_SUCCESS, "[UTILITY] Process ID: %lu", g_Pid);
	}

	if (!GetProcessModules()) {
		UtilityClass::Log(LOG_ERROR, "[UTILITY] Error enumerating process modules");
		g_UtilityInitialized = FALSE;
	}
	else {
		UtilityClass::Log(LOG_SUCCESS, "[UTILITY] Module enumeration completed successfully");
	}
}

BOOL UtilityClass::FindTextSection(HMODULE Module, BYTE** TextStart, uint32_t* Size)
{
	if (!Module || !TextStart || !Size)
		return FALSE;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)Module;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;

	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)Module + dosHeader->e_lfanew);
	if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
		return FALSE;

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

	for (uint32_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++)
	{
		if (memcmp(section->Name, ".text", 5) == 0)
		{
			*TextStart = (BYTE*)Module + section->VirtualAddress;
			*Size = section->Misc.VirtualSize;
			//Log(LOG_INFO, "[UTILITY] Found .text section at %p (size: %u bytes)", *TextStart, *Size);
			return TRUE;
		}
	}

	UtilityClass::Log(LOG_ERROR, "[UTILITY] .text section not found in module");
	return FALSE;
}

BOOL UtilityClass::GetProcessModules()
{
	HANDLE snapshotHandle;
	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, g_Pid);
	if (snapshotHandle == INVALID_HANDLE_VALUE) {
		UtilityClass::Log(LOG_ERROR, "[UTILITY] Error getting process modules");
		return FALSE;
	}

	g_ModuleList.clear();

	if (Module32First(snapshotHandle, &moduleEntry)) {
		do {
			//Log(LOG_INFO, "[MODULE] %ws", moduleEntry.szExePath);
			g_ModuleList.push_back(moduleEntry);
		} while (Module32Next(snapshotHandle, &moduleEntry));
	}

	CloseHandle(snapshotHandle);

	if (g_ModuleList.empty()) {
		UtilityClass::Log(LOG_ERROR, "[UTILITY] No modules found");
		return FALSE;
	}

	UtilityClass::Log(LOG_SUCCESS, "[UTILITY] Total modules: %zu", g_ModuleList.size());
	return TRUE;
}

BOOL UtilityClass::SetUpConsole()
{
	if (AllocConsole()) {
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		freopen_s(&fp, "CONIN$", "r", stdin);

		SetConsoleTitleW(L" \t \t [+]USERCALL LOGGER[+]");
		return TRUE;
	}
	return FALSE;
}

static void PrintHash(const std::array<BYTE, 32>& hash) {
	for (BYTE b : hash) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
	}
	std::cout << std::dec << std::endl; 
}

BOOL UtilityClass::HashModules() {
	if (g_ModuleList.empty()) {
		m_Utility.Log(LOG_INFO, "No modules to hash");
		return FALSE;
	}


	g_Hashes.resize(g_ModuleList.size());

	for (auto i = 0; i < g_ModuleList.size(); ++i) {
		if (!m_Integrity.HashTextSection(g_ModuleList[i].hModule, g_Hashes[i].data())) {
			m_Utility.Log(LOG_ERROR, "Failed to hash module: %ls", g_ModuleList[i].szModule);
		}
		else {
			m_Utility.Log(LOG_SUCCESS, "Module %ls hashed successfully", g_ModuleList[i].szModule);
			PrintHash(g_Hashes[i]);
		}
	}

	return TRUE;
}