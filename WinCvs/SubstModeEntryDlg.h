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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- August 2003
 */

/*
 * SubstModeEntryDlg.h : CVSROOT entry complex
 */

#if !defined(AFX_SUBSTMODEENTRYDLG_H__C1669CF1_D25C_4439_905B_F652482CE575__INCLUDED_)
#define AFX_SUBSTMODEENTRYDLG_H__C1669CF1_D25C_4439_905B_F652482CE575__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubstModeEntryDlg.h : header file
//

#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

#define CHR_SUBSTMODE_TEXT				't'
#define CHR_SUBSTMODE_BINARY			'b'
#define CHR_SUBSTMODE_BINARYDELTA		'B'
#define CHR_SUBSTMODE_UNICODE			'u'
#define CHR_SUBSTMODE_EXTENDEDENCODING	'{'
#define CHR_SUBSTMODE_SUBSTITUTEKEYWORD	'k'
#define CHR_SUBSTMODE_SUBSTITUTEVALUE	'v'
#define CHR_SUBSTMODE_NOSUBSTITUTION	'o'
#define CHR_SUBSTMODE_LOCKERSNAME		'l'
#define CHR_SUBSTMODE_FORCERESERVEDEDIT	'c'
#define CHR_SUBSTMODE_UNIXLINEENDINGS	'L'
#define CHR_SUBSTMODE_COMPRESSDELTAS	'z'

#define CHR_SUBSTMODE_EXTENDEDENCODING_CLOSE	'}'

/// Class to operate on substitution modes
class CKOpt
{
public:
	// Construction
	CKOpt(const char* kOpt);

private:
	// Data members
	std::string m_kOpt;			/*!< Keyword expansion option */
	static char m_encodings[];	/*!< Encoding array */

	// Methods
	typedef std::string::size_type size_type;
	
	size_type FindFlagsPos() const;

public:
	// Interface
	const std::string& GetOptString() const;

	bool HasOption(const char opt, const bool isEncoding = false) const;
	char GetEncoding() const;

	bool RemoveOption(const char opt, const bool isEncoding = false);
	bool SetOption(const char opt, const bool isEncoding = false);

	std::string FormatDescription() const;
};

/////////////////////////////////////////////////////////////////////////////
// CSubstModeEntryDlg dialog

class CSubstModeEntryDlg : public CHHelpPropertyPage
{
	DECLARE_DYNCREATE(CSubstModeEntryDlg)

// Construction
public:
	CSubstModeEntryDlg();
	~CSubstModeEntryDlg();

// Dialog Data
	//{{AFX_DATA(CSubstModeEntryDlg)
	enum { IDD = IDD_SUBSTMODE_ENTRY };
	CEdit	m_keywordDescEdit;
	CButton	m_browseKeywordButton;
	CSmartComboBox	m_keywordCombo;
	CString	m_keyword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSubstModeEntryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSubstModeEntryDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeKeywordOpt();
	afx_msg void OnBrowseKeyword();
	afx_msg void OnSubstmodeText();
	afx_msg void OnSubstmodeBinary();
	afx_msg void OnSubstmodeBinarydelta();
	afx_msg void OnSubstmodeCompressdeltas();
	afx_msg void OnSubstmodeExtendedencoding();
	afx_msg void OnSubstmodeForcereservededit();
	afx_msg void OnSubstmodeLockersname();
	afx_msg void OnSubstmodeNosubstitution();
	afx_msg void OnSubstmodeSubstitutekeyword();
	afx_msg void OnSubstmodeSubstitutevalue();
	afx_msg void OnSubstmodeUnicode();
	afx_msg void OnSubstmodeUnixlineendings();
	afx_msg void OnEditchangeKeywordOpt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CBitmap m_popupBitmap;

	// Methods
	virtual void InitHelpID();

	void ToggleSubstmodeOption(const char opt, const bool isEncoding = false);
	void UpdateDescription();
	
public:
	// Interface
	BOOL Create(const CWnd* pWndInsertAfter, CDialog* pParentWnd, const bool updateData = true);
	void EnableControls(const BOOL enable);
	
	BOOL UpdateData(BOOL bSaveAndValidate, CString& entryText);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBSTMODEENTRYDLG_H__C1669CF1_D25C_4439_905B_F652482CE575__INCLUDED_)
