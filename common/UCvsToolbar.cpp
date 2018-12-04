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

#include "UCvsToolbar.h"
#include "UCvsCommands.h"

UIMPLEMENT_DYNAMIC(UCvsToolbar, UToolbar)

UBEGIN_MESSAGE_MAP(UCvsToolbar, UToolbar)
	ON_UDESTROY(UCvsToolbar)
	ON_UCREATE(UCvsToolbar)
UEND_MESSAGE_MAP()

UCvsToolbar::UCvsToolbar(int widid) : UToolbar(widid)
{
}

UCvsToolbar::~UCvsToolbar()
{
}

void UCvsToolbar::OnDestroy(void)
{
	delete this;
}

void UCvsToolbar::OnCreate(void)
{
}

void CreateMainToolbar(void)
{
	UTB_INIT init;
	init.widthbtn = 16;
	init.heightbtn = 16;
	init.title = "Main Toolbar";
	UEventSendMessage(kUCvsToolbarID, EV_TB_INIT, 0, &init);

	UTB_BUTTON updtBtn = {
		"update.xpm",
		cmdUPDATE,
		DDV_NONE,
		DDD_NONE,
		_i18n("Update the selected items, changes your disk only\nUpdate selected")
	};
	UTB_BUTTON commitBtn = {
		"commit.xpm",
		cmdCOMMIT,
		DDV_NONE,
		DDD_NONE,
		_i18n("Commit the selected items, changes the remote repository\nCommit selected")
	};
	UTB_BUTTON stopBtn = {
		"stop.xpm",
		cmdSTOPCVS,
		DDV_NONE,
		DDD_NONE,
		_i18n("Stop when cvs is running...\nStop cvs...")
	};
	UTB_BUTTON exploreBtn = {
		"explore.xpm",
		cmdEXPLORE,
		DDV_NONE,
		DDD_NONE,
		_i18n("Open an explorer window for this selection\nExplore selection")
	};
	UTB_BUTTON upBtn = {
		"upfolder.xpm",
		cmdUPFOLDER,
		DDV_NONE,
		DDD_NONE,
		_i18n("Go to the upper folder and refresh\nUp one level")
	};
	UTB_BUTTON diffBtn = {
		"diff.xpm",
		cmdDIFF,
		DDV_NONE,
		DDD_NONE,
		_i18n("Print the differences with the CVS remote repository\nDiff selected")
	};
	UTB_BUTTON logBtn = {
		"log.xpm",
		cmdLOG,
		DDV_NONE,
		DDD_NONE,
		_i18n("Print the history of the selected files\nLog selected")
	};
	UTB_BUTTON statusBtn = {
		"status.xpm",
		cmdSTATUS,
		DDV_NONE,
		DDD_NONE,
		_i18n("Print the current status of the selected files\nStatus selected")
	};
	UTB_BUTTON graphBtn = {
		"graph.xpm",
		cmdGRAPH,
		DDV_NONE,
		DDD_NONE,
		_i18n("Print the history as a graph of the selected files\nGraph selected")
	};
	UTB_BUTTON trashBtn = {
		"trash.xpm",
		cmdTRASH,
		DDV_NONE,
		DDD_NONE,
		_i18n("Erase the selection from your disk\nErase selected")
	};
	UTB_BUTTON addBtn = {
		"add.xpm",
		cmdADD,
		DDV_NONE,
		DDD_NONE,
		_i18n("Add the selected items to the remote CVS repository\nAdd selected")
	};
	UTB_BUTTON addbBtn = {
		"addb.xpm",
		cmdADDB,
		DDV_NONE,
		DDD_NONE,
		_i18n("Add the selected binary items to the remote CVS repository\nAdd selected binary")
	};
	UTB_BUTTON rmvBtn = {
		"remove.xpm",
		cmdRMV,
		DDV_NONE,
		DDD_NONE,
		_i18n("Remove the selected items to the remote CVS repository\nRemove selected")
	};
	UTB_BUTTON tagBtn = {
		"tag.xpm",
		cmdTAGNEW,
		DDV_NONE,
		DDD_NONE,
		_i18n("Create/move a tag on the selection\nTag selection")
	};
	UTB_BUTTON untagBtn = {
		"untag.xpm",
		cmdTAGDELETE,
		DDV_NONE,
		DDD_NONE,
		_i18n("Remove a tag on the selection\nUntag selection")
	};
	UTB_BUTTON branchBtn = {
		"branch.xpm",
		cmdTAGBRANCH,
		DDV_NONE,
		DDD_NONE,
		_i18n("Open (fork) a branch on the selection\nFork selection")
	};
	UTB_BUTTON lockBtn = {
		"lock.xpm",
		cmdLOCKF,
		DDV_NONE,
		DDD_NONE,
		_i18n("Lock the selected file on the remote repository until next commit\nLock selection")
	};
	UTB_BUTTON unlockBtn = {
		"unlock.xpm",
		cmdUNLOCKF,
		DDV_NONE,
		DDD_NONE,
		_i18n("Unlock the selected file on the remote repository\nUnlock selection")
	};
	UTB_BUTTON watchBtn = {
		"watch.xpm",
		cmdWATCHON,
		DDV_NONE,
		DDD_NONE,
		_i18n("Put a watch on the selected files\nWatch selection")
	};
	UTB_BUTTON unwatchBtn = {
		"unwatch.xpm",
		cmdWATCHOFF,
		DDV_NONE,
		DDD_NONE,
		_i18n("Release a watch on the selected files\nRelease watch selection")
	};
	UTB_BUTTON editBtn = {
		"edit.xpm",
		cmdEDIT,
		DDV_NONE,
		DDD_NONE,
		_i18n("Edit the selected files\nEdit selection")
	};
	UTB_BUTTON reseditBtn = {
		"res_edit.xpm",
		cmdRESEDIT,
		DDV_NONE,
		DDD_NONE,
		_i18n("Reserved edit the selected files\nReserve edit selection")
	};
	UTB_BUTTON uneditBtn = {
		"unedit.xpm",
		cmdUNEDIT,
		DDV_NONE,
		DDD_NONE,
		_i18n("Unedit the selected files\nUnedit selection")
	};
	UTB_BUTTON releaseBtn = {
		"release.xpm",
		cmdRELEASE,
		DDV_NONE,
		DDD_NONE,
		_i18n("Release the selected files\nRelease selection")
	};

	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &updtBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &commitBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &stopBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &addBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &addbBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &rmvBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &exploreBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &upBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &diffBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &logBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &statusBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &graphBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &trashBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &tagBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &untagBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &branchBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &lockBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &unlockBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &watchBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &unwatchBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &editBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &reseditBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &uneditBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsToolbarID, EV_TB_BUTTON, 0, &releaseBtn);
	UEventSendMessage(kUCvsToolbarID, EV_TB_END, 0, 0L);
}

