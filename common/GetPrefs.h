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

#if !defined(AFX_GETPREFS_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_GETPREFS_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SmartComboBox.h"

#ifdef WIN32
#	include "Authen.h"
#	include "HHelpSupport.h"
#	include "HtmlStatic.h"
#endif 

#ifdef qUnix
#include "uwidget.h"
#endif

void CompatGetPrefs(void);

#ifdef WIN32

/// Global preferences property page
class CGetPrefs_GLOBALS : public CHHelpPropertyPage
{
	DECLARE_DYNCREATE(CGetPrefs_GLOBALS)

// Construction
public:
	CGetPrefs_GLOBALS();
	~CGetPrefs_GLOBALS();

// Dialog Data
	//{{AFX_DATA(CGetPrefs_GLOBALS)
	enum { IDD = IDD_PREFS_GLOBALS };
	CSmartComboBox	m_cvsMessagesCombo;
	CSmartComboBox	m_checkoutFileAttributeCombo;
	CSpinButtonCtrl	m_logoutSpinButton;
	CEdit	m_logoutEdit;
	CButton	m_logoutCheck;
	CSpinButtonCtrl	m_refreshInactiveSpinButton;
	CEdit	m_refreshInactiveEdit;
	CButton	m_refreshInactiveCheck;
	CSpinButtonCtrl	m_zspinSpinButton;
	CEdit	m_zlevelEdit;
	CButton	m_tcpipCheck;
	BOOL	m_prune;
	BOOL	m_tcpip;
	BOOL	m_cntladd;
	BOOL	m_logout;
	UINT	m_logoutTime;
	BOOL	m_refreshInactive;
	UINT	m_refreshInactiveTime;
	BOOL	m_unixlf;
	UINT	m_zlevel;
	BOOL	m_disableSplashScreen;
	BOOL	m_matchEntriesCase;
	BOOL	m_smartSorting;
	BOOL	m_encryptCommunication;
	int		m_checkoutFileAttribute;
	int		m_cvsMessages;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGetPrefs_GLOBALS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPrefs_GLOBALS)
	afx_msg void OnLogout();
	afx_msg void OnRefreshInactive();
	afx_msg void OnCheckunixlf();
	afx_msg void OnTcpip();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool m_isModified;	/*!< Flag indicating data change */

	// Methods
	virtual void InitHelpID();

public:
	// Interface
	void StoreValues();
};

/// WinCvs preferences property page
class CGetPrefs_WINCVS : public CHHelpPropertyPage
{
	DECLARE_DYNCREATE(CGetPrefs_WINCVS)

// Construction
public:
	CGetPrefs_WINCVS();
	~CGetPrefs_WINCVS();

// Dialog Data
	//{{AFX_DATA(CGetPrefs_WINCVS)
	enum { IDD = IDD_PREFS_WINCVS };
	CListCtrl	m_settingsList;
	CButton	m_clearButton;
	CButton	m_browseButton;
	CSmartComboBox	m_settingsTypeCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGetPrefs_WINCVS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPrefs_WINCVS)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWincvsSettingsType();
	afx_msg void OnBrowseWincvsSettings();
	afx_msg void OnClearWincvsSettings();
	afx_msg void OnDblclkWincvsSettingsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedWincvsSettingsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool m_isModified; /*!< Flag indicating data change */
	int m_lastSelectedType; /*!< Store last selected type */
	
	CString m_extDiff;
	CString m_viewer;
	CString m_conflictEditor;
	CString m_annotateViewer;
	CString m_pythonLibrary;
	CString m_tclLibrary;

	CString m_settingsFolder;
	CString m_macrosFolder;
	CString m_pythonLibFolder;

	/// Settings type enum
	enum kSettingsType
	{
		kPrograms = 0,
		kFolders
	};

	/// Setting info helper class
	class CSettingInfo
	{
	public:
		/// Browse type enum
		enum kBrowseType
		{
			kBrowseExecutable = 0,
			kBrowseDll,
			kBrowseFolder,
		};

		// Construction
		CSettingInfo(kBrowseType browseType, const char* defaultPattern);

		// Interface
		kBrowseType GetBrowseType() const;
		const std::string& GetDefaultPattern() const;

	private:
		// Data members
		kBrowseType m_browseType;
		std::string m_defaultPattern;
	};


	// Methods
	virtual void InitHelpID();
	void InitListCtrl();
	int AddSetting(const char* name, const char* value, CSettingInfo::kBrowseType browseType, const char* defaultPattern = "");
	void EnableEditControls();
	void GetSettings();
	void ClearItemsDisplay();
	void ShowSettings();

