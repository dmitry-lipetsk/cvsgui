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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- March 2004
 */

// CvsRootDlg.cpp : implementation file
//


#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#include "CvsRootDlg.h"
#include "AppConsole.h"
#include "CvsArgs.h"
#include "CvsAlert.h"
#include "MultiString.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/// Token separator in CVS root string
#define CHR_ROOT_TOKEN_SEPARATOR		':'

/// Keyword quote character in CVS root string
#define CHR_KEYWORD_QUOTE				'\''

/// Protocol token separator in CVS root string
#define CHR_PROTOCOL_TOKEN_SEPARATOR	';'

/// Keyword token separator
#define CHR_KEYWORD_TOKEN_SEPARATOR		'='

static CMString	gPrevCvsRootRepositoryPaths(20, "P_PrevCvsRootRepositoryPaths");
static CMKeyString gPrevCvsRootKeywords(200, "P_PrevCvsRootKeywords");

//////////////////////////////////////////////////////////////////////////
// CCvsOutBufferProxy

CCvsOutBufferProxy::CCvsOutBufferProxy(const char* data, int size)
	: m_data(data), m_size(size)
{
}

CCvsOutBufferProxy::CCvsOutBufferProxy()
	: m_data(NULL), m_size(0)
{
}

/// Get the buffer data
const char* CCvsOutBufferProxy::GetData() const
{
	return m_data;
}

/// Get the buffer size
int CCvsOutBufferProxy::GetSize() const
{
	return m_size;
}

//////////////////////////////////////////////////////////////////////////
// CCvsRootConsole

CCvsRootConsole::CCvsRootConsole()
{
	m_launchRes = 0;
}

CCvsRootConsole::~CCvsRootConsole()
{
}

long CCvsRootConsole::cvs_out(const char*txt, long len)
{
	m_cvsOutVec.resize(m_cvsOutVec.size() + len);
	memcpy(&(*m_cvsOutVec.begin()) + m_cvsOutVec.size() - len, txt, len);
	
	return len;
}

long CCvsRootConsole::cvs_err(const char* txt, long len)
{
	cvs_errstr(txt, len);
	
	m_cvsErrVec.resize(m_cvsErrVec.size() + len);
	memcpy(&(*m_cvsErrVec.begin()) + m_cvsErrVec.size() - len, txt, len);

	return len;
}

/*!
	Virtual overridable to parse the result of browse command
	\param launchRes The result of the command launch
	\note Call default implementation after parsing is finished to clear the stored command output and store the launch result, 
	the error output is not cleared however  to allow further analysis
*/
void CCvsRootConsole::Parse(int launchRes)
{
	m_launchRes = launchRes;
	m_cvsOutVec.clear();
}

/// Get the result of the command launch
int CCvsRootConsole::GetLaunchRes() const
{
	return m_launchRes;
}

/// Get the command result
CCvsOutBufferProxy CCvsRootConsole::GetCvsOut() const
{
	return CCvsOutBufferProxy(m_cvsOutVec.size() > 0 ? &(*m_cvsOutVec.begin()) : NULL, m_cvsOutVec.size()) ;
}

/// Set the command result
void CCvsRootConsole::SetCvsOut(const char* str, size_t lLen)
{
	m_cvsOutVec.assign(str,str+lLen);
}

/// Get the error result
CCvsOutBufferProxy CCvsRootConsole::GetCvsErr() const
{
	return CCvsOutBufferProxy(m_cvsErrVec.size() > 0 ? &(*m_cvsErrVec.begin()) : NULL, m_cvsErrVec.size());
}

//////////////////////////////////////////////////////////////////////////
// CInfoParserConsole

CInfoParserConsole::CInfoParserConsole()
{
}

CInfoParserConsole::~CInfoParserConsole()
{
}

void CInfoParserConsole::Parse(int launchRes)
{
	bool protocols = false;
	
	string line;

#ifdef HAVE_SSTREAM
	istringstream myCvsOut(GetCvsOut().GetData());
#else
	istrstream myCvsOut(GetCvsOut().GetData(), GetCvsOut().GetSize());
#endif

	while( getline(myCvsOut, line) )
	{
		if( protocols )
		{
			stringstream stream;
			string protocol;
			
			stream << line;
			stream >> protocol;
			
			if( !protocol.empty() )
			{
				string description;

				while( !stream.eof() )
				{
					string token;
					stream >> token;

					if( !description.empty() )
					{
						description += " ";
					}

					description += token;
				}

				m_cvsProtocolList.insert(std::make_pair(protocol, description));
			}
		}
		
		if( line.find("Available protocols:") != string::npos )
		{
			protocols = true;
		}
	}
	
	CCvsRootConsole::Parse(launchRes);
}

/*!
	Get the CVS protocol list
	\return CVS protocol list
*/
const CvsProtocolList& CInfoParserConsole::GetCvsProtocolList() const
{
	return m_cvsProtocolList;
}

//////////////////////////////////////////////////////////////////////////
// CProtocolParserConsole

