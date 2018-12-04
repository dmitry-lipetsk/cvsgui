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
* Author : V.Antonevich <v_antonevich@hotmail.com> --- November 2000
*/

/*
* FileViewWatcher.h --- file modification watcher thread
*/

#ifndef __FILEVIEWWATCHWER_H__
#define __FILEVIEWWATCHWER_H__

/// File modification watcher thread
class KoWatcher : public CObject
{
// Construction
public:
	KoWatcher(HWND hWnd);
private:
	~KoWatcher();

private:
	/// Handles index enum
	enum
	{
		kStartHandle = 0,	/*!< Start handle index */
		kTerminateHandle,	/*!< Terminate handle index */
		kActivateHandle,	/*!< Application activate handle index */
		kNotifyHandle,		/*!< Notify handle index */
		kEndHandle			/*!< End handle index */
	};

	// Data members
	CEvent m_startEvent;			/*!< Signals that watcher thread shall start watching */
	CEvent m_terminateEvent;		/*!< Signals that watcher thread should terminate */
	CEvent m_activateEvent;			/*!< Signals that watcher thread about application active status change */

	TCHAR m_path[_MAX_PATH];		/*!< Path to watch for changes */
	HWND m_hWnd;					/*!< Window to post notifications to */
	bool m_isRecursive;				/*!< Flag to tell whether to watch for recursive changes */
	
	DWORD m_notificationTime;		/*!< Store the last notification tick count to assure refreshes under heave load */
	int m_rapidNotificationCount;	/*!< Count rapid notifications */

	HANDLE m_handles[kEndHandle];	/*!< Objects being watched */
	DWORD m_count;					/*!< Count of handles being watched */

	CCriticalSection m_section;		/*!< Synchronization object */

	// Methods
	static UINT DelegateRun(LPVOID pWatcher);

	UINT Run();
	bool StartWatch();
	void StopWatch();
	
	BOOL Notify();
	DWORD GetTimeout(const bool ignoreTimeout = false);

	void ResetNotificationTime();

// Operations
public:
	// Interface

	/// Messages posted out
	enum 
	{ 
		WM_FILECHANGE = WM_APP + 51 /*!< Private message for delivering file change notification */
	};

	void SignalStartWatch(LPCTSTR path, bool isRecursive);
	void SignalTerminate();

	void OnActiveChanged();
};



#endif
// __FILEVIEWWATCHWER_H__