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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * LogDlg.h : the cvs log dialog
 */

#if !defined(AFX_LOGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_LOGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;

bool CompatGetLog(const MultiFiles* mf,
				  bool& noRecurs, bool& defBranch, std::string& date,
				  bool& headerOnly, bool& headerDesc, bool& supress, bool& cvsRcsOutput, 
				  bool& noTags, bool& onlyRCSfile, bool& hasRev,
				  std::string& rev, std::string& states, bool& hasUser, std::string& users,
				  bool outGraph = false);

#ifdef WIN32

/// Main log dialog
class CLogMain : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CLogMain)

// Construction
public:
	CLogMain(bool supress, bool cvsRcsOutput, bool noRecurs, bool defBranch, bool headerOnly, bool headerDesc, bool noTags, bool onlyRCSfile);
	~CLogMain();

// Dialog Data
	//{{AFX_DATA(CLogMain)
	enum { IDD = IDD_LOG_MAIN };
	CButton	m_headerDescCheck;
	CButton	m_headerOnlyCheck;
	BOOL	m_supress;
	BOOL	m_defBranch;
	BOOL	m_noRecurs;
	BOOL	m_noTags;
	BOOL	m_onlyRCS;
	BOOL	m_headerOnly;
	BOOL	m_headerDesc;
	BOOL	m_cvsRcsOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogMain)
	afx_msg void OnHeaders();
	afx_msg void OnHeadersdesc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

/// Filters log dialog
class CLogFilters : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CLogFilters)

// Construction
public:
	CLogFilters(const MultiFiles* mf,
		const char* sLastDate, const char* sLastRev,
		const char* sLastUsers, const char* sLastStates);
	~CLogFilters();

// Dialog Data
	//{{AFX_DATA(CLogFilters)
	enum { IDD = IDD_LOG_FILTERS };
	CButton	m_hasUserCheck;
	CButton	m_hasStateCheck;
	CButton	m_hasRevCheck;
	CButton	m_hasDateCheck;
	CButton	m_browseTagButton;
	CSmartComboBox	m_userCombo;
	CSmartComboBox	m_stateCombo;
	CSmartComboBox	m_revCombo;
	CSmartComboBox	m_dateCombo;
	CString	m_date;
	CString	m_rev;
	CString	m_state;
	CString	m_user;
	BOOL	m_hasDate;
	BOOL	m_hasRev;
	BOOL	m_hasState;
	BOOL	m_hasUser;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogFilters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogFilters)
	afx_msg void OnDate();
	afx_msg void OnRev();
	afx_msg void OnState();
	afx_msg void OnUser();
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
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_LOGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
