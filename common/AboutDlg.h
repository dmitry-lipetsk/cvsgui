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
 * Author : Karl-Heinz Brünen <k.bruenen@yahoo.com> --- March 2002
 */

// AboutDlg.h

#ifndef ABOUT_DLG_H
#define ABOUT_DLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "HtmlStatic.h"
#endif

void CompatAboutDialog(void);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CHHelpDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHtmlStatic	m_CvsGuiSfUrl;
	CHtmlStatic	m_WinCvsUrl;
	CString	m_vers;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#ifdef qUnix
class UAboutDlg : public UWidget
{
	UDECLARE_DYNAMIC(UAboutDlg)
public:
	UAboutDlg() : UWidget(::UEventGetWidID()) {}

	virtual ~UAboutDlg() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kAppPix,	     		// 1
		kAppName,				// 2
		kAppURL					// 3
	};
	
	virtual void DoDataExchange(bool fill);
	
protected:	
	ev_msg int OnOk(void);
	ev_msg int OnURL(void);
	
	UDECLARE_MESSAGE_MAP()
};
#endif // qUnix

#endif // ABOUT_DLG_H
