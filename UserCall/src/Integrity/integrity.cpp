#include "integrity.h"
#include <iostream>

#pragma comment (lib,"bcrypt.lib")

static VOID PrintTextSection(BYTE* TextSection, uint32_t SectionSize) {
	for (int i = 0; i < SectionSize; i++) {
		printf("%02X", TextSection[i]);
	}
}

BOOL IntegrityClass::CalculateHash(BYTE* Memory, int Size, BYTE Result[32]) {
	BCRYPT_ALG_HANDLE hAlg = NULL;
	BCRYPT_HASH_HANDLE hHash = NULL;
	NTSTATUS status;

	status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);

	if (!BCRYPT_SUCCESS(status)) {
		m_Utility.Log(LOG_ERROR,"[INTEGRITY] Fail opening Bcryptopenalgorithmprovider\n");
		return 0;
	}

	status = BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);

	if (!BCRYPT_SUCCESS(status)) {
		m_Utility.Log(LOG_ERROR, "[INTEGRITY] Fail creating hash\n");
		return 0;
	}

	status = BCryptHashData(hHash, (PUCHAR)Memory, Size, 0);

	if (!BCRYPT_SUCCESS(status)) {
		m_Utility.Log(LOG_ERROR, "[INTEGRITY] Fail hashing data\n");
		return 0;
	}

	status = BCryptFinishHash(hHash, Result, 32, 0);

	BCryptDestroyHash(hHash);
	BCryptCloseAlgorithmProvider(hAlg, 0);

	if (!BCRYPT_SUCCESS(status)) {
		m_Utility.Log(LOG_ERROR, "[INTEGRITY] Fail obtaining the hashed result\n");
		return 0;
	}

	return 1;
}


BOOL IntegrityClass::HashTextSection(HMODULE Handle, BYTE HashResult[32]) {

	BYTE* textPointer = 0;
	uint32_t sectionSize = 0;

	if (Handle == NULL) {
		Handle = GetModuleHandle(NULL);
	}

	//m_Utility.Log(LOG_INFO,"Module handle : %p\n", mainHandle);

	if (!Handle) {
		m_Utility.Log(LOG_ERROR, "[INTEGRITY] Error getting text section handle\n");
		return 0;
	}

	if (!m_Utility.FindTextSection(Handle, &textPointer, &sectionSize)) {
		m_Utility.Log(LOG_ERROR, "[INTEGRITY] Error finding text section of the handle\n");
		return 0;
	}


	if (!CalculateHash(textPointer, sectionSize, HashResult)) {
		m_Utility.Log(LOG_ERROR, "[INTEGRITY] Error calculating the hash of the text section\n");
		return 0;
	}

	//PrintTextSection(textPointer, sectionSize);


	return 1;
}
