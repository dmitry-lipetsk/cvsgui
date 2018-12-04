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

// wincvsDoc.cpp : implementation of the CWincvsDoc class
//

#include "stdafx.h"
#include "wincvs.h"
#include <afxrich.h>
#include "wincvsDoc.h"
#include "CvsCommands.h"
#include "GetPrefs.h"
#include "Authen.h"
#include "CvsPrefs.h"
#include "AppConsole.h"
#include "PromptFiles.h"
#include "WinCvsBrowser.h"
#include "TclGlue.h"
#include "PythonGlue.h"
#include "cvsgui_process.h"
#include "Appglue.h"
#include "BrowseViewHandlers.h"
#include "CvsAlert.h"
#include "wincvsView.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWincvsDoc

IMPLEMENT_DYNCREATE(CWincvsDoc, CDocument)

BEGIN_MESSAGE_MAP(CWincvsDoc, CDocument)
	//{{AFX_MSG_MAP(CWincvsDoc)
	ON_COMMAND(ID_APP_PREFERENCES, OnAppPreferences)
	ON_COMMAND(ID_APP_RTAGCREATE, OnAppRtagNew)
	ON_COMMAND(ID_APP_RTAGDELETE, OnAppRtagDelete)
	ON_COMMAND(ID_APP_RTAGBRANCH, OnAppRtagBranch)
	ON_COMMAND(ID_APP_CMDLINE, OnAppCmdline)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGCREATE, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_PREFERENCES, OnUpdatePrefs)
	ON_COMMAND(ID_BROWSER_OPEN, OnBrowserOpen)
	ON_COMMAND(ID_APP_LOGIN, OnAppLogin)
	ON_COMMAND(ID_APP_LOGOUT, OnAppLogout)
	ON_COMMAND(ID_VIEW_INIT, OnViewInit)
	ON_COMMAND(ID_SAVESETTINGS, OnSavesettings)
	ON_UPDATE_COMMAND_UI(ID_SAVESETTINGS, OnUpdateSavesettings)
	ON_COMMAND(ID_VIEW_BROWSELOCATION_REMOVEUNWANTED, OnViewBrowselocationRemoveunwanted)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INIT, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGDELETE, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_RTAGBRANCH, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_CMDLINE, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_LOGIN, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_BROWSER_OPEN, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_APP_LOGOUT, OnUpdateCvsCmd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BROWSELOCATION_REMOVEUNWANTED, OnUpdateCvsCmd)
	ON_COMMAND(ID_USE_PYSHELL, OnUsePyshell)
	ON_UPDATE_COMMAND_UI(ID_USE_PYSHELL, OnUpdateUsePyshell)
	ON_COMMAND(ID_USE_TCLSHELL, OnUseTclshell)
	ON_UPDATE_COMMAND_UI(ID_USE_TCLSHELL, OnUpdateUseTclshell)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	//ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, CDocument::OnUpdatePasteMenu)
