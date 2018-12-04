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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 2007
 */

/*
 * RemoveDlg.h : the cvs remove dialog
 */

#if !defined(AFX_REMOVEMAIN_H__CB4B8E19_FD86_4EE7_86C6_DC63F79C6957__INCLUDED_)
#define AFX_REMOVEMAIN_H__CB4B8E19_FD86_4EE7_86C6_DC63F79C6957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;

/// Files remove type enum
typedef enum
{
	kNoFilesRemove,
	kRemoveByCvs,
	kMoveToRecycleBin,
}kFilesRemoveType;

bool CompatGetRemove(const MultiFiles* mf, 
					 bool& noRecurs, 
					 kFilesRemoveType& filesRemoveType);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CRemoveMain dialog

class CRemoveMain : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CRemoveMain)

// Construction
public:
	CRemoveMain(const MultiFiles* mf, const bool noRecurs, const kFilesRemoveType filesRemoveType);
	~CRemoveMain();

// Dialog Data
	//{{AFX_DATA(CRemoveMain)
	enum { IDD = IDD_REMOVE_MAIN };
	CSmartComboBox	m_filesRemoveTypeCombo;
	BOOL	m_norecurs;
	int		m_filesRemoveType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRemoveMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRemoveMain)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;	/*!< Browser selection */

	// Methods
	virtual void InitHelpID();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_REMOVEMAIN_H__CB4B8E19_FD86_4EE7_86C6_DC63F79C6957__INCLUDED_)
