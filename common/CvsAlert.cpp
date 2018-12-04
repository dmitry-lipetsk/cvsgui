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

// CvsAlert.cpp : implementation file
//

#include "stdafx.h"

#ifdef TARGET_OS_MAC
#	include <Dialogs.h>
#	include <TextUtils.h>
#	include <string.h>
#	include "MacMisc.h"
# include "MacCvsApp.h"
# include "UModalDialogs.h"
#endif /* TARGET_OS_MAC */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef WIN32
#	include "wincvs.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#include "CvsAlert.h"
#include "Persistent.h"

/// Default CvsAlert dialog title
#ifdef WIN32 
#	define DEFAULT_ALERT_TITLE _i18n("WinCvs Alert");
#endif
#ifdef TARGET_OS_MAC
#	define DEFAULT_ALERT_TITLE "";
#endif
#if qUnix
#	define DEFAULT_ALERT_TITLE _i18n("gCvs Alert");
#endif

#ifdef TARGET_OS_MAC
namespace {
  pascal Boolean _ModalFilter(DialogRef theDialog, EventRecord *theEvent, DialogItemIndex *itemHit);
  ModalFilterUPP _ModalFilterUPP(NewModalFilterUPP(_ModalFilter));
}
#endif

//////////////////////////////////////////////////////////////////////////
//{ CvsAlert

CvsAlert::CvsAlert(const kCvsAlertIconType icon, 
				   const char* message, const char* messageDetails /*= NULL*/,
				   const char* okButtonTitle /*= BUTTONTITLE_OK*/, const char* cancelButtonTitle /*= BUTTONTITLE_CANCEL*/)
{
	m_icon = icon;
	
	m_message = message ? message : "";
	m_messageDetails = messageDetails ? messageDetails : "";
	
	m_okButtonTitle = okButtonTitle ? okButtonTitle : "";
	m_cancelButtonTitle = cancelButtonTitle ? cancelButtonTitle : "";
	
	if( icon == kCvsAlertNoteIcon )
	{
		// Note needs one button only
		m_cancelButtonTitle.erase();
	}

	m_pHideOption = NULL;
	
	m_alertTitle = DEFAULT_ALERT_TITLE;
}

