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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- June 2004
 */

// SaveSettingsDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif

#include "SaveSettingsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CSaveSettingsDlg dialog


CSaveSettingsDlg::CSaveSettingsDlg(const char* path, const char* oldname, CWnd* pParent /*=NULL*/)
	: m_copyFromPathCombo(USmartCombo::AutoDropWidth), CHHelpDialog(CSaveSettingsDlg::IDD, pParent)
{
	string strOldName = oldname;

	string defname;
	if( strOldName.empty() )
	{
		string uppath;
		SplitPath(path, uppath, defname);
	}
	else
		defname = oldname;

	//{{AFX_DATA_INIT(CSaveSettingsDlg)
	m_settingName = defname.c_str();
	m_copyFrom = _T("");
	m_path = path;
	m_copy = FALSE;
	m_copyPath = _T("");
	//}}AFX_DATA_INIT
}


void CSaveSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveSettingsDlg)
	DDX_Control(pDX, IDC_COPY_SETTINGS, m_copyCheck);
	DDX_Control(pDX, IDC_COPY_SETTINGS_NAME, m_copyFromPathCombo);
	DDX_Text(pDX, IDC_SETTINGS_NAME, m_settingName);
	DDX_CBString(pDX, IDC_COPY_SETTINGS_NAME, m_copyFrom);
	DDX_Text(pDX, IDC_PATH, m_path);
	DDX_Check(pDX, IDC_COPY_SETTINGS, m_copy);
	DDX_Text(pDX, IDC_COPY_PATH, m_copyPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveSettingsDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CSaveSettingsDlg)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_COPY_SETTINGS, OnCopySettings)
	ON_CBN_SELCHANGE(IDC_COPY_SETTINGS_NAME, OnSelchangeCopySettingsName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveSettingsDlg message handlers

/// OnInitDialog virtual override, initialize controls and set help IDs
BOOL CSaveSettingsDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	// Fill the "copy from" combo
	if( GetAllPersistentSettings(m_settingsList) )
	{
		map<string, string>::const_iterator i;
		for(i = m_settingsList.begin(); i != m_settingsList.end(); ++i)
		{
			m_copyFromPathCombo.AddString((*i).first.c_str());
		}
	}

	OnCopySettings();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSaveSettingsDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_SETTINGS_NAME,		IDH_IDC_SETTINGS_NAME);
	SetCtrlHelpID(IDC_PATH,					IDH_IDC_PATH);
	SetCtrlHelpID(IDC_COPY_SETTINGS,		IDH_IDC_COPY_SETTINGS);
	SetCtrlHelpID(IDC_COPY_SETTINGS_NAME,	IDH_IDC_COPY_SETTINGS_NAME);
	SetCtrlHelpID(IDC_COPY_PATH,			IDH_IDC_PATH);
}

/// BN_CLICKED message handler, display dialog help
void CSaveSettingsDlg::OnHelp() 
{
	ShowHelp();
}

/// BN_CLICKED message handler, enable copy settings
void CSaveSettingsDlg::OnCopySettings() 
{
	m_copyFromPathCombo.EnableWindow(m_copyCheck.GetCheck());

	if( !m_copyCheck.GetCheck() )
	{
		m_copyFromPathCombo.SetCurSel(-1);
	}
}

/// CBN_SELCHANGE message handler, update copy path
void CSaveSettingsDlg::OnSelchangeCopySettingsName() 
{
	UpdateData(TRUE);

	map<string, string>::const_iterator it = m_settingsList.find(string(m_copyFrom));
	if( it != m_settingsList.end() )
	{
		m_copyPath = it->second.c_str();
		UpdateData(FALSE);
	}
}

#endif
