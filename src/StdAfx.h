//==============================================================================
//
//   StdAfx.h
//   Sunday, May 7, 2023 8:13:37 PM
//
//==============================================================================
//   This software is provided 'as-is', without any express or implied
//   warranty.  In no event will the authors be held liable for any damages
//   arising from the use of this software.
//
//   Permission is granted to anyone to use this software for any purpose,
//   including commercial applications, and to alter it and redistribute it
//   freely, subject to the following restrictions:
//
//   1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//   2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//
//   3. This notice may not be removed or altered from any source distribution.
//==============================================================================
//   Copyright (C) Guilaume Plante 2020 <guillaumeplante.qc@gmail.com>
//==============================================================================

#ifndef __STDAFX_H__f2ea2c38__
#define __STDAFX_H__f2ea2c38__

#define _WIN32_WINNT _WIN32_WINNT_MAXVER

#define ENCRYPTION_V2

#ifdef _RELEASE
#undef DEJA_ENABLED
#ifndef DEJA_DISABLED
#define DEJA_DISABLED
#endif //DEJA_DISABLED
#endif //_RELEASE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // __STDAFX_H__f2ea2c38__

