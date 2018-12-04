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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * MacCvsApp.h --- application main
 */

#ifndef MACCVSAPP_H
#define MACCVSAPP_H

#include "MultiString.h"
#include "TextBinary.h"

#include "cvsgui_process.h"

class CLogWindow;
class CLogNode;
class LBWCursorList;
class TCVSToolbar;
class CvsArgs;

#if PP_Debug
	class LDebugMenuAttachment;
#endif

#pragma options align=mac68k
struct CWindowSpec {
	int		pathIndex;
	Boolean	visible;
	Boolean	active;
	Rect	globalBounds;
	};
#pragma options align=reset

class CMacCvsApp	: public LDocApplication
					, public LListener 
					, public LBroadcaster 
{
public:
	CMacCvsApp();
	~CMacCvsApp();
	
	static CMacCvsApp *gApp;
		// the current application

	void SetCvsProcess(CvsProcess* proc) { m_CvsProcess = proc; };
	CvsProcess* GetCvsProcess(void) { return m_CvsProcess; }
		// the running process (if any)

	void SetCvsRunning(bool state);
	inline bool IsCvsRunning(void) { return m_CvsRunning > 0; }
		// when the cvs processus (or thread) is running

	void SetCvsStopping(bool state);
	inline bool IsCvsStopping(void) { return gCvsStopping; }
		// when the cvs processus is stopping (it may take a while)

	LPeriodical*	mLogoutTask;
	
	virtual	void UpdateMenus(void);
	virtual Boolean ObeyCommand(CommandT inCommand, void *ioParam);
	virtual void FindCommandStatus(CommandT inCommand, Boolean &outEnabled,
									  Boolean &outUsesMark, UInt16 &outMark,
									  Str255 outName);
	virtual void ListenToMessage(MessageT inMessage, void *ioParam);
	virtual void ShowAboutBox();
	virtual void ProcessNextEvent();
	virtual void DispatchEvent	(const EventRecord& inMacEvent);
		// LApplication callbacks

	//virtual void ChooseDocument();
	//virtual void OpenDocument(FSSpec *inMacFSSpec);
	inline CLogWindow *GetLogWindow(void) {return mWindow;}
	
	void RefreshLogFont(void);
		// restore the log window font regarding some persistent values

	void RemoveWindowFromListByView(LView * aView);
		// remove a window from the window list using
		// a sub-view or the window itself
	
	void RefreshBrowsers(bool forceReload = false);
		// refresh all the browsers opened
	
	void CloseBrowsers(void);
		// close all the browsers opened
	
	LWindow* NewBrowser(const char *dir = 0L, const CWindowSpec* inSpec = 0, bool inForceNewBrowser=false);
	LWindow* NewBrowser(const UFSSpec & spec, const CWindowSpec* inSpec = 0, bool inForceNewBrowser=false);
		// create a new browser
	
	void NewLogTree(CLogNode *node, const char *dir);
		// create a new log tree
	
	inline bool IsFrontProcess(void) const { return m_IsFrontProcess; }
	
	inline bool HasNotification(void) const { return m_Notif != 0L; }

	bool NotifyUser(const char *msg);
  bool InteractWithUser(SInt32 inTimeoutInTicks=kAEDefaultTimeout);
  
	void SpendSomeTime(void);
	
	static bool CanUseKeychain();

	static const char *GetAppPath(void);
	static const char *GetMacCvsPrefsPath(void);
	static const char *GetAppResourcesPath(void);
	
	void DoSpinCursor(void);
		// do spin since CursorCtl.h doesn't exist anymore
  
  bool CheckForUserCancel(void);
  	// check for user cancel events (but only once every few ticks, so this function can safely be called as often as desired)
  	
	void UpdateBrowserMenu(const char* dir = 0);
		// rebuild last visited browser location
	
	static void SavePersistentSettings(bool quiet = false);

	bool CheckLoopingPassword(const char *key);
	void PushValidPassword(const char *key);
	void FlushValidPasswords(void);
		// we store the password using the keychain and we may
		// enter in an endless loop if the password is not valid anymore
		// (cause ssh will re-ask until the password is valid)
  
   static bool Execute(int argc, char* const* argv);

	virtual Boolean		AttemptQuit( SInt32 inSaveOption );

	virtual void		GetAppName( UStr & name );
	
	short GetToolbarHeight() const;
	Rect GetToolbarGlobalBounds() const;
#if PP_Debug
	void SetSleepTime( UInt32 inSleepTime );
#endif
protected:
	UInt32  m_CvsRunning;
	bool gCvsStopping;
	CvsProcess *m_CvsProcess;
		// the running process (if any)

	CLogWindow *mWindow;
	TArray<LWindow*> mWindowList;
	LBWCursorList *mSpinCursor;
	UInt32 mSpinCounter;
	UInt32  mLastSpinTicks;
	UInt32  mLastUserCancelCheckTicks;
	
	bool m_IsFrontProcess;
	NMRecPtr m_Notif;
	Handle m_NotifIcon;
	TCVSToolbar *m_toolbar;
	SDimension16    m_undockedToolbarSize;
	
	virtual void MakeModelDirector();
		// we make our own AE director cause we want
		// to drive it our way
	
	void UpdateWindowsMenu();
		// rebuild the windows menu according to our
		// windows list
	
	virtual void MakeMenuBar(void);
	virtual void Initialize();
		// from LApplication
	
	void DoCredits(void);
	void DoCopyright(void);
		// commercials

  void DockToolbar(bool dockIt);
  
	virtual void EventResume(const EventRecord &inMacEvent);
	virtual void			EventSuspend	(const EventRecord& inMacEvent);
		// LEventDispatcher

	virtual void			AdjustCursor	(const EventRecord& inMacEvent);

#if PP_Debug
	LDebugMenuAttachment*	mDebugAttachment;
#endif
};

extern CMString sBrowserLoc;

#endif /* MACCVSAPP_H */
