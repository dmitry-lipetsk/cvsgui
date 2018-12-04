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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- August 2003
 */

/*
 * SubstModeEntryDlg.cpp : CVSROOT entry complex
 */

// SubstModeEntryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "SubstModeEntryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/// Default keyword expantion modes
char *sDefKeywordModes[] = 
{
	"o",
	"b",
	"u",
	"kv",
	"k",
	"v",
	0L
};

/// Keyword modes history
CMString gKeywordModes(20, "P_KeywordModes", sDefKeywordModes);

//////////////////////////////////////////////////////////////////////////
// CKOpt

char CKOpt::m_encodings[] = {
	CHR_SUBSTMODE_TEXT, 
	CHR_SUBSTMODE_BINARY, 
	CHR_SUBSTMODE_BINARYDELTA, 
	CHR_SUBSTMODE_UNICODE, 
	CHR_SUBSTMODE_EXTENDEDENCODING
};

CKOpt::CKOpt(const char* kOpt)
{
	if( kOpt )
	{
		m_kOpt = kOpt;
	}
}

/*!
	Get substitution mode string
	\return Substitution mode string
*/
const std::string& CKOpt::GetOptString() const
{
	return m_kOpt;
}

/*!
	Verify that a given option is present
	\param opt Option to check
	\param isEncoding true if the option is an encoding, false otherwise
	\return true if option is found, false otherwise
*/
bool CKOpt::HasOption(const char opt, const bool isEncoding /*= false*/) const
{
	bool res = false;

	if( isEncoding )
	{
		const char encoding = GetEncoding();
		
		res = encoding == opt && m_kOpt.find(encoding) < FindFlagsPos();
	}
	else
	{
		res = m_kOpt.find(opt, FindFlagsPos()) != std::string::npos;
	}
	
	return res;
}

/*!
	Get the encoding
	\return The encoding
*/
char CKOpt::GetEncoding() const
{
	char res = CHR_SUBSTMODE_TEXT;
	
	const size_type encodingPos = m_kOpt.find_first_of(CKOpt::m_encodings, 0, sizeof(CKOpt::m_encodings));
	if( encodingPos != std::string::npos )
	{
		res = m_kOpt.at(encodingPos);
	}
	
	return res;
}

/*!
	Find the flags position
	\return Flags position
	\note Returned position may point past the string if there are no flags
*/
CKOpt::size_type CKOpt::FindFlagsPos() const
{
	size_type res = 0;

	if( m_kOpt.length() > 0 )
	{
		const char encoding = GetEncoding();
		
		const size_type encodingPos = m_kOpt.find(encoding);
		if( encodingPos != std::string::npos )
		{
			res = 1;
			
			if( encoding == CHR_SUBSTMODE_EXTENDEDENCODING )
			{
				const size_type encodingClosePos = m_kOpt.find(CHR_SUBSTMODE_EXTENDEDENCODING_CLOSE);
				if( encodingClosePos != std::string::npos && encodingClosePos > encodingPos )
				{
					res = encodingClosePos + 1;
				}
			}
		}
	}

	return res;
}

/*!
	Remove option
	\param opt Option to remove
	\param isEncoding true if the option is an encoding, false otherwise
	\return true if option is found, false otherwise
*/
bool CKOpt::RemoveOption(const char opt, const bool isEncoding /*= false*/)
{
	bool res = HasOption(opt, isEncoding);

	if( res )
	{
		size_type flagsPos = FindFlagsPos();
		if( flagsPos < m_kOpt.length() )
		{
			const size_type optPos = m_kOpt.find(opt, flagsPos);
			if( optPos != std::string::npos )
			{
				m_kOpt.erase(optPos, 1);
			}
		}
	}

	return res;
}

