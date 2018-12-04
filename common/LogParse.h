/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * LogParse.h : parse the cvs log output
 */

#if !defined(AFX_LOGPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_LOGPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#	pragma once
#endif // _MSC_VER >= 1000

class CvsArgs;
class CLogNode;
class CColorConsole;

void CvsLogParse(const char* dir, const CvsArgs& args, const bool outGraph = false, void* defWnd = 0L);
void CvsLogOutput(CColorConsole& out, CLogNode* node);

#ifdef qMacCvsPP
  #include "Persistent.h"
  enum {
    kMsgEncodingUTF8 = 1,
    kMsgEncodingMacOSRoman,
    kMsgEncodingISO8859
  };
  extern CPersistentInt    sCommitMsgEncoding;
#endif

#endif // !defined(AFX_LOGPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
