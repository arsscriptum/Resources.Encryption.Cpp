//==============================================================================
//
//   ResCrypt.cpp
//   Sunday, May 7, 2023 8:13:37 PM
//
//==============================================================================
//   Copyright (C) Guilaume Plante 2020 <guillaumeplante.qc@gmail.com>
//==============================================================================


#include "stdafx.h"
#include "ResCrypt.h"
#include "CmdLine.h"
#include "Tokenizer.h"
#include "Stream.h"
#include <stdio.h>
#include <stdlib.h>
#include "Base64.h"
#include "log.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEBUG_PRINTF(x,...)


#pragma message( "Compiling " __FILE__ )
#pragma message( "Last modified on " __TIMESTAMP__ )

void banner() {
    std::wcout << std::endl;
    cprint_b("ResCrypt v2.1 - String Encryption Tool\n");
    cprint_b("Built on %s\n", __TIMESTAMP__);
    cprint_b("Copyright (C) 2000-2021 Guillaume Plante\n");
    std::wcout << std::endl;
}
void usage() {
    cprint_b("Usage: rescrypt.exe [-h][-s]<string>[-p]<password>[-i]<input>[-o]<output>\n");
    cprint_b("   -h          Print help\n");
    cprint_b("   -i          input file\n");
    cprint_b("   -o          output file\n");
    cprint_b("   -s          string to encrypt\n");
    cprint_b("   -p          password\n");
    std::wcout << std::endl;
    cprint_m("examples:\n");
    cprint_c("   rescrypt.exe -i stringfile.rsc -o strings.cpp");
    cprint_c("   rescrypt.exe -s \"Please CLose Dialog\" -p secret");
    std::wcout << std::endl;
}



/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

/////////////////////////////////////////////////////////////////

static bool     ProcessFile(CStdioFile &in, CStdioFile &out);
static bool     AddDecode(const CString & csPassword, CStdioFile &out);
static CString  Encrypt(const CString &csIn, const char *pPass);
static CString  Decrypt(const char *pIn, const char *pPass);
static bool     ExpandOctal(const CString &csIn, CString &csOut, int &iConsumed);
static CString  TranslateCString(const CString &csIn);
static bool     ExpandHex(const CString &csIn, CString &csOut, int &iConsumed);
static CString    EscapeCString(const char *pIn);

// these can be adjusted in the range 1 to 239
const int basechar1 = 128;
const int basechar2 = 128;

/////////////////////////////////////////////////////////////////

#include <string>
#include <algorithm>
typedef unsigned char BYTE;

const char* __pCXRPassword = "ThereAreTooManySecrets_";
const int __iCXRDecBase1 = 128;
const int __iCXRDecBase2 = 128;

class CCXR
{
protected:
    CCXR(const BYTE* key, unsigned int ks)
    {
        int i; BYTE rs; unsigned kp;
        for (i = 0; i < 256; i++)c[i] = i; kp = 0; rs = 0; for (i = 255; i; i--)std::swap(c[i], c[kr(i, key, ks, &rs, &kp)]); r2 = c[1]; r1 = c[3]; av = c[5]; lp = c[7]; lc = c[rs]; rs = 0; kp = 0;
    }
    inline void SC() { BYTE st = c[lc]; r1 += c[r2++]; c[lc] = c[r1]; c[r1] = c[lp]; c[lp] = c[r2]; c[r2] = st; av += c[st]; }
    BYTE c[256], r2, r1, av, lp, lc;

