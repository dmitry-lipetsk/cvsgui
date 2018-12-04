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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 1998
 */

/*
 * CommitDlg.cpp : the cvs commit dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include <process.h>
#else /* !WIN32 */
#	include <ctype.h>
#endif /* !WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LMultiPanelView.h>
#	include <LPopupGroupBox.h>
#	include <LCheckBox.h>
#	include <LTextEditView.h>
#	include <LPushButton.h>
#	include "LPopupFiller.h"

#	include "MacCvsConstant.h"
# include "MacMisc.h"
# include "LogParse.h"
#endif /* qMacCvsPP */

#include "CommitDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "getline.h"
#include "MultiString.h"
#include "CvsCommands.h"
#include "EditorMonDlg.h"
#include "ItemListDlg.h"
#include "BrowseViewHandlers.h"

using namespace std;

CMString gCommitLogs(20, "P_CommitLogs");
static CPersistentBool gCheckValidEdits("P_CheckValidEdits", false, kAddSettings);

#ifndef WIN32
/*!
	Show the ellipsis if the string is too long
	\param str String to modify
*/
void combozify(string& str)
{
	// extract the first xxx words
	const char* tmp = str.c_str();
	string result;
	int numWords = 0;
#	define HOWMUCH_WORDS 5
	while(*tmp != '\0' && numWords < HOWMUCH_WORDS)
	{
		while(*tmp != '\0' && isspace(*tmp))
			tmp++;

		while(*tmp != '\0' && !isspace(*tmp))
			result += *tmp ++;

		if( *tmp != '\0' )
			result += ' ';

		numWords++;
	}

	if( *tmp != '\0' )
	{
		result += "...";
	}

	str = result;
}
#endif

#ifdef WIN32
#	include "GetPrefs.h"

#	ifdef _DEBUG
#		define new DEBUG_NEW
#		undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
#	endif

IMPLEMENT_DYNAMIC(CCommit_MAIN, CHHelpPropertyPage)

CCommit_MAIN::CCommit_MAIN(const MultiFiles* mf, const char* logMsg) 
	: m_mf(mf), 
	m_multilineEntryDlg(NULL, mf), 
	m_fileToDiffCombo(USmartCombo::AutoDropWidth | USmartCombo::ReadOnly), 
	CHHelpPropertyPage(CCommit_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CCommit_MAIN)
	m_logMsg = logMsg;
	//}}AFX_DATA_INIT
}

CCommit_MAIN::~CCommit_MAIN()
{
}

void CCommit_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommit_MAIN)
	DDX_Control(pDX, IDC_MULTILINE_ENTRY_STATIC, m_multilineEntryPlaceholder);
	DDX_Control(pDX, IDC_PREVIEWINFO, m_previewInfo);
	DDX_Control(pDX, IDC_FILETODIFF, m_fileToDiffCombo);
	//}}AFX_DATA_MAP

	m_multilineEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_logMsg);
}

BEGIN_MESSAGE_MAP(CCommit_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CCommit_MAIN)
	ON_BN_CLICKED(IDC_DIFF, OnDiff)
	ON_CBN_SELCHANGE(IDC_FILETODIFF, OnSelchangeFiletodiff)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/// Display preview file info
void CCommit_MAIN::UpdatePreviewInfo()
{
	CString strPreviewInfo;
	CString strPreviewInfoNumbers;
	int nSelInfo = m_fileToDiffCombo.GetCurSel();
	int nFiles = m_fileToDiffCombo.GetCount();
	if( nSelInfo < 0 || !nFiles )
	{
		nSelInfo = 0;
		strPreviewInfoNumbers = "not supported";
	}
	else
	{
		if( nSelInfo == 0 && nFiles > 1 )
			strPreviewInfoNumbers.Format("%d files", nFiles - 1);
		else
		{
			if( nFiles > 1 )
				nFiles--;
			else
				nSelInfo++;

			strPreviewInfoNumbers.Format("%d/%d", nSelInfo, nFiles);
		}
	}

	strPreviewInfo.Format("Preview changes (%s):", (LPCSTR)strPreviewInfoNumbers);
	m_previewInfo.SetWindowText(strPreviewInfo);
}

