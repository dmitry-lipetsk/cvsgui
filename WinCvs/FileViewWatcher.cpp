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
* FileViewWatcher.cpp --- file modification watcher thread
*/

#include "stdafx.h"
#include "wincvs.h"
#include "FileViewWatcher.h"
#include "CvsPrefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Foreground timeout period - fast notifications (background timeout is configurable)
#define WATCHER_FOREGROUND_TIMEOUT	1000

/// Factor to ignore the notifications for
#define WATCHER_IGNORE_FACTOR		0.8f

/// Factor to detect rapid notifications
#define WATCHER_RAPID_FACTOR		1.2f

/// Threshold to reduce rapid notifications
#define WATCHER_RAPID_THRESHOLD		5

/*!
	Application active status changed notification function
	\param obj Watcher to be notified
*/
static void OnActiveChanged(CObject* obj)
{
	((KoWatcher*)obj)->OnActiveChanged();
}

//////////////////////////////////////////////////////////////////////////
// KoWatcher

KoWatcher::KoWatcher(HWND hWnd)
	: m_isRecursive(false), 
	m_hWnd(hWnd), 
	m_notificationTime(0)
{
	m_rapidNotificationCount = 0;
	ResetNotificationTime();

	m_handles[kStartHandle] = m_startEvent;
	m_handles[kTerminateHandle] = m_terminateEvent;
	m_handles[kActivateHandle] = m_activateEvent;
	m_handles[kNotifyHandle] = INVALID_HANDLE_VALUE;
	
	m_count = kEndHandle - 1;
	*m_path = 0;
	
	// Start the thread immediately
	AfxBeginThread(DelegateRun, (LPVOID)this);
}

/// Private destructor - SignalTerminate() must be called to destroy this object
KoWatcher::~KoWatcher()
{
}

/*!
	Delegate control to non-static member
	\param pWatcher Watcher object
	\return true on success, false otherwise
*/
UINT KoWatcher::DelegateRun(LPVOID pWatcher)
{
	KoWatcher* watcher = (KoWatcher*)pWatcher;
	return watcher->Run();
}

/*!
	Responds to events until requested to terminate
	\return Zero
*/
UINT KoWatcher::Run()
{
	DWORD timeout = INFINITE;

	for( ; ; )
	{
		const DWORD wait = WaitForMultipleObjects(m_count, m_handles, FALSE, timeout);
		switch( wait )
		{
		case WAIT_OBJECT_0 + kStartHandle:
			{
				// Process signal to start watching
				StopWatch();
				StartWatch();
				
				m_startEvent.Unlock();
				timeout = INFINITE;
				break;
			}
		case WAIT_OBJECT_0 + kTerminateHandle:
			{
				// Process signal to terminate
				StopWatch();
				delete this;
				return 0;
			}
		case WAIT_OBJECT_0 + kActivateHandle:
			{
				const bool backgroundProcess = ((CWincvsApp*)AfxGetApp())->IsBackgroundProcess();
				if( !backgroundProcess && timeout != INFINITE )
				{
					// Just activated - refresh right away!
					Notify();
					timeout = INFINITE;
				}
			}
			break;
		case WAIT_OBJECT_0 + kNotifyHandle:
			{
				StopWatch();
				timeout = GetTimeout();

				if( GetTickCount() - m_notificationTime > timeout )
				{
					Notify();
					timeout = INFINITE;
				}
				else
				{
					StartWatch();

					static HANDLE ignoreHandles[2] = { m_handles[kActivateHandle], m_handles[kTerminateHandle] };
					const DWORD ignoreTimeout = GetTimeout(true);

					// Ignore some possible notifications to reduce the thread's load, refresh on activation
					const DWORD ignoreWait = WaitForMultipleObjects(2, ignoreHandles, FALSE, ignoreTimeout);
					switch( ignoreWait )
					{
					case WAIT_OBJECT_0: // Activated, refresh
						Notify();
						timeout = INFINITE;
						break;
					case WAIT_OBJECT_0 + 1:
						// Process signal to terminate
						StopWatch();
						delete this;
						return 0;
					case WAIT_TIMEOUT:
						timeout /= 2;
						break;
					}
				}

				m_startEvent.Unlock();
				break;
			}
		case WAIT_TIMEOUT:
			{
				StopWatch();
				Notify();
				
				timeout = INFINITE;
				break;
			}
		}
	}

	return 0;
}