    BYTE kr(unsigned int lm, const BYTE* uk, BYTE ks, BYTE* rs, unsigned* kp)
    {
        unsigned rl = 0, mk = 1, u; while (mk < lm)mk = (mk << 1) + 1; do { *rs = c[*rs] + uk[(*kp)++]; if (*kp >= ks) { *kp = 0; *rs += ks; }u = mk & *rs; if (++rl > 11)u %= lm; } while (u > lm); return u;
    }
};
struct CXRD :CCXR
{
    CXRD(const BYTE* userKey, unsigned int keyLength = 16) : CCXR(userKey, keyLength) {}
    inline BYTE pb(BYTE b) { SC(); lp = b ^ c[(c[r1] + c[r2]) & 0xFF] ^ c[c[(c[lp] + c[lc] + c[av]) & 0xFF]]; lc = b; return lp; }
};
CString __CXRDecrypt(const char* pIn)
{
    CString x; char b[3]; b[2] = 0;
    CXRD sap((const BYTE*)__pCXRPassword, strlen(__pCXRPassword));
    int iLen = strlen(pIn);
    if (iLen > 2)
    {
        int ibl = strlen(pIn);
        if (ibl & 0x01)
        {
            //ASSERT(!"Illegal string length in Decrypt");
            return pIn;
        }
        ibl /= 2;
        for (int i = 0; i < ibl; i++)
        {
            int b1 = pIn[i * 2] - __iCXRDecBase1; int b2 = pIn[i * 2 + 1] - __iCXRDecBase2;
            int c = (b1 << 4) | b2; char ch = (char)(sap.pb((BYTE)(c)));
            if (i > 0) x += ch;
        }
    }
    return x;
}


int _tmain(int argc, char* argv[], char* envp[])
{
	int nRetCode = 0;

   srand((unsigned int)time(NULL));

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
      


      CCmdLine cmd;

      if (cmd.SplitLine(argc, argv) >= 2)
      {
          bool quiet = cmd.HasSwitch("-q") || cmd.HasSwitch("--quiet");
          bool help = cmd.HasSwitch("-h") || cmd.HasSwitch("--help");
          if (!quiet) {
              banner();
          }
          if (help) {
              usage();
              return 0;
          }
         CString csInFile = cmd.GetSafeArgument("-i", 0, "");
         CString csOutFile = cmd.GetSafeArgument("-o", 0, "");
         CString csClearString = cmd.GetSafeArgument("-s", 0, "");
         CString csPassword = cmd.GetSafeArgument("-p", 0, "");
         if (!csClearString.IsEmpty() && !csPassword.IsEmpty()){
             //cout << _T("Encrypt String \"") << (LPCTSTR)csClearString << _T("\" with password \"") << (LPCTSTR)csPassword << _T("\"") << endl;
             CString encStr = Encrypt(csClearString, (LPCTSTR)csPassword);
             cout << _T("SUCCESS") << endl;
             cout << (LPCTSTR)encStr << endl;
             return 0;
         }else if (!csInFile.IsEmpty() && !csOutFile.IsEmpty())
         {
		      // open the input file
            CStdioFile fileIn;

            // open the output file
            CStdioFile fileOut;
            

            if (fileIn.Open(csInFile, CFile::modeRead | CFile::typeText))
            {
               if (fileOut.Open(csOutFile, CFile::modeCreate | CFile::modeWrite | CFile::typeText ))
               {
                  if (!ProcessFile(fileIn, fileOut))
                  {
                     cerr << "CXR failed\n";
                     nRetCode = 1;
                  }
               }
               else
               {
                  cerr << _T("Unable to open output file: ") << (LPCTSTR)csOutFile << endl;
                  nRetCode = 1;
               }
            }
            else
            {
               cerr << _T("Unable to open input file: ") << (LPCTSTR)csInFile << endl;
               nRetCode = 1;
            }

            if (nRetCode==0)
            {
               cout << "CXR created: " << (LPCTSTR)csOutFile << "\n";
            }
         }
         else
         {
            cerr << _T("Not enough parameters") << endl;
            nRetCode = 1;
         }
      }      
      else
      {
         cerr << _T("Not enough parameters") << endl;
         nRetCode = 1;
      }
	}

 	return nRetCode;
}

/////////////////////////////////////////////////////////////////

