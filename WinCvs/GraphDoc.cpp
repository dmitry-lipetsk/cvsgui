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

// GraphDoc.cpp : implementation of the CGraphDoc class
//

#include "stdafx.h"
#include <direct.h>
#include <sys/stat.h>
#include "Wincvs.h"
#include "GraphDoc.h"
#include "CvsLog.h"
#include "WinCvsView.h"
#include "GraphView.h"
#include "CvsEntries.h"
#include "CvsAlert.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*!
	Normalize folder/fname since the latter can be a path relative to folder
	\param folder Input folder
	\param fn Input file name
	\param dir Normalized folder
	\param name Normalized file name
*/
static void NormalizeFn(const char* folder, const char* fn, char* dir, char* name)
{
	char buffer[_MAX_PATH];
	char dirBuffer[_MAX_PATH];
	char nameBuffer[_MAX_PATH];
	char drive[_MAX_PATH];
	char ext[_MAX_EXT];

	_makepath(buffer, NULL, folder, fn, NULL);
	_splitpath(buffer, drive, dirBuffer, nameBuffer, ext);
	_makepath(dir, drive, dirBuffer, NULL, NULL);
	_makepath(name, NULL, NULL, nameBuffer, ext);
}

/////////////////////////////////////////////////////////////////////////////
// CGraphDoc

IMPLEMENT_DYNCREATE(CGraphDoc, CDocument)

BEGIN_MESSAGE_MAP(CGraphDoc, CDocument)
	//{{AFX_MSG_MAP(CGraphDoc)
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowCloseall)
	//}}AFX_MSG_MAP
//	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
//	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CGraphDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CGraphDoc)
	DISP_FUNCTION(CGraphDoc, "NumNodes", NumNodes, VT_I4, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ITest to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {09F456B5-0018-11D2-BCAA-000000000000}
static const IID IID_ITest =
{ 0x9f456b5, 0x18, 0x11d2, { 0xbc, 0xaa, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } };

BEGIN_INTERFACE_MAP(CGraphDoc, CDocument)
	INTERFACE_PART(CGraphDoc, IID_ITest, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDoc construction/destruction

CGraphDoc::CGraphDoc()
{
	EnableAutomation();

	AfxOleLockApp();

	m_fNode = 0L;
	m_fData = 0L;

	m_reloading = false;
}

CGraphDoc::~CGraphDoc()
{
	AfxOleUnlockApp();

	Clear();
}

/*!
	Clear the data
*/
void CGraphDoc::Clear(void)
{
	if( m_fNode != 0L )
	{
		delete m_fNode;
		m_fNode = 0L;
	}

	if( m_fData != 0L )
	{
		m_fData->UnRef();
		m_fData = 0L;
	}

	m_fDir.Empty();
	m_fName.Empty();
}

/// OnNewDocument virtual override, clear the data
BOOL CGraphDoc::OnNewDocument()
{
	Clear();

	if( !CDocument::OnNewDocument() )
		return FALSE;

	return TRUE;
}

/*!
	Set the node data
	\param node Log node
	\param folder Folder
*/
void CGraphDoc::SetNode(CLogNode* node, const char* folder)
{
	Clear();

	// Normalize folder/fname
	const char* fn = (**(CLogNodeHeader*)node).WorkingFile().c_str();
	char dir[_MAX_PATH] = "";
	char name[_MAX_PATH] = "";
	NormalizeFn(folder, fn, dir, name);

	m_fNode = node;
	m_fDir = dir;
	m_fName = name;

	// Get the CVS/Entries infos for this file
	CSortList<ENTNODE> entries(200, ENTNODE::Compare);
	if( !Entries_Open(entries, dir) )
	{
		cvs_err("Warning : Error while accessing the CVS folder in '%s'\n", dir);
	}
	else
	{
		if( node->GetType() != kNodeHeader )
		{
			cvs_err("Warning : Top node is not a RCS file description !\n");
		}
		else
		{
			string fullPath;
			fullPath += m_fDir;
			fullPath += name;

			struct stat sb;
			if( stat(fullPath.c_str(), &sb) == -1 )
			{
				cvs_err("Warning : File '%s' not found\n", name);
			}
			else
			{
				m_fData = Entries_SetVisited(dir, entries, name, sb, false);
				m_fData->Ref();
			}
		}
	}

	// Create the graph and redraw
	CGraphView* view = GetTreeView();
	if( view != 0L )
	{
		view->CalcImageSize();
		view->Invalidate();
		SetTitle(m_fName);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGraphDoc serialization

void CGraphDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/*!
	Get the graph (tree) view
	\return The graph view
*/
CGraphView* CGraphDoc::GetTreeView(void)
{
	CView* view;

	POSITION posv = GetFirstViewPosition();
	while( posv != NULL )
	{
		view = GetNextView(posv);
		if( view->IsKindOf(RUNTIME_CLASS(CGraphView)) )
			return (CGraphView*)view;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphDoc diagnostics

#ifdef _DEBUG
void CGraphDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGraphDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphDoc commands

/*!
	Get the number of nodes
	\return The number of nodes
*/
long CGraphDoc::NumNodes() 
{
	return 0;
}

/*!
	Set the reloading state
	\param reloading true to signal the reloading in progress, false otherwise
*/
void CGraphDoc::SetReloading(const bool reloading)
{
	m_reloading = reloading;
}

/// OnCloseDocument virtual override, prevent closing the document while refreshing is in progress
void CGraphDoc::OnCloseDocument() 
{
	if( m_reloading )
	{
		CvsAlert(kCvsAlertStopIcon, 
			_i18n("CVS is currently running."), _i18n("Stop CVS first before closing the graph view."), 
			BUTTONTITLE_OK, NULL).ShowAlert();
		
		return;
	}
	
	CDocument::OnCloseDocument();
}

/// ID_WINDOW_CLOSEALL message handler, close all graph documents
void CGraphDoc::OnWindowCloseall() 
{
	if( CWinApp* app = AfxGetApp() )
	{
		for(POSITION pos = app->GetFirstDocTemplatePosition(); pos; )
		{
			if( CDocTemplate* pDocTemplate = app->GetNextDocTemplate(pos) )
			{
				for(POSITION posDoc = pDocTemplate->GetFirstDocPosition(); posDoc; )
				{
					if( CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc) )
					{
						if( pDoc->IsKindOf(RUNTIME_CLASS(CGraphDoc)) )
						{
							pDoc->OnCloseDocument();
						}
					}
				}
			}
		}
	}
}
