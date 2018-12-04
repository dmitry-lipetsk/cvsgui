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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * DiffDlg.h : the cvs diff dialog
 */

#if !defined(AFX_DIFFDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_DIFFDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"
#include "uwidget.h"
#include "CvsCommands.h"

#ifdef WIN32
#	include "SortHeaderCtrl.h"
#	include "HHelpSupport.h"
#	include "SubstModeEntryDlg.h"
#endif

class MultiFiles;

/// Compare type enum
enum
{
	kCompareSameRev = 0,	/*!< Compare local copy against the same remote revision */
	kCompareAnotherRev,		/*!< Compare local copy against another revision/tag/branch or date */
	kCompareTwoRev			/*!< Compare two revisions/tags/branches or dates against each other */
};

bool CompatGetDiff(const MultiFiles* mf, kDiffCmdType diffType, 
				   bool& noRecurs, bool& isDate1, bool& isDate2,
				   std::string& rev1, std::string& rev2, bool& useExtDiff, std::string& extDiffTool,
				   bool& bulkUpdate, bool& useUnifiedDiff, bool& noWhiteSpace, bool& noBlankLines, bool& ignoreCase,
				   std::string& keyword, std::string& diffOptions, 
				   bool allowExtDiff = false);

#ifdef WIN32

class CDiff_MAIN : public CHHelpPropertyPage
{
	DECLARE_DYNAMIC(CDiff_MAIN)

// Construction
public:
	CDiff_MAIN(const MultiFiles* mf, kDiffCmdType diffType, 
		const char* rev1, const char* rev2, 
		bool allowExtDiff, bool useExtDiff, bool bulkUpdate,
		bool unifiedDiff, bool noWhiteSpace, bool ignoreCase,
		const char* keyword, const char* diffOptions);
	~CDiff_MAIN();

// Dialog Data
	//{{AFX_DATA(CDiff_MAIN)
	enum { IDD = IDD_DIFF_MAIN };
	CStatic	m_substModeEntryPlaceholder;
	CSmartComboBox	m_diffOptionsCombo;
	CButton	m_noWhiteSpaceCheck;
	CButton	m_ignoreCaseCheck;
	CButton	m_extDiffButton;
	CButton	m_bulkUpdateButton;
	CButton	m_optionsMenuButton;
	CButton	m_noRecursCheck;
	CButton	m_unifiedDiffCheck;
	CButton	m_browseTag2Button;
	CButton	m_browseTag1Button;
	CButton	m_isDate2Check;
	CButton	m_isDate1Check;
	CSmartComboBox	m_compareTypeCombo;
	CSmartComboBox	m_rev2Combo;
	CSmartComboBox	m_rev1Combo;
	BOOL	m_isDate1;
	BOOL	m_isDate2;
	CString	m_rev1;
	CString	m_rev2;
	BOOL	m_noWhiteSpace;
	BOOL	m_ignoreCase;
	int		m_compareType;
	BOOL	m_unifiedDiff;
	BOOL	m_noRecurs;
	BOOL	m_extDiff;
	BOOL	m_bulkUpdate;
	CString	m_diffOptions;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDiff_MAIN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDiff_MAIN)
	afx_msg void OnExtdiff();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnBrowseTag1();
	afx_msg void OnBrowseTag2();
	afx_msg void OnSelchangeCompareType();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptionsmenu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	/// Input change enum
	typedef enum
	{
		kDefRev1Change,	/*!< Revision 1 date checkbox changed */
		kDefRev2Change,	/*!< Revision 2 date checkbox changed */
		kDefTypeChange	/*!< Compare type change */
	} kInputChange;


	// Data members
	CSubstModeEntryDlg m_substModeEntryDlg; /*!< CVSROOT entry child dialog */
	const MultiFiles* m_mf;					/*!< Browser selection */

	bool m_allowExtDiff;					/*!< Allow external diff flag */
	kDiffCmdType m_diffType;				/*!< Diff command type */

	// Methods
	virtual void InitHelpID();

	void EnableCompareType(const int compareType);
	void EnableCmdLineDiffOptions(bool enable);
	
	void SetDefaultInput(const kInputChange inputChange, const int compareType);
	bool IsDefaultedRev(const CComboBox* comboBox);

