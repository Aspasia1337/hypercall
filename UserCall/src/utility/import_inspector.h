#pragma once
#include <Windows.h>
#include <unordered_map>
#include <array>
#include <string>

struct FunctInfo {
    const char* modName;
    BYTE* functionAddress;
};

class ImportInspector {
public:
    BOOL InspectImports(HMODULE baseModule);
    const std::unordered_map<std::string, FunctInfo>& GetImports() const;

private:
    std::unordered_map<std::string, FunctInfo> imports;
};