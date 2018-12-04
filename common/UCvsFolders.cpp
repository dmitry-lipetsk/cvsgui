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
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "UCvsFolders.h"
#include "UCvsDialogs.h"
#include "UCvsApp.h"
#include "UCvsFiles.h"
#include "UCvsCommands.h"
#include "UCvsFrame.h"
#include "MultiString.h"
#include "FileTraversal.h"
#include "CvsEntries.h"
#include "CvsPrefs.h"
#include "AppConsole.h"
#include "CvsCommands.h"
#include "TclGlue.h"
#ifdef USE_PYTHON
#	include "PythonGlue.h"
#endif
#include "MacrosSetup.h"
#include "CvsArgs.h"
#include "CvsAlert.h"

#ifdef WIN32
static PCStr gOldLoc("P_BrowserLoc", "C:\\");
#endif

#ifdef qUnix
static CPersistentString gOldLoc("P_BrowserLoc");
#endif

static const char *gDummyFile = "@@@dummy file@@@";

static CMString gHistoryLocs(100, "P_BrowserLocs");

static void *sFolderIcon;
static void *sFolderUnknownIcon;
static void *sFolderIgnoredIcon;
static void *sFolderMissingIcon;

enum
{
	kIconFolderClosed,
	kIconFolderOpened,
	kIconFolderCVSClosed,
	kIconFolderCVSOpened,
	kIconFolderIgnoreClosed,
	kIconFolderIgnoreOpened,
	kIconFolderIgnoreLost
};

UIMPLEMENT_DYNAMIC(UCvsFolders, UWidget)

UBEGIN_MESSAGE_MAP(UCvsFolders, UWidget)
	ON_UUPDATECMD(cmdUPDATE, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdCOMMIT, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdDIFF, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdEDIT, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdRESEDIT, UCvsFolders::OnCmdUIResEdit)
	ON_UUPDATECMD(cmdEDITORS, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdLOCKF, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdLOG, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdRELEASE, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdSTATUS, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdUNEDIT, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdUPDATE, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdQUERYUPDATE, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdUNLOCKF, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdWATCHERS, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdWATCHON, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdWATCHOFF, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdTAGNEW, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdTAGDELETE, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdTAGBRANCH, UCvsFolders::OnCmdUIUpdate)
	ON_UUPDATECMD(cmdADD, UCvsFolders::OnCmdUIAdd)
	ON_UUPDATECMD(cmdEXPLORE, UCvsFolders::OnCmdUIExplore)
	ON_UUPDATECMD(cmdCHECKOUT, UCvsFolders::OnCmdUICheckout)
	ON_UUPDATECMD(cmdIMPORT, UCvsFolders::OnCmdUIImport)
#ifdef USE_PYTHON
	ON_UUPDATECMD_RANGE(cmdPyMACROS, cmdPyMACROSEND, UCvsFolders::OnCmdUIMacrosSel)
#else
	ON_UUPDATECMD_RANGE(cmdSELMACRO, cmdSELMACROEND, UCvsFolders::OnCmdUIMacrosSel)
#endif
	ON_UDESTROY(UCvsFolders)
	ON_UCREATE(UCvsFolders)
	ON_TREE_EXPANDING(kUMainWidget, UCvsFolders::OnTreeExpanding)
	ON_TREE_SELECTING(kUMainWidget, UCvsFolders::OnTreeSelecting)
	ON_UCOMMAND(cmdUPDATE, UCvsFolders::OnCmdUpdate)
	ON_UCOMMAND(cmdADD, UCvsFolders::OnCmdAdd)
	ON_UCOMMAND(cmdCOMMIT, UCvsFolders::OnCmdCommit)
	ON_UCOMMAND(cmdQUERYUPDATE, UCvsFolders::OnCmdQueryUpdate)
	ON_UCOMMAND(cmdDIFF, UCvsFolders::OnCmdDiff)
	ON_UCOMMAND(cmdLOG, UCvsFolders::OnCmdLog)
	ON_UCOMMAND(cmdSTATUS, UCvsFolders::OnCmdStatus)
	ON_UCOMMAND(cmdLOCKF, UCvsFolders::OnCmdLock)
	ON_UCOMMAND(cmdUNLOCKF, UCvsFolders::OnCmdUnlock)
	ON_UCOMMAND(cmdWATCHON, UCvsFolders::OnCmdWatchOn)
	ON_UCOMMAND(cmdWATCHOFF, UCvsFolders::OnCmdWatchOff)
	ON_UCOMMAND(cmdEDIT, UCvsFolders::OnCmdEdit)
	ON_UCOMMAND(cmdRESEDIT, UCvsFolders::OnCmdResEdit)
	ON_UCOMMAND(cmdUNEDIT, UCvsFolders::OnCmdUnedit)
	ON_UCOMMAND(cmdWATCHERS, UCvsFolders::OnCmdWatchers)
	ON_UCOMMAND(cmdEDITORS, UCvsFolders::OnCmdEditors)
	ON_UCOMMAND(cmdRELEASE, UCvsFolders::OnCmdRelease)
	ON_UCOMMAND(cmdTAGNEW, UCvsFolders::OnCmdTagNew)
	ON_UCOMMAND(cmdTAGDELETE, UCvsFolders::OnCmdTagDelete)
	ON_UCOMMAND(cmdTAGBRANCH, UCvsFolders::OnCmdTagBranch)
	ON_UCOMMAND(cmdEXPLORE, UCvsFolders::OnCmdExplore)
	ON_UCOMMAND(cmdCHECKOUT, UCvsFolders::OnCmdCheckout)
	ON_UCOMMAND(cmdIMPORT, UCvsFolders::OnCmdImport)
