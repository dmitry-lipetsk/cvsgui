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
 * Author : V.Antonevich <v_antonevich@hotmail.com>
 */

#include "stdafx.h"
#include "wincvs.h"
#include "SortHeaderCtrl.h"
#include "AppConsole.h"
#include "FilterSelectionDlg.h"
#include "BrowseFileView.h"
#include "BrowseViewColumn.h"

using namespace std;

/// Buffer size for GetItem call
#define GETITEM_BUFFER_SIZE	256

// Default filter change timeout
#define DEFAULT_FILTERCHANGETIMEOUT	(5 * 60 * 1000)

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl

IMPLEMENT_DYNAMIC(CSortHeaderCtrl, CHeaderCtrl)

BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortHeaderCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
	ON_NOTIFY_REFLECT(HDN_FILTERCHANGE, OnFilterChanged)
	ON_NOTIFY_REFLECT(HDN_FILTERBTNCLICK, OnFilterBtnClicked)
END_MESSAGE_MAP()

CSortHeaderCtrl::CSortHeaderCtrl()
{
	m_bSortAsc = TRUE;
	m_nSortCol = -1;
	m_bHeaderFilters = FALSE;

	ResetLastFilter();
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
}

/*!
	Set the sort column
	\param nCol Column to become sorted
	\param bAsc TRUE to sort in an ascending order, FALSE to sort in descending order
	\return The previous sort column
*/
int CSortHeaderCtrl::SetSortImage(const int nCol, const BOOL bAsc)
{
	const int nPrevCol = m_nSortCol;

	m_nSortCol = nCol;
	m_bSortAsc = bAsc;

	// Invalidate header control so that it gets redrawn
	Invalidate();

	return nPrevCol;
}

/*!
	Draw the item
	\param itemID Item ID (column)
	\param rcItem Item rectangle (the top half only if the filter bar is in use
	\param bSelected TRUE if the column is selected (mouse pressed), FALSE otherwise
	\param bSortCol TRUE if the column is the sorted column so the indicator will be drawn, FALSE otherwise
	\param bSortAsc TRUE if the column is sorted in the ascending order, FALSE if it's sorted in descending order
	\param hDC Handle to the devide context
*/
void CSortHeaderCtrl::DrawItem(const UINT itemID, const RECT rcItem, 
							   const BOOL bSelected, const BOOL bSortCol, const BOOL bSortAsc, 
							   const HDC hDC)
{
	CDC dc;

	dc.Attach(hDC);

	// Get the column rect
	CRect rcLabel(rcItem);

	// Save DC
	int nSavedDC = dc.SaveDC();

	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcLabel);
	dc.SelectObject(&rgn);
	rgn.DeleteObject();

	// Draw the header "button"
	dc.DrawFrameControl(&rcLabel, DFC_BUTTON, DFCS_BUTTONPUSH | (bSelected ? DFCS_PUSHED : 0));

	// Labels are offset by a certain amount	
	// This offset is related to the width of a space character
	int offset = dc.GetTextExtent(_T(" "), 1).cx * 2;

	// Get the column text and format
	TCHAR buf[GETITEM_BUFFER_SIZE];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = GETITEM_BUFFER_SIZE - 1;

	GetItem(itemID, &hditem);

	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS;

	if( hditem.fmt & HDF_CENTER)
	{
		uFormat |= DT_CENTER;
	}
	else if( hditem.fmt & HDF_RIGHT)
	{
		uFormat |= DT_RIGHT;
	}
	else
	{
		uFormat |= DT_LEFT;
	}

	// Adjust the rect if the mouse button is pressed on it
	if( bSelected )
	{
		rcLabel.left++;
		rcLabel.top += 2;
		rcLabel.right++;
	}

	// Adjust the rect further if Sort arrow is to be displayed
	if( bSortCol )
	{
		rcLabel.right -= 3 * offset;
	}

	rcLabel.left += offset;
	rcLabel.right -= offset;

	// Draw column label
	if( rcLabel.left < rcLabel.right )
	{
		const int prevBkMode = dc.SetBkMode(TRANSPARENT);
		const COLORREF prevTextColor = dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));

		dc.DrawText(buf, -1, rcLabel, uFormat);

		dc.SetBkMode(prevBkMode);
		dc.SetTextColor(prevTextColor);
	}

	// Draw the Sort arrow
	if( bSortCol )
	{
		CRect rcIcon(rcItem);

		// Set up pens to use for drawing the triangle
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen* pOldPen = dc.SelectObject(&penLight);

		offset = (rcIcon.bottom - rcIcon.top) / 4;

		if( bSortAsc )
		{
			// Draw triangle pointing downwards
			dc.MoveTo(rcIcon.right - offset-1, offset);
			dc.LineTo(rcIcon.right - 2*offset-1, rcIcon.bottom - offset);
			dc.MoveTo(rcIcon.right - 2*offset-2, rcIcon.bottom - offset);
			
			dc.SelectObject( &penShadow );
			dc.LineTo(rcIcon.right - 3*offset-1, offset);
			dc.LineTo(rcIcon.right - offset-1, offset);
		}
		else
		{
			// Draw triangle pointing upwards
			dc.MoveTo(rcIcon.right - 2* offset, offset);
			dc.LineTo(rcIcon.right - offset, rcIcon.bottom - offset-1);
			dc.LineTo(rcIcon.right - 3*offset-2, rcIcon.bottom - offset-1);
			dc.MoveTo(rcIcon.right - 3*offset-1, rcIcon.bottom - offset-1);
			
			dc.SelectObject(&penShadow);
			dc.LineTo(rcIcon.right - 2*offset, offset-1);
		}

		// Restore the pen
		dc.SelectObject(pOldPen);
	}

	// Restore dc
	dc.RestoreDC(nSavedDC);

	// Detach the dc before returning
	dc.Detach();
}

