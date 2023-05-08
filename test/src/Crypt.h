

#ifndef CXR_CRYPT_H
#define CXR_CRYPT_H

#include <algorithm>
#include <string>
#include <atlstr.h>
typedef unsigned char BYTE;

CString Decrypt(const char* pIn, const char* pPass);
CString Encrypt(const CString& csIn, const char* pPass);

#endif