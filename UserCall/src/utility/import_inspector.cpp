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

BOOL ImportsInspector::Inspector(HMODULE baseModule) {
    if (!baseModule) return FALSE;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)baseModule;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;

    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((BYTE*)baseModule + dos->e_lfanew);
    DWORD importRVA = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (!importRVA) return FALSE;

    PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)baseModule + importRVA);
    IntegrityChecker checker;
    std::unordered_map<std::string, FunctInfo> currentImports;

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

                std::string hexHash = HashToHex(hash);
                FunctInfo info = { dllName, funcAddr, funcName };
                currentImports[hexHash] = info;
            }
            ++thunkILT;
            ++thunkIAT;
        }
        ++importDesc;
    }

    for (const auto& [hash, info] : currentImports) {
        auto it = imports.find(hash);
        if (it == imports.end()) {
            Logger::Log(LogLevel::LOG_INFO, "[+] New import detected: %s (%s) at %p",
                info.funcName.c_str(), info.dllName.c_str(), info.funcAddr);
            imports[hash] = info;
        }
        else {
            if (it->second.funcAddr != info.funcAddr) {
                Logger::Log(LogLevel::LOG_INFO, "[!] Import modified: %s (%s) %p -> %p",
                    info.funcName.c_str(), info.dllName.c_str(),
                    it->second.funcAddr, info.funcAddr);
                it->second = info;
            }
        }
    }

    for (auto it = imports.begin(); it != imports.end();) {
        if (currentImports.find(it->first) == currentImports.end()) {
            Logger::Log(LogLevel::LOG_INFO, "[-] Import removed: %s (%s) at %p",
                it->second.funcName.c_str(), it->second.dllName.c_str(), it->second.funcAddr);
            it = imports.erase(it);
        }
        else {
            ++it;
        }
    }

    Logger::Log(LogLevel::LOG_SUCCESS, "[Inspector] Import map updated. Current total: %zu", imports.size());
    return TRUE;
}

const std::unordered_map<std::string, FunctInfo>& ImportsInspector::GetImports() const {
    return imports;
}