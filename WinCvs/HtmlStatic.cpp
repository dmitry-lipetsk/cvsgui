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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- August 2005
 */

// HtmlStatic.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "HtmlStatic.h"
#include "LaunchHandlers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlStatic

CHtmlStatic::CHtmlStatic()
{
	m_textColor = RGB(0, 0, 255);
	m_ctrlBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
}

CHtmlStatic::~CHtmlStatic()
{
}


BEGIN_MESSAGE_MAP(CHtmlStatic, CStatic)
	//{{AFX_MSG_MAP(CHtmlStatic)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlStatic message handlers

/// PreSubclassWindow virtual override, prepare styles, fonts and tooltips
void CHtmlStatic::PreSubclassWindow() 
{
	// Add notify to enable click
	ModifyStyle(0, SS_NOTIFY);
	
	// Change the font to Underline
	if( CFont* pFont = GetFont() )
	{
		LOGFONT logFont = { 0 };
		
		if( pFont->GetLogFont(&logFont) )
		{
			logFont.lfUnderline = (BYTE)TRUE;
			m_underlineFont.CreateFontIndirect(&logFont);
			
			SetFont(&m_underlineFont);
		}
	}

	if( !m_tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX) || 
		!m_tooltip.AddTool(this, GetLink()) )
	{
		ASSERT(FALSE); // Can't add tooltip
	}

	CStatic::PreSubclassWindow();
}

/*!
	Get the link
	\return The link
*/
CString CHtmlStatic::GetLink() const
{
	CString strRes(m_link);

	if( strRes.IsEmpty() )
	{
		GetWindowText(strRes);
	}

	ASSERT(!strRes.IsEmpty()); // Empty link

	return strRes;
}

/// BN_CLICKED notification handler, open the link
void CHtmlStatic::OnClicked() 
{
	LaunchHandler(kLaunchOpen, GetLink());
}

/// WM_CTLCOLOR message handler, set the link color
HBRUSH CHtmlStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_textColor);
	pDC->SetBkMode(TRANSPARENT);
	
	return m_ctrlBrush;
}

/*!
	Set the link URL
	\param link Link to be set
	\note If the link is not explicitly set then window text will be used
*/
void CHtmlStatic::SetLink(const char* link)
{
	m_link = link;

	if( ::IsWindow(GetSafeHwnd()) )
	{
		m_tooltip.UpdateTipText(GetLink(), this);
	}
}

/// PreTranslateMessage virtual override, relay messages to the tool tip control
BOOL CHtmlStatic::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);
	
	return CStatic::PreTranslateMessage(pMsg);
}

/// WM_SETCURSOR message handler, set the hand cursor
BOOL CHtmlStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	return TRUE;
}
