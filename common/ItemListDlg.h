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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> (Originally: Sebastien Abras <sabdev@ibelgique.com>) --- July 2002
 */

#if !defined(AFX_ITEMLISTDLG_H__E6D16D27_1A84_4E8A_A926_3B76728E0F73__INCLUDED_)
#define AFX_ITEMLISTDLG_H__E6D16D27_1A84_4E8A_A926_3B76728E0F73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemListDlg.h : header file
//

#include "MultiFiles.h"
#include "AppGlue.h"
#include "SmartComboBox.h"

#ifdef WIN32
#	include "SortHeaderCtrl.h"
#	include "HHelpSupport.h"
#	include "CvsrootEntryDlg.h"
#endif

#include "CvsRootDlg.h"

#include <string>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <algorithm>

class ItemSelectionData;

bool CompatGetTagListItem(const MultiFiles* mf, 
						  std::string& tagName, 
							 const ItemSelectionData* itemSelectionData = NULL);

bool CompatGetModuleListItem(const MultiFiles* mf, 
							 std::string& moduleName, 
							 const ItemSelectionData* itemSelectionData = NULL);

// persistent default values for the dates, tags...
extern CMString gRevNames;
extern CMString gDateNames;

/// Enum types of list dialog
typedef enum
{
	kItemListTag,	/*!< List branches/tags */
	kItemListModule	/*!< List modules/directories & files */
} kItemListDlg;

/// Enum types of item list auto refresh
typedef enum
{
	kAutoRefreshDisable,	/*!< Disable auto refresh */
	kAutoRefreshFirstTime,	/*!< First time dialog opens refresh only */
	kAutoRefreshAlways		/*!< Always refresh */
} kItemListAutoRefreshType;

/// Item's description container
typedef std::set<std::string> ItemDescription;

/// Structure to hold item's row information
typedef struct
{
	ItemDescription m_description;		/*!< Item's description */
	ItemDescription m_descriptionEx;	/*!< Item's extended description */
} ItemRowInfo;

/// Item's rows container
typedef std::map<std::string, ItemRowInfo> ItemList;

/// Path to the list location
typedef std::list<std::string> ListPath;

/// Item selection data class
class ItemSelectionData
{
public:
	// Construction
	ItemSelectionData();
#ifdef WIN32
	ItemSelectionData(const CCvsrootEntryDlg& cvsrootEntryDlg);
#endif

private:
	// Data members
	bool m_forceCvsroot;	/*!< Force CVSROOT */
	std::string m_cvsroot;	/*!< CVSROOT */

public:
	// Interface
	bool GetForceCvsroot() const;
	const std::string& GetCvsroot() const;
};

/// Base class for item list console
class CItemListConsole : public CCvsRootConsole
{
public:
	// Construction
	CItemListConsole();
	virtual ~CItemListConsole();

protected:
	// Data members
	ItemList m_itemList;	/*!< Parsing result */

public:
	// Interface
	const ItemList& GetItemList() const;
};

///	Class to parse tags/branches (obtained using <b>cvs log</b> or <b>cvs rlog</b>)
class CTagParserConsole : public CItemListConsole
{
public:
	// Construction
	CTagParserConsole();
	virtual ~CTagParserConsole();

public:
	// Interface
	virtual void Parse(int launchRes);
};

/// Class to parse modules (obtained using <b>cvs co -c</b>)
class CModuleParserConsole : public CItemListConsole
{
public:
	// Construction
	CModuleParserConsole();
	virtual ~CModuleParserConsole();

private:
	// Methods
	void Normalize();
	bool NeedsNormalize() const;

public:
	// Interface
	virtual void Parse(int launchRes);
};

/// Class to parse directories/files (obtained using <b>cvs ls</b>)
class CListParserConsole : public CItemListConsole
{
public:
	// Construction
	CListParserConsole();
	virtual ~CListParserConsole();

public:
	// Interface
	virtual void Parse(int launchRes);
};



/// Display type enum
typedef enum
{
	kItemDisplayWait,	/*!< Display the wait prompt while retrieving the data */
	kItemDisplayData,	/*!< Display the data */
	kItemDisplayInfo	/*!< Display the information */
} kItemDisplayType;

/// Class to hold and support data display (e.g. sorting or storing the result for next display)
class ItemDisplayInfo
{
private:
	// Data members
	std::string m_item;				/*!< Item text */
	std::string m_description;		/*!< Description text */
	std::string m_descriptionEx;	/*!< Extended description text */

	ItemRowInfo m_rowInfo;	/*!< Original row data after parsing */

public:
	// Interface
	void Create(const std::string& item, const ItemRowInfo& rowInfo);

	const std::string& GetItem() const;
	const std::string& GetDescription() const;
	const std::string& GetDescriptionEx() const;

	const ItemRowInfo& GetRowInfo() const;
};

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CItemListDlg dialog

