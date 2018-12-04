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
 * CheckoutDlg.cpp : the cvs checkout dialog
 */

#if !defined(AFX_CHECKOUTDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_CHECKOUTDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"
#include <string>

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "CvsrootEntryDlg.h"
#endif

class MultiFiles;

bool CompatGetCheckout(const MultiFiles* mf,
					   std::string& modname, std::string& path, bool& norecurs,
					   bool& toStdout, std::string& date, std::string& rev,
					   bool& useMostRecent, 
					   std::string& rev1, std::string& rev2,
					   bool& branchPointMerge, 
					   bool& threeWayConflicts, 
					   bool& forceCvsroot, std::string& cvsroot, 
					   bool& overrideCheckoutDir, std::string& checkoutDir,
					   std::string& keyword,
					   bool& resetSticky,
					   bool& dontShortenPaths,
					   bool& caseSensitiveNames,
					   bool& lastCheckinTime);

bool CompatGetExport(const MultiFiles* mf,
					   std::string& modname, std::string& path, bool& norecurs,
					   std::string& date, std::string& rev,
					   bool& useMostRecent, 
					   bool& forceCvsroot, std::string& cvsroot, 
					   bool& overrideCheckoutDir, std::string& checkoutDir,
					   std::string& keyword,
					   bool& dontShortenPaths,
					   bool& caseSensitiveNames,
					   bool& lastCheckinTime);

extern CMString gOldModules;

#ifdef WIN32

/// Main checkout / export page
class CCheckout_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CCheckout_MAIN)

// Construction
public:
	CCheckout_MAIN(const MultiFiles* mf, 
		const char* modname, const char* path, const bool dontShortenPaths,
		bool forceRoot, const char* cvsroot);
	~CCheckout_MAIN();

// Dialog Data
	//{{AFX_DATA(CCheckout_MAIN)
	enum { IDD = IDD_CHECKOUT_MAIN };
	CStatic	m_cvsrootEntryPlaceholder;
	CButton	m_dontShortenPathsCheck;
	CEdit	m_checkoutDirEdit;
	CButton	m_overrideCoDirCheck;
	CEdit	m_pathEdit;
	CSmartComboBox	m_moduleCombo;
	CString	m_moduleName;
	CString	m_path;
	BOOL	m_noRecurse;
	BOOL	m_overrideCoDir;
	CString	m_checkoutDir;
	BOOL	m_dontShortenPaths;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCheckout_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCheckout_MAIN)
	afx_msg void OnBtnPath();
	afx_msg void OnCheckOverridecheckoutdir();
	afx_msg void OnBrowseModules();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CCvsrootEntryDlg m_cvsrootEntryDlg; /*!< CVSROOT entry child dialog */
	const MultiFiles* m_mf;				/*!< Browser selection */

	// Methods
	virtual void InitHelpID();
	
public:
	// Interface
	const CCvsrootEntryDlg& GetCvsrootEntryDlg() const;

	BOOL m_forceRoot;	/*!< Force CVSROOT flag */
	CString m_cvsroot;	/*!< CVSROOT string */
};

//{{AFX_INSERT_LOCATION}}
#endif /* WIN32 */

#endif // !defined(AFX_CHECKOUTDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
