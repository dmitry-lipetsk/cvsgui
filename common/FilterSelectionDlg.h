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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- September 2003
 */

/*
 * FilterSelectionDlg.h : the filter selection dialog
 */

#if !defined(AFX_FILTERSELECTIONDLG_H__933ECF3F_C417_4C4E_AD6F_39769CCC4E6F__INCLUDED_)
#define AFX_FILTERSELECTIONDLG_H__933ECF3F_C417_4C4E_AD6F_39769CCC4E6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CvsEntries.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

bool CompatGetFilterSelection(const int columnIndex, std::string& filterText);

CMString* GetPrevFilter(const int columnIndex);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CFilterSelectionDlg dialog

class CFilterSelectionDlg : public CHHelpDialog
{
// Construction
public:
	CFilterSelectionDlg(const int columnIndex, const char* filterText, 
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilterSelectionDlg)
	enum { IDD = IDD_FILTER_SELECTION };
	CSmartComboBox	m_filterCombo;
	CSmartComboBox	m_contentsCombo;
	CString	m_filterText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterSelectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterSelectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeContentsCombo();
	afx_msg void OnDblclkCombofilter();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	int m_columnIndex;		/*!< Column index */

	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif	/* WIN32 */

#endif // !defined(AFX_FILTERSELECTIONDLG_H__933ECF3F_C417_4C4E_AD6F_39769CCC4E6F__INCLUDED_)
