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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

#if !defined(AFX_LINECMD_H__5946A8C5_8517_11D1_8949_444553540000__INCLUDED_)
#define AFX_LINECMD_H__5946A8C5_8517_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LineCmd.h : header file
//

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "MultilineEntryDlg.h"
#	include "CvsrootEntryDlg.h"
#endif

#include "CvsArgs.h"

#define DEF_CVS_CMD	CVS_CMD " "

class CvsArgs;
class MultiFiles;

bool CompatGetCommand(const MultiFiles* mf,
					  std::string& cmd, std::string& dir, bool& addDefault, bool& addSelection,
					  bool& forceCvsroot, std::string& cvsroot);

#ifdef WIN32

/// Command line settings property page
class CCmdLine_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CCmdLine_MAIN)

// Construction
public:
	CCmdLine_MAIN(const MultiFiles* mf, 
		const char* lastcmd, bool hasPath, const char* lastpath,
		bool addDefault, bool addSelection, 
		bool forceRoot, const char* cvsroot);
	~CCmdLine_MAIN();

// Dialog Data
	//{{AFX_DATA(CCmdLine_MAIN)
	enum { IDD = IDD_CMDLINE_MAIN };
	CStatic	m_cvsrootEntryPlaceholder;
	CStatic	m_multilineEntryPlaceholder;
	CSmartComboBox	m_folderLocCombo;
	CButton	m_addSelectionCheck;
	CButton	m_checkPathCheck;
	CButton	m_selFolderButton;
	BOOL	m_checkPath;
	CString	m_folderLoc;
	BOOL	m_addDefault;
	BOOL	m_addSelection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCmdLine_MAIN)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCmdLine_MAIN)
	afx_msg void OnCheckpath();
	afx_msg void OnSelfolder();
	virtual BOOL OnInitDialog();
	afx_msg void OnAddSelection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CCvsrootEntryDlg m_cvsrootEntryDlg; /*!< CVSROOT entry child dialog */
	const MultiFiles* m_mf;		/*!< Browser selection */
	bool m_hasSelection;		/*!< Flag to indicate there is a selection in the GUI */

	CString m_cmdLine;			/*!< Entered command line */
	
	CMultilineEntryDlg m_multilineEntryDlg; /*!< Multiline entry child dialog */

	// Methods
	virtual void InitHelpID();

public:
	// Interface
	CString GetCmdLine() const;

	BOOL m_forceRoot;	/*!< Force CVSROOT flag */
	CString m_cvsroot;	/*!< CVSROOT string */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_LINECMD_H__5946A8C5_8517_11D1_8949_444553540000__INCLUDED_)