/*!
	Display an alert with 1-3 buttons and standard icons
	\param defaultButton Default button
	\result The ID of the button that was pressed
*/
kCvsAlertButtonType CvsAlert::ShowAlert(const kCvsAlertButtonType defaultButton /*= kCvsAlertOKButton*/)
{
#if defined(WIN32) || defined(qUnix)
	CCVSAlert_MAIN cvsAlertDlg(this, defaultButton);

#ifdef WIN32
	if( CvsPrompt* pCvsPrompt = dynamic_cast<CvsPrompt*>(this) )
	{
		cvsAlertDlg.m_prompt = pCvsPrompt->GetValue().c_str();
	}
#endif

	const kCvsAlertButtonType res = (kCvsAlertButtonType)cvsAlertDlg.DoModal();

#ifdef WIN32
	if( CvsPrompt* pCvsPrompt = dynamic_cast<CvsPrompt*>(this) )
	{
		pCvsPrompt->SetValue(cvsAlertDlg.m_prompt);
	}
#endif

	return res;
#endif /* WIN32 */

#ifdef TARGET_OS_MAC
  // make sure user knows we need input
	CMacCvsApp::gApp->InteractWithUser();
	// and then display a standard alert
  AlertStdCFStringAlertParamRec       alertParams;
  kCvsAlertButtonType                 result(kCvsAlertCancelButton);
  SInt16                              itemHit(kAlertStdAlertCancelButton);
  if ( GetStandardAlertDefaultParams(&alertParams, kStdCFStringAlertVersionOne) == noErr )
  {
      CFStringRef     okButtonTitle(NULL);
      CFStringRef     cancelButtonTitle(NULL);
      CFStringRef     otherButtonTitle(NULL);
      CFStringRef     primaryMessage(m_message.empty() ? NULL : CFStringCreateWithCString(NULL, m_message.c_str(), CFStringGetSystemEncoding()));
      CFStringRef     secondaryMessage(m_messageDetails.empty() ? NULL : CFStringCreateWithCString(NULL, m_messageDetails.c_str(), CFStringGetSystemEncoding()));
      
      /* first option for using the title: prefix the message with the title - not quite the real thing...
      if ( !m_alertTitle.empty() )
      {
        CFStringRef   alertTitle(CFStringCreateWithCString(NULL, m_alertTitle.c_str(), CFStringGetSystemEncoding()));
        if ( alertTitle )
        {
          if ( primaryMessage )
          {
            CFStringRef message(primaryMessage);
            primaryMessage = CFStringCreateWithFormat(NULL, NULL, message ? CFSTR("%@:\n%@") : CFSTR("%@:"), alertTitle, message);
            if ( message ) CFRelease(message);
          }
        }
      }
      */
      
      // set up button title CFStrings
      if ( !m_okButtonTitle.empty()  &&  m_okButtonTitle != BUTTONTITLE_OK ) 
        okButtonTitle = CFStringCreateWithCString(NULL, m_okButtonTitle.c_str(), CFStringGetSystemEncoding());
      if ( !m_cancelButtonTitle.empty()  &&  m_cancelButtonTitle != BUTTONTITLE_CANCEL ) 
        cancelButtonTitle = CFStringCreateWithCString(NULL, m_cancelButtonTitle.c_str(), CFStringGetSystemEncoding());
      if ( !m_otherButtonTitle.empty() ) 
        otherButtonTitle = CFStringCreateWithCString(NULL, m_otherButtonTitle.c_str(), CFStringGetSystemEncoding());
      
      // customize alert params      
      alertParams.movable = true;
      alertParams.defaultText = m_okButtonTitle != BUTTONTITLE_OK ? okButtonTitle : (CFStringRef)kAlertDefaultOKText;
      alertParams.cancelText = m_cancelButtonTitle != BUTTONTITLE_CANCEL ? cancelButtonTitle : (CFStringRef)kAlertDefaultCancelText;
      alertParams.otherText = otherButtonTitle;
      // set button behaviors
      switch ( defaultButton )
      {
        case kCvsAlertOKButton: alertParams.defaultButton = kAlertStdAlertOKButton; break;
        case kCvsAlertCancelButton: alertParams.defaultButton = kAlertStdAlertCancelButton; break;
        case kCvsAlertOtherButton: alertParams.defaultButton = kAlertStdAlertOtherButton; break;
      }
      // cancel button can only behave as either cancel _or_ default button
      if ( alertParams.cancelText != NULL  &&  defaultButton != kCvsAlertCancelButton )
         alertParams.cancelButton = kAlertStdAlertCancelButton;

      // translate alert type
      AlertType       alertType;
      switch ( m_icon )
      {
        case kCvsAlertStopIcon:
          alertType = kAlertStopAlert;
          break;
        case kCvsAlertNoteIcon:
          alertType = kAlertNoteAlert;
          break;
        case kCvsAlertWarningIcon:
        case kCvsAlertQuestionIcon:
          alertType = kAlertCautionAlert;
          break;
        case kCvsAlertNoIcon:
        default:
          alertType = kAlertPlainAlert;
      }
      
      // create and run the alert
      DialogRef       alert;
      if ( CreateStandardAlert(alertType, primaryMessage, secondaryMessage, &alertParams, &alert) == noErr )
      {
      /* another option for using the title: actually set the window title - unfortunately, this is against the Apple UI guidelines
        if ( !m_alertTitle.empty() )
        {
          CFStringRef title(CFStringCreateWithCString(NULL, m_alertTitle.c_str(), CFStringGetSystemEncoding()));
          if (title ) {
            SetWindowTitleWithCFString(GetDialogWindow(alert), title);
            CFRelease(title);
          }
        }
      */
        RunStandardAlert(alert, _ModalFilterUPP, &itemHit);
      }
      
      // clean up strings
      if ( okButtonTitle ) CFRelease(okButtonTitle);
      if ( cancelButtonTitle ) CFRelease(cancelButtonTitle);
      if ( otherButtonTitle ) CFRelease(otherButtonTitle);
      if ( primaryMessage ) CFRelease(primaryMessage);
      if ( secondaryMessage ) CFRelease(secondaryMessage);
  }
  // translate button result
  switch ( itemHit )
  {
    case kAlertStdAlertOKButton: return kCvsAlertOKButton;
    case kAlertStdAlertCancelButton: return kCvsAlertCancelButton;
    default:
      return kCvsAlertOtherButton;
  }
#endif /* TARGET_OS_MAC */

	return defaultButton;
}

