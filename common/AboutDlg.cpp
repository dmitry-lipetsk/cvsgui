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
  * Author : Karl-Heinz Brünen <gcvs@bruenen-net.de> --- March 2002
 */

// AboutDlg.cpp : About Dialog!

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* !WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#include "uwidget.h"
#include "AboutDlg.h"
#include "UCvsApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

#ifdef WIN32
CAboutDlg::CAboutDlg() : CHHelpDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_vers = _T("WinCvs ");
	//}}AFX_DATA_INIT
	
	string vers;
	if( ::GetAppVersion(vers) )
	{
		m_vers = vers.c_str();
	}
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_CVSGUISFURL, m_CvsGuiSfUrl);
	DDX_Control(pDX, IDC_WINCVSURL, m_WinCvsUrl);
	DDX_Text(pDX, IDC_WINCVSVER, m_vers);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CHHelpDialog)
//{{AFX_MSG_MAP(CAboutDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/// OnInitDialog virtual override, initialize controls
BOOL CAboutDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	m_WinCvsUrl.SetLink(_T("http://www.wincvs.org"));
	m_CvsGuiSfUrl.SetLink(_T("http://cvsgui.sourceforge.net/"));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_WINCVSVER,	IDH_IDC_WINCVSVER);
	SetCtrlHelpID(IDC_WINCVSURL,	IDH_IDC_WINCVSURL);
	SetCtrlHelpID(IDC_CVSGUISFURL,	IDH_IDC_CVSGUISFURL);
}

#endif /* WIN32 */

#ifdef qUnix
UIMPLEMENT_DYNAMIC(UAboutDlg, UWidget)
UBEGIN_MESSAGE_MAP(UAboutDlg, UWidget)
ON_UCOMMAND(kOK, UAboutDlg::OnOk)
//	ON_UCOMMAND(kAppURL, UAboutDlg::OnURL)
UEND_MESSAGE_MAP()

int UAboutDlg::OnOk(void)
{
	EndModal(true);
	return 0;
}

int UAboutDlg::OnURL(void)
{
	return 0;
}

void UAboutDlg::DoDataExchange(bool fill)
{
	string  appURL = "http://cvsgui.sourceforge.net";
	string  appText;
		
	appText  = UCvsApp::gApp->GetAppName();
	appText += " ";
	appText += UCvsApp::gApp->GetAppVersion();
	
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kAppName, (void *)(const char *)appText.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kAppURL, (void *)(const char *)appURL.c_str());
	}
	else
	{
	}
}
#endif // qUnix


/// Show about dialog
void CompatAboutDialog(void)
{
#ifdef WIN32
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
#endif /* WIN32 */

#ifdef qUnix
	void *wid = UCreate_AboutDlg();
	const char*  appText = UCvsApp::gApp->GetAppName();
		
	UAboutDlg *dlg = new UAboutDlg ();
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	UEventSendMessage(dlg->GetWidID(), EV_SETTEXT, kUMainWidget, const_cast<char*>(appText));	
	
	bool res = false;
	
	res = dlg->DoModal();
#endif // qUnix

}
