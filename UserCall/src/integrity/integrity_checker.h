#pragma once
#include <Windows.h>

class IntegrityChecker {
public:
    BOOL CalculateHash(BYTE* Memory, int Size, BYTE Result[32]);
    BOOL HashTextSection(HMODULE Handle, BYTE HashResult[32]);
};