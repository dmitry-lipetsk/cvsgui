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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

// wincvs.h : main header file for the WINCVS application
//

#if !defined(AFX_WINCVS_H__D2D77DC7_8299_11D1_8949_444553540000__INCLUDED_)
#define AFX_WINCVS_H__D2D77DC7_8299_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "MultiString.h"
#include "SmartComboBox.h"
#include "cvsgui_process.h"
#include "BrowseViewModel.h"

/// View update hint
typedef enum {
	kNullHint = 0,		/*!< No hint */
	kGraphFontChanged	/*!< Font changed in the graph view */
} tUpdateHint;

void AFXAPI DDV_CheckPathExists(CDataExchange* pDX, int nIDC, CString& value, bool promptForCreation = true);
void AFXAPI DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars);
void AFXAPI DDV_CheckCVSROOT(CDataExchange* pDX, CString& value);
void AFXAPI DDX_ComboMString(CDataExchange* pDX, int nIDC, CSmartComboBox& combo);

/////////////////////////////////////////////////////////////////////////////
// CWincvsApp:
// See wincvs.cpp for the implementation of this class
//

class CWincvsView;
class CWinCvsBrowser;
class CBrowseFileView;
class CMainFrame;
class CStatusBar;
class KiFilterModel;
class KiRecursionModel;
class KiIgnoreModel;

/// WinCvs application
class CWincvsApp : public CWinApp
{
public:
	// Construction
	CWincvsApp();
	virtual ~CWincvsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWincvsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL SaveAllModified();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:


// Implementation

	//{{AFX_MSG(CWincvsApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppCopyrights();
	afx_msg void OnAppCredits();
	afx_msg void OnLogWindowIsPresent(CCmdUI* pCmdUI);
	afx_msg void OnStopcvs();
	afx_msg void OnUpdateStopcvs(CCmdUI* pCmdUI);
	afx_msg void OnHelpCvs();
	//}}AFX_MSG
	afx_msg void OnFileNew();

	DECLARE_MESSAGE_MAP()
private:
	// Data members
	DWORD m_idletime;
	
	_CvsProcess* m_CvsProcess;		/*!< Current CVS process */
	bool m_CvsRunning;				/*!< CVS command is running */
	bool m_CvsStopping;				/*!< CVS command is about to stop */

	NotificationManager m_notificationManager;	/*!< Notification manager */
	bool m_isBackgroundProcess;		/*!< Flag to indicate that the application is in the background */
	int m_modalCount;				/*!< Modal dialog entries count */

	CMultiDocTemplate* m_logTempl;	/*!< Browse file view document */
	CMultiDocTemplate* m_logGraph;	/*!< Graph view document */

	bool m_hasTCL;					/*!< Flag indicating whether the TCL library is loaded */
	bool m_hasPY;					/*!< Flag indicating whether the Python library is loaded */
	
	// Methods
	void ShowTipAtStartup(void);
	void ShowTipOfTheDay(void);
	
public:
	// Interface
	inline CMultiDocTemplate* GetLogGraph();

	inline bool IsBackgroundProcess(void) const;
	inline void SetBackgroundProcess(const bool isBackgroundProcess);
	NotificationManager* GetNotificationManager();

	inline bool IsModalDialog(void) const;
	inline void IncrementModalCount();
	inline void DecrementModalCount();

	CWincvsView* GetConsoleView(void) const;

	CWinCvsBrowser* GetBrowserView(void) const;
	void ActivateBrowserView(void);
		
	inline CMainFrame* GetMainFrame(void) const;
	CCJShellTree* GetExplorerView(void) const;
	CBrowseFileView* GetFileView(void) const;
	CDocument* GetDocument(void) const;
	CStatusBar* GetStatusBar() const;

	CString GetRoot() const;

	void PeekPumpAndIdle(bool doIdle);

	void SetCvsProcess(CvsProcess* proc);
	CvsProcess* GetCvsProcess(void) const;

	void SetCvsRunning(bool state);
	bool IsCvsRunning(void) const;

	void SetCvsStopping(bool state);
	bool IsCvsStopping(void) const;

