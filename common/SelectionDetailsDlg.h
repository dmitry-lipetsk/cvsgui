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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 2003
 */

#if !defined(AFX_SELECTIONDETAILSDLG_H__42708DEE_6E9B_41FD_BAE6_0224C8EE09D8__INCLUDED_)
#define AFX_SELECTIONDETAILSDLG_H__42708DEE_6E9B_41FD_BAE6_0224C8EE09D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectionDetailsDlg.h : header file
//

#include "MultiFiles.h"

#ifdef WIN32
#	include "SortHeaderCtrl.h"
#	include "HHelpSupport.h"
#endif

/// Enum types of selection details dialog
typedef enum
{
	kSelDetFile,		/*!< Select file */
	kSelDetRevision,	/*!< Select revision */
	kSelDetPath			/*!< Select path */
} kSelDetType;

bool CompatGetSelectionDetails(const MultiFiles* mf, kSelDetType& selDetType, std::string& selection);

#ifdef WIN32

/// Class to hold and support data display (e.g. sorting)
class SelectionDisplayInfo
{
private:
	// Data members
	std::string m_fileName;	/*!< Filename */
	std::string m_revision;	/*!< Revision */
	std::string m_path;		/*!< Path */

public:
	// Interface
	void Create(const std::string& fileName, const std::string& revision, const std::string& path);

	const std::string& GetFileName() const;
	const std::string& GetRevision() const;
	const std::string& GetPath() const;
};

/////////////////////////////////////////////////////////////////////////////
// CSelectionDetailsDlg dialog

class CSelectionDetailsDlg : public CHHelpDialog
{
// Construction
public:
	CSelectionDetailsDlg(const MultiFiles* mf, const kSelDetType selDetType, const char* selection, 
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectionDetailsDlg)
	enum { IDD = IDD_SELECTION_DETAILS };
	CButton	m_revisionCheck;
	CButton	m_fileNameCheck;
	CButton	m_pathCheck;
	CEdit	m_selectionEdit;
	CSpinButtonCtrl	m_revisionSpinCtrl;
	CListCtrl	m_selectionListCtrl;
	CString	m_selection;
	int		m_selDetType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectionDetailsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectionDetailsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkSelectionlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickSelectionlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedSelectionlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposRevisionSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFilename();
	afx_msg void OnPath();
	afx_msg void OnRevision();
	afx_msg void OnDestroy();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const MultiFiles* m_mf;	/*!< Browser selection */

	// Methods
	virtual void InitHelpID();

	void DisplaySelection();
	void DeleteAllItems();
	
	void EnableControls();
	void UpdateSelect(int item);

	// Sorting

	/// Structure to hold the sorting settings
	typedef struct
	{
		int m_column;				/*!< Sorting column */
		bool m_asc;					/*!< Sort ascending or descending */
	} CompareInfo;

	bool m_sortAsc;		/*!< Current ascending/descending sorting */
	int m_sortColumn;	/*!< Current sort column */
	CSortHeaderCtrl m_headerCtrl;

	void SortItems(int column, bool asc);
	static int CALLBACK CompareItems(LPARAM itemData1, LPARAM itemData2, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif /* !WIN32 */

#endif // !defined(AFX_SELECTIONDETAILSDLG_H__42708DEE_6E9B_41FD_BAE6_0224C8EE09D8__INCLUDED_)