CProtocolParserConsole::CProtocolParserConsole()
{
}

CProtocolParserConsole::~CProtocolParserConsole()
{
}

void CProtocolParserConsole::Parse(int launchRes)
{
	bool keywords = false;
	
	string line;
	
#ifdef HAVE_SSTREAM
	istringstream myCvsOut(GetCvsOut().GetData());
#else
	istrstream myCvsOut(GetCvsOut().GetData(), GetCvsOut().GetSize());
#endif

	while( getline(myCvsOut, line) )
	{
		if( keywords )
		{
			stringstream stream;
			string keyword;
			
			stream << line;
			stream >> keyword;
			
			if( !keyword.empty() )
			{
				string description;

				while( !stream.eof() )
				{
					string token;
					stream >> token;

					if( !description.empty() )
					{
						if( token.find("(alias:") != string::npos || token.find("(aliases:") != string::npos )
						{
							// Bail out to eliminate alias(es)
							break;
						}

						description += " ";
					}

					description += token;
				}

				KeywordInfo rowInfo;
				rowInfo.m_keyword = keyword;
				rowInfo.m_description = description;
				
				m_keywordList.push_back(rowInfo);
			}
		}
		
		if( line.find("Keywords available:") != string::npos )
		{
			keywords = true;
		}
	}
	
	CCvsRootConsole::Parse(launchRes);
}

/*!
	Get the protocol keywords list
	\return Protocol keywords list
*/
const KeywordList& CProtocolParserConsole::GetKeywordList() const
{
	return m_keywordList;
}

//////////////////////////////////////////////////////////////////////////
// CCvsRootParser

/*!
	Normalize CVS root string to prepare for parsing
	\param strCvsRoot Return CVS root string
	\param tokenSeparator Token separator to be used in normalized string
	\return The count of tokens in the CVS root string
*/
int CCvsRootParser::NormalizeCvsRoot(std::string& strCvsRoot, const char tokenSeparator) const
{
	int tokenCount = 0;
	
	// Eliminate leading token separator
	if( !strCvsRoot.empty() && CHR_ROOT_TOKEN_SEPARATOR == *strCvsRoot.begin() )
	{
		strCvsRoot.erase(strCvsRoot.begin());
	}
	
	// Flag token separators
	bool inQuoted = false;
	
	for(string::iterator it = strCvsRoot.begin(); it != strCvsRoot.end() ; it++)
	{
		if( CHR_KEYWORD_QUOTE == *it )
		{
			inQuoted = !inQuoted;
		}
		
		if( inQuoted )
		{
			// We are inside quoted value
			continue;
		}
		
		if( 0 == tokenCount )
		{
			bool isRootTokenSeparator = CHR_ROOT_TOKEN_SEPARATOR == *it;
			
			if( CHR_PROTOCOL_TOKEN_SEPARATOR == *it || isRootTokenSeparator )
			{
				*it = tokenSeparator;
				tokenCount++;
			}
			
			if( isRootTokenSeparator )
			{
				if( it + 1 != strCvsRoot.end() )
				{
					tokenCount++;
				}
				
				break;
			}
		}
		else if( 1 == tokenCount )
		{
			if( CHR_ROOT_TOKEN_SEPARATOR == *it )
			{
				*it = tokenSeparator;
				tokenCount++;
				
				if( it + 1 != strCvsRoot.end() )
				{
					tokenCount++;
				}
				
				break;
			}
		}
	}
	
	return tokenCount;
}

/*!
	Split CVS root string into tokens
	\param cvsRoot CVS root string
	\param protocol Return protocol
	\param keywords Return keywords string
	\param path Return path
	\return true on success, false otherwise
*/
bool CCvsRootParser::SplitCvsRoot(const char* cvsRoot, std::string& protocol, std::string& keywords, std::string& path) const
{
	bool res = false;
	
	if( cvsRoot )
	{
		string strCvsRoot = cvsRoot;
		
		const char tokenSeparator = '\x01';
		
		// Preprocess CVS root to prevent colons posing as a component separator
		const int tokenCount = NormalizeCvsRoot(strCvsRoot, tokenSeparator);

		// Separate tokens
		if( tokenCount > 1 )
		{
			string token;
			stringstream cvsRootStream(strCvsRoot);
			
			// Get the protocol
			if( getline(cvsRootStream, token, tokenSeparator) )
			{
				protocol = token;
				
				// Get keywords
				if( tokenCount > 2 )
				{
					if( getline(cvsRootStream, token, tokenSeparator) )
					{
						keywords = token;
					}
				}

				// Get the path
				if( getline(cvsRootStream, token, tokenSeparator) )
				{
					path = token;
					
					// At this point parsing can be considered a success
					res = true;
				}
			}
		}
	}

	return res;
}