/*!
	Set option
	\param opt Option to set
	\param isEncoding true if the option is an encoding, false otherwise
	\return true if option was present, false otherwise
*/
bool CKOpt::SetOption(const char opt, const bool isEncoding /*= false*/)
{
	bool res = HasOption(opt);

	if( isEncoding )
	{
		if( m_kOpt.length() > 0 )
		{
			const char encoding = GetEncoding();
			
			const size_type encodingPos = m_kOpt.find(encoding);
			if( encodingPos != std::string::npos )
			{
				const size_type eraseCount = FindFlagsPos() - encodingPos;

				m_kOpt.erase(encodingPos, eraseCount);
			}
		}

		if( opt == CHR_SUBSTMODE_EXTENDEDENCODING )
		{
			m_kOpt.insert(m_kOpt.begin(), 1, CHR_SUBSTMODE_EXTENDEDENCODING_CLOSE);
		}
		
		m_kOpt.insert(m_kOpt.begin(), 1, opt);
	}
	else
	{
		if( !res )
		{
			m_kOpt += opt;
		}
	}

	return res;
}

/*!
	Format substitution mode description
	\return Formatted substitution mode description
*/
std::string CKOpt::FormatDescription() const
{
	std::string res = "Current";

	if( !m_kOpt.empty() )
	{
		switch( GetEncoding() )
		{
		case CHR_SUBSTMODE_TEXT:
			res = "Text" " encoding";
			break;
		case CHR_SUBSTMODE_BINARY:
			res = "Binary" " encoding";
			break;
		case CHR_SUBSTMODE_BINARYDELTA:
			res = "Binary deltas" " encoding";
			break;
		case CHR_SUBSTMODE_UNICODE:
			res = "Unicode" " encoding";
			break;
		case CHR_SUBSTMODE_EXTENDEDENCODING:
			res = "Extended" " encoding";
			break;
		}

		if( HasOption(CHR_SUBSTMODE_SUBSTITUTEKEYWORD) )
		{
			res += ", " "substitute " "keyword";
		}

		if( HasOption(CHR_SUBSTMODE_SUBSTITUTEVALUE) )
		{
			res += ", " "substitute " "value";
		}

		if( HasOption(CHR_SUBSTMODE_NOSUBSTITUTION) )
		{
			res += ", " "no substitution";
		}

		if( HasOption(CHR_SUBSTMODE_LOCKERSNAME) )
		{
			res += ", " "insert lockers name";
		}

		if( HasOption(CHR_SUBSTMODE_FORCERESERVEDEDIT) )
		{
			res += ", " "force reserved edit";
		}

		if( HasOption(CHR_SUBSTMODE_UNIXLINEENDINGS) )
		{
			res += ", " "unix line endings";
		}

		if( HasOption(CHR_SUBSTMODE_COMPRESSDELTAS) )
		{
			res += ", " "compress deltas";
		}
	}

	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CSubstModeEntryDlg property page

IMPLEMENT_DYNCREATE(CSubstModeEntryDlg, CHHelpPropertyPage)

CSubstModeEntryDlg::CSubstModeEntryDlg() 
	: m_keywordCombo(USmartCombo::AutoDropWidth), CHHelpPropertyPage(CSubstModeEntryDlg::IDD)
{
	//{{AFX_DATA_INIT(CSubstModeEntryDlg)
	m_keyword = _T("");
	//}}AFX_DATA_INIT
	
	m_keywordCombo.SetItems(&gKeywordModes);
}

CSubstModeEntryDlg::~CSubstModeEntryDlg()
{
}

void CSubstModeEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubstModeEntryDlg)
	DDX_Control(pDX, IDC_KEYWORD_DESC, m_keywordDescEdit);
	DDX_Control(pDX, IDC_BROWSE_KEYWORD, m_browseKeywordButton);
	DDX_Control(pDX, IDC_KEYWORD_OPT, m_keywordCombo);
	DDX_CBString(pDX, IDC_KEYWORD_OPT, m_keyword);
	//}}AFX_DATA_MAP

	DDX_ComboMString(pDX, IDC_KEYWORD_OPT, m_keywordCombo);
}