#ifdef USE_PYTHON
	ON_UCOMMAND_RANGE(cmdPyMACROS, cmdPyMACROSEND, UCvsFolders::OnCmdMacrosSel)
#else
	ON_UCOMMAND_RANGE(cmdSELMACRO, cmdSELMACROEND, UCvsFolders::OnCmdMacrosSel)
#endif
UEND_MESSAGE_MAP()

#if 0
	ON_WM_RBUTTONDOWN() // TODO
#endif

static bool sTemporaryTurnOffNotify = false;

class CTempToggleBool
{
public:
	CTempToggleBool(bool & value) : m_value(value), m_didSetIt(false)
	{
		if(!m_value)
		{
			m_didSetIt = true;
			m_value = true;
		}
	}

	~CTempToggleBool()
	{
		if(m_didSetIt)
			m_value = false;
	}
protected:
	bool & m_value;
	bool m_didSetIt;
};

static void *GetIcon(UCvsFolders *treeCrtl, UTREEITEM item, bool hasCvsInfos, bool expand, EntnodeData *data)
{
	int newImage;
	void *result = 0L;

	if(data == 0L)
	{
		if(expand)
			newImage = hasCvsInfos ? kIconFolderCVSOpened : kIconFolderOpened;
		else
			newImage = hasCvsInfos ? kIconFolderCVSClosed : kIconFolderClosed;
	}
	else
	{
		if(expand)
		{
			if(data->IsIgnored())
				newImage = kIconFolderIgnoreOpened;
			else if(data->IsUnknown())
				newImage = kIconFolderOpened;
			else if(data->IsMissing())
				newImage = kIconFolderIgnoreLost;
			else
				newImage = kIconFolderCVSOpened;
		}
		else
		{
			if(data->IsIgnored())
				newImage = kIconFolderIgnoreClosed;
			else if(data->IsUnknown())
				newImage = kIconFolderClosed;
			else if(data->IsMissing())
				newImage = kIconFolderIgnoreLost;
			else
				newImage = kIconFolderCVSClosed;
		}
	}

#if qGTK
	void *pixmap = 0L;
	switch(newImage)
	{
	case kIconFolderClosed:
	case kIconFolderOpened:
		pixmap = sFolderUnknownIcon;
		break;
	case kIconFolderCVSClosed:
	case kIconFolderCVSOpened:
		pixmap = sFolderIcon;
		break;
	case kIconFolderIgnoreClosed:
	case kIconFolderIgnoreOpened:
		pixmap = sFolderIgnoredIcon;
		break;
	case kIconFolderIgnoreLost:
		pixmap = sFolderMissingIcon;
		break;
	}
	if(pixmap != 0L)
	{
		result = gtk_pixmap_new (GTK_PIXMAP(pixmap)->pixmap, GTK_PIXMAP(pixmap)->mask);
	}
#endif

	return result;
}

// regarding a path and a root item, create an
// item with a dummy item inside if and only if
// the folder is not empty.
class TBrowserFillDummy : public TraversalReport
{
public:
	UTREEITEM	m_root;
	UTREEITEM	m_item;
	UCvsFolders	*m_treeCtrl;
	bool		m_empty;
	bool		m_hascvs;
	EntnodeData *m_data;
	std::string		m_dirname;

