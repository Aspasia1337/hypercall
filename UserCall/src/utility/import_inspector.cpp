#include "import_inspector.h"
#include "../integrity/integrity_checker.h"
#include "logger.h"
#include <sstream>
#include <iomanip>

static std::string HashToHex(const BYTE* hash, size_t len = 32) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return oss.str();
}

BOOL ImportInspector::InspectImports(HMODULE baseModule) {
    if (!baseModule) return FALSE;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)baseModule;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;

    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE*)baseModule + dos->e_lfanew);
    DWORD importRVA = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)baseModule + importRVA);

    IntegrityChecker checker;

    while (importDesc->Name) {
        const char* dllName = (const char*)baseModule + importDesc->Name;
        HMODULE hDll = LoadLibraryA(dllName);
        if (!hDll) { ++importDesc; continue; }

        PIMAGE_THUNK_DATA thunkILT = (PIMAGE_THUNK_DATA)((BYTE*)baseModule + importDesc->OriginalFirstThunk);
        PIMAGE_THUNK_DATA thunkIAT = (PIMAGE_THUNK_DATA)((BYTE*)baseModule + importDesc->FirstThunk);

        while (thunkIAT->u1.AddressOfData) {
            if (!(thunkILT->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
                auto importByName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)baseModule + thunkILT->u1.AddressOfData);
                const char* funcName = (const char*)(importByName->Name);
                BYTE* funcAddr = (BYTE*)GetProcAddress(hDll, funcName);

                BYTE hash[32] = {};
                if (!checker.CalculateHash(funcAddr, 20, hash))
                    continue;

                FunctInfo fi = { dllName, funcAddr };
                std::string hexHash = HashToHex(hash);
                imports[hexHash] = fi;

                Logger::Log(LogLevel::LOG_INFO, "Import %s from %s at %p [hash: %s]",
                    funcName, dllName, funcAddr, hexHash.c_str());
            }
            ++thunkILT;
            ++thunkIAT;
        }
        ++importDesc;
    }

    return TRUE;
}

const std::unordered_map<std::string, FunctInfo>& ImportInspector::GetImports() const {
    return imports;
}