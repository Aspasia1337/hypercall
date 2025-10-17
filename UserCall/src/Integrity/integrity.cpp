#include "integrity.h"

#pragma comment (lib,"bcrypt.lib")

BOOL CalculateHash(BYTE* Memory, int Size, BYTE Result[32]) {
	BCRYPT_ALG_HANDLE hAlg = NULL;
	BCRYPT_HASH_HANDLE hHash = NULL;
	NTSTATUS status;

	status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);

	if (!BCRYPT_SUCCESS(status)) {
		printf("Fail opening Bcryptopenalgorithmprovider\n");
		return 0;
	}

	status = BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);

	if (!BCRYPT_SUCCESS(status)) {
		printf("Fail creating hash\n");
		return 0;
	}

	status = BCryptHashData(hHash, (PUCHAR)Memory, Size, 0);

	if (!BCRYPT_SUCCESS(status)) {
		printf("Fail hashing data\n");
		return 0;
	}

	status = BCryptFinishHash(hHash, Result, 32, 0);

	BCryptDestroyHash(hHash);
	BCryptCloseAlgorithmProvider(hAlg, 0);

	if (!BCRYPT_SUCCESS(status)) {
		printf("Fail obtaining the hashed result\n");
		return 0;
	}

	return 1;
}


BOOL FindTextSection(HMODULE Module, BYTE** TextStart, uint32_t* Size)
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
			return TRUE;
		}
	}

	return FALSE;
}


BOOLEAN HashTextSection(BYTE HashResult[32]) {

	HMODULE mainHandle = 0;
	BYTE* textPointer = 0;
	uint32_t sectionSize = 0;

	mainHandle = GetModuleHandle(NULL);

	printf("Module handle : %p\n", mainHandle);

	if (!mainHandle) {
		printf("Error getting text section handle\n");
		return 0;
	}

	if (!FindTextSection(mainHandle, &textPointer, &sectionSize)) {
		printf("Error finding text section of the handle\n");
		return 0;
	}


	if (!CalculateHash(textPointer, sectionSize, HashResult)) {
		printf("Error calculating the hash of the text section\n");
		return 0;
	}

	PrintTextSection(textPointer, sectionSize);

	return 1;
}

VOID PrintTextSection(BYTE* TextSection, uint32_t SectionSize) {
	for (int i = 0; i < SectionSize; i++) {
		printf("%02X", TextSection[i]);
	}
}