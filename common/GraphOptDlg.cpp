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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- September 2001
 */

/*
 * GraphOptDlg.cpp --- graph options dialog
 */

#include "stdafx.h"

#ifdef WIN32
#include "wincvs.h"
#include "MainFrm.h"
#include "GraphView.h"
#endif /* WIN32 */

#include "GraphOptDlg.h"
#include "CvsPrefs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphOptDlg dialog

#ifdef WIN32

CGraphOptDlg::CGraphOptDlg(CPersistentInt& selColor, CPersistentInt& shadowColor, 
						   CPersistentInt& headerColor, CPersistentInt& tagColor, CPersistentInt& branchColor, 
						   CPersistentInt& nodeColor, CPersistentInt& deadNodeColor, CPersistentInt& userStateNodeColor,
						   CPersistentInt& mergeColor, CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CGraphOptDlg::IDD, pParent),
	m_selColor(selColor), m_shadowColor(shadowColor), 
	m_headerColor(headerColor), m_tagColor(tagColor), m_branchColor(branchColor), 
	m_nodeColor(nodeColor), m_deadNodeColor(deadNodeColor), m_userStateNodeColor(userStateNodeColor),
	m_mergeColor(mergeColor)
{
	//{{AFX_DATA_INIT(CGraphOptDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_rgbShadow.SetColor((int)m_shadowColor);
	m_rgbSelection.SetColor((int)m_selColor);
	m_rgbState.SetColor((int)m_userStateNodeColor);
	m_rgbDead.SetColor((int)m_deadNodeColor);
	m_rgbNode.SetColor((int)m_nodeColor);
	m_rgbBranch.SetColor((int)m_branchColor);
	m_rgbTag.SetColor((int)m_tagColor);
	m_rgbHeader.SetColor((int)m_headerColor);
	m_rgbMerge.SetColor((int)m_mergeColor);

	m_origSelColor = m_selColor;
	m_origShadowColor = m_shadowColor;
	m_origHeaderColor = m_headerColor;
	m_origTagColor = m_tagColor;
	m_origBranchColor = m_branchColor;
	m_origNodeColor = m_deadNodeColor;
	m_origDeadNodeColor = m_deadNodeColor;
	m_origUserStateNodeColor = m_userStateNodeColor;
	m_origMergeColor = m_mergeColor;
}


void CGraphOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphOptDlg)
	DDX_Control(pDX, IDC_APPLY, m_apply);
	DDX_Control(pDX, IDC_RGB_SHADOW, m_rgbShadow);
	DDX_Control(pDX, IDC_RGB_SELECTION, m_rgbSelection);
	DDX_Control(pDX, IDC_RGB_STATE, m_rgbState);
	DDX_Control(pDX, IDC_RGB_DEAD, m_rgbDead);
	DDX_Control(pDX, IDC_RGB_NODE, m_rgbNode);
	DDX_Control(pDX, IDC_RGB_BRANCH, m_rgbBranch);
	DDX_Control(pDX, IDC_RGB_TAG, m_rgbTag);
	DDX_Control(pDX, IDC_RGB_HEADER, m_rgbHeader);
	DDX_Control(pDX, IDC_RGB_MERGE, m_rgbMerge);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphOptDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CGraphOptDlg)
	ON_BN_CLICKED(IDC_COLOR_HEADER, OnHeader)
	ON_BN_CLICKED(IDC_COLOR_TAG, OnTag)
	ON_BN_CLICKED(IDC_COLOR_BRANCH, OnBranch)
	ON_BN_CLICKED(IDC_COLOR_NODE, OnNode)
	ON_BN_CLICKED(IDC_COLOR_DEAD, OnDead)
	ON_BN_CLICKED(IDC_COLOR_STATE, OnState)
	ON_BN_CLICKED(IDC_COLOR_SELECTION, OnSelection)
	ON_BN_CLICKED(IDC_COLOR_SHADOW, OnShadow)
	ON_BN_CLICKED(IDC_RESET_COLORS, OnReset)
	ON_BN_CLICKED(IDC_COLOR_MERGE, OnMerge)
	ON_BN_CLICKED(IDHELP, OnHelp)
	ON_BN_CLICKED(IDC_APPLY, OnApplyNow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphOptDlg message handlers

void CGraphOptDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_COLOR_HEADER,		IDH_IDC_COLOR_HEADER);
	SetCtrlHelpID(IDC_COLOR_TAG,		IDH_IDC_COLOR_TAG);
	SetCtrlHelpID(IDC_COLOR_BRANCH,		IDH_IDC_COLOR_BRANCH);
	SetCtrlHelpID(IDC_COLOR_NODE,		IDH_IDC_COLOR_NODE);
	SetCtrlHelpID(IDC_COLOR_DEAD,		IDH_IDC_COLOR_DEAD);
	SetCtrlHelpID(IDC_COLOR_STATE,		IDH_IDC_COLOR_STATE);
	SetCtrlHelpID(IDC_COLOR_SELECTION,	IDH_IDC_COLOR_SELECTION);
	SetCtrlHelpID(IDC_COLOR_SHADOW,		IDH_IDC_COLOR_SHADOW);
	SetCtrlHelpID(IDC_COLOR_MERGE,		IDH_IDC_COLOR_MERGE);
	SetCtrlHelpID(IDC_RGB_HEADER,		IDH_IDC_RGB_HEADER);
	SetCtrlHelpID(IDC_RGB_TAG,			IDH_IDC_RGB_TAG);
	SetCtrlHelpID(IDC_RGB_BRANCH,		IDH_IDC_RGB_BRANCH);
	SetCtrlHelpID(IDC_RGB_NODE,			IDH_IDC_RGB_NODE);
	SetCtrlHelpID(IDC_RGB_DEAD,			IDH_IDC_RGB_DEAD);
	SetCtrlHelpID(IDC_RGB_STATE,		IDH_IDC_RGB_STATE);
	SetCtrlHelpID(IDC_RGB_SELECTION,	IDH_IDC_RGB_SELECTION);
	SetCtrlHelpID(IDC_RGB_SHADOW,		IDH_IDC_RGB_SHADOW);
	SetCtrlHelpID(IDC_RGB_MERGE,		IDH_IDC_RGB_MERGE);
	SetCtrlHelpID(IDC_RESET_COLORS,		IDH_IDC_RESET_COLORS);
	SetCtrlHelpID(IDC_APPLY,			IDH_IDC_APPLY);
}

/// BN_CLICKED message handler, display dialog help
void CGraphOptDlg::OnHelp() 
{
	ShowHelp();
}

/*!
	Retrieve all colors
*/
void CGraphOptDlg::GetAllColors()
{
	m_shadowColor = (int)m_rgbShadow.GetColor();
	m_selColor = (int)m_rgbSelection.GetColor();
	m_userStateNodeColor = (int)m_rgbState.GetColor();
	m_deadNodeColor = (int)m_rgbDead.GetColor();
	m_nodeColor = (int)m_rgbNode.GetColor();
	m_branchColor = (int)m_rgbBranch.GetColor();
	m_tagColor = (int)m_rgbTag.GetColor();
	m_headerColor = (int)m_rgbHeader.GetColor();
	m_mergeColor = (int)m_rgbMerge.GetColor();
}

/// OnOK virtual override, retrieve all colors into corresponding variables
void CGraphOptDlg::OnOK() 
{
	GetAllColors();

	CHHelpDialog::OnOK();
}

/// BN_CLICKED message handler, apply any color changes
void CGraphOptDlg::OnApplyNow() 
{
	GetAllColors();

	CWincvsApp* pApp = (CWincvsApp*)AfxGetApp();
	if( pApp )
	{
		pApp->UpdateAllDocsView();
	}
	
	m_apply.EnableWindow(FALSE);
}

