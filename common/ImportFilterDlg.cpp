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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * ImportFilterDlg.cpp - edit filtered import
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef qMacCvsPP
#	include <Icons.h>
#include "FSRefGetFileIcon.h"

#	include <UModalDialogs.h>
#	include <LTableMultiGeometry.h>
#	include <LOutlineKeySelector.h>
#	include <LTableSingleSelector.h>
#	include <LOutlineRowSelector.h>
#	include <UAttachments.h>
#	include <UGAColorRamp.h>
#	include <LTextEditView.h>

#	include "MacCvsConstant.h"
#endif /* qMacCvsPP */

#include "ImportFilterDlg.h"
#include "AppConsole.h"

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */


/// Columns enum
enum
{
	COL_STATE = 0,	/*!< State */
	COL_DESC,		/*!< Description */
	COL_KIND,		/*!< Kind */
	COL_NUM			/*!< Number of columns */
};

	
/// Column labels
static char* _gszColumnLabel[COL_NUM] =
{
	"Entry state", "Entry description", "Entry kind"
};

/// Column widths
static int _gnColumnWidth[COL_NUM] = 
{
#if qUnix
	80, 270, 80
#else
	80, 270, 140
#endif
};


#ifdef WIN32

//////////////////////////////////////////////////////////////////////////
// CImportWarning

CImportWarning::CImportWarning(ReportWarning* warn, CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CImportWarning::IDD, pParent)
{
	string print;
	warn->PrintOut(print);

	//{{AFX_DATA_INIT(CImportWarning)
	m_desc = print.c_str();
	//}}AFX_DATA_INIT
}


void CImportWarning::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportWarning)
	DDX_Text(pDX, IDC_WARNING_DESC, m_desc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportWarning, CHHelpDialog)
	//{{AFX_MSG_MAP(CImportWarning)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportWarning message handlers

void CImportWarning::InitHelpID()
{
	SetCtrlHelpID(IDC_WARNING_DESC,	IDH_IDC_WARNING_DESC);
}

/// BN_CLICKED message handler, display dialog help
void CImportWarning::OnHelp() 
{
	ShowHelp();
}

//////////////////////////////////////////////////////////////////////////
// CImportEntry

CImportEntry::CImportEntry(ReportConflict* entry, CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CImportEntry::IDD, pParent)
{
	string print;
	entry->PrintOut(print);

	//{{AFX_DATA_INIT(CImportEntry)
	//}}AFX_DATA_INIT
	
	//keep these outside MFC remarks
	m_desc = print.c_str();

	if( entry->HasForceBinary() )
		m_radios = 1;
	else if( entry->HasForceUnicode() )
		m_radios = 2;
	else if( entry->HasForceText() )
		m_radios = 3;
	else if( entry->HasForceNoKeywords() )
		m_radios = 4;
	else if( entry->HasIgnore() )
		m_radios = 5;
	else
		m_radios = 0;
}


void CImportEntry::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportEntry)
	DDX_Text(pDX, IDC_CONFLICT_DESC, m_desc);
	DDX_Radio(pDX, IDC_ENTRYSTATE_NO_CHANGE, m_radios);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportEntry, CHHelpDialog)
	//{{AFX_MSG_MAP(CImportEntry)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportEntry message handlers

void CImportEntry::InitHelpID()
{
	SetCtrlHelpID(IDC_ENTRYSTATE_NO_CHANGE,		IDH_IDC_ENTRYSTATE_NO_CHANGE);
	SetCtrlHelpID(IDC_ENTRYSTATE_FORCE_BINARY,	IDH_IDC_ENTRYSTATE_FORCE_BINARY);
	SetCtrlHelpID(IDC_ENTRYSTATE_FORCE_UNICODE,	IDH_IDC_ENTRYSTATE_FORCE_UNICODE);
	SetCtrlHelpID(IDC_ENTRYSTATE_FORCE_TEXT,	IDH_IDC_ENTRYSTATE_FORCE_TEXT);
	SetCtrlHelpID(IDC_ENTRYSTATE_NO_EXPANSION,	IDH_IDC_ENTRYSTATE_NO_EXPANSION);
	SetCtrlHelpID(IDC_ENTRYSTATE_IGNORE,		IDH_IDC_ENTRYSTATE_IGNORE);
	SetCtrlHelpID(IDC_CONFLICT_DESC,			IDH_IDC_CONFLICT_DESC);
}

