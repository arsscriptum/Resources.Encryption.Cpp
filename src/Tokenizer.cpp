//==============================================================================
//
//   Tokenizer.cpp
//   Sunday, May 7, 2023 8:13:37 PM
//
//==============================================================================
//   Copyright (C) Guilaume Plante 2020 <guillaumeplante.qc@gmail.com>
//==============================================================================


#include "stdafx.h"
#include "Tokenizer.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CXR does not have comments
#undef ALLOW_COMMENTS

// strip quotes on strings, please
#undef RETURN_QUOTED_STRINGS

#ifdef ALLOW_COMMENTS
#define COMMENT_START   "/*"
#define COMMENT_STOP    "*/"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTokenizer::CTokenizer(TCHAR **pKeyWords, 
                       int iKeyWords,
                       TCHAR *pMetaChars,
                       int iMetaChars)
{
   m_pKeyWords = pKeyWords;
   m_iKeyWords = iKeyWords;
   m_MetaChars = pMetaChars;
   m_iMetaChars = iMetaChars;
}

//////////////////////////////////////////////////////////////////////
// bye bye
CTokenizer::~CTokenizer()
{

}

//////////////////////////////////////////////////////////////////////
// remove tokens
void CTokenizer::Clear()
{
   m_tokens.clear();
}

void CTokenizer::Init()
{
   Clear();
}

//////////////////////////////////////////////////////////////////////
// extract an array of tokens from this input text
int CTokenizer::Tokenize(const TCHAR *pInputLine)
{
   int err = eErrorNone;

   Init();

   int iLen = _tcslen(pInputLine);

   bool bInComment = false;

   for (int i=0; i < iLen; i++)
   {
      CString curTokenString;
      bool bQuotedString = false;

      int iConsumed = GetToken(pInputLine + i, curTokenString, bQuotedString);

      if (iConsumed > 0)
      {
         //if (curTokenString.length() > 0)
         {
#ifdef ALLOW_COMMENTS
            if (curTokenString == COMMENT_START)
            {
               bInComment = true;
               i+=iConsumed;
               continue;
            }
            
            if (curTokenString == COMMENT_STOP)
            {
               if (!bInComment)
               {
                  err = eErrorSyntax;
                  break;
               }

               bInComment = false;

               i+=iConsumed;
               continue;
            }
#endif

            if (!bInComment)
            {
               int iStart = i;
               int iStop = i + iConsumed - 1;

               CSAToken curToken(curTokenString, iStart, iStop, bQuotedString);

               m_tokens.push_back(curToken);
            }
         }
         
         i+=iConsumed;

         i--; // back up! the for iteration will increase i
         continue;
      }
      else
      {
         if (IsMetaChar(*(pInputLine + i)))
         {
            if (!isspace(*(pInputLine + i)))
            {
               curTokenString.Empty();
               curTokenString+=*(pInputLine + i);

               CSAToken curToken(curTokenString, i, curTokenString.GetLength() - 1, bQuotedString);

               m_tokens.push_back(curToken);
            }
         }
      }
   }

   return err;
}

//////////////////////////////////////////////////////////////////////
// is this text a keyword?
bool CTokenizer::IsKeyWord(CString &str)
{
   return IsKeyWord((const TCHAR *)str);
}

//////////////////////////////////////////////////////////////////////

bool CTokenizer::IsKeyWord(const TCHAR *pInput)
{
   if (m_pKeyWords==NULL)
   {
      return false;
   }

   int i_tcslen = _tcslen(pInput);

   int iBestMatch = -1;

   for (int i=0; i < m_iKeyWords; i++)
   {
      int iCurKWLen = _tcslen(m_pKeyWords[i]);

      if (i_tcslen <= iCurKWLen)
      {
         if (_strnicmp(m_pKeyWords[i], pInput, iCurKWLen)==0)
         {
            iBestMatch = i;
         }
      }
   }

   if (iBestMatch==-1)
   {
#ifdef ALLOW_COMMENTS
      if (CSAScrUtil::sa_strnicmp(COMMENT_START, pInput, _tcslen(COMMENT_START))==0)
      {
         iBestMatch = m_iKeyWords + 1;
      }

      if (CSAScrUtil::sa_strnicmp(COMMENT_STOP, pInput, _tcslen(COMMENT_STOP))==0)
      {
         iBestMatch = m_iKeyWords + 2;
      }
#endif
   }
   return (iBestMatch != -1);
  
}