/// CBN_SELCHANGE message handler, update preview file info
void CCommit_MAIN::OnSelchangeFiletodiff() 
{
	UpdatePreviewInfo();
}

/// BN_CLICKED message handler, launch external diff to preview the changes
void CCommit_MAIN::OnDiff() 
{
	const int nComboIndex = m_fileToDiffCombo.GetCurSel();
	if( nComboIndex > CB_ERR )
	{
		MultiFilesEntryIndex* entryIndex = (MultiFilesEntryIndex*)m_fileToDiffCombo.GetItemData(nComboIndex);
		string path;
		string fileName;
		string currRev;
		int nFiles = m_fileToDiffCombo.GetCount();

		if( nComboIndex == 0 && nFiles > 1 )
		{
			MultiFiles mf(*m_mf);

			KoDiffHandler handler;
			handler.OnFiles(&mf);
		}
		else if( entryIndex )
		{
			MultiFiles::const_iterator mfi = entryIndex->GetIterator();

			if( mfi->get(entryIndex->GetIndex(), path, fileName, currRev) )
			{
				MultiFiles mf;
				mf.newdir(path.c_str());
				mf.newfile(fileName.c_str(), 0, currRev.c_str());
				
				KoDiffHandler handler;
				handler.OnFiles(&mf);
			}
			else
			{
				KoDiffHandler handler;
				handler.OnFolder(mfi->GetDir());
			}
		}
	}
}

/// OnInitDialog virtual override, initialize controls
BOOL CCommit_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization here
	m_multilineEntryDlg.Create(kMultilineLogMessage, &gCommitLogs, &m_multilineEntryPlaceholder, this);

	if( m_mf )
	{
		MultiFiles::const_iterator mfi;
		for(mfi = m_mf->begin(); mfi != m_mf->end(); ++mfi)
		{
			int nIndexMax = mfi->NumFiles();
			if( !nIndexMax )
				nIndexMax++;

			for( int nIndex = 0; nIndex < nIndexMax; nIndex++ )
			{
				string fullPath;
				if( mfi->NumFiles() )
				{
					mfi->GetFullPath(nIndex, fullPath);
				}
				else
				{
					fullPath = mfi->GetDir();
					TrimRight(fullPath, kUnivPathDelimiters);
				}

				const int nComboIndex = m_fileToDiffCombo.AddString(fullPath.c_str());
				
				MultiFilesEntryIndex* entryIndex = new MultiFilesEntryIndex(nComboIndex, mfi);
				m_fileToDiffCombo.SetItemData(nComboIndex, (DWORD)entryIndex);
			}
		}
	}

	if( m_fileToDiffCombo.GetCount() > 1 )
	{
		m_fileToDiffCombo.InsertString(0, "<All>");
	}

	if( m_fileToDiffCombo.GetCount() ) 
	{
		m_fileToDiffCombo.SetCurSel(0);
	}
	else
	{
		m_fileToDiffCombo.EnableWindow(FALSE);
		CWnd* pButton = GetDlgItem(IDC_DIFF);
		if( pButton )
		{
			pButton->EnableWindow(FALSE);
		}
	}
	
	UpdatePreviewInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCommit_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_PREVIEWINFO,	helpTopicIgnore);
	SetCtrlHelpID(IDC_FILETODIFF,	IDH_IDC_FILETODIFF);
	SetCtrlHelpID(IDC_DIFF,			IDH_IDC_DIFF);
}

/// WM_DESTROY message handler, release allocated memory
void CCommit_MAIN::OnDestroy() 
{
	m_multilineEntryDlg.UpdateData(TRUE, m_logMsg);

	const int count = m_fileToDiffCombo.GetCount();
	for(int nIndex = 0; nIndex < count; nIndex++)
	{
		MultiFilesEntryIndex* entryIndex = (MultiFilesEntryIndex*)m_fileToDiffCombo.GetItemData(nIndex);
		delete entryIndex;
	}

	CHHelpPropertyPage::OnDestroy();
}

/*!
	Get the log message
	\return The log message
*/
CString CCommit_MAIN::GetLogMsg() const
{
	return m_logMsg;
}

/////////////////////////////////////////////////////////////////////////////
// CCommit_OPTIONS property page

IMPLEMENT_DYNAMIC(CCommit_OPTIONS, CHHelpPropertyPage)

