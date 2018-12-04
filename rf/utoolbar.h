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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- May 1999
 */

#ifndef UTOOLBAR_H
#define UTOOLBAR_H

#include "uwidget.h"

#ifdef __cplusplus

class WININFO;

#ifdef __AFX_H__
class CDynToolBar : public CCJToolBar
{
	DECLARE_DYNAMIC(CDynToolBar)

// Construction
public:
	CDynToolBar();

// Attributes
public:
protected:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDynToolBar();

	HBITMAP m_bitmap;
	bool m_tracking;

	inline int GetPseudoID(void) const { return m_widid; }
	inline void SetPseudoID(int id) { m_widid = id; }
	inline WININFO *GetWinInfo(void) const { return m_info; }
	inline void SetWinInfo(WININFO *info) { m_info = info; }
	inline CFrameWnd *GetMainFrame() { return GetRoutingFrame(); }

// Generated message map functions
protected:
	WININFO *m_info;
	int m_widid;

	virtual void OnDropDownButton(const NMTOOLBAR& nmtb, UINT nID, CRect rc);

	//{{AFX_MSG(CDynToolBar)
	afx_msg void OnDestroy(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#else /* ! __AFX_H__ */
class CDynToolBar;
#endif /* !__AFX_H__ */

class UToolbar : public UWidget
{
	UDECLARE_DYNAMIC(UToolbar)
public:
	UToolbar(int widid);
	virtual ~UToolbar();

	virtual int UDispatchMessage(int/*UEventType*/ evt, int arg1, void *arg2, UDispatchInfo & dispatchInfo);

protected:
	ev_msg void OnDestroy();
	ev_msg void OnTbInit(UTB_INIT *info);
	ev_msg void OnTbEnd(void);
	ev_msg void OnTbButton(UTB_BUTTON *info);
	ev_msg void OnTbDropDown(UTB_DROPDOWN *info);
	ev_msg void OnTbCheckBtn(UTB_BUTTON *info);
	ev_msg void OnTbSeparator(void);
	ev_msg void OnSetTooltip(int cmd, UCmdUI *pCmdUI);
	ev_msg int OnCommands(int cmd);
	ev_msg void OnUpdateUI(void);

	WININFO *m_winInfo;

#ifdef WIN32
	CDynToolBar *m_wndToolBar;

	void RebuildBar(bool onlyBitmap = true);
#endif

	UDECLARE_MESSAGE_MAP()
};
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* UTOOLBAR_H */