//////////////////////////////////////////////////////////////////////
// find the next token in the string

int CTokenizer::GetToken(const TCHAR *pInput, CString &out, bool &bQuotedString)
{
   int i_tcslen = _tcslen(pInput);

   bool bFoundChars = false;

   bool bScanningMetaKW = false;

   bool bInQuotes = false;

   bQuotedString = false;

   TCHAR c;
   int iWS = 0;
   for (iWS = 0; iWS < i_tcslen; iWS++)
   {
      c = *(pInput + iWS);
      if (!isspace(c))
         break;
      if (!iscntrl(c))
         break;
   }
   int i = 0;
   for (i=iWS; i<i_tcslen; i++)
   {
      c = *(pInput + i);

      // not in quotes?
      // open quote
      if ((c==_T('"')) && (!bInQuotes))
      {
         // not in quotes, but we found a ", and we've already found chars?
         if (bFoundChars)
         {
            break;
         }

         bInQuotes = true;
#ifdef RETURN_QUOTED_STRINGS
         out+=c;
#endif
         continue;
      }

      // close quote
      if ((c==_T('"')) && (bInQuotes))
      {
         bInQuotes = false;
#ifdef RETURN_QUOTED_STRINGS
         out+=c;
#endif
         i+=1;

         bQuotedString = true;
         break;
      }

      if (bInQuotes)
      {
         // escape!
         if (c==_T('\\'))
         {
            //out+=c;
            if (i < i_tcslen - 1)
            {
               switch (*(pInput + i + 1))
               {
                  // if we see \" in a quoted string, don't treat it as a string end!
               case _T('"'):
                  out+= _T("\\\""); // output the slash and the quote
                  i++; // skip the quote in the string
                  continue;
                  break;
               
               default:
                  // don't touch the other escapes.
                  out+=c;
                  continue;
                  break;
               }
            }
         }

         out+=c;
         continue;
      }

      // break non-quoted strings on ctrl or white space
      if (iscntrl(c) || isspace(c))
      {
         break;
      }

      // metachars...
      if (IsMetaChar(c))
      {
         if (iscntrl(c) || isspace(c))
         {
            break;
         }

         if (!bFoundChars)
         {
            bScanningMetaKW = true;

            bFoundChars = true;

            // in case some KWs start with metachars
				if (i < i_tcslen - 2)
				{
					TCHAR buf[3];
					buf[0]=c;
					buf[1]=*(pInput + i + 1);
					buf[2]=0;

					if (IsKeyWord(buf))
					{
						out+=c;
						out+=*(pInput + i + 1);

						i+=2;

						break;
					}
					else
					{
						buf[1] = 0;
						if (IsKeyWord(buf))
						{
							out+=c;

							i+=1;

							break;
						}
					}
				}

				out+=c;
         }
         else
         {
            break;
         }
      }
      else
      {
         if (!bFoundChars)
         {
            bScanningMetaKW = false;

            bFoundChars = true;

            out+=c;
         }
         else
         {
            if (bScanningMetaKW)
            {
               break;
            }
            else
            {
               out+=c;
            }
         }
      }
   }

   return i;
}


bool CTokenizer::IsMetaChar(const TCHAR c)
{
   for (int i=0; i < m_iMetaChars; i++)
   {
      if (c==m_MetaChars[i])
      {

         return true;
      }
   }

   return false;
}



bool CTokenizer::IsMetaChar(CString &str)
{
   if (str.GetLength() > 1)
   {
      return false;
   }

   TCHAR c = str.GetAt(0);

   for (int i=0; i < m_iMetaChars; i++)
   {
      if (c==m_MetaChars[i])
      {
         return true;
      }
   }

   return false;
}



void CTokenizer::Dump()
{
   CSATokenVector::iterator theIterator;

   int i=0;
   for (theIterator = m_tokens.begin(); theIterator < m_tokens.end(); theIterator++)
   {
      TRACE("%d [%d-%d] : \"%s\"\n", i++, (*theIterator).iStart, (*theIterator).iStop, (const TCHAR *)((*theIterator).csToken));
   }
}



bool operator < (const CSAToken &a, const CSAToken &b)
{
   return (a.iStart < b.iStart);
}

bool operator == (const CSAToken &a, const CSAToken &b)
{
   return (a.iStart==b.iStart);
}