public:
	// Interface
	CString m_keyword;	/*!< Keyword substitution */
};

/////////////////////////////////////////////////////////////////////////////
// CDiff_Options dialog

class CDiff_Options : public CHHelpDialog
{
// Construction
public:
	CDiff_Options(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiff_Options)
	enum { IDD = IDD_DIFF_OPTIONS };
	CListCtrl	m_optionsListCtrl;
	CString	m_currentOptions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiff_Options)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDiff_Options)
	virtual void OnOK();
	afx_msg void OnItemchangedOptionslist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickOptionslist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CString m_diffOptions;	/*!< Diff options */

	// Sorting
	/// Structure to hold the sorting settings
	typedef struct
	{
		int m_column;	/*!< Sorting column */
		bool m_asc;		/*!< Sort ascending or descending */
	} CompareInfo;

	bool m_sortAsc;			/*!< Current ascending/descending sorting */
	int m_sortColumn;		/*!< Current sort column */
	CSortHeaderCtrl m_headerCtrl;

	// Methods
	virtual void InitHelpID();

	void SortItems(int column, bool asc);
	static int CALLBACK CompareItems(LPARAM itemData1, LPARAM itemData2, LPARAM lParam);

public:
	// Interface
	CString GetDiffOptions();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif /* WIN32 */

#ifdef qUnix
#ifdef _DIFF_CPP_
class UCvsDiff : public UWidget
{
	UDECLARE_DYNAMIC(UCvsDiff)
public:
	UCvsDiff() : UWidget(::UEventGetWidID()) {}
	virtual ~UCvsDiff() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kTabGeneral				// 2
	};

	virtual void DoDataExchange(bool fill);

protected:
	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsDiff, UWidget)

UBEGIN_MESSAGE_MAP(UCvsDiff, UWidget)
	ON_UCOMMAND(UCvsDiff::kOK, UCvsDiff::OnOK)
	ON_UCOMMAND(UCvsDiff::kCancel, UCvsDiff::OnCancel)
UEND_MESSAGE_MAP()

class UCvsDiff_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(UCvsDiff_MAIN)
public:
	UCvsDiff_MAIN(const char *rev1, const char *rev2, bool allowExtDiff, bool useUnifiedDiff, 
  		 		  bool noWhiteSpace, bool noBlankLines, bool ignoreCase);
	virtual ~UCvsDiff_MAIN() {}

	enum
	{
		kCheckDiff1 = EV_COMMAND_START,	// 0
		kCheckDiff2,			// 1
		kCheckDiff3,			// 2
		kCheckDate1,			// 3
		kCheckDate2,			// 4
		kComboRev1,				// 5
		kComboRev2,				// 6
		kCheckRecurs,			// 7
		kCheckExtDiff,			// 8
		kCheckNoWhiteSpace,     // 9
		kCheckIgnoreCase,       // 10
		kCheckNoBlankLines,		// 11
		kCheckUnifiedDiff		// 12
	};

	virtual void DoDataExchange(bool fill);

	bool m_check1;
	bool m_check2;
	std::string m_rev1;
	std::string m_rev2;
	int m_diff;
	bool m_norecurs;
	bool m_extdiff;
	bool m_unifiedDiff;
	bool m_allowExtDiff;
	bool m_noWhiteSpace;
	bool m_noBlankLines;
	bool m_ignoreCase;

protected:
	ev_msg int OnDiff(int cmd);

	UDECLARE_MESSAGE_MAP()
};

UIMPLEMENT_DYNAMIC(UCvsDiff_MAIN, UWidget)

UBEGIN_MESSAGE_MAP(UCvsDiff_MAIN, UWidget)
	ON_UCOMMAND_RANGE(UCvsDiff_MAIN::kCheckDiff1, UCvsDiff_MAIN::kCheckDiff2, UCvsDiff_MAIN::OnDiff)
UEND_MESSAGE_MAP()
#endif // _DIFF_CPP_

#endif // qUnix

#endif // !defined(AFX_DIFFDLG_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