/*!
	Parse given CVS root string
	\param cvsRoot CVS root string
	\param clearData true to clear the data
	\return true on success, false otherwise
*/
bool CCvsRootParser::Parse(const char* cvsRoot, const bool clearData /*= true*/)
{
	bool res = false;

	if( clearData )
	{
		Clear();
	}

	string protocol;
	string keywords;
	string path;

	if( SplitCvsRoot(cvsRoot, protocol, keywords, path) )
	{
		m_protocol = protocol;
		m_path = path;

		// Parse the keywords
		if( !keywords.empty() )
		{
			string protocolToken;
			stringstream protocolStream(keywords);
			
			// Keywords
			while( getline(protocolStream, protocolToken, CHR_PROTOCOL_TOKEN_SEPARATOR) )
			{
				string keywordToken;
				string valueToken;
				stringstream keywordStream(protocolToken);
				
				if( getline(keywordStream, keywordToken, CHR_KEYWORD_TOKEN_SEPARATOR) &&
					getline(keywordStream, valueToken, CHR_KEYWORD_TOKEN_SEPARATOR) )
				{
					valueToken.erase(0, valueToken.find_first_not_of(CHR_KEYWORD_QUOTE));
					valueToken.erase(valueToken.find_last_not_of(CHR_KEYWORD_QUOTE) + 1, string::npos);

					m_cvsProtocolList.erase(keywordToken);
					m_cvsProtocolList.insert(std::make_pair(keywordToken, valueToken));
				}
			}
		}

		res = true;
	}

	return res;
}

/*!
	Clear the data
*/
void CCvsRootParser::Clear()
{
	m_protocol.erase();
	m_path.erase();
	m_cvsProtocolList.clear();
}

/*!
	Test whether the parser data are empty
	\return true on if the parser data are empty, false otherwise
*/
bool CCvsRootParser::IsEmpty() const
{
	return m_protocol.empty() && m_path.empty();
}

/*!
	Get the protocol
	\return Protocol
*/
const std::string& CCvsRootParser::GetProtocol() const
{
	return m_protocol;
}
	
/*!
	Get the repository path
	\return Repository path
*/
const std::string& CCvsRootParser::GetPath() const
{
	return m_path;
}

/*!
	Get the protocol keywords list
	\return Protocol keywords list
*/
const CvsProtocolList& CCvsRootParser::GetCvsProtocolList() const
{
	return m_cvsProtocolList;
}


/*!
	Launch and parse CVS command appropriate to the type and parameters
	\param cvsRootConsole CVS root console
	\param protocol Protocol to get the information for, NULL to get the protocols list
	\return The launch result
*/
static int LaunchCvsRootCommand(CCvsRootConsole* cvsRootConsole, const char* protocol = NULL)
{
	CvsArgs args;
	args.add("info");

	if( protocol )
	{
		args.add(protocol);
	}

	const char* dir = NULL;
	
	args.print(dir);
	
	const int launchRes = launchCVS(dir, args.Argc(), args.Argv(), cvsRootConsole);
	cvsRootConsole->Parse(launchRes);

	return launchRes;
}

/*!
	Format CVS root
	\param protocol Protocol
	\param displayInfoList Display info list
	\param path Repository path
	\return CVS root string
*/
static string FormatCvsRoot(const char* protocol, KeywordDisplayInfoList& displayInfoList, const char* path)
{
	string cvsroot;

	cvsroot = CHR_ROOT_TOKEN_SEPARATOR;

	if( protocol )
	{
		cvsroot += protocol;
	}

	for(KeywordDisplayInfoList::const_iterator it = displayInfoList.begin(); it != displayInfoList.end(); it++)
	{
		const KeywordDisplayInfo* displayInfo = *it;
		if( displayInfo && !displayInfo->GetValue().empty() )
		{
			bool needsQuote = displayInfo->GetValue().find(CHR_ROOT_TOKEN_SEPARATOR) != string::npos;

			cvsroot += CHR_PROTOCOL_TOKEN_SEPARATOR;
			cvsroot += displayInfo->GetRowInfo().m_keyword;
			cvsroot += CHR_KEYWORD_TOKEN_SEPARATOR;
			
			if( needsQuote )
			{
				cvsroot += CHR_KEYWORD_QUOTE;
			}

			cvsroot += displayInfo->GetValue();

			if( needsQuote )
			{
				cvsroot += CHR_KEYWORD_QUOTE;
			}
		}
	}

	cvsroot += CHR_ROOT_TOKEN_SEPARATOR;
	cvsroot += path;

	return cvsroot;
}

//////////////////////////////////////////////////////////////////////////
// KeywordDisplayInfo

/*!
	Create display info
	\param rowInfo Row info for the keyword
*/
void KeywordDisplayInfo::Create(const KeywordInfo& rowInfo)
{
	m_name = rowInfo.m_keyword;

	string::iterator it = m_name.begin();
	it++;

	if( it != m_name.end() )
	{
		transform(m_name.begin(), it, m_name.begin(), (int(*)(int))toupper);
	}

	m_rowInfo = rowInfo;
}

/// Get name text
const std::string& KeywordDisplayInfo::GetName() const
{
	return m_name;
}

/// Get the keyword value
const std::string& KeywordDisplayInfo::GetValue() const
{
	return m_value;
}

