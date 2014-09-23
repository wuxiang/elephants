// HmacOtp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define cbSHA1_RESULT                   20
#define cbMD5_RESULT                    16
#define cbHMAC_PADDING                  64
#define cbMIN_OTP                       6

DWORD g_rgdwDigitsPower [] = 
     // 0 1  2   3    4     5      6       7        8
     {1,10,100,1000,10000,100000,1000000,10000000,100000000};

//
// Heap helpers
//

LPVOID Alloc(DWORD cb)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cb);
}

void Free(LPVOID pv)
{
    HeapFree(GetProcessHeap(), 0, pv);
}

//
// Flow macros
//

static 
void
WINAPI
_OutputDbgStr(
    __in        LPSTR szMsg,
    __in        DWORD dwStatus)
{
    CHAR rgsz [256];

    StringCbPrintfA(
        rgsz,
        sizeof(rgsz),
        "ERROR: %s - 0x%x\n",
        szMsg,
        dwStatus);
    OutputDebugStringA(rgsz);
}

#define TRY_DWORD(_X) {                                             \
    if (ERROR_SUCCESS != (status = _X)) {                           \
        _OutputDbgStr(#_X, status);                                 \
        __leave;                                                    \
    }                                                               \
}

#define TRY_BOOL(_X) {                                              \
    if (FALSE == (_X)) {                                            \
        status = GetLastError();                                    \
        _OutputDbgStr(#_X, status);                                 \
        __leave;                                                    \
    }                                                               \
}

#define TRY_ALLOC(_X) {                                             \
    if (NULL == (_X)) {                                             \
        status = ERROR_NOT_ENOUGH_MEMORY;                           \
        __leave;                                                    \
    }                                                               \
}

//
// Generate an RFC 2104 HMAC value
// 
DWORD
WINAPI
GenerateHMAC(
    __in                        ALG_ID aiHash,
    __in_ecount(cbSecret)       PBYTE pbSecret,
    __in                        DWORD cbSecret,
    __in_ecount(cbMsg)          PBYTE pbMsg,
    __in                        DWORD cbMsg,
    __out_ecount(cbHMAC)        PBYTE pbHMAC,
    __in                        DWORD cbHMAC)
{
    DWORD status = ERROR_SUCCESS;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbIPad [cbHMAC_PADDING];
    BYTE rgbOPad [cbHMAC_PADDING];
    DWORD dwIndex = 0;
    BYTE bTemp = 0;

    __try
    {
        //
        // Check input
        // 

        if (cbSecret > cbHMAC_PADDING)
        {
            status = ERROR_INVALID_PARAMETER;
            __leave;
        }

        //
        // Set the padding strings
        // 

        ZeroMemory(rgbIPad, sizeof(rgbIPad));
        ZeroMemory(rgbOPad, sizeof(rgbOPad));

        memcpy(rgbIPad, pbSecret, cbSecret);
        memcpy(rgbOPad, pbSecret, cbSecret);

        for ( ; dwIndex < cbHMAC_PADDING; dwIndex++)
        {
            rgbIPad [dwIndex] ^= 0x36;
            rgbOPad [dwIndex] ^= 0x5C;
        }

        // Reverse both padding strings in place
        for (dwIndex = 0; dwIndex < cbHMAC_PADDING; dwIndex++)
        {
            bTemp = rgbIPad [dwIndex];
            rgbIPad [dwIndex] = rgbIPad [cbHMAC_PADDING - dwIndex - 1];
            rgbIPad [cbHMAC_PADDING - dwIndex - 1] = bTemp;

            bTemp = rgbOPad [dwIndex];
            rgbOPad [dwIndex] = rgbOPad [cbHMAC_PADDING - dwIndex - 1];
            rgbOPad [cbHMAC_PADDING - dwIndex - 1] = bTemp;
        }

        //
        // Get the inner hash
        //

        TRY_BOOL(CryptAcquireContext(
            &hProv, 
            NULL, 
            MS_ENHANCED_PROV, 
            PROV_RSA_FULL, 
            CRYPT_VERIFYCONTEXT));

        TRY_BOOL(CryptCreateHash(hProv, aiHash, 0, 0, &hHash));
        TRY_BOOL(CryptHashData(hHash, rgbIPad, sizeof(rgbIPad), 0));
        TRY_BOOL(CryptHashData(hHash, pbMsg, cbMsg, 0));
        TRY_BOOL(CryptGetHashParam(hHash, HP_HASHVAL, pbHMAC, &cbHMAC, 0));

        CryptDestroyHash(hHash);
        hHash = 0;

        //
        // Get the outer hash
        // 

        TRY_BOOL(CryptCreateHash(hProv, aiHash, 0, 0, &hHash));
        TRY_BOOL(CryptHashData(hHash, rgbOPad, sizeof(rgbOPad), 0));
        TRY_BOOL(CryptHashData(hHash, pbHMAC, cbHMAC, 0));
        TRY_BOOL(CryptGetHashParam(hHash, HP_HASHVAL, pbHMAC, &cbHMAC, 0));
    }
    __finally
    {
        if (0 != hHash)
            CryptDestroyHash(hHash);
        if (0 != hProv)
            CryptReleaseContext(hProv, 0);
    }

    return status;
}

//
// Generate an OTP value per RFC 4226.
// 
extern "C" __declspec(dllexport) 
DWORD
WINAPI
GenerateOTP(
    __in_ecount(cbSecret)       PBYTE pbSecret,
    __in                        DWORD cbSecret,
    __in                        DWORD64 qwCount,
    __out_ecount(cchOTP)        LPSTR szOTP,
    __in                        DWORD cchOTP)
{
    DWORD status = ERROR_SUCCESS;
    BYTE rgbText [sizeof(qwCount)];
    DWORD iText = 0;
    BYTE rgbHash [cbSHA1_RESULT];
    DWORD dwOffset = 0;
    DWORD dwBinary = 0;
	DWORD dwOTP = 0;

    __try
    {
        //
        // Check input
        // 

        if (cchOTP > (sizeof(g_rgdwDigitsPower) + 1) ||
            cchOTP < cbMIN_OTP)
        {
            status = ERROR_INVALID_PARAMETER;
            __leave;
        }

        //
        // Get the text
        // 

        for ( ; iText < sizeof(rgbText); iText++)
        {
            rgbText [sizeof(rgbText) - iText - 1] = (BYTE) qwCount & 0xFF;
            qwCount >>= 8;
        }

        //
        // Get the HMAC result
        //

        TRY_DWORD(GenerateHMAC(
            CALG_SHA1, 
            pbSecret, 
            cbSecret, 
            rgbText, 
            sizeof(rgbText), 
            rgbHash, 
            sizeof(rgbHash)));

        //
        // Compute the OTP from the HMAC result
        // 

        dwOffset = rgbHash [cbSHA1_RESULT - 1] & 0xF;

        dwBinary = ((rgbHash [dwOffset] & 0x7F) << 24) |
            ((rgbHash [dwOffset + 1] & 0xFF) << 16) |
            ((rgbHash [dwOffset + 2] & 0xFF) << 8) |
            (rgbHash [dwOffset + 3] & 0xFF);

        dwOTP = dwBinary % g_rgdwDigitsPower [cchOTP - 1];
        _itoa_s(dwOTP, szOTP, cchOTP, 10);
    }
    __finally
    {
    }

    return status;
}