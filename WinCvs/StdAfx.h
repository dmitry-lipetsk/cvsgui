// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D2D77DCA_8299_11D1_8949_444553540000__INCLUDED_)
#define AFX_STDAFX_H__D2D77DCA_8299_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable : 4786)

#if _MSC_VER >= 1400
#	pragma warning(disable : 4996)
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions
#include <afxmt.h>			// MFC Multithreaded Extensions

#include <HtmlHelp.h>		// HTML Help

#include <shlwapi.h>		// Interface for the Windows light-weight utility APIs

#include <afxrich.h>		// Rich edit
#include <afxcview.h>		// Common control classes

//////////////////////////////////////////////////////////////////////////
// CvsGui stuff

#include "cvsgui_i18n.h"

#include "CJLibrary.h"		// CJLibrary

#ifdef _DEBUG
#	define qCvsDebug 1
#else
#	define qCvsDebug 0
#endif

/// For the pseudo-interface
#define UASSERT ASSERT

/// Comment out for Win95 to disable multi-monitor support
#define MULTIMONITOR_SUPPORT

#include "TipDlg.h"			// Tip of the Day
#include "wincvs_winutil.h"	// Windows untilities functions

//////////////////////////////////////////////////////////////////////////
// STL

#include <vector>
#include <map>
#include <string>
#include <strstream>

#ifdef _DEBUG
#	include <malloc.h>
#	include <stdlib.h>
#	include <crtdbg.h>

#	define   malloc(s)         _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#	define   calloc(c, s)      _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#	define   realloc(p, s)     _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#	define   free(p)           _free_dbg(p, _NORMAL_BLOCK)
#endif

#include <sys/stat.h>
// Replacement stat() routine
struct wnt_stat {
        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        _off_t st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };
int wnt_stat(const char *name, struct wnt_stat *buf);
int wnt_fstat (int fildes, struct wnt_stat *buf);
int wnt_lstat (const char *name, struct wnt_stat *buf);

#define fstat wnt_fstat
#define lstat wnt_lstat
#define stat wnt_stat

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D2D77DCA_8299_11D1_8949_444553540000__INCLUDED_)