/// BN_CLICKED message handler, display dialog help
void CImportEntry::OnHelp() 
{
	ShowHelp();
}

/////////////////////////////////////////////////////////////////////////////
// CImportFilterDlg dialog

CImportFilterDlg::CImportFilterDlg(ReportConflict* entries, ReportWarning* warnings,
								   CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CImportFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportFilterDlg)
	m_entries = entries;
	m_warnings = warnings;
	//}}AFX_DATA_INIT
}


void CImportFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportFilterDlg)
	DDX_Control(pDX, IDC_ENTRIES_LIST, m_list);
	DDX_Control(pDX, IDC_ENTRY_EDIT, m_edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportFilterDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CImportFilterDlg)
	ON_BN_CLICKED(IDC_ENTRY_EDIT, OnEditentry)
	ON_NOTIFY(NM_DBLCLK, IDC_ENTRIES_LIST, OnDblclkListentries)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportFilterDlg message handlers

/// BN_CLICKED message handler, edit the selected entry
void CImportFilterDlg::OnEditentry() 
{
	// The user wants to edit an entry.
	// Start at the top of the list and walk through it until an item 
	// that is selected is found.
	int nItem = m_list.GetNextItem(-1, LVNI_ALL);
	while( nItem != -1 )
	{
		UINT nState = m_list.GetItemState(nItem, LVIS_SELECTED);
		if( (nState & LVIS_SELECTED) == LVIS_SELECTED )
			break;

		nItem = m_list.GetNextItem(nItem, LVNI_ALL);
	}

	if( nItem == -1 )
		return;

	int i = 0;
	ReportWarning* warn = m_warnings;
	while( warn != 0L )
	{
		if( i == nItem )
		{
			CImportWarning dlg(warn);
			dlg.DoModal();
			return;
		}

		warn = warn->next;
		i++;
	}

	ReportConflict* conf = m_entries;
	while( conf != 0L )
	{
		if( i == nItem )
		{
			CImportEntry dlg(conf);
			if( dlg.DoModal() == IDOK )
			{
				switch(dlg.m_radios)
				{
				case 0:
					conf->DisableUserSettings();
					break;
				case 1:
					conf->ForceBinary();
					break;
				case 2:
					conf->ForceUnicode();
					break;
				case 3:
					conf->ForceText();
					break;
				case 4:
					conf->ForceNoKeywords();
					break;
				case 5:
					conf->Ignore();
					break;
				}

				m_list.SetItemText(nItem, COL_KIND, conf->GetKindDesc().c_str());
			}

			return;
		}

		conf = conf->next;
		i++;
	}
}

/// BN_CLICKED message handler, edit the selected entry
void CImportFilterDlg::OnDblclkListentries(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEditentry();
	*pResult = 0;
}