//	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
//	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CWincvsDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CWincvsDoc)
	DISP_PROPERTY_NOTIFY(CWincvsDoc, "ViewPath", m_viewPath, OnViewPathChanged, VT_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IWincvs to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {D2D77DC4-8299-11D1-8949-444553540000}
static const IID IID_IWincvs =
{ 0xd2d77dc4, 0x8299, 0x11d1, { 0x89, 0x49, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };

BEGIN_INTERFACE_MAP(CWincvsDoc, CDocument)
	INTERFACE_PART(CWincvsDoc, IID_IWincvs, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWincvsDoc construction/destruction

CWincvsDoc::CWincvsDoc()
{
	// Use OLE compound files
	//EnableCompoundFile();

	// TODO: add one-time construction code here

	//EnableAutomation();

	//AfxOleLockApp();
}

CWincvsDoc::~CWincvsDoc()
{
	//AfxOleUnlockApp();
}

/// OnNewDocument virtual override
BOOL CWincvsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWincvsDoc serialization

class _afxRichEditCookie
{
public:
	CArchive& m_ar;
	DWORD m_dwError;
	_afxRichEditCookie(CArchive& ar) : m_ar(ar) {m_dwError=0;}
};

// return 0 for no error, otherwise return error code
extern "C" DWORD CALLBACK EditStreamCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	_afxRichEditCookie* pCookie = (_afxRichEditCookie*)dwCookie;
	CArchive& ar = pCookie->m_ar;
	ar.Flush();
	DWORD dw = 0;
	*pcb = cb;
	TRY
	{
		if (ar.IsStoring())
			ar.GetFile()->Write(pbBuff, cb);
		else
			*pcb = ar.GetFile()->Read(pbBuff, cb);
	}
	CATCH(CFileException, e)
	{
		*pcb = 0;
		pCookie->m_dwError = (DWORD)e->m_cause;
		dw = 1;
	}
	AND_CATCH_ALL(e)
	{
		*pcb = 0;
		pCookie->m_dwError = (DWORD)CFileException::generic;
		dw = 1;
	}
	END_CATCH_ALL
	return dw;
}

void CWincvsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CWinApp* app = AfxGetApp();
		CWincvsView *view = ((CWincvsApp *)app)->GetConsoleView();
		if( view && view->IsKindOf(RUNTIME_CLASS(CRichEditView)) )
		{
			int nFormat = SF_TEXT;
			EDITSTREAM es = {0, 0, (EDITSTREAMCALLBACK)EditStreamCallBack};
			_afxRichEditCookie cookie(ar);
			es.dwCookie = (DWORD)&cookie;
			((CRichEditView*)view)->GetRichEditCtrl().StreamOut(nFormat, es);
		}

		/*
		// This doesn't work because our view is actually a frame
		POSITION pos = GetFirstViewPosition();
		CView* view;
		while((view = GetNextView(pos)) != NULL)
		{
			if(view->IsKindOf(RUNTIME_CLASS(CRichEditView)))
			{
				int nFormat = SF_TEXT;
				EDITSTREAM es = {0, 0, (EDITSTREAMCALLBACK)EditStreamCallBack};
				_afxRichEditCookie cookie(ar);
				es.dwCookie = (DWORD)&cookie;
				((CRichEditView *)view)->GetRichEditCtrl().StreamOut(nFormat, es);
				break;
			}
		}
		*/
	}
	else
	{
		// We don't load right now
	}

	// Calling the base class CDocument enables serialization
	//  of the container document's COleClientItem objects.
	// (but we don't have COleClientItem)
	CDocument::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CWincvsDoc diagnostics

#ifdef _DEBUG
void CWincvsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWincvsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWincvsDoc commands

/// WM_COMMAND message handler, show preferences dialog
void CWincvsDoc::OnAppPreferences()
{
	CompatGetPrefs();
}

/// WM_COMMAND message handler, show command line dialog
void CWincvsDoc::OnAppCmdline() 
{
	KoCommandLineHandler handler;
	CvsCmdCommandLine(handler);
}

/// WM_COMMAND message handler, show login dialog
void CWincvsDoc::OnAppLogin() 
{
	CvsCmdLogin();
}

/// On update command
void CWincvsDoc::OnUpdateCvsCmd(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	pCmdUI->Enable(app->IsCvsRunning() ? FALSE : TRUE);
}

/// On update preferences
void CWincvsDoc::OnUpdatePrefs(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	pCmdUI->Enable(app->IsCvsRunning() ? FALSE : TRUE);
}

/// WM_COMMAND message handler, set browse location
void CWincvsDoc::OnBrowserOpen() 
{
	string newPath;
	if( BrowserGetDirectory("Select a folder to open with the browser", newPath) )
	{
		if( CWincvsApp* app = (CWincvsApp*)AfxGetApp() )
		{
			CheckLocationDriveType(newPath.c_str());

			if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
			{
				pBrowser->ResetBrowser(newPath.c_str(), true);
			}
		}
	}
}

/// Propery change handler
void CWincvsDoc::OnViewPathChanged() 
{
	// TODO: Add notification handler code

}

/// WM_COMMAND message handler, show create rtag dialog
void CWincvsDoc::OnAppRtagNew() 
{
	KoRtagCreateHandler handler;
	CvsCmdRtag(handler, kTagCreate);
}

/// WM_COMMAND message handler, show delete rtag dialog
void CWincvsDoc::OnAppRtagDelete()
{
	KoRtagDeleteHandler handler;
	CvsCmdRtag(handler, kTagDelete);
}

/// WM_COMMAND message handler, show create branch rtag dialog
void CWincvsDoc::OnAppRtagBranch() 
{
	KoRtagBranchHandler handler;
	CvsCmdRtag(handler, kTagBranch);
}

/// WM_COMMAND message handler, logout
void CWincvsDoc::OnAppLogout() 
{
	CvsCmdLogout();
}

/// WM_COMMAND message handler, show create repository dialog
void CWincvsDoc::OnViewInit() 
{
	CvsCmdInit();
}

/// WM_COMMAND message handler, show save settings dialog
void CWincvsDoc::OnSavesettings() 
{
	if( CWincvsApp* app = (CWincvsApp *)AfxGetApp() )
	{
		if( CWinCvsBrowser* pBrowser = app->GetBrowserView() )
		{
			AskCreatePersistentSettings(pBrowser->GetRoot().c_str());
		}
	}
}

/// Update save settings dialog
void CWincvsDoc::OnUpdateSavesettings(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	pCmdUI->Enable(!app->IsCvsRunning());
}

/// WM_COMMAND message handler, show information about removing unwanted browse locations
void CWincvsDoc::OnViewBrowselocationRemoveunwanted() 
{
	CvsAlert cvsAlert(kCvsAlertNoteIcon, 
		"You can remove combo box items using Del key.", "To remove the unwanted location history items click Del key when selecting from the combo box list.");

	cvsAlert.ShowAlert();
}

/// WM_COMMAND message handler, set the shell to Python
void CWincvsDoc::OnUsePyshell() 
{
	gCvsPrefs.SetUseShell(SHELL_PYTHON);
	cvs_out("Switching the shell to Python...\n");

	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app )
	{
		app->RestartScriptEngine();
	}
}

