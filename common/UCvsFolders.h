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

#ifndef UCVSFOLDER_H
#define UCVSFOLDER_H

#include "uwidget.h"
#include "CvsEntries.h"
#include "BrowseViewHandlers.h"

class UCvsFolders : public UWidget
{
	UDECLARE_DYNAMIC(UCvsFolders)
public:
	UCvsFolders();
	virtual ~UCvsFolders();

	void ResetBrowser(const char *path, bool notifyView = false);
		// set the browser root path.

	inline const char *GetRoot(void) const { return m_root.c_str(); }
		// get the base of the browser

	static void SetDefaultRoot(const char *root);
		// set the opening root

	void RetrievePath(UTREEITEM item, std::string & path);
		// given an item of the tree, guess the full path.

	void StepToLocation(const char *path, bool notifyView = false);
		// expand and select the path and optionally synchronize the
		// file view.

	void DeleteAllItems(void);
		// used when resetting and take care about our item data
		// delete all

	void DeleteAllItems(UTREEITEM root);
		// used when resetting and take care about our item data
		// if root != 0L : delete all childs of root (except root)
		// if root == 0L : delete all

	void ResetView(bool forceReload = false, bool notifyView = false);
		// force reload or check modifications before redrawing
		// and optionally synchronize the file view.
		
protected:
	std::string m_root;
		// current path of the browser
		
	void OnUpdateGeneric(UCmdUI* pCmdUI, bool needCvsInfos);
	bool DisableGeneric();
		// used for the handling of EV_UPDTCMD

	void StoreExpanded(std::vector<std::string> & allExpanded, UTREEITEM root = 0L);
	UTREEITEM GetItemByLocation(const char *path);
		// used internally

	int ProcessSelectionCommand(KiSelectionHandler& handler);
    // Process the selection handler command
    // param:  handler Command selection handler
    // return: true if the command was processed, false otherwise

	ev_msg void OnDestroy();
	ev_msg void OnCreate();
	ev_msg void OnTreeExpanding(int collapse, UTREEITEM item);
	ev_msg void OnTreeSelecting(int deselect, UTREEITEM item);
	ev_msg void OnCmdUIMacrosSel(int cmd, UCmdUI *pCmdUI);
	ev_msg void OnCmdUIUpdate(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIAdd(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIExplore(UCmdUI *pCmdUI);
	ev_msg void OnCmdUICheckout(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIImport(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIResEdit(UCmdUI *pCmdUI);
	ev_msg int OnCmdUpdate(void);
	ev_msg int OnCmdAdd(void);
	ev_msg int OnCmdCommit(void);
	ev_msg int OnCmdQueryUpdate(void);
	ev_msg int OnCmdDiff(void);
	ev_msg int OnCmdLog(void);
	ev_msg int OnCmdStatus(void);
	ev_msg int OnCmdLock(void);
	ev_msg int OnCmdUnlock(void);
	ev_msg int OnCmdWatchOn(void);
	ev_msg int OnCmdWatchOff(void);
	ev_msg int OnCmdEdit(void);
	ev_msg int OnCmdResEdit(void);
	ev_msg int OnCmdUnedit(void);
	ev_msg int OnCmdWatchers(void);
	ev_msg int OnCmdEditors(void);
	ev_msg int OnCmdRelease(void);
	ev_msg int OnCmdTagNew(void);
	ev_msg int OnCmdTagDelete(void);
	ev_msg int OnCmdTagBranch(void);
	ev_msg int OnCmdExplore(void);
	ev_msg int OnCmdCheckout(void);
	ev_msg int OnCmdImport(void);
	ev_msg int OnCmdMacrosSel(int cmd);

	UDECLARE_MESSAGE_MAP()
};

#endif
