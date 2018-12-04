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
 * TagDlg.h : the cvs tag dialog
 */

#if !defined(AFX_TAGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_TAGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"
#include "CvsCommands.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;

bool CompatGetTag(const MultiFiles* mf, const kTagCmdType tagType, 
				  bool& noRecurs, bool& movDelBranch,
				  bool& overwriteExisting,
				  std::string& tagName, bool& checkUnmod,
				  std::string& date, std::string& rev, bool& useMostRecent);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CTag_MAIN dialog

class CTag_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CTag_MAIN)

// Construction
public:
	CTag_MAIN(const MultiFiles* mf, const kTagCmdType type, 
		const char* sLastTag, 
		bool noRecurs, bool movDelBranch,
		bool checkUnmod = false, bool overwriteExisting = false);
	~CTag_MAIN();

// Dialog Data
	//{{AFX_DATA(CTag_MAIN)
	enum { IDD = IDD_TAG_MAIN };
	CButton	m_browseTagButton;
	CButton	m_noRecursCheck;
	CStatic	m_tagDescStatic;
	CButton	m_grpBox;
	CButton	m_overwriteExistingCheck;
	CButton	m_checkUnmodCheck;
	CSmartComboBox	m_tagCombo;
	BOOL	m_noRecurs;
	BOOL	m_checkUnmod;
	BOOL	m_overwriteExisting;
	CString	m_tagName;
	BOOL	m_movDelBranch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTag_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTag_MAIN)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseTag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;
	kTagCmdType m_type;

	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_TAGDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