/*!
	Enable or disable the headers bar
	\param enable TRUE to enable filter bar, FALSE to disable
	\return The previous state of headers bar
*/
BOOL CSortHeaderCtrl::EnableHeaderFilters(const BOOL enable /*= TRUE*/)
{
	const BOOL blnPrevEnable = m_bHeaderFilters;

	if( enable != m_bHeaderFilters )
	{
		m_bHeaderFilters = enable;

		if( m_bHeaderFilters )
		{
			ModifyStyle(0, HDS_FILTERBAR);
			SetFilterChangeTimeout(DEFAULT_FILTERCHANGETIMEOUT);
		}
		else
		{
			ModifyStyle(HDS_FILTERBAR, 0);
		}

		Invalidate();
	}

	return blnPrevEnable;
}

/// NM_CUSTOMDRAW notification handler, draw sort order indication
void CSortHeaderCtrl::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMCUSTOMDRAW pNmcustomdraw = (LPNMCUSTOMDRAW)pNMHDR;
	
	if( CDDS_ITEM & pNmcustomdraw->dwDrawStage )
	{
		if( (const int)pNmcustomdraw->dwItemSpec == m_nSortCol )
		{
			if( CDDS_PREPAINT & pNmcustomdraw->dwDrawStage )
			{
				// Ask for the post-paint notification so we can draw sort indicator
				*pResult = CDRF_NOTIFYPOSTPAINT;
				return;
			}
			else if( CDDS_POSTPAINT & pNmcustomdraw->dwDrawStage )
			{
				RECT rcItem = pNmcustomdraw->rc;
				
				if( m_bHeaderFilters )
				{
					// Reduce the size to assure filter bar is visible
					rcItem.bottom = (rcItem.bottom - rcItem.top) / 2;
				}

				DrawItem(pNmcustomdraw->dwItemSpec, rcItem, 
					pNmcustomdraw->uItemState & CDIS_SELECTED, TRUE, m_bSortAsc, 
					pNmcustomdraw->hdc);
			}
		}
	}

	if( CDDS_PREPAINT == pNmcustomdraw->dwDrawStage )
	{
		// We want notifications for all items
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}
	
	*pResult = 0;
}

/// HDN_FILTERCHANGE notification handler, send the change notification to any observers
void CSortHeaderCtrl::OnFilterChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER pNMHeader = (LPNMHEADER)pNMHDR;

	CWnd* pFocusWnd = GetFocus();
	const bool hasFocus = pFocusWnd && pFocusWnd->GetParent() && (pFocusWnd->GetParent()->GetSafeHwnd() == GetSafeHwnd());

	CWincvsApp* pApp = (CWincvsApp*)AfxGetApp();
	CBrowseFileView* pBrowseFileView = pApp->GetFileView();
	
	if( pBrowseFileView )
	{
		CMString* prevFilter = GetPrevFilter(pBrowseFileView->GetColumnModel()->GetType(pNMHeader->iItem));
		if( prevFilter )
		{
			const CString strFilterText = GetFilterText(pNMHeader->iItem);
			if( !strFilterText.IsEmpty() )
			{
				const bool overrideLast = hasFocus && pNMHeader->iItem == m_nLastFilterChanged;
				if( overrideLast && !m_strLastInsertedFilterText.IsEmpty() )
				{
					prevFilter->Remove(m_strLastInsertedFilterText);
				}
				
				prevFilter->Insert(strFilterText);
				m_strLastInsertedFilterText = strFilterText;
			}
		}
	}
	
	if( hasFocus )
	{
		m_nLastFilterChanged = pNMHeader->iItem;
	}
	else
	{
		ResetLastFilter();
	}

	m_notificationManager.NotifyAll();

	*pResult = 0;
}