/// OnInitDialog virtual override, initialize controls
BOOL CImportFilterDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	
	// Need to set up to display the little icons next to each entry
	if( m_imageList.m_hImageList == 0L )
		m_imageList.Create(IDB_IMPORT, 16, 1, RGB(255, 255, 255));

	m_list.SetImageList(&m_imageList, LVSIL_SMALL);	

    // Full row selection
	SetListCtrlExtendedStyle(&m_list, LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT | LVS_EX_LABELTIP);

	// Set up columns inside the list view
	m_list.InsertColumn(COL_STATE,	_gszColumnLabel[COL_STATE], LVCFMT_LEFT, _gnColumnWidth[COL_STATE]);
	m_list.InsertColumn(COL_DESC,	_gszColumnLabel[COL_DESC], LVCFMT_LEFT, _gnColumnWidth[COL_DESC]);	
	m_list.InsertColumn(COL_KIND,	_gszColumnLabel[COL_KIND], LVCFMT_LEFT, _gnColumnWidth[COL_KIND]);

	int i = 0;
	ReportWarning* warn = m_warnings;
	while( warn != 0L )
	{
		// Store this information in the list view
		m_list.InsertItem(i, "", warn->IsError() ? 0 : 1);

		m_list.SetItemText(i, COL_DESC, warn->GetDesc().c_str());
		m_list.SetItemText(i, COL_STATE, warn->GetStateDesc().c_str());

		warn = warn->next;
		i++;
	}

	ReportConflict* conf = m_entries;
	while( conf != 0L )
	{
		// Store this information in the list view
		m_list.InsertItem(i, "", conf->HasConflict() ? 0 : 1);

		m_list.SetItemText(i, COL_STATE, conf->GetStateDesc().c_str());
		m_list.SetItemText(i, COL_DESC, conf->GetDesc().c_str());
		m_list.SetItemText(i, COL_KIND, conf->GetKindDesc().c_str());

		conf = conf->next;
		i++;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportFilterDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_ENTRIES_LIST,	IDH_IDC_ENTRIES_LIST);
	SetCtrlHelpID(IDC_ENTRY_EDIT,	IDH_IDC_ENTRY_EDIT);
}

/// BN_CLICKED message handler, display dialog help
void CImportFilterDlg::OnHelp() 
{
	ShowHelp();
}

#endif /* WIN32 */

#ifdef qMacCvsPP
static ReportConflict * gEntries;
static ReportWarning * gWarnings;
	
COutlineTable::COutlineTable(
	LStream *inStream )
		: LOutlineTable( inStream )
{
	// set the table geometry
	LTableMultiGeometry *geom = NEW LTableMultiGeometry(this, 50, 20);
	ThrowIfNil_(geom);
	SetTableGeometry(geom);
	
	// set the table selector
	SetTableSelector(NEW LTableSingleSelector( this )/*LOutlineRowSelector*/ );
	
	// and note that we don't set the table storage....
	
	// most of the table classes not only maintain the graphical
	// representation of your data but also the data itself. But
	// LOutlineTable doesn't really do this...it mostly handles
	// graphical representation... you need to handle your data
	// maintenance elsewhere by yourself.
	
	// insert a couple columns (name and size)
	InsertCols( 3, 0, nil, nil, false );
	geom->SetColWidth(100, 1, 1);
	geom->SetColWidth(310, 2, 2);
	geom->SetColWidth(90, 3, 3);

	// Set up keyboard selection and scrolling.
	AddAttachment(NEW LOutlineKeySelector(this, msg_AnyMessage));
	AddAttachment(NEW LKeyScrollAttachment(this));

	// Try to become default commander in the window.
	if (mSuperCommander != nil)
		mSuperCommander->SetLatentSub(this);
}

COutlineTable::~COutlineTable()
{
}

void COutlineTable::FinishCreateSelf()
{
	// insert the root level of the disk into the table
	LOutlineItem *theItem = nil;
	LOutlineItem *lastItem = nil;

	ReportWarning * warn = gWarnings;
	while(warn != 0L)
	{
		// Store this information in the list view
		CWarningItem *theItem = NEW CWarningItem(warn);
		ThrowIfNil_(theItem);
		
		// and insert it at the end of the table
		InsertItem( theItem, nil, lastItem );
		lastItem = theItem;

		warn = warn->next;
	}

	ReportConflict * conf = gEntries;
	while(conf != 0L)
	{
		// Store this information in the list view
		CEntryItem *theItem = NEW CEntryItem(conf);
		ThrowIfNil_(theItem);
		
		// and insert it at the end of the table
		InsertItem( theItem, nil, lastItem );
		lastItem = theItem;

		conf = conf->next;
	}
}