/*!
	Set the alert title
	\param alertTitle Alert title
*/
void CvsAlert::SetAlertTitle(const char* alertTitle)
{
	m_alertTitle = alertTitle ? alertTitle : "";
}

/*!
	Set the button title
	\param buttonType Button type to set the title for
	\param buttonTitle Button title
*/
void CvsAlert::SetButtonTitle(const kCvsAlertButtonType buttonType, const char* buttonTitle)
{
	if( !buttonTitle )
	{
		buttonTitle = "";
	}

	switch( buttonType )
	{
	case kCvsAlertOKButton:
		m_okButtonTitle = buttonTitle;
		break;
	case kCvsAlertCancelButton:
		m_cancelButtonTitle = buttonTitle;
		break;
	case kCvsAlertOtherButton:
		m_otherButtonTitle = buttonTitle;
		break;
	default:
#ifdef WIN32
		ASSERT(FALSE); // Incorrect button type
#endif
		break;
	}
}

/*!
	Set the hide option
	\param pHideOption Hide option
*/
void CvsAlert::SetHideOption(CPersistentBool* pHideOption)
{
	m_pHideOption = pHideOption;
}

/*!
	Get the icon type
	\return The icon type
*/
kCvsAlertIconType CvsAlert::GetIcon() const
{
	return m_icon;
}

/*!
	Get the message
	\return The message
*/
std::string CvsAlert::GetMessage() const
{
	return m_message;
}

/*!
	Get the message details
	\return The message details
*/
std::string CvsAlert::GetMessageDetails() const
{
	return m_messageDetails;
}

/*!
	Get the alert title
	\return The alert title 
*/
std::string CvsAlert::GetAlertTitle() const
{
	return m_alertTitle;
}

/*!
	Get the alert title
	\param buttonType Button type to get the title for
	\return The alert title 
*/
std::string CvsAlert::GetButtonTitle(const kCvsAlertButtonType buttonType) const
{
	switch( buttonType )
	{
	case kCvsAlertOKButton:
		return m_okButtonTitle;
	case kCvsAlertCancelButton:
		return m_cancelButtonTitle;
	case kCvsAlertOtherButton:
		return m_otherButtonTitle;
	default:
#ifdef WIN32
		ASSERT(FALSE); // Incorrect button type
#endif
		break;
	}

	return "";
}

/*!
	Get the hide option
	\return The hide option
*/
CPersistentBool* CvsAlert::GetHideOption() const
{
	return m_pHideOption;
}
//} CvsAlert

//////////////////////////////////////////////////////////////////////////
// CvsPrompt

CvsPrompt::CvsPrompt(const kCvsAlertIconType icon, 
					 const char* message, const char* messageDetails /*= NULL*/,
					 const char* okButtonTitle /*= BUTTONTITLE_OK*/, const char* cancelButtonTitle /*= BUTTONTITLE_CANCEL*/)
					 : CvsAlert(icon, message, messageDetails, okButtonTitle, cancelButtonTitle)
{
}

/*!
	Set the value
	\param value Value
*/
void CvsPrompt::SetValue(const char* value)
{
	m_value = value;
}

/*!
	Get the value
	\return The value
*/
std::string CvsPrompt::GetValue() const
{
	return m_value;
}

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
//{ CCVSAlert_MAIN dialog


CCVSAlert_MAIN::CCVSAlert_MAIN(const CvsAlert* pcvsAlert, const kCvsAlertButtonType defaultButton, CWnd* pParent /*=NULL*/)
	: m_pcvsAlert(pcvsAlert), CDialog(CCVSAlert_MAIN::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCVSAlert_MAIN)
	m_messageDetails = "";
	m_alertMessage = "";
	m_bOptionChecked = m_pcvsAlert->GetHideOption() ? (bool)(*m_pcvsAlert->GetHideOption()) : FALSE;
	m_prompt = _T("");
	//}}AFX_DATA_INIT

	m_defaultButton = defaultButton;
}


