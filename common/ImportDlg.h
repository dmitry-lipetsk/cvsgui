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
 * ImportDlg.h : the cvs import dialog
 */

#if !defined(AFX_IMPORTDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_IMPORTDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#	include "MultilineEntryDlg.h"
#	include "CvsrootEntryDlg.h"
#endif

class MultiFiles;
class ReportConflict;
class ReportWarning;

bool CompatGetImport(const MultiFiles* mf,
					 bool& serverCvswrappers, ReportConflict*& entries, ReportWarning*& warnings, 
					 std::string& modname, std::string& logmsg,
					 std::string& vendortag, std::string& reltag, std::string& vendorBranchId, std::string& path, 
					 bool& useDefIgnore, bool& useFilesTime, bool& createSandbox,
					 bool& dontCreateVendorTag, bool& overwriteReleaseTags, 
					 bool& forceCvsroot, std::string& cvsroot);

#ifdef WIN32

/// Import settings property page
class CImport_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CImport_MAIN)

// Construction
public:
	CImport_MAIN(const MultiFiles* mf, 
		const char* logMsg, const char* modname,
		const char* rtag, const char* vtag, const bool dontCreateVendorTag, const bool overwriteReleaseTags, 
		bool forceRoot, const char* cvsroot);
	~CImport_MAIN();

// Dialog Data
	//{{AFX_DATA(CImport_MAIN)
	enum { IDD = IDD_IMPORT_MAIN };
	CStatic	m_cvsrootEntryPlaceholder;
	CButton	m_overwriteReleaseTagsCheck;
	CStatic	m_multilineEntryPlaceholder;
	CButton	m_vtagButton;
	CButton	m_rtagButton;
	CButton	m_dontCreateVendorTagCheck;
	CSmartComboBox	m_moduleCombo;
	CSmartComboBox	m_rtagCombo;
	CSmartComboBox	m_vtagCombo;
	CString	m_vtag;
	CString	m_rtag;
	CString	m_moduleName;
	BOOL	m_dontCreateVendorTag;
	BOOL	m_overwriteReleaseTags;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImport_MAIN)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImport_MAIN)
	afx_msg void OnBrowseTag1();
	afx_msg void OnBrowseTag2();
	afx_msg void OnBrowseModules();
	virtual BOOL OnInitDialog();
	afx_msg void OnDontcreatevendortag();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;	/*!< Browser selection */
	CString m_logMsg;		/*!< Selected message */

	CMultilineEntryDlg m_multilineEntryDlg; /*!< Multiline entry child dialog */
	CCvsrootEntryDlg m_cvsrootEntryDlg;		/*!< CVSROOT entry child dialog */

	// Methods
	virtual void InitHelpID();

public:
	// Interface
	CString GetLogMsg() const;
	const CCvsrootEntryDlg& GetCvsrootEntryDlg() const;
	
	BOOL m_forceRoot;	/*!< Force CVSROOT flag */
	CString m_cvsroot;	/*!< CVSROOT string */
};

/// Import options property page
class CImport_OPTIONS : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CImport_OPTIONS)

// Construction
public:
	CImport_OPTIONS(const CCvsrootEntryDlg* cvsrootEntryDlg, 
		const MultiFiles* mf, 
		const bool serverCvswrappers, ReportConflict*& entries, ReportWarning*& warnings, 
		const char* path, const bool useDefIgnore, const bool useFilesTime, const bool createSandbox);
	~CImport_OPTIONS();

// Dialog Data
	//{{AFX_DATA(CImport_OPTIONS)
	enum { IDD = IDD_IMPORT_OPTIONS };
	CButton	m_serverCvswrappersCheck;
	CSmartComboBox	m_pathCombo;
	CSmartComboBox	m_vendorBranchCombo;
	CButton	m_vendorBranchButton;
	CButton	m_vendorBranchCheck;
	BOOL	m_useDefIgnore;
	BOOL	m_useFilesTime;
	BOOL	m_hasVendorBranch;
	CString	m_vendorBranch;
	BOOL	m_createSandbox;
	CString	m_path;
	BOOL	m_serverCvswrappers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CImport_OPTIONS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImport_OPTIONS)
	afx_msg void OnBtnpath();
	afx_msg void OnBrowseBranchid();
	afx_msg void OnBranchid();
	afx_msg void OnImportfilter();
	afx_msg void OnSelchangePath();
	afx_msg void OnServerCvswrappers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const CCvsrootEntryDlg* m_cvsrootEntryDlg;	/*!< CVSROOT entry */
	
	const MultiFiles* m_mf;		/*!< Browser selection */
	ReportConflict*& m_entries;	/*!< Conflicts */
	ReportWarning*& m_warnings;	/*!< Warnings */

	// Methods
	virtual void InitHelpID();
	
	bool ImportFilter(const char* path);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_IMPORTDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
