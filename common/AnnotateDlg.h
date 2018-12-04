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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- June 2001
 */

/*
 * AnnotateDlg.h : the cvs annotate dialog
 */

#if !defined(AFX_ANNOTATEDLG_H__864729EE_838B_473D_8723_E1269829F781__INCLUDED_)
#define AFX_ANNOTATEDLG_H__864729EE_838B_473D_8723_E1269829F781__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartComboBox.h"
#include "CvsCommands.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;

bool CompatGetAnnotate(const MultiFiles* mf, kAnnotateCmdType annotateType, 
					   bool& noRecurs, bool& isDate, std::string& rev, bool& force,
					   bool& bulkAnnotate, kLaunchType& launchType);

extern CMString gRevNames;
extern CMString gDateNames;

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CAnnotate_MAIN dialog

class CAnnotate_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CAnnotate_MAIN)

// Construction
public:
	CAnnotate_MAIN(const MultiFiles* mf, kAnnotateCmdType annotateType, 
		const char* rev, bool isDate, bool bulkAnnotate, kLaunchType launchType);
	~CAnnotate_MAIN();

// Dialog Data
	//{{AFX_DATA(CAnnotate_MAIN)
	enum { IDD = IDD_ANNOTATE_MAIN };
	CStatic	m_fileActionDescStatic;
	CSmartComboBox	m_fileActionCombo;
	CButton	m_forceCheck;
	CButton	m_noRecurseCheck;
	CButton	m_hasRevCheck;
	CButton	m_browseTagButton;
	CButton	m_isDateCheck;
	CSmartComboBox	m_revCombo;
	BOOL	m_hasRev;
	BOOL	m_isDate;
	BOOL	m_force;
	BOOL	m_noRecurse;
	CString	m_rev;
	BOOL	m_bulkAnnotate;
	int		m_fileAction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAnnotate_MAIN)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAnnotate_MAIN)
	afx_msg void OnSpecifyRev();
	afx_msg void OnIsdate();
	afx_msg void OnBrowseTag();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAction();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;
	kAnnotateCmdType m_annotateType;
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

#endif // !defined(AFX_ANNOTATEDLG_H__864729EE_838B_473D_8723_E1269829F781__INCLUDED_)
