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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 1998
 */

/*
 * CommitDlg.h : the cvs commit dialog
 */

#if !defined(AFX_COMMITDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_COMMITDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"

#ifdef WIN32
#	include "MultilineEntryDlg.h"
#	include "HHelpSupport.h"
#endif

class MultiFiles;

bool CompatGetCommit(const MultiFiles* mf,
					 std::string& commit, bool& norecurs, 
					 bool& forceCommit, bool& forceRecurse, std::string& rev, bool& noModuleProgram,
					 bool& checkValidEdits);

#ifndef WIN32
	void combozify(std::string& str);
#endif

extern CMString gRevNames;

#ifdef WIN32
class CCommit_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CCommit_MAIN)

// Construction
public:
	CCommit_MAIN(const MultiFiles* mf, const char* logMsg);
	~CCommit_MAIN();

// Dialog Data
	//{{AFX_DATA(CCommit_MAIN)
	enum { IDD = IDD_COMMIT_MAIN };
	CStatic	m_multilineEntryPlaceholder;
	CButton	m_previewInfo;
	CSmartComboBox	m_fileToDiffCombo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCommit_MAIN)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCommit_MAIN)
	virtual BOOL OnInitDialog();
	afx_msg void OnDiff();
	afx_msg void OnSelchangeFiletodiff();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;	/*!< Browser selection */
	CString m_logMsg;		/*!< Selected message */
	
	CMultilineEntryDlg m_multilineEntryDlg; /*!< Multiline entry child dialog */

	// Methods
	virtual void InitHelpID();
	
	void UpdatePreviewInfo();

public:
	// Interface
	CString GetLogMsg() const;
};

/////////////////////////////////////////////////////////////////////////////
// CCommit_OPTIONS dialog

class CCommit_OPTIONS : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CCommit_OPTIONS)

// Construction
public:
	CCommit_OPTIONS(const MultiFiles* mf);
	~CCommit_OPTIONS();

// Dialog Data
	//{{AFX_DATA(CCommit_OPTIONS)
	enum { IDD = IDD_COMMIT_OPTIONS };
	CSmartComboBox	m_revCombo;
	BOOL	m_forceCommit;
	CString	m_rev;
	BOOL	m_forceRecurse;
	BOOL	m_forceRevision;
	BOOL	m_noModuleProgram;
	BOOL	m_noRecurse;
	BOOL	m_checkValidEdits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCommit_OPTIONS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCommit_OPTIONS)
	afx_msg void OnForcerevision();
	afx_msg void OnBrowseTag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;

	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
#endif /* WIN32 */

#endif // !defined(AFX_COMMITDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
