// CJWindowPlacement.h : header file
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
 * $Date: 2005/08/09 00:22:45 $
 * $Revision: 1.2 $
 * $Archive: /CodeJock/Include/CJWindowPlacement.h $
 *
 * $History: CJWindowPlacement.h $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:49p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:09p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 10:15p
 * Updated in $/CodeJockey/Include
 * Modified LoadWindowPos(...) to allow registry names to be directly
 * passed in.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 9:41p
 * Updated in $/CodeJockey/Include
 * Initial creation.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/01/99    Time: 9:30p
 * Created in $/CodeJockey/Include
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJWINDOWPLACEMENT_H__
#define __CJWINDOWPLACEMENT_H__

// CCJWindowPlacement is a WINDOWPLACEMENT derived class which is used to
// save and restore your applications window position.
class _CJX_EXT_CLASS CCJWindowPlacement : public WINDOWPLACEMENT
{
public:
	
	// Default constructor
	//
	CCJWindowPlacement();

	// Virtual destructor
	//
	virtual ~CCJWindowPlacement();

private:
	
	CString		m_strFormat;	// format string for registry settings.
	CString		m_strSection;	// registry section name.
	CString		m_strWndPos;	// registry entry name.

public:

	// call this member function to load your applications previously saved
	// window position. To use, add a member variable to your CWinApp derived
	// class ie: CCJWindowPlacement	m_wndPlacement; then replace the call
	// to pMainFrame->ShowWindow(...) with m_wndPlacement.LoadWindowPos( pMainFrame );
	//
	virtual BOOL LoadWindowPos( CFrameWnd* pFrameWnd, LPCTSTR lpszFormat=NULL, LPCTSTR lpszSection=NULL, LPCTSTR lpszWndPos=NULL );

	// call this member function to save your applications window position. To
	// use, add the ON_WM_CLOSE message handler to your CFrameWnd derived class,
	// then make a call to theApp.m_wndPlacement.SaveWindowPos( this ); just before
	// you call the base class OnClose().
	//
	virtual void SaveWindowPos( CFrameWnd* pFrameWnd );
};

/////////////////////////////////////////////////////////////////////////////

#endif // __CJWINDOWPLACEMENT_H__

