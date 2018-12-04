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
 * CommitDlg.cpp : Multiline entry complex
 */

#include "stdafx.h"
#include "wincvs.h"
#include <process.h>

#include "MultilineEntryDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "getline.h"
#include "MultiString.h"
#include "CvsCommands.h"
#include "EditorMonDlg.h"
#include "ItemListDlg.h"
#include "BrowseViewHandlers.h"
#include "GetPrefs.h"
#include "TemplateDlg.h"
#include "LineCmd.h"
#include "CvsRootDlg.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ENTRY_LOGMESSAGE_DESC_FORMAT	"Enter the log &message (%s):"
#define ENTRY_CMDLINE_DESC_FORMAT		"Enter the &command line (%s):"
#define ENTRY_STATE_DESC_FORMAT			"Enter the &state (%s):"
#define ENTRY_DESCRIPTION_DESC_FORMAT	"Enter the &description (%s):"

//////////////////////////////////////////////////////////////////////////
// CMultiLineEditDialogKeyHelper

CMultiLineEditDialogKeyHelper::CMultiLineEditDialogKeyHelper()
	: m_pDlg(NULL), m_hParentDlg(NULL), m_ctrlPressedOnOK(FALSE)
{
}

/*!
	Set the dialog to process the messages for
	\param ctrlPressedOnOK Ctrl key flag
	\param pDlg Dialog
	\param hParentDlg Dialog's parent handle
*/
void CMultiLineEditDialogKeyHelper::Create(const int ctrlPressedOnOK, CDialog* pDlg, const HWND hParentDlg /*= NULL*/)
{
	m_ctrlPressedOnOK = ctrlPressedOnOK;

	m_pDlg = pDlg;
	m_hParentDlg = hParentDlg;
}

/*!
	Process the message
	\param pMsg Message
	\return TRUE if the message was processed, FALSE otherwise
*/
BOOL CMultiLineEditDialogKeyHelper::RelayEvent(const MSG* pMsg)
{
	BOOL res = FALSE;

	if( pMsg->message == WM_KEYDOWN )
	{
		switch( (int)pMsg->wParam )
		{
		case VK_ESCAPE:
			{
				if( m_pDlg )
				{
					m_pDlg->EndDialog(IDCANCEL);
					res = TRUE;
				}

				if( m_hParentDlg )
				{
					CDialog* pDialog = (CDialog*)CWnd::FromHandle(m_hParentDlg);
					if( pDialog )
					{
						pDialog->EndDialog(IDCANCEL);
						res = TRUE;
					}
				}

			}
			break;
		case VK_RETURN:
			{
				const BOOL ctrlKeyPressed = ::GetKeyState(VK_CONTROL) < 0;

				if( !m_ctrlPressedOnOK && !ctrlKeyPressed || 
					m_ctrlPressedOnOK && ctrlKeyPressed )
				{
					if( m_pDlg )
					{
						res = TRUE;

						if( m_pDlg->UpdateData() )
						{
							m_pDlg->EndDialog(IDOK);
						}
						else
						{
							break;
						}
					}

					if( m_hParentDlg )
					{
						CDialog* pDialog = (CDialog*)CWnd::FromHandle(m_hParentDlg);
						if( pDialog )
						{
							if( pDialog->UpdateData() )
							{
								pDialog->EndDialog(IDOK);
							}
							
							res = TRUE;
						}
					}
				}
			}
			break;
		default:
			// Nothing to do
			break;
		}
	}

	return res;
}

/*!
	Format line break hint message
	\return Formatted line break hint
*/
const char* CMultiLineEditDialogKeyHelper::FormatLineBreakHint() const
{
	return m_ctrlPressedOnOK ? "Enter for line break, Ctrl+Enter to confirm dialog" : "Ctrl+Enter for line break, Enter to confirm dialog";
}

//////////////////////////////////////////////////////////////////////////
// CMultilineEntryDlg

IMPLEMENT_DYNAMIC(CMultilineEntryDlg, CHHelpPropertyPage)

CMultilineEntryDlg::CMultilineEntryDlg(const CCvsrootEntryDlg* cvsrootEntryDlg, const MultiFiles* mf) 
	: m_cvsrootEntryDlg(cvsrootEntryDlg),
	m_mf(mf), 
	m_actionCombo(CSmartComboBox::AutoDropWidth), 
	CHHelpPropertyPage(CMultilineEntryDlg::IDD)
{
	//{{AFX_DATA_INIT(CMultilineEntryDlg)
	m_entryText = _T("");
	m_wordWrap = FALSE;
	//}}AFX_DATA_INIT
	
	m_switchingWrap = false;
	m_firstFocus = true;
	m_wordWrap = gCvsPrefs.WordWrapLogMsg();
}

