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

// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */
#if TARGET_OS_MAC
# include "MacCvsConstant.h"
# include "MacCvsApp.h"
# include "LProgressBar.h"
#endif

#include "ProgressDlg.h"
#include "AppConsole.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CProgress

CProgress::CProgress(bool interruptable, const char* title, kProgressType progressType /*= kProgressBar*/) 
	: m_abort(false)
{
#ifdef WIN32
	m_dlg = new CProgressDlg(m_abort, interruptable, title, progressType);
	if( !m_dlg || !m_dlg->Create() )
	{
		cvs_err("Failed to create Progress dialog\n");
		
		delete m_dlg;
		m_dlg = 0L;
		
		return;
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
#endif /* WIN32 */
#if TARGET_OS_MAC
	m_dlg = new CProgressDlg(m_abort, interruptable, title, progressType);
  if ( m_dlg ) {
    UCursor::SetArrow();
  }
  else {
		cvs_err("Failed to create Progress dialog\n");
  }
#endif
}

CProgress::~CProgress()
{
#ifdef WIN32
	if( m_dlg != 0L )
		m_dlg->DestroyWindow();
#endif /* WIN32 */
#if TARGET_OS_MAC
  delete m_dlg;
#endif
}

/*!
	Set the progress indicator
	\param percent Value, as a percentage, to be set
*/
void CProgress::SetProgress(int percent)
{
#ifdef WIN32
	if( m_dlg == 0L )
		return;

	m_dlg->SetProgress(percent);
	
	if( m_abort )
		m_dlg = 0L;
#endif /* WIN32 */
#if TARGET_OS_MAC
  if ( m_dlg ) m_dlg->SetProgress(percent);
#endif
}

/*!
	Set the progress indicator
	\param text Value, as a text, to be set
*/
void CProgress::SetProgress(const char* text)
{
#ifdef WIN32
	if( m_dlg == 0L )
		return;

	m_dlg->SetProgress(text);
	
	if( m_abort )
		m_dlg = 0L;
#endif /* WIN32 */
#if TARGET_OS_MAC
  if ( m_dlg ) m_dlg->SetProgress(text);
#endif
}

/*!
	Force the progress indicator to abort
	\note Normally the user aborts the operation, use this method in the exceptional cases only
*/
void CProgress::ForceAbort(void)
{
#ifdef WIN32
	if( m_dlg != 0L )
	{
		m_dlg->DestroyWindow();
	}

	m_dlg = 0L;
#endif /* WIN32 */
#if TARGET_OS_MAC
  delete m_dlg;
  m_dlg = NULL;
#endif
}

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

CProgressDlg::CProgressDlg(bool& abort, bool interruptable, 
						   const char* title /*= NULL*/, kProgressType progressType /*= kProgressBar*/, 
						   CWnd* pParent /*=NULL*/)
	: m_abort(abort), m_interruptable(interruptable), 
	m_title(title), m_progressType(progressType), 
	m_pParent(pParent), m_hWndParent(0L),
	CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	//}}AFX_DATA_INIT
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDOK, m_cancelButton);
	DDX_Control(pDX, IDC_PROGRESS, m_progressCtrl);
	DDX_Control(pDX, IDC_PROGRESS_TEXT, m_progressTextStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

/*!
	Create the progress dialog
	\return TRUE on success, FALSE otherwise
*/
BOOL CProgressDlg::Create()
{
	m_hWndParent = PreModal();
	return CDialog::Create(CProgressDlg::IDD, m_pParent);
}

/// OnInitDialog virtual override, enable and show appropriate controls
BOOL CProgressDlg::OnInitDialog() 
{
	CenterWindow();

	CDialog::OnInitDialog();

	// Enable/disable controls
	m_cancelButton.EnableWindow(m_interruptable);

	// Show progress indicator
	switch( m_progressType )
	{
	default:
	case kProgressBar:
		m_progressTextStatic.ShowWindow(SW_HIDE);
		break;
	case kProgressText:
	case kProgressFile:
		m_progressCtrl.ShowWindow(SW_HIDE);

		if( kProgressText == m_progressType )
		{
			m_progressTextStatic.ModifyStyle(0, SS_ENDELLIPSIS);
		}
		else if( kProgressFile == m_progressType )
		{
			m_progressTextStatic.ModifyStyle(0, SS_PATHELLIPSIS);
		}

		break;
	}

	// Set the window title
	if( !m_title.IsEmpty() )
	{
		SetWindowText(m_title);
	}

	// Show the main window
	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
}

/// OnOK virtual override, set the abort flag to true and destroy the progress dialog window
void CProgressDlg::OnOK() 
{
	m_abort = true;
	DestroyWindow();
}

/// PostNcDestroy virtual override, restore the active window and release the allocated memory
void CProgressDlg::PostNcDestroy() 
{
	MSG msg;
	if( m_hWndParent != NULL && ::GetActiveWindow() == m_hWnd )
	{
		::SetActiveWindow(m_hWndParent);
	}

	PostModal();

	while( PeekMessage(&msg, m_hWnd, 0, 0, PM_NOREMOVE) == TRUE )
	{
		AfxGetApp()->PumpMessage();
	}

	delete this;
}

/*!
	Set the progress bar indicator
	\param percent Value to be set
*/
void CProgressDlg::SetProgress(int percent)
{
	m_progressCtrl.SetPos(percent);
	
	CProgressDlg::FlushDisplay(this, m_abort);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
}

/*!
	Set the progress text indicator
	\param text Value to be set
*/
void CProgressDlg::SetProgress(const char* text)
{
	m_progressTextStatic.SetWindowText(text);
	
	CProgressDlg::FlushDisplay(this, m_abort);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
}

/*!
	Peek and pump application messages, flush the messages if the dialog was aborted
	\param dlg Dialog
	\param abort Abort flag
*/
void CProgressDlg::FlushDisplay(const CDialog* dlg, const bool& abort)
{
	MSG msg;
	while( PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE) == TRUE )
	{
		AfxGetApp()->PumpMessage();
	}
	
	while( PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) == TRUE )
	{
		CPoint pt(LOWORD(msg.lParam), HIWORD(msg.lParam));
		::ClientToScreen(msg.hwnd, &pt);
		
		CWnd* wnd = CWnd::WindowFromPoint(pt);
		
		if( !abort && wnd && dlg && (wnd->GetSafeHwnd() == dlg->GetSafeHwnd() || dlg->IsChild(wnd)) )
		{
			AfxGetApp()->PumpMessage();
		}
		else
		{
			PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);
		}
	}
}

