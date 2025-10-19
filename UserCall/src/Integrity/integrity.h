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
	BOOL HashTextSection(BYTE HashResult[32]);
	VOID PrintTextSection(BYTE* TextSection, uint32_t SectionSize);

private :
	std::vector<BYTE [32]> g_ModuleList;
};

inline IntegrityClass m_Integrity;