CWarningItem::CWarningItem(ReportWarning * entry)
{
	mEntry = entry;
}

CWarningItem::~CWarningItem()
{
}

// this is the routine called to know what to draw within the
// table cell. See the comments in LOutlineItem.cp for more info.
void
CWarningItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	switch (inCell.col)
	{
	case 1:
	{
        	bool isError = mEntry->Kind() != kFileIsOK && mEntry->Kind() != kTextEscapeChar;
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outHasIcon = true;
		
		if(isError)
		{
		  CIconRef    errIcon;
      if ( ::GetSysIcon( kAlertStopIcon, kSelectorAllAvailableData, errIcon ) == noErr )
        ioDrawContents.outIconSuite = errIcon.GetIconSuite();
      else
		    ioDrawContents.outHasIcon = false;
			LString::CopyPStr("\pError", ioDrawContents.outTextString);
			ioDrawContents.outTextTraits.style = bold;
		}
		else
		{
		  CIconRef    warnIcon;
      if ( ::GetSysIcon( kAlertCautionIcon, kSelectorAllAvailableData, warnIcon ) == noErr )
        ioDrawContents.outIconSuite = warnIcon.GetIconSuite();
      else
		    ioDrawContents.outHasIcon = false;
			LString::CopyPStr("\pWarning", ioDrawContents.outTextString);
			ioDrawContents.outTextTraits.style = 0;
		}
	}
	break;
        case 2:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outTextTraits.style = 0;
		
		LString::CopyPStr(mEntry->GetDesc(), ioDrawContents.outTextString);
		break;
	case 3:
		ioDrawContents.outShowSelection = false;
		break;
	}
}

// just to be cute, we'll draw an adornment (again, see the LOutlineItem.cp
// comments for more information). We'll draw a groovy gray background
void
CWarningItem::DrawRowAdornments(
	const Rect&		inLocalRowRect )
{
	ShadeRow(UGAColorRamp::GetColor(0), inLocalRowRect);
}

void CWarningItem::EditImportCell(void)
{
	StDialogHandler	theHandler(dlg_ImpFilterWarn, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	
	LTextEditView *warn = dynamic_cast<LTextEditView*>
		(theDialog->FindPaneByID(item_ImpEditWarn));
	std::string print;
	mEntry->PrintOut(print);
	warn->SetTextPtr((Ptr)(const char *)print, print.length());	
	
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
	}
	theDialog->Hide();
}

void
CWarningItem::DoubleClick(
	const STableCell&			/* inCell */,
	const SMouseDownEvent&		/* inMouseDown */,
	const SOutlineDrawContents&	/* inDrawContents */,
	Boolean						/* inHitText */)
{
	EditImportCell();
}

CEntryItem::CEntryItem( ReportConflict * entry)
{
	mEntry = entry;
}

CEntryItem::~CEntryItem()
{
}

