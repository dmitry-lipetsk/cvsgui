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
* Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- February 2004
*/

/*
* FileActionDlg.h : the file action dialog
*/

#if !defined(AFX_FILEACTIONDLG_H__9C280EEF_16E5_4208_8A31_24AE6290E9D7__INCLUDED_)
#define AFX_FILEACTIONDLG_H__9C280EEF_16E5_4208_8A31_24AE6290E9D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartComboBox.h"
#include "LaunchHandlers.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;

bool CompatGetFileAction(const MultiFiles* mf, kLaunchType& launchType);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CFileAction_MAIN dialog

class CFileAction_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CFileAction_MAIN)

// Construction
public:
	CFileAction_MAIN(const MultiFiles* mf, kLaunchType launchType);
	~CFileAction_MAIN();

// Dialog Data
	//{{AFX_DATA(CFileAction_MAIN)
	enum { IDD = IDD_OPEN_MAIN };
	CStatic	m_fileActionDescStatic;
	CSmartComboBox	m_fileActionCombo;
	int		m_fileAction;
	BOOL	m_sameActionNextTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFileAction_MAIN)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFileAction_MAIN)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAction();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;
	kLaunchType m_launchType;

	// Methods
	virtual void InitHelpID();

public:
	// Interface
	kLaunchType GetLaunchType() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif /* WIN32 */


#endif // !defined(AFX_FILEACTIONDLG_H__9C280EEF_16E5_4208_8A31_24AE6290E9D7__INCLUDED_)