void CCVSAlert_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCVSAlert_MAIN)
	DDX_Control(pDX, IDC_PROMPT, m_promptEdit);
	DDX_Control(pDX, IDC_OPTIONCHECK_BUTTON, m_optionCheck);
	DDX_Control(pDX, IDC_ALERT_MESSAGE, m_alertMessageEdit);
	DDX_Control(pDX, IDC_MESSAGE_DETAILS, m_messageDetailsEdit);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, IDC_OTHER_BUTTON, m_otherButton);
	DDX_Control(pDX, IDC_ICON_STATIC, m_iconStatic);
	DDX_Text(pDX, IDC_MESSAGE_DETAILS, m_messageDetails);
	DDX_Text(pDX, IDC_ALERT_MESSAGE, m_alertMessage);
	DDX_Check(pDX, IDC_OPTIONCHECK_BUTTON, m_bOptionChecked);
	DDX_Text(pDX, IDC_PROMPT, m_prompt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCVSAlert_MAIN, CDialog)
	//{{AFX_MSG_MAP(CCVSAlert_MAIN)
	ON_BN_CLICKED(IDC_OTHER_BUTTON, OnOtherButton)
	ON_BN_CLICKED(IDC_OPTIONCHECK_BUTTON, OnOptioncheckButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCVSAlert_MAIN message handlers

/// OnInitDialog virtual override, arrange buttons
BOOL CCVSAlert_MAIN::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Extra initialization
	ASSERT(m_pcvsAlert != NULL); // No alert info class

	if( m_pcvsAlert )
	{
		SetIcon();
		SetTitle();
		SetMessages();
		Resize();
		SetButtonTitles();
		SetAccelerators();
		SetDefaultButton();
		SetHideOptionCheck();
		SetPrompt();

		UpdateData(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/*!
	Set the icon
*/
void CCVSAlert_MAIN::SetIcon()
{
	ASSERT(m_pcvsAlert != NULL); // Bad pointer

	switch( m_pcvsAlert->GetIcon() )
	{
	case kCvsAlertStopIcon:
		m_iconStatic.SetIcon(AfxGetApp()->LoadIcon(IDI_MB_STOP));
		break;
	case kCvsAlertNoteIcon:
		m_iconStatic.SetIcon(AfxGetApp()->LoadIcon(IDI_MB_INFORMATION));
		break;
	default:
		// Fall thru
	case kCvsAlertWarningIcon:
		m_iconStatic.SetIcon(AfxGetApp()->LoadIcon(IDI_MB_EXCLAMATION));
		break;
	case kCvsAlertQuestionIcon:
		m_iconStatic.SetIcon(AfxGetApp()->LoadIcon(IDI_MB_QUESTION));
		break;
	}
}

/*!
	Set dialog title
*/
void CCVSAlert_MAIN::SetTitle()
{
	if( !m_pcvsAlert->GetAlertTitle().empty() )
	{
		SetWindowText(m_pcvsAlert->GetAlertTitle().c_str());
	}
}

/*!
	Set the message and message details
*/
void CCVSAlert_MAIN::SetMessages()
{
	// Change the font to Bold for alert message control
	CFont* pFont = m_alertMessageEdit.GetFont();
	LOGFONT logFont = { 0 };
	
	pFont->GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;
	m_alertMessageFont.CreateFontIndirect(&logFont);

	m_alertMessageEdit.SetFont(&m_alertMessageFont);
	
	// Set messages
	m_alertMessage = m_pcvsAlert->GetMessage().c_str();
	
	if( !m_pcvsAlert->GetMessageDetails().empty() )
	{
		m_messageDetails += m_pcvsAlert->GetMessageDetails().c_str();
	}
	else
	{
		m_messageDetailsEdit.EnableWindow(FALSE);
		m_messageDetailsEdit.ShowWindow(SW_HIDE);
	}

	// Fix the newline characters
	m_alertMessage.Replace("\n", "\r\n");
	m_messageDetails.Replace("\n", "\r\n");
}

/*!
	Get the window text extent
	\param wnd Window
	\param text Text
	\return The window text extent as CSize
	\note Current window font and fonts metrics are taken into account
*/
CSize CCVSAlert_MAIN::GetWndTextExtent(CWnd* wnd, const char* text)
{
	CSize res(0, 0);

	CDC* pDC = wnd->GetDC();
	ASSERT(pDC != NULL); // Invalid device context
	
	if( pDC )
	{
		CFont* pOldFond = pDC->SelectObject(wnd->GetFont());
		TEXTMETRIC textMetric = { 0 };

		pDC->GetTextMetrics(&textMetric);
		res = pDC->GetTextExtent(text);
		res.cx += textMetric.tmOverhang;
		
		if( pOldFond )
		{
			pDC->SelectObject(pOldFond);
		}
	}

	return res;
}

/*!
	Calculate the window text size
	\param wnd Window
	\param text Text
	\param width The width of the window rectangle
	\return The window text size as CSize
	\note Current window font is taken into account
*/
CSize CCVSAlert_MAIN::CalcWndTextSize(CWnd* wnd, const char* text, const int width)
{
	CSize res(0, 0);

	CDC* pDC = wnd->GetDC();
	ASSERT(pDC != NULL); // Invalid device context
	
	if( pDC )
	{
		CFont* pOldFond = pDC->SelectObject(wnd->GetFont());
		CRect rect;

		wnd->GetClientRect(rect);
		rect.right += width - rect.Width();

		pDC->DrawText(text, -1, rect, DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX | DT_WORDBREAK);
		res = rect.Size();
		
		if( pOldFond )
		{
			pDC->SelectObject(pOldFond);
		}
	}

	return res;
}

/*!
	Resize the dialog to fit the alert message if necessary
*/
void CCVSAlert_MAIN::Resize()
{
	int horizAdj = 0;
	int vertAdj = 0;
	int vertPromptAdj = 0;

	// Calculate adjustments
	{
		// Horizontal
		const CSize alertSize = GetWndTextExtent(&m_alertMessageEdit, m_pcvsAlert->GetMessage().c_str());
		
		CRect alertWndRect;
		m_alertMessageEdit.GetWindowRect(alertWndRect);
		
		if( alertSize.cx > alertWndRect.Width() )
		{
			horizAdj = alertSize.cx - alertWndRect.Width();
		}

		// Vertical
		CRect messageDetailsWndRect;
		m_messageDetailsEdit.GetWindowRect(messageDetailsWndRect);

		if( !m_pcvsAlert->GetMessageDetails().empty() )
		{
			const CSize messageDetailsSize = CalcWndTextSize(&m_messageDetailsEdit, 
				m_pcvsAlert->GetMessageDetails().c_str(), 
				messageDetailsWndRect.Width() + horizAdj);

			if( messageDetailsSize.cy > messageDetailsWndRect.Height() )
			{
				vertAdj = messageDetailsSize.cy - messageDetailsWndRect.Height();
			}
		}
		else
		{
			// Moving all controls up
			vertAdj -= messageDetailsWndRect.top - alertWndRect.bottom; 
			vertAdj -= messageDetailsWndRect.Height();
		}
		
		// Prompt
		if( !dynamic_cast<const CvsPrompt*>(m_pcvsAlert) )
		{
			CRect promptWndRect;
			m_promptEdit.GetWindowRect(promptWndRect);
			
			// Moving all controls up
			vertPromptAdj -= promptWndRect.top - messageDetailsWndRect.bottom;
			vertPromptAdj -= promptWndRect.Height();
		}
	}

	// Resize dialog and controls
	CRect rect;
	
	// Resize the alert message
	m_alertMessageEdit.GetWindowRect(rect);
	m_alertMessageEdit.SetWindowPos(NULL, 0, 0, rect.Width() + horizAdj, rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	
	// Resize the message details
	m_messageDetailsEdit.GetWindowRect(rect);
	m_messageDetailsEdit.SetWindowPos(NULL, 0, 0, rect.Width() + horizAdj, rect.Height() + vertAdj, SWP_NOMOVE | SWP_NOZORDER);
	
	// Resize the prompt
	m_promptEdit.GetWindowRect(rect);
	ScreenToClient(rect);
	m_promptEdit.SetWindowPos(NULL, rect.left, rect.top + vertAdj, rect.Width() + horizAdj, rect.Height(), SWP_NOZORDER);
	
	// Accommodate for any prompt adjustment
	vertAdj += vertPromptAdj;

	// Resize the dialog
	GetWindowRect(rect);
	SetWindowPos(NULL, 0, 0, rect.Width() + horizAdj, rect.Height() + vertAdj, SWP_NOMOVE | SWP_NOZORDER);
	
	// Move the OK button
	m_okButton.GetWindowRect(rect);
	ScreenToClient(rect);
	m_okButton.SetWindowPos(NULL, rect.left + horizAdj, rect.top + vertAdj, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	// Move the Other button
	m_otherButton.GetWindowRect(rect);
	ScreenToClient(rect);
	m_otherButton.SetWindowPos(NULL, rect.left + horizAdj, rect.top + vertAdj, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	// Move the Cancel button
	m_cancelButton.GetWindowRect(rect);
	ScreenToClient(rect);
	m_cancelButton.SetWindowPos(NULL, rect.left + horizAdj, rect.top + vertAdj, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	// Move the hide option checkbox
	m_optionCheck.GetWindowRect(rect);
	ScreenToClient(rect);
	m_optionCheck.SetWindowPos(NULL, rect.left, rect.top + vertAdj, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Adjust the edge of the window
	if( !m_pcvsAlert->GetHideOption() )
	{
		CRect okButtonRect;
		m_okButton.GetWindowRect(okButtonRect);

		CRect optionCheckRect;
		m_optionCheck.GetWindowRect(optionCheckRect);

		GetWindowRect(rect);
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height() - (optionCheckRect.bottom - okButtonRect.bottom), SWP_NOMOVE | SWP_NOZORDER);
	}
}

/*!
	Set the buttons titles and layout
*/
void CCVSAlert_MAIN::SetButtonTitles()
{
	if( !m_pcvsAlert->GetButtonTitle(kCvsAlertOKButton).empty() )
	{
		m_okButton.SetWindowText(m_pcvsAlert->GetButtonTitle(kCvsAlertOKButton).c_str());
	}
	
	if( !m_pcvsAlert->GetButtonTitle(kCvsAlertCancelButton).empty() )
	{
		m_cancelButton.SetWindowText(m_pcvsAlert->GetButtonTitle(kCvsAlertCancelButton).c_str());
	}
	else
	{
		// Hide the Cancel button (but keep it enabled so OnCancel can be called)
		//m_cancelButton.EnableWindow(FALSE);
		m_cancelButton.ShowWindow(SW_HIDE);
		
		CRect rect;
		
		// Move the Other button to fill in the gap
		m_cancelButton.GetWindowRect(rect);
		ScreenToClient(rect);
		m_otherButton.MoveWindow(rect);
		
		// Move the OK button to fill in the gap
		m_otherButton.GetWindowRect(rect);
		ScreenToClient(rect);
		m_okButton.MoveWindow(rect);
	}
	
	if( !m_pcvsAlert->GetButtonTitle(kCvsAlertOtherButton).empty() )
	{
		m_otherButton.SetWindowText(m_pcvsAlert->GetButtonTitle(kCvsAlertOtherButton).c_str());
	}
	else
	{
		// Hide the extra button
		m_otherButton.EnableWindow(FALSE);
		m_otherButton.ShowWindow(SW_HIDE);
		
		// Move the OK button to fill in the gap
		CRect rect;
		m_otherButton.GetWindowRect(rect);
		ScreenToClient(rect);
		m_okButton.MoveWindow(rect);
	}
}

/*!
	Set the accelerators
*/
void CCVSAlert_MAIN::SetAccelerators()
{
	SetButtonAccelerator(m_okButton);
	SetButtonAccelerator(m_otherButton);
	SetButtonAccelerator(m_cancelButton);
}

/*!
	Set the default and initially focused button
*/
void CCVSAlert_MAIN::SetDefaultButton()
{
	UINT defID = 0;
	HWND defHwnd = NULL;

	switch( m_defaultButton )
	{
	case kCvsAlertOKButton:
		if( m_okButton.IsWindowEnabled() )
		{
			defID = m_okButton.GetDlgCtrlID();
			defHwnd = m_okButton.GetSafeHwnd();
		}
		break;
	case kCvsAlertCancelButton:
		if( m_cancelButton.IsWindowEnabled() )
		{
			defID = m_cancelButton.GetDlgCtrlID();
			defHwnd = m_cancelButton.GetSafeHwnd();
		}
		break;
	case kCvsAlertOtherButton:
		if( m_otherButton.IsWindowEnabled() )
		{
			defID = m_otherButton.GetDlgCtrlID();
			defHwnd = m_otherButton.GetSafeHwnd();
		}
		break;
	default:
		ASSERT(FALSE); // Incorrect button type
		break;
	}

	if( defID )
	{
		SetDefID(defID);
	}

	if( defHwnd )
	{
		PostMessage(WM_NEXTDLGCTL, (WPARAM)defHwnd, (LPARAM)TRUE);
	}
}

/*!
	Set the button accelerator
	\param button Button to set the accelerator for
*/
void CCVSAlert_MAIN::SetButtonAccelerator(CButton& button)
{
	CString buttonTitle;
	button.GetWindowText(buttonTitle);

	if( buttonTitle.CompareNoCase(BUTTONTITLE_OK) && buttonTitle.CompareNoCase(BUTTONTITLE_CANCEL) )
	{
		buttonTitle = "&" + buttonTitle;
		button.SetWindowText(buttonTitle);
	}
}

/*!
	Set the hide option checkbox
*/
void CCVSAlert_MAIN::SetHideOptionCheck()
{
	// Set the hide option text
	if( m_pcvsAlert->GetHideOption() )
	{
		m_optionCheck.SetWindowText(FormatHideOptionText());
	}
	else
	{
		m_optionCheck.ShowWindow(SW_HIDE);
		m_optionCheck.EnableWindow(FALSE);
	}
}

/*!
	Show or hide the prompt edit box
*/
void CCVSAlert_MAIN::SetPrompt()
{
	if( !dynamic_cast<const CvsPrompt*>(m_pcvsAlert) )
	{
		m_promptEdit.EnableWindow(FALSE);
		m_promptEdit.ShowWindow(SW_HIDE);
	}
}

/// OnOK virtual override, set the return value
void CCVSAlert_MAIN::OnOK() 
{
 	CDialog::OnOK();

	EndDialog(kCvsAlertOKButton);
}

/// OnCancel virtual override, set the return value
void CCVSAlert_MAIN::OnCancel() 
{
	CDialog::OnCancel();

	EndDialog(kCvsAlertCancelButton);
}

/// BN_CLICKED message handler, set the return value
void CCVSAlert_MAIN::OnOtherButton() 
{
	EndDialog(kCvsAlertOtherButton);
}

/// DoModal virtual override, do modal count
INT_PTR CCVSAlert_MAIN::DoModal() 
{
	CWincvsApp::CModalGuard modalGuard;
	
	return !m_bOptionChecked || (m_bOptionChecked && IsHideCommandKeyPressed()) ? CDialog::DoModal() : kCvsAlertOKButton;
}

/// BN_CLICKED message handler, update hide option
void CCVSAlert_MAIN::OnOptioncheckButton() 
{
	if( m_pcvsAlert->GetHideOption() )
	{
		*m_pcvsAlert->GetHideOption() = m_optionCheck.GetCheck() ? true : false;
	}
}
#endif /* WIN32 */

#ifdef qUnix

CCVSAlert_MAIN::CCVSAlert_MAIN(const CvsAlert* pcvsAlert, const kCvsAlertButtonType defaultButton) 
    : UWidget(::UEventGetWidID()), m_pcvsAlert(pcvsAlert)
{
	m_messageDetails = "";
	m_alertMessage = "";

	m_defaultButton = defaultButton;
	
	OnInitDialog();
}


void CCVSAlert_MAIN::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kMessage, (void *)m_alertMessage.c_str());	
		UEventSendMessage(GetWidID(), EV_SETTEXT, kMessageDetails, (void *)m_messageDetails.c_str());	
	}
	else
	{
	}
}

UIMPLEMENT_DYNAMIC(CCVSAlert_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(CCVSAlert_MAIN, UWidget)
	ON_UCOMMAND(CCVSAlert_MAIN::kOK, CCVSAlert_MAIN::OnOK)
	ON_UCOMMAND(CCVSAlert_MAIN::kCancel, CCVSAlert_MAIN::OnCancel)
	ON_UCOMMAND(CCVSAlert_MAIN::kOtherBtn, CCVSAlert_MAIN::OnOtherButton)
UEND_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCVSAlert_MAIN message handlers

/// OnInitDialog virtual override, arrange buttons
bool CCVSAlert_MAIN::OnInitDialog() 
{
	UWidget::OnInitWidget(kUMainWidget, UCreate_Alert());
	
	// Extra initialization
	ASSERT(m_pcvsAlert != NULL); // No alert info class

	if( m_pcvsAlert )
	{
		SetIcon();
		SetTitle();
		SetMessages();
		SetButtonTitles();
		SetAccelerators();
		SetDefaultButton();
	}
	return false;
}

/*!
	Set the icon
*/
void CCVSAlert_MAIN::SetIcon()
{
	ASSERT(m_pcvsAlert != NULL); // Bad pointer

	void* icon = NULL;
	
	switch( m_pcvsAlert->GetIcon() )
	{
	case kCvsAlertStopIcon:
		icon = UCreate_pixmap(this, "stopAlert.xpm");
		break;
	case kCvsAlertNoteIcon:
		icon = UCreate_pixmap(this, "info.xpm");
		break;
	default:
		// Fall thru
	case kCvsAlertWarningIcon:
		icon = UCreate_pixmap(this, "warning.xpm");
		break;
	case kCvsAlertQuestionIcon:
		icon = UCreate_pixmap(this, "question.xpm");
		break;
	}

	if( icon != NULL )
		UEventSendMessage(GetWidID(), EV_SETICON, kIcon, icon);
}

/*!
	Set dialog title
*/
void CCVSAlert_MAIN::SetTitle()
{
	if( !m_pcvsAlert->GetAlertTitle().empty() )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kUMainWidget, (void*)(m_pcvsAlert->GetAlertTitle().c_str()));
	}
}

/*!
	Set the message and message details
*/
void CCVSAlert_MAIN::SetMessages()
{
	// Set messages
	m_alertMessage = m_pcvsAlert->GetMessage().c_str();
	
	if( !m_pcvsAlert->GetMessageDetails().empty() )
	{
		m_messageDetails += m_pcvsAlert->GetMessageDetails().c_str();
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_SHOW_WIDGET, UMAKEINT(kMessageDetails, 0), 0L);	
	}
}

/*!
	Set the buttons titles and layout
*/
void CCVSAlert_MAIN::SetButtonTitles()
{
	if( !m_pcvsAlert->GetButtonTitle(kCvsAlertOKButton).empty() )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kOK, (void *)(m_pcvsAlert->GetButtonTitle(kCvsAlertOKButton).c_str()));
	}
	
	if( !m_pcvsAlert->GetButtonTitle(kCvsAlertCancelButton).empty() )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kCancel, (void *)(m_pcvsAlert->GetButtonTitle(kCvsAlertCancelButton).c_str()));
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_SHOW_WIDGET, UMAKEINT(kCancel, 0), 0L);	
	}
	
	if( !m_pcvsAlert->GetButtonTitle(kCvsAlertOtherButton).empty() )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kOtherBtn, (void *)(m_pcvsAlert->GetButtonTitle(kCvsAlertOtherButton).c_str()));
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_SHOW_WIDGET, UMAKEINT(kOtherBtn, 0), 0L);	
	}
}

