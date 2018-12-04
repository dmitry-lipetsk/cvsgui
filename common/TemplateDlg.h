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
 * TemplateDlg.h : CVS Template entry
 */

#if !defined(AFX_TEMPLATEDLG_H__FB71138C_D374_42EE_88E8_59890D6DEDE2__INCLUDED_)
#define AFX_TEMPLATEDLG_H__FB71138C_D374_42EE_88E8_59890D6DEDE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

class MultiFiles;

bool CompatGetCvsTemplate(const MultiFiles* mf, std::string& templateText);

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog

class CTemplateDlg : public CHHelpDialog
{
// Construction
public:
	CTemplateDlg(const MultiFiles* mf, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateDlg)
	enum { IDD = IDD_CVSTEMPLATE };
	CButton	m_okButton;
	CEdit	m_previewEdit;
	CListCtrl	m_templateList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkTemplateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTemplateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;	/*!< Browser selection */
	std::string m_templateText;	/*!< Selected template file text */

	// Methods
	virtual void InitHelpID();

	bool ReadSelectedTemplate(std::string& templateText);

public:
	// Interface
	const char* GetTemplateText() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#endif // !defined(AFX_TEMPLATEDLG_H__FB71138C_D374_42EE_88E8_59890D6DEDE2__INCLUDED_)