bool ProcessFile(CStdioFile &in, CStdioFile &out)
{
   enum 
   {
      eStateWantPassword,
      eStateHavePassword,
   };

   int iState = eStateWantPassword;

   CString csPassword;
   CString line;

   char *pMetachars = _T("/\\=();'");
   char *pKeyWords[3] = {_T("//"), _T("_CXR"), _T("CXRP")};
   
   CTokenizer tokenizer(pKeyWords, 3, pMetachars, strlen(pMetachars));
   int iErr = CTokenizer::eErrorNone;
   bool ok = true;

   out.WriteString(_T("//==============================================================================\n//\n"));
   out.WriteString(_T("//   "));
   out.WriteString(out.GetFileName());
   out.WriteString(_T("\n//\n"));
   out.WriteString(_T("//   This file was generated the string encryption tool CXR.\n"));
   out.WriteString(_T("//   https://github.com/arsscriptum/String.Encrypt.cpp\n//\n//\n"));
   out.WriteString(_T("//                       ***DO NOT EDIT THIS FILE***\n//   Any changes here will be overwritten on the next compilation.\n//   You can edit: "));
   out.WriteString(in.GetFilePath());
   out.WriteString(_T("\n//\n"));
   out.WriteString(_T("//==============================================================================\n\n"));
   out.WriteString(_T("#include \"stdafx.h\"\n"));
   out.WriteString(_T("#include \"cxr_inc.h\"\n\n"));

   bool bFoundCXR = false;

   do 
   {
      if (!in.ReadString(line))
      {
         break;
      }

      switch (iState)
      {
      case eStateWantPassword:
         iErr = tokenizer.Tokenize(line);
         if (iErr == CTokenizer::eErrorNone)
         {
            if (tokenizer.GetTokenCount() >= 4)
            {
               // password declaration always looks like : // CXRP = "Password"
               if ((tokenizer.GetToken(0).csToken == _T("//")) && 
                  (tokenizer.GetToken(1).csToken == _T("CXRP")) && 
                  (tokenizer.GetToken(2).csToken == _T("=")) && 
                  (tokenizer.GetToken(3).bIsQuotedString))
               {
                  // we'll use the password from the file, literally. it's not treated as
                  // a C string-literal, just as a section of a text file. when we
                  // go to write the decoder, we'll have to fix it up to make sure
                  // the compiler gets the same text by adding any necessary escapes.
                  csPassword = tokenizer.GetToken(3).csToken;

                  if (csPassword.IsEmpty())
                  {
                     cerr << _T("Invalid CXR password: \"") << (LPCTSTR)csPassword << _T("\"") << endl;
                     ASSERT(0);
                     break;
                  }
                  cout << _T("CXR password: \"") << (LPCTSTR)csPassword << _T("\"") << endl;
                  iState = eStateHavePassword;
                  continue;
               }
            }
         }
         break;
      case eStateHavePassword:
         bFoundCXR = false;
         iErr = tokenizer.Tokenize(line);
         if (iErr == CTokenizer::eErrorNone)
         {
            if (tokenizer.GetTokenCount() > 4)
            {
               for (int i=0;i<tokenizer.GetTokenCount() - 4; i++)
               {
                  // looking for _CXR ( "..." )
                  if (
                     (tokenizer.GetToken(i).csToken == _T("_CXR")) && !tokenizer.GetToken(i).bIsQuotedString &&
                     (tokenizer.GetToken(i + 1).csToken == _T("(")) && !tokenizer.GetToken(i + 1).bIsQuotedString &&
                     (tokenizer.GetToken(i + 2).bIsQuotedString) &&
                     (tokenizer.GetToken(i + 3).csToken == _T(")")) && !tokenizer.GetToken(i + 3).bIsQuotedString
                     )
                  {
                      //CString csTrans = TranslateCString(tokenizer.GetToken(i + 2).csToken);;
                      CString clearText = tokenizer.GetToken(i + 2).csToken;
                      CString csEnc = Encrypt(clearText, csPassword);
                      LOG_TRACE("CXR::ProcessFile", "Encrypt RESULT %s", csEnc.GetBuffer());
                      CString csDec = Decrypt(csEnc, csPassword);
   
                      LOG_TRACE("CXR::ProcessFile", "Decrypt RESULT %s", csDec.GetBuffer());
                     out.WriteString(_T("//==============================================================================\n//   STRINGS DECLARATIONS\n"));
                     out.WriteString(_T("//==============================================================================\n\n"));
                     out.WriteString(_T("#ifdef _USING_CXR\n"));

                     /*
                     out.WriteString("//");
                     out.WriteString(csDec);
                     out.WriteString("\n");
                     */

                     // output up to _CXR
                     out.WriteString(line.Left(tokenizer.GetToken(i).iStart));

                     // encrypted stuff
                     out.WriteString(_T("\""));
                     out.WriteString(csEnc);
                     out.WriteString(_T("\""));

                     // to the end of the line
                     out.WriteString(line.Mid(tokenizer.GetToken(i + 4).iStop));

                     out.WriteString(_T("\n"));
 
                     out.WriteString(_T("#else // _USING_CXR\n"));
                     out.WriteString(line);
                     out.WriteString(_T("\n#endif // _USING_CXR\n\n"));

                     bFoundCXR = true;

                     break;

                  } // found a good string ?
                  
               } // loop over tokens

            }  // > 4 tokens

         } // tokenizer OK

         if (bFoundCXR)
         {
            continue;
         }
      
         break; // switch
      }

      // done with it
      out.WriteString(line);
      out.WriteString("\n");

   } while (1);

   if (iState == eStateWantPassword)
   {
      cerr << "No password line found in input file\n";
      return false;
   }

   ASSERT(iState==eStateHavePassword);

   // add the decoder functions
   AddDecode(csPassword, out);

   return true;
}