/*!
	Set the accelerators
*/
void CCVSAlert_MAIN::SetAccelerators()
{
}

void CCVSAlert_MAIN::SetDefID(unsigned int defID)
{
	UEventSendMessage(GetWidID(), EV_SETDEFAULT, defID, 0L);
}

/*!
	Set the default and initially focused button
*/
void CCVSAlert_MAIN::SetDefaultButton()
{
	unsigned int defID = 0;
	
	switch( m_defaultButton )
	{
	case kCvsAlertOKButton:
		defID = kOK;
		break;
	case kCvsAlertCancelButton:
		defID = kCancel;
		break;
	case kCvsAlertOtherButton:
		defID = kOtherBtn;
		break;
	default:
		break;
	}

	if( defID )
	{
		SetDefID(defID);
	}
}

/// OnOK virtual override, set the return value
void CCVSAlert_MAIN::OnOK() 
{
	EndDialog(kCvsAlertOKButton);
}

/// OnCancel virtual override, set the return value
void CCVSAlert_MAIN::OnCancel() 
{
	EndDialog(kCvsAlertCancelButton);
}

/// BN_CLICKED message handler, set the return value
void CCVSAlert_MAIN::OnOtherButton() 
{
	EndDialog(kCvsAlertOtherButton);
}
#endif /* qUnix */


//} CCVSAlert_MAIN
#if TARGET_OS_MAC
namespace {
  pascal Boolean _ModalFilter(DialogRef theDialog, EventRecord *theEvent, DialogItemIndex *itemHit)
  {
  	try {
  		UModalAlerts::ProcessModalEvent(*theEvent);		// Do normal PP event processing
  	}

  	catch (...) { }

    return StdFilterProc (theDialog, theEvent, itemHit);
  }
}
#endif
