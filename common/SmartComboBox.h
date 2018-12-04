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

#if !defined(AFX_SMARTCOMBOBOX_H__E3F8DAE6_C2EF_49CE_AE52_3AE54F5A629C__INCLUDED_)
#define AFX_SMARTCOMBOBOX_H__E3F8DAE6_C2EF_49CE_AE52_3AE54F5A629C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SmartComboBox.h : header file
//

#include "ustr.h"

// Forward declaration
class CMString;

/*!
	Abstract class for Smart Combo Box (CSmartComboBox)
	
	Use as a base class to derive the Smart Combo Box for different platforms
	\note To set the desired features support by Smart Combo Box objects 
	      use the combinations of values enumerated under USmartCombo::Feature
*/
class USmartCombo
{
public:
	// Construction
	USmartCombo(int feature);
	virtual ~USmartCombo();

private:
	int m_feature;			/*!< Pattern of used features */
	CMString* m_pstrItems;	/*!< Items collection */
	std::string m_keyword;	/*!< Keyword to be used with Feature::Keyword */

public:
	//! Feature enum
	/*! Enum to specify the desired features for Smart Combo Box object */
	enum Feature{
		None =			0x00,	/*!< No features, just standard Combo Box. */
		AutoDropWidth =	0x01,	/*!< Automatically adjust the width of the drop-down list to fit the long items. */
		RemoveItems =	0x02,	/*!< Allows to remove items from combo boxes containing previous values. */
		ReadOnly =		0x04,	/*!< Make the edit portion of combo box read-only (while still allowing to change the selection it prevents from typing those and copy it's values). */
		Keyword =		0x10,	/*!< Indicates that combo box items are keywords or multi-keywords */
		//
		DefaultFeature = AutoDropWidth | RemoveItems  /*!< Default combination of features passed to the Smart Combo Box constructor. */
	};

public:
	// Get / Set methods
	void SetFeature(int feature);
	bool HasFeature(int feature) const;

	void SetItems(CMString* pstrItems);
	CMString* GetItems();

	void SetKeyword(const char* keyword);
	std::string GetKeyword() const;

	// Overrides
	virtual int CalcDroppedWidth();
	virtual void SetReadOnly(bool readOnly = true);
	virtual bool RemoveItem(const char* strItem);
};

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox window

/// Private window messages enum
enum{
	WM_SMCB_MIN = WM_APP + 1,	/*!< Start marker */
	WM_SMCB_SETFEATURE,			/*!< Set the features, see CSmartComboBox::OnSetFeature for more details */
	WM_SMCB_SETITEMS,			/*!< Set the drop-down list items collection, see CSmartComboBox::OnSetItems for more details */
	WM_SMCB_SETREADONLY,		/*!< Set the read-only feature, see CSmartComboBox::OnSetFeature for more details */
	WM_SMCB_MAX					/*!< End marker */
};


/*!
	Smart Combo Box class(CSmartComboBox)

	Use that class as a replacement for CComboBox. Set the desired features 
	using the constructor or CSmartComboBox::SetFeature method. To use the Del
	key feature when the drop-down list is dropped down you need to also call CSmartComboBox::SetItems
	method to setup appropriate persistent collection (CMString object). 

	The CSmartComboBox::SetReadOnlymethod can be used to make an edit portion of the combo
	readonly or to reverse that feature. For use with the UWidget the messages to set appropriate 
	features and data members are provided (#WM_SMCB_SETFEATURE, #WM_SMCB_SETITEMS and #WM_SMCB_SETREADONLY).
*/
class CSmartComboBox : public CComboBox, public USmartCombo
{
// Construction
public:
	CSmartComboBox(int feature = USmartCombo::DefaultFeature);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmartComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSmartComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSmartComboBox)
	afx_msg void OnDropdown();
	//}}AFX_MSG
	afx_msg LRESULT OnSelectString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetFeature(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetItems(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetReadOnly(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool m_selectExact;	/*!< true to attempt to select the exact string when CB_SELECTSTRING message is received */

	// Methods
	virtual int CalcDroppedWidth();
	BOOL DelKeyTest(MSG* pMsg);

public:
	// Interface
	virtual void SetReadOnly(bool readOnly = true);
	virtual void SetSelectExact(bool selectExact = true);

	inline virtual CEdit* GetEditCtrl() const;
	inline virtual int EnsureSelchangeMatch();

	inline virtual CString GetCurSelText() const;
	
	inline virtual CString GetWindowText() const;
	inline void GetWindowText(CString& rString) const;
};

//////////////////////////////////////////////////////////////////////////
// Inline implementation

/*!
	Get the edit control portion of the combo box
	\return The edit control portion of the combo box
*/
inline CEdit* CSmartComboBox::GetEditCtrl() const
{
	return (CEdit*)GetDlgItem(1001);
}

/*!
	Ensure that the edit control text retrieved with GetWindowText API matches the currently selected item
	\return The current selection or -1 if none
	\warning It should be only called from within CBN_SELCHANGE handler
	\note For more details see kb66365: http://support.microsoft.com/default.aspx?scid=kb;en-us;66365
*/
inline int CSmartComboBox::EnsureSelchangeMatch()
{
	const int item = GetCurSel();
	
	if( item > -1 )
	{
		CString path;
		GetLBText(item, path);
		SetWindowText(path);
	}

	return item;
}

/*!
	Get currently selected text
	\return Currently selected text
*/
inline CString CSmartComboBox::GetCurSelText() const
{
	CString strRes;

	const int item = GetCurSel();
	if( item > -1 )
	{
		GetLBText(item, strRes);
	}

	return strRes;
}

/*!
	Get window text
	\return Window text
*/
inline CString CSmartComboBox::GetWindowText() const
{
	CString strRes;
	GetWindowText(strRes);

	return strRes;
}

/*!
	Get window text
	\return Window text
	\note Just forwarding to the base class to allow standard operations working
*/
inline void CSmartComboBox::GetWindowText(CString& rString) const
{
	CComboBox::GetWindowText(rString);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_SMARTCOMBOBOX_H__E3F8DAE6_C2EF_49CE_AE52_3AE54F5A629C__INCLUDED_)