/////////////////////////////////////////////////////////////////

void AddEncByte(BYTE c, CString &csOut)
{
  
   char buf[4];

   BYTE b1 = c >> 4;
   BYTE b2 = c & 0x0f;

   _snprintf(buf, 3, "%x", b1 + basechar1);
   csOut+="\\x";
   csOut+=buf;

   _snprintf(buf, 3, "%x", b2 + basechar1);
   csOut+="\\x";
   csOut+=buf;
}

/////////////////////////////////////////////////////////////////

#ifdef ENCRYPTION_V2

/////////////////////////////////////////////////////////////////
CString Encrypt(const CString& csIn, const char* pPass)
{
    CString csOut;
  
    LOG_TRACE("CXR::ENCRYPT","Encrypt %s %s", csIn.GetString(), pPass);
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
        BYTE c = sap.ProcessByte((BYTE)(t));
        
        allbytes.push_back(c);
       
       
    }
   
    std::string encodedData = base64_encode(allbytes.data(), allbytes.size());
    csOut = encodedData.c_str();
    LOG_TRACE("CXR::ENCRYPT", "Encrypt RESULT %s", encodedData.c_str());
    return csOut;
}



#else

CString Encrypt(const CString &csIn, const char *pPass)
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
   BYTE seed = rand() % 256;
   BYTE c = sap.ProcessByte((BYTE)(seed));
   AddEncByte(c, csOut);

   std::vector<BYTE> allbytes;
   // encrypt and convert to hex string
   for (int i=0; i < csIn.GetLength(); i++)
   {
      char t = csIn.GetAt(i);
      BYTE c = sap.ProcessByte((BYTE)(t));
      AddEncByte(c, csOut);
      allbytes.push_back(c);
   }

   return csOut;
}
#endif

/////////////////////////////////////////////////////////////////

#ifdef ENCRYPTION_V2

CString Decrypt(const char* pIn, const char* pPass)
{
    LOG_TRACE("CXR::DECRYPT", "Decrypt %s %s", pIn, pPass);
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
    LOG_TRACE("CXR::DECRYPT", "Decrypt RESULT %s", outString.c_str());
    return csOut;
}

#else

CString Decrypt(const char *pIn, const char *pPass)
{
   CString csOut;

   CCXRIntDec sap((const BYTE *)pPass, strlen(pPass));

   int iLen = _tcslen(pIn);

   if (iLen > 2)
   {
      int iBufLen = strlen(pIn);
      if (iBufLen & 0x01)
      {
         cerr << "Illegal string length in Decrypt\n";
         return pIn;
      }

      iBufLen/=2;

      for (int i=0;i<iBufLen;i++)
      {
         int b1 = pIn[i * 2] - basechar1;
         int b2 = pIn[i * 2 + 1] - basechar2;
         int c = (b1 << 4) | b2;

         BYTE bc = sap.ProcessByte((BYTE)(c));

         if (i>0) csOut+=(char)bc;
      }
   }

   return csOut;
}
#endif


/////////////////////////////////////////////////////////////////


// Additional Control&Reporting

/////////////////////////////////////////////////////////////////