CMultilineEntryDlg::~CMultilineEntryDlg()
{
}

void CMultilineEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultilineEntryDlg)
	DDX_Control(pDX, IDC_MULTILINE_POSITION, m_editPositionEdit);
	DDX_Control(pDX, IDC_MULTILINE_ACTION, m_actionCombo);
	DDX_Control(pDX, IDC_MULTILINE_TYPE, m_entryStatic);
	DDX_Control(pDX, IDC_MULTILINE_WORDWRAP, m_wordWrapCtrl);
	DDX_Control(pDX, IDC_MULTILINE_ENTRY_WRAP, m_entryTextCtrlWrap);
	DDX_Control(pDX, IDC_MULTILINE_ENTRY, m_entryTextCtrl);
	DDX_Control(pDX, IDC_MULTILINE_PREV_ENTRY, m_prevLogsCombo);
	DDX_Check(pDX, IDC_MULTILINE_WORDWRAP, m_wordWrap);
	//}}AFX_DATA_MAP

	if( !pDX->m_bSaveAndValidate )
	{
		DDX_Text(pDX, IDC_MULTILINE_ENTRY_WRAP, m_entryText);
		DDX_Text(pDX, IDC_MULTILINE_ENTRY, m_entryText);
		DDX_ComboMString(pDX, IDC_MULTILINE_PREV_ENTRY, m_prevLogsCombo);
		m_prevLogsCombo.SelectString(-1, m_entryText);
	}
	else
	{
		DDX_Text(pDX, GetEntryTextID(), m_entryText);
		if( !m_entryText.IsEmpty() )
		{
			m_pPreviousEntries->Insert(m_entryText);
		}
	}
}

BEGIN_MESSAGE_MAP(CMultilineEntryDlg, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CMultilineEntryDlg)
	ON_CBN_SELCHANGE(IDC_MULTILINE_PREV_ENTRY, OnSelchangeCombo)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_MULTILINE_WORDWRAP, OnWordwrap)
	ON_BN_CLICKED(IDC_MULTILINE_GO_ACTION, OnActionGo)
	ON_EN_SETFOCUS(IDC_MULTILINE_ENTRY, OnSetfocusLogmsg)
	ON_EN_SETFOCUS(IDC_MULTILINE_ENTRY_WRAP, OnSetfocusLogmsgWrap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// Message handlers

/*!
	Create the entry dialog
	\param multilineEntryType Entry type
	\param pPreviousEntries Previous entries collection
	\param pWndInsertAfter Placeholder window
	\param pParentWnd Parent window
	\param updateData true to invoke UpdateData(FALSE) for the parent dialog after the creation to display the initial text entry, false otherwise
	\return TRUE on success, FALSE otherwise
*/
BOOL CMultilineEntryDlg::Create(const kMultilineEntryType multilineEntryType, CMString* pPreviousEntries, 
								const CWnd* pWndInsertAfter, CDialog* pParentWnd, const bool updateData /*= true*/)
{
	BOOL res = FALSE;

	ASSERT(pWndInsertAfter != NULL);	// Invalid pointer
	ASSERT(pParentWnd != NULL);			// Invalid pointer
	ASSERT(pPreviousEntries != NULL);	// Invalid pointer

	m_multilineEntryType = multilineEntryType;
	m_pParentWnd = pParentWnd;
	m_pPreviousEntries = pPreviousEntries;
	
	m_prevLogsCombo.SetItems(m_pPreviousEntries);

	if( CHHelpPropertyPage::Create(CMultilineEntryDlg::IDD, m_pParentWnd) )
	{
		CRect windowRect;
		pWndInsertAfter->GetWindowRect(windowRect);
		pParentWnd->ScreenToClient(windowRect);

		SetWindowPos(pWndInsertAfter, windowRect.left, windowRect.top, windowRect.Width(), windowRect.Height(), SWP_SHOWWINDOW);

		if( updateData )
		{
			m_pParentWnd->UpdateData(FALSE);
		}

		res = TRUE;
	}

	return res;
}

/// OnInitDialog virtual override, initialize controls appropriate to the entry type
BOOL CMultilineEntryDlg::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_multiLineEditDialogKeyHelper.Create(gCvsPrefs.CtrlPressedOnOK(), m_pParentWnd, m_pParentWnd->GetParent()->GetSafeHwnd());
	
	SwitchWrapEdit(gCvsPrefs.WordWrapLogMsg());
	SetEntryDescription();
	SetActions();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMultilineEntryDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_MULTILINE_TYPE,		helpTopicIgnore);
	SetCtrlHelpID(IDC_MULTILINE_ENTRY,		IDH_IDC_MULTILINE_ENTRY);
	SetCtrlHelpID(IDC_MULTILINE_ENTRY_WRAP,	IDH_IDC_MULTILINE_ENTRY_WRAP);
	SetCtrlHelpID(IDC_MULTILINE_PREV_ENTRY,	IDH_IDC_MULTILINE_PREV_ENTRY);
	SetCtrlHelpID(IDC_MULTILINE_ACTION,		IDH_IDC_MULTILINE_ACTION);
	SetCtrlHelpID(IDC_MULTILINE_GO_ACTION,	IDH_IDC_MULTILINE_GO_ACTION);
	SetCtrlHelpID(IDC_MULTILINE_WORDWRAP,	IDH_IDC_MULTILINE_WORDWRAP);
	SetCtrlHelpID(IDC_MULTILINE_POSITION,	IDH_IDC_MULTILINE_POSITION);
}

