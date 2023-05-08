//==============================================================================
//
//   ResCryptDefines.h
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

#ifndef __RESCRYPT_DEFINES_H__0b17d2d5__
#define __RESCRYPT_DEFINES_H__0b17d2d5__

/*********************************************************************

   Copyright (C) 2002 Smaller Animals Software, Inc.

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.

   http://www.smalleranimals.com
   smallest@smalleranimals.com

**********************************************************************/
#ifndef CRXHeaderH
#define CRXHeaderH

#define _USING_CXR

#ifndef _USING_CXR
#define _CXR(x) x
#else
#define _CXR(x) __CXRDecrypt(x)
extern std::string __CXRDecrypt(const char* in, const char* pw);
#endif

#endif

#endif // __RESCRYPT_DEFINES_H__0b17d2d5__