BEGIN_MESSAGE_MAP(CSubstModeEntryDlg, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CSubstModeEntryDlg)
	ON_CBN_SELCHANGE(IDC_KEYWORD_OPT, OnSelchangeKeywordOpt)
	ON_BN_CLICKED(IDC_BROWSE_KEYWORD, OnBrowseKeyword)
	ON_COMMAND(ID_SUBSTMODE_TEXT, OnSubstmodeText)
	ON_COMMAND(ID_SUBSTMODE_BINARY, OnSubstmodeBinary)
	ON_COMMAND(ID_SUBSTMODE_BINARYDELTA, OnSubstmodeBinarydelta)
	ON_COMMAND(ID_SUBSTMODE_COMPRESSDELTAS, OnSubstmodeCompressdeltas)
	ON_COMMAND(ID_SUBSTMODE_EXTENDEDENCODING, OnSubstmodeExtendedencoding)
	ON_COMMAND(ID_SUBSTMODE_FORCERESERVEDEDIT, OnSubstmodeForcereservededit)
	ON_COMMAND(ID_SUBSTMODE_LOCKERSNAME, OnSubstmodeLockersname)
	ON_COMMAND(ID_SUBSTMODE_NOSUBSTITUTION, OnSubstmodeNosubstitution)
	ON_COMMAND(ID_SUBSTMODE_SUBSTITUTEKEYWORD, OnSubstmodeSubstitutekeyword)
	ON_COMMAND(ID_SUBSTMODE_SUBSTITUTEVALUE, OnSubstmodeSubstitutevalue)
	ON_COMMAND(ID_SUBSTMODE_UNICODE, OnSubstmodeUnicode)
	ON_COMMAND(ID_SUBSTMODE_UNIXLINEENDINGS, OnSubstmodeUnixlineendings)
	ON_CBN_EDITCHANGE(IDC_KEYWORD_OPT, OnEditchangeKeywordOpt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubstModeEntryDlg message handlers

void CSubstModeEntryDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_KEYWORD_OPT,		IDH_IDC_KEYWORD_OPT);
	SetCtrlHelpID(IDC_KEYWORD_DESC,		IDH_IDC_KEYWORD_DESC);
	SetCtrlHelpID(IDC_BROWSE_KEYWORD,	IDH_IDC_BROWSE_KEYWORD);
}

/*!
	Create the entry dialog
	\param pWndInsertAfter Placeholder window
	\param pParentWnd Parent window
	\param updateData true to invoke UpdateData(FALSE) for the parent dialog after the creation to display the initial text entry, false otherwise
	\return TRUE on success, FALSE otherwise
*/
BOOL CSubstModeEntryDlg::Create(const CWnd* pWndInsertAfter, CDialog* pParentWnd, const bool updateData /*= true*/)
{
	BOOL res = FALSE;

	ASSERT(pWndInsertAfter != NULL);	// Invalid pointer
	ASSERT(pParentWnd != NULL);			// Invalid pointer

	m_pParentWnd = pParentWnd;

	if( CHHelpPropertyPage::Create(CSubstModeEntryDlg::IDD, m_pParentWnd) )
	{
		// Get the width difference
		CRect entryRect;
		GetWindowRect(entryRect);

		CRect windowRect;
		pWndInsertAfter->GetWindowRect(windowRect);

		const int widthDiff = windowRect.Width() - entryRect.Width();

		// Adjust the size of keyword description and move the browse button to stick in
		if( widthDiff )
		{
			CRect keywordDescRect;
			m_keywordDescEdit.GetWindowRect(keywordDescRect);
			
			m_keywordDescEdit.SetWindowPos(NULL, 0, 0, keywordDescRect.Width() + widthDiff, keywordDescRect.Height(), SWP_NOZORDER | SWP_NOMOVE);
		}

		// Move the entry window into the placeholder space
		pParentWnd->ScreenToClient(windowRect);
		SetWindowPos(pWndInsertAfter, windowRect.left, windowRect.top, windowRect.Width(), windowRect.Height(), SWP_SHOWWINDOW);

		// Update state of controls
		if( updateData )
		{
			m_pParentWnd->UpdateData(FALSE);
		}

		OnSelchangeKeywordOpt();

		res = TRUE;
	}

	return res;
}

/*!
	Enable or disable controls
	\param enable Set to TRUE to enable controls, FALSE to disable controls
*/
void CSubstModeEntryDlg::EnableControls(const BOOL enable)
{
	m_keywordDescEdit.EnableWindow(enable);
	m_keywordCombo.EnableWindow(enable);
	m_browseKeywordButton.EnableWindow(enable);
}