// this is the routine called to know what to draw within the
// table cell. See the comments in LOutlineItem.cp for more info.
void
CEntryItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	CPStr str;
	
	switch (inCell.col)
	{
	case 1:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outHasIcon = true;
		
		if(mEntry->HasConflict())
		{
			str << "Conflict";
			ioDrawContents.outTextTraits.style = bold;
		  CIconRef    icon;
		  if ( ::GetSysIcon(kAlertStopIcon, kSelectorAllAvailableData, icon) == noErr )
		    ioDrawContents.outIconSuite = icon.GetIconSuite();
		  else
		    ioDrawContents.outHasIcon = false;
		}
		else if(mEntry->HasTypeCreatorConflict())
		{
			str << "Warning";
			ioDrawContents.outTextTraits.style = 0;
		  CIconRef    icon;
		  if ( ::GetSysIcon(kAlertCautionIcon, kSelectorAllAvailableData, icon) == noErr )
		    ioDrawContents.outIconSuite = icon.GetIconSuite();
		  else
		    ioDrawContents.outHasIcon = false;
		}
		else
		{
			str << "OK";
			ioDrawContents.outTextTraits.style = 0;
		  CIconRef    icon;
		  if ( ::GetSysIcon(kAlertNoteIcon, kSelectorAllAvailableData, icon) == noErr )
		    ioDrawContents.outIconSuite = icon.GetIconSuite();
		  else
		    ioDrawContents.outHasIcon = false;
		}
		LString::CopyPStr(str, ioDrawContents.outTextString);
		break;

	case 2:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outHasIcon = false/*true*/;
		ioDrawContents.outIconSuite = 0L/*mIconH*/;
		ioDrawContents.outTextTraits.style = 0;
		
		str << mEntry->GetDesc();
		LString::CopyPStr(str, ioDrawContents.outTextString);
		break;

	case 3:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outHasIcon = false;
		ioDrawContents.outTextTraits.style = 0;
		
		if(mEntry->HasForceBinary())
		{
			LString::CopyPStr("\pFORCE BINARY", ioDrawContents.outTextString);
			ioDrawContents.outTextTraits.style = bold;
		}
		else if(mEntry->HasForceText())
		{
			LString::CopyPStr("\pFORCE TEXT", ioDrawContents.outTextString);
			ioDrawContents.outTextTraits.style = bold;
		}
		else if(mEntry->HasIgnore())
		{
			LString::CopyPStr("\pIGNORE", ioDrawContents.outTextString);
			ioDrawContents.outTextTraits.style = bold;
		}
		else if(mEntry->IsBinary())
		{
			LString::CopyPStr("\pBINARY", ioDrawContents.outTextString);
			ioDrawContents.outTextTraits.style = 0;
		}
		else
		{
			LString::CopyPStr("\pTEXT", ioDrawContents.outTextString);
			ioDrawContents.outTextTraits.style = 0;
		}
		break;
	}
}

// just to be cute, we'll draw an adornment (again, see the LOutlineItem.cp
// comments for more information). We'll draw a groovy gray background
void
CEntryItem::DrawRowAdornments(
	const Rect&		inLocalRowRect )
{
	ShadeRow(UGAColorRamp::GetColor(0), inLocalRowRect);
}

static void DoDataExchange_ImpEditEntry(LWindow *theDialog, ReportConflict * mEntry, bool putValue)
{
	LTextEditView *warn = dynamic_cast<LTextEditView*>
		(theDialog->FindPaneByID(item_ImpEditWarn));
	
	if(putValue)
	{
		if(mEntry->HasForceBinary())
			theDialog->SetValueForPaneID (item_ImpEntryFBin, Button_On);
		else if(mEntry->HasForceText())
			theDialog->SetValueForPaneID (item_ImpEntryFText, Button_On);
		else if(mEntry->HasIgnore())
			theDialog->SetValueForPaneID (item_ImpEntryFIgn, Button_On);
		else
			theDialog->SetValueForPaneID (item_ImpEntryUnch, Button_On);
		std::string print;
		mEntry->PrintOut(print);
		warn->SetTextPtr((Ptr)(const char *)print, print.length());	
	}
	else
	{
		if(theDialog->GetValueForPaneID (item_ImpEntryFBin) == Button_On)
			mEntry->ForceBinary();
		else if(theDialog->GetValueForPaneID (item_ImpEntryFText) == Button_On)
			mEntry->ForceText();
		else if(theDialog->GetValueForPaneID (item_ImpEntryFIgn) == Button_On)
			mEntry->Ignore();
		else
			mEntry->DisableUserSettings();
	}
}

void CEntryItem::EditImportCell(void)
{
	StDialogHandler	theHandler(dlg_ImpFilterEntry, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	
	DoDataExchange_ImpEditEntry(theDialog, mEntry, true);
	
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
	}
	theDialog->Hide();
	
	if(hitMessage == msg_OK)
	{
		DoDataExchange_ImpEditEntry(theDialog, mEntry, false);
		mOutlineTable->Refresh();
	}
}