CCommit_OPTIONS::CCommit_OPTIONS(const MultiFiles* mf) 
	: m_mf(mf), CHHelpPropertyPage(CCommit_OPTIONS::IDD)
{
	//{{AFX_DATA_INIT(CCommit_OPTIONS)
	m_forceCommit = FALSE;
	m_rev = _T("");
	m_forceRecurse = FALSE;
	m_forceRevision = FALSE;
	m_noModuleProgram = FALSE;
	m_noRecurse = FALSE;
	m_checkValidEdits = FALSE;
	//}}AFX_DATA_INIT

	m_checkValidEdits = (bool)gCheckValidEdits;
	m_revCombo.SetItems(&gRevNames);
}

CCommit_OPTIONS::~CCommit_OPTIONS()
{
}

void CCommit_OPTIONS::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommit_OPTIONS)
	DDX_Control(pDX, IDC_REV_TAG, m_revCombo);
	DDX_Check(pDX, IDC_FORCE_COMMIT, m_forceCommit);
	DDX_CBString(pDX, IDC_REV_TAG, m_rev);
	DDX_Check(pDX, IDC_FORCE_RECURSE, m_forceRecurse);
	DDX_Check(pDX, IDC_FORCE_REVISION, m_forceRevision);
	DDX_Check(pDX, IDC_NO_MODULE_PROGRAM, m_noModuleProgram);
	DDX_Check(pDX, IDC_NORECURS, m_noRecurse);
	DDX_Check(pDX, IDC_CHECK_VALID_EDITS, m_checkValidEdits);
	//}}AFX_DATA_MAP

	if( m_forceRevision )
	{
		DDV_MinChars(pDX, m_rev, 1);
	}
	
	if( !pDX->m_bSaveAndValidate )
	{
		OnForcerevision();
	}
	
	DDX_ComboMString(pDX, IDC_REV_TAG, m_revCombo);
}


BEGIN_MESSAGE_MAP(CCommit_OPTIONS, CHHelpPropertyPage)
//{{AFX_MSG_MAP(CCommit_OPTIONS)
	ON_BN_CLICKED(IDC_FORCE_REVISION, OnForcerevision)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommit_OPTIONS message handlers

void CCommit_OPTIONS::InitHelpID()
{
	SetCtrlHelpID(IDC_NORECURS,				IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_FORCE_COMMIT,			IDH_IDC_FORCE_COMMIT);
	SetCtrlHelpID(IDC_FORCE_REVISION,		IDH_IDC_HAS_REV);
	SetCtrlHelpID(IDC_REV_TAG,				IDH_IDC_REV_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG,			IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_FORCE_RECURSE,		IDH_IDC_FORCE_RECURSE);
	SetCtrlHelpID(IDC_NO_MODULE_PROGRAM,	IDH_IDC_NO_MODULE_PROGRAM);
	SetCtrlHelpID(IDC_CHECK_VALID_EDITS,	IDH_IDC_CHECK_VALID_EDITS);
}

/// BN_CLICKED message handler, enable controls for force revision option
void CCommit_OPTIONS::OnForcerevision() 
{
	CButton* but = (CButton*)GetDlgItem(IDC_FORCE_REVISION);

	CWnd* wnd = GetDlgItem(IDC_REV_TAG);
	if( wnd )
	{
		wnd->EnableWindow(but->GetCheck());
	}

	wnd = GetDlgItem(IDC_BROWSE_TAG);
	if( wnd )
	{
		wnd->EnableWindow(but->GetCheck());
	}
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CCommit_OPTIONS::OnBrowseTag() 
{
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName) )
	{
		m_rev = tagName.c_str();
		m_revCombo.SetWindowText(m_rev);
	}
}

#endif /* WIN32 */

#ifdef qMacCvsPP
class LLogFillerWrapper : public LFillerWrapper
{
public:
	LLogFillerWrapper(LMenuController *menu, CMString & mstr, const char *pathTmpl) :
		LFillerWrapper(menu), fMStr(mstr), fFirstTime(true)
	{
		fTmplPath = pathTmpl;
		if(!fTmplPath.empty())
		{
			if(!fTmplPath.endsWith(kPathDelimiter))
				fTmplPath << kPathDelimiter;
			fTmplPath << "CVS";
			fTmplPath << kPathDelimiter;
			fTmplPath << "Template";
		}

	}
	