	TBrowserFillDummy(UCvsFolders *treeCtrl, UTREEITEM root, UTREEITEM item, EntnodeData *data) :
		m_root(root), m_item(item), m_treeCtrl(treeCtrl), m_empty(true),
		m_hascvs(false), m_data(data) {}

	virtual ~TBrowserFillDummy() {}

	virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname, const FSSpec * macspec)
	{
		m_dirname = dirname;
#if qUnix
		if(m_dirname.empty())
			m_dirname = fullpath; // '/'
#endif
		return kContinueTraversal;
	}

	virtual kTraversal ExitDirectory(const char *fullpath)
	{
		if(m_item == 0L)
		{
			UTREE_INSERT insert;
			insert.parent = m_root;
			insert.icon = GetIcon(m_treeCtrl, m_item, m_hascvs, false, m_data);
			insert.title = m_dirname.c_str();
			insert.data = 0L;
			UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_INSERT, kUMainWidget, &insert);
			m_item = insert.result;
		}

		// assign the entries to this item
		UTREE_INFO query;
		query.item = m_item;
		if(m_data != 0L)
		{
			UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
			EntnodeData *data = (EntnodeData *)query.data;

			if(m_data != 0L)
				m_data->Ref();

			query.data = m_data;
			UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_SETDATA, kUMainWidget, &query);

			if(data != 0L)
				data->UnRef();
		}
		else
		{
			UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
			m_data = (EntnodeData *)query.data;
		}

		if(m_empty)
			return kContinueTraversal;

		// add a dummy item to simulate the folder
		// is not empty. This dummy item will be
		// replaced by a complete listing when the
		// node is expanding...

		UTREE_INSERT insert;
		insert.parent = m_item;
		insert.icon = 0L;
		insert.title = gDummyFile;
		insert.data = 0L;
		UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_INSERT, kUMainWidget, &insert);

		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char *err, int errcode)
	{
    cvs_err(err);
    cvs_err("\n");
    cvs_err(strerror(errcode));
    cvs_err("\n");
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char *fullpath)
	{
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
		if(stricmp(name, "cvs") == 0)
#endif
			m_hascvs = true;
		else
			m_empty = false;

		return kSkipFile;
	}
};

// regarding a path and a root item, create an
// item with all the subdirectories inside.
class TBrowserFill : public TraversalReport
{
public:
	UTREEITEM m_root;
	UTREEITEM m_item;
	UCvsFolders *m_treeCtrl;
	CSortList<ENTNODE> & m_entries;
	std::vector<std::string> m_ignlist;
	bool m_isShowIgnored;

	TBrowserFill(UCvsFolders *treeCtrl, UTREEITEM root, UTREEITEM item, CSortList<ENTNODE> & entries) :
			m_root(root), m_item(item), m_treeCtrl(treeCtrl),
			m_entries(entries), m_isShowIgnored(false)
	{
		if( UCvsApp* app = UCvsApp::gApp )
			m_isShowIgnored = app->GetIgnoreModel()->IsShowIgnored();
	}

	virtual ~TBrowserFill() {}

	virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname, const FSSpec * macspec)
	{
		ASSERT(m_item != 0L);

		// assign the entries to this item
		Entries_Open (m_entries, fullpath);
		BuildIgnoredList(m_ignlist, fullpath);

		return kContinueTraversal;
	}

	virtual kTraversal ExitDirectory(const char *fullpath)
	{
		m_ignlist.erase(m_ignlist.begin(), m_ignlist.end());

		UTREE_INFO query;
		query.item = m_item;
		UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
		query.icon = GetIcon(m_treeCtrl, m_item, true /*ignored*/, true, (EntnodeData *)query.data);
		UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_SETICON, kUMainWidget, &query);

		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char *err, int errcode)
	{
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char *fullpath)
	{
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
		if(stricmp(name, "cvs") == 0)
#endif
			return kSkipFile;

		// is the sub-directory ignored ?
		EntnodeData *data = Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);
		if(!m_isShowIgnored && data->IsIgnored())
			return kSkipFile;
		
#if qUnix
		// suppress hidden directories
		if( name[0] == '.')
			return kSkipFile;