/*!
	Set the value
	\param value Value, can be NULL
*/
void KeywordDisplayInfo::SetValue(const char* value)
{
	m_value = value ? value : "";
}

/// Get the row info
const KeywordInfo& KeywordDisplayInfo::GetRowInfo() const
{
	return m_rowInfo;
}

#ifdef WIN32

#define WM_READ_PROTOCOLS	WM_APP + 1

/////////////////////////////////////////////////////////////////////////////
// CEditKeywordDlg dialog


CEditKeywordDlg::CEditKeywordDlg(const char* description, const char* keyword, const char* value, 
						   CWnd* pParent /*=NULL*/)
: CHHelpDialog(CEditKeywordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditKeywordDlg)
	m_description = description;
	m_keyword = keyword;
	m_value = value;
	//}}AFX_DATA_INIT

	m_valueCombo.SetItems(&gPrevCvsRootKeywords);
	m_valueCombo.SetKeyword(m_keyword);
}


void CEditKeywordDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditKeywordDlg)
	DDX_Control(pDX, IDC_KEYWORD_VALUE, m_valueCombo);
	DDX_Text(pDX, IDC_KEYWORD_DESCRIPTION, m_description);
	DDX_Text(pDX, IDC_PROTOCOL_KEYWORD, m_keyword);
	DDX_CBString(pDX, IDC_KEYWORD_VALUE, m_value);
	//}}AFX_DATA_MAP

	DDX_ComboMString(pDX, IDC_KEYWORD_VALUE, m_valueCombo);
}


BEGIN_MESSAGE_MAP(CEditKeywordDlg, CHHelpDialog)
//{{AFX_MSG_MAP(CEditKeywordDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditKeywordDlg message handlers

void CEditKeywordDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_PROTOCOL_KEYWORD,		IDH_IDC_PROTOCOL_KEYWORD);
	SetCtrlHelpID(IDC_KEYWORD_DESCRIPTION,	IDH_IDC_KEYWORD_DESCRIPTION);
	SetCtrlHelpID(IDC_KEYWORD_VALUE,		IDH_IDC_KEYWORD_VALUE);
}

/// BN_CLICKED message handler, display dialog help
void CEditKeywordDlg::OnHelp() 
{
	ShowHelp();
}

/// OnOK virtual override, warn when password entered
void CEditKeywordDlg::OnOK() 
{
	if( !UpdateData(TRUE) )
	{
		return;
	}

	if( m_keyword.CompareNoCase("password") == 0 && !m_value.IsEmpty() )
	{
		CvsAlert cvsAlert(kCvsAlertWarningIcon, 
			_i18n("Entering password in CVSROOT is a security risk!"), 
			_i18n("Password entered as part of CVSROOT string will be stored by CVS client in its metadata in clear text and may pose a security risk."),
			BUTTONTITLE_OK, BUTTONTITLE_CANCEL);
		
		if( cvsAlert.ShowAlert() != kCvsAlertOKButton )
		{
			return;
		}
	}
	
	CHHelpDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CCvsRootDlg dialog


CCvsRootDlg::CCvsRootDlg(const char* cvsRoot, CWnd* pParent /*=NULL*/)
	: m_protocolCombo(USmartCombo::AutoDropWidth), 
	CHHelpDialog(CCvsRootDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCvsRootDlg)
	m_protocol = _T("");
	m_cvsroot = _T("");
	m_repositoryPath = _T("");
	//}}AFX_DATA_INIT

	m_cvsroot = cvsRoot;
	m_cvsRootParser.Parse(m_cvsroot);

	m_cvsStopped = false;
	m_repositoryPathCombo.SetItems(&gPrevCvsRootRepositoryPaths);
}


void CCvsRootDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCvsRootDlg)
	DDX_Control(pDX, IDC_REPOSITORY_PATH, m_repositoryPathCombo);
	DDX_Control(pDX, IDC_PREVIEW_CVSROOT, m_cvsrootStatic);
	DDX_Control(pDX, IDC_KEYWORDLIST_DESC, m_keywordDescStatic);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, IDC_PROTOCOL_DESC, m_protocolDescStatic);
	DDX_Control(pDX, IDC_CLEAR_KEYWORD, m_clearButton);
	DDX_Control(pDX, IDC_EDIT_KEYWORD, m_editButton);
	DDX_Control(pDX, IDC_KEYWORDLIST, m_keywordList);
	DDX_Control(pDX, IDC_PROTOCOL, m_protocolCombo);
	DDX_CBString(pDX, IDC_PROTOCOL, m_protocol);
	DDX_Text(pDX, IDC_PREVIEW_CVSROOT, m_cvsroot);
	DDX_CBString(pDX, IDC_REPOSITORY_PATH, m_repositoryPath);
	//}}AFX_DATA_MAP

	DDV_MinChars(pDX, m_repositoryPath, 1);
	DDX_ComboMString(pDX, IDC_REPOSITORY_PATH, m_repositoryPathCombo);
}


