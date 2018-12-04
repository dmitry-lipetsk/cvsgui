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

#ifndef UCVSFRAME_H
#define UCVSFRAME_H

#include "uwidget.h"
#include "UCvsCommands.h"
#include "BrowseViewModel.h"

#define NUM_FILEMASKS	20

class UCvsFrame : public UWidget
{
	UDECLARE_DYNAMIC(UCvsFrame)
public:
	UCvsFrame();
	virtual ~UCvsFrame();

	bool AskForQuitting(void);
		// true if we can quit the application

	enum
	{
		kDirCombo = EV_COMMAND_START,	// 0
		kBrowserPopup					// 1
	};
	
// Attributes
protected:
	KoFilterModel m_filter;
	KoRecursionModel m_recursion;
	KoIgnoreModel m_ignore;
	
	bool InUpdateUI;
	
public:
	// KiFilterModel implementation
	KiFilterModel* GetFilterModel(void)
	{
		return &m_filter;
	}

	// KiRecursionModel implementation
	KiRecursionModel* GetRecursionModel(void)
	{
		return &m_recursion;
	}

	// KiRecursionModel implementation
	KiIgnoreModel* GetIgnoreModel(void)
	{
		return &m_ignore;
	}

	void OnFilterMaskEntered(void);
	bool FreshenFileMask(void);
	
	ev_msg int OnCmdIgnore(void);
	
protected:
	ev_msg void OnCmdUIAnytime(int cmd, UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewRecursive(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewIgnored(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIFilterMask(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewAdded(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewModified(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewConflict(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewUnknown(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewMissing(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewRemoved(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIViewChanged(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIHideUnknown(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIReload(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIQuit(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIIgnore(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIStopcvs(UCmdUI *pCmdUI);
	ev_msg void OnCmdUICvsCmd(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIMacroAdmin(int cmd, UCmdUI *pCmdUI);
	ev_msg void OnCmdUIPyMacro(int cmd, UCmdUI *pCmdUI);
	ev_msg void OnCreate(void);
	ev_msg void OnDestroy(void);
	ev_msg void OnCmdUIUseTclShell(UCmdUI *pCmdUI);
	ev_msg void OnCmdUIUsePyShell(UCmdUI *pCmdUI);
	ev_msg int OnQuit(void);
	ev_msg int OnCopyrights(void);
	ev_msg int OnCredits(void);
	ev_msg int OnAbout(void);
	ev_msg void OnComboSelDir(int pos, const char *txt);
	ev_msg int OnBrowserPopup(void);
	ev_msg int OnCmdPreferences(void);
	ev_msg int OnCmdSaveSettings(void);
	ev_msg int OnCmdRtagNew(void);
	ev_msg int OnCmdRtagDelete(void);
	ev_msg int OnCmdRtagBranch(void);
	ev_msg int OnCmdUseTclShell(void);
	ev_msg int OnCmdUsePyShell(void);
	ev_msg int OnCmdCmdline(void);
	ev_msg int OnCmdLogin(void);
	ev_msg int OnCmdLogout(void);
	ev_msg int OnCmdStopcvs(void);
	ev_msg int OnCmdInit(void);
	ev_msg int OnCmdMacroAdmin(int cmd);
	ev_msg int OnViewIgnored(void);
	ev_msg int OnViewRecursive(void);
	ev_msg int OnViewAdded(void);
	ev_msg int OnViewModified(void);
	ev_msg int OnViewConflict(void);
	ev_msg int OnViewUnknown(void);
	ev_msg int OnViewMissing(void);
	ev_msg int OnViewRemoved(void);
	ev_msg int OnViewChanged(void);
	ev_msg int OnHideUnknown(void);
	ev_msg int OnCmdReload(void);
	UDECLARE_MESSAGE_MAP()
};

// little class to get the cursor to wait during the existence of the object
class UWaitCursor
{
public:
	UWaitCursor()
	{
		UEventSendMessage(kUCvsFrameID, EV_CHGCURSOR, UMAKEINT(kUMainWidget, uCursWatch), 0L);
	}
	virtual ~UWaitCursor()
	{
		UEventSendMessage(kUCvsFrameID, EV_CHGCURSOR, UMAKEINT(kUMainWidget, uCursArrow), 0L);
	}
};

#endif
