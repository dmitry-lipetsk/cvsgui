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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com>
 */

// CvsStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "wincvs.h"
#include "CvsStatusBar.h"
#include "CvsPrefs.h"
#include "CvsSandboxInfo.h"
#include "AppConsole.h"
#include "BrowseViewModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Status bar indicators array
static UINT indicators[] =
{
	ID_SEPARATOR,		/*!< Status line     - kStatusMain */ 
	ID_SEPARATOR,		/*!< CVSROOT         - kStatusCVSROOT */ 
	ID_SEPARATOR,		/*!< Repository path - kStatusRepository */ 
	ID_SEPARATOR,		/*!< Rotating icon   - kStatusThrobber */
	ID_INDICATOR_CAPS,	/*!< Caps Lock */
	ID_INDICATOR_NUM,	/*!< Num Lock */
	ID_INDICATOR_SCRL,	/*!< Scroll Lock */
}; 

/*!
	Ignore changed notification function
	\param obj Browser view to be notified
*/
static void OnIgnoreChanged(CObject* obj)
{
	((CCvsStatusBar*)obj)->OnIgnoreChanged();
}

/*!
	Filtering changed notification function
	\param obj Browse file view to be notified
*/
static void OnFilteringChanged(CObject* obj)
{
	((CCvsStatusBar*)obj)->OnFilteringChanged();
}

/////////////////////////////////////////////////////////////////////////////
// CCvsStatusBar

CCvsStatusBar::CCvsStatusBar() 
	: m_step(-1), m_settimer(false)
{
	m_indic.LoadBitmap(IDB_IDLE);

	m_hasFilters = false;
	m_isShowIgnored = false;
}

CCvsStatusBar::~CCvsStatusBar()
{
}

BEGIN_MESSAGE_MAP(CCvsStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CCvsStatusBar)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCvsStatusBar message handlers

/*!
	Set the status bar indicators
	\return TRUE on success, FALSE otherwise
	\note Must be called after the status bar is created
*/
BOOL CCvsStatusBar::SetIndicators()
{
	return CStatusBar::SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
}

/*!
	Refresh status bar pane
	\param pane Pane id to be refreshed
*/
void CCvsStatusBar::RefreshPane(kStatusPaneId pane)
{
	CRect rect;
	GetItemRect(kStatusInfo, rect);
	InvalidateRect(rect, FALSE);
}

/*!
	Update path info
	\param path Path
*/
void CCvsStatusBar::UpdatePathInfo(const char* path)
{
	std::auto_ptr<CCvsSandboxInfo> pSandboxInfo(new CCvsSandboxInfo(path));
	if( pSandboxInfo.get() )
	{
		SetPaneText(kStatusCVSROOT, pSandboxInfo->GetRoot().c_str());
		SetPaneText(kStatusRepository, pSandboxInfo->GetRepository().c_str());
	}
}

#define COLOR_FILTEREDINFOBACKGROUND RGB(255, 128, 255)

/// DrawItem virtual override, draw animation while CVS command is running
void CCvsStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( !m_settimer )
	{
		SetTimer(999, 100, 0L);
		m_settimer = true;
	}

	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() || gCvsPrefs.IsTclFileRunning() )
	{
		SetPaneText(kStatusInfo, _i18n("CVS command is running"));
		DrawCvsRunningAnimation(lpDrawItemStruct);
	}
	else
	{
		if( m_hasFilters || m_isShowIgnored )
		{
			SetPaneText(kStatusInfo, _i18n("View is filtered"));
			DrawTextRect(lpDrawItemStruct, COLOR_FILTEREDINFOBACKGROUND, _i18n("Filtered"));
		}
		else
		{
			SetPaneText(kStatusInfo, "");
		}
	}
}

/*!
	Draw animation while CVS command is running
	\param lpDrawItemStruct Draw item structure
*/
void CCvsStatusBar::DrawCvsRunningAnimation(const LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	static int tab[12] = {
		-2, -1, 1, 4, 8, 11, 13, 11,
			8, 4, 1, -1};
		
	CBrush brush(RGB(0, 0, 255));
	CRect rect(lpDrawItemStruct->rcItem);
	
	CDC hdc;
	hdc.Attach(lpDrawItemStruct->hDC);
	
	hdc.FillRect(rect, &brush);
	
	CPoint off(rect.TopLeft());
	off.x -= m_step;
	
	int cnt = 0;
	while(off.x < rect.right)
	{
		CPoint offy(off);
		offy.y += tab[(m_step + cnt) % 12];
		hdc.DrawState(offy, CSize(16,16), m_indic, DSS_NORMAL);
		off.x += 24;
		cnt++;
	}
	
	hdc.Detach();
}