void
CEntryItem::DoubleClick(
	const STableCell&			/* inCell */,
	const SMouseDownEvent&		/* inMouseDown */,
	const SOutlineDrawContents&	/* inDrawContents */,
	Boolean						/* inHitText */)
{
	EditImportCell();
}
#endif /* qMacCvsPP */

#if qUnix
class UCvsImpWarning : public UWidget
{
	UDECLARE_DYNAMIC(UCvsImpWarning)
public:
	UCvsImpWarning(ReportWarning * warn);
	virtual ~UCvsImpWarning() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kDesc					// 1
	};

	virtual void DoDataExchange(bool fill);

protected:
	std::string m_desc;

	ev_msg int OnOK(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsImpWarning, UWidget)

UBEGIN_MESSAGE_MAP(UCvsImpWarning, UWidget)
	ON_UCOMMAND(UCvsImpWarning::kOK, UCvsImpWarning::OnOK)
UEND_MESSAGE_MAP()

UCvsImpWarning::UCvsImpWarning(ReportWarning * warn) : UWidget(::UEventGetWidID())
{
	warn->PrintOut(m_desc);
}

int UCvsImpWarning::OnOK(void)
{
	EndModal(true);
	return 0;
}

void UCvsImpWarning::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kDesc, (void *)(const char *)m_desc.c_str());
	}
	else
	{
	}
}

class UCvsImpEntry : public UWidget
{
	UDECLARE_DYNAMIC(UCvsImpEntry)
public:
	UCvsImpEntry(ReportConflict * entry);
	virtual ~UCvsImpEntry() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kRadUnchanged,			// 2
		kRadBinary,				// 3
		kRadText,				// 4
		kRadIgnore,				// 5
		kDesc					// 6
	};

	virtual void DoDataExchange(bool fill);

	int m_radios;
protected:
	std::string m_desc;

	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);
	ev_msg int OnRadios(int cmd);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsImpEntry, UWidget)

UBEGIN_MESSAGE_MAP(UCvsImpEntry, UWidget)
	ON_UCOMMAND(UCvsImpEntry::kOK, UCvsImpEntry::OnOK)
	ON_UCOMMAND(UCvsImpEntry::kCancel, UCvsImpEntry::OnCancel)
	ON_UCOMMAND_RANGE(UCvsImpEntry::kRadUnchanged, UCvsImpEntry::kRadIgnore, UCvsImpEntry::OnRadios)
UEND_MESSAGE_MAP()

UCvsImpEntry::UCvsImpEntry(ReportConflict * entry) : UWidget(::UEventGetWidID())
{
	entry->PrintOut(m_desc);

	if(entry->HasForceBinary())
		m_radios = 1;
	else if(entry->HasForceText())
		m_radios = 2;
	else if(entry->HasIgnore())
		m_radios = 3;
	else
		m_radios = 0;
}

int UCvsImpEntry::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsImpEntry::OnCancel(void)
{
	EndModal(false);
	return 0;
}

int UCvsImpEntry::OnRadios(int cmd)
{
	m_radios = cmd - kRadUnchanged;
	return 0;
}

void UCvsImpEntry::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kDesc, (void *)(const char *)m_desc.c_str());
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kRadUnchanged + m_radios, 1), 0L);
	}
	else
	{
	}
}

#if qGTK
static void *sWarningIcon;
static void *sErrorIcon;
#endif

class UCvsImpFilter : public UWidget
{
	UDECLARE_DYNAMIC(UCvsImpFilter)
public:
	UCvsImpFilter(ReportConflict * entries, ReportWarning * warnings);
	virtual ~UCvsImpFilter() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kList,					// 2
		kEdit					// 3
	};

	virtual void DoDataExchange(bool fill);