/*!
	Invoke DDX for multiline entry
	\param bSaveAndValidate Validation flag
	\param modeText Return the entered text
	\return TRUE on success, FALSE otherwise
*/
BOOL CSubstModeEntryDlg::UpdateData(BOOL bSaveAndValidate, CString& modeText)
{
	BOOL res = FALSE;

	if( bSaveAndValidate )
	{
		res = ::IsWindow(GetSafeHwnd()) ? CHHelpPropertyPage::UpdateData(TRUE) : TRUE;

		modeText = m_keyword;
	}
	else
	{
		m_keyword = modeText;

		res = ::IsWindow(GetSafeHwnd()) ? CHHelpPropertyPage::UpdateData(FALSE) : TRUE;
	}

	return res;
}

/// OnInitDialog virtual override, initialize controls
BOOL CSubstModeEntryDlg::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	UpdateDescription();

	COLORMAP colorMap[1] = { RGB(255, 0, 255), GetSysColor(COLOR_3DFACE) };

	m_popupBitmap.LoadMappedBitmap(IDB_BUTTON_POPUP, 0, colorMap, 1);
	m_browseKeywordButton.SetBitmap(m_popupBitmap);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// BN_CLICKED message handler, browse for keyword substitution option
void CSubstModeEntryDlg::OnBrowseKeyword() 
{
	CMenu keywordMenu;

	if( keywordMenu.LoadMenu(IDR_SUBSTMODE) )
	{
		CRect keywordButtonRect;
		m_browseKeywordButton.GetWindowRect(keywordButtonRect);

		CPoint point(keywordButtonRect.right, keywordButtonRect.top);

		CMenu* pSubMenu = keywordMenu.GetSubMenu(0);
		if( pSubMenu )
		{
			CString strSubstMode;
			m_keywordCombo.GetWindowText(strSubstMode);

			CKOpt kOpt(strSubstMode);

			// Select the encoding as radio
			int encoding = GetMenuItemPos(pSubMenu, ID_SUBSTMODE_TEXT);

			switch( kOpt.GetEncoding() )
			{
			case CHR_SUBSTMODE_BINARY:
				encoding = GetMenuItemPos(pSubMenu, ID_SUBSTMODE_BINARY);
				break;
			case CHR_SUBSTMODE_BINARYDELTA:
				encoding = GetMenuItemPos(pSubMenu, ID_SUBSTMODE_BINARYDELTA);
				break;
			case CHR_SUBSTMODE_UNICODE:
				encoding = GetMenuItemPos(pSubMenu, ID_SUBSTMODE_UNICODE);
				break;
			case CHR_SUBSTMODE_EXTENDEDENCODING:
				encoding = GetMenuItemPos(pSubMenu, ID_SUBSTMODE_EXTENDEDENCODING);
				break;
			}

			pSubMenu->CheckMenuRadioItem(GetMenuItemPos(pSubMenu, ID_SUBSTMODE_TEXT), 
				GetMenuItemPos(pSubMenu, ID_SUBSTMODE_EXTENDEDENCODING), 
				encoding, MF_BYPOSITION);
			
			// Set other options as check marks
			pSubMenu->CheckMenuItem(ID_SUBSTMODE_SUBSTITUTEKEYWORD,	MF_BYCOMMAND | (kOpt.HasOption(CHR_SUBSTMODE_SUBSTITUTEKEYWORD) ? MF_CHECKED : MF_UNCHECKED));
			pSubMenu->CheckMenuItem(ID_SUBSTMODE_SUBSTITUTEVALUE,	MF_BYCOMMAND | (kOpt.HasOption(CHR_SUBSTMODE_SUBSTITUTEVALUE) ? MF_CHECKED : MF_UNCHECKED));
			pSubMenu->CheckMenuItem(ID_SUBSTMODE_LOCKERSNAME,		MF_BYCOMMAND | (kOpt.HasOption(CHR_SUBSTMODE_LOCKERSNAME) ? MF_CHECKED : MF_UNCHECKED));
			pSubMenu->CheckMenuItem(ID_SUBSTMODE_NOSUBSTITUTION,	MF_BYCOMMAND | (kOpt.HasOption(CHR_SUBSTMODE_NOSUBSTITUTION) ? MF_CHECKED : MF_UNCHECKED));
			pSubMenu->CheckMenuItem(ID_SUBSTMODE_FORCERESERVEDEDIT,	MF_BYCOMMAND | (kOpt.HasOption(CHR_SUBSTMODE_FORCERESERVEDEDIT) ? MF_CHECKED : MF_UNCHECKED));
			pSubMenu->CheckMenuItem(ID_SUBSTMODE_UNIXLINEENDINGS,	MF_BYCOMMAND | (kOpt.HasOption(CHR_SUBSTMODE_UNIXLINEENDINGS) ? MF_CHECKED : MF_UNCHECKED));
			pSubMenu->CheckMenuItem(ID_SUBSTMODE_COMPRESSDELTAS,	MF_BYCOMMAND | (kOpt.HasOption(CHR_SUBSTMODE_COMPRESSDELTAS) ? MF_CHECKED : MF_UNCHECKED));

			// Disable menus unavailable for selected encoding
			if( kOpt.GetEncoding() == CHR_SUBSTMODE_BINARY || kOpt.GetEncoding() == CHR_SUBSTMODE_BINARYDELTA )
			{
				pSubMenu->EnableMenuItem(ID_SUBSTMODE_SUBSTITUTEKEYWORD, MF_BYCOMMAND | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_SUBSTMODE_SUBSTITUTEVALUE, MF_BYCOMMAND | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_SUBSTMODE_LOCKERSNAME, MF_BYCOMMAND | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_SUBSTMODE_NOSUBSTITUTION, MF_BYCOMMAND | MF_GRAYED);
				pSubMenu->EnableMenuItem(ID_SUBSTMODE_UNIXLINEENDINGS, MF_BYCOMMAND | MF_GRAYED);
			}

			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
		else
		{
			ASSERT(FALSE); // Popup menu not found
		}
	}
	else
	{
		ASSERT(FALSE); // Menu not found
	}
}

/*!
	Toggle substitution mode option
	\param opt Option to be toggled
	\param isEncoding true if the option is an encoding, false otherwise
*/
void CSubstModeEntryDlg::ToggleSubstmodeOption(const char opt, const bool isEncoding /*= false*/)
{
	// Get current options
	CString strSubstMode;
	m_keywordCombo.GetWindowText(strSubstMode);

	strSubstMode.TrimLeft();
	strSubstMode.TrimRight();
	
	CKOpt kOpt(strSubstMode);

	if( !isEncoding || kOpt.GetEncoding() != opt )
	{
		// Toggle option
		if( !isEncoding && kOpt.HasOption(opt, isEncoding) )
		{
			kOpt.RemoveOption(opt, isEncoding);
		}
		else
		{
			if( isEncoding )
			{
				if( opt == CHR_SUBSTMODE_BINARY || opt == CHR_SUBSTMODE_BINARYDELTA )
				{
					kOpt.RemoveOption(CHR_SUBSTMODE_SUBSTITUTEKEYWORD);
					kOpt.RemoveOption(CHR_SUBSTMODE_SUBSTITUTEVALUE);
					kOpt.RemoveOption(CHR_SUBSTMODE_NOSUBSTITUTION);
					kOpt.RemoveOption(CHR_SUBSTMODE_LOCKERSNAME);
					kOpt.RemoveOption(CHR_SUBSTMODE_UNIXLINEENDINGS);
				}
			}
			else
			{
				switch( opt )
				{
				case CHR_SUBSTMODE_NOSUBSTITUTION:
					kOpt.RemoveOption(CHR_SUBSTMODE_SUBSTITUTEKEYWORD);
					kOpt.RemoveOption(CHR_SUBSTMODE_SUBSTITUTEVALUE);
					kOpt.RemoveOption(CHR_SUBSTMODE_LOCKERSNAME);
					break;
				case CHR_SUBSTMODE_SUBSTITUTEKEYWORD:
				case CHR_SUBSTMODE_SUBSTITUTEVALUE:
				case CHR_SUBSTMODE_LOCKERSNAME:
					kOpt.RemoveOption(CHR_SUBSTMODE_NOSUBSTITUTION);
					break;
				}
			}

			kOpt.SetOption(opt, isEncoding);
		}
	}

	// Set the modified options string back into control
	m_keywordCombo.SetFocus();
	m_keywordCombo.GetEditCtrl()->SetSel(0, -1, TRUE);
	m_keywordCombo.GetEditCtrl()->ReplaceSel(kOpt.GetOptString().c_str(), TRUE);
	
	const int caretStartPos = isEncoding ? 1 : kOpt.GetOptString().length();
	int caretStopPos = caretStartPos;

	if( isEncoding && kOpt.GetEncoding() == CHR_SUBSTMODE_EXTENDEDENCODING )
	{
		const int extendedEncodingEndPos = kOpt.GetOptString().find(CHR_SUBSTMODE_EXTENDEDENCODING_CLOSE);
		if( extendedEncodingEndPos != std::string::npos )
		{
			caretStopPos = extendedEncodingEndPos;
		}
	}

	m_keywordCombo.GetEditCtrl()->SetSel(0, 0);
	m_keywordCombo.GetEditCtrl()->SetSel(caretStartPos, caretStopPos);

	UpdateDescription();
}

/*!
	Update current keyword expansion mode description
*/
void CSubstModeEntryDlg::UpdateDescription()
{
	CString strSubstMode;
	m_keywordCombo.GetWindowText(strSubstMode);
	
	CKOpt kOpt(strSubstMode);
	
	m_keywordDescEdit.SetWindowText(kOpt.FormatDescription().c_str());
}

/// ID_SUBSTMODE_TEXT message handler
void CSubstModeEntryDlg::OnSubstmodeText() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_TEXT, true);
}

