// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__EDF5958A_92F6_11D3_9365_00C0F00AE104__INCLUDED_)
#define AFX_STDAFX_H__EDF5958A_92F6_11D3_9365_00C0F00AE104__INCLUDED_

#pragma warning (disable : 4786)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef _DEBUG
#	define qDebug 1
#else
#	define qDebug 0
#endif

#define UASSERT ASSERT
	// the pseudo ASSERT

#include "CJLibrary.h"
		// the CJ library provides a bunch of usefull Windows classes.

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__EDF5958A_92F6_11D3_9365_00C0F00AE104__INCLUDED_)
