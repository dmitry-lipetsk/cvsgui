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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- December 2001
 */

// SmartComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartComboBox.h"

#ifdef WIN32
#	include "resource.h"
#endif /* WIN32 */

#include "MultiString.h"
#include "CvsAlert.h"

#ifdef WIN32

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/// Minimum drop-down height
#define MIN_DROPDOWN_HEIGHT	211

#endif /* WIN32 */

/// Constructor
USmartCombo::USmartCombo(int feature)
	: m_feature(feature)
{
	m_pstrItems = NULL;
}

/// Destructor
USmartCombo::~USmartCombo()
{
}

/*!
	Set the features
	\param feature Feature pattern to be set
	\note Reflects the read-only state appropriate
*/
void USmartCombo::SetFeature(int feature)
{
	m_feature = feature;
	SetReadOnly(0 != (m_feature & USmartCombo::ReadOnly));
}

/*!
	Test whether the feature is supported
	\param feature Feature pattern to be tested
	\return true if the feature is set, false otherwise
*/
bool USmartCombo::HasFeature(int feature) const
{
	return (m_feature & feature) != 0;
}

/*!
	Set the items persistent collection
	\param pstrItems pointer to CMString class object so that it can be removed with Del key when the drop-down list is dropped down
	\return true on success, false otherwise
	\note It will also set the feature member appropriate: 
	      - if pstrItems is NULL it will remove 
	      - if it's valid then it will add the USmartCombo::RemoveItems feature.
*/
void USmartCombo::SetItems(CMString* pstrItems)
{
	m_pstrItems = pstrItems;
	if( m_pstrItems )
	{
		m_feature |= USmartCombo::RemoveItems;
	}
	else
	{
		m_feature &= ~USmartCombo::RemoveItems;
	}
}

/*!
	Get the items persistent collection
	\return pointer to CMString that contains the items persistent collection, can be NULL 
	\note Used to populate combo box in DDX routine etc.
*/
CMString* USmartCombo::GetItems()
{
	return m_pstrItems;
}

/*!
	Set the keyword
	\param keyword Keyword
	\note It will also set the feature member appropriate
*/
void USmartCombo::SetKeyword(const char* keyword)
{
	m_keyword = keyword;
	m_feature |= USmartCombo::Keyword;
}

/*!
	Get the keyword
	\return The keyword string
*/
std::string USmartCombo::GetKeyword() const
{
	return m_keyword;
}

/*!
	Calculate the width of the dropped items list to fit all items
	\return The width of the dropped items list
	\note The default implementation does nothing, you should override that method to perform the real calculation
*/
int USmartCombo::CalcDroppedWidth()
{
	return 0;
}

/*!
	Set the read-only feature
	\param readOnly true to turn on the read-only feature, false to turn if off
	\note Call the base class first when you override USmartCombo::SetReadOnly
*/
void USmartCombo::SetReadOnly(bool readOnly /*= true*/)
{
	if( readOnly )
	{
		m_feature |= USmartCombo::ReadOnly;
	}
	else
	{
		m_feature &= ~USmartCombo::ReadOnly;
	}
}

/*!
	Remove the item from the list
	\param strItem Item to be removed
	\return true if item removed, false otherwise
	\note Prompts the user before removing the item
*/
bool USmartCombo::RemoveItem(const char* strItem)
{
	bool res = false;

	string item("Item: ");
	item += strItem;
	
	CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
		_i18n("Do you want to remove item from the list?"), item.c_str(), 
		BUTTONTITLE_YES, BUTTONTITLE_NO);

	if( cvsAlert.ShowAlert() == kCvsAlertOKButton )
	{
		if( HasFeature(Keyword) )
		{
			if( CMKeyString* pMKeyString = dynamic_cast<CMKeyString*>(GetItems()) )
			{
				string mapValue;
				pMKeyString->Concatenate(mapValue, GetKeyword().c_str(), strItem);
				
				res = m_pstrItems->Remove(mapValue.c_str());
			}
			else if( CKeyString* pKeyString = dynamic_cast<CKeyString*>(GetItems()) )
			{
				vector<string> keyList;
				
				const CMString::list_t& list = pKeyString->GetList();
				if( !list.empty() )
				{
					for(CMString::list_t::const_iterator i = list.begin(); i != list.end(); ++i)
					{
						string key;
						string value;
						
						pKeyString->Split(i->c_str(), key, value);
						if( key.compare(GetKeyword()) == 0 )
						{
							keyList.push_back(key);
						}
					}
				}
				
				for(vector<string>::const_iterator i = keyList.begin(); i != keyList.end() ; ++i)
				{
					string mapValue;
					pKeyString->Concatenate(mapValue, i->c_str(), strItem);
					res = m_pstrItems->Remove(mapValue.c_str());
				}
			}
		}
		else
		{
			res = m_pstrItems->Remove(strItem);
		}
	}

	return res;
}

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox

CSmartComboBox::CSmartComboBox(int feature /*= USmartCombo::DefaultFeature*/)
	: USmartCombo(feature), m_selectExact(true)
{
}

CSmartComboBox::~CSmartComboBox()
{
}


BEGIN_MESSAGE_MAP(CSmartComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSmartComboBox)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CB_SELECTSTRING, OnSelectString)

	ON_MESSAGE(WM_SMCB_SETFEATURE, OnSetFeature)
	ON_MESSAGE(WM_SMCB_SETITEMS, OnSetItems)
	ON_MESSAGE(WM_SMCB_SETREADONLY, OnSetReadOnly)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox message handlers