	virtual void OnAddingItem(string & str)
	{
		if(fFirstTime)
		{
			fMenu->InsertMenuItem("\pInsert CVS/Template", max_Int16);
			mTmplItem = ::CountMenuItems(fMenu->GetMacMenuH());
			fMenu->InsertMenuItem("\p(-", max_Int16, false);
			fFirstTime = false;
		}
		
		combozify(str);
		fMenu->InsertMenuItem(UTF8_to_LString(str, true), max_Int16);
	}
	
	virtual bool OnSelectingItem(SInt32 inItem, string & str)
	{
		str = "";
		if(inItem == mTmplItem)
		{
			bool gotIt = false;
			FILE *output;
			if(!fTmplPath.empty() && (output = fopen(fTmplPath, "r")) != 0L)
			{
				char *line = 0L;
				size_t line_chars_allocated = 0;
				int line_length;
				while ((line_length = getline (&line, &line_chars_allocated, output)) > 0)
				{
					str << line;
				}

				gotIt = true;
				if (line_length < 0 && !feof (output))
					gotIt = false;

				if(line != 0L)
					free (line);

				fclose(output);
			}
			if(!gotIt)
				str << "*found no CVS/Template file*";
			return true;
		}
		else if(inItem >= mTmplItem + 2)
		{
			str = fMStr.GetList()[inItem - 4];
			return true;
		}
		
		return false;
	}
protected:
	CMString & fMStr;
	string fTmplPath;
	SInt32 mTmplItem;
	bool fFirstTime;
};

static void DoDataExchange_Commit(LWindow *theDialog, string & commit,
	const char *pathTmpl, UInt16 & msgEncoding, bool putValue)
{
	LTextEditView *editLog = dynamic_cast<LTextEditView*>
		(theDialog->FindPaneByID(item_LogMsg));
	LPopupFiller *fillLog = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerRev1));
	LLogFillerWrapper *wrapper = putValue ? NEW LLogFillerWrapper(fillLog, gCommitLogs, pathTmpl) : 0L;
	LPopupButton *messageEncoding = dynamic_cast<LPopupButton*>(theDialog->FindPaneByID(item_LogMsgEncoding));
	
	fillLog->DoDataExchange(gCommitLogs, putValue, wrapper);

	if(putValue)
	{
	  string    sysEncodedString(UTF8_to_SysEncoding(commit));
		editLog->SetTextPtr((Ptr)(const char *)sysEncodedString, sysEncodedString.length());	
		theDialog->SetLatentSub(editLog);
		if ( messageEncoding ) messageEncoding->SetValue(msgEncoding);
	}
	else
	{
		Handle hdl = editLog->GetTextHandle();
		Assert_(hdl != 0L);
		string    str;
		str.set(*hdl, GetHandleSize(hdl));
		commit = SysEncoding_to_UTF8(str);
		if ( messageEncoding ) msgEncoding = messageEncoding->GetValue();
	}
}

static void 
DoDataExchange_CommitOptions(
	LView *theDialog, 
	const MultiFiles* mf,
	bool & norecurs,
	bool & forceCommit,
	bool & forceRecurse,
	bool & hasRev,
	string & lastRev, 
	bool & noModuleProgram,
	bool putValue)
{
	LPopupFiller *fillTag = dynamic_cast<LPopupFiller*> (theDialog->FindPaneByID(item_FillerTag));
	LTagWrapper *wrapper = putValue ? NEW LTagWrapper(fillTag, mf) : 0L;
	fillTag->DoDataExchange(gRevNames, putValue, wrapper);

	DoDataExchange (theDialog, item_NoRecurs, norecurs, putValue);
	DoDataExchange (theDialog, item_ForceCommit, forceCommit, putValue);
	DoDataExchange (theDialog, item_ForceRecurse, forceRecurse, putValue);
	DoDataExchange (theDialog, item_NoModuleProgram, noModuleProgram, putValue);
	DoDataExchange (theDialog, item_CheckRevTag, hasRev, putValue);
	DoDataExchange (theDialog, item_EditRevTag, lastRev, putValue);
}
#endif /* qMacCvsPP */

