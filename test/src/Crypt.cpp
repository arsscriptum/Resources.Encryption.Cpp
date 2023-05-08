
#include <stdio.h>
#include "Crypt.h"
#include "Stream.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "Base64.h"
#include <vector>
#include "nowarns.h"
using namespace std;



/////////////////////////////////////////////////////////////////
CString Encrypt(const CString& csIn, const char* pPass)
{
    CString csOut;

    // initialize out 
    CCXRIntEnc sap((const BYTE*)pPass, strlen(pPass));

    /*
       start each string with a random char.
       because this is a stream cipher, the ciphertext of a
       string like "ABC" will be the same as the first 3 bytes
       of the ciphertext for "ABCDEFG".

       by starting with a random value, the cipher will be in a
       different state (255:1) when it starts the real text. the
       decoder will simply discard the first plaintext byte.
    */
    srand((unsigned int)time(nullptr));
    std::vector<BYTE> allbytes;
    BYTE seed = rand() % 256;
    BYTE c = sap.ProcessByte((BYTE)(seed));

    allbytes.push_back(c);

    // encrypt and convert to hex string

    for (int i = 0; i < csIn.GetLength(); i++)
    {
        char t = csIn.GetAt(i);
        c = sap.ProcessByte((BYTE)(t));

        allbytes.push_back(c);


    }

    std::string encodedData = base64_encode(allbytes.data(), allbytes.size());
    csOut = encodedData.c_str();
    return csOut;
}



CString Decrypt(const char* pIn, const char* pPass)
{

    std::vector<BYTE> decodedBytes = base64_decode(pIn);
    std::string inString(reinterpret_cast<const char*>(&decodedBytes[0]), decodedBytes.size());
    CString csOut;

    CCXRIntDec sap((const BYTE*)pPass, strlen(pPass));

    std::vector<BYTE> allbytes;
    // encrypt and convert to hex string

    for (unsigned int i = 0; i < inString.length(); i++)
    {
        char t = inString.at(i);
        BYTE c = sap.ProcessByte((BYTE)(t));

        if (i > 0) {
            allbytes.push_back(c);
        }

    }

    std::string outString(reinterpret_cast<const char*>(&allbytes[0]), allbytes.size());
    csOut = outString.c_str();
  
    return csOut;
}
