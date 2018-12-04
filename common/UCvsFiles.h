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

#ifndef UCVSFILES_H
#define UCVSFILES_H

#include "uwidget.h"
#include "CvsEntries.h"
#include "BrowseViewHandlers.h"
#include "BrowseViewModel.h"
#include "BrowseViewColumn.h"

class UCvsFiles : public UWidget
{
	UDECLARE_DYNAMIC(UCvsFiles)
public:
	UCvsFiles();
	virtual ~UCvsFiles();

    void OnFilteringChanged();
        // called to notify the view that app's filtering has changed

	void ResetView(std::string path, bool notifyBrowser);
		// reset to this path and notify optionally the directories tree

	void ResetView(const char *path, bool notifyBrowser);
		// reset to this path and notify optionally the directories tree

	void ResetView(bool forceReload, bool notifyBrowser);
		// - reload and try to guess if something changed
		// according to the mod. time of CVS
		// - notify optionally the directories tree

	static void *GetImageForEntry(EntnodeData *data);
		// return the icon of the image which symbolizes the
		// state of the file/directory

	inline bool IsSortAscendant(void) { return m_ascendant; }

	// retrieves columnar model of the data
	KiColumnModel* GetColumnModel() const
	{
		return m_modelColumns;
	}

protected:
	void GetEntriesModTime(time_t & newEntriesMod, time_t & newEntriesLogMod);
		// used internally to get the time stamp of the CVS/Entries

	void EditSel(EntnodeData *data, bool useDefault = false);
		// used internally to open a file

	void Resort(void);
		// resort according to the current key

	CSortList<ENTNODE> m_entries;
		// CVS/Entries infos
	std::string m_path;
		// current path
	time_t m_entriesMod;
	time_t m_entriesLogMod;
		// time stamp of CVS/
	int m_sort;
		// sort key
	bool m_ascendant;
		// sort flag

	KiColumnModel* m_modelColumns;
		// columnar model in use

	bool DisableGeneric(bool needSelection);
	bool DisableGeneric(void);
		// used for the handling of EV_UPDTCMD

	int Search(const char *title);
  
	int ProcessSelectionCommand(KiSelectionHandler& handler);

	void SaveColumns(void);
		// saves column state

	bool LoadColumns(void);
		// loads column state

	void ShowColumns(void);
		// show all columns of the actual model

	void HideColumns(void);
		// hide all columns of the actual model

	void SetRecursive(bool isRecursive);

	bool m_isRecursive;
		// true if displaying recursively

	ev_msg void OnDestroy();
	ev_msg void OnCreate();
	ev_msg int OnCmdUpdate(void);
	ev_msg int OnCmdAdd(void);
	ev_msg int OnCmdAddb(void);
	ev_msg int OnCmdCommit(void);
	ev_msg int OnCmdRmv(void);
	ev_msg int OnCmdQueryUpdate(void);
	ev_msg int OnCmdUpone(void);
	ev_msg int OnCmdTrash(void);
	ev_msg int OnCmdDiff(void);
	ev_msg int OnCmdLog(void);
	ev_msg int OnCmdGraph(void);
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
	ev_msg int OnCmdEditsel(void);
	ev_msg int OnCmdEditseldef(void);
	ev_msg int OnMacroSel(int cmd);
	ev_msg int OnViewRecursive(void);
	ev_msg void OnCmdUIUpdate(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIAdd(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIAddB(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIRelease(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIRmv(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIGraph(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIEditseldef(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIResEdit(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIUpone(UCmdUI *pCmdUI);
	ev_msg void OnCmdUITrash(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIExplore(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIMacroSel(int cmd, UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewRecursive(UCmdUI *pCmdUI);
	ev_msg void OnDblClick(int);
	ev_msg void OnPopup(void);
	ev_msg void OnSelColumn(int column);
	
	UDECLARE_MESSAGE_MAP()
public:
	// member access
	const std::string& GetPath() const { return m_path; };
};

#endif
