//==============================================================================
//
//   Stream.h
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

#ifndef __STREAM_H__ae4d98e1__
#define __STREAM_H__ae4d98e1__

    
class CCXRIntBase
{
protected:
   CCXRIntBase(const BYTE *key, unsigned int ks)
   {
      int i;BYTE rs;unsigned kp;
      for(i=0;i<256;i++)c[i]=i;kp=0;rs=0;for(i=255;i;i--)std::swap(c[i],c[kr(i,key,ks,&rs,&kp)]);r2=c[1];r1=c[3];av=c[5];lp=c[7];lc=c[rs];rs=0;kp=0;
   }
	inline void SC(){BYTE st=c[lc];r1+=c[r2++];c[lc]=c[r1];c[r1]=c[lp];c[lp]=c[r2];c[r2]=st;av+=c[st];}
	BYTE c[256],r2,r1,av,lp,lc;    

   BYTE kr(unsigned int lm, const BYTE *uk, BYTE ks, BYTE *rs, unsigned *kp)
   {
      unsigned rl=0,mk=1,u;
      while(mk<lm)mk=(mk<<1)+1;
      do{*rs=c[*rs]+uk[(*kp)++];
      if(*kp>=ks){*kp=0;*rs+=ks;}u=mk&*rs;
      if(++rl>11)u%=lm;}while(u>lm);
      return u;
   }
};
struct CCXRIntDec:CCXRIntBase
{
	CCXRIntDec(const BYTE *userKey, unsigned int keyLength=16) : CCXRIntBase(userKey, keyLength) {}
	inline BYTE ProcessByte(BYTE b){SC();lp=b^c[(c[r1]+c[r2])&0xFF]^c[c[(c[lp]+c[lc]+c[av])&0xFF]];lc=b;return lp;}
};

struct CCXRIntEnc:CCXRIntBase
{
	CCXRIntEnc(const BYTE *userKey, unsigned int keyLength=16) : CCXRIntBase(userKey, keyLength) {}
	inline BYTE ProcessByte(BYTE b){SC();lc=b^c[(c[r1]+c[r2])&0xFF]^c[c[(c[lp]+c[lc]+c[av])&0xFF]];lp=b;return lc;}
};

#endif // __STREAM_H__ae4d98e1__