#endif

		// create the item for the sub-directory
		TBrowserFillDummy traverse(m_treeCtrl, m_item, 0L, data);
		/*kTraversal res = */FileTraverse(fullname, traverse);

		// assign the icon regarding our Entries info
		if(traverse.m_item != 0L)
		{
			UTREE_INFO query;
			query.item = traverse.m_item;
			query.icon = GetIcon(m_treeCtrl, m_item, true /*ignored*/, false, data);
			UEventSendMessage(m_treeCtrl->GetWidID(), EV_TREE_SETICON, kUMainWidget, &query);
		}

		return kSkipFile;
	}
};

UCvsFolders::UCvsFolders() : UWidget(kUCvsFoldersID)
{
}

UCvsFolders::~UCvsFolders()
{
}

// Process the selection handler command
// param:  handler Command selection handler
// return: true if the command was processed, false otherwise

int UCvsFolders::ProcessSelectionCommand(KiSelectionHandler& handler)
{
  bool res = false;
  UTREEITEM *selItem;
  
	UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &selItem);
	
  if( selItem ) {
    std::string path;
    RetrievePath(selItem, path);
  
    handler.OnFolder(path.c_str());
    res = true;
  } else {
    CvsAlert alert(kCvsAlertWarningIcon, 
                   "Checkout folder required",
                   "Please select a folder before checking out a module",
                   BUTTONTITLE_OK,
                   NULL
                   );
    alert.ShowAlert();  }
  
  return res;
}

void UCvsFolders::OnDestroy(void)
{
	delete this;
}

void UCvsFolders::OnCreate(void)
{
	if(sFolderIcon == 0L)
	{
		sFolderIcon = UCreate_pixmap(this, "folder.xpm");
		sFolderUnknownIcon = UCreate_pixmap(this, "foldunk.xpm");
		sFolderIgnoredIcon = UCreate_pixmap(this, "foldign.xpm");
		sFolderMissingIcon = UCreate_pixmap(this, "foldmiss.xpm");
	}
	std::string newpath;
	newpath = (const char *)gOldLoc.c_str();
	if(newpath.empty())
		newpath = gCvsPrefs.Home();

	// set the initial root
	ResetBrowser(newpath.c_str(), true);	
}

void UCvsFolders::ResetBrowser(const char *path, bool notifyView)
{
	UWaitCursor wait;

	// Must make a copy of 'path' before calling DeleteAllItems(), since
	// this function may have been called with 'm_root' passed as the first
	// parameter (in fact it is in several places in the code) whose value
	// gets deleted by the call to DeleteAllItems() below, making 'path'
	// point to an invalid memory location
	std::string pathCopy(path);
	DeleteAllItems();
	bool isReload = !pathCopy.empty() && !m_root.empty() && stricmp(m_root.c_str(), pathCopy.c_str()) == 0;

	m_root = pathCopy;

	if( pathCopy.empty() )
		return;
	
	if(!isReload)
	{
		if(HasPersistentSettings(m_root.c_str()))
			LoadPersistentSettings(m_root.c_str());
		else
		{
			// check if the path has a CVS folder. If not,
			// we don't want the user to be prompted (like when 
			// WinCvs is starting for the first time).
			std::string cvsFolder(m_root);
			NormalizeFolderPath(cvsFolder);
	
			cvsFolder += "CVS";
			struct stat sb;
			if (stat(cvsFolder.c_str(), &sb) != -1 && S_ISDIR(sb.st_mode))
				AskCreatePersistentSettings(m_root.c_str());
		}
	}

	if(stricmp(gOldLoc.c_str(), m_root.c_str()) != 0)
		gOldLoc = m_root.c_str();

	// update the history combo
	gHistoryLocs.Insert(m_root.c_str());
	UEventSendMessage(kUCvsFrameID, EV_COMBO_RESETALL, UCvsFrame::kDirCombo, 0L);
	const std::vector<std::string> & list = gHistoryLocs.GetList();
	std::vector<std::string>::const_iterator i;
	int pos = 0;
	for(i = list.begin(); i != list.end(); ++i, pos++)
	{
		UEventSendMessage(kUCvsFrameID, EV_COMBO_APPEND,
						  UCvsFrame::kDirCombo, (void *)(const char *)(*i).c_str());
	}
	UEventSendMessage(kUCvsFrameID, EV_COMBO_SETSEL, UMAKEINT(UCvsFrame::kDirCombo, 0), 0L);

	// fill the browser
	TBrowserFillDummy traverse(this, 0L, 0L, 0L);
	/*kTraversal res = */FileTraverse(m_root.c_str(), traverse);

	if(traverse.m_item != 0L)
	{
		UEventSendMessage(GetWidID(), EV_TREE_EXPAND, UMAKEINT(kUMainWidget, 0), traverse.m_item);
		if(notifyView && UCvsApp::gApp->GetFilesView())
			UCvsApp::gApp->GetFilesView()->ResetView(pathCopy, false);
	}
}

