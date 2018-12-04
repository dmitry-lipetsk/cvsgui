// CJMaskEdit.h : header file
//
// This file is part of the CJLibrary for Visual C++ / MFC
// Copyright C 1998-1999 COdejock Software, All Rights Reserved.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 1, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// Based on the article "A masked edit control (2)" by Jeremy Davis [jmd@jvf.co.uk]
// http://www.codeguru.com/editctrl/masked_edit2.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 2005/08/09 00:22:45 $
 * $Revision: 1.2 $
 * $Archive: /CodeJock/Include/CJMaskEdit.h $
 *
 * $History: CJMaskEdit.h $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:21p
 * Updated in $/CodeJock/Include
 * Overrode OnPaint() and  OnEraseBkgnd(...) for flicker free drawing.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:44p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/19/99    Time: 12:35a
 * Created in $/CodeJockey/Include
 * Initial creation and addition to library.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMASKEDIT_H__
#define __CJMASKEDIT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CCJMaskEdit is a CEdit derived class which allows text masking to be 
// applied to the control to format it for special editing restrictions.
class _CJX_EXT_CLASS CCJMaskEdit : public CEdit
{
	DECLARE_DYNAMIC(CCJMaskEdit)

// Construction
public:
	CCJMaskEdit();

// Attributes
public:

	bool		m_bUseMask;				// true to use edit mask.
	bool		m_bMaskKeyInProgress;	// true when 
	CString		m_strWindowText;		// buffer that holds the actual edit text.
	CString		m_strMask;				// buffer that holds the actual edit mask value.
	CString		m_strLiteral;
	CString		m_strValid;
	CString		m_strMaskLiteral;
	CFont		m_Font;					// font used by masked edit control.

// Operations
public:

	virtual void SetEditMask(LPCTSTR lpszMask, LPCTSTR lpszLiteral, LPCTSTR lpszWindowText);
	virtual void SendChar(UINT nChar);
	virtual bool CheckChar(UINT nChar);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJMaskEdit)
	//}}AFX_VIRTUAL

	virtual bool ProcessMask(UINT nChar, int nEndPos);

// Implementation
public:
	virtual void UpdateFont();
	virtual ~CCJMaskEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJMaskEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

// CCJDateEdit is a CCJMaskEdit derived class which is specifically 
// geared toward editing date fields.
class _CJX_EXT_CLASS CCJDateEdit : public CCJMaskEdit
{
	DECLARE_DYNAMIC(CCJDateEdit)

// Construction
public:
	CCJDateEdit();

	virtual void SetDateTime(COleDateTime& dt);
	virtual void SetDateTime(CString strDate);
	virtual COleDateTime GetDateTime();
	virtual CString GetWindowDateTime();
	virtual COleDateTime ReadOleDateTime(LPCTSTR lpszData);

	virtual bool ProcessMask(UINT nChar, int nEndPos);
	virtual void FormatOleDateTime(CString& strData, COleDateTime dt);
};

// CCJTimeEdit is a CCJDateEdit derived class which is specifically geared 
// toward editing time fields.
class _CJX_EXT_CLASS CCJTimeEdit : public CCJDateEdit
{
	DECLARE_DYNAMIC(CCJTimeEdit)

// Construction
public:
	CCJTimeEdit();

	CString		m_strHours;
	CString		m_strMins;

	virtual void SetHours(int nHours);
	virtual void SetMins(int nMins);

	virtual bool ProcessMask(UINT nChar, int nEndPos);
	virtual void FormatOleDateTime(CString& strData, COleDateTime dt);
};

/////////////////////////////////////////////////////////////////////////////
// DDX for date mask control

void AFXAPI DDX_OleDateTime(CDataExchange* pDX, int nIDC, CCJDateEdit& rControl, COleDateTime& rDateTime);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJMASKEDIT_H__

