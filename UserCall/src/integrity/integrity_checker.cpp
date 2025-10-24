#include "integrity_checker.h"
#include "../utility/logger.h"
#include "../utility/process_manager.h"
#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

BOOL IntegrityChecker::CalculateHash(BYTE* Memory, int Size, BYTE Result[32]) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;

    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0)))
        return FALSE;
    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0)))
        return FALSE;
    if (!BCRYPT_SUCCESS(BCryptHashData(hHash, Memory, Size, 0)))
        return FALSE;
    if (!BCRYPT_SUCCESS(BCryptFinishHash(hHash, Result, 32, 0)))
        return FALSE;

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return TRUE;
}

BOOL IntegrityChecker::HashTextSection(HMODULE Handle, BYTE HashResult[32]) {
    BYTE* textPtr = nullptr;
    uint32_t size = 0;

    ProcessManager pm;
    if (!pm.FindTextSection(Handle, &textPtr, &size))
        return FALSE;

    return CalculateHash(textPtr, size, HashResult);
}