/*!
	Draw text on a color background
	\param lpDrawItemStruct Draw item structure
	\param color Background color
	\param text Text to draw
*/
void CCvsStatusBar::DrawTextRect(const LPDRAWITEMSTRUCT lpDrawItemStruct, const COLORREF color, const char* text)
{
	CRect rect(lpDrawItemStruct->rcItem);
	CBrush brush(color);
	
	CDC hdc;
	hdc.Attach(lpDrawItemStruct->hDC);
	
	hdc.FillRect(rect, &brush);
	
	hdc.SetBkColor(color);
	hdc.DrawText(text, -1, rect, DT_END_ELLIPSIS | DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	
	hdc.Detach();
}

/// WM_TIMER message handler, trigger animation
void CCvsStatusBar::OnTimer(UINT_PTR /*nIDEvent*/) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app->IsCvsRunning() || gCvsPrefs.IsTclFileRunning() )
	{
		if( m_step-- <= 0 )
			m_step = 23;

		RefreshPane(kStatusInfo);
	}
	else if( m_step != -1 )
	{
		m_step = -1;
		RefreshPane(kStatusInfo);
	}
}

/// WM_SIZE message handler, resize indicators
void CCvsStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	
	if( CommandToIndex(ID_SEPARATOR) >= 0 )
	{
		UINT nID;
		UINT nStyle;
		int cxWidth;

		GetPaneInfo(kStatusMain, nID, nStyle, cxWidth);
		SetPaneInfo(kStatusMain, nID, nStyle, int(floor(cx * .10)));

		GetPaneInfo(kStatusCVSROOT, nID, nStyle, cxWidth);
		SetPaneInfo(kStatusCVSROOT, nID, nStyle, int(floor(cx * .25)));

		GetPaneInfo(kStatusRepository, nID, nStyle, cxWidth);
		SetPaneInfo(kStatusRepository, nID, nStyle, int(floor(cx * .25)));

		GetPaneInfo(kStatusInfo, nID, nStyle, cxWidth);
		SetPaneInfo(kStatusInfo, nID, nStyle | SBPS_OWNERDRAW, 100);
	}
}

/// WM_CREATE message handler, set indicators and create tool tip control
int CCvsStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set indicators
	SetIndicators();
	
	// Create tool tip
	if( !m_tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX) || 
		!m_tooltip.AddTool(this, "Status Bar") )
	{
		cvs_err("Unable to create tool tip control for status bar\n");
	}

	// Register for filter changes notifications
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->GetFilterModel()->GetNotificationManager()->CheckIn(this, ::OnFilteringChanged);
		app->GetIgnoreModel()->GetNotificationManager()->CheckIn(this, ::OnIgnoreChanged);
	}

	return 0;
}

/// PreTranslateMessage virtual override, relay messages to the tool tip control
BOOL CCvsStatusBar::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg);

	return CStatusBar::PreTranslateMessage(pMsg);
}

/// WM_NCHITTEST message handler, set the tool tip text
#if _MSC_VER >= 1400
LRESULT CCvsStatusBar::OnNcHitTest(CPoint point) 
#else
UINT CCvsStatusBar::OnNcHitTest(CPoint point) 
#endif
{
	// Set the tool tip text according to the mouse position pane
	const int paneCount = GetCount();
	for(int nIndex = 0; nIndex < paneCount; nIndex++)
	{
		CRect paneRect;
		GetItemRect(nIndex, paneRect);
		ClientToScreen(paneRect);

		if( paneRect.PtInRect(point) )
		{
			CString strCurTip;
			m_tooltip.GetText(strCurTip, this);

			CString strTip;

			if( nIndex <= kStatusInfo )
			{
				strTip = GetPaneText(nIndex);
			}

			if( strTip != strCurTip )
			{
				m_tooltip.UpdateTipText(strTip, this);
			}

			break;
		}
	}
	
	return CStatusBar::OnNcHitTest(point);
}

/// Ignore changed notification handler, update display
void CCvsStatusBar::OnIgnoreChanged()
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		const bool isShowIgnored = ((KoIgnoreModel*)app->GetIgnoreModel())->IsShowIgnored();
		if( isShowIgnored != m_isShowIgnored )
		{
			m_isShowIgnored = isShowIgnored;
			RefreshPane(kStatusInfo);
		}
	}
}

// Filtering changed notification handler, update display
void CCvsStatusBar::OnFilteringChanged()
{
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		const bool hasFilters = ((KoFilterModel*)app->GetFilterModel())->HasFilters();
		if( hasFilters != m_hasFilters )
		{
			m_hasFilters = hasFilters;
			RefreshPane(kStatusInfo);
		}
	}
}