protected:
	ReportConflict *m_entries;
	ReportWarning *m_warnings;

	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);
	ev_msg int OnEdit(void);
	ev_msg void OnDblClick(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsImpFilter, UWidget)

UBEGIN_MESSAGE_MAP(UCvsImpFilter, UWidget)
	ON_UCOMMAND(UCvsImpFilter::kOK, UCvsImpFilter::OnOK)
	ON_UCOMMAND(UCvsImpFilter::kCancel, UCvsImpFilter::OnCancel)
	ON_UCOMMAND(UCvsImpFilter::kEdit, UCvsImpFilter::OnEdit)
	ON_LIST_DBLCLICK(UCvsImpFilter::kList, UCvsImpFilter::OnDblClick)
UEND_MESSAGE_MAP()

UCvsImpFilter::UCvsImpFilter(ReportConflict * entries, ReportWarning * warnings) :
	UWidget(::UEventGetWidID())
{
	m_entries = entries;
	m_warnings = warnings;
}

int UCvsImpFilter::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsImpFilter::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsImpFilter::OnDblClick(void)
{
	OnEdit();
}

void UCvsImpFilter::DoDataExchange(bool fill)
{
	if(sWarningIcon == 0L)
	{
		sWarningIcon = UCreate_pixmap(this, "warning.xpm");
		sErrorIcon = UCreate_pixmap(this, "error.xpm");
	}

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kList, 0), 0L);
		UEventSendMessage(GetWidID(), EV_LIST_ADDCOLUMNS, UMAKEINT(kList, COL_NUM), 0L);
		for(int i = 0; i < COL_NUM; i++)
		{
			UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kList, i),
							  _gszColumnLabel[i]);
			UEventSendMessage(GetWidID(), EV_LIST_SETCOLWIDTH, UMAKEINT(kList, i),
							  (void *)_gnColumnWidth[i]);
		}
		UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kList, 1), 0L);

		UEventSendMessage(GetWidID(), EV_LIST_RESETALL, kList, 0L);

		ReportWarning * warn = m_warnings;
		while(warn != 0L)
		{
			int rownum;
			UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kList, &rownum);
			UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kList, rownum), warn);
			ULIST_INSERT entry;
			entry.row = rownum;
			
			entry.col = 0;
			entry.title = warn->GetStateDesc().c_str();
			entry.icon = 0L;
#if qGTK
			entry.icon = warn->IsError() ? sErrorIcon : sWarningIcon;
#endif
			UEventSendMessage(GetWidID(), EV_LIST_INSERT, kList, &entry);

			entry.col = 1;
			entry.title = warn->GetDesc().c_str();
			entry.icon = 0L;
			UEventSendMessage(GetWidID(), EV_LIST_INSERT, kList, &entry);

			warn = warn->next;
		}
		
		ReportConflict * conf = m_entries;
		while(conf != 0L)
		{
			int rownum;
			UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kList, &rownum);
			UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kList, rownum), conf);
			ULIST_INSERT entry;
			entry.row = rownum;
			
			entry.col = 0;
			entry.title = conf->GetStateDesc().c_str();
			entry.icon = 0L;
#if qGTK
			entry.icon = conf->HasConflict() ? sErrorIcon : sWarningIcon;
#endif
			UEventSendMessage(GetWidID(), EV_LIST_INSERT, kList, &entry);

			entry.col = 1;
			entry.title = conf->GetDesc().c_str();
			entry.icon = 0L;
			UEventSendMessage(GetWidID(), EV_LIST_INSERT, kList, &entry);

			entry.col = 2;
			entry.title = conf->GetKindDesc().c_str();
			entry.icon = 0L;
			UEventSendMessage(GetWidID(), EV_LIST_INSERT, kList, &entry);
			
			conf = conf->next;
		}
	}
	else
	{
	}
}