void UCvsFolders::ResetView(bool forceReload, bool notifyView)
{
	UWaitCursor doWait;

	std::vector<std::string> allExpanded;
	StoreExpanded(allExpanded);

	bool contentChanged = false;
	std::string selPath;
	UTREEITEM selItem;
	UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &selItem);
	if(selItem != 0L)
		RetrievePath(selItem, selPath);

	if(forceReload)
	{
		ResetBrowser(m_root.c_str());
		contentChanged = true;
	}

	if(contentChanged)
	{
		// restore the expanded paths and the selected item
		std::vector<std::string>::const_iterator i;
		for(i = allExpanded.begin(); i != allExpanded.end(); ++i)
		{
			StepToLocation((*i).c_str());
		}
		if(!selPath.empty())
		{
			selItem = GetItemByLocation(selPath.c_str());
			if(selItem != 0L)
				UEventSendMessage(GetWidID(), EV_TREE_SELECT, UMAKEINT(kUMainWidget, 0), selItem);
		}
	}

	// we don't need to notify the file view if we force the reload
	// because that's already done
	if(notifyView)
	{
		UCvsApp::gApp->GetFilesView()->ResetView(forceReload, false);
	}
	UEventSendMessage(GetWidID(), EV_CHGFOCUS, kUMainWidget, 0L);
}

void UCvsFolders::StoreExpanded(std::vector<std::string> & allExpanded, UTREEITEM root)
{
	UTREE_INFO query;
	query.item = root;
	UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);

	UTREEITEM item = query.item;
	if(item == 0L)
		return;

	if(query.expanded)
	{
		std::string path;
		RetrievePath(item, path);
		allExpanded.push_back(path);
	}

	UTREEITEM childItem = query.child;

	while(childItem != 0L)
	{
		StoreExpanded(allExpanded, childItem);
		query.item = childItem;
		UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
		childItem = query.next;
	}
}

void UCvsFolders::StepToLocation(const char *path, bool notifyView)
{
	std::string root(m_root);
	NormalizeFolderPath(root);
	
	std::string subpath(path);
	NormalizeFolderPath(subpath);

	// check if it is a sub-path
	if(strncmp(root.c_str(), subpath.c_str(), root.length()) != 0)
		return;

	UTREE_INFO query;
	query.item = 0L;
	UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);

	UTREEITEM item = query.item;
	UTREEITEM lastitem = 0L;
	if(item == 0L)
		return;

	// step inside synchronized
	const char *tmp = (const char *)subpath.c_str() + root.length() - 1;
	while((tmp = strchr(tmp, kPathDelimiter)) != 0L)
	{
		const char *name = ++tmp;
		if(name[0] == '\0')
			break;

		std::string subname(name);
		const std::string::size_type pos = subname.find(kPathDelimiter);

		subname.substr(0, pos-1);


		query.item = item;
		UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
		UTREEITEM childItem = query.child;

		// find the subitem which matches this name
		while(childItem != 0L)
		{
			query.item = childItem;
			UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
			EntnodeData *data = (EntnodeData *)query.data;
			if(data != 0L)
			{
#ifdef qUnix
				if(strcmp(((*data)[EntnodeData::kName]).c_str(), subname.c_str()) == 0)
#else
				if(_stricmp((*data)[EntnodeData::kName], subname.c_str()) == 0)
#endif
				{
					// found it !
					lastitem = item = childItem;
					UEventSendMessage(GetWidID(), EV_TREE_EXPAND, UMAKEINT(kUMainWidget, 0), item);
					break;
				}
			}

			childItem = query.next;
		}
	}

	// in case this is called by the view, turn off notifying
	// when the item gets selected
	if(lastitem != 0L)
	{
		if(!notifyView)
		{
			CTempToggleBool toggler(sTemporaryTurnOffNotify);
			UEventSendMessage(GetWidID(), EV_TREE_SELECT, UMAKEINT(kUMainWidget, 0), lastitem);
		}
		else
		{
			UEventSendMessage(GetWidID(), EV_TREE_SELECT, UMAKEINT(kUMainWidget, 0), lastitem);
		}
	}
}

