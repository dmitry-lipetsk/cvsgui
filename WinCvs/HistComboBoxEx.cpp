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

// HistComboBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "HistComboBoxEx.h"
#include "FileTraversal.h"
#include "CvsEntries.h"

#include "MainFrm.h"
#include "CvsAlert.h"
#include "CvsCommands.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Browse location
CPersistentString gOldLoc("P_BrowserLoc", "C:\\");

/*!
	Traverse browse location history items
	\param browseHist Browse location combo box
	\return The number of browse history combo box items
*/
int TraverseHistory(CHistComboBoxEx& browseHist)
{
	CWaitCursor waitCursor;

	char szPath[MAX_PATH] = "";
	int nIndex = 0;

	for(nIndex = 0; nIndex < browseHist.GetCount(); nIndex++)
	{
		COMBOBOXEXITEM cbi = { 0 };
		cbi.mask = CBEIF_TEXT;
		cbi.iItem = nIndex;
		cbi.pszText = szPath;
		cbi.cchTextMax = MAX_PATH - 1;
		
		browseHist.GetItem(&cbi);
		browseHist.SetIcon(cbi, true);
	}

	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
// CHistComboBoxEx

CHistComboBoxEx::CHistComboBoxEx(int feature /*= USmartCombo::DefaultFeature*/)
	: USmartCombo(feature)
{
}

CHistComboBoxEx::~CHistComboBoxEx()
{
}


BEGIN_MESSAGE_MAP(CHistComboBoxEx, CComboBoxEx)
	//{{AFX_MSG_MAP(CHistComboBoxEx)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistComboBoxEx message handlers

/*!
	Get the browse location
	\return The browse location
*/
const char* CHistComboBoxEx::GetOldLoc()
{
	return gOldLoc.c_str();
}

/*!
	Set new browse location root
	\param root New browse location root
	\return The browse location items count
*/
int CHistComboBoxEx::NewRoot(const char* root)
{
	if( stricmp(gOldLoc.c_str(), root) != 0 )
		gOldLoc = root;

	CMString* pstrItems = GetItems();
	ASSERT(pstrItems != NULL);

	// Update the history combo
	pstrItems->Insert(root);
	ResetContent();

	const CMString::list_t& list = pstrItems->GetList();
	for(CMString::list_t::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		InsertString(-1, i->c_str());
	}

	if( GetCount() )
	{
		// Set the selection and the status icon
		CString strText;

		COMBOBOXEXITEM cbi = { 0 };
		cbi.mask = CBEIF_TEXT;
		cbi.iItem = 0;
		cbi.pszText = strText.GetBuffer(MAX_PATH);
		cbi.cchTextMax = MAX_PATH - 1;
		
		if( GetItem(&cbi) )
		{
			SetIcon(cbi, true);
		}

		strText.ReleaseBuffer();
		
		SetCurSel(0);
	}

	return GetCount();
}

/*!
	Find the best root for the passed path
	\param strItems Items to search in
	\param path Path to find the root for
	\param mustExist true if the path must exist, false otherwise
	\return The best root for the passed path
*/
CString CHistComboBoxEx::FindBestRoot(CMString& strItems, const char* path, bool mustExist /*= false*/)
{
	CString normPath(path);
	CString bestRoot;
	normPath.TrimRight("\\/");
	normPath.MakeLower();

	const CMString::list_t& list = strItems.GetList();
	for(CMString::list_t::const_iterator i = list.begin(); i != list.end(); ++i)
	{
		CString root = i->c_str();
		root.TrimRight("\\/");
		root.MakeLower();

		if( normPath.Find(root) != -1 )
		{
			// We want the longest root so it's near the file
			if( root.GetLength() > bestRoot.GetLength() )	
			{
				bestRoot = i->c_str();
			}
		}
	}

	if( !mustExist && bestRoot.IsEmpty() )
	{
		bestRoot = path;
		bestRoot.TrimRight("\\/");
	}

	// Handle the drive root
	if( bestRoot.GetAllocLength() &&
		':' == bestRoot[bestRoot.GetLength()-1])
	{
		bestRoot += kPathDelimiter;
	}
	
	return bestRoot;
}

/*!
	Insert string into drop-down list
	\param nIndex Index to insert the string at
	\param lpszString String to be inserted
	\return The index of the newly inserted item
	\note Doesn't set the status icon
*/
int CHistComboBoxEx::InsertString(int nIndex, LPCTSTR lpszString)
{
	int nRes = -1;

	COMBOBOXEXITEM cbi = { 0 };
	cbi.mask = CBEIF_TEXT;
	cbi.iItem = 0 > nIndex ? GetCount() : nIndex;
	cbi.pszText = (LPTSTR)lpszString;	// This cast is safe because we are inserting an item only
	cbi.cchTextMax = 0;

	nRes = InsertItem(&cbi);
	SetIcon(cbi, false);

	return nRes;
}

/*!
	Add the string to the end of the drop-down list
	\param lpszString String to be added
	\return true on success, false otherwise
*/
int CHistComboBoxEx::AddString(LPCTSTR lpszString)
{
	return InsertString(-1, lpszString);
}

/*!
	Set the item icon
	\param cbi Combo box item data
	\param showStatus true to show the status icon, false to set the default folder icon
	\return TRUE on success, FALSE otherwise
*/
BOOL CHistComboBoxEx::SetIcon(COMBOBOXEXITEM cbi, bool showStatus)
{
	// Initialize the image list if necessary
	if( m_imageList.m_hImageList == 0L )
	{
		m_imageList.Create(IDB_HISTLOC, HISTLOC_IMAGE_CX, 1, RGB(255, 255, 255));
		SetImageList(&m_imageList);
	}
	
	cbi.mask = CBEIF_IMAGE | /*CBEIF_OVERLAY |*/ CBEIF_SELECTEDIMAGE;
	cbi.iImage = cbi.iSelectedImage = kLocationIconUnknown;
	//cbi.iOverlay = ;

	if( showStatus )
	{
		CSortList<ENTNODE> entries(200, ENTNODE::Compare);
		ignored_list_type ignlist;
		struct stat sb;
		const char *fullpath = cbi.pszText;
		
		int statres = stat(fullpath, &sb);
		if( statres != -1 )
		{
			if( S_ISDIR(sb.st_mode) )
			{
				string uppath, folder;
				if( !SplitPath(fullpath, uppath, folder) || uppath.empty() )
				{
					uppath = fullpath;
					folder = ".";
				}
				
				BuildIgnoredList(ignlist, uppath.c_str());
				
				EntnodeData *data = Entries_SetVisited(uppath.c_str(), entries, folder.c_str(), sb, true, &ignlist);
				data->Ref();

				if( !data->IsUnknown() )
				{
					cbi.iImage = cbi.iSelectedImage = kLocationIcon;
				}

				if( data->IsIgnored() )
				{
					cbi.iImage = cbi.iSelectedImage = kLocationIconIgnored;
				}

				if( data->IsMissing() )
				{
					cbi.iImage = cbi.iSelectedImage = kLocationIconMissing;
				}

				data->UnRef();
			}
		}
		else
		{
			cbi.iImage = cbi.iSelectedImage = kLocationIconErased;
		}
	}

	return SetItem(&cbi);
}

/// PreTranslateMessage virtual override, perform the key test to determine whether to delete item
BOOL CHistComboBoxEx::PreTranslateMessage(MSG* pMsg) 
{
	if( HasFeature(USmartCombo::RemoveItems) )
	{
		if( DelKeyTest(pMsg) )
		{
			return TRUE;
		}
	}

	return CComboBoxEx::PreTranslateMessage(pMsg);
}

/// CBN_DROPDOWN notification message handler, traverse the history items to set the appropriate icons and calculate the width of the drop-down list
void CHistComboBoxEx::OnDropdown() 
{
	TraverseHistory(*this);
	if( HasFeature(USmartCombo::AutoDropWidth) )
	{
		SetDroppedWidth(CalcDroppedWidth());
	}
}

/*!
	Calculate the proper width for drop-down list so the items can fit into it
	\return The width of the drop-down list
	\note Make sure it doesn't go off-screen as well
*/
int CHistComboBoxEx::CalcDroppedWidth()
{
	int maxSize = 0;

	CClientDC ClientDC(this);
	int savedDc = ClientDC.SaveDC();
    ClientDC.SelectObject(GetFont());
	
	CString strLBText;
	for(int nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		GetLBText(nIndex, strLBText);
		CSize size = ClientDC.GetTextExtent(strLBText);
		
		if( size.cx > maxSize )
		{
			maxSize = size.cx;
		}
	}

	ClientDC.RestoreDC(savedDc);

	maxSize += ::GetSystemMetrics(SM_CXVSCROLL)		// Vertical scroll bar
		+ 4*::GetSystemMetrics(SM_CXEDGE)			// The border aroung the text and icon: Edge-Icon-Edge - Edge-Text-Edge
		+ HISTLOC_IMAGE_CX							// The icon width
		+ 2;										// The focus frame width
	
	CRect rect;
	GetWindowRect(rect);

#ifdef MULTIMONITOR_SUPPORT
	HMONITOR hMonitor = MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST);
	MONITORINFO MonitorInfo;
	MonitorInfo.cbSize = sizeof(MonitorInfo);
	VERIFY(GetMonitorInfo(hMonitor, &MonitorInfo));
	
	int maxSizeClipped = MonitorInfo.rcMonitor.right - rect.TopLeft().x;
	
	maxSize = min(maxSizeClipped, maxSize);
#else
	maxSize = min(ClientDC.GetDeviceCaps(HORZRES) - rect.TopLeft().x, maxSize);
#endif

	return maxSize;
}