	void UpdateAllDocsView(CView* pSender = NULL, tUpdateHint hint = kNullHint, CObject* pHint = NULL);

	inline DWORD GetIdleTime(void) const;
	inline void ResetIdleTime(void);
	
	void IdleCvsProgress(void);
    
	KiFilterModel* GetFilterModel() const;
	KiRecursionModel* GetRecursionModel() const;
	KiIgnoreModel* GetIgnoreModel() const;

	const char* GetEnvMainThread(const char* name);

	void StartScriptEngine(const bool forceReload = false);
	void StopScriptEngine();
	void RestartScriptEngine();

	bool HasTCL() const;
	bool HasPython() const;

	/// Modal dialog count helper
	class CModalGuard
	{
	public:
		// Constructor
		CModalGuard();
		~CModalGuard();
	};
};

//////////////////////////////////////////////////////////////////////////
// Inline implementation

/*!
	Get the graph document
	\return The graph document
*/
inline CMultiDocTemplate* CWincvsApp::GetLogGraph()
{
	return m_logGraph;
}

/*!
	Test whether the application is in background
	\return true if app is in the background, false otherwise
*/
inline bool CWincvsApp::IsBackgroundProcess(void) const
{
	return m_isBackgroundProcess;
}

/*!
	Set the application background status flag
	\param isBackgroundProcess Application background status flag
*/
inline void CWincvsApp::SetBackgroundProcess(const bool isBackgroundProcess)
{
	m_isBackgroundProcess = isBackgroundProcess;
	m_notificationManager.NotifyAll();
}

/*!
	Check whether the application is displaying the modal dialog
	\return true if modal dialog is being displayed, false otherwise
*/
inline bool CWincvsApp::IsModalDialog(void) const
{
	return m_modalCount > 0;
}

/*!
	Increment modal dialog count
*/
inline void CWincvsApp::IncrementModalCount()
{
	m_modalCount++;
}

/*!
	Decrement modal dialog count
*/
inline void CWincvsApp::DecrementModalCount()
{
	m_modalCount--;
	
	if( m_modalCount < 0 )
	{
		m_modalCount = 0;
	}
}

/*!
	Get the main frame
	\return The main frame, may be NULL
*/
inline CMainFrame* CWincvsApp::GetMainFrame(void) const
{
	return (CMainFrame*)AfxGetApp()->GetMainWnd();
}

/*!
	Set the current CVS process
	\param proc current CVS process
*/
inline void CWincvsApp::SetCvsProcess(CvsProcess* proc)
{
	m_CvsProcess = proc;
}

/*!
	Get the current CVS process
	\return Current CVS process
*/
inline CvsProcess* CWincvsApp::GetCvsProcess(void) const
{
	return m_CvsProcess;
}

/*!
	Set the CVS running state
	\param state State
*/
inline void CWincvsApp::SetCvsRunning(bool state)
{
	m_CvsRunning = state;
}

/*!
	Get the CVS running state
	\return The CVS running state
*/
inline bool CWincvsApp::IsCvsRunning(void) const
{
	return m_CvsRunning;
}

/*!
	Set the CVS stopping state
	\param state CVS stopping state
*/
inline void CWincvsApp::SetCvsStopping(bool state)
{
	m_CvsStopping = state;
}

/*!
	Get the CVS stopping state
	\return The CVS stopping state
*/
inline bool CWincvsApp::IsCvsStopping(void) const
{
	return m_CvsStopping;
}

/*!
	Get the idle time
	\return the idle time
*/
inline DWORD CWincvsApp::GetIdleTime(void) const
{
	return m_idletime;
}

/*!
	Reset idle time
*/
inline void CWincvsApp::ResetIdleTime(void)
{
	m_idletime = 0;
}

/*!
	Check the TCL library flag
	\return true if TCL is loaded, false otherwise
*/
inline bool CWincvsApp::HasTCL() const
{
	return m_hasTCL;
}

/*!
	Check the Python library flag
	\return true if Python is loaded, false otherwise
*/
inline bool CWincvsApp::HasPython() const
{
	return m_hasPY;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCVS_H__D2D77DC7_8299_11D1_8949_444553540000__INCLUDED_)