#ifdef ENCRYPTION_V2
bool AddDecode(const CString & csPassword, CStdioFile &out)
{
   out.WriteString(_T("\n\n//==============================================================================\n"));
   out.WriteString(_T("// generated decoder code below\n"));
   out.WriteString(_T("//==============================================================================\n\n"));
   out.WriteString(_T("#include <algorithm>\n"));
   out.WriteString(_T("#include <string>\n"));
   out.WriteString(_T("#include \"Base64.h\"\n"));

   // the password that encrypted the text used the literal text from the file (non-escaped \ chars).
   // we need to make sure that compiler sees the same text when it gets the passowrd. so,
   // we must double any "\" chars, to prevent them from becoming C-style escapes.
   //out.WriteString(EscapeCString(csPassword));
   //out.WriteString(_T("\";\n"));
                   
   // the high-level decoding function
    const char *pDec1 = 
"\n\n //==============================================================================\n" \
" // the high-level decoding function\n" \
" //==============================================================================\n" \
" std::string __CXRDecrypt(const char* in, const char* pw) \n" \
" { \n" \
"    std::vector<BYTE> db = base64_decode(in); \n" \
"    std::string is(reinterpret_cast<const char*>(&db[0]), db.size()); \n" \
"    std::vector<BYTE> bos; \n" \
"    CXRD sap((const BYTE*)pw, strlen(pw)); \n" \
"    for (unsigned int i = 0; i < is.length(); i++){ \n" \
"        char c = (char)(sap.pb((BYTE)(is.at(i)))); \n" \
"        if (i > 0) { bos.push_back(c); } \n" \
"    } \n" \
"    std::string x(reinterpret_cast<const char*>(&bos[0]), bos.size()); \n" \
"    return x; \n" \
"}\n";

    const char* pStr1 =
        // the stream cipher
        "\n\n //==============================================================================\n" \
        " // the stream cipher\n" \
        " //==============================================================================\n" \
        " class CCXR \n" \
        " { \n" \
        " protected: \n" \
        "    CCXR(const BYTE *key, unsigned int ks) \n" \
        "    { \n" \
        "       int i;BYTE rs;unsigned kp; \n" \
        "       for(i=0;i<256;i++)c[i]=i;kp=0;rs=0;for(i=255;i;i--)std::swap(c[i],c[kr(i,key,ks,&rs,&kp)]);r2=c[1];r1=c[3];av=c[5];lp=c[7];lc=c[rs];rs=0;kp=0; \n" \
        "    } \n" \
        "       inline void SC(){BYTE st=c[lc];r1+=c[r2++];c[lc]=c[r1];c[r1]=c[lp];c[lp]=c[r2];c[r2]=st;av+=c[st];} \n" \
        "       BYTE c[256],r2,r1,av,lp,lc;     \n" \
        "  \n" \
        "    BYTE kr(unsigned int lm, const BYTE *uk, BYTE ks, BYTE *rs, unsigned *kp) \n" \
        "    { \n" \
        "       unsigned rl=0,mk=1,u;while(mk<lm)mk=(mk<<1)+1;do{*rs=c[*rs]+uk[(*kp)++];if(*kp>=ks){*kp=0;*rs+=ks;}u=mk&*rs;if(++rl>11)u%=lm;}while(u>lm);return u; \n" \
        "    } \n" \
        " }; \n" \
        "  \n" \
        " struct CXRD:CCXR \n" \
        " { \n" \
        "       CXRD(const BYTE *userKey, unsigned int keyLength=16) : CCXR(userKey, keyLength) {} \n" \
        "       inline BYTE pb(BYTE b){SC();lp=b^c[(c[r1]+c[r2])&0xFF]^c[c[(c[lp]+c[lc]+c[av])&0xFF]];lc=b;return lp;} \n" \
        " }; \n";

   out.WriteString(pStr1);
   out.WriteString(pDec1);

   return true;
}
#else