/*!
	Signal to start watching for file changes in particular destination
	\param path Path to watch
	\param isRecursive true to watch recursively, false otherwise
*/
void KoWatcher::SignalStartWatch(LPCTSTR path, bool isRecursive)
{
	// Store parameters
	m_section.Lock();
	
	if( stricmp(m_path, path) != 0 )
	{
		ResetNotificationTime();
	}

	_tcscpy(m_path, path);
	m_isRecursive = isRecursive;

	m_section.Unlock();
	m_startEvent.SetEvent();
}

/*!
	Set signal to terminate
*/
void KoWatcher::SignalTerminate()
{
	m_terminateEvent.SetEvent();
}

/*!
	Start monitoring
	\return true on success, false otherwise
*/
bool KoWatcher::StartWatch()
{
	// Retrieve parameters first
	TCHAR path[_MAX_PATH];
	bool isRecursive;
	
	m_section.Lock();
	
	lstrcpyn(path, m_path, _MAX_PATH);
	isRecursive = m_isRecursive;
	
	m_section.Unlock();

	// Start watching
	const HANDLE notifyHandle = FindFirstChangeNotification(path, 
		isRecursive,
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		//FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE |
		//FILE_NOTIFY_CHANGE_LAST_ACCESS |
#if _MSC_VER >= 1200
		//FILE_NOTIFY_CHANGE_CREATION | (makes Win98 fail)
#endif
		//FILE_NOTIFY_CHANGE_SECURITY |
		FILE_NOTIFY_CHANGE_ATTRIBUTES);

	if( notifyHandle != INVALID_HANDLE_VALUE )
	{
		m_handles[kNotifyHandle] = notifyHandle;
		m_count = kEndHandle;
	}

	// Register for notifications on active status change
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->GetNotificationManager()->CheckIn(this, ::OnActiveChanged);
	}

	return m_count == kEndHandle;
}

/*!
	Stop monitoring
*/
void KoWatcher::StopWatch()
{
	// Unregister for notifications on active status change
	if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
	{
		app->GetNotificationManager()->CheckOut(this);
	}
	
	if( m_count == kEndHandle && m_handles[kNotifyHandle] != INVALID_HANDLE_VALUE )
	{
		// Reset and close the handle
		FindNextChangeNotification(m_handles[kNotifyHandle]); 
		FindCloseChangeNotification(m_handles[kNotifyHandle]);
		m_handles[kNotifyHandle] = INVALID_HANDLE_VALUE;
		m_count = kNotifyHandle;
	}
}

/*!
	Notify about the file change
	\return TRUE on success, FALSE otherwise
*/
BOOL KoWatcher::Notify()
{
	const DWORD notificationTime = m_notificationTime;
	
	m_notificationTime = GetTickCount();
	if( m_notificationTime - notificationTime < WATCHER_FOREGROUND_TIMEOUT * WATCHER_RAPID_FACTOR )
	{
		m_rapidNotificationCount++;
	}
	else
	{
		m_rapidNotificationCount--;
		if( m_rapidNotificationCount < 0 )
		{
			m_rapidNotificationCount = 0;
		}
	}

	return PostMessage(m_hWnd, WM_FILECHANGE, 0, 0);
}

/*!
	Get the timeout value
	\param ignoreTimeout true to get the ignore timeout, false to get the notification timeout
	\return Timeout value
*/
DWORD KoWatcher::GetTimeout(const bool ignoreTimeout /*= false*/)
{
	const bool backgroundProcess = ((CWincvsApp*)AfxGetApp())->IsBackgroundProcess();

	DWORD timeout = backgroundProcess ? gCvsPrefs.RefreshInactiveTimeOut() : WATCHER_FOREGROUND_TIMEOUT;

	if( ignoreTimeout )
	{
		timeout = DWORD((float)timeout * WATCHER_IGNORE_FACTOR);
		if( !backgroundProcess && m_rapidNotificationCount > WATCHER_RAPID_THRESHOLD )
		{
			timeout *= 2;
		}
	}

	return timeout;
}

/*!
	Reset the notification timeout so that the next file modification triggers instant notification
*/
void KoWatcher::ResetNotificationTime()
{
	m_notificationTime = ::GetTickCount() - max(WATCHER_FOREGROUND_TIMEOUT, gCvsPrefs.RefreshInactiveTimeOut()) - 1;
}

/*!
	Notify watcher about application active status change
*/
void KoWatcher::OnActiveChanged()
{
	m_activateEvent.PulseEvent();
}
