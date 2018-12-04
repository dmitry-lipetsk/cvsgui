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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- March 2004
 */

#if !defined(AFX_CVSROOTDLG_H__E7B87F15_691C_407E_AFAE_910BE8E98A0A__INCLUDED_)
#define AFX_CVSROOTDLG_H__E7B87F15_691C_407E_AFAE_910BE8E98A0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CvsRootDlg.h : header file
//

#include "AppGlue.h"
#include "SmartComboBox.h"

#ifdef WIN32
#	include "HHelpSupport.h"
#endif

#include <string>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <algorithm>

bool CompatGetCvsRoot(std::string& cvsRoot);

/// Enum types of info commands
typedef enum
{
	kInfoCmd,		/*!< Info command */
	kProtocolCmd	/*!< Protocol info command */
} kInfoCmdType;

/// CVS protocol list container
typedef std::map<std::string, std::string> CvsProtocolList;

/// Structure to hold protocol row information
typedef struct
{
	std::string m_keyword;		/*!< Keyword */
	std::string m_description;	/*!< Description */
} KeywordInfo;

/// Keyword list
typedef std::vector<KeywordInfo> KeywordList;

/// CVS Output/Error buffer proxy
class CCvsOutBufferProxy
{
public:
	// Construction
	CCvsOutBufferProxy(const char* data, int size);
private:
	CCvsOutBufferProxy();

	// Data members
	const char* m_data;	/*!< Pointer to the data */
	int m_size;			/*!< Data size */

public:
	// Interface
	const char* GetData() const;
	int GetSize() const;
};

/// Base class for cvsRoot list console
class CCvsRootConsole : public CCvsConsole
{
public:
	// Construction
	CCvsRootConsole();
	virtual ~CCvsRootConsole();

	// Interface
	virtual long cvs_out(const char* txt, long len);
	virtual long cvs_err(const char* txt, long len);
	virtual void Parse(int launchRes) = 0L;

private:
	// Data members
	std::vector<char> m_cvsOutVec;	/*!< CVS command result (stdout) used for parsing */
	std::vector<char> m_cvsErrVec;	/*!< CVS command result (stderr) used for parsing */

protected:
	int m_launchRes;	/*!< The result of the command launch */

public:
	int GetLaunchRes() const;
	CCvsOutBufferProxy GetCvsOut() const;
	void SetCvsOut(const char* str, size_t lLen);
	CCvsOutBufferProxy GetCvsErr() const;
};

///	Class to parse protocols available (obtained using <b>cvs info</b>)
class CInfoParserConsole : public CCvsRootConsole
{
public:
	// Construction
	CInfoParserConsole();
	virtual ~CInfoParserConsole();

private:
	// Data members
	CvsProtocolList m_cvsProtocolList;	/*!< Parsed CVS protocol list */

public:
	// Interface
	virtual void Parse(int launchRes);

	const CvsProtocolList& GetCvsProtocolList() const;
};

/// Class to parse protocol info (obtained using <b>cvs info <i>protocol</i></b>)
class CProtocolParserConsole : public CCvsRootConsole
{
public:
	// Construction
	CProtocolParserConsole();
	virtual ~CProtocolParserConsole();

private:
	// Data members
	KeywordList m_keywordList;	/*!< Parsed protocol keywords list */

public:
	// Interface
	virtual void Parse(int launchRes);
	
	const KeywordList& GetKeywordList() const;
};

/// Class to parse CVS root string
class CCvsRootParser
{
private:
	// Data members
	std::string m_protocol;				/*!< Parsed protocol */
	std::string m_path;					/*!< Parsed repository path */
	CvsProtocolList m_cvsProtocolList;	/*!< Parsed CVS protocol list */

	// Methods
	bool SplitCvsRoot(const char* cvsRoot, std::string& protocol, std::string& keywords, std::string& path) const;
	int NormalizeCvsRoot(std::string& strCvsRoot, const char tokenSeparator) const;

public:
	// Interface
	bool Parse(const char* cvsRoot, const bool clearData = true);
	void Clear();

