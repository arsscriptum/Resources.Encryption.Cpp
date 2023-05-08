#include "resource.h"
#include <Windows.h>


#ifndef __DUMP_RES__H_
#define __DUMP_RES__H_



HANDLE g_hFile;   // global handle to resource info file
// Declare callback functions.
BOOL EnumTypesFunc(
	HMODULE hModule,
	LPTSTR lpType,
	LONG lParam);

BOOL EnumNamesFunc(
	HMODULE hModule,
	LPCTSTR lpType,
	LPTSTR lpName,
	LONG lParam);

BOOL EnumLangsFunc(
	HMODULE hModule,
	LPCTSTR lpType,
	LPCTSTR lpName,
	WORD wLang,
	LONG lParam);

#endif