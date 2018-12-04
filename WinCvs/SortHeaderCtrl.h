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

#ifndef __SORTHEADERCTRL_H__
#define __SORTHEADERCTRL_H__

#include "BrowseViewModel.h"

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl

/// Sort header control
class CSortHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CSortHeaderCtrl)

// Construction
public:
	CSortHeaderCtrl();

// Attributes
protected:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	public:
	protected:
	//}}AFX_VIRTUAL

	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	
// Implementation
public:
	virtual ~CSortHeaderCtrl();

// Generated message map functions
protected:
	//{{AFX_MSG(CSortHeaderCtrl)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	
	afx_msg void OnFilterChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFilterBtnClicked(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	int m_nSortCol;				/*!< Sort column */
	BOOL m_bSortAsc;			/*!< Sorting order */
	BOOL m_bHeaderFilters;		/*!< Header filters flag */

	NotificationManager m_notificationManager;	/*!< Notification manager */

	int m_nLastFilterChanged;				/*!< Last filter changed notification */
	CString m_strLastInsertedFilterText;	/*!< Last inserted filter text */

	// Methods
	void DrawItem(const UINT itemID, const RECT rcItem, 
		const BOOL bSelected, const BOOL bSortCol, const BOOL bSortAsc, 
		const HDC hDC);
	
	void ResetLastFilter();

public:
	// Interface
	int SetSortImage(const int nCol, const BOOL bAsc);

	BOOL EnableHeaderFilters(const BOOL enable = TRUE);
	NotificationManager* GetNotificationManager();

	CString GetFilterText(const int intItem);
	BOOL SetFilterText(const int intItem, const char* text);
	void SetFilterChangeTimeout(const int nFilterChangeTimeout);
};

#endif
// __SORTHEADERCTRL_H__