class CItemListDlg : public CHHelpDialog
{
// Construction
public:
	CItemListDlg(const MultiFiles* mf,
		const char* item, kItemListDlg type, ItemList& itemList, const ListPath* listPath,
		bool forceRoot, const char* cvsroot, 
		const char* rev, const char* date,
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CItemListDlg)
	enum { IDD = IDD_ITEM_LIST };
	CStatic	m_cvsrootEntryPlaceholder;
	CSmartComboBox	m_autoRefreshCombo;
	CButton	m_selectionButton;
	CButton	m_cancelButton;
	CButton	m_browseTagButton;
	CEdit	m_itemEdit;
	CButton	m_refreshButton;
	CButton	m_okButton;
	CButton	m_browseReadFromButton;
	CStatic	m_itemLabelStatic;
	CButton	m_hasDateCheck;
	CButton	m_hasRevCheck;
	CSmartComboBox	m_dateCombo;
	CSmartComboBox	m_revCombo;
	CButton	m_optionCheck;
	CListCtrl	m_itemListCtrl;
	CSmartComboBox	m_readFromCombo;
	CString	m_readFrom;
	CString	m_item;
	BOOL	m_option;
	BOOL	m_hasRev;
	CString	m_rev;
	CString	m_date;
	BOOL	m_hasDate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemListDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CItemListDlg)
	afx_msg void OnRefresh();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnOption();
	afx_msg void OnCheckrev();
	afx_msg void OnCheckdate();
	afx_msg void OnBrowseTag();
	virtual void OnOK();
	afx_msg void OnDblclkItemlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedItemlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickItemlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBrowseReadfrom();
	afx_msg void OnSelection();
	afx_msg void OnSelchangeAutoRefresh();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CCvsrootEntryDlg m_cvsrootEntryDlg; /*!< CVSROOT entry child dialog */

	const MultiFiles* m_mf;	/*!< Browser selection */
	
	kItemListDlg m_type;	/*!< Dialog type */
	ItemList& m_itemList;	/*!< Item's container */
	bool m_cvsStopped;		/*!< Flag indicating that user stopped the command */

	// Methods
	virtual void InitHelpID();

	void InitListCtrl(const kItemDisplayType displayType);
	void InitOptionCtrls();
	void EnableControls(BOOL enable);
	bool EnableDateRev();

	bool CheckSettings();
	BOOL PostClickRefresh();

	// Items management

	/// Images enum
	enum
	{
		kItemImgBlank,				/*!< Blank */
		kItemImgUpDir,				/*!< Go one directory up */
		kItemImgDir,				/*!< Directory */
		kItemImgFile,				/*!< File */
		kItemImgModule,				/*!< Module */
		kItemImgRevision,			/*!< Revision */
		kItemImgBranch,				/*!< Branch */
		kItemImgBranchAndRevision,	/*!< Branch and revision */
	};

	CImageList m_itemsImageList;

	bool SelectItem(const char* item);
	void DisplayItems(const ItemList& itemList, const int launchRes, std::istrstream* cvsErr = NULL, const bool showUpDir = true);
	int GetImageForItem(kItemListDlg type, bool option, const ItemDisplayInfo* displayInfo);
	void DeleteAllItems();

	// Sorting

	/// Structure to hold the sorting settings
	typedef struct
	{
		int m_column;			/*!< Sorting column */
		bool m_asc;				/*!< Sort ascending or descending */
		kItemListDlg m_type;	/*!< Dialog type */
		bool m_option;			/*!< Option state */
	} CompareInfo;

	bool m_sortAsc;		/*!< Current ascending/descending sorting */
	int m_sortColumn;	/*!< Current sort column */
	CSortHeaderCtrl m_headerCtrl;

	void SortItems(int column, bool asc, kItemListDlg type, bool option);
	static int CALLBACK CompareItems(LPARAM itemData1, LPARAM itemData2, LPARAM lParam);
	
public:
	// Interface
	BOOL m_forceRoot;		/*!< Force CVSROOT flag */
	CString m_cvsroot;		/*!< CVSROOT string */

	ListPath m_listPath;	/*!< Path to the current list location */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#ifdef qMacCvsPP
#include <LOutlineTable.h>
#include <LCommander.h>
#include <LOutlineItem.h>
#include <LString.h>

class LSelectTable : public LOutlineTable, public LCommander, public LBroadcaster
{
public:
	enum { class_ID = 'Sfil' };

	LSelectTable( LStream *inStream );
	virtual ~LSelectTable();
	
	Handle mIconModule;
	Handle mIconTag;
	Handle mIconBranch;
protected:
	virtual	void FinishCreateSelf();
};

class CSelectItem : public LOutlineItem
{
public:
	CSelectItem(const string& item, const ItemRowInfo& rowInfo);
	CSelectItem();
	virtual ~CSelectItem();

	string m_item;			/*!< Item text */
	string m_description;	/*!< Description text */
	string m_descriptionEx;	/*!< Extended description text */
	Handle mIcon;

protected:
	virtual void GetDrawContentsSelf(const STableCell& inCell, SOutlineDrawContents& ioDrawContents);

	virtual void DrawRowAdornments(const Rect& inLocalRowRect);

	virtual void DoubleClick(const STableCell& inCell, const SMouseDownEvent& inMouseDown,
		const SOutlineDrawContents&	inDrawContents, Boolean inHitText);
	

	ItemRowInfo m_rowInfo;	/*!< Original row data after parsing */
};
#endif /* qMacCvsPP */

#endif // !defined(AFX_ITEMLISTDLG_H__E6D16D27_1A84_4E8A_A926_3B76728E0F73__INCLUDED_)