BEGIN_MESSAGE_MAP(CCvsRootDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CCvsRootDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_KEYWORDLIST, OnDblclkKeywordlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_KEYWORDLIST, OnItemchangedKeywordlist)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_PROTOCOL, OnSelchangeProtocol)
	ON_BN_CLICKED(IDC_EDIT_KEYWORD, OnEdit)
	ON_BN_CLICKED(IDC_CLEAR_KEYWORD, OnClear)
	ON_CBN_EDITCHANGE(IDC_REPOSITORY_PATH, OnEditchangePath)
	ON_CBN_SELCHANGE(IDC_REPOSITORY_PATH, OnSelchangePath)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_READ_PROTOCOLS, OnReadProtocols)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCvsRootDlg message handlers

/// OnInitDialog virtual override, initializes controls and triggers data reading if necessary
BOOL CCvsRootDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	SetListCtrlExtendedStyle(&m_keywordList, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_GRIDLINES);
	
	InitListCtrl(kCvsRootDisplayData);

	ShowProtocolDescription();

	// Force the window to show itself and trigger the reading of available protocols
	ShowWindow(SW_SHOW);
	PostReadProtocolsMsg();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCvsRootDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_PROTOCOL,			IDH_IDC_PROTOCOL);
	SetCtrlHelpID(IDC_PROTOCOL_DESC,	IDH_IDC_PROTOCOL_DESC);
	SetCtrlHelpID(IDC_REPOSITORY_PATH,	IDH_IDC_REPOSITORY_PATH);
	SetCtrlHelpID(IDC_KEYWORDLIST,		IDH_IDC_KEYWORDLIST);
	SetCtrlHelpID(IDC_KEYWORDLIST_DESC,	IDH_IDC_KEYWORDLIST_DESC);
	SetCtrlHelpID(IDC_EDIT_KEYWORD,		IDH_IDC_EDIT_KEYWORD);
	SetCtrlHelpID(IDC_CLEAR_KEYWORD,	IDH_IDC_CLEAR_KEYWORD);
	SetCtrlHelpID(IDC_PREVIEW_CVSROOT,	IDH_IDC_PREVIEW_CVSROOT);
}

/*!
	Set the list control columns appropriate to the dialog type, show prompt message eventually
	\param displayType Display type as specified by kCvsRootDisplayType enum
	\note Prompt message will be shown while data are being read
*/
void CCvsRootDlg::InitListCtrl(const kCvsRootDisplayType displayType)
{
	CWndRedrawMngr redrawMngr(&m_keywordList);

	// Remove all items and columns
	DeleteAllItems();

	int columnCount = m_keywordList.GetHeaderCtrl()->GetItemCount();
	while( columnCount-- )
	{
		m_keywordList.DeleteColumn(0);
	}

	// Get the client area width to properly size columns
	CRect rect;
	m_keywordList.GetClientRect(rect);

	const int listCtrlWidth = rect.Width();

	// Set the appropriate columns
	switch( displayType )
	{
	case kCvsRootDisplayWait:
		m_keywordList.InsertColumn(0, _i18n("Action"), LVCFMT_LEFT, listCtrlWidth);

		m_keywordList.InsertItem(0, _i18n("Please wait while retrieving data"));
		m_keywordList.InsertItem(1, _i18n("You can press Stop button to cancel the operation"));
		break;
	case kCvsRootDisplayData:
		{
			const int listCtrlWidthNoScroll = listCtrlWidth - GetSystemMetrics(SM_CXVSCROLL);
			const int fistColWidth = 100;

			m_keywordList.InsertColumn(0, _i18n("Keyword"), LVCFMT_LEFT, fistColWidth);
			m_keywordList.InsertColumn(1, _i18n("Value"), LVCFMT_LEFT, listCtrlWidthNoScroll - fistColWidth);
		}
		break;
	case kCvsRootDisplayInfo:
		m_keywordList.InsertColumn(0, _i18n("Information"), LVCFMT_LEFT, listCtrlWidth);
		break;
	default:
		ASSERT(FALSE);	// unknown display type
		break;
	}
}

/*!
	Enable or disable controls
	\param enable Set to TRUE to enable controls, FALSE to disable controls
*/
void CCvsRootDlg::EnableControls(BOOL enable)
{
	m_okButton.EnableWindow(enable);
	m_keywordList.EnableWindow(enable);
	m_protocolCombo.EnableWindow(enable);
	m_editButton.EnableWindow(enable);
	m_clearButton.EnableWindow(enable);
	m_repositoryPathCombo.EnableWindow(enable);
}

/*!
	Enable or disable keyword edit controls
	\param displayInfo Display info of the selected item, NULL if there is no selection
*/
void CCvsRootDlg::EnableEditControls(const KeywordDisplayInfo* displayInfo)
{
	if( displayInfo )
	{
		m_editButton.EnableWindow(TRUE);
		m_clearButton.EnableWindow(!displayInfo->GetValue().empty());
	}
	else
	{
		m_editButton.EnableWindow(FALSE);
		m_clearButton.EnableWindow(FALSE);
	}
}

