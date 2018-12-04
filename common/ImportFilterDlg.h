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
 * ImportFilterDlg.h - edit filtered import
 */

#if !defined(AFX_IMPORTFILDERDLG_H__CFB7AA41_BDDB_11D1_B965_0020AF51D9DA__INCLUDED_)
#define AFX_IMPORTFILDERDLG_H__CFB7AA41_BDDB_11D1_B965_0020AF51D9DA__INCLUDED_

#include "ImportFilter.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

bool CompatImportFilter(const char* path, ReportConflict*& entries, ReportWarning*& warnings);

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// CImportFilterDlg dialog

class CImportFilterDlg : public CHHelpDialog
{
// Construction
public:
	CImportFilterDlg(ReportConflict* entries, ReportWarning* warnings,
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportFilterDlg)
	enum { IDD = IDD_IMPORTFILTER };
	CListCtrl	m_list;
	CButton	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CImportFilterDlg)
	afx_msg void OnEditentry();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListentries(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CImageList m_imageList;		/*!< Image list */
	ReportConflict* m_entries;	/*!< Conflicts */
	ReportWarning* m_warnings;	/*!< Warnings */

	// Methods
	virtual void InitHelpID();
};

class CImportWarning : public CHHelpDialog
{
// Construction
public:
	CImportWarning(ReportWarning* warn, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportWarning)
	enum { IDD = IDD_WARNINGDESC };
	CString	m_desc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportWarning)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportWarning)
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

class CImportEntry : public CHHelpDialog
{
// Construction
public:
	CImportEntry(ReportConflict* entry, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportEntry)
	enum { IDD = IDD_ENTRYDESC };
	CString	m_desc;
	int m_radios;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportEntry)
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#ifdef qMacCvsPP
#include <LOutlineTable.h>
#include <LCommander.h>
#include <LOutlineItem.h>
#include <LString.h>

class COutlineTable : public LOutlineTable, public LCommander
{
public:
	enum { class_ID = 'Cout' };

	COutlineTable( LStream *inStream );
	virtual ~COutlineTable();
protected:
	virtual	void FinishCreateSelf();
private:
	// defensive programming
	COutlineTable();
	COutlineTable( const COutlineTable &inOriginal );
	COutlineTable&		operator=( const COutlineTable &inOriginal );
};

class CImportItem : public LOutlineItem
{
public:
	virtual void EditImportCell(void) = 0;
};

class CEntryItem : public CImportItem
{
public:
	CEntryItem(ReportConflict * entry);
	virtual ~CEntryItem();
	
	virtual void EditImportCell(void);
protected:

	virtual void GetDrawContentsSelf(const STableCell& inCell, SOutlineDrawContents& ioDrawContents);

	virtual void DrawRowAdornments(const Rect& inLocalRowRect);

	virtual void DoubleClick(const STableCell& inCell, const SMouseDownEvent& inMouseDown,
		const SOutlineDrawContents&	inDrawContents, Boolean inHitText);
	
	ReportConflict * mEntry;
private:
	// defensive programming
	CEntryItem();
	CEntryItem( const CEntryItem &inOriginal );
	CEntryItem&	operator=( const CEntryItem &inOriginal );
};

class CWarningItem : public CImportItem
{
public:
	CWarningItem(ReportWarning * warn);
	virtual ~CWarningItem();

	virtual void EditImportCell(void);
protected:

	virtual void GetDrawContentsSelf(const STableCell& inCell, SOutlineDrawContents& ioDrawContents);

	virtual void DrawRowAdornments(const Rect& inLocalRowRect);

	virtual void DoubleClick(const STableCell& inCell, const SMouseDownEvent& inMouseDown,
		const SOutlineDrawContents&	inDrawContents, Boolean inHitText);
	
	ReportWarning * mEntry;
private:
	// defensive programming
	CWarningItem();
	CWarningItem( const CWarningItem &inOriginal );
	CWarningItem&	operator=( const CWarningItem &inOriginal );
};
#endif /* qMacCvsPP */

#endif // !defined(AFX_IMPORTFILDERDLG_H__CFB7AA41_BDDB_11D1_B965_0020AF51D9DA__INCLUDED_)