#if qUnix
class UCvsCommit : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCommit)
public:
	UCvsCommit() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsCommit() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kTabGeneral             // 2
	};

	virtual void DoDataExchange(bool fill);

protected:
	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsCommit, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCommit, UWidget)
	ON_UCOMMAND(UCvsCommit::kOK, UCvsCommit::OnOK)
	ON_UCOMMAND(UCvsCommit::kCancel, UCvsCommit::OnCancel)
UEND_MESSAGE_MAP()

int UCvsCommit::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsCommit::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsCommit::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

class UCvsCommit_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCommit_MAIN)
public:
	UCvsCommit_MAIN( const MultiFiles *mf, const char *logmsg, const char *pathTmpl);
	virtual ~UCvsCommit_MAIN() {}

	enum
	{
		kEditLog = EV_COMMAND_START,	// 0
		kComboLog,				// 1
		kComboDiffSelection,     // 2
		kExecDiff                // 3
	};

	virtual void DoDataExchange(bool fill);

	string m_logmsg;
	string m_tmplPath;

protected:

	ev_msg void OnComboSel(int pos, const char *txt);
	ev_msg void OnComboDiffSel(int pos, const char *txt);
	ev_msg int OnExecDiff(void);

	UDECLARE_MESSAGE_MAP()
private:
	const MultiFiles* m_mf;
	int m_selected_diff_file;
};

UIMPLEMENT_DYNAMIC(UCvsCommit_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCommit_MAIN, UWidget)
	ON_COMBO_SEL(UCvsCommit_MAIN::kComboLog, UCvsCommit_MAIN::OnComboSel)
	ON_COMBO_SEL(UCvsCommit_MAIN::kComboDiffSelection, UCvsCommit_MAIN::OnComboDiffSel)
	ON_UCOMMAND(UCvsCommit_MAIN::kExecDiff, UCvsCommit_MAIN::OnExecDiff)
UEND_MESSAGE_MAP()

UCvsCommit_MAIN::UCvsCommit_MAIN( const MultiFiles *mf, const char *logmsg, const char *pathTmpl)
	: UWidget(::UEventGetWidID())
{
	m_mf = mf;
	m_logmsg = logmsg;
	m_tmplPath = pathTmpl;
	m_selected_diff_file = 0;
	
	if(!m_tmplPath.empty())
	{
		NormalizeFolderPath(m_tmplPath);

		m_tmplPath += "CVS";
		m_tmplPath += kPathDelimiter;
		m_tmplPath += "Template";
	}
}

void UCvsCommit_MAIN::OnComboSel(int pos, const char *txt)
{
	if(pos == 0)
	{
		bool gotIt = false;
		FILE *output;

		if(!m_tmplPath.empty() && (output = fopen(m_tmplPath.c_str(), "r")) != 0L)
		{
			char *line = 0L;
			size_t line_chars_allocated = 0;
			int line_length;
			while ((line_length = getline (&line, &line_chars_allocated, output)) > 0)
			{
				UEventSendMessage(GetWidID(), EV_SETTEXT, kEditLog, line);
			}

			gotIt = true;
			if (line_length < 0 && !feof (output))
				gotIt = false;

			if(line != 0L)
				free (line);

			fclose(output);
		}
		if(!gotIt)
			UEventSendMessage(GetWidID(), EV_SETTEXT, kEditLog, (void *)"*found no CVS/Template file*");
	}
	else
	{
		const std::vector<string> & list = gCommitLogs.GetList();
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditLog, (void *)(const char *)list[pos - 1].c_str());
	}
}

void UCvsCommit_MAIN::OnComboDiffSel(int pos, const char *txt)
{
	m_selected_diff_file = pos;
	printf( "selected file %d, '%s'\n", pos, txt);
}

