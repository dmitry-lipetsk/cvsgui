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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- June 2004
 */

#if !defined(AFX_SAVESETTINGSDLG_H__73E4731F_7A3E_46B2_835A_056922641479__INCLUDED_)
#define AFX_SAVESETTINGSDLG_H__73E4731F_7A3E_46B2_835A_056922641479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveSettingsDlg.h : header file
//

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

#include <string>
#include <map>

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CSaveSettingsDlg dialog

class CSaveSettingsDlg : public CHHelpDialog
{
// Construction
public:
	CSaveSettingsDlg(const char* path, const char* oldname, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveSettingsDlg)
	enum { IDD = IDD_SAVESETTINGS };
	CButton	m_copyCheck;
	CSmartComboBox	m_copyFromPathCombo;
	CString	m_settingName;
	CString	m_copyFrom;
	CString	m_path;
	BOOL	m_copy;
	CString	m_copyPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveSettingsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnCopySettings();
	afx_msg void OnSelchangeCopySettingsName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data 
	std::map<std::string, std::string> m_settingsList;

	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

#endif // !defined(AFX_SAVESETTINGSDLG_H__73E4731F_7A3E_46B2_835A_056922641479__INCLUDED_)