/// ID_SUBSTMODE_BINARY message handler
void CSubstModeEntryDlg::OnSubstmodeBinary() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_BINARY, true);
}

/// ID_SUBSTMODE_BINARYDELTA message handler
void CSubstModeEntryDlg::OnSubstmodeBinarydelta() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_BINARYDELTA, true);
}

/// ID_SUBSTMODE_COMPRESSDELTAS message handler
void CSubstModeEntryDlg::OnSubstmodeCompressdeltas() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_COMPRESSDELTAS);
}

/// ID_SUBSTMODE_EXTENDEDENCODING message handler
void CSubstModeEntryDlg::OnSubstmodeExtendedencoding() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_EXTENDEDENCODING, true);
}

/// ID_SUBSTMODE_FORCERESERVEDEDIT message handler
void CSubstModeEntryDlg::OnSubstmodeForcereservededit() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_FORCERESERVEDEDIT);
}

/// ID_SUBSTMODE_LOCKERSNAME message handler
void CSubstModeEntryDlg::OnSubstmodeLockersname() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_LOCKERSNAME);
}

/// ID_SUBSTMODE_NOSUBSTITUTION message handler
void CSubstModeEntryDlg::OnSubstmodeNosubstitution() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_NOSUBSTITUTION);
}

/// ID_SUBSTMODE_SUBSTITUTEKEYWORD message handler
void CSubstModeEntryDlg::OnSubstmodeSubstitutekeyword() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_SUBSTITUTEKEYWORD);
}

/// ID_SUBSTMODE_SUBSTITUTEVALUE message handler
void CSubstModeEntryDlg::OnSubstmodeSubstitutevalue() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_SUBSTITUTEVALUE);
}

/// ID_SUBSTMODE_UNICODE message handler
void CSubstModeEntryDlg::OnSubstmodeUnicode() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_UNICODE, true);
}

/// ID_SUBSTMODE_UNIXLINEENDINGS message handler
void CSubstModeEntryDlg::OnSubstmodeUnixlineendings() 
{
	ToggleSubstmodeOption(CHR_SUBSTMODE_UNIXLINEENDINGS);
}

/// CBN_EDITCHANGE message handler, update the description text
void CSubstModeEntryDlg::OnEditchangeKeywordOpt() 
{
	UpdateDescription();
}

/// CBN_SELCHANGE message handler, update the description text
void CSubstModeEntryDlg::OnSelchangeKeywordOpt() 
{
	m_keywordCombo.EnsureSelchangeMatch();
	
	UpdateDescription();
}
