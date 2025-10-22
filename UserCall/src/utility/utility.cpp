#include "utility.h"
#include <unordered_set>

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
			//Log(LOG_INFO, "[UTILITY] Found .text section at %p (size: %u bytes) ", *TextStart, *Size);
			return TRUE;
		}
	}

	UtilityClass::Log(LOG_ERROR, "[UTILITY] .text section not found in module");
	return FALSE;
}

// Modules = DLLs the program imports
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

	//UtilityClass::Log(LOG_SUCCESS, "[UTILITY] Total modules: %zu", g_ModuleList.size());
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

BOOL UtilityClass::TamperCheck() {
	
	std::unordered_set<HMODULE> hModuleSet;

	if (!GetProcessModules()) {
		m_Utility.Log(LOG_ERROR, "Fail getting modules");
		return 0;
	}
	
	if (g_ModuleList.size() > 0) {
		for (auto i = 0; i < g_ModuleList.size(); i++) {
			hModuleSet.insert(g_ModuleList[i].hModule);
			BYTE temporalHash[32] = {};
			if (m_Integrity.HashTextSection(g_ModuleList[i].hModule, temporalHash)) {
				auto it = g_ModuleMap.find(g_ModuleList[i].hModule);
				if (it == g_ModuleMap.end()) {
					std::array<BYTE, 32> hashArray;
					memcpy(hashArray.data(), temporalHash, 32);
					g_ModuleMap.emplace(g_ModuleList[i].hModule, hashArray);
					m_Utility.Log(LOG_INFO, "New module loaded %ls", g_ModuleList[i].szExePath);
					continue;
				}
				else {
					if (memcmp(temporalHash, it->second.data(), 32) != 0) {
						m_Utility.Log(LOG_INFO, "MODULE %ls TAMPERED", g_ModuleList[i].szExePath);
						memcpy(it->second.data(), temporalHash, 32);
					}
				}
			}
			else{
				m_Utility.Log(LOG_ERROR, "[HashModules]Error hashing module %ls ", g_ModuleList[i].szExePath);
				return FALSE;
			}
		}
	}
	for (auto it = g_ModuleMap.begin(); it != g_ModuleMap.end();) {
		if (hModuleSet.find(it->first) == hModuleSet.end()) {
			m_Utility.Log(LOG_INFO, "Module unloaded ");
			it = g_ModuleMap.erase(it);
		}
		else {
			it++;
		}
	}

	
	return TRUE;
}

// Imports = Functions that the program takes from those DLLs
BOOL UtilityClass::GetProgamImports() {
	HMODULE pHandle = GetHandle();

	Log(LOG_INFO, "Starting Imports");

	if (pHandle == INVALID_HANDLE_VALUE) {
		Log(LOG_ERROR, "[GetProgramImports] Error getting handle");
		CloseHandle(pHandle);
		return 0;
	}

	PIMAGE_DOS_HEADER imageDosHeader = (PIMAGE_DOS_HEADER)pHandle;

	if (imageDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		Log(LOG_ERROR, "[GetProgramImports] Not valid image header");
		CloseHandle(pHandle);
		return 0;
	}

	PIMAGE_NT_HEADERS imageNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)pHandle + imageDosHeader->e_lfanew);

	if (imageNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
		Log(LOG_ERROR, "[GetProgramImports] Not valid nt headers");
		CloseHandle(pHandle);
		return 0;
	}

	DWORD importDirRVA = imageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	PIMAGE_IMPORT_DESCRIPTOR importTable = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)pHandle + importDirRVA);

	if (!importTable) {
		Log(LOG_ERROR, "Import table address is null");
		CloseHandle(pHandle);
		return 0;
	}

	while (importTable->Name) {
		const char* dllName = (const char*)((BYTE*)pHandle + importTable->Name);
		Log(LOG_INFO, "[GetProgamImports] : Import name : %s", dllName);
		
		HMODULE hModule = LoadLibraryA(dllName);
		if (hModule == NULL) {
			Log(LOG_ERROR, "[GetProgamImports] : Error getting handle from %s", dllName);
			CloseHandle(pHandle);
			CloseHandle(hModule);
			importTable++;
			continue;
		}
		DWORD iltRVA = importTable->OriginalFirstThunk;
		DWORD iatRVA = importTable->FirstThunk;

		PIMAGE_THUNK_DATA thunkILT = (PIMAGE_THUNK_DATA)((BYTE*)pHandle + iltRVA);
		PIMAGE_THUNK_DATA thunkIAT = (PIMAGE_THUNK_DATA)((BYTE*)pHandle + iatRVA);

		while (thunkIAT->u1.AddressOfData) {
			if (!(thunkILT->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
				PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)pHandle + thunkILT->u1.AddressOfData);
				const char* funcName = (const char*)(importByName->Name);
				Log(LOG_SUCCESS, "Function name : %s", funcName);

			}
			thunkIAT++;
			thunkILT++;
		}
	
		importTable++;
	}
	return 1;
}