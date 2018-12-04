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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 2000
 */

#if !defined(AFX_CUSTOMIZEMENUS_H__28A6AE7E_C54B_47A5_A8F5_A127E2E9F3BF__INCLUDED_)
#define AFX_CUSTOMIZEMENUS_H__28A6AE7E_C54B_47A5_A8F5_A127E2E9F3BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomizeMenus.h : header file
//

#include "Persistent.h"
#include "HHelpSupport.h"

class CMMenuString;

/// Custom menu item
typedef struct
{
	const char* title;	/*!< Menu title */
	const char* key;	/*!< Key */
	int cmd;			/*!< Command */
	bool isTcl;			/*!< Flag to indicate Tcl macro */
} CustInvertTable;

const CustInvertTable* CustGetByKey(const char* entry);

CMenu* GetCustomMenu(int whichmenu, CView* pThis);
void CustomizeMenus(int whichmenu);

/////////////////////////////////////////////////////////////////////////////
// CCustomizeMenus dialog

class CCustomizeMenus : public CHHelpDialog
{
// Construction
public:
	CCustomizeMenus(CMMenuString* fromMenu, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomizeMenus)
	enum { IDD = IDD_CUSTOMIZE };
	CListCtrl	m_menuList;
	CListCtrl	m_cmdsList;
	CButton	m_upbtn;
	CButton	m_removebtn;
	CButton	m_downbtn;
	CButton	m_addbtn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomizeMenus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomizeMenus)
	afx_msg void OnRemove();
	afx_msg void OnAdd();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedAllcmds(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkMenulist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkAllcmds(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedMenulist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDown();
	afx_msg void OnUp();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CMMenuString* m_Menu;
	CImageList m_SmallImageList;
	
	// Methods
	virtual void InitHelpID();
	
	void OnMove(int offset);
	int GetIcon(const CustInvertTable* entry);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMIZEMENUS_H__28A6AE7E_C54B_47A5_A8F5_A127E2E9F3BF__INCLUDED_)
