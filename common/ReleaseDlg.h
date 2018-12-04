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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2000
 */

/*
 * ReleaseDlg.h : the cvs release dialog
 */

#if !defined(AFX_RELEASEDLG_H__12B5FA97_703E_4616_B361_F2917D45E54A__INCLUDED_)
#define AFX_RELEASEDLG_H__12B5FA97_703E_4616_B361_F2917D45E54A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "CvsrootEntryDlg.h"
#endif

#include "ustr.h"

class MultiFiles;

/// Release command delete option enum
typedef enum
{
	kReleaseNoDel,		/*!< No delete */
	kReleaseDelDir,		/*!< Delete directory (-d) */
	kReleaseDelAll,		/*!< Delete directory and all files (even non-CVS controlled) (-f) */
	kReleaseDelCvsInfo	/*!< Delete CVS meta-data files (-e) */
} kReleaseDelType;

bool CompatGetRelease(const MultiFiles* mf, 
					  const char* dir, kReleaseDelType& removeDirectoryOption,
					  bool& forceCvsroot, std::string& cvsroot);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CRelease_MAIN dialog

class CRelease_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CRelease_MAIN)

// Construction
public:
	CRelease_MAIN(const char* dir,
		bool forceRoot, const char* cvsroot);
	~CRelease_MAIN();

// Dialog Data
	//{{AFX_DATA(CRelease_MAIN)
	enum { IDD = IDD_RELEASE_MAIN };
	CStatic	m_cvsrootEntryPlaceholder;
	CButton	m_delDirCheck;
	CButton	m_delCvsInfoCheck;
	CButton	m_delAllCheck;
	CString	m_dir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRelease_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRelease_MAIN)
	afx_msg void OnDelcvsinfo();
	afx_msg void OnDeldir();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CCvsrootEntryDlg m_cvsrootEntryDlg;			/*!< CVSROOT entry child dialog */
	kReleaseDelType m_removeDirectoryOption;	/*!< Remove directory option */

	// Methods
	virtual void InitHelpID();

public:
	// Interface
	BOOL m_forceRoot;	/*!< Force CVSROOT flag */
	CString m_cvsroot;	/*!< CVSROOT string */

	kReleaseDelType GetRemoveDirectoryOption() const;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /* WIN32 */


#endif // !defined(AFX_RELEASEDLG_H__12B5FA97_703E_4616_B361_F2917D45E54A__INCLUDED_)