UTREEITEM UCvsFolders::GetItemByLocation(const char *path)
{
	std::string root(m_root);
	NormalizeFolderPath(root);
	
	std::string subpath(path);
	NormalizeFolderPath(subpath);

	// check if it is a sub-path
	if(strncmp(root.c_str(), subpath.c_str(), root.length()) != 0)
		return 0L;

	UTREE_INFO query;
	query.item = 0L;
	UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
	UTREEITEM item = query.item;
	if(item == 0L)
		return 0L;

	// step inside synchronized
	const char *tmp = (const char *)subpath.c_str() + root.length() - 1;
	while((tmp = strchr(tmp, kPathDelimiter)) != 0L)
	{
		const char *name = ++tmp;
		if(name[0] == '\0')
			break;

		std::string subname(name);
		const std::string::size_type pos = subname.find(kPathDelimiter);

		subname.substr(0, pos-1);
			
		UTREEITEM childItem = query.child;

		// find the subitem which matches this name
		while(childItem != 0L)
		{
			query.item = childItem;
			UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
			EntnodeData *data = (EntnodeData *)query.data;
			if(data != 0L)
			{
#if qUnix
				if(strcmp(((*data)[EntnodeData::kName]).c_str(), subname.c_str()) == 0)
#else
				if(_stricmp((*data)[EntnodeData::kName], subname.c_str()) == 0)
#endif
				{
					item = childItem;
					break;
				}
			}

			childItem = query.next;
		}
		if(childItem == 0L)
			return 0L;
	}

	return item;
}

void UCvsFolders::OnTreeExpanding(int collapse, UTREEITEM item)
{
	// many situations make that we enter here twice. Fortunately enough
	// we only dare the first signal.
	static bool sSemaphore = false;
	USemaphore policeman(sSemaphore);
	if(policeman.IsEnteredTwice())
		return;

	UWaitCursor wait;

	UTREE_INFO query;
	query.item = item;
	UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
	EntnodeData *data = (EntnodeData *)query.data;

	// we refuse to expand in this case cause the folder is not really here
  	if(data != 0L && data->IsMissing())
	{
		UEventSendMessage(GetWidID(), EV_TREE_EXPAND, UMAKEINT(kUMainWidget, 1), item);
  		return;
	}

	// erase all the childs
	DeleteAllItems(item);

	std::string path;
	RetrievePath(item, path);

	if(collapse)
	{
		TBrowserFillDummy traverse(this, 0L, item, 0L);
		/*kTraversal res = */FileTraverse(path.c_str(), traverse);
	}
	else
	{
		CSortList<ENTNODE> entries(200, ENTNODE::Compare);
		TBrowserFill traverse(this, 0L, item, entries);
		/*kTraversal res = */FileTraverse(path.c_str(), traverse);

#if qGTK
		// gtk is closing the tree when we removed everything in it
		UEventSendMessage(GetWidID(), EV_TREE_EXPAND, UMAKEINT(kUMainWidget, 0), item);
#endif

		// add the missing folders
		Entries_SetMissing(entries);
		int numEntries = entries.NumOfElements();
		for(int i = 0; i < numEntries; i++)
		{
			const ENTNODE & theNode = entries.Get(i);
			EntnodeData *data = ((ENTNODE *)&theNode)->Data();
			if(!data->IsMissing() || data->GetType() != ENT_SUBDIR)
				continue;
			
			UTREE_INSERT insert;
			insert.parent = item;
			insert.icon = GetIcon(this, item, false, false, data->Ref());
			insert.title = ((*data)[EntnodeData::kName]).c_str();
			insert.data = data;
			UEventSendMessage(GetWidID(), EV_TREE_INSERT, kUMainWidget, &insert);
		}
	}
}

void UCvsFolders::OnTreeSelecting(int deselect, UTREEITEM item)
{
	if(deselect)
		return;

	UTREE_INFO query;
	query.item = item;
	UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
	EntnodeData *data = (EntnodeData *)query.data;
	if(data != 0L && data->IsMissing())
		return;

	UCvsFiles *fileView = UCvsApp::gApp->GetFilesView();
	
	if(!sTemporaryTurnOffNotify && fileView)
	{
		std::string path;	
		RetrievePath(item, path);

		// reset file view only if path changes
		if(fileView->GetPath() != path)
		{
			ASSERT(fileView->IsKindOf(URUNTIME_CLASS(UCvsFiles)));
			fileView->ResetView(path.c_str(), false);
		}
		
		/* recover the focus */
		UEventSendMessage(GetWidID(), EV_CHGFOCUS, kUMainWidget, 0L);
	}
}