/// HDN_FILTERBTNCLICK notification handler, send the change notification to any observers
void CSortHeaderCtrl::OnFilterBtnClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHDFILTERBTNCLICK pNMBClick = (LPNMHDFILTERBTNCLICK)pNMHDR;

	*pResult = 0;

	string filterText = GetFilterText(pNMBClick->iItem);
	CWincvsApp* pApp = (CWincvsApp*)AfxGetApp();
	CBrowseFileView* pBrowseFileView = pApp->GetFileView();

	if( pBrowseFileView && 
		CompatGetFilterSelection(pBrowseFileView->GetColumnModel()->GetType(pNMBClick->iItem), filterText) )
	{
		if( !filterText.empty() )
		{
			SetFilterText(pNMBClick->iItem, filterText.c_str());
			*pResult = TRUE;
		}
	}
}

/*!
	Get the notification manager
	\return The notification manager reference
*/
NotificationManager* CSortHeaderCtrl::GetNotificationManager()
{
	return &m_notificationManager;
}

/*!
	Get the filter bar item text
	\param intItem Item	to get the text for
	\return The item bar text string
*/
CString CSortHeaderCtrl::GetFilterText(const int intItem)
{
	TCHAR buffer[GETITEM_BUFFER_SIZE] = { 0 };
	strcpy(buffer, "");

	if( intItem >= 0 && intItem < GetItemCount() )
	{
		HDITEM hdItem = { 0 };
		HDTEXTFILTER hdTextFilter = { 0 };
		
		hdTextFilter.pszText = buffer;
		hdTextFilter.cchTextMax = GETITEM_BUFFER_SIZE - 1;
		
		hdItem.mask = HDI_FILTER;
		hdItem.type = HDFT_ISSTRING;
		hdItem.pvFilter = &hdTextFilter;
		
		if( !GetItem(intItem, &hdItem) )
		{
			// Copy an empty string just to be sure the return value is correct
			strcpy(buffer, "");
		}
	}

	return buffer;
}

/*!
	Set the filter bar item text
	\param intItem Item	to set the text for
	\param text Text to set
	\return TRUE on success, FALSE otherwise
*/
BOOL CSortHeaderCtrl::SetFilterText(const int intItem, const char* text)
{
	ASSERT(intItem >= 0 && intItem < GetItemCount()); //item out-of-bound
	
	TCHAR buffer[GETITEM_BUFFER_SIZE] = { 0 };
	HDITEM hdItem = { 0 };
	HDTEXTFILTER hdTextFilter = { 0 };
	
	hdTextFilter.pszText = buffer;
	hdTextFilter.cchTextMax = GETITEM_BUFFER_SIZE - 1;
	
	hdItem.mask = HDI_FILTER;
	hdItem.type = HDFT_ISSTRING;
	hdItem.pvFilter = &hdTextFilter;

	strncpy(buffer, text, hdTextFilter.cchTextMax);

	return SetItem(intItem, &hdItem);
}

/*!
	Set the filter change timeout
	\param nFilterChangeTimeout Filter change timeout in seconds
*/
void CSortHeaderCtrl::SetFilterChangeTimeout(const int nFilterChangeTimeout)
{
	Header_SetFilterChangeTimeout(m_hWnd, nFilterChangeTimeout);
}

/// WM_KILLFOCUS message handler, reset the last filter cache
void CSortHeaderCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CHeaderCtrl::OnKillFocus(pNewWnd);
	
	ResetLastFilter();
}

/*!
	Reset the last filter cache data
*/
void CSortHeaderCtrl::ResetLastFilter()
{
	m_nLastFilterChanged = -1;
	m_strLastInsertedFilterText.Empty();
}

/// WM_KEYUP message handler, reset the last filter cache if Return key was pressed
void CSortHeaderCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( VK_RETURN == nChar )
	{
		ResetLastFilter();
	}

	CHeaderCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}
