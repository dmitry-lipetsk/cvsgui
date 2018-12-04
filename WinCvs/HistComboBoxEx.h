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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- July 2001
 */

#if !defined(AFX_HISTCOMBOBOXEX_H__E330E7B9_8C79_44CB_8F07_01D395975365__INCLUDED_)
#define AFX_HISTCOMBOBOXEX_H__E330E7B9_8C79_44CB_8F07_01D395975365__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistComboBoxEx.h : header file
//

#define HISTLOC_IMAGE_CX		16
#define HISTLOC_VISIBLEITEMS	8

/// Browse location status icons
enum
{
	kLocationIconUnknown = 0,	/*!< Unknown icon */
	kLocationIcon,				/*!< CVS folder icon */
	kLocationIconIgnored,		/*!< Ignored icon */
	kLocationIconMissing,		/*!< Missing icon */
	kLocationIconErased			/*!< Erased icon */
};

/////////////////////////////////////////////////////////////////////////////
// CHistComboBoxEx window

/// Browse location history combo box class
class CHistComboBoxEx : public CComboBoxEx, public USmartCombo
{
// Construction
public:
	CHistComboBoxEx(int feature = USmartCombo::DefaultFeature);
	virtual ~CHistComboBoxEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistComboBoxEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistComboBoxEx)
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CImageList m_imageList;	/*!< Image list */

	// Methods
	virtual int CalcDroppedWidth();
	BOOL DelKeyTest(MSG* pMsg);

public:
	// Interface
	int AddString(LPCTSTR lpszString);
	int InsertString(int nIndex, LPCTSTR lpszString);
	BOOL SetIcon(COMBOBOXEXITEM cbi, bool showStatus);
	
	const char* GetOldLoc();
	int NewRoot(const char* root);
	static CString FindBestRoot(CMString& strItems, const char* path, bool mustExist = false);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTCOMBOBOXEX_H__E330E7B9_8C79_44CB_8F07_01D395975365__INCLUDED_)
