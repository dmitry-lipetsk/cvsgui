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

// wincvsView.h : interface of the CWincvsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINCVSVIEW_H__D2D77DD0_8299_11D1_8949_444553540000__INCLUDED_)
#define AFX_WINCVSVIEW_H__D2D77DD0_8299_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxrich.h>
#include "AppConsole.h"

#if _RICHEDIT_VER >= 0x0210
#	define WCHARFORMAT CHARFORMAT2
#else
#	define WCHARFORMAT CHARFORMAT
#endif

/// Max character count in the line
#define MAX_CHAR_BY_LINE 1024

class CWincvsDoc;

/// Character format wrapper class
class CCharFormat : public WCHARFORMAT
{
public:
	// Construction
	CCharFormat()
	{
		USES_CONVERSION;
		
		CString strDefFont = _T("Courier New");
		//VERIFY(strDefFont.LoadString(IDS_DEFFONT));

		cbSize = sizeof(WCHARFORMAT);
		dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT | CFM_SIZE | CFM_COLOR | CFM_OFFSET | CFM_PROTECTED;
		dwEffects = CFE_AUTOCOLOR;
		yHeight = 200; //10pt
		yOffset = 0;
		crTextColor = RGB(0, 0, 0);
		bCharSet = 0;
		bPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

		ASSERT(strDefFont.GetLength() < LF_FACESIZE);
		
		lstrcpynA(szFaceName, T2A((LPTSTR)(LPCTSTR)strDefFont), LF_FACESIZE);
		dwMask |= CFM_FACE;
	}

	CCharFormat(const WCHARFORMAT& newf)
	{
		*(WCHARFORMAT*)this = newf;
	}

public:
	// Interface
	
	/// WCHARFORMAT& operator
	inline operator WCHARFORMAT&()
	{
		return *(WCHARFORMAT*)this;
	}
};

/// Console window class
class CWincvsView : public CRichEditView
{
protected: // create from serialization only
	CWincvsView();
	DECLARE_DYNCREATE(CWincvsView)

// Attributes
public:
	CWincvsDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWincvsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWincvsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWincvsView)
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMyFormatFont();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditClearAll();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	char* m_sbuf;		/*!< Print buffer */
	int m_snumbuf;		/*!< Print buffer size */
	UINT m_fRtfFormat;	/*!< Clipboard format */

	CCriticalSection m_syncObject;	/*!< Output synchronization object */

	// Methods
	virtual HRESULT QueryAcceptData(LPDATAOBJECT, CLIPFORMAT*, DWORD, BOOL, HGLOBAL);

	void OutLine(const char* buf, int numbuf, bool isStderr);
	BOOL CanPaste() const;

public:
	// Interface
	WCHARFORMAT GetCurFormat(bool jumpEnd = true);

	void OutConsole(const char* txt, size_t len, bool isStderr = false);

	void OutColor(const char* txt, long len);
	void OutColor(kConsoleColor color);
	void OutColor(WCHARFORMAT& format);
};

#ifndef _DEBUG  // debug version in wincvsView.cpp
inline CWincvsDoc* CWincvsView::GetDocument()
   { return (CWincvsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCVSVIEW_H__D2D77DD0_8299_11D1_8949_444553540000__INCLUDED_)
