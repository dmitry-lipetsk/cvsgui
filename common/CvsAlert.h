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

// CvsAlert.h : header file
//

#if !defined(AFX_CVSALERT_H__ACFE8C41_9836_11D1_AE50_00C0F0067852__INCLUDED_)
#define AFX_CVSALERT_H__ACFE8C41_9836_11D1_AE50_00C0F0067852__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>

#include "uwidget.h"
#include "cvsgui_i18n.h"

/// Title for OK button
#define BUTTONTITLE_OK			_i18n("OK")

/// Title for Cancel button
#define BUTTONTITLE_CANCEL		_i18n("Cancel")

/// Title for Continue button
#define BUTTONTITLE_CONTINUE	_i18n("Continue")

/// Title for Yes button
#define BUTTONTITLE_YES			_i18n("Yes")

/// Title for No button
#define BUTTONTITLE_NO			_i18n("No")

/// Title for Delete button
#define BUTTONTITLE_DELETE		_i18n("Delete")

class CPersistentBool;

/// Icon type enum
typedef enum {
	kCvsAlertNoIcon,      /*!< No icon */
	kCvsAlertStopIcon,    /*!< Stop icon (for critical errors) */
	kCvsAlertNoteIcon,    /*!< Note/information icon */
	kCvsAlertWarningIcon, /*!< Warning/exclamation icon */
	kCvsAlertQuestionIcon /*!< Question icon */
}kCvsAlertIconType;

/// Button IDs enum
typedef enum {
	kCvsAlertOKButton,      /*!< OK button ID */
	kCvsAlertCancelButton,  /*!< Cancel button ID */
	kCvsAlertOtherButton    /*!< ID of optional 3rd button */
}kCvsAlertButtonType;

/// CVS Alert class
class CvsAlert
{
public:
	// Construction
	CvsAlert(const kCvsAlertIconType icon, 
		const char* message, const char* messageDetails = NULL,
		const char* okButtonTitle = BUTTONTITLE_OK, const char* cancelButtonTitle = BUTTONTITLE_CANCEL);
	virtual ~CvsAlert() {}

protected:
	// Data members
	kCvsAlertIconType m_icon;			/*!< Icon used to indicate the nature of the alert */
	
	std::string m_message;				/*!< Main alert message */
	std::string m_messageDetails;		/*!< Details of the alert message */
	
	std::string m_alertTitle;			/*!< Alert title */

	std::string m_okButtonTitle;		/*!< OK button title */
	std::string m_cancelButtonTitle;	/*!< Cancel button title */
	std::string m_otherButtonTitle;		/*!< Third("other") button title */

	CPersistentBool* m_pHideOption;		/*!< Hide option */

public:
	// Interface
	virtual kCvsAlertButtonType ShowAlert(const kCvsAlertButtonType defaultButton = kCvsAlertOKButton);

	void SetAlertTitle(const char* alertTitle);
	void SetButtonTitle(const kCvsAlertButtonType buttonType, const char* buttonTitle);
	void SetHideOption(CPersistentBool* pHideOption);

	kCvsAlertIconType GetIcon() const;
	std::string GetMessage() const;
	std::string GetMessageDetails() const;
	std::string GetAlertTitle() const;
	std::string GetButtonTitle(const kCvsAlertButtonType buttonType) const;
	CPersistentBool* GetHideOption() const;
};

/// CVS Prompt class
class CvsPrompt : public CvsAlert
{
public:
	// Construction
	CvsPrompt(const kCvsAlertIconType icon, 
		const char* message, const char* messageDetails = NULL,
		const char* okButtonTitle = BUTTONTITLE_OK, const char* cancelButtonTitle = BUTTONTITLE_CANCEL);
	virtual ~CvsPrompt() {}

protected:
	// Data members
	std::string	m_value;	/*!< Entered value */

public:
	// Interface
	void SetValue(const char* value);
	std::string GetValue() const;
};

/////////////////////////////////////////////////////////////////////////////
// CCVSAlert_MAIN dialog

#ifdef WIN32
class CCVSAlert_MAIN : public CDialog
{
// Construction
public:
	CCVSAlert_MAIN(const CvsAlert* pcvsAlert, const kCvsAlertButtonType defaultButton, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCVSAlert_MAIN)
	enum { IDD = IDD_ALERT };
	CEdit	m_promptEdit;
	CButton	m_optionCheck;
	CEdit	m_alertMessageEdit;
	CEdit	m_messageDetailsEdit;
	CButton	m_okButton;
	CButton	m_cancelButton;
	CButton	m_otherButton;
	CStatic	m_iconStatic;
	CString	m_messageDetails;
	CString	m_alertMessage;
	BOOL	m_bOptionChecked;
	CString	m_prompt;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCVSAlert_MAIN)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCVSAlert_MAIN)
	virtual BOOL OnInitDialog();
	afx_msg void OnOtherButton();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnOptioncheckButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	const CvsAlert* m_pcvsAlert;			/*!< Alert data */
	kCvsAlertButtonType m_defaultButton;	/*!< Default button */
	
	CFont m_alertMessageFont;	/*!< Alert message font */

	// Methods
	void SetIcon();
	void SetTitle();
	void SetMessages();
	void Resize();
	void SetButtonTitles();
	void SetAccelerators();
	void SetDefaultButton();
	void SetHideOptionCheck();
	void SetPrompt();

	static void SetButtonAccelerator(CButton& button);
	static CSize GetWndTextExtent(CWnd* wnd, const char* text);
	static CSize CalcWndTextSize(CWnd* wnd, const char* text, const int width);
};
#endif /* WIN32 */

#ifdef qUnix
class CCVSAlert_MAIN : public UWidget
{
	UDECLARE_DYNAMIC(CCVSAlert_MAIN)
	
// Construction
public:
	CCVSAlert_MAIN(const CvsAlert* pcvsAlert, const kCvsAlertButtonType defaultButton);
	virtual ~CCVSAlert_MAIN() {}
		
// Dialog Data
	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kOtherBtn,			// 2
		kIcon,					// 3
		kMessage,				// 4
		kMessageDetails	// 5
	};
	
	std::string m_messageDetails;
	std::string m_alertMessage;

// Overrides
	protected:
	virtual void DoDataExchange(bool fill);
	
// Implementation
protected:
	virtual bool OnInitDialog();
	ev_msg void OnOtherButton();
	virtual void OnOK();
	virtual void OnCancel();

	UDECLARE_MESSAGE_MAP()
private:
	// Data members
	const CvsAlert* m_pcvsAlert;			/*!< Alert data */
	kCvsAlertButtonType m_defaultButton;	/*!< Default button */
	
	// Methods
	void SetIcon();
	void SetTitle();
	void SetMessages();
	void Resize();
	void SetButtonTitles();
	void SetAccelerators();
	void SetDefaultButton();

	void SetDefID(unsigned int defID);
};
#endif /* qUnix */

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVSALERT_H__ACFE8C41_9836_11D1_AE50_00C0F0067852__INCLUDED_)