/*!
	Post the private message to trigger the reading of the available protocols
	\return TRUE on success, FALSE otherwise
*/
BOOL CCvsRootDlg::PostReadProtocolsMsg()
{
	return PostMessage(WM_READ_PROTOCOLS);
}

/// WM_READ_PROTOCOLS message handler, read the protocols available
void CCvsRootDlg::OnReadProtocols()
{
	ReadAvailableProtocols();

	if( m_protocolCombo.GetCount() > 0 )
	{
		if( !m_cvsRootParser.IsEmpty() )
		{
			m_protocolCombo.SelectString(-1, m_cvsRootParser.GetProtocol().c_str());
			m_repositoryPathCombo.SetWindowText(m_cvsRootParser.GetPath().c_str());

			ShowProtocolDescription();
			ReadProtocolDetails();
		}
	}
}

/*!
	Fill protocol selection combo box
	\param infoParserConsole CVS Info parser console
*/
void CCvsRootDlg::FillProtocolsComboBox(CInfoParserConsole& infoParserConsole)
{
	m_protocolCombo.ResetContent();
	m_protocolDescStatic.SetWindowText("");
	
	const CvsProtocolList& cvsProtocolList = infoParserConsole.GetCvsProtocolList();
	if( cvsProtocolList.size() == 0 )
	{
		if( !m_cvsStopped )
		{
			if( infoParserConsole.GetLaunchRes() == 0 )
			{
				InitListCtrl(kCvsRootDisplayInfo);
				m_keywordList.InsertItem(0, _i18n("No protocols available, check your CVS configuration"));

				EnableControls(FALSE);
			}
			else
			{
				InitListCtrl(kCvsRootDisplayInfo);
				
				// Show the error in the list control
				m_keywordList.InsertItem(0, _i18n("Command failed:"));
				
				string line;

				istrstream myCvsErr(infoParserConsole.GetCvsErr().GetData(), infoParserConsole.GetCvsErr().GetSize());

				while( getline(myCvsErr, line) )
				{
					m_keywordList.InsertItem(m_keywordList.GetItemCount(), line.c_str());
				}
				
				EnableControls(FALSE);
			}
		}
	}
	else
	{
		// Fill the protocols combobox
		for(CvsProtocolList::const_iterator it = cvsProtocolList.begin(); it != cvsProtocolList.end(); it++)
		{
			const int item = m_protocolCombo.AddString((*it).first.c_str());
			if( item > -1 )
			{
				m_protocolCombo.SetItemData(item, (DWORD)new string((*it).second));
			}
		}

		// Show instructions and enable protocols combobox
		InitListCtrl(kCvsRootDisplayInfo);
		m_keywordList.InsertItem(0, _i18n("Select the protocol and enter repository path and any keywords required"));
		
		EnableControls(FALSE);
		m_protocolCombo.EnableWindow(TRUE);
	}
}

/*!
	Display items in the list
	\param protocolParserConsole Protocol parser console
*/
void CCvsRootDlg::DisplayKeywords(CProtocolParserConsole& protocolParserConsole)
{
	const KeywordList& itemList = protocolParserConsole.GetKeywordList();
	if( itemList.size() == 0 )
	{
		if( !m_cvsStopped )
		{
			if( protocolParserConsole.GetLaunchRes() == 0 )
			{
				InitListCtrl(kCvsRootDisplayInfo);
				m_keywordList.InsertItem(0, _i18n("No keywords available"));
				
				m_keywordList.EnableWindow(FALSE);
				EnableEditControls(FALSE);
			}
			else
			{
				InitListCtrl(kCvsRootDisplayInfo);
				
				// Show the error in the list control
				m_keywordList.InsertItem(0, _i18n("Command failed:"));
				
				string line;

				istrstream myCvsErr(protocolParserConsole.GetCvsErr().GetData(), protocolParserConsole.GetCvsErr().GetSize());

				while( getline(myCvsErr, line) )
				{
					m_keywordList.InsertItem(m_keywordList.GetItemCount(), line.c_str());
				}
				
				EnableControls(FALSE);
			}
		}
	}
	else
	{
		// Insert the items
		for(KeywordList::const_iterator it = itemList.begin(); it != itemList.end(); it++)
		{
			KeywordDisplayInfo* displayInfo = new KeywordDisplayInfo;
			if( displayInfo )
			{
				displayInfo->Create(*it);
				
				const int itemIndex = m_keywordList.InsertItem(m_keywordList.GetItemCount(), displayInfo->GetName().c_str());
				if( itemIndex > -1 )
				{
					if( !m_keywordList.SetItemData(itemIndex, (DWORD)displayInfo) )
					{
						delete displayInfo;
					}
				}
				else
				{
					delete displayInfo;
				}
			}
		}
	}
}

