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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- October 2003
 */

/*
 * TemplateDlg.cpp : CVS Template entry
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#endif /* WIN32 */

#include "TemplateDlg.h"
#include "MultiFiles.h"
#include "CvsAlert.h"
#include <string>

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog


CTemplateDlg::CTemplateDlg(const MultiFiles* mf, CWnd* pParent /*=NULL*/)
	: m_mf(mf), CHHelpDialog(CTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateDlg)
	//}}AFX_DATA_INIT
}


void CTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateDlg)
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDC_TEMPLATE_PREVIEW, m_previewEdit);
	DDX_Control(pDX, IDC_TEMPLATE_LIST, m_templateList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CTemplateDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TEMPLATE_LIST, OnDblclkTemplateList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TEMPLATE_LIST, OnItemchangedTemplateList)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg message handlers

/// OnInitDialog virtual override
BOOL CTemplateDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	// Setup template list control
	{
		SetListCtrlExtendedStyle(&m_templateList, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

	
		CRect rect;
		m_templateList.GetClientRect(rect);
		
		const int listCtrlWidth = rect.Width();
		const int listCtrlWidthNoScroll = listCtrlWidth - GetSystemMetrics(SM_CXVSCROLL);
		
		// Insert columns
		m_templateList.InsertColumn(0, "Folder", LVCFMT_LEFT, 250);
		m_templateList.InsertColumn(1, "Full Path", LVCFMT_LEFT, listCtrlWidthNoScroll - m_templateList.GetColumnWidth(0));
	}

	if( m_mf )
	{
		for(int nIndex = 0; nIndex < m_mf->NumDirs(); nIndex++)
		{
			string dir;
			if( m_mf->getdir(nIndex, dir) )
			{
				string path = dir;
				NormalizeFolderPath(path);
				path += "CVS";
				path += kPathDelimiter;
				path += "Template";

				FILE* templateFile = fopen(path.c_str(), "r");
				if( templateFile )
				{
					fclose(templateFile);

					const int item = m_templateList.InsertItem(m_templateList.GetItemCount(), dir.c_str());
					if( item > -1 )
					{
						m_templateList.SetItemText(item, 1, path.c_str());
					}
				}
			}
		}
	}
	
	// Select the first template on the list
	if( m_templateList.GetItemCount() )
	{
		m_templateList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		
		// If there is only one consider it as a chosen one
		if( m_templateList.GetItemCount() == 1 )
		{
			OnOK();
		}
	}
	else
	{
		CvsAlert(kCvsAlertStopIcon, 
			"Template file not found.", "You need to setup Template using rcsinfo file. Please refer to CVS documentation for more details.", 
			BUTTONTITLE_OK, NULL).ShowAlert();
		
		OnCancel();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplateDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_TEMPLATE_LIST,	IDH_IDC_TEMPLATE_LIST);
	SetCtrlHelpID(IDC_TEMPLATE_PREVIEW,	IDH_IDC_TEMPLATE_PREVIEW);
}

/// OnOK virtual override, read the selected template file
void CTemplateDlg::OnOK() 
{
	if( m_templateList.GetSelectedCount() > 0 )
	{
		if( ReadSelectedTemplate(m_templateText) )
		{
			CHHelpDialog::OnOK();
		}
	}
	else
	{
		CvsAlert(kCvsAlertStopIcon, 
			"No selection.", "Select the Template file from the list.", 
			BUTTONTITLE_OK, NULL).ShowAlert();
	}
}

/*!
	Read the selected template file
	\param templateText Return read template text
	\return true on success, false otherwise
*/
bool CTemplateDlg::ReadSelectedTemplate(std::string& templateText)
{
	bool res = false;

	// Get the currently selected item
	POSITION pos = m_templateList.GetFirstSelectedItemPosition();
	if( pos )
	{
		const int item = m_templateList.GetNextSelectedItem(pos);
		const CString templateFile = m_templateList.GetItemText(item, 1);
		
		// Copy the file into message string
		FILE* stream = fopen(templateFile, "r");
		if( stream )
		{
			char line[100];
			while( fgets(line, 100, stream) )
			{
				templateText += line;
			}
			
			fclose(stream);
			
			res = true;
		}
		else
		{
			CvsAlert(kCvsAlertStopIcon, 
				"Can't open Template file.", "File " + templateFile + " may not exist or it may be in use by other application.", 
				BUTTONTITLE_OK, NULL).ShowAlert();
		}
	}

	return res;
}

/*!
	Get the selected template text
	\return The selected template text
*/
const char* CTemplateDlg::GetTemplateText() const
{
	return m_templateText.c_str();
}

/// LVN_ITEMCHANGED notification handler, update preview and enable OK button
void CTemplateDlg::OnItemchangedTemplateList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 && 
		pNMListView->uChanged & LVIF_STATE && pNMListView->uNewState & (LVIS_SELECTED | LVIS_FOCUSED) )
	{
		string templateText;
		if( ReadSelectedTemplate(templateText) )
		{
			CString itemText(templateText.c_str());
			itemText.Replace("\n", "\r\n");

			m_previewEdit.SetWindowText(itemText);
		}
	}

	if( !m_templateList.GetSelectedCount() )
	{
		m_previewEdit.SetWindowText("");
	}

	m_okButton.EnableWindow(m_templateList.GetSelectedCount());

	*pResult = 0;
}

/// NM_DBLCLK message handler, confirm selection
void CTemplateDlg::OnDblclkTemplateList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( m_templateList.GetSelectedCount() )
	{
		OnOK();
	}
	
	*pResult = 0;
}

/// BN_CLICKED message handler, display dialog help
void CTemplateDlg::OnHelp() 
{
	ShowHelp();
}

#endif /* WIN32 */

/*!
	Get the CVS template file contents
	\param mf Browser selection
	\param templateText 
*/
bool CompatGetCvsTemplate(const MultiFiles* mf, std::string& templateText)
{
	bool userHitOK = false;
	
#ifdef WIN32
	CTemplateDlg templateDlg(mf);

	if( templateDlg.DoModal() == IDOK )
	{
		templateText = templateDlg.GetTemplateText();

		userHitOK = true;
	}
#endif /* WIN32 */

#ifdef qMacCvsPP
#endif /* qMacCvsPP */

#if qUnix
#endif // qUnix

	return userHitOK;
}
