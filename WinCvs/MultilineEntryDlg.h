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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2003
 */

/*
 * MultilineEntryDlg.h : Multiline entry complex
 */

#if !defined(AFX_MULTILINEENTRYDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_MULTILINEENTRYDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;
class CCvsrootEntryDlg;

extern CMString gRevNames;

/// Multiline entry type enum
typedef enum {
	kMultilineLogMessage = 0,	/*!< Log message */
	kMultilineCmdLine,			/*!< Command line */
	kMultilineState,			/*!< State */
	kMultilineDescription,		/*!< Description */
}kMultilineEntryType;

/// Action enum
typedef enum {
	kActionOpenEditor = 0,	/*!< Open external editor */
	kActionOpenTemplate,	/*!< Open CVS template */
	kActionBrowseTag,		/*!< Browse repository for tag */
	kActionBrowseModule,	/*!< Browse repository for module */
	kActionFormatCvsRoot	/*!< Format CVSROOT */
}kActionType;

/// Helper class to process dialog messages for dialogs containing multi-line edit control
class CMultiLineEditDialogKeyHelper
{
public:
	// Construction
	CMultiLineEditDialogKeyHelper();
	
private:
	// Data members
	CDialog* m_pDlg;	/*!< Dialog */
	HWND m_hParentDlg;	/*!< Dialog's parent handle */
	
	BOOL m_ctrlPressedOnOK;	/*!< Ctrl key flag: TRUE to confirm the dialog when Ctrl key is pressed, FALSE otherwise */
	
public:
	// Interface
	void Create(const int ctrlPressedOnOK, CDialog* pDlg, const HWND hParentDlg = NULL);
	BOOL RelayEvent(const MSG* pMsg);
	
	const char* FormatLineBreakHint() const;
};

/// Multiline entry controls complex
class CMultilineEntryDlg : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CMultilineEntryDlg)

// Construction
public:
	CMultilineEntryDlg(const CCvsrootEntryDlg* cvsrootEntryDlg, const MultiFiles* mf);
	~CMultilineEntryDlg();

// Dialog Data
	//{{AFX_DATA(CMultilineEntryDlg)
	enum { IDD = IDD_MULTILINE_ENTRY };
	CEdit	m_editPositionEdit;
	CSmartComboBox	m_actionCombo;
	CButton	m_entryStatic;
	CButton	m_wordWrapCtrl;
	CEdit	m_entryTextCtrlWrap;
	CEdit	m_entryTextCtrl;
	CSmartComboBox	m_prevLogsCombo;
	CString	m_entryText;
	BOOL	m_wordWrap;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMultilineEntryDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMultilineEntryDlg)
	afx_msg void OnSelchangeCombo();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWordwrap();
	virtual BOOL OnInitDialog();
	afx_msg void OnActionGo();
	afx_msg void OnSetfocusLogmsg();
	afx_msg void OnSetfocusLogmsgWrap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const CCvsrootEntryDlg* m_cvsrootEntryDlg;	/*!< CVSROOT entry */

	const MultiFiles* m_mf;						/*!< Browser selection */
	kMultilineEntryType m_multilineEntryType;	/*!< Entry type */

	CDialog* m_pParentWnd;			/*!< Parent dialog */
	CMString* m_pPreviousEntries;	/*!< Previous entries */
	
	bool m_switchingWrap;			/*!< Switching the wrap control flag */
	bool m_firstFocus;				/*!< Flag to mark the first time edit control is focused */

	CMultiLineEditDialogKeyHelper m_multiLineEditDialogKeyHelper;
	
	// Methods
	virtual void InitHelpID();
	
	void SwitchWrapEdit(const BOOL bWordWrap);
	void SetEntryDescription();
	void SetActions();

	void AddAction(const kActionType actionType, const char* actionDescription, const bool select = false);
	void UpdateInfo();

	int GetEntryTextID();
	CWnd* GetEntryTextWnd();

	void ActionOpenEditor();
	void ActionOpenTemplate();
	void ActionBrowseTag();
	void ActionBrowseModule();
	void ActionFormatCvsRoot();

public:
	// Interface
	BOOL Create(const kMultilineEntryType multilineEntryType, CMString* pPreviousEntries, 
		const CWnd* pWndInsertAfter, CDialog* pParentWnd, const bool updateData = true);

	BOOL UpdateData(BOOL bSaveAndValidate, CString& entryText);
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MULTILINEENTRYDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