/*!
	Update keywords with any found in the protocol list
	\param protocolList Protocol list
*/
void CCvsRootDlg::UpdateKeywords(const CvsProtocolList& protocolList)
{
	const int itemCount = m_keywordList.GetItemCount();
	for(int nIndex = 0; nIndex < itemCount; nIndex++)
	{
		KeywordDisplayInfo* displayInfo = (KeywordDisplayInfo*)m_keywordList.GetItemData(nIndex);
		if( displayInfo )
		{
			CvsProtocolList::const_iterator it = protocolList.find(displayInfo->GetRowInfo().m_keyword);
			if( it != m_cvsRootParser.GetCvsProtocolList().end() )
			{
				displayInfo->SetValue((*it).second.c_str());
				m_keywordList.SetItemText(nIndex, 1, displayInfo->GetValue().c_str());
			}
		}
	}
}

/*!
	Remove all items and release associated memory held in Item Data of the list control
*/
void CCvsRootDlg::DeleteAllItems()
{
	const int itemCount = m_keywordList.GetItemCount();
	for(int nIndex = 0; nIndex < itemCount; nIndex++)
	{
		KeywordDisplayInfo* displayInfo = (KeywordDisplayInfo*)m_keywordList.GetItemData(nIndex);
		delete displayInfo;
	}

	m_keywordList.DeleteAllItems();
}

/// OnCancel virtual override, stop CVS command or cancel dialog
void CCvsRootDlg::OnCancel() 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app && app->IsCvsRunning() && !app->IsCvsStopping() )
	{
		m_keywordList.DeleteAllItems();
		m_keywordList.InsertItem(0, _i18n("Stopping..."));

		m_cvsStopped = true;
		stopCVS();
		return;
	}
	
	CHHelpDialog::OnCancel();
}

/// NM_DBLCLK message handler, edit the selected keyword
void CCvsRootDlg::OnDblclkKeywordlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 )
	{
		OnEdit();
	}

	*pResult = 0;
}

/// LVN_ITEMCHANGED message handler, update the keyword description
void CCvsRootDlg::OnItemchangedKeywordlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 && 
		pNMListView->uChanged & LVIF_STATE && pNMListView->uNewState & (LVIS_SELECTED | LVIS_FOCUSED) )
	{
		const KeywordDisplayInfo* displayInfo = (KeywordDisplayInfo*)m_keywordList.GetItemData(pNMListView->iItem);
		if( displayInfo )
		{
			m_keywordDescStatic.SetWindowText(displayInfo->GetRowInfo().m_description.c_str());

			EnableEditControls(displayInfo);
		}
	}

	if( m_keywordList.GetNextItem(-1, LVNI_SELECTED) < 0 )
	{
		m_keywordDescStatic.SetWindowText("");

		EnableEditControls(NULL);
	}
	
	*pResult = 0;
}

/// WM_DESTROY message handler, release any memory allocated
void CCvsRootDlg::OnDestroy() 
{
	DeleteAllItems();

	const int count = m_protocolCombo.GetCount();
	for(int nIndex = 0; nIndex < count; nIndex++)
	{
		delete (string*)m_protocolCombo.GetItemData(nIndex);
	}

	CHHelpDialog::OnDestroy();
}

/// CBN_SELCHANGE notification handler, show the selected protocol description and trigger the reading of the details
void CCvsRootDlg::OnSelchangeProtocol() 
{
	ShowProtocolDescription();
	ReadProtocolDetails();
}

/*!
	Show the selected protocol description
*/
void CCvsRootDlg::ShowProtocolDescription()
{
	const int item = m_protocolCombo.GetCurSel();
	if( item > -1 )
	{
		string* description = (string*)m_protocolCombo.GetItemData(item);
		if( description )
		{
			m_protocolDescStatic.SetWindowText(description->c_str());
		}
	}
	else
	{
		m_protocolDescStatic.SetWindowText("");
		m_keywordDescStatic.SetWindowText("");
	}
}

/*!
	Read available protocols
*/
void CCvsRootDlg::ReadAvailableProtocols()
{
	m_cvsStopped = false;

	// Prepare controls
	EnableControls(FALSE);

	// Change the Cancel button's name to "Stop"
	CString strPrevCancelText;
	m_cancelButton.GetWindowText(strPrevCancelText);
	m_cancelButton.SetWindowText(_i18n("Stop"));

	// Display the reading data prompt
	InitListCtrl(kCvsRootDisplayWait);

	// Launch command
	CInfoParserConsole infoParserConsole;
	LaunchCvsRootCommand(&infoParserConsole);

	// Initialize list control
	InitListCtrl(kCvsRootDisplayData);

	// Restore control's state
	EnableControls(TRUE);

	// Display data
	FillProtocolsComboBox(infoParserConsole);

	// Restore Cancel's button name
	m_cancelButton.SetWindowText(strPrevCancelText);

	// If there are any protocols set the focus to protocol combobox and enable path entry
	if( m_protocolCombo.GetCount() )
	{
		GotoDlgCtrl(&m_protocolCombo);
		m_repositoryPathCombo.EnableWindow(TRUE);
	}
}