/*!
	Invoke DDX for multiline entry
	\param bSaveAndValidate Validation flag
	\param entryText Return the entered text
	\return TRUE on success, FALSE otherwise
*/
BOOL CMultilineEntryDlg::UpdateData(BOOL bSaveAndValidate, CString& entryText)
{
	BOOL res = FALSE;

	if( bSaveAndValidate )
	{
		res = ::IsWindow(GetSafeHwnd()) ? CHHelpPropertyPage::UpdateData(TRUE) : TRUE;

		entryText = m_entryText;
	}
	else
	{
		m_entryText = entryText;

		res = ::IsWindow(GetSafeHwnd()) ? CHHelpPropertyPage::UpdateData(FALSE) : TRUE;
	}

	return res;
}

/// CBN_SELCHANGE message handler, copy the selected text as the log message
void CMultilineEntryDlg::OnSelchangeCombo() 
{
	int sel = m_prevLogsCombo.GetCurSel();
	if( sel == CB_ERR )
		return;

	CEdit* pEdit = (CEdit*)GetEntryTextWnd();
	pEdit->SetSel(0, -1);

	const CMString::list_t& list = m_pPreviousEntries->GetList();
	pEdit->ReplaceSel(list[sel].c_str());
}

/// PreTranslateMessage virtual override, update text entry info and relay message for the multi-line processing
BOOL CMultilineEntryDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message >= WM_KEYFIRST ||
		pMsg->message <= WM_KEYLAST ||
		pMsg->message == WM_COPY || pMsg->message == WM_CUT || pMsg->message == WM_PASTE )
	{
		UpdateInfo();
	}

	if( m_multiLineEditDialogKeyHelper.RelayEvent(pMsg) )
	{
		return TRUE;
	}
	
	return CHHelpPropertyPage::PreTranslateMessage(pMsg);
}

/*!
	Add the action to the action combo box
	\param actionType Action to add
	\param actionDescription Action description
	\param select true to set the new action item selected, false otherwise
*/
void CMultilineEntryDlg::AddAction(const kActionType actionType, const char* actionDescription, const bool select /*= false*/)
{
	const int item = m_actionCombo.AddString(actionDescription);
	if( item > -1 )
	{
		m_actionCombo.SetItemData(item, (DWORD)actionType);

		if( select )
		{
			m_actionCombo.SetCurSel(item);
		}
	}
}

/*!
	Display entry text info
*/
void CMultilineEntryDlg::UpdateInfo()
{
	CEdit* pEdit = (CEdit*)GetEntryTextWnd();
	
	const int nLineNumber = pEdit->LineFromChar();
	const int nLineLenght = pEdit->LineLength(pEdit->LineIndex());
	
	CString strInfo;
	strInfo.Format("Line %d (%d chars)", nLineNumber + 1, nLineLenght);
	
	CString strOldInfo;
	m_editPositionEdit.GetWindowText(strOldInfo);

	if( strOldInfo != strInfo )
	{
		m_editPositionEdit.SetWindowText(strInfo);
	}
}