bool AddDecode(const CString & csPassword, CStdioFile &out)
{
   out.WriteString(_T("\n\n/////////////////////////////////////////////////////////////\n"));
   out.WriteString(_T("// CXR-generated decoder follows\n\n"));
   out.WriteString(_T("#include <algorithm>\n"));
   out.WriteString(_T("const char * __pCXRPassword = \""));  

   // the password that encrypted the text used the literal text from the file (non-escaped \ chars).
   // we need to make sure that compiler sees the same text when it gets the passowrd. so,
   // we must double any "\" chars, to prevent them from becoming C-style escapes.
   out.WriteString(EscapeCString(csPassword));

   out.WriteString(_T("\";\n"));
   CString t; 
   t.Format("const int __iCXRDecBase1 = %d;\nconst int __iCXRDecBase2 = %d;\n\n", basechar1, basechar2);
   out.WriteString(t);
                            
   // the high-level decoding function
const char *pDec1 = 
"std::string __CXRDecrypt(const char *pIn)\n"\
"{\n"\
"   CString x;char b[3];b[2]=0;\n"\
"   CXRD sap((const BYTE*)__pCXRPassword, strlen(__pCXRPassword));\n"\
"   int iLen = strlen(pIn);\n"\
"   if (iLen > 2)\n"\
"   {\n"\
"      int ibl=strlen(pIn);\n"\
"      if (ibl&0x01)\n"\
"      {\n"\
"         ASSERT(!\"Illegal string length in Decrypt\");\n"\
"         return pIn;\n"\
"      }\n"\
"      ibl/=2;\n"\
"      for (int i=0;i<ibl;i++)\n"\
"      {\n"\
"         int b1 =pIn[i*2]-__iCXRDecBase1;int b2=pIn[i*2+1]-__iCXRDecBase2;\n"\
"         int c = (b1 << 4) | b2; char ch =(char)(sap.pb((BYTE)(c)));\n"\
"         if (i>0) x+=ch;\n"\
"      }\n"\
"   }\n"\
"   return x;\n"\
"}\n";
 
   // the stream cipher
   const char *pStr1 =
"class CCXR\n" \
"{\n" \
"protected:\n" \
"   CCXR(const BYTE *key, unsigned int ks)\n" \
"   {\n" \
"      int i;BYTE rs;unsigned kp;\n" \
"      for(i=0;i<256;i++)c[i]=i;kp=0;rs=0;for(i=255;i;i--)std::swap(c[i],c[kr(i,key,ks,&rs,&kp)]);r2=c[1];r1=c[3];av=c[5];lp=c[7];lc=c[rs];rs=0;kp=0;\n" \
"   }\n" \
"	inline void SC(){BYTE st=c[lc];r1+=c[r2++];c[lc]=c[r1];c[r1]=c[lp];c[lp]=c[r2];c[r2]=st;av+=c[st];}\n" \
"	BYTE c[256],r2,r1,av,lp,lc;    \n" \
"\n" \
"   BYTE kr(unsigned int lm, const BYTE *uk, BYTE ks, BYTE *rs, unsigned *kp)\n" \
"   {\n" \
"      unsigned rl=0,mk=1,u;while(mk<lm)mk=(mk<<1)+1;do{*rs=c[*rs]+uk[(*kp)++];if(*kp>=ks){*kp=0;*rs+=ks;}u=mk&*rs;if(++rl>11)u%=lm;}while(u>lm);return u;\n" \
"   }\n" \
"};\n" \
"struct CXRD:CCXR\n" \
"{\n" \
"	CXRD(const BYTE *userKey, unsigned int keyLength=16) : CCXR(userKey, keyLength) {}\n" \
"	inline BYTE pb(BYTE b){SC();lp=b^c[(c[r1]+c[r2])&0xFF]^c[c[(c[lp]+c[lc]+c[av])&0xFF]];lc=b;return lp;}\n" \
"};\n";

   out.WriteString(pStr1);
   out.WriteString(pDec1);

   return true;
}

#endif 


/////////////////////////////////////////////////////////////////

