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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- June 2004
 */

#if !defined(AFX_HHELPSUPPORT_H__E97D92A1_F935_41B3_9748_8000F0A1A76C__INCLUDED_)
#define AFX_HHELPSUPPORT_H__E97D92A1_F935_41B3_9748_8000F0A1A76C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HHelpSupport.h : header file
//

#include <functional>
#include "HHelp\wincvs.h"

/// Use to specify any control ID that doesn't have help topic
const DWORD helpTopicIgnore = -1;

/// Use to mark control-topic array mapping end
const DWORD helpTopicEnd = 0;

/// HTML help support class
class CHHelpSupport  
{
public:
	// Construction
	CHHelpSupport();
	~CHHelpSupport();
	
private:
	// Types
	typedef std::map<DWORD, DWORD> tHelpIDMap;	/*!< Help ID mapping container type */
	typedef std::vector<DWORD> tHelpIDsData;	/*!< Help ID data container type */

	// Data members
	tHelpIDMap m_hhelpIDs;	/*!< Help ID container */

protected:
	// Methods
	virtual CWnd* GetWnd();

	virtual void InitHelpID() = 0;
	
	void CreateHelpIDsData(tHelpIDsData& helpIDsData) const;

	BOOL DoOnHelpInfo(const HELPINFO* pHelpInfo);
	void DoOnContextMenu(const CWnd* pWnd, CPoint point);

	BOOL IsHelpCtrl(const UINT ctrlID) const;
	BOOL IsIgnoredCtrl(const UINT ctrlID) const;

public:
	// Interface
	void EnableWndContextHelp();
	DWORD SetCtrlHelpID(const DWORD ctrlID, const DWORD helpTopic);

	void ShowHelp();

	static LPCTSTR GetHelpFilePath();
	static void InitHTMLHelp();
};

/// HTML Help property sheet
class CHHelpPropertySheet : public CPropertySheet, public CHHelpSupport
{
	DECLARE_DYNAMIC(CHHelpPropertySheet)

// Construction
public:
	CHHelpPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CHHelpPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHHelpPropertySheet)
	public:
	virtual INT_PTR DoModal();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHHelpPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHHelpPropertySheet)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	// Methods
	virtual void InitHelpID();
};

/// HTML Help property page
class CHHelpPropertyPage : public CPropertyPage, public CHHelpSupport
{
	DECLARE_DYNAMIC(CHHelpPropertyPage)

// Construction
public:
	CHHelpPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
	CHHelpPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
	~CHHelpPropertyPage();

// Dialog Data
	//{{AFX_DATA(CHHelpPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHHelpPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHHelpPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/// HTML Help dialog
class CHHelpDialog : public CDialog, public CHHelpSupport
{
// Construction
public:
	CHHelpDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CHHelpDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CHHelpDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHHelpDialog)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHHelpDialog)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HHELPSUPPORT_H__E97D92A1_F935_41B3_9748_8000F0A1A76C__INCLUDED_)