/// Update set shell to Python
void CWincvsDoc::OnUpdateUsePyshell(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();

	pCmdUI->Enable(app && app->HasPython());
	pCmdUI->SetRadio(gCvsPrefs.GetUseShell() == SHELL_PYTHON);
}

/// WM_COMMAND message handler, set the shell to TCL
void CWincvsDoc::OnUseTclshell() 
{
	gCvsPrefs.SetUseShell(SHELL_TCL);
	cvs_out("Switching the shell to TCL...\n");

	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app )
	{
		app->RestartScriptEngine();
	}
}

/// Update set shell to TCL
void CWincvsDoc::OnUpdateUseTclshell(CCmdUI* pCmdUI) 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();

	pCmdUI->Enable(app && app->HasTCL());
	pCmdUI->SetRadio(gCvsPrefs.GetUseShell() == SHELL_TCL);
}

//------------------------------------------------------------------------
BOOL CWincvsDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
 if(lpszPathName==NULL && m_strPathName.IsEmpty())
 {
  //m_strTitle will be used as template for new file name

  m_strTitle.LoadString(AFX_IDS_UNTITLED);
 }//if

 return CDocument::DoSave(lpszPathName,bReplace);
}//DoSave

//------------------------------------------------------------------------
/// SetTitle virtual override, set the path name to enable Save command working properly
void CWincvsDoc::SetTitle(LPCTSTR lpszTitle) 
{
	//[2012-11-08]
    // sf_animal: WTF??? Problem [bug check] with VS2010+Win7
	// see CWincvsDoc::DoSave
	//	- m_strPathName should contain the name of real file!
	//
	// #OLD_CODE - remove this method.
	//
	//m_strPathName.LoadString(AFX_IDS_UNTITLED);
	
	CDocument::SetTitle(lpszTitle);
}
