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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- July 1998
 */

/*
 * RtagDlg.h : the cvs rtag dialog
 */

#if !defined(AFX_RTAGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_RTAGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"
#include "CvsCommands.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "CvsrootEntryDlg.h"
#endif

class MultiFiles;

bool CompatGetRtag(const MultiFiles* mf, const kTagCmdType tagType, 
				   bool& noRecurs, bool& movDelBranch,
				   bool& overwriteExisting, std::string& tagName,
				   std::string& modName, std::string& date, std::string& rev,
				   bool& useMostRecent, bool& lookAttic,
				   bool& forceCvsroot, std::string& cvsroot);

extern CMString gOldModules;

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// CRtag_MAIN dialog

class CRtag_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CRtag_MAIN)

// Construction
public:
	CRtag_MAIN(const MultiFiles* mf, const kTagCmdType type,
		const char* sLastModule, const char* sLastTag,
		bool noRecurs, bool movDelBranch, bool lookAttic, 
		bool forceRoot, const char* cvsroot,
		bool overwriteExist = false);
	~CRtag_MAIN();

// Dialog Data
	//{{AFX_DATA(CRtag_MAIN)
	enum { IDD = IDD_RTAG_MAIN };
	CStatic	m_cvsrootEntryPlaceholder;
	CButton	m_overwriteExistingCheck;
	CStatic	m_moduleDescStatic;
	CStatic	m_tagDescStatic;
	CButton	m_grpBox;
	CSmartComboBox	m_moduleCombo;
	CSmartComboBox	m_tagCombo;
	CString	m_tagName;
	CString	m_moduleName;
	BOOL	m_noRecurs;
	BOOL	m_lookAttic;
	BOOL	m_overwriteExisting;
	BOOL	m_movDelBranch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRtag_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRtag_MAIN)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseTag();
	afx_msg void OnBrowseModules();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CCvsrootEntryDlg m_cvsrootEntryDlg; /*!< CVSROOT entry child dialog */

	const MultiFiles* m_mf;				/*!< Browser selection */
	kTagCmdType m_type;					/*!< Tag command type */

	// Methods
	virtual void InitHelpID();

public:
	// Interface
	const CCvsrootEntryDlg& GetCvsrootEntryDlg() const;

	BOOL m_forceRoot;	/*!< Force CVSROOT flag */
	CString m_cvsroot;	/*!< CVSROOT string */
};

/////////////////////////////////////////////////////////////////////////////
// CRtag_OPTIONS dialog

class CRtag_OPTIONS : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CRtag_OPTIONS)

// Construction
public:
	CRtag_OPTIONS(const CCvsrootEntryDlg* cvsrootEntryDlg, 
		const MultiFiles* mf, const kTagCmdType type,
		bool hasDate, bool hasRev,
		const char* sLastDate, const char* sLastRev,
		bool useMostRecent);
	~CRtag_OPTIONS();

// Dialog Data
	//{{AFX_DATA(CRtag_OPTIONS)
	enum { IDD = IDD_RTAG_OPTIONS };
	CButton	m_useMostRecentCheck;
	CButton	m_browseTagButton;
	CButton	m_hasRevCheck;
	CButton	m_hasDateCheck;
	CSmartComboBox	m_revCombo;
	CSmartComboBox	m_dateCombo;
	CButton	m_grpBox;
	CString	m_date;
	CString	m_rev;
	BOOL	m_hasDate;
	BOOL	m_hasRev;
	BOOL	m_useMostRecent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRtag_OPTIONS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRtag_OPTIONS)
	afx_msg void OnDate();
	afx_msg void OnRev();
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseTag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const CCvsrootEntryDlg* m_cvsrootEntryDlg;	/*!< CVSROOT entry */
	
	const MultiFiles* m_mf;	/*!< Browser selection */
	kTagCmdType m_type;		/*!< Tag command type */

	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_RTAGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
