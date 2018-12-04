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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2001
 */

/*
 * AdminOptionDlg.h --- admin options dialog
 */

#if !defined(AFX_ADMINOPTIONDLG_H__C9A89564_F0A4_4294_8C6C_18469D18BCB3__INCLUDED_)
#define AFX_ADMINOPTIONDLG_H__C9A89564_F0A4_4294_8C6C_18469D18BCB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdminOptionDlg.h : header file
//

#ifdef WIN32
#	include "MultilineEntryDlg.h"
#	include "HHelpSupport.h"
#endif

/// Admin option dialog type
typedef enum
{
	kChangeLog,		/*!< Change log message */
	kSetState,		/*!< Set the revision state */
	kSetDescription	/*!< Set the file description */
} kAdminOptionDlgType;

bool CompatGetAdminOptions(const MultiFiles* mf, kAdminOptionDlgType type, std::string& optionString);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CAdminOption_MAIN dialog

class CAdminOption_MAIN : public CHHelpPropertyPage
{
// Construction
public:
	CAdminOption_MAIN(const MultiFiles* mf, kAdminOptionDlgType type, const char* optionString, 
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdminOption_MAIN)
	enum { IDD = IDD_ADMINOPTION };
	CStatic	m_multilineEntryPlaceholder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdminOption_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdminOption_MAIN)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	kAdminOptionDlgType m_type;	/*!< Option type */
	CString	m_optionString;		/*!< Option string */

	CMultilineEntryDlg m_multilineEntryDlg; /*!< Multiline entry child dialog */

	// Methods
	virtual void InitHelpID();
	
	void SetMultilineEntry();

public:
	// Interface
	CString GetOptionString() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#endif // !defined(AFX_ADMINOPTIONDLG_H__C9A89564_F0A4_4294_8C6C_18469D18BCB3__INCLUDED_)
