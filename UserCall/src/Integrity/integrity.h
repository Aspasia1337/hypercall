#pragma once
#include <Windows.h>
#include <bcrypt.h>
#include <stdio.h>

#include "../utility/utility.h"

class IntegrityClass {

public:
	IntegrityClass() = default;
	~IntegrityClass() = default;

	BOOL CalculateHash(BYTE* Memory, int Size, BYTE Result[32]);
	BOOL HashTextSection(HMODULE Handle, BYTE HashResult[32]);
	VOID PrintTextSection(BYTE* TextSection, uint32_t SectionSize);
	
};

inline IntegrityClass m_Integrity;