void CreateBrowseToolbar(void)
{
    //	not used yet
	return;
	
	UTB_INIT init;
	init.widthbtn = 16;
	init.heightbtn = 16;
	init.title = "Browse Toolbar";
	UEventSendMessage(kUCvsBrowseToolbarID, EV_TB_INIT, 0, &init);

	UEventSendMessage(kUCvsBrowseToolbarID, EV_TB_END, 0, 0L);
}

void CreateFilterToolbar(void)
{
	UTB_INIT init;
	init.widthbtn = 16;
	init.heightbtn = 16;
	init.title = "Filter Toolbar";
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_INIT, 0, &init);

	UTB_BUTTON flatBtn = {
		"flat.xpm",
		cmdFLAT,
		DDV_NONE,
		DDD_NONE,
		_i18n("view recursive")
	};
	
	UTB_DROPDOWN viewFilterDropDown = {
		cmdVIEW_FILEMASK,
		0x0,
		_i18n("Enter file mask(s) of files to display")
	};
	
	UTB_BUTTON viewModifiedBtn = {
		"viewModified.xpm",
		cmdVIEW_MODIFIED,
		DDV_NONE,
		DDD_NONE,
		_i18n("view modified files")
	};
	
	UTB_BUTTON viewChangedBtn = {
		"viewChanged.xpm",
		cmdVIEW_CHANGED,
		DDV_NONE,
		DDD_NONE,
		_i18n("view changed files")
	};
	
	UTB_BUTTON viewAddedBtn = {
		"viewAdded.xpm",
		cmdVIEW_ADDED,
		DDV_NONE,
		DDD_NONE,
		_i18n("view added files")
	};
	
	UTB_BUTTON viewRemovedBtn = {
		"viewRemove.xpm",
		cmdVIEW_REMOVED,
		DDV_NONE,
		DDD_NONE,
		_i18n("view removed files")
	};
	
	UTB_BUTTON viewConflictBtn = {
		"viewConflict.xpm",
		cmdVIEW_CONFLICT,
		DDV_NONE,
		DDD_NONE,
		_i18n("view conflict files")
	};
	
	UTB_BUTTON viewUnknownBtn = {
		"viewUnknown.xpm",
		cmdVIEW_UNKNOWN,
		DDV_NONE,
		DDD_NONE,
		_i18n("view unknown files")
	};
	
	UTB_BUTTON hideUnknownBtn = {
		"hideUnknown.xpm",
		cmdHIDE_UNKNOWN,
		DDV_NONE,
		DDD_NONE,
		_i18n("hide unknown files")
	};
	
	UTB_BUTTON viewMissingBtn = {
		"viewMissing.xpm",
		cmdVIEW_MISSING,
		DDV_NONE,
		DDD_NONE,
		_i18n("view only missing files")
	};
	
	UTB_BUTTON viewIgnoredBtn = {
		"viewIgnored.xpm",
		cmdIGNORE,
		DDV_NONE,
		DDD_NONE,
		_i18n("View or hide the cvs ignored files\nIgnore switch")
	};

	UTB_BUTTON reloadBtn = {
		"reload.xpm",
		cmdRELOAD,
		DDV_NONE,
		DDD_NONE,
		_i18n("Refresh the views, updating the contents\nRefresh view")
	};
	
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &flatBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_DROPDOWN, 0, &viewFilterDropDown);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewModifiedBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewChangedBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewAddedBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewRemovedBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewConflictBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewUnknownBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &hideUnknownBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewMissingBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_CHECKBTN, 0, &viewIgnoredBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_SEPARATOR, 0, 0L);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_BUTTON, 0, &reloadBtn);
	UEventSendMessage(kUCvsFilterToolbarID, EV_TB_END, 0, 0L);
}
