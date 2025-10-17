#pragma once
#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

BOOL CalculateHash(BYTE* Memory, int Size, BYTE Result[32]);
BOOL FindTextSection(HMODULE Module, BYTE** TextStart, uint32_t* Size);
BOOLEAN HashTextSection(BYTE HashResult[32]);
VOID PrintTextSection(BYTE* TextSection, uint32_t SectionSize);