void UCvsFolders::SetDefaultRoot(const char *root)
{
	struct stat sb;

	if (stat(root, &sb) == -1 || !S_ISDIR(sb.st_mode))
	{
		cvs_err(_i18n("Cannot access directory '%s' (error %d)\n"), root, errno);
		return;
	}
	
	gOldLoc = root;
}

void UCvsFolders::RetrievePath(UTREEITEM item, std::string& path)
{
	std::string tmp, newPath;
	path = "";

	do
	{
		UTREE_INFO query;
		query.item = item;
		UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
		
		item = query.parent;

		tmp = path;
		newPath = item == 0L ? (const char *)m_root.c_str() : query.title;
		NormalizeFolderPath(newPath);
		newPath += path;
		path = newPath;
	} while(item != 0L);
}

void UCvsFolders::DeleteAllItems(void)
{
	DeleteAllItems(NULL);
}

void UCvsFolders::DeleteAllItems(UTREEITEM root)
{
	// we don't want the file view to be notified when things get deleted
	CTempToggleBool toggler(sTemporaryTurnOffNotify);

	UTREE_INFO query;
	bool eraseRoot = false;
	query.item = root;
	UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
	if(root == 0L)
	{
		root = query.item;
		eraseRoot = true;
	}

	if(root == 0L)
		return;

	UTREEITEM childItem = query.child;

	// erase all the childs
	while(childItem != 0L)
	{
		UTREE_INFO queryChild;
		queryChild.item = childItem;
		UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &queryChild);

		EntnodeData *data = (EntnodeData *)queryChild.data;
		if(data != 0L)
		{
			queryChild.data = 0L; // So that subsequent references after UnRef() won't SEGV
			UEventSendMessage(GetWidID(), EV_TREE_SETDATA, kUMainWidget, &queryChild);
			data->UnRef();	// Or move this one after DeleteAllItems? so we don't loose one system call. For now, safe way
		}

		DeleteAllItems(childItem);

		UEventSendMessage(GetWidID(), EV_TREE_DELETE, kUMainWidget, childItem);
		childItem = queryChild.next;
	}

	// erase the root only if initially root was 0L
	if(eraseRoot)
	{
		EntnodeData *data = (EntnodeData *)query.data;
		ASSERT(data == 0L);

		UEventSendMessage(GetWidID(), EV_TREE_DELETE, kUMainWidget, root);
	}
}

bool UCvsFolders::DisableGeneric()
{
	UCvsApp *app = UCvsApp::gApp;
	return app->IsCvsRunning() || gCvsPrefs.empty() || !HasFocus();
}

void UCvsFolders::OnUpdateGeneric(UCmdUI* pCmdUI, bool needCvsInfos)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}

	UTREEITEM *item;
	UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &item);
	if(item == 0L)
	{
		pCmdUI->Enable(false);
		return;
	}

	std::string path;
	RetrievePath(item, path);
	NormalizeFolderPath(path);
	path += "CVS";

	struct stat sb;
	if (stat(path.c_str(), &sb) == -1 || !S_ISDIR(sb.st_mode))
		pCmdUI->Enable(!needCvsInfos);
	else
		pCmdUI->Enable(needCvsInfos);
}



int UCvsFolders::OnCmdUpdate(void)
{
  KoUpdateHandler handler;
  return ProcessSelectionCommand(handler);
}

void UCvsFolders::OnCmdUIUpdate(UCmdUI *pCmdUI)
{
	OnUpdateGeneric(pCmdUI, true);
}

