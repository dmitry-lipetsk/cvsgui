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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com>
 */

#if !defined(AFX_PROGRESSDLG_H__09D80542_6467_11D3_85F5_00609419FCC1__INCLUDED_)
#define AFX_PROGRESSDLG_H__09D80542_6467_11D3_85F5_00609419FCC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDialog.h : header file
//

#include "ustr.h"


/// Progress indicator type enum
typedef enum
{
	kProgressBar,	/*!< Progress bar as the progress indicator */
	kProgressText,	/*!< Text as the progress indicator */
	kProgressFile	/*!< File name as the progress indicator */
} kProgressType;

#if defined(WIN32) || TARGET_OS_MAC
class CProgressDlg;
#endif /* WIN32 ||ÊTARGET_OS_MAC */

//////////////////////////////////////////////////////////////////////////
// CProgress

/// Progress indicator wrapper class
class CProgress
{
public:
	// Construction
	CProgress(bool interruptable, const char* title = 0L, kProgressType progressType = kProgressBar);
	virtual ~CProgress();

private:
	// Data members
	bool m_abort;	/*!< Flag indicating that the operation was cancelled by the user */

#if defined(WIN32) || TARGET_OS_MAC
	CProgressDlg* m_dlg;	/*!< Progress dialog */
#endif /* WIN32 ||ÊTARGET_OS_MAC */

	// Methods
	void ForceAbort(void);

public:
	// Interface

	/*!
		Check the value of the abort flag
		\return true if the operation was aborted, false otherwise
	*/
	inline bool HasAborted(void) const { return m_abort; }

	void SetProgress(int percent);
	void SetProgress(const char* text);
};

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

/// Progress dialog, should be dynamically allocated
class CProgressDlg : public CDialog
{
public:
// Construction
	CProgressDlg(bool& abort, bool interruptable, 
		const char* title = NULL, kProgressType progressType = kProgressBar, 
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS };
	CButton	m_cancelButton;
	CProgressCtrl	m_progressCtrl;
	CStatic	m_progressTextStatic;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool& m_abort;					/*!< Flag indicating that the operation was cancelled by the user */
	bool m_interruptable;			/*!< Flag indicating that the operation can be stopped by pressing the Cancel button */
	CString m_title;				/*!< Progress dialog title */
	kProgressType m_progressType;	/*!< Progress dialog type */

	CWnd* m_pParent;				/*!< Parent dialog (used at the creation time */
	HWND m_hWndParent;				/*!< Parent window handle (activated after progress dialog closes */

	// Methods
	static void FlushDisplay(const CDialog* dlg, const bool& abort);

public:
	// Interface
	BOOL Create();

	void SetProgress(int percent);
	void SetProgress(const char* text);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif	/* WIN32 */

#if TARGET_OS_MAC
class CProgressDlg : public StDialogHandler
{
public:
  CProgressDlg(bool& abort, bool interruptable, 
		           const char* title = NULL, kProgressType progressType = kProgressBar, 
		           LWindow* pParent = NULL);
  ~CProgressDlg();
  
	void SetProgress(int percent);
	void SetProgress(const char* text);
	
protected:
  bool&           mAbort;           /*!< Flag indicating that the operation was cancelled by the user */
	bool            mInterruptable;		/*!< Flag indicating that the operation can be stopped by pressing the Cancel button */
	CPStr           mTitle;				    /*!< Progress dialog title */
	CPStr           mProgressText;    /*!< Last progress text set */
	int             mProgressPercent; /*!< Last progress percentage set */
	kProgressType   mProgressType;	  /*!< Progress dialog type */
  UInt32          mNextCheckTicks;  /*!< Time to run next check */
	LWindow         *mParentWindow;				  /*!< Parent window */
	
	void CheckCancel(bool force=false);
};
#endif // TARGET_OS_MAC
#endif // !defined(AFX_PROGRESSDLG_H__09D80542_6467_11D3_85F5_00609419FCC1__INCLUDED_)