/*!
	Switch the edit wrap and non-wrap edit boxes
	\param bWordWrap TRUE to enable word wrap, FALSE otherwise
*/
void CMultilineEntryDlg::SwitchWrapEdit(const BOOL bWordWrap)
{
	CString strMessage;

	if( bWordWrap )
	{
		m_entryTextCtrl.GetWindowText(strMessage);
		m_entryTextCtrlWrap.SetWindowText(strMessage);

		m_switchingWrap = true;
		m_entryTextCtrl.ShowWindow(SW_HIDE);
		m_entryTextCtrlWrap.ShowWindow(SW_SHOW);
		m_switchingWrap = false;
	}
	else
	{
		m_entryTextCtrlWrap.GetWindowText(strMessage);
		m_entryTextCtrl.SetWindowText(strMessage);

		m_switchingWrap = true;
		m_entryTextCtrlWrap.ShowWindow(SW_HIDE);
		m_entryTextCtrl.ShowWindow(SW_SHOW);
		m_switchingWrap = false;
	}
}

/*!
	Set the entry description depending on entry type
*/
void CMultilineEntryDlg::SetEntryDescription()
{
	CString entryDescription;

	switch( m_multilineEntryType )
	{
	default:
		ASSERT(FALSE); // Unknown type
	case kMultilineLogMessage:
		entryDescription.Format(ENTRY_LOGMESSAGE_DESC_FORMAT, m_multiLineEditDialogKeyHelper.FormatLineBreakHint());
		break;
	case kMultilineCmdLine:
		entryDescription.Format(ENTRY_CMDLINE_DESC_FORMAT, m_multiLineEditDialogKeyHelper.FormatLineBreakHint());
		break;
	case kMultilineState:
		entryDescription.Format(ENTRY_STATE_DESC_FORMAT, m_multiLineEditDialogKeyHelper.FormatLineBreakHint());
		break;
	case kMultilineDescription:
		entryDescription.Format(ENTRY_DESCRIPTION_DESC_FORMAT, m_multiLineEditDialogKeyHelper.FormatLineBreakHint());
		break;
	}

	if( !entryDescription.IsEmpty() )
	{
		m_entryStatic.SetWindowText(entryDescription);
	}
}

/*!
	Set the actions depending on entry type
*/
void CMultilineEntryDlg::SetActions()
{
	switch( m_multilineEntryType )
	{
	default:
		ASSERT(FALSE); // Unknown type
	case kMultilineLogMessage:
		AddAction(kActionOpenEditor, "Editor", true);
		AddAction(kActionOpenTemplate, "Template");
		break;
	case kMultilineCmdLine:
		AddAction(kActionBrowseTag, "Tag", true);
		AddAction(kActionBrowseModule, "Module");
		AddAction(kActionFormatCvsRoot, "CVSROOT");
		break;
	case kMultilineState:
		AddAction(kActionOpenEditor, "Editor", true);
		break;
	case kMultilineDescription:
		AddAction(kActionOpenEditor, "Editor", true);
		break;
	}
}

/*!
	Get the entry window dialog ID
	\return The entry window dialog ID
*/
int CMultilineEntryDlg::GetEntryTextID()
{
	return m_wordWrapCtrl.GetCheck() ? IDC_MULTILINE_ENTRY_WRAP : IDC_MULTILINE_ENTRY;
}

/*!
	Get the entry window
	\return The entry window
*/
CWnd* CMultilineEntryDlg::GetEntryTextWnd()
{
	return GetDlgItem(GetEntryTextID());
}

/// WM_ERASEBKGND message handler, prevent flickering while switching the entry text edit controls
BOOL CMultilineEntryDlg::OnEraseBkgnd(CDC* pDC) 
{
	if( m_switchingWrap )
	{
		return TRUE;
	}
	
	return CHHelpPropertyPage::OnEraseBkgnd(pDC);
}

/// BN_CLICKED message handler, switch the log message windows 
void CMultilineEntryDlg::OnWordwrap() 
{
	gCvsPrefs.SetWordWrapLogMsg(m_wordWrapCtrl.GetCheck());
	SwitchWrapEdit(gCvsPrefs.WordWrapLogMsg());
}

/// BN_CLICKED message handler, execute the selected action
void CMultilineEntryDlg::OnActionGo() 
{
	const int selItem = m_actionCombo.GetCurSel();
	if( selItem > -1 )
	{
		const kActionType action = (kActionType)m_actionCombo.GetItemData(selItem);
		
		switch( action )
		{
		case kActionOpenEditor:
			ActionOpenEditor();
			break;
		case kActionOpenTemplate:
			ActionOpenTemplate();
			break;
		case kActionBrowseTag:
			ActionBrowseTag();
			break;
		case kActionBrowseModule:
			ActionBrowseModule();
			break;
		case kActionFormatCvsRoot:
			ActionFormatCvsRoot();
			break;
		default:
			ASSERT(FALSE);	// Unknown action
			break;
		}
	}
}