/*!
	Read selected protocol details
*/
void CCvsRootDlg::ReadProtocolDetails()
{
	m_cvsStopped = false;

	m_keywordDescStatic.SetWindowText("");

	CString cvsRoot;
	m_cvsrootStatic.GetWindowText(cvsRoot);
	
	CString protocol;
	
	{
		const int item = m_protocolCombo.GetCurSel();
		if( item > -1 )
		{
			m_protocolCombo.GetLBText(item, protocol);
		}
		else
		{
			// No protocol selected
			return;
		}
	}

	// Prepare controls
	EnableControls(FALSE);

	// Change the Cancel button's name to "Stop"
	CString strPrevCancelText;
	m_cancelButton.GetWindowText(strPrevCancelText);
	m_cancelButton.SetWindowText(_i18n("Stop"));

	// Display the reading data prompt
	InitListCtrl(kCvsRootDisplayWait);

	// Launch command
	CProtocolParserConsole protocolParserConsole;
	LaunchCvsRootCommand(&protocolParserConsole, protocol);

	// Initialize list control
	InitListCtrl(kCvsRootDisplayData);

	// Restore control's state
	EnableControls(TRUE);

	// Display data
	DisplayKeywords(protocolParserConsole);

	// Restore Cancel's button name
	m_cancelButton.SetWindowText(strPrevCancelText);

	// If there are any keywords select the first keyword
	if( m_keywordList.IsWindowEnabled() && m_keywordList.GetItemCount() )
	{
		m_keywordList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}

	GotoDlgCtrl(&m_protocolCombo);

	m_cvsRootParser.Parse(cvsRoot, false);
	UpdateKeywords(m_cvsRootParser.GetCvsProtocolList());

	UpdateCvsRoot();
}

/// BN_CLICKED message handler, edit selected keyword
void CCvsRootDlg::OnEdit() 
{
	const int item = m_keywordList.GetNextItem(-1, LVNI_SELECTED);
	if( item > -1 )
	{
		KeywordDisplayInfo* displayInfo = (KeywordDisplayInfo*)m_keywordList.GetItemData(item);
		if( displayInfo )
		{
			CEditKeywordDlg editKeywordDlg(displayInfo->GetRowInfo().m_description.c_str(), displayInfo->GetName().c_str(), displayInfo->GetValue().c_str());

			if( editKeywordDlg.DoModal() == IDOK )
			{
				displayInfo->SetValue(editKeywordDlg.m_value);
				m_keywordList.SetItemText(item, 1, displayInfo->GetValue().c_str());

				EnableEditControls(displayInfo);
				UpdateCvsRoot();
			}
		}
	}
}

/// BN_CLICKED message handler, clear selected keyword
void CCvsRootDlg::OnClear() 
{
	const int item = m_keywordList.GetNextItem(-1, LVNI_SELECTED);
	if( item > -1 )
	{
		KeywordDisplayInfo* displayInfo = (KeywordDisplayInfo*)m_keywordList.GetItemData(item);
		if( displayInfo )
		{
			displayInfo->SetValue("");
			m_keywordList.SetItemText(item, 1, displayInfo->GetValue().c_str());

			EnableEditControls(displayInfo);
			UpdateCvsRoot();
		}
	}
}

/// CBN_EDITCHANGE notification handler, update CVSROOT display
void CCvsRootDlg::OnEditchangePath() 
{
	UpdateCvsRoot();
}

/// CBN_SELCHANGE notification handler, update CVSROOT display
void CCvsRootDlg::OnSelchangePath() 
{
	m_repositoryPathCombo.EnsureSelchangeMatch();

	UpdateCvsRoot();
}

/*!
	Update CVS root string display
*/
void CCvsRootDlg::UpdateCvsRoot()
{
	KeywordDisplayInfoList displayInfoList;
	
	const int itemCount = m_keywordList.GetItemCount();
	for(int nIndex = 0; nIndex < itemCount; nIndex++)
	{
		const KeywordDisplayInfo* displayInfo = (KeywordDisplayInfo*)m_keywordList.GetItemData(nIndex);
		if( displayInfo )
		{
			displayInfoList.push_back(displayInfo);
		}
	}

	const CString protocol = m_protocolCombo.GetCurSelText();
	const CString path = m_repositoryPathCombo.GetWindowText();
	const string cvsroot = FormatCvsRoot(protocol, displayInfoList, path);

	m_cvsrootStatic.SetWindowText(cvsroot.c_str());
}

/// BN_CLICKED message handler, display dialog help
void CCvsRootDlg::OnHelp() 
{
	ShowHelp();
}

#endif	// WIN32

/// Get CVSROOT
bool CompatGetCvsRoot(std::string& cvsRoot)
{
	bool userHitOK = false;

#ifdef WIN32
	CCvsRootDlg cvsRootDlg(cvsRoot.c_str());
	
	if( cvsRootDlg.DoModal() == IDOK )
	{
		cvsRoot = cvsRootDlg.m_cvsroot;

		userHitOK = true;
	}
#endif /* WIN32 */

#if qUnix
#endif // qUnix
	
#ifdef qMacCvsPP
#endif /* qMacCvsPP */
	
	return userHitOK;
}
