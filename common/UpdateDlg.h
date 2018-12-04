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
 * UpdateDlg.h : the cvs update dialog
 */

#if !defined(AFX_UPDATEDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_UPDATEDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "MultiString.h"
#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "SubstModeEntryDlg.h"
#endif

class MultiFiles;
class CCvsrootEntryDlg;

bool CompatGetUpdate(const MultiFiles* mf,
					 const bool queryOnly,
					 bool& toStdout, bool& noRecurs, bool& resetSticky,
					 std::string& date, std::string& rev,
					 bool& useMostRecent,
					 std::string& rev1, std::string& rev2,
					 bool& branchPointMerge, 
					 bool& threeWayConflicts, 
					 bool& createMissDir,
					 bool& getCleanCopy,
					 std::string& keyword,
					 bool& caseSensitiveNames,
					 bool& lastCheckinTime);

#ifdef WIN32

/// Main update page
class CUpdateMain : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CUpdateMain)

// Construction
public:
	CUpdateMain(bool createMissDir);
	~CUpdateMain();

// Dialog Data
	//{{AFX_DATA(CUpdateMain)
	enum { IDD = IDD_UPDATE_MAIN };
	BOOL	m_norecurs;
	BOOL	m_createmissdir;
	BOOL	m_getcleancopy;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUpdateMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUpdateMain)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

/// Update options page
class CUpdateOptions : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CUpdateOptions)
		
// Construction
public:
	CUpdateOptions(const CCvsrootEntryDlg* cvsrootEntryDlg, 
		const MultiFiles* mf, 
		const bool isExport, 
		const char* rev, const char* date, const char* keyword, 
		const bool resetSticky, const bool caseSensitiveNames, const bool lastCheckinTime);
	~CUpdateOptions();
	
// Dialog Data
	//{{AFX_DATA(CUpdateOptions)
	enum { IDD = IDD_UPDATE_OPTIONS };
	CButton	m_stdoutCheck;
	CStatic	m_substModeEntryPlaceholder;
	CButton	m_matchCheck;
	CButton	m_resetStickyCheck;
	CButton	m_browseTagButton;
	CButton	m_hasRevCheck;
	CButton	m_hasDateCheck;
	CSmartComboBox	m_revCombo;
	CSmartComboBox	m_dateCombo;
	CString	m_date;
	CString	m_rev;
	BOOL	m_hasDate;
	BOOL	m_match;
	BOOL	m_hasRev;
	BOOL	m_resetSticky;
	BOOL	m_stdout;
	BOOL	m_caseSensitiveNames;
	BOOL	m_lastCheckinTime;
	//}}AFX_DATA
	
	
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUpdateOptions)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUpdateOptions)
	afx_msg void OnDate();
	afx_msg void OnRev();
	afx_msg void OnBrowseTag();
	virtual BOOL OnInitDialog();
	afx_msg void OnDelsticky();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const CCvsrootEntryDlg* m_cvsrootEntryDlg;	/*!< CVSROOT entry */

	CSubstModeEntryDlg m_substModeEntryDlg; /*!< CVSROOT entry child dialog */
	const MultiFiles* m_mf;					/*!< Browser selection */

	bool m_export;	/*!< true for export options */
	
	// Methods
	virtual void InitHelpID();

public:
	// Interface
	CString m_keyword;	/*!< Keyword substitution */
};

/// Merge update page
class CUpdateMerge : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CUpdateMerge)

// Construction
public:
	CUpdateMerge(const CCvsrootEntryDlg* cvsrootEntryDlg, 
		const MultiFiles* mf,
		bool isCheckout, 
		const char* rev1, const char* rev2, const char* date1, const char* date2,
		const bool threeWayConflicts);
	~CUpdateMerge();

// Dialog Data
	//{{AFX_DATA(CUpdateMerge)
	enum { IDD = IDD_UPDATE_MERGE };
	CSmartComboBox	m_mergeTypeCombo;
	CButton	m_branchPointMergeCheck;
	CButton	m_threeWayConflictsCheck;
	CButton	m_hasDate2Check;
	CButton	m_hasDate1Check;
	CButton	m_browse2TagButton;
	CButton	m_browse1TagButton;
	CSmartComboBox	m_date2Combo;
	CSmartComboBox	m_date1Combo;
	CSmartComboBox	m_rev2Combo;
	CSmartComboBox	m_rev1Combo;
	CString	m_rev1;
	CString	m_rev2;
	BOOL	m_hasDate1;
	BOOL	m_hasDate2;
	CString	m_date1;
	CString	m_date2;
	BOOL	m_threeWayConflicts;
	BOOL	m_branchPointMerge;
	int		m_mergeType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUpdateMerge)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUpdateMerge)
	afx_msg void OnCheckdate1();
	afx_msg void OnCheckdate2();
	afx_msg void OnBrowseTag1();
	afx_msg void OnBrowseTag2();
	afx_msg void OnSelchangeMergeType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const CCvsrootEntryDlg* m_cvsrootEntryDlg;	/*!< CVSROOT entry */

	const MultiFiles* m_mf;	/*!< Browser selection */
	bool m_isCheckout;		/*!< true for the checkout, false for update */

	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_UPDATEDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