/// OnCancel virtual override, restore original colors
void CGraphOptDlg::OnCancel() 
{
	m_selColor = m_origSelColor;
	m_shadowColor = m_origShadowColor;
	m_headerColor = m_origHeaderColor;
	m_tagColor = m_origTagColor;
	m_branchColor = m_origBranchColor;
	m_deadNodeColor = m_origNodeColor;
	m_deadNodeColor = m_origDeadNodeColor;
	m_userStateNodeColor = m_origUserStateNodeColor;
	m_mergeColor = m_origMergeColor;
	
	CHHelpDialog::OnCancel();
}

/*!
	Select the color
	\param colorStatic Preview static control
*/
void CGraphOptDlg::PromptForColor(CColorStatic& colorStatic)
{
	CColorDialog colorDialog(colorStatic.GetColor());
	if( colorDialog.DoModal() == IDOK )
	{
		if( colorDialog.GetColor() != colorStatic.GetColor() )
		{
			colorStatic.SetColor(colorDialog.GetColor());
			m_apply.EnableWindow();
		}
	}
}

/// BN_CLICKED message handler, prompt for header color
void CGraphOptDlg::OnHeader() 
{
	PromptForColor(m_rgbHeader);
}

/// BN_CLICKED message handler, prompt for tag color
void CGraphOptDlg::OnTag() 
{
	PromptForColor(m_rgbTag);
}

/// BN_CLICKED message handler, prompt for branch color
void CGraphOptDlg::OnBranch() 
{
	PromptForColor(m_rgbBranch);
}

/// BN_CLICKED message handler, prompt for node color
void CGraphOptDlg::OnNode() 
{
	PromptForColor(m_rgbNode);
}

/// BN_CLICKED message handler, prompt for dead color
void CGraphOptDlg::OnDead() 
{
	PromptForColor(m_rgbDead);
}

/// BN_CLICKED message handler, prompt for state color
void CGraphOptDlg::OnState() 
{
	PromptForColor(m_rgbState);
}

/// BN_CLICKED message handler, prompt for selection color
void CGraphOptDlg::OnSelection() 
{
	PromptForColor(m_rgbSelection);
}

/// BN_CLICKED message handler, prompt for shadow color
void CGraphOptDlg::OnShadow() 
{
	PromptForColor(m_rgbShadow);
}

/// BN_CLICKED message handler, prompt for merge color
void CGraphOptDlg::OnMerge() 
{
	PromptForColor(m_rgbMerge);
}


/// BN_CLICKED message handler, reset all preview colors
void CGraphOptDlg::OnReset() 
{
	m_rgbShadow.SetColor(RGB_DEF_SHADOW);
	m_rgbSelection.SetColor(RGB_DEF_SEL);
	m_rgbState.SetColor(RGB_DEF_USERSTATENODE);
	m_rgbDead.SetColor(RGB_DEF_DEADNODE);
	m_rgbNode.SetColor(RGB_DEF_NODE);
	m_rgbBranch.SetColor(RGB_DEF_BRANCH);
	m_rgbTag.SetColor(RGB_DEF_TAG);
	m_rgbHeader.SetColor(RGB_DEF_HEADER);
	m_rgbMerge.SetColor(RGB_DEF_MERGE);
	
	m_apply.EnableWindow();
}

#endif /* WIN32 */

/// Get the graph options
bool CompatGetGraphOptions(CPersistentInt& selColor, CPersistentInt& shadowColor, 
						   CPersistentInt& headerColor, CPersistentInt& tagColor, CPersistentInt& branchColor, 
						   CPersistentInt& nodeColor, CPersistentInt& deadNodeColor, CPersistentInt& userStateNodeColor,
						   CPersistentInt& mergeColor)
{
	bool userHitOK = false;

#ifdef WIN32
	CGraphOptDlg graphOptDlg(selColor, shadowColor, 
		headerColor, tagColor, branchColor, 
		nodeColor, deadNodeColor, userStateNodeColor, mergeColor);

	if( graphOptDlg.DoModal() == IDOK )
	{
		userHitOK = true;
	}
#endif /* WIN32 */
#ifdef qMacCvsPP
#endif /* qMacCvsPP */
#if qUnix
#endif // qUnix
	
	return userHitOK;
}