int UCvsImpFilter::OnEdit() 
{
	int row = -1;
	if((row = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL,
								UMAKEINT(kList, row), 0L)) == -1)
		return 0;

	ULIST_INFO info;
	info.col = 0;
	info.row = row;
	UEventSendMessage(GetWidID(), EV_LIST_GETINFO, kList, &info);

	ReportWarning * warn = m_warnings;
	while(warn != 0L)
	{
		if(warn == info.data)
		{
			void *wid = UCreate_ImpWarningDlg();
			UCvsImpWarning *dlg = new UCvsImpWarning(warn);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			dlg->DoModal();
			delete dlg;
			return 0;
		}
		warn = warn->next;
	}

	ReportConflict * conf = m_entries;
	while(conf != 0L)
	{
		if(conf == info.data)
		{
			void *wid = UCreate_ImpEntryDlg();
			UCvsImpEntry *dlg = new UCvsImpEntry(conf);
			UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
			if(dlg->DoModal())
			{
				ULIST_INSERT entry;
				entry.row = row;
			
				entry.col = 2;
				entry.title = 0L;
				entry.icon = /*UCvsFiles::GetImageForEntry(data)*/0L;
				switch(dlg->m_radios)
				{
				case 0:
					conf->DisableUserSettings();
					break;
				case 1:
					conf->ForceBinary();
					break;
				case 2:
					conf->ForceText();
					break;
				case 3:
					conf->Ignore();
					break;
				}

				entry.title = conf->GetKindDesc().c_str();

				if( entry.title != 0L )
					UEventSendMessage(GetWidID(), EV_LIST_INSERT, kList, &entry);
			}
			delete dlg;
			return 0;
		}

		conf = conf->next;
	}
	return 0;
}
#endif /* qUnix */

/// Get the import filter options
bool CompatImportFilter(const char* path, ReportConflict*& entries, ReportWarning*& warnings)
{
	bool result = false;

	bool searched = false;

	if( !entries && !warnings )
	{
		searched = true;

		const kTraversal traversalResult = list_all_types_recur(path, warnings, entries);
		if( traversalResult != kContinueTraversal )
		{
			cvs_err("Import filtering aborted on user's request.\n");
			
			free_list_types(warnings, entries);
			entries = 0L;
			warnings = 0L;
			
			return false;
		}
	}

	if( entries == 0L && warnings == 0L )
	{
		cvs_err("Import filtering aborted - nothing to import.\n");
		return false;
	}

#ifdef WIN32
	CImportFilterDlg pages(entries, warnings);
	if( pages.DoModal() == IDOK )
	{
		result = true;
	}
	else
	{
		if( searched )
		{
			cvs_err("Import filtering aborted.\n");
		}
	}
#endif /* WIN32 */
#ifdef qMacCvsPP
	gWarnings = warnings;
	gEntries = entries;
	
	StDialogHandler	theHandler(dlg_ImpFilter, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	
	theDialog->Show();
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if(hitMessage == item_ImpPushEdit)
		{
			// simulate dbl click
			LOutlineTable *tab = dynamic_cast<LOutlineTable*>
				(theDialog->FindPaneByID(item_ImpFilterList));
			
			STableCell	cell = tab->GetFirstSelectedCell();
			if(!cell.IsNullCell())
			{
				//TableIndexT index;
				//tab->CellToIndex(cell, index);
				CImportItem *item = dynamic_cast<CImportItem*>(tab->FindItemForRow(cell.row));
				Assert_(item != 0L);
				if(item != 0L)
					item->EditImportCell();
			}
		}
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
	}
	theDialog->Hide();
	
	if(hitMessage == msg_OK)
	{
		result = true;
	}
#endif /* qMacCvsPP */
#if qUnix
	void *wid = UCreate_ImportFilterDlg();

	UCvsImpFilter *dlg = new UCvsImpFilter(entries, warnings);
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	

	if(dlg->DoModal())
	{
		result = true;
	}

	delete dlg;
#endif // qUnix

	if( !result && searched )
	{
		free_list_types(warnings, entries);
		entries = 0L;
		warnings = 0L;
	}

	return result;
}
