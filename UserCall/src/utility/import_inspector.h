#pragma once
#include <Windows.h>
#include <unordered_map>
#include <array>
#include <string>


struct FunctInfo {
    std::string dllName;
    BYTE* funcAddr;
    std::string funcName;
};

class ImportsInspector{
public:
    BOOL Inspector(HMODULE baseModule);
    const std::unordered_map<std::string, FunctInfo>& GetImports() const;

private:
    std::unordered_map<std::string, FunctInfo> imports;
};