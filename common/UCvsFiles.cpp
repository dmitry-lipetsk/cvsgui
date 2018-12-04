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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

/*
 * 
 */

#include "stdafx.h"

#if qGTK
#	include <gtk/gtk.h>
#	include "support.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <string>

#include "BrowseViewModel.h"
#include "BrowseViewColumn.h"
#include "UCvsFiles.h"
#include "UCvsDialogs.h"
#include "UCvsFolders.h"
#include "UCvsCommands.h"
#include "UCvsFrame.h"
#include "UCvsApp.h"
#include "CvsPrefs.h"
#include "FileTraversal.h"
#include "MultiFiles.h"
#include "CvsCommands.h"
#include "CvsArgs.h"
#include "TclGlue.h"
#ifdef USE_PYTHON
#	include "PythonGlue.h"
#endif
#include "AppConsole.h"
#include "MoveToTrash.h"
#include "MacrosSetup.h"

#define CBrowseFileView UCvsFiles

UIMPLEMENT_DYNAMIC(UCvsFiles, UWidget)

UBEGIN_MESSAGE_MAP(UCvsFiles, UWidget)
	ON_UUPDATECMD(cmdUPDATE, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdCOMMIT, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdDIFF, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdEDIT, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdRESEDIT, UCvsFiles::OnCmdUIResEdit)
	ON_UUPDATECMD(cmdEDITORS, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdLOCKF, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdLOG, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdTAGNEW, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdTAGDELETE, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdTAGBRANCH, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdSTATUS, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdUNEDIT, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdQUERYUPDATE, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdUNLOCKF, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdWATCHERS, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdWATCHON, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdWATCHOFF, UCvsFiles::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdADD, UCvsFiles::OnCmdUIAdd)
	ON_UUPDATECMD(cmdADDB, UCvsFiles::OnCmdUIAddB)
	ON_UUPDATECMD(cmdRELEASE, UCvsFiles::OnCmdUIRelease)
	ON_UUPDATECMD(cmdRMV, UCvsFiles::OnCmdUIRmv)
	ON_UUPDATECMD(cmdGRAPH, UCvsFiles::OnCmdUIGraph)
	ON_UUPDATECMD(cmdEDITSELDEF, UCvsFiles::OnCmdUIEditseldef)
	ON_UUPDATECMD(cmdUPFOLDER, UCvsFiles::OnCmdUIUpone)
	ON_UUPDATECMD(cmdTRASH, UCvsFiles::OnCmdUITrash)
	ON_UUPDATECMD(cmdEXPLORE, UCvsFiles::OnCmdUIExplore)
	ON_UUPDATECMD(cmdEDITSEL, UCvsFiles::OnCmdUITrash)
#ifdef USE_PYTHON
	ON_UUPDATECMD_RANGE(cmdPyMACROS, cmdPyMACROSEND, UCvsFiles::OnCmdUIMacroSel)
#else	
	ON_UUPDATECMD_RANGE(cmdSELMACRO, cmdSELMACROEND, UCvsFiles::OnCmdUIMacroSel)
#endif
	ON_UUPDATECMD(cmdFLAT, UCvsFiles::OnCmdUIViewRecursive)
	ON_UCOMMAND(cmdUPDATE, UCvsFiles::OnCmdUpdate)
	ON_UCOMMAND(cmdADD, UCvsFiles::OnCmdAdd)
	ON_UCOMMAND(cmdADDB, UCvsFiles::OnCmdAddb)
	ON_UCOMMAND(cmdCOMMIT, UCvsFiles::OnCmdCommit)
	ON_UCOMMAND(cmdRMV, UCvsFiles::OnCmdRmv)
	ON_UCOMMAND(cmdQUERYUPDATE, UCvsFiles::OnCmdQueryUpdate)
	ON_UCOMMAND(cmdUPFOLDER, UCvsFiles::OnCmdUpone)
	ON_UCOMMAND(cmdTRASH, UCvsFiles::OnCmdTrash)
	ON_UCOMMAND(cmdDIFF, UCvsFiles::OnCmdDiff)
	ON_UCOMMAND(cmdLOG, UCvsFiles::OnCmdLog)
	ON_UCOMMAND(cmdGRAPH, UCvsFiles::OnCmdGraph)
	ON_UCOMMAND(cmdSTATUS, UCvsFiles::OnCmdStatus)
	ON_UCOMMAND(cmdLOCKF, UCvsFiles::OnCmdLock)
	ON_UCOMMAND(cmdUNLOCKF, UCvsFiles::OnCmdUnlock)
	ON_UCOMMAND(cmdWATCHON, UCvsFiles::OnCmdWatchOn)
	ON_UCOMMAND(cmdWATCHOFF, UCvsFiles::OnCmdWatchOff)
	ON_UCOMMAND(cmdEDIT, UCvsFiles::OnCmdEdit)
	ON_UCOMMAND(cmdRESEDIT, UCvsFiles::OnCmdResEdit)
	ON_UCOMMAND(cmdUNEDIT, UCvsFiles::OnCmdUnedit)
	ON_UCOMMAND(cmdWATCHERS, UCvsFiles::OnCmdWatchers)
	ON_UCOMMAND(cmdEDITORS, UCvsFiles::OnCmdEditors)
	ON_UCOMMAND(cmdRELEASE, UCvsFiles::OnCmdRelease)
	ON_UCOMMAND(cmdTAGNEW, UCvsFiles::OnCmdTagNew)
	ON_UCOMMAND(cmdTAGDELETE, UCvsFiles::OnCmdTagDelete)
	ON_UCOMMAND(cmdTAGBRANCH, UCvsFiles::OnCmdTagBranch)
	ON_UCOMMAND(cmdEXPLORE, UCvsFiles::OnCmdExplore)
	ON_UCOMMAND(cmdEDITSEL, UCvsFiles::OnCmdEditsel)
	ON_UCOMMAND(cmdEDITSELDEF, UCvsFiles::OnCmdEditseldef)
	ON_UCOMMAND(cmdFLAT, UCvsFiles::OnViewRecursive)
#ifdef USE_PYTHON
	ON_UCOMMAND_RANGE(cmdPyMACROS, cmdPyMACROSEND, UCvsFiles::OnMacroSel)
#else
	ON_UCOMMAND_RANGE(cmdSELMACRO, cmdSELMACROEND, UCvsFiles::OnMacroSel)
#endif
	ON_LIST_DBLCLICK(kUMainWidget, UCvsFiles::OnDblClick)
	ON_LIST_POPUP(kUMainWidget, UCvsFiles::OnPopup)
	ON_LIST_SELCOLUMN(kUMainWidget, UCvsFiles::OnSelColumn)
	ON_UDESTROY(UCvsFiles)
	
	ON_UCREATE(UCvsFiles)
UEND_MESSAGE_MAP()

#if _0_
	ON_UUPDATECMD(cmdSMALLICONS, OnCmdUISmallIcons)
	ON_UUPDATECMD(cmdFULLLIST, OnCmdUIList)
	ON_UUPDATECMD(cmdROWDETAILS, OnCmdUIFullRowDetails)
	ON_COMMAND(ID_VIEW_SMALLICONS, OnViewSmallIcons)
	ON_COMMAND(ID_VIEW_FULLLIST, OnViewList)
	ON_COMMAND(ID_VIEW_ROWDETAILS, OnViewFullRowDetails)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_WM_TIMER()

	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
#endif

static void *sFileIconTextIcon = NULL;
static void *sFileIconBinaryIcon;
static void *sFileIconUnknownIcon;
static void *sFileIconAddedIcon;
static void *sFileIconConflictIcon;
static void *sFileIconMissIcon;
static void *sFolderIconIcon;
static void *sFolderIconUnknownIcon;
static void *sFolderIconMissIcon;
static void *sFileIconTextModIcon;
static void *sFileIconBinaryModIcon;
static void *sFileIconIgnoredIcon;
static void *sFolderIconIgnoredIcon;
static void *sFileIconRemovedIcon;

/*
kFileIconText,cvsfile
kFileIconBinary,binfile
kFileIconUnknown,unkfile
kFileIconAdded,addfile
kFileIconConflict,conflict
kFileIconMiss,missfile
kFolderIcon,folder
kFolderIconUnknown,foldunk
kFolderIconMiss,foldmiss
kFileIconTextMod,modfile
kFileIconBinaryMod,modbin
kFileIconIgnored,ignfile
kFolderIconIgnored,foldign
kFileIconRemoved,delfile
*/

enum
{
	kFileIconText = 0,
	kFileIconBinary,
	kFileIconUnknown,
	kFileIconAdded,
	kFileIconConflict,
	kFileIconMiss,
	kFolderIcon,
	kFolderIconUnknown,
	kFolderIconMiss,
	kFileIconTextMod,
	kFileIconBinaryMod,
	kFileIconIgnored,
	kFolderIconIgnored,
	kFileIconRemoved
};

#ifdef _0_
static const char * month_names[12] =
{
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"
}; 
#endif

CPersistentInt gFileViewSort("P_FileViewSort", EntnodeData::kName);
CPersistentBool gFileViewSortAsc("P_FileViewSortAsc", false);
static struct CSortParam *gSortParam = NULL;

// context for traversing
class KoTraversalContext
{
    bool m_isRecursive;
	bool m_isShowIgnored;
	KiFilterModel* m_filter;

public:
	CBrowseFileView* m_view;

	KoTraversalContext(CBrowseFileView* view)
        : m_isRecursive(false), m_isShowIgnored(false), m_filter(NULL), m_view(view)
	{
#ifdef WIN32
		CWincvsApp* app = (CWincvsApp *)AfxGetApp()
#elif qUnix
		UCvsApp* app = UCvsApp::gApp;
#elif TARGET_RT_MAC_MACHO
		CMacCvsApp* app = CMacCvsApp::gApp;
#endif
		if (app)
        {
			m_filter = app->GetFilterModel();
			m_isRecursive = app->GetRecursionModel()->IsShowRecursive();
			m_isShowIgnored = app->GetIgnoreModel()->IsShowIgnored();
        }
	}

	bool IsRecursive() const
	{
		return m_isRecursive;
	}

	bool IsShowIgnored() const
	{
		return m_isShowIgnored;
	}

	bool IsMatch(EntnodeData* data) const
	{
		return m_filter->IsMatch(data);
	}
};

class TViewFill : public TraversalReport
{
public:
	CSortList<ENTNODE> /*&*/ m_entries;

	std::vector<std::string> m_ignlist;
	const KoTraversalContext *m_context;
	bool m_isShowIgnored;

	TViewFill(const KoTraversalContext* context) :
		m_entries(200, ENTNODE::Compare), m_context(context) {}
	
	TViewFill(const KoTraversalContext* context, CSortList<ENTNODE> & entries) :
		m_entries(entries), m_context(context) {}


	virtual ~TViewFill() {}

	UCvsFiles* GetView() const
	{
		return m_context->m_view;
	}
	
	int GetListCtrl() const
	{
		return GetView()->GetWidID();
	}

	bool IsRecursive() const
	{
		return m_context->IsRecursive();
	}
	
	bool IsShowIgnored() const
	{
		return m_context->IsShowIgnored();
	} 

	// initializes and adds new item to list view
	int InitItem(EntnodeData *data, void* iImage, const char *tag = 0L)
	{
			KoColumnContext context(GetView()->GetPath().c_str());
			char buf[256];
			GetView()->GetColumnModel()->GetAt(0)->GetText(&context, data, buf, 256);
		
			int rownum;
			UEventSendMessage(GetListCtrl(), EV_LIST_NEWROW, kUMainWidget, &rownum);
			UEventSendMessage(GetListCtrl(), EV_LIST_ROWSETDATA, UMAKEINT(kUMainWidget, rownum), data);
			ULIST_INSERT entry;
			entry.row = rownum;
			entry.col = 0;
			entry.title = buf;
			entry.icon = /*UCvsFiles::GetImageForEntry(data);*/iImage;
			UEventSendMessage(GetListCtrl(), EV_LIST_INSERT, kUMainWidget, &entry);
	
			entry.icon = 0L;
			
			if(rownum >= 0) {
				data->Ref();

#if 0 /* TODO */
			lvi.state = data->IsLocked() ?
				INDEXTOSTATEIMAGEMASK(2) : INDEXTOSTATEIMAGEMASK(1);
#endif
			KiColumnModel* model = GetView()->GetColumnModel();				

			// set item text for additional columns
			int columnCount = model->GetCount();
			for(int j = 1; j < columnCount; j++)
			{
				char buf[256];
				model->GetAt(j)->GetText(&context, data, buf, 256);
					
				if(buf[0] != 0L)
				{
					entry.col = j;
					entry.title = buf;
					UEventSendMessage(GetListCtrl(), EV_LIST_INSERT, kUMainWidget, &entry);
				}
			}

			// set tag column to given special tag
			if(tag != 0L && tag[0] != '\0' && !IsRecursive()) {
				entry.col = model->GetColumn(EntnodeFile::kTag);
				entry.title = tag == 0L ? "" : (const char *)tag;
				UEventSendMessage(GetListCtrl(), EV_LIST_INSERT, kUMainWidget, &entry);
			}
		}

		return rownum;
	}
	
	virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname, const FSSpec * macspec)
	{
		Entries_Open (m_entries, fullpath);
		BuildIgnoredList(m_ignlist, fullpath);

		return kContinueTraversal;
	}

	virtual kTraversal ExitDirectory(const char *fullpath)
	{
		// add the missing files
		Entries_SetMissing(m_entries);

		int numEntries = m_entries.NumOfElements();
		for(int i = 0; i < numEntries; i++)
		{
			const ENTNODE & theNode = m_entries.Get(i);
			EntnodeData *data = theNode.Data();
			if (data->IsMissing() && m_context->IsMatch(data))
			{
				if (!IsRecursive() || data->GetType() == ENT_FILE)
				{
					InitItem(data, UCvsFiles::GetImageForEntry(data));
				}
			}
		}
		
		m_ignlist.erase(m_ignlist.begin(), m_ignlist.end());
		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char *err, int errcode)
	{
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char *fullpath)
	{
#ifdef qUnix
		// Allow to stop traversing using the escape key
		if(IsEscKeyPressed())
		{
			return kStopTraversal;
		}
#endif
		
		return kContinueTraversal;
	}

	virtual kTraversal OnDirectory(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const FSSpec * macspec)
	{
#if qUnix
		if(strcmp(name, "CVS") == 0)
#else
		if(stricmp(name, "CVS") == 0)
#endif
			return kSkipFile;

		EntnodeData *data = Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);
		if(!IsShowIgnored() && data->IsIgnored())
			return kSkipFile;

		if (IsRecursive())
		{
			if (!data->IsIgnored() && !data->IsUnknown() && !data->IsMissing())
			{
				TViewFill traverse(m_context);
				FileTraverse(fullname, traverse);
			}
			return kSkipFile;
		}

		// get the tag
		std::string subCVS;
		std::string tagName;
		subCVS = fullname;
		NormalizeFolderPath(subCVS);
		subCVS += "CVS";
		if(chdir(subCVS.c_str()) == 0)
			Tag_Open(tagName, subCVS.c_str());
		if(chdir(fullpath) != 0)
			return kTraversalError;

		InitItem(data, UCvsFiles::GetImageForEntry(data), tagName.c_str());

		return kSkipFile;
	}

	virtual kTraversal OnAlias(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const FSSpec * macspec)
	{
		return OnFile(fullpath, fullname, name, dir, macspec);
	}

	virtual kTraversal OnFile(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const FSSpec * macspec)
	{
		EntnodeData *data = Entries_SetVisited(fullpath, m_entries, name, dir, false, &m_ignlist);
		if(!IsShowIgnored() && data->IsIgnored())
			return kContinueTraversal;

		if( m_context->IsMatch(data) )
		{
			InitItem(data, UCvsFiles::GetImageForEntry(data));
		}
		
		return kContinueTraversal;
	}
};

UCvsFiles::UCvsFiles() : UWidget(kUCvsFilesID), m_entries(200, ENTNODE::Compare), m_entriesMod(0), m_entriesLogMod(0)

{
	m_sort = (int)gFileViewSort;
	m_ascendant = (bool)gFileViewSortAsc;

	SetRecursive(false);
}

UCvsFiles::~UCvsFiles()
{
	gFileViewSort = m_sort;
	gFileViewSortAsc = m_ascendant;
}

int UCvsFiles::ProcessSelectionCommand(KiSelectionHandler& handler)
{
  int nItem = -1;

  MultiFiles mf;
  mf.newdir(GetPath().c_str());
  int pathLen = GetPath().length();
  
  // first add the folders
  while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
  {
	EntnodeData *data;
	std::string buf;
	
	UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
	const char* fullpath = data->GetFullPathName(buf);
	
    if(data->GetType() == ENT_SUBDIR)
    {
		handler.OnFolder(fullpath);
    }
    else
    {
		const char* fn = data->GetFullPathName(buf) + pathLen;
		if(*fn == kPathDelimiter ) {
			fn++;
		}
		
		
		mf.newfile(fn, 0, ((*data)[EntnodeFile::kVN]).c_str());
    }
  }
  if(mf.TotalNumFiles() != 0)
  {
    handler.OnFiles(&mf);
  
    ResetView(handler.GetNeedViewReset(), false);
  }
  
  return 0;
}

void UCvsFiles::OnDestroy(void)
{
	// unregister notifications
	if (UCvsApp* app = UCvsApp::gApp)
	{
		app->GetFilterModel()->GetNotificationManager()->CheckOut(this);
		app->GetRecursionModel()->GetNotificationManager()->CheckOut(this);
		app->GetIgnoreModel()->GetNotificationManager()->CheckOut(this);
	}

	delete this;
}

static void onFilteringChanged(CObject* obj)
{
        ((UCvsFiles *)obj)->OnFilteringChanged();
}

void UCvsFiles::OnCreate(void)
{
	KoTraversalContext context(this);
	bool recursive;
	
	if(sFileIconTextIcon == 0L)
	{
		sFileIconTextIcon = UCreate_pixmap(this, "cvsfile.xpm");
		sFileIconBinaryIcon = UCreate_pixmap(this, "binfile.xpm");
		sFileIconUnknownIcon = UCreate_pixmap(this, "unkfile.xpm");
		sFileIconAddedIcon = UCreate_pixmap(this, "addfile.xpm");
		sFileIconConflictIcon = UCreate_pixmap(this, "conflict.xpm");
		sFileIconMissIcon = UCreate_pixmap(this, "missfile.xpm");
		sFolderIconIcon = UCreate_pixmap(this, "folder.xpm");
		sFolderIconUnknownIcon = UCreate_pixmap(this, "foldunk.xpm");
		sFolderIconMissIcon = UCreate_pixmap(this, "foldmiss.xpm");
		sFileIconTextModIcon = UCreate_pixmap(this, "modfile.xpm");
		sFileIconBinaryModIcon = UCreate_pixmap(this, "modbin.xpm");
		sFileIconIgnoredIcon = UCreate_pixmap(this, "ignfile.xpm");
		sFolderIconIgnoredIcon = UCreate_pixmap(this, "foldign.xpm");
		sFileIconRemovedIcon = UCreate_pixmap(this, "delfile.xpm");
	}

	UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kUMainWidget, 0), 0L);

    recursive = context.IsRecursive();

	SetRecursive(!recursive);
	HideColumns();
	
	SetRecursive(recursive);
	ShowColumns();
	
	if( UCvsApp* app = UCvsApp::gApp ) {
		app->GetFilterModel()->GetNotificationManager()->CheckIn(this, onFilteringChanged);
		app->GetRecursionModel()->GetNotificationManager()->CheckIn(this, onFilteringChanged);
		app->GetIgnoreModel()->GetNotificationManager()->CheckIn(this, onFilteringChanged);
	}
	UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kUMainWidget, 1), 0L);
}

void UCvsFiles::ResetView(const char *path, bool notifyBrowser)
{
	m_path = path;
	ResetView(true, false);

	std::string title(UCvsApp::gApp->GetAppName());
	title += ": ";
	title += GetPath();
	UEventSendMessage(kUCvsFrameID, EV_SETTEXT, kUMainWidget, (void *)(const char *)title.c_str());

	if(notifyBrowser)
	{
		// notify the tree
		UCvsApp::gApp->GetBrowserView()->StepToLocation(path);
	}
}

void UCvsFiles::ResetView(std::string path, bool notifyBrowser)
{
	ResetView(path.c_str(), notifyBrowser);
}

void UCvsFiles::GetEntriesModTime(time_t & newEntriesMod, time_t & newEntriesLogMod)
{
	newEntriesMod = 0;
	newEntriesLogMod = 0;
	if(chdir(GetPath().c_str()) != 0)
		return;
	if(chdir("CVS") != 0)
		return;

	struct stat sb;
	if (stat("Entries", &sb) != -1)
		newEntriesMod = sb.st_mtime;
	if (stat("Entries.log", &sb) != -1)
		newEntriesLogMod = sb.st_mtime;
	chdir(GetPath().c_str());
}

int UCvsFiles::Search(const char *title)
{
	int row = -1;
	while((row = UEventSendMessage(GetWidID(), EV_LIST_GETNEXT,
									 UMAKEINT(kUMainWidget, row), 0L)) != -1)
	{
		ULIST_INFO info;
		info.col = 0;
		info.row = row;
		UEventSendMessage(GetWidID(), EV_LIST_GETINFO, kUMainWidget, &info);
		if(info.title == 0L)
			continue;

#if qUnix
		if(strcmp(title, info.title) == 0)
#else
		if(stricmp(title, info.title) == 0)
#endif
			return row;
	}

	return -1;
}

void UCvsFiles::ResetView(bool forceReload, bool notifyBrowser)
{
	UWaitCursor wait;

	static bool sSemaphore = false;
	USemaphore policeman(sSemaphore);
	if(policeman.IsEnteredTwice())
		return;

	// - check if we really need to reload (forceReload == false)
	// - wait a bit for the watcher in order to let him tell us
	// if something was modified.
	time_t newEntriesMod;
	time_t newEntriesLogMod;
	GetEntriesModTime(newEntriesMod, newEntriesLogMod);
	if(!forceReload)
	{
		if(m_entriesMod == newEntriesMod && m_entriesLogMod == newEntriesLogMod)
			return;
	}	

	// reset all the watchers
	m_entriesMod = newEntriesMod;
	m_entriesLogMod = newEntriesLogMod;

	// get the selection to later try to restore it
	CvsArgs selection(false);
	int nItem = -1;
	std::string buf;
	
	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL,
									 UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
		//selection.add((*data)[EntnodeData::kName]);
		selection.add(data->GetFullPathName(buf));
	}

	UEventSendMessage(GetWidID(), EV_LIST_RESETALL, kUMainWidget, 0L);

	UEventSendMessage(GetWidID(), EV_CHGFOCUS, kUMainWidget, 0L);

	// make sure columns are ok
	KoTraversalContext context(this);

	if ((context.IsRecursive() && !m_isRecursive) || (!context.IsRecursive() && m_isRecursive))
	{
	
		KiColumnModel* oldModel = GetColumnModel();
		int oldCount = oldModel->GetCount();
		const KiColumn* oldColumn = (0 <= m_sort && m_sort < oldCount) ? oldModel->GetAt(m_sort) : NULL;
		HideColumns();
		
		SetRecursive(context.IsRecursive());
		ShowColumns();
		
		if (oldColumn == NULL)
		{
				m_sort = 0;
		}
		else
		{
			KiColumnModel* newModel = GetColumnModel();
			int newCount = newModel->GetCount();
			int sort = -1;
			for (int pos = 0; pos < newCount && sort == -1; pos++)
			{
				if (newModel->GetAt(pos) == oldColumn)
				{
					sort = pos;
				}
			}
			m_sort = (sort >= 0) ? sort : 0;
		}
	}
	
	// refetch all items
	TViewFill traverse(/*this, */&context, m_entries);
	/*kTraversal res = */FileTraverse(GetPath().c_str(), traverse);

	// add the missing files
	Entries_SetMissing(m_entries);

	int numEntries = m_entries.NumOfElements();
	for(int i = 0; i < numEntries; i++)
	{
		const ENTNODE & theNode = m_entries.Get(i);
		EntnodeData *data = ((ENTNODE *)&theNode)->Data();
		if(!data->IsMissing())
			continue;

		//TViewFill::
		traverse.InitItem(data, UCvsFiles::GetImageForEntry(data));
#ifdef _0_
		int rownum;
		UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kUMainWidget, &rownum);
		UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kUMainWidget, rownum), data);
		ULIST_INSERT entry;
		entry.row = rownum;

		entry.col = 0;
		entry.title = (*data)[EntnodeData::kName];
		entry.icon = UCvsFiles::GetImageForEntry(data);
		UEventSendMessage(GetWidID(), EV_LIST_INSERT, kUMainWidget, &entry);

		entry.icon = 0L;

		// set item text for additional columns
		for(int j = 1; j < m_modelColumns->GetCount(); j++)
		{
			const char *info = (*data)[j];
			if(info != 0L)
			{
				entry.col = j;
				entry.title = info;
				UEventSendMessage(GetWidID(), EV_LIST_INSERT, kUMainWidget, &entry);
			}
		}
		entry.col = EntnodeData::kState;
		entry.title = data->GetDesc();
		UEventSendMessage(GetWidID(), EV_LIST_INSERT, kUMainWidget, &entry);
#endif
	}

	Resort();

	// now restore the selection
	int argc = selection.Argc(), c;
	char * const *argv = selection.Argv();
	for(c = 0; c < argc; c++)
	{
		int row = Search(argv[c]);
		if(row != -1)
			UEventSendMessage(GetWidID(), EV_LIST_ADDSEL, UMAKEINT(kUMainWidget, row), 0L);
	}

	if(notifyBrowser)
	{
		// notify the tree
		UCvsApp::gApp->GetBrowserView()->ResetView(forceReload);
	}
#ifdef GTK
	if(wid != 0L)
	{	
		gtk_signal_disconnect (GTK_OBJECT (wid), sigHandler);
	}
#endif
	
}

void *UCvsFiles::GetImageForEntry(EntnodeData *data)
{
	int result;
	void *resIcon = 0L;

	if(data->GetType() == ENT_FILE)
	{
		const char *info = 0L;
		if(data->IsIgnored())
		{
			result = kFileIconIgnored;
		}
		else if(data->IsUnknown())
		{
			result = kFileIconUnknown;
		}
		else if(data->IsMissing())
		{
			result = data->IsRemoved() ? kFileIconRemoved : kFileIconMiss;
		}
		else if( !IsNullOrEmpty(data->GetConflict()) )
		{
			result = kFileIconConflict;
		}
		else if(data->IsRemoved())
		{
			result = kFileIconRemoved;
		}
		else if(((info = ((*data)[EntnodeFile::kOption]).c_str())) != 0L && strcmp(info, "-kb") == 0)
		{
			result = data->IsUnmodified() ? kFileIconBinary : kFileIconBinaryMod;
		}
		else
		{
			result = data->IsUnmodified() ? kFileIconText : kFileIconTextMod;
		}
	}
	else
	{
		if(data->IsIgnored())
		{
			result = kFolderIconIgnored;
		}
		else if(data->IsUnknown())
		{
			result = kFolderIconUnknown;
		}
		else if(data->IsMissing())
		{
			result = kFolderIconMiss;
		}
		else
		{
			result = kFolderIcon;
		}
	}

#if qGTK
	void *pixmap = 0L;
	switch(result)
	{
	case kFileIconText:
		pixmap = sFileIconTextIcon;
		break;
	case kFileIconBinary:
		pixmap = sFileIconBinaryIcon;
		break;
	case kFileIconUnknown:
		pixmap = sFileIconUnknownIcon;
		break;
	case kFileIconAdded:
		pixmap = sFileIconAddedIcon;
		break;
	case kFileIconConflict:
		pixmap = sFileIconConflictIcon;
		break;
	case kFileIconMiss:
		pixmap = sFileIconMissIcon;
		break;
	case kFolderIcon:
		pixmap = sFolderIconIcon;
		break;
	case kFolderIconUnknown:
		pixmap = sFolderIconUnknownIcon;
		break;
	case kFolderIconMiss:
		pixmap = sFolderIconMissIcon;
		break;
	case kFileIconTextMod:
		pixmap = sFileIconTextModIcon;
		break;
	case kFileIconBinaryMod:
		pixmap = sFileIconBinaryModIcon;
		break;
	case kFileIconIgnored:
		pixmap = sFileIconIgnoredIcon;
		break;
	case kFolderIconIgnored:
		pixmap = sFolderIconIgnoredIcon;
		break;
	case kFileIconRemoved:
		pixmap = sFileIconRemovedIcon;
		break;
	}
	if(pixmap != 0L)
	{
		resIcon = gtk_pixmap_new (GTK_PIXMAP(pixmap)->pixmap, GTK_PIXMAP(pixmap)->mask);
	}
#endif

	return resIcon;
}


int UCvsFiles::OnCmdUpdate(void)
{
  KoUpdateHandler handler;
  
  return ProcessSelectionCommand(handler);
}

bool UCvsFiles::DisableGeneric(bool needSelection)
{
	return ((UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, -1), 0L) == -1) && needSelection) ||
		UCvsApp::gApp->IsCvsRunning() || gCvsPrefs.empty() || !HasFocus();
}

bool UCvsFiles::DisableGeneric()
{
	return DisableGeneric(true);
}

void UCvsFiles::OnCmdUIUpdate(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;
	int numFiles = 0;
	int numFolders = 0;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(data->IsUnknown())
		{
			res = false;
			break;
		}
		if(data->GetType() == ENT_FILE)
			numFiles++;
		else
			numFolders++;

		if((numFiles != 0) && (numFolders != 0))
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIAdd(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;
	int numFiles = 0;
	int numFolders = 0;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(!(data->IsUnknown() || (data->IsRemoved() && data->IsMissing())))
		{
			res = false;
			break;
		}
		if(data->GetType() == ENT_FILE)
		{
			numFiles++;
		}
		else
			numFolders++;

		if((numFiles != 0) && (numFolders != 0))
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIAddB(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(data->GetType() != ENT_FILE)
		{
			res = false;
			break;
		}
		if(!(data->IsUnknown() || (data->IsRemoved() && data->IsMissing())))
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIRelease(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;
	int numFolders = 0;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(data->IsUnknown() || data->GetType() != ENT_SUBDIR)
		{
			res = false;
			break;
		}
		numFolders++;

		if(numFolders > 1)
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIRmv(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(data->IsUnknown() || data->IsRemoved() || data->GetType() == ENT_SUBDIR)
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIGraph(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(data->IsUnknown() || data->GetType() == ENT_SUBDIR)
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIEditseldef(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(data->GetType() == ENT_SUBDIR || data->IsMissing())
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
	if(gCvsPrefs.Viewer() != 0L)
	{
		std::string title(_i18n("Edit with "));
		title += gCvsPrefs.Viewer();
		pCmdUI->SetText(title.c_str());
	}
}

void UCvsFiles::OnCmdUIUpone(UCmdUI *pCmdUI)
{
	pCmdUI->Enable(!DisableGeneric());
}

void UCvsFiles::OnCmdUITrash(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
		if(data->GetType() == ENT_SUBDIR || data->IsMissing())
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIExplore(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	int nItem = -1;
	bool res = true;
	int numItem = 0;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		if(data->IsMissing() || ++numItem > 1)
		{
			res = false;
			break;
		}
	}

	pCmdUI->Enable(res);
}

void UCvsFiles::OnCmdUIViewRecursive(UCmdUI *pCmdUI)
{
	KoRecursionModel *m_recursion = (KoRecursionModel *)UCvsApp::gApp->GetRecursionModel();
	pCmdUI->Enable(!UCvsApp::gApp->IsCvsRunning());
	pCmdUI->Check(m_recursion->IsShowRecursive());
}

void UCvsFiles::OnCmdUIMacroSel(int cmd, UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

#ifdef USE_PYTHON
	if( !PyIsUICacheValid() )
    {
		int nItem = -1;
		while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
		{
			fprintf(stderr, "%d\n", nItem);
			EntnodeData *data;
			UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
			PyAppendCache(data);
		}
	}
	
	UCmdUI ucmdui((pCmdUI->m_nID));
	ucmdui.pCmdUI = pCmdUI;
	PyDoCmdUI(&ucmdui);
#else
	int nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, -1), 0L);
 	pCmdUI->Enable(nItem != -1 && CTcl_Interp::IsAvail());
#endif
}

int UCvsFiles::OnCmdAdd(void)
{
  KoAddHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnViewRecursive(void)
{
	KoRecursionModel *m_recursion = (KoRecursionModel *)UCvsApp::gApp->GetRecursionModel();
	m_recursion->ToggleRecursion();
	ResetView(true, true);
	
	UCvsApp::gApp->GetMainFrame()->OnFilterMaskEntered();

	return 0;
}

int UCvsFiles::OnCmdAddb(void)
{
  KoAddBinaryHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdCommit(void)
{
  KoCommitHandler handler;
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdRmv(void)
{
  KoFileRemoveHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdQueryUpdate(void)
{
  KoQueryUpdateHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdUpone(void)
{
	std::string uppath, filename;
	SplitPath(GetPath().c_str(), uppath, filename);
	ResetView( /*(const char*)*/uppath, true);
	return 0;
}

int UCvsFiles::OnCmdTrash(void)
{
  KoMoveToTrashHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdDiff(void)
{
  KoDiffHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdLog(void)
{
  KoLogHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdGraph(void)
{
  KoGraphHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdStatus(void)
{
  KoStatusHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdLock(void)
{
  KoLockHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdUnlock(void)
{
  KoUnlockHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdWatchOn(void)
{
  KoWatchOnHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdWatchOff(void)
{
  KoWatchOnHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdEdit(void)
{
  KoEditHandler handler;
  
  return ProcessSelectionCommand(handler);
}

void UCvsFiles::OnCmdUIResEdit(UCmdUI *pCmdUI)
{
  if(gCvsPrefs.CvsVersion() == 1)
    OnCmdUIUpdate(pCmdUI);
}

int UCvsFiles::OnCmdResEdit(void)
{
  KoReservedEditHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdUnedit(void)
{
  KoUneditHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdWatchers(void)
{
  KoWatchersHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdEditors(void)
{
  KoEditorsHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdRelease(void)
{
  KoReleaseHandler handler;
  
  return ProcessSelectionCommand(handler);
}


int UCvsFiles::OnCmdTagNew(void)
{
  KoCreateTagHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdTagDelete(void)
{
  KoDeleteTagHandler handler;
  
  return ProcessSelectionCommand(handler);
}

int UCvsFiles::OnCmdTagBranch(void)
{
  KoBranchTagHandler handler;
  
  return ProcessSelectionCommand(handler);
}


int UCvsFiles::OnCmdExplore(void)
{
	const char* fullpath = GetPath().c_str();

	int nItem = -1;

	// first add the folders
	if((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);

		std::string buf;
		fullpath = data->GetFullPathName(buf);

#ifdef WIN32
		HINSTANCE hInst = ShellExecute(*AfxGetMainWnd(), "explore", fullpath,
									   0L, 0L, SW_SHOWDEFAULT);
		if((long)hInst < 32)
		{
			cvs_err(_i18n("Unable to explore '%s' (error %d)\n"), (char *)GetPath(), GetLastError());
		}
#endif
#ifdef qUnix
		CvsArgs args(false);
		args.add(gCvsPrefs.Browser());
		args.add(fullpath);
		UCvsApp::gApp->Execute(args.Argc(), args.Argv());
#endif
	}
	return 0;
}

int UCvsFiles::OnCmdEditsel(void)
{
	int nItem = -1;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
		EditSel(data);
	}
	return 0;
}

int UCvsFiles::OnCmdEditseldef(void)
{
	int nItem = -1;

	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
		EditSel(data, true);
	}
	return 0;
}

int UCvsFiles::OnMacroSel(int cmd)
{

#ifdef USE_PYTHON
	PyDoPython(cmd);
#else
	CTcl_Interp interp;
 	CMacroEntry & entry = gMacrosSel.entries[cmd - cmdSELMACRO];
 	CStr path = entry.path;
 	CTcl_Interp::Unixfy(path);
 
 	TclBrowserReset();
 
 	int nItem = -1;
 	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
 	{
 		EntnodeData *data;
 		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
 		TclBrowserAppend(GetPath(), data);
 	}
 
 	interp.DoScriptVar("source \"%s\"", (const char *)path);
#endif
	ResetView(true, true);

	return 0;
}

void UCvsFiles::EditSel(EntnodeData *data, bool useDefault)
{
	std::string buf;
	std::string fullpath = data->GetFullPathName(buf);
	
	if(data->GetType() == ENT_SUBDIR)
	{
		ResetView( /*(const char*)*/fullpath.c_str(), true);
	}
	else
	{
#ifdef WIN32
		// This replaces huge chunk of code for now, even thought it's not ever called
		LaunchViewer((*data)[EntnodeData::kName]);
#endif
#ifdef qUnix
		std::string fullpath = data->GetFullPathName(buf);
		
		CvsArgs args(false);
		args.add(gCvsPrefs.Viewer());
		args.add(fullpath.c_str());
		UCvsApp::gApp->Execute(args.Argc(), args.Argv());
#endif
	}
}

void UCvsFiles::OnDblClick(int button)
{
	int nItem = -1;

	if(button != 1)
		return;
	
	while((nItem = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL, UMAKEINT(kUMainWidget, nItem), 0L)) != -1)
	{
		EntnodeData *data;
		UEventSendMessage(GetWidID(), EV_LIST_ROWGETDATA, UMAKEINT(kUMainWidget, nItem), &data);
		EditSel(data);
	}
}

void UCvsFiles::OnPopup(void)
{
	fprintf(stderr, "UCvsFiles: Popup called\n");
}

#ifdef _0_
static int compareName(UWidget *wid, void *data1, void *data2)
{
	UCvsFiles *w = (UCvsFiles *)wid;
	EntnodeData *d1 = (EntnodeData *)data1;
	EntnodeData *d2 = (EntnodeData *)data2;
	int res = stricmp((*d1)[EntnodeData::kName], (*d2)[EntnodeData::kName]);
	return w->IsSortAscendant() ? res : -res;
}

static int compareStatus(UWidget *wid, void *data1, void *data2)
{
	UCvsFiles *w = (UCvsFiles *)wid;
	EntnodeData *d1 = (EntnodeData *)data1;
	EntnodeData *d2 = (EntnodeData *)data2;
	int res = stricmp((*d1)[EntnodeData::kState], (*d2)[EntnodeData::kState]);
	if(res == 0)
		res = stricmp((*d1)[EntnodeData::kName], (*d2)[EntnodeData::kName]);
	return w->IsSortAscendant() ? res : -res;
}

static int compareOption(UWidget *wid, void *data1, void *data2)
{
	UCvsFiles *w = (UCvsFiles *)wid;
	EntnodeData *d1 = (EntnodeData *)data1;
	EntnodeData *d2 = (EntnodeData *)data2;
	const char *s1 = (*d1)[EntnodeFile::kOption];
	const char *s2 = (*d2)[EntnodeFile::kOption];
	int res;
	if(s1 != 0L && s2 != 0L)
		res = strcmp(s1, s2);
	else
		res = (long)s1 < (long)s2 ? -1 : ((long)s1 > (long)s2 ? 1 : 0);
	if(res == 0)
		res = stricmp((*d1)[EntnodeData::kName], (*d2)[EntnodeData::kName]);
	return w->IsSortAscendant() ? res : -res;
}

static int revcmp(const char *rev1, const char *rev2)
{
	if(rev1 == 0L && rev2 == 0L)
		return 0;
	else if(rev1 == 0L || rev2 == 0L)
		return rev1 == 0L ? -1 : 1;

	CStr r1(rev1), r2(rev2);
	CStr q1, q2;
	char *tmp;
	int v1, v2;

	if((tmp = strchr(r1, '.')) != 0L)
	{
		tmp[0] = '\0';
		q1 = tmp + 1;
	}

	v1 = atoi(r1);

	if((tmp = strchr(r2, '.')) != 0L)
	{
		tmp[0] = '\0';
		q2 = tmp + 1;
	}
	v2 = atoi(r2);

	if(v1 == v2)
		return revcmp(q1.empty() ? (char *)0L : (const char *)q1, q2.empty() ? (char *)0L : (const char *)q2);

	return v1 < v2 ? -1 : 1;
}

static int compareRevs(UWidget *wid, void * data1, void * data2)
{
	UCvsFiles *w = (UCvsFiles *)wid;
	EntnodeData *d1 = (EntnodeData *)data1;
	EntnodeData *d2 = (EntnodeData *)data2;
	const char *s1 = (*d1)[EntnodeFile::kVN];
	const char *s2 = (*d2)[EntnodeFile::kVN];
	int res;
	if(s1 != 0L && s2 != 0L)
		res = revcmp(s1, s2);
	else
		res = (long)s1 < (long)s2 ? -1 : ((long)s1 > (long)s2 ? 1 : 0);
	if(res == 0)
		res = stricmp((*d1)[EntnodeData::kName], (*d2)[EntnodeData::kName]);
	return w->IsSortAscendant() ? res : -res;
}

static int compareTag(UWidget *wid, void * data1, void * data2)
{
	UCvsFiles *w = (UCvsFiles *)wid;
	EntnodeData *d1 = (EntnodeData *)data1;
	EntnodeData *d2 = (EntnodeData *)data2;
	const char *s1 = (*d1)[EntnodeFile::kTag];
	const char *s2 = (*d2)[EntnodeFile::kTag];
	int res;
	if(s1 != 0L && s2 != 0L)
		res = strcmp(s1, s2);
	else
		res = (long)s1 < (long)s2 ? -1 : ((long)s1 > (long)s2 ? 1 : 0);
	if(res == 0)
		res = stricmp((*d1)[EntnodeData::kName], (*d2)[EntnodeData::kName]);
	return w->IsSortAscendant() ? res : -res;
}

static int compareConflict(UWidget *wid, void * data1, void * data2)
{
	UCvsFiles *w = (UCvsFiles *)wid;
	EntnodeData *d1 = (EntnodeData *)data1;
	EntnodeData *d2 = (EntnodeData *)data2;
	const char *s1 = (*d1)[EntnodeFile::kConflict];
	const char *s2 = (*d2)[EntnodeFile::kConflict];
	int res;
	if(s1 != 0L && s2 != 0L)
		res = strcmp(s1, s2);
	else
		res = (long)s1 < (long)s2 ? -1 : ((long)s1 > (long)s2 ? 1 : 0);
	if(res == 0)
		res = stricmp((*d1)[EntnodeData::kName], (*d2)[EntnodeData::kName]);
	return w->IsSortAscendant() ? res : -res;
}

static int compareTimestamp(UWidget *wid, void *data1, void *data2)
{
	UCvsFiles *w = (UCvsFiles *)wid;
	EntnodeData *d1 = (EntnodeData *)data1;
	EntnodeData *d2 = (EntnodeData *)data2;
	
	const char *s1 = (*d1)[EntnodeFile::kTS];
	const char *s2 = (*d2)[EntnodeFile::kTS];
	
	int res;
	
	if(s1 != 0L && s2 != 0L ) {
		UStr str[2] = {s1, s2};
		
		UStr t[2];
		
		for(int i=0; i < 2; i++) {
			// put the year in front of the date string
			t[i] = str[i].substr(str[i].rfind(' ')+1, 4);
			t[i] += " ";

			// convert month name to month number
			for(int j=0; j < 12; j++) {
				if( !(strcmp(str[i].substr(4,3), month_names[j]) ) ) {
					if(j<9)
						t[i] += "0";
					
					t[i] += j+1; 
					
					break;
				}
			}

			t[i] += " ";
			// paste day and time to time string
			t[i] += str[i].substr(8, str[i].rfind(' ')-4);
		}
		
		res = t[0].compare(t[1]);
		
#ifdef DEBUG
		fprintf(stderr, "%s %c %s\n", t[0].c_str(), (res==1?'<':'>') , t[1].c_str());
#endif
		
	} else {
		res = s1 < s2 ? -1 : (s1 > s2 ? 1 : 0);
	}
	
	if(res == 0)
		res = stricmp((*d1)[EntnodeData::kName], (*d2)[EntnodeData::kName]);
		
	return w->IsSortAscendant() ? res : -res;
}
#endif

static int gtkCompareWrap(UWidget *wid, void *data1, void *data2)
{
	return _Compare(data1, data2, gSortParam);
}

void UCvsFiles::Resort(void)
{
	KiColumnModel* model = GetColumnModel();
	KoColumnContext context(GetPath().c_str());
	gSortParam = new CSortParam (m_ascendant, true, model->GetAt(m_sort), model->GetAt(EntnodeData::kName), &context);
	
	UEventSendMessage(GetWidID(), EV_LIST_RESORT, UMAKEINT(kUMainWidget, m_sort), (void *)gtkCompareWrap);
	UEventSendMessage(GetWidID(), EV_LIST_SETSORTDIR, UMAKEINT(kUMainWidget, m_sort), (void *)m_ascendant);
}

void UCvsFiles::OnSelColumn(int column)
{
	if(column == m_sort)
		m_ascendant = !m_ascendant;
	else
	{
		m_ascendant = true;
		switch(column)
		{
		case EntnodeData::kName:
		case EntnodeData::kState:
		case EntnodeFile::kEncoding:
		case EntnodeFile::kExt:
		case EntnodeFile::kOption:
		case EntnodeFile::kTag:
		case EntnodeFile::kConflict:
		case EntnodeFile::kTS:
		case EntnodeFile::kVN:
			m_sort = column;
			break;
		}
	}

	Resort();
}

void UCvsFiles::SetRecursive(bool isRecursive)
{
	m_isRecursive = isRecursive;
	m_modelColumns = (m_isRecursive) ? KiColumnModel::GetRecursiveModel() : KiColumnModel::GetRegularModel();
}

void UCvsFiles::ShowColumns(void)
{
	KiColumnModel* model = GetColumnModel();
	int columnCount = model->GetCount();
	
	UEventSendMessage(GetWidID(), EV_LIST_SHOWCOLUMNS, UMAKEINT(kUMainWidget, columnCount), 0L);

	for(int i = 0; i<columnCount; i++)
	{
		LV_COLUMN lv;

		m_modelColumns->GetAt(i)->GetSetupData(&lv);
		UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kUMainWidget, i), (char*)lv.name);
		UEventSendMessage(GetWidID(), EV_LIST_SETCOLWIDTH, UMAKEINT(kUMainWidget, i), (void *)lv.width);
	}
}
   
void UCvsFiles::HideColumns(void)
{
	KiColumnModel* model = GetColumnModel();
	int columnCount = model->GetCount();
	
	UEventSendMessage(GetWidID(), EV_LIST_HIDECOLUMNS, UMAKEINT(kUMainWidget, columnCount), 0L);
}

void UCvsFiles::OnFilteringChanged()
{
	ResetView(true, false);
}
