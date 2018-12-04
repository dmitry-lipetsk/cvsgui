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

// wincvsDoc.h : interface of the CWincvsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINCVSDOC_H__D2D77DCE_8299_11D1_8949_444553540000__INCLUDED_)
#define AFX_WINCVSDOC_H__D2D77DCE_8299_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWincvsDoc : public CDocument
{
public:
	DECLARE_DYNCREATE(CWincvsDoc)

// Attributes
	CWincvsDoc();

	//virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo = NULL) const;

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWincvsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWincvsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWincvsDoc)
	afx_msg void OnAppPreferences();
	afx_msg void OnAppRtagNew();
	afx_msg void OnAppRtagDelete();
	afx_msg void OnAppRtagBranch();
	afx_msg void OnAppCmdline();
	afx_msg void OnUpdateCvsCmd(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrefs(CCmdUI* pCmdUI);
	afx_msg void OnBrowserOpen();
	afx_msg void OnAppLogin();
	afx_msg void OnAppLogout();
	afx_msg void OnViewInit();
	afx_msg void OnSavesettings();
	afx_msg void OnUpdateSavesettings(CCmdUI* pCmdUI);
	afx_msg void OnViewBrowselocationRemoveunwanted();
	afx_msg void OnUsePyshell();
	afx_msg void OnUpdateUsePyshell(CCmdUI* pCmdUI);
	afx_msg void OnUseTclshell();
	afx_msg void OnUpdateUseTclshell(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CWincvsDoc)
	CString m_viewPath;
	afx_msg void OnViewPathChanged();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCVSDOC_H__D2D77DCE_8299_11D1_8949_444553540000__INCLUDED_)