void UCvsCommit_MAIN::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kEditLog, (void *)(const char *)m_logmsg.c_str());

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kComboLog, 0L);
		UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kComboLog,
						  (void *)"Insert CVS/Template");
		const std::vector<string> & list = gCommitLogs.GetList();
		std::vector<string>::const_iterator i;
		for(i = list.begin(); i != list.end(); ++i)
		{
			string str = *i;
			combozify(str);
			UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kComboLog, (void *)(const char *)str.c_str());
		}

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kComboDiffSelection, 0L);
		if( m_mf && m_mf->TotalNumFiles() )
		{
			MultiFiles::const_iterator mfi;
			for(mfi = m_mf->begin(); mfi != m_mf->end(); ++mfi)
			{
				for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
				{
					string fullPath;
					mfi->GetFullPath(nIndex, fullPath);

					UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kComboDiffSelection,
						  (void *)(const char*)fullPath.c_str());
				}
			}
		}
		
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kEditLog, &m_logmsg);
		if(!m_logmsg.empty())
			gCommitLogs.Insert(m_logmsg.c_str());
 	}
}

int UCvsCommit_MAIN::OnExecDiff(void)
{
	if( m_mf && m_mf->TotalNumFiles() )
	{
		MultiFiles::const_iterator mfi;
		int diff_idx;
		for( diff_idx = 0, mfi = m_mf->begin(); mfi != m_mf->end(); ++mfi)
		{
			for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++, diff_idx++ )
			{
				if( diff_idx == m_selected_diff_file) 
				{
					string path;
					string fileName;
					string currRev;

					mfi->get(nIndex, path, fileName, currRev);

					MultiFiles mf;
					mf.newdir(path.c_str());
					mf.newfile(fileName.c_str(), 0, currRev.c_str());
						
					KoDiffHandler handler;
					handler.OnFiles(&mf);
					return 0;
				}
			}
		}
		// oops! file not found!
	}
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - -

class UCvsCommit_OPTIONS : public UWidget
{
	UDECLARE_DYNAMIC(UCvsCommit_OPTIONS)
public:
	UCvsCommit_OPTIONS( const MultiFiles *mf);
	virtual ~UCvsCommit_OPTIONS() {}

	enum
	{
		kNoRecurse = EV_COMMAND_START,	// 0
		kForceCommit,				// 1
		kForceRevision,     // 2
		kRevList,                // 3
		kBrowseRev, // 4
		kForceRecurse, // 5
		kNoRunModuleProgram, // 6
		kCheckValidEdits, // 7
	};

	virtual void DoDataExchange(bool fill);

	bool	m_forceCommit;
	bool	m_forceRecurse;
	bool	m_forceRevision;
	bool	m_noModuleProgram;
	bool	m_noRecurse;
	bool	m_checkValidEdits;
	string	m_rev;

protected:

	ev_msg int OnBrowseRevision(void);
	ev_msg int OnForceHit(void);
	ev_msg void OnRevisionSel(int pos, const char *txt);

	UDECLARE_MESSAGE_MAP()
private:
	const MultiFiles* m_mf;
	bool m_localforcerev;
};

UIMPLEMENT_DYNAMIC(UCvsCommit_OPTIONS, UWidget)

UBEGIN_MESSAGE_MAP(UCvsCommit_OPTIONS, UWidget)
	ON_UCOMMAND(UCvsCommit_OPTIONS::kBrowseRev, UCvsCommit_OPTIONS::OnBrowseRevision)
	ON_UCOMMAND(UCvsCommit_OPTIONS::kForceRevision, UCvsCommit_OPTIONS::OnForceHit)
	ON_COMBO_SEL(UCvsCommit_OPTIONS::kRevList, UCvsCommit_OPTIONS::OnRevisionSel)
UEND_MESSAGE_MAP()

UCvsCommit_OPTIONS::UCvsCommit_OPTIONS( const MultiFiles *mf)
	: UWidget(::UEventGetWidID())
{
	m_mf              = mf;
	m_localforcerev   = false;
	m_forceCommit     = false;
	m_forceRecurse    = false;
	m_forceRevision   = false;
	m_noModuleProgram = false;
	m_noRecurse       = false;
	m_checkValidEdits = (bool)gCheckValidEdits;
}