/// DoModal virtual override, do modal count
INT_PTR CProgressDlg::DoModal() 
{
	CWincvsApp::CModalGuard modalGuard;
	
	return CDialog::DoModal();
}

#endif	/* WIN32 */

#if TARGET_OS_MAC
CProgressDlg::CProgressDlg(bool& abort, bool interruptable, 
		           const char* title, kProgressType progressType, 
		           LWindow* pParent) : StDialogHandler(dlg_Progress, pParent ? static_cast<LCommander*>(pParent) : static_cast<LCommander*>(CMacCvsApp::gApp)), 
		           mAbort(abort),
		           mInterruptable(interruptable),
		           mTitle(title),
		           mProgressPercent(0),
		           mProgressType(progressType),
		           mNextCheckTicks(TickCount())
{
	if ( !interruptable ) {
	  // disable and hide cancel button
	  mDialog->FindPaneByID(item_ProgressCancel)->Disable();
	  mDialog->FindPaneByID(item_ProgressCancel)->Hide();
	}
	
	if ( title ) {
	  mDialog->FindPaneByID(item_ProgressTitle)->SetDescriptor(mTitle);
	}
	
  // adjust progress bar type
  LProgressBar* progressBar = dynamic_cast<LProgressBar*>(mDialog->FindPaneByID(item_ProgressBar));
  if ( progressBar ) {
    progressBar->SetIndeterminateFlag(progressType != kProgressBar);
    if ( progressType == kProgressBar ) {
      // units are percent
      progressBar->SetMinValue(0);
      progressBar->SetMaxValue(100);
    }
  }
  
/*
  LControlPane*  staticText = dynamic_cast<LControlPane*>(mDialog->FindPaneByID(item_ProgressText));
  if ( staticText ) {
    // make the control truncate the text
    TruncCode   truncation = truncMiddle;
    staticText->SetDataTag(kControlEntireControl, kControlStaticTextTruncTag, sizeof(truncation), &truncation);
  }
*/
  
  mSleepTime = 0; // keep humming
  
  CheckCancel();
}

CProgressDlg::~CProgressDlg()
{
}
  
void CProgressDlg::SetProgress(int percent)
{
  bool    forceCheck = false;
  if ( percent != mProgressPercent ) {
    mProgressPercent = percent;
    mDialog->FindPaneByID(item_ProgressBar)->SetValue(percent);
    forceCheck = true;
  }
  CheckCancel(forceCheck);
}

void CProgressDlg::SetProgress(const char* text)
{
  bool  forceCheck = false;
  if ( mProgressText != text ) {
    LControlPane*        textPane = dynamic_cast<LControlPane*>(mDialog->FindPaneByID(item_ProgressText));
    if ( textPane ) {
      LStr255       truncatedText(static_cast<ConstStringPtr>(mProgressText));
      SDimension16  paneDimension;
      
      textPane->GetFrameSize(paneDimension);
      mProgressText = text;
      {
        StColorState  colorState;
        StTextState   textState;
        UTextTraits::SetPortTextTraits(textPane->GetTextTraitsID());
        ::TruncString(paneDimension.width - 6, truncatedText, truncMiddle);
      }
      textPane->SetDescriptor(truncatedText);
      forceCheck = true;
    }
  }
  CheckCancel(forceCheck);
}

void CProgressDlg::CheckCancel(bool force)
{
  if ( force  ||  TickCount() >= mNextCheckTicks ) {
    mNextCheckTicks = TickCount() + 10; // only check every 10 ticks
    if (  DoDialog() == msg_Cancel  ||  CheckEventQueueForUserCancel() )
      mAbort = true;
  }
}


#endif // TARGET_OS_MAC
