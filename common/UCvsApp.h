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

#ifndef UCVSAPP_H
#define UCVSAPP_H

#ifdef ENABLE_NLS
#	include "cvsgui_i18n.h"
#endif

#include "uwidget.h"
#include "cvsgui_process.h"
#include "BrowseViewModel.h"

class UCvsConsole;
class UCvsFrame;
class UCvsFiles;
class UCvsFolders;
class UCvsStatusbar;

class UCvsApp : public UWidget
{
	UDECLARE_DYNAMIC(UCvsApp)
public:
	UCvsApp(int argc, char **argv);
	virtual ~UCvsApp();

	static UCvsApp *gApp;
		// a pointer on the pseudo application

	void SayWelcome(void);
		// make the greetings and ask for the CVSROOT if necessary

	inline void SetCvsProcess(CvsProcess* proc) { gCvsProcess = proc; };
	inline CvsProcess *GetCvsProcess(void) { return gCvsProcess; }
		// the cvs processus (or thread) when running
	
	void SetCvsRunning(bool state);
	inline bool IsCvsRunning(void) { return gCvsRunning; }
		// when the cvs processus (or thread) is running

	void SetCvsStopping(bool state);
	inline bool IsCvsStopping(void) { return gCvsStopping; }
		// when the cvs processus is stopping (it may take a while)

	
	void SetStatusBarText(const char*);
		// set new statusbar text
	
	UCvsStatusbar *GetStatusBar(void);
		// get the statusbar

	UCvsFolders *GetBrowserView(void);
		// get the left pane browser tree

	UCvsFiles *GetFilesView(void);
		// get the right pane file list

	UCvsConsole *GetConsole(void);
		// get the right pane console

	const char *GetAppName(void) const;
	const char *GetAppLibFolder(void) const;
	const char *GetAppVersion(void) const;

	void Execute(int argc, char * const *argv);
	
	void PeekPumpAndIdle(bool);

	KiFilterModel* GetFilterModel() const;
		// retrieve filtering model to use
	
	KiRecursionModel* GetRecursionModel() const;
		// retrieve recursion model to use
	
	KiIgnoreModel* GetIgnoreModel() const;
		// ignore model in use
	
	UCvsFrame* GetMainFrame(void) { return m_mainFrame; }
		// get the document
	
	void StartScriptEngine(const bool forceReload = false);
		// Start the script engine
	
	void StopScriptEngine();
		// Stop the script engine
	
	void RestartScriptEngine();
		// Restart the script engine
	
	enum
	{
		kProcessTimer = EV_COMMAND_START
	};

protected:
	UCvsFrame* m_mainFrame;
	
	// please use public function to change or read
	bool gCvsRunning;
	bool gCvsStopping;

	bool m_hasTCL;					/*!< Flag indicating whether the TCL library is loaded */
	bool m_hasPY;					/*!< Flag indicating whether the Python library is loaded */

	// the running process (if any)
	static CvsProcess *gCvsProcess;
	
	ev_msg void OnMainTimer(long time);

	UDECLARE_MESSAGE_MAP()
};

bool IsAppEnabled(void);
bool IsEscKeyPressed(void);
#endif