/// CBN_DROPDOWN message handler, calculates the width of the drop-down list if the USmartCombo::AutoDropWidth feature is set
void CSmartComboBox::OnDropdown() 
{
	if( HasFeature(USmartCombo::AutoDropWidth) )
	{
		SetDroppedWidth(CalcDroppedWidth());
	}
}

/// CB_SELECTSTRING message handler, intercepted to work around problems with DDX data exchange
LRESULT CSmartComboBox::OnSelectString(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = CB_ERR;

	LRESULT findRes = SendMessage(m_selectExact ? CB_FINDSTRINGEXACT : CB_FINDSTRING, wParam, lParam);
	if( findRes != CB_ERR )
	{
		// Check whether there is a better match
		if( m_selectExact )
		{
			CString strLBText;
			for(int nIndex = 0; nIndex < GetCount(); nIndex++)
			{
				GetLBText(nIndex, strLBText);
				if( strLBText.Compare((LPCSTR)lParam) == 0 )
				{
					// Perfect match found
					findRes = nIndex;
					break;
				}
			}
		}

		result = SendMessage(CB_SETCURSEL, (WPARAM)findRes, 0);
	}

	return result;
}

/// PreTranslateMessage virtual override, if the USmartCombo::RemoveItems feature is set perform the key test to determine whether to delete item
BOOL CSmartComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if( HasFeature(USmartCombo::RemoveItems) )
	{
		if( DelKeyTest(pMsg) )
		{
			return TRUE;
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

/// CalcDroppedWidth virtual override, calculate drop-down list width and make sure it doesn't go off-screen as well
int CSmartComboBox::CalcDroppedWidth()
{
	int maxSize = 0;

	CClientDC ClientDC(this);
	int savedDC = ClientDC.SaveDC();
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

	ClientDC.RestoreDC(savedDC);

	maxSize += ::GetSystemMetrics(SM_CXVSCROLL)	+ 3*::GetSystemMetrics(SM_CXEDGE);
	
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
	\param pMsg Message to test
	\return TRUE if the drop-down list was dropped and Del key was pressed
	\note Make sure the items collection is set earlier (call USmartCombo::SetItems to set the collection)
*/
BOOL CSmartComboBox::DelKeyTest(MSG* pMsg)
{
	CMString* pstrItems = GetItems();
	ASSERT(pstrItems != NULL);

	BOOL bRes = FALSE;

	if( WM_KEYDOWN == pMsg->message && 
		VK_DELETE == (int)pMsg->wParam && 
		pstrItems && 
		GetDroppedState() )
	{
		int sel = GetCurSel();
		if( CB_ERR < sel )
		{
			CString strItem;
			GetLBText(sel, strItem);

			if( RemoveItem(strItem) )
			{
				DeleteString(sel);
			}
			
			//we have to prevent it going to another window
			bRes = TRUE;
		}
	}

	return bRes;
}

/// PreSubclassWindow virtual override, sets the read-only state if the USmartCombo::ReadOnly feature is set
void CSmartComboBox::PreSubclassWindow() 
{
	if( HasFeature(USmartCombo::ReadOnly) )
	{
		SetReadOnly();
	}
	
	CRect windowRect;
	GetWindowRect(windowRect);

	if( windowRect.Height() < MIN_DROPDOWN_HEIGHT )
	{
		SetWindowPos(NULL, 0, 0, windowRect.Width(), MIN_DROPDOWN_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
	}

	CComboBox::PreSubclassWindow();
}

/*!
	Set the read-only state (edit part of combo box)
	\param readOnly true to set the read-only state, false otherwise
	\note It can be used at runtime, but it's better to set read-only using constructor (CSmartComboBox::CSmartComboBox)
*/
void CSmartComboBox::SetReadOnly(bool readOnly /*= true*/)
{
	USmartCombo::SetReadOnly(readOnly);

	if( IsWindow(m_hWnd) )
	{
		CEdit* pEdit = (CEdit*)GetDlgItem(1001);
		if( pEdit && IsWindow(pEdit->m_hWnd) )
		{
			pEdit->SetReadOnly(readOnly);
		}
	}
}

/*!
	Set the exact selection flag
	\param selectExact true to set the exact selection on, false otherwise
*/
void CSmartComboBox::SetSelectExact(bool selectExact /*= true*/)
{
	m_selectExact = selectExact;
}

/*!
	WM_SMCB_SETFEATURE private message handler
	\param wParam Value or a combinations of values enumerated under USmartCombo::Feature
	\param lParam Unused
	\return 0
*/
LRESULT CSmartComboBox::OnSetFeature(WPARAM wParam, LPARAM lParam)
{
	SetFeature((int)wParam);
	return 0;
}

/*!
	WM_SMCB_SETITEMS private message handler
	\param wParam Pointer to persistent collection of items
	\param lParam Unused
	\return 0
*/
LRESULT CSmartComboBox::OnSetItems(WPARAM wParam, LPARAM lParam)
{
	SetItems((CMString*)wParam);
	return 0;
}

/*!
	WM_SMCB_SETREADONLY private message handler
	\param wParam Boolean value to set the read-only state
	\param lParam Unused
	\return 0
*/
LRESULT CSmartComboBox::OnSetReadOnly(WPARAM wParam, LPARAM lParam)
{
	SetReadOnly(0 != wParam);
	return 0;
}

#endif /* WIN32 */