void UCvsCommit_OPTIONS::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kForceCommit, m_forceCommit), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kForceRecurse, m_forceRecurse), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kForceRevision, m_forceRevision), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kNoRunModuleProgram, m_noModuleProgram), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kNoRecurse, m_noRecurse), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckValidEdits, m_checkValidEdits), 0L);

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kRevList, 0L);
		const std::vector<string> & list = gRevNames.GetList();
		std::vector<string>::const_iterator i;
		for(i = list.begin(); i != list.end(); ++i)
		{
			UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kRevList, (void *)(const char *)(*i).c_str());
		}
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevList, m_forceRevision), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kBrowseRev, m_forceRevision), 0L);
	}
	else
	{
		m_forceCommit = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceCommit, 0L);
		m_forceRecurse = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRecurse, 0L);
		m_forceRevision = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRevision, 0L);
		m_noModuleProgram = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kNoRunModuleProgram, 0L);
		m_noRecurse = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kNoRecurse, 0L);
		m_checkValidEdits = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckValidEdits, 0L);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kRevList, &m_rev);
	}
}

ev_msg int UCvsCommit_OPTIONS::OnBrowseRevision(void)
{
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName) )
	{
		m_rev = (const char*)tagName.c_str();
		if( !m_rev.empty())
		{
			UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kRevList, (void *)(const char *)tagName.c_str());
			int sel = UEventSendMessage(GetWidID(), EV_COMBO_GETCOUNT, kRevList, 0L);
			UEventSendMessage(GetWidID(), EV_COMBO_SETSEL, UMAKEINT(kRevList, sel-1), 0L);
		}
	}
	return 0;
}

ev_msg int UCvsCommit_OPTIONS::OnForceHit(void)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRevision, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevList, state), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kBrowseRev, state), 0L);
	return 0;
}

void UCvsCommit_OPTIONS::OnRevisionSel(int pos, const char *txt)
{
	m_rev = txt;
}


#endif // qUnix

