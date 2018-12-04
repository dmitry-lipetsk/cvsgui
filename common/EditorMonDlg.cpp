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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2001
 */

/*
 * EditorMonDlg.cpp --- editor monitor dialog
 */

#include "stdafx.h"
#include "CvsPrefs.h"
#include "CvsCommands.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "EditorMonDlg.h"
#endif /* WIN32 */

#include "CvsAlert.h"
#include "LaunchHandlers.h"

#include <string>
#include <sstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorMonDlg dialog

#ifdef WIN32

CEditorMonDlg::CEditorMonDlg(CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CEditorMonDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditorMonDlg)
	m_tempFile = _T("");
	//}}AFX_DATA_INIT
}


void CEditorMonDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditorMonDlg)
	DDX_Text(pDX, IDC_TEMPFILE, m_tempFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditorMonDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CEditorMonDlg)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorMonDlg message handlers

/// WM_DESTROY message handler, kill the monitoring timer
void CEditorMonDlg::OnDestroy() 
{
	if( m_timer )
	{
		KillTimer(m_timer);
	}

	CHHelpDialog::OnDestroy();
}

/// OnInitDialog virtual override, start monitoring timer
BOOL CEditorMonDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	m_timer = SetTimer(2048, 200, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditorMonDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_TEMPFILE,	IDH_IDC_TEMPFILE);
}

/// BN_CLICKED message handler, display dialog help
void CEditorMonDlg::OnHelp() 
{
	ShowHelp();
}

/// WM_TIMER message handler, check whether the process was terminated
void CEditorMonDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == m_timer && INVALID_HANDLE_VALUE != m_hProcess )
	{
		const DWORD status = WaitForSingleObject(m_hProcess, 0);
		if( status == WAIT_OBJECT_0 )
		{
			EndDialog(IDOK);
		}
	}
	
	CHHelpDialog::OnTimer(nIDEvent);
}

#endif /* WIN32 */

/// Launch and monitor editor to get the log message
bool CompatGetEditorMsg(std::string& message)
{
	bool msgOK = false;
	
	if( gCvsPrefs.Viewer() == 0L )
	{
		CvsAlert(kCvsAlertStopIcon, 
			"No default editor specified.", "You need to define a default editor in the preferences", 
			BUTTONTITLE_OK, NULL).ShowAlert();

		return msgOK;
	}
	
#ifdef WIN32
	string tmpFile;
	if( MakeTmpFile(tmpFile, "LogMsg", "tmp", true) )
	{
		// Copy the message string into a file 
		if( FILE* stream = fopen(tmpFile.c_str(), "wt") )
		{
			string strMessage = NormalizeLogMsg(message);

			fwrite(strMessage.c_str(), sizeof(char), strMessage.size(), stream);
			fclose(stream);
		}
		
		// Launch editor
		PROCESS_INFORMATION sProcessInformation;
		if( LaunchDefaultEditor(tmpFile.c_str(), (void*)&sProcessInformation) )
		{
			// We don't need the thread's handle so close it now
			CloseHandle(sProcessInformation.hThread);
			
			// Track the editor until it closes
			string uppath, folder;
			SplitPath(tmpFile.c_str(), uppath, folder);

			CEditorMonDlg editorMonDlg;
			editorMonDlg.m_hProcess = sProcessInformation.hProcess;
			editorMonDlg.m_tempFile = folder.c_str();
			
			if( editorMonDlg.DoModal() == IDOK )
			{
				msgOK = true;
			}
			else
			{
				if( PostThreadMessage(sProcessInformation.dwThreadId, WM_QUIT, (WPARAM)EXIT_SUCCESS, 0) )
				{
					WaitForSingleObject(sProcessInformation.hProcess, 500);
				}
				
				int nStatus = WaitForSingleObject(sProcessInformation.hProcess, 0);
				if( nStatus != WAIT_OBJECT_0 )
				{
					TerminateProcess(sProcessInformation.hProcess, EXIT_SUCCESS);
				}
			}
			
			CloseHandle(sProcessInformation.hProcess);
		}
		
		// Copy the file into message string
		if( FILE* stream = fopen(tmpFile.c_str(), "r") )
		{
			char line[100];
			while( fgets(line, 100, stream) )
			{
				message += line;
			}

			fclose(stream);
		}
		
		// Cleanup
		::DeleteFile(tmpFile.c_str());
	}
	
#endif /* WIN32 */

// the code below might help to do stuff on Unix or Mac later on
#if 0	
	const char * argv[2] = { 0L, 0L };
	CStr program;
	if(strchr(gCvsPrefs.Viewer(), ' ') != 0L)
	{
		program << '\"';
		program << gCvsPrefs.Viewer();
		program << '\"';
	}
	else
		program = gCvsPrefs.Viewer();
	
	argv[0] = program;
	int process = _spawnvp(_P_NOWAIT, gCvsPrefs.Viewer(), argv);
#endif 
	

#ifdef qMacCvsPP
#endif /* qMacCvsPP */
#if qUnix
#endif // qUnix
	
	return msgOK;
}