	bool IsEmpty() const;

	const std::string& GetProtocol() const;
	const std::string& GetPath() const;
	const CvsProtocolList& GetCvsProtocolList() const;
};

/// Display type enum
typedef enum
{
	kCvsRootDisplayWait,	/*!< Display the wait prompt while retrieving the data */
	kCvsRootDisplayData,	/*!< Display the data */
	kCvsRootDisplayInfo		/*!< Display the information */
} kCvsRootDisplayType;

/// Class to hold and support data display
class KeywordDisplayInfo
{
private:
	// Data members
	std::string m_name;		/*!< Display keyword's name */
	std::string m_value;	/*!< Keyword's value */

	KeywordInfo m_rowInfo;	/*!< Original row data after parsing */

public:
	// Interface
	void Create(const KeywordInfo& rowInfo);

	const std::string& GetName() const;
	
	const std::string& GetValue() const;
	void SetValue(const char* value);

	const KeywordInfo& GetRowInfo() const;
};

/// Display info list
typedef std::vector<const KeywordDisplayInfo*> KeywordDisplayInfoList;

#ifdef WIN32

/////////////////////////////////////////////////////////////////////////////
// CCvsRootDlg dialog

class CCvsRootDlg : public CHHelpDialog
{
// Construction
public:
	CCvsRootDlg(const char* cvsRoot, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCvsRootDlg)
	enum { IDD = IDD_CVSROOT };
	CSmartComboBox	m_repositoryPathCombo;
	CEdit	m_cvsrootStatic;
	CStatic	m_keywordDescStatic;
	CButton	m_okButton;
	CButton	m_cancelButton;
	CStatic	m_protocolDescStatic;
	CButton	m_clearButton;
	CButton	m_editButton;
	CListCtrl	m_keywordList;
	CSmartComboBox	m_protocolCombo;
	CString	m_protocol;
	CString	m_cvsroot;
	CString	m_repositoryPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCvsRootDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCvsRootDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDblclkKeywordlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedKeywordlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeProtocol();
	afx_msg void OnEdit();
	afx_msg void OnClear();
	afx_msg void OnEditchangePath();
	afx_msg void OnSelchangePath();
	afx_msg void OnHelp();
	//}}AFX_MSG
	afx_msg void OnReadProtocols();
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	bool m_cvsStopped;				/*!< Flag indicating that user stopped the command */
	CCvsRootParser m_cvsRootParser;	/*!< CVS root parser */

	// Methods
	virtual void InitHelpID();

	void InitListCtrl(const kCvsRootDisplayType displayType);
	void EnableControls(BOOL enable);
	
	void EnableEditControls(const KeywordDisplayInfo* displayInfo);
	
	BOOL PostReadProtocolsMsg();
	
	void ReadAvailableProtocols();
	void ReadProtocolDetails();
	void ShowProtocolDescription();

	void FillProtocolsComboBox(CInfoParserConsole& infoParserConsole);
	void DisplayKeywords(CProtocolParserConsole& protocolParserConsole);
	void UpdateKeywords(const CvsProtocolList& protocolList);

	void DeleteAllItems();

	void UpdateCvsRoot();
};

/////////////////////////////////////////////////////////////////////////////
// CEditKeywordDlg dialog

class CEditKeywordDlg : public CHHelpDialog
{
// Construction
public:
	CEditKeywordDlg(const char* description, const char* keyword, const char* value, 
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditKeywordDlg)
	enum { IDD = IDD_EDITKEYWORD };
	CSmartComboBox	m_valueCombo;
	CString	m_description;
	CString	m_keyword;
	CString	m_value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditKeywordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditKeywordDlg)
	afx_msg void OnHelp();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Methods
	virtual void InitHelpID();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // WIN32

#endif // !defined(AFX_CVSROOTDLG_H__E7B87F15_691C_407E_AFAE_910BE8E98A0A__INCLUDED_)