/*!
	Test if the Del key is pressed and remove the item if the drop-down list is dropped down
	\param pMsg Message to check
	\return TRUE if the drop-down list was dropped and Del key was pressed, FALSE otherwise
	\attention Make sure the items collection is set earlier (call USmartCombo::SetItems to set the collection).
*/
BOOL CHistComboBoxEx::DelKeyTest(MSG* pMsg)
{
	CMString* pstrItems = GetItems();
	ASSERT(pstrItems != NULL);

	BOOL bRes = FALSE;

	if( WM_KEYDOWN == pMsg->message && 
		VK_DELETE == (int)pMsg->wParam && 
		pstrItems && 
		GetDroppedState() )
	{
		const int sel = GetCurSel();
		if( sel > CB_ERR && GetCount() > 1 )
		{
			CString strItem;
			GetLBText(sel, strItem);

			if( RemoveItem(strItem) )
			{
				DeleteString(sel);
				
				if( GetCount() )
				{
					const int nNewSel = (sel == 0 ? sel : (GetCount() ? 0 : -1));
					if( nNewSel != 0 || sel == 0 )
					{
						SetCurSel(nNewSel);
						
						// Trigger the location change for all views
						CWincvsApp* app = (CWincvsApp*)AfxGetApp();
						CMainFrame* mainFrm = app->GetMainFrame();
						mainFrm->ChangeLocation();
					}
				}
			}
		}
		else if( GetCount() == 1 )
		{
			CvsAlert(kCvsAlertStopIcon, 
				"Can't remove the last item from the history.", "Please add a new location first before removing the current one.", 
				BUTTONTITLE_OK, NULL).ShowAlert();
		}

		// We have to prevent it going to another window
		bRes = TRUE;
	}

	return bRes;
}

/// WindowProc virtual override, intercept messages to allow GetLBText to work
LRESULT CHistComboBoxEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( CB_GETLBTEXTLEN == message )
	{
		const int index = (int)wParam;
		if( index < GetCount() )
		{
			char szBuffer[MAX_PATH] = "";

			COMBOBOXEXITEM cbi = { 0 };
			cbi.mask = CBEIF_TEXT;
			cbi.iItem = index;
			cbi.pszText = szBuffer;
			cbi.cchTextMax = MAX_PATH - 1;
			
			if( GetItem(&cbi) )
			{
				return strlen(szBuffer);
			}
		}
	}
	else if( CB_GETLBTEXT == message )
	{
		const int index = (int)wParam;
		if( index < GetCount() )
		{
			LPSTR szBuffer = (LPSTR)lParam;

			COMBOBOXEXITEM cbi = { 0 };
			cbi.mask = CBEIF_TEXT;
			cbi.iItem = index;
			cbi.pszText = szBuffer;
			cbi.cchTextMax = MAX_PATH - 1;
			
			if( GetItem(&cbi) )
			{
				return strlen(szBuffer);
			}
		}
	}
	
	return CComboBoxEx::WindowProc(message, wParam, lParam);
}