public:
	// Interface
	void StoreValues();
};

/// CVS preferences property page
class CGetPrefs_CVS : public CHHelpPropertyPage
{
	DECLARE_DYNCREATE(CGetPrefs_CVS)

// Construction
public:
	CGetPrefs_CVS();
	~CGetPrefs_CVS();

// Dialog Data
	//{{AFX_DATA(CGetPrefs_CVS)
	enum { IDD = IDD_PREFS_CVS };
	CButton	m_commandLineLimitCheck;
	CEdit	m_commandLineLimitEdit;
	CSmartComboBox	m_altCvsCombo;
	CButton	m_cvsConsoleCheck;
	CSmartComboBox	m_showCvsCombo;
	CButton	m_selAltCvsButton;
	CButton	m_useAltCvsCheck;
	CEdit	m_homeEdit;
	CString	m_home;
	BOOL	m_useAltCvs;
	BOOL	m_cvsConsole;
	int		m_showCvs;
	CString	m_altCvs;
	int		m_commandLineLimit;
	BOOL	m_enableCommandLineLimit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGetPrefs_CVS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPrefs_CVS)
	afx_msg void OnBtnhome();
	afx_msg void OnAltcvschk();
	afx_msg void OnAltcvssel();
	virtual BOOL OnInitDialog();
	afx_msg void OnConsole();
	afx_msg void OnEnableCmdlineLimit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool m_isModified; /*!< Flag indicating data change */

	// Methods
	virtual void InitHelpID();

public:
	// Interface
	void StoreValues();
};

/// Command dialogs preferences property page
class CGetPrefs_CVSCMD_DIALOGS : public CHHelpPropertyPage
{
	DECLARE_DYNCREATE(CGetPrefs_CVSCMD_DIALOGS)

// Construction
public:
	CGetPrefs_CVSCMD_DIALOGS();
	~CGetPrefs_CVSCMD_DIALOGS();

	void StoreValues();

// Dialog Data
	//{{AFX_DATA(CGetPrefs_CVSCMD_DIALOGS)
	enum { IDD = IDD_PREFS_CVSCMD_DIALOGS };
	CCheckListBox	m_skipDialogsListBox;
	int		m_keyForce;
	BOOL	m_ctrlPressedOnOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGetPrefs_CVSCMD_DIALOGS)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPrefs_CVSCMD_DIALOGS)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool m_isModified;	/*!< Flag indicating data change */

	// Methods
	virtual void InitHelpID();
};

/// Dummy General preferences dialog with migration information only
class CGetPrefs_General : public CHHelpPropertyPage
{
	DECLARE_DYNCREATE(CGetPrefs_General)

// Construction
public:
	CGetPrefs_General();
	~CGetPrefs_General();

// Dialog Data
	//{{AFX_DATA(CGetPrefs_General)
	enum { IDD = IDD_PREFS_GENERAL_DUMMY };
	CHtmlStatic	m_cvsrootMigration;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGetPrefs_General)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGetPrefs_General)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif /* WIN32 */

#ifdef qUnix

class UCvsPrefs_GCVS : public UWidget
{
	UDECLARE_DYNAMIC(UCvsPrefs_GCVS)
public:
	UCvsPrefs_GCVS();
	virtual ~UCvsPrefs_GCVS() {}

	enum
	{
		// program paths
		kdiffApp = EV_COMMAND_START,  // 0
		kviewerApp,		              // 1
		kbrowserApp,	              // 2
		khelpApp,		              // 3
		kconsoleFont,                 // 4  
		kcvsApp,                      // 5		

		// browse button
		kdiffBtn, 	                  // 6
		kviewerBtn,	 	              // 7
		kbrowserBtn,	              // 8
		khelpBtn,		              // 9		
		kcvsBtn,                      // 10
		kconsoleFontBtn,              // 11

		kuseAltCvs                    // 12		
	};

	virtual void DoDataExchange(bool fill);
	
	void StoreValues(void);

protected:
	std::string m_diffApp;
	std::string  m_viewerApp;
	std::string  m_browserApp;
	std::string  m_helpApp;
	bool m_useAltCvs;
    std::string  m_cvsApp;
	std::string  m_consoleFont;

	ev_msg int OnPickProgram(int cmd);
	ev_msg int OnUseAlternateCvs(void);
	ev_msg int UCvsPrefs_GCVS::OnSelectFont(void);

	bool checkCVS(const char* filename);

	UDECLARE_MESSAGE_MAP()
};
#endif /* qUnix */

#endif // !defined(AFX_GETPREFS_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