/*!
	Open the external editor to edit the text
*/
void CMultilineEntryDlg::ActionOpenEditor()
{
	CString strText;
	CEdit* pEdit = (CEdit*)GetEntryTextWnd();

	pEdit->GetWindowText(strText);

	string message = strText;
	
	if( CompatGetEditorMsg(message) )
	{
		CString strMessage(message.c_str());
		strMessage.Replace("\n", "\r\n");
		
		if( !strMessage.IsEmpty() )
		{
			pEdit->SetSel(0, -1);
			pEdit->ReplaceSel(strMessage, TRUE);		
		}
	}
	
	GotoDlgCtrl(GetEntryTextWnd());
}

/*!
	Open the CVS template file and insert as edited text
*/
void CMultilineEntryDlg::ActionOpenTemplate()
{
	string message;
	
	if( CompatGetCvsTemplate(m_mf, message) )
	{
		CString strMessage(message.c_str());
		strMessage.Replace("\n", "\r\n");
		
		if( !strMessage.IsEmpty() )
		{
			CEdit* pEdit = (CEdit*)GetEntryTextWnd();
			pEdit->SetSel(0, -1);
			pEdit->ReplaceSel(strMessage, TRUE);		
		}
	}
	
	GotoDlgCtrl(GetEntryTextWnd());
}

/*!
	Browse for tag to insert into edited text
*/
void CMultilineEntryDlg::ActionBrowseTag()
{
	auto_ptr<ItemSelectionData> itemSelectionData;
	
	if( m_cvsrootEntryDlg )
	{
		itemSelectionData = auto_ptr<ItemSelectionData>(new ItemSelectionData(*m_cvsrootEntryDlg));
	}
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, itemSelectionData.get()) )
	{
		CEdit* pEdit = (CEdit*)GetEntryTextWnd();
		pEdit->ReplaceSel(tagName.c_str(), TRUE);
	}
}

/*!
	Browse for module or path to insert into edited text
*/
void CMultilineEntryDlg::ActionBrowseModule()
{
	auto_ptr<ItemSelectionData> itemSelectionData;
	
	if( m_cvsrootEntryDlg )
	{
		itemSelectionData = auto_ptr<ItemSelectionData>(new ItemSelectionData(*m_cvsrootEntryDlg));
	}
	
	string moduleName;
	if( CompatGetModuleListItem(m_mf, moduleName, itemSelectionData.get()) )
	{
		CEdit* pEdit = (CEdit*)GetEntryTextWnd();
		pEdit->ReplaceSel(moduleName.c_str(), TRUE);
	}
}

/*!
	Format CVSROOT string
*/
void CMultilineEntryDlg::ActionFormatCvsRoot()
{
	string cvsRoot;
	if( CompatGetCvsRoot(cvsRoot) )
	{
		CEdit* pEdit = (CEdit*)GetEntryTextWnd();
		pEdit->ReplaceSel(cvsRoot.c_str(), TRUE);
	}
}

/// EN_SETFOCUS message handler, cancel the selection if it's the first time command line control gets focus
void CMultilineEntryDlg::OnSetfocusLogmsg() 
{
	if( m_firstFocus )
	{
		m_firstFocus = false;
		
		if( kMultilineCmdLine == m_multilineEntryType )
		{
			CString cmdLine;
			m_entryTextCtrl.GetWindowText(cmdLine);
			
			if( DEF_CVS_CMD == cmdLine )
			{
				m_entryTextCtrl.SetSel(-1, -1);
			}
		}
	}
}

/// EN_SETFOCUS message handler, cancel the selection if it's the first time command line control gets focus
void CMultilineEntryDlg::OnSetfocusLogmsgWrap() 
{
	if( m_firstFocus )
	{
		m_firstFocus = false;
		
		if( kMultilineCmdLine == m_multilineEntryType )
		{
			CString cmdLine;
			m_entryTextCtrlWrap.GetWindowText(cmdLine);
			
			if( DEF_CVS_CMD == cmdLine )
			{
				m_entryTextCtrlWrap.SetSel(-1, -1);
			}
		}
	}
}
