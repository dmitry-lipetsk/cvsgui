// CJLibrary.cpp : Defines the initialization routines for the DLL.
//
// This file is part of the CJLibrary for Visual C++ / MFC
// Copyright C 1998-1999 COdejock Software, All Rights Reserved.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 1, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 2005/08/09 00:22:44 $
 * $Revision: 1.2 $
 * $Archive: /CodeJock/CJLibrary/CJLibrary.cpp $
 *
 * $History: CJLibrary.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/25/99   Time: 11:02p
 * Updated in $/CodeJock/CJLibrary
 * Moved check for common control dll to DLL_PROCESS_ATTACH portion of
 * DllMain.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/CJLibrary
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxdllx.h>
#include "CJToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE CJLibraryDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("CJLIBRARY.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(CJLibraryDLL, hInstance))
			return 0;

		// see what version of the common control we are using.
		_GetComCtlVersion();
		if (_ComCtlVersion < VERSION_IE4) {
			::MessageBox(NULL, _T("Invalid Common Control DLL!\nYou need version 4.70 or higher to use CJLibrary...\nAborting!\n\nCopyright © 1999 CodeJock.com and Copyright © 1998-1999 Kirk W. Stowell, All Rights Reserved\nmailto:kstowell@codejock.com\nhttp://www.codejock.com"), _T("Invalid COMCTL32.DLL"), MB_ICONSTOP);
			return 0;
		}

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(CJLibraryDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("CJLIBRARY.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(CJLibraryDLL);
	}
	return 1;   // ok
}