/*!
	Get the commit options
*/
bool CompatGetCommit(const MultiFiles* mf,
					 std::string& commit, bool& norecurs, 
					 bool& forceCommit, bool& forceRecurse, std::string& rev, bool& noModuleProgram,
					 bool& checkValidEdits)
{
	bool userHitOK = false;
	
	static string sLastMsg;
	static string sLastRev;
	
	bool hasRev = false;
	
	norecurs = false;
	commit = "";
	forceCommit = false;
	forceRecurse = false;
	rev = "";
	noModuleProgram = false;
	checkValidEdits = (bool)gCheckValidEdits;
	CMString::list_t CommitLogsList = gCommitLogs.GetList();
	
#ifdef WIN32
	CHHelpPropertySheet pages("Commit settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	CCommit_MAIN page1(mf, sLastMsg.c_str());
	CCommit_OPTIONS page2(mf);
	CGetPrefs_GLOBALS page3;

	pages.AddPage(&page1);
	pages.AddPage(&page2);
	pages.AddPage(&page3);

	if( pages.DoModal() == IDOK )
	{
		commit = page1.GetLogMsg();

		norecurs = page2.m_noRecurse ? true : false;
		forceCommit = page2.m_forceCommit ? true : false;
		forceRecurse = page2.m_forceRecurse ? true : false;
		hasRev = page2.m_forceRevision ? true : false;
		sLastRev = page2.m_rev;
		noModuleProgram = page2.m_noModuleProgram ? true : false;
		gCheckValidEdits = page2.m_checkValidEdits ? true : false;
		
		page3.StoreValues();

		userHitOK = true;
	}

	sLastMsg = (const char*)page1.GetLogMsg();
#endif /* WIN32 */

#ifndef WIN32
	// Redundant, to be phased out since it can be extracted in the dialog itself
	const char* pathTmpl = 0L;
	string tmpl;
	if( mf->NumDirs() > 0 )
	{
		if( mf->getdir(0, tmpl) )
		{
			pathTmpl = tmpl.c_str();
		}
	}
#endif

#ifdef qMacCvsPP
	StDialogHandler	theHandler(dlg_Commit, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	static UInt16 sRuntimePanel = 1;
	UInt16      msgEncoding(sCommitMsgEncoding);
	
	LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
		(theDialog->FindPaneByID(item_MultiViewPrefs));
	LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
	
	multiView->SwitchToPanel(3);
	DoDataExchange_Globals(theDialog, true);
	multiView->SwitchToPanel(2);
	DoDataExchange_CommitOptions(theDialog, mf, norecurs, forceCommit, forceRecurse, hasRev, sLastRev, noModuleProgram, true);
	multiView->SwitchToPanel(1);
	DoDataExchange_Commit(theDialog, sLastMsg, pathTmpl, msgEncoding, true);

	LPopupButton *previewDiff = dynamic_cast<LPopupButton*>
								(theDialog->FindPaneByID(item_PreviewDiffPopup));
	LPushButton *previewBtn = dynamic_cast<LPushButton*>
								(theDialog->FindPaneByID(item_PreviewDiffBtn));
	previewBtn->AddListener(&theHandler);
	previewDiff->DeleteAllMenuItems();
	if( mf && mf->TotalNumFiles() )
	{
		MultiFiles::const_iterator mfi;
		for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
		{
			for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
			{
				string fullPath;
				mfi->GetFullPath(nIndex, fullPath);
				
				previewDiff->InsertMenuItem(LStr255(fullPath), ::CountMenuItems(previewDiff->GetMacMenuH()));
			}
		}
	}
	else
	{
		previewDiff->Disable();
		previewBtn->Disable();
	}

	groupView->SetValue(sRuntimePanel);
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
		
		if(hitMessage == item_PreviewDiffBtn)
		{
			int file = previewDiff->GetCurrentMenuItem(), count = 1;
			MultiFiles::const_iterator mfi;
			for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
			{
				for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++, count++)
				{
					if(count == file)
					{
						string path;
						string fileName, currRev;
						mfi->get(nIndex, path, fileName, currRev);
						
						MultiFiles mf;
						mf.newdir(path);
						mf.newfile(fileName, 0, currRev);
						
						KoDiffHandler handler;
						handler.OnFiles(&mf);
						break;
					}
				}
			}
		}
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
		DoDataExchange_Commit(theDialog, sLastMsg, pathTmpl, msgEncoding, false);
		multiView->SwitchToPanel(2);
		DoDataExchange_CommitOptions(theDialog, mf, norecurs, forceCommit, forceRecurse, hasRev, sLastRev, noModuleProgram, false);
		multiView->SwitchToPanel(3);
		DoDataExchange_Globals(theDialog, false);
		sCommitMsgEncoding = msgEncoding;
		if ( msgEncoding == kMsgEncodingUTF8 ) {
  		commit = sLastMsg;
		} else {
		  commit = ConvertString(sLastMsg, kCFStringEncodingUTF8, msgEncoding == kMsgEncodingMacOSRoman ? kCFStringEncodingMacRoman : kCFStringEncodingISOLatin1, true);
		}
		commit.replace('\015', '\012');
		userHitOK = true;
	}
	else
	{
		UInt16 uDummy;
		DoDataExchange_Commit(theDialog, sLastMsg, NULL, uDummy, false);
	}
#endif /* qMacCvsPP */
#if qUnix
	void *wid = UCreate_CommitDlg();

	UCvsCommit *dlg = new UCvsCommit();
	UCvsCommit_MAIN *tab1 = new UCvsCommit_MAIN( mf, sLastMsg.c_str(), pathTmpl);
	UCvsCommit_OPTIONS *tab2 = new UCvsCommit_OPTIONS( mf);

	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsCommit::kTabGeneral, 0);
	dlg->AddPage(tab2, UCvsCommit::kTabGeneral, 1);

	if(dlg->DoModal())
	{
		sLastMsg = tab1->m_logmsg;
		norecurs = tab2->m_noRecurse;
		forceCommit = tab2->m_forceCommit;
		forceRecurse = tab2->m_forceRecurse;
		hasRev = tab2->m_forceRevision;
		sLastRev = tab2->m_rev;
		noModuleProgram = tab2->m_noModuleProgram;
		gCheckValidEdits = tab2->m_checkValidEdits;

		userHitOK = true;
	}

	// TODO: Verify if the message is saved when the user hits Cancel
	// Expected behaviour: In Commit dialog, enter a message and leave it without
	// committing; the text entered still should be in the commit dialog the next time
	// it is opened.
	commit = tab1->m_logmsg;

	delete dlg;
#endif // qUnix

	if( userHitOK )
	{
		if( commit.length() == 0 )
			commit = "no message";
		
		if( hasRev && !sLastRev.empty() )
			rev = sLastRev;

		checkValidEdits = (bool)gCheckValidEdits;

		gCvsPrefs.save();
	}
	else
		gCommitLogs.SetList(CommitLogsList);

	CommitLogsList.clear();

	return userHitOK;
}