CString TranslateCString(const CString &csIn)
{
   // translate C-style string escapes as documented in K&R 2nd, A2.5.2

   CString csOut;
   char buffer[sizeof(int) * 8 + 1];
   for (int i=0;i<csIn.GetLength(); i++)
   {
      int c = csIn.GetAt(i);
      switch (c)
      {
      default:
         // normal text
         csOut+=_itoa(c, buffer,10);
         break;
         // c-style escape
      case _T('\\'):
         if (i < csIn.GetLength() - 1)
         {
            c = csIn.GetAt(i + 1);
            switch (c)
            {
            case _T('n'):
               csOut+=_T('\n');
               break;
            case _T('t'):
               csOut+=_T('\t');
               break;
            case _T('v'):
               csOut+=_T('\v');
               break;
            case _T('b'):
               csOut+=_T('\b');
               break;
            case _T('r'):
               csOut+=_T('\r');
               break;
            case _T('f'):
               csOut+=_T('\f');
               break;
            case _T('a'):
               csOut+=_T('\a');
               break;
            case _T('\\'):
               csOut+=_T('\\');
               break;
            case _T('?'):
               csOut+=_T('?');
               break;
            case _T('\''):
               csOut+=_T('\'');
               break;
            case _T('\"'):
               csOut+=_T('\"');
               break;
            case _T('0'):
            case _T('1'):
            case _T('2'):
            case _T('3'):
            case _T('4'):
            case _T('5'):
            case _T('6'):
            case _T('7'):
               {
                  // expand octal
                  int iConsumed = 0;
                  if (!ExpandOctal(csIn.Mid(i), csOut, iConsumed))
                  {
                     cerr << _T("Invalid octal sequence: ") << _T('\"') << (LPCTSTR)csIn << _T('\"') << endl;
                     csOut = csIn;
                     break;
                  }

                  i+=iConsumed - 1;
               }
               break;
            case _T('x'):
               { 
                  // expand hex
                  int iConsumed = 0;
                  if (!ExpandHex(csIn.Mid(i), csOut, iConsumed))
                  {
                     cerr << _T("Invalid hex sequence: ") << _T('\"') << (LPCTSTR)csIn << _T('\"') << endl;
                     csOut = csIn;
                     break;
                  }

                  i+=iConsumed - 1;

               }
               break;
            }

            i++;
            continue;
         }
         else
         {
            cerr << _T("Invalid escape sequence: ") << _T('\"') << (LPCTSTR)csIn << _T('\"') << endl;
            csOut = csIn;
            break;
         }
         break;
      }
   }

   return csOut;
}

/////////////////////////////////////////////////////////////////

bool ExpandOctal(const CString &csIn, CString &csOut, int &iConsumed)
{
    char buffer[sizeof(int) * 8 + 1];
   // staring with the escape, we need at least one more char
   if (csIn.GetLength() < 2)
   {
      return false;
   }

   if (csIn.GetAt(0) != _T('\\'))
   {
      return false;
   }

   int iStart = 1;
   int iCur = iStart;

   CString digits;
   int c = csIn.GetAt(iCur);
   while ((c >= _T('0')) && (c <= _T('7')))
   {
      digits+= _itoa(c, buffer, 10);

      // an escape can't hold more that 3 octal digits (K&R 2nd A2.5.2)
      if (iCur == 3)
      {
         break;
      }
         
      iCur++;
      c = csIn.GetAt(iCur);
   }

   char *end;
   int octval = (char)_tcstol(digits, &end, 8);

   iConsumed = digits.GetLength();

   csOut+=_itoa(octval,buffer,10);

   return true;
}

/////////////////////////////////////////////////////////////////

bool ExpandHex(const CString &csIn, CString &csOut, int &iConsumed)
{
    char buffer[sizeof(int) * 8 + 1];
   // staring with the escape and the 'x', we need at least one more char
   if (csIn.GetLength() < 3)
   {
      return false;
   }

   if ((csIn.GetAt(0) != _T('\\')) || (csIn.GetAt(1) != _T('x')))
   {
      return false;
   }

   int iStart = 2;
   int iCur = iStart;

   CString digits;
   int c = csIn.GetAt(iCur);
   while (_istxdigit(c))
   {
      digits+= _itoa(c, buffer, 10);

      iCur++;
      c = csIn.GetAt(iCur);
   }

   char *end;

   // "There is no limit on the number of digits, but the behavior is undefined
   // if the resulting character value exceeds that of the largest character"
   // (K&R 2nd A2.5.2)
   int hex = (char)_tcstol(digits, &end, 16);

   iConsumed = digits.GetLength();

   iConsumed++; // count the "x"

   csOut += _itoa(hex, buffer, 10);

   return true;
}

/////////////////////////////////////////////////////////////////

CString EscapeCString(const char *pIn)
{
 
   CString csOut;

   int iLen = _tcslen(pIn);

   for (int i=0;i<iLen;i++)
   {
      csOut+=pIn[i];
      // double all "\" chars
      if (pIn[i] == _T('\\'))
      {
         csOut+=_T('\\');
      }
   }

   return csOut;
}

/////////////////////////////////////////////////////////////////



