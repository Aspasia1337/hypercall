#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

BOOL CalculateHash(BYTE* Memory, uint64_t Size, BYTE Result[32]);
BOOL FindTextSection(HMODULE Module, BYTE* TextStart, uint32_t Size);
