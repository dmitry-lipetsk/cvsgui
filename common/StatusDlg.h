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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- March 2002
 */

/*
 * StatusDlg.cpp : the cvs status dialog
 */

#if !defined(AFX_STATUSDLG_H__5EEE9989_6C76_4FD3_83FC_2D90DEA0CE21__INCLUDED_)
#define AFX_STATUSDLG_H__5EEE9989_6C76_4FD3_83FC_2D90DEA0CE21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CvsPrefs.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;

bool CompatGetStatus(const MultiFiles* mf, bool& noRecurs, STATUSOUTPUT_TYPE& outputType);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CStatus_MAIN dialog

class CStatus_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CStatus_MAIN)

// Construction
public:
	CStatus_MAIN(bool noRecurs, STATUSOUTPUT_TYPE outputType);
	~CStatus_MAIN();

// Dialog Data
	//{{AFX_DATA(CStatus_MAIN)
	enum { IDD = IDD_STATUS_MAIN };
	BOOL	m_noRecurs;
	int		m_outputType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStatus_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CStatus_MAIN)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSDLG_H__5EEE9989_6C76_4FD3_83FC_2D90DEA0CE21__INCLUDED_)

#endif /* WIN32 */