int UCvsFolders::OnCmdAdd(void)
{
	KoAddHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdCommit(void)
{
	KoCommitHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdQueryUpdate(void)
{
	KoQueryUpdateHandler handler;
	return ProcessSelectionCommand(handler);  
}

int UCvsFolders::OnCmdDiff(void)
{
	KoDiffHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdLog(void)
{
	KoLogHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdStatus(void)
{
	KoStatusHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdLock(void)
{
	KoLockHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdUnlock(void)
{
	KoUnlockHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdWatchOn(void)
{
	KoWatchOnHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdWatchOff(void)
{
	KoWatchOffHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdEdit(void)
{
	KoEditHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdResEdit(void)
{
	KoReservedEditHandler handler;
	return ProcessSelectionCommand(handler);
}

void UCvsFolders::OnCmdUIResEdit(UCmdUI *pCmdUI)
{
	if(gCvsPrefs.CvsVersion() == 1)
		OnCmdUIUpdate(pCmdUI);
}

int UCvsFolders::OnCmdUnedit(void)
{
	KoUneditHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdWatchers(void)
{
	KoWatchersHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdEditors(void)
{
	KoEditorsHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdRelease(void)
{
	KoReleaseHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdTagNew(void)
{
	KoCreateTagHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdTagDelete(void)
{
	KoDeleteTagHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdTagBranch(void)
{
	KoBranchTagHandler handler;
	return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdExplore(void)
{
	UTREEITEM *item;
	UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &item);
	ASSERT(item != 0L);

	std::string path;
	RetrievePath(item, path);

#ifdef WIN32
	HINSTANCE hInst = ShellExecute(*AfxGetMainWnd(), "explore", path,
			0L, 0L, SW_SHOWDEFAULT);
	if((long)hInst < 32)
	{
		cvs_err(_i18n("Unable to explore '%s' (error %d)\n"), (const char *)path, GetLastError());
	}
#endif
#ifdef qUnix
	CvsArgs args(false);
	args.add(gCvsPrefs.Browser());
	args.add(path.c_str());
	UCvsApp::gApp->Execute(args.Argc(), args.Argv());
#endif

	return 0;
}

int UCvsFolders::OnCmdCheckout(void)
{
  KoCheckoutHandler handler;
  return ProcessSelectionCommand(handler);
}

int UCvsFolders::OnCmdImport(void)
{
  KoImportHandler handler;
  return ProcessSelectionCommand(handler);
}

void UCvsFolders::OnCmdUIAdd(UCmdUI *pCmdUI)
{
	OnUpdateGeneric(pCmdUI, false);
}

void UCvsFolders::OnCmdUIExplore(UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}
	UTREEITEM selItem;
	UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &selItem);
	pCmdUI->Enable(selItem != 0L);
}

void UCvsFolders::OnCmdUICheckout(UCmdUI *pCmdUI)
{
	UCvsApp *app = UCvsApp::gApp;
	pCmdUI->Enable(!app->IsCvsRunning() && !gCvsPrefs.empty());
}

void UCvsFolders::OnCmdUIImport(UCmdUI *pCmdUI)
{
	UCvsApp *app = UCvsApp::gApp;
	pCmdUI->Enable(!app->IsCvsRunning() && !gCvsPrefs.empty());
}

void UCvsFolders::OnCmdUIMacrosSel(int cmd, UCmdUI *pCmdUI)
{
	if(DisableGeneric())
	{
		pCmdUI->Enable(false);
		return;
	}
#ifdef USE_PYTHON
	// Fill the cache with current selection
	if( !PyIsUICacheValid() )
	{
		UTREEITEM selItem;
		UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &selItem);
	}
	
	UCmdUI ucmdui((pCmdUI->m_nID));
	ucmdui.pCmdUI = pCmdUI;
	PyDoCmdUI(&ucmdui);
#else
	UTREEITEM selItem;
 	UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &selItem);
 	pCmdUI->Enable(selItem != 0L && CTcl_Interp::IsAvail());
#endif
}

int UCvsFolders::OnCmdMacrosSel(int cmd)
{
#ifdef USE_PYTHON
	PyDoPython(cmd);
#else
 	CTcl_Interp interp;
 	CMacroEntry & entry = gMacrosSel.entries[cmd - cmdSELMACRO];
 	CStr path = entry.path;
 	CTcl_Interp::Unixfy(path);
 
 	UTREEITEM selItem;
 	UEventSendMessage(GetWidID(), EV_TREE_GETSEL, kUMainWidget, &selItem);
 
 	CStr selPath;
 	RetrievePath(selItem, selPath);
 	CStr uppath, folder;
 	SplitPath(selPath, uppath, folder);
 
 	UTREE_INFO query;
 	query.item = selItem;
 	UEventSendMessage(GetWidID(), EV_TREE_GETINFO, kUMainWidget, &query);
 	bool deleteData = false;
 	EntnodeData *data = (EntnodeData *)query.data;
 	if(data == 0L)
 	{
 		deleteData = true;
 		EntnodePath *apath = new EntnodePath(uppath);
 		data = new EntnodeFolder(folder, apath);
 		apath->UnRef();
 	}
 
 	TclBrowserReset();
 	TclBrowserAppend(uppath, data);
 
 	interp.DoScriptVar("source \"%s\"", (const char *)path);
 
 	if(deleteData)
 		data->UnRef();
#endif

	ResetView(true, true);

	return 0;
}
