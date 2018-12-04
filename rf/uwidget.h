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

#ifndef UWIDGET_H
#define UWIDGET_H

#include "umain.h"
#include "udraw.h"
#include "ustr.h"

#ifdef qGTK
#	include <gdk/gdkkeysyms.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
	LRESULT UWinPseudoWindowProc(CWnd *wnd, UWidget *wid, UINT message, WPARAM wParam, LPARAM lParam);
		// - intercepts the messages and broadcast to the pseudo-widgets associated with.
		// - if it returns FALSE, you should return FALSE too.
		// - you need to overide the CWnd::WindowProc in order to use it.
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#ifdef _MSC_VER
#	pragma warning (disable : 4786)
#	if _MSC_VER >= 1400
#		pragma warning(disable : 4996)
#	endif
#endif

#include <vector>
#include <map>

class CMString;

/*!
 * \class UTimer uwidget.h
 * \brief used by the UWidget class to implement timers/repeaters.
 */
class UTimer
{
public:
	UTimer(int lap, int lastlap, int cmd) : m_lap(lap), m_lastlap(lastlap), m_cmd(cmd) {}
	UTimer() : m_lap(0), m_lastlap(0), m_cmd(0) {}
	UTimer(const UTimer & src) : m_lap(src.m_lap), m_lastlap(src.m_lastlap), m_cmd(src.m_cmd) {}

	UTimer & operator=(const UTimer & src)
	{
		m_lastlap = src.m_lastlap;
		m_lap = src.m_lap;
		m_cmd = src.m_cmd;

		return *this;
	}
	long m_lap; // in millisecond
	long m_lastlap; // in millisecond
	int m_cmd; // command associated with
};

/*!
 * \struct UColor uwidget.h
 * \brief portable color definition
 */

struct UColor
{
	unsigned char r, g, b;
};

/// kUMainWidget the command associated with the main widget.
#define kUMainWidget 0

#if qGTK
#	define kUgtkDataToken "_pseudoWidget"
#	define kUgtkPrefixToken "pseudo" /* pseudo_0, pseudoA_0, pseudo<opt>_<cmdid> */
#	define SW_SHOWNORMAL true
#	define SW_HIDE false
#endif
#ifdef WIN32
	class CDialog;
#endif

/*!
 * \class UWidget uwidget.h
 * \brief This is the pseudo widget class.
 *
 * The pseudo widget class allows to access mapped widgets
 * (using the event EV_INIT_WIDGET) and to manipulate them
 * (i.e. set the caption, visible state...).
 * By derivating this class, you can also take advantage of
 * features like modaling dialogs, timers...
 */

class UEXPORT UWidget : public UCmdTarget
{
	UDECLARE_DYNAMIC(UWidget)
public:
	//! \arg \c widid unique pseudo widget identifier
	UWidget(int widid);
	virtual ~UWidget();

	//! \arg \c listener has the widget events broadcasted to it.
	//! \note listener gets destroyed by the widget when it is destroyed.
	virtual void AddListener(UCmdTarget * listener);

	//! this is overided in order to broadcast to the listeners
	virtual int UDispatchMessage(int/*UEventType*/ evt, int arg1, void *arg2, UDispatchInfo & dispatchInfo);

	//! return a sub-widget as inserted by EV_INIT_WIDGET
	//! \arg \c cmd the sub-widget identifier.
	//! \return a native pointer for the sub-widget.
	//! \note the native pointer type depends on the platform implementation.
	//! \note for example on Windows it's a derivated CWnd *
	void *GetWidget(int cmd) const;

	//! return all the sub-widgets inserted by EV_INIT_WIDGET
	//! \return a STL map with a pair of id and native pointer.
	inline std::map<int, void *> & GetWidgets(void) { return m_widgets; }

	//! return true if a widget has the focus ...
	//! \arg \c cmd the sub-widget identifier.
	//! \warning only on Win32
	bool WidgetIsActive(int cmd) const;

	//! return the tooltip (i.e. status/nprompt) or 0L if no tooltip
	//! associated with this command
	//! \arg \c cmd the sub-widget identifier.
	const char *GetTooltip(int cmd) const;

	//! associate a tooltip with a sub-widget
	//! \arg \c cmd the sub-widget identifier.
	//! \arg \c tooltipID a string.
	void AddTooltip(int cmd, const char* tooltipID);

	//! timer handling - add a timer.
	//! \arg \c lap in milliseconds.
	//! \arg \c cmd to fire to the pseudo-widget each time the lap expires.
	// \note this timer is removed automatically when the object is removed.
	void AddTimer(long lap, int cmd);

	//! timer handling - remove a timer.
	//! \arg \c cmd associated with the timer.
	void RemoveTimer(int cmd);

	//! timer handling - get the timers. Used internally by the event mamanger.
	std::vector<UTimer> & GetTimers(void);

	//! modality : works only on the top widget and if it is a window
	//! \arg \c id associated dialog id.
	//! \return true -> OK, false -> Cancel.
	//! \note the 'id' is significant by platform (on Mac, that is the id view)
	int DoModal(int id = 0);

	//! while modaling, allows to quit the interaction
	//! \arg \c result set the return value for DoModal.
	//! obsolete
	void EndModal(bool result);

	//! while modaling, allows to quit the interaction
	//! \arg \c result set the return value for DoModal.
	void EndDialog(int result);

	//! while modaling, allows to fill/read the dialog content.
	//! \arg \c fill says if it expects to fill or read the dialog content.
	virtual void DoDataExchange(bool /*fill*/) {}

	//! while modaling, allows to detect when the user cancels the dialog.
	virtual void Dismiss(void) {}

	//! tells if any other widget is currently modaling.
	static bool HasModal(void);

	//! update user interface handling : check validation
	//! says if at this moment the UI is valid (i.e. menus, widgets
	//! states are correct)
	static bool IsUIValid(); /*{ return sValidMenu; }*/

	//! update user interface handling : invalidate/validate UI
	//! \arg \c how determines if the UI will be valid.
	//! \note if the UI is not valid, the event manager will fire
	//! a EV_UPDATEUI event next time it spends time.
	//! \warning you generally don't have to use this function since
	//! \warning the pseudo-widgets detect the UI validity automatically
	static void DoValidUI(bool how); /*{ sValidMenu = how; } */

	//! update user interface handling : before the EV_UPDATEUI get
	//! fired, give a chance to all the widgets
	//! to cache some computations used for the handling of the event.
	virtual void BeforeUpdateUI(void) {}
	
	//! returns the current pseudo-widget which has the focus
	//! \note it is important cause it is used by the event manager
	//! \note in order to decide which routing for the events
	static int GetFocus(void); /*{ return sFocusWidID; } */
	static int GetFocusCmd(void); /*{ return sFocusCmdID; }*/

	//! tells if the pseudo-widget has the focus
	inline bool HasFocus(void) { return GetFocus() == GetWidID(); }

	//! tells if the pseudo-widget and the sub-widget have the focus
	inline bool HasFocus(int cmdid) { return GetFocus() == GetWidID() && GetFocusCmd() == cmdid; }

	//! blocks a eventhandler
	void OnBlockHandler(void* widget, int cmd, int event);
		
	//! unblocks a eventhandler
	void OnUnblockHandler(void* widget, int cmd, int event);
		
	//! map the widgets inside a pseudo tab-page and the pseudo tab-page
	//! to the pseudo-widget.
	//! \warning not implemented yet on \b Win32
	void AddPage(UWidget *page, int pagecmd, int num_page);

#ifdef WIN32
	void OverideMapWidget(int cmdid, CRuntimeClass *widClass);
	inline std::map<int, CRuntimeClass *> & GetOverideList(void) { return m_overide; }
		// use a class instead of the one by default for this item.
		// to call before DoModal, before the widgets get created.

	inline CDialog *GetCurModal() const { return m_curModal; }
		// during DoModal, this value is set to the MFC dialog
#endif
#if qMacPP || qMacAPP
	//! translates to the ID used by PP constructor or MacAPP AdLib
	static SInt32 GetPaneID(int cmdid);
#endif
	
	static void ResetKeyState();
		// reset last pressed key marker
	
	static bool IsEscKeyPressed();
		// was ESC the last pressed key
	
	static short GetAsyncKeyState(unsigned int keyFlag);
		// is 'keyFlag' pressed
	
protected:

	std::vector<UCmdTarget*> m_listeners;
	std::map<int, void*> m_widgets;
	std::map<int, std::string> m_tooltips;
	std::vector<UTimer> m_timers;
	std::map<int, UWidget*> m_tabs;

	int m_modalRes;
	int m_currentPage;
#if qGTK || qMacPP
	bool m_modalLoop;
#endif
#ifdef WIN32
	CDialog *m_curModal;
	std::map<int, CRuntimeClass *> m_overide; // see WinInitWidgets
	bool m_noDispatch;
#endif
	static bool sValidMenu;

	static int sFocusWidID;
	static int sFocusCmdID;

#if qGTK
	static guint lastKey;
#endif

	//! EV_QUERYSTATE message handling
	//! \arg \c cmd the sub-widget
	//! \return 0 or 1 if the widget is checked/pushed...
 	ev_msg int OnGetState(int cmd);
	
  //! EV_SETSTATE message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c state of the widget to set (pushed/checked...)
 	ev_msg void OnSetState(int cmd, int state);
	
  //! EV_ENABLECMD message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c state of the widget to enable/disable (grayed)
 	ev_msg void OnEnableCmd(int cmd, int state);
	
  //! EV_IS_ENABLED message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c state of the widget enable/disable (grayed)
 	ev_msg void OnIsEnabled(int cmd);
	
  //! EV_IS_ENABLED UWidget message handling
	ev_msg bool IsEnabled(int cmd);
	
  //! EV_CHGFOCUS UWidget message handling
	//! \arg \c cmd the sub-widget to change focus to
 	ev_msg void OnChgFocus(int cmd);
	
  //! EV_SETDEFAULT UWidget message handling
	//! \arg \c cmd the sub-widget to change default to
 	ev_msg void OnSetDefault(int cmd);
	
  //! EV_INVALIDATE UWidget message handling
	//! \arg \c cmd the sub-widget to redraw (delayed)
 	ev_msg void OnInvalidate(int cmd);
	
  //! EV_REDRAW UWidget message handling
	//! \arg \c cmd the sub-widget to redraw *now*
 	ev_msg void OnRedraw(int cmd);
	
  //! EV_SETICON message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c pixmap the pixmap to set
 	ev_msg void OnSetIcon(int cmd, void *pixmap);
	
  //! EV_SETTEXT message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c text the c-string caption to set
 	ev_msg void OnSetText(int cmd, const char *text);
	
  //! EV_GETTEXT message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c text is filled with the widget caption
	ev_msg void OnGetText(int cmd, std::string* text);
	
  //! EV_SETFLOAT UWidget message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c f the float value the to set
 	ev_msg void OnSetFloat(int cmd, const float *f);
	
  //! EV_GETFLOAT UWidget message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c f the float value the to get
 	ev_msg void OnGetFloat(int cmd, float *f);
	
  //! EV_INIT_WIDGET UWidget message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c widget the native pointer/handle for the sub-widget
	ev_msg void OnInitWidget(int cmd, void *widget);
	
  //! EV_INIT_TAB_WIDGET UWidget message handling
	//! \arg \c cmd the sub-widget page holder
	//! \arg \c page the location for the page insertion
	//! \arg \c widget the native pointer/handle for the sub-widget page
	//! \warning not implemented yet on \b Win32
	ev_msg void OnInitTabWidget(int cmd, int page, void *widget);
	
  //! EV_SHOW message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c state of the widget to show/hide
 	ev_msg void OnShowWidget(int cmd, int state);
	
  //! EV_SET_SLIDER UWidget message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c pos is the position to set
	ev_msg void OnSetSlider(int cmd, int pos);
	
  //! EV_GET_SLIDERPROP UWidget message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c prop get the informations about this scroller/slider
	ev_msg void OnGetSliderProperty(int cmd, USLIDER *prop);
	
  //! EV_SET_SLIDERPROP UWidget message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c prop set the informations about this scroller/slider
	ev_msg void OnSetSliderProperty(int cmd, const USLIDER *prop);
	
  //! EV_USETTOOLTIP message handling
	//! \warning not documented
	ev_msg void OnSetTooltip(int cmd, UCmdUI *pCmdUI);
	
  //! EV_UGETLLOC message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c r get the position of the widget relative to its parent
	ev_msg void OnGetLoc(int cmd, URECT * r);
	
  //! EV_USETLLOC message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c r set the position of the widget relative to its parent
	ev_msg void OnSetLoc(int cmd, const URECT * r);
	
  //! EV_CHGCURSOR UWidget message handling
	//! \arg \c cmd the sub-widget
	//! \arg \c cursor to use in this sub-widget
	//! \warning \b GTK only and limited implementation
	ev_msg void OnChgCursor(int cmd, UCURSOR cursor);
	
  //! EV_UPDATEUI is a notification sent to all the widgets
	//! when the user interface is invalid.
	//! the default is to send a EV_UPDTCMD to each
	//! sub-widget.
	ev_msg void OnUpdateUI(void);
	
  //! EV_TREE_RESETALL UWidget message handling
	ev_msg void OnTreeResetAll(int cmd);
	
  //! EV_TREE_INSERT UWidget message handling
	ev_msg void OnTreeInsert(int cmd, UTREE_INSERT *data);
	
  //! EV_TREE_GETINFO UWidget message handling
	ev_msg void OnTreeGetInfo(int cmd, UTREE_INFO *data);
	
  //! EV_TREE_SETDATA UWidget message handling
	ev_msg void OnTreeSetData(int cmd, UTREE_INFO *data);
	
  //! EV_TREE_SETICON UWidget message handling
	ev_msg void OnTreeSetIcon(int cmd, UTREE_INFO *data);
	
  //! EV_TREE_DELETE UWidget message handling
	ev_msg void OnTreeDelete(int cmd, UTREEITEM item);
	
  //! EV_TREE_EXPAND UWidget message handling
	ev_msg void OnTreeExpand(int cmd, int collapse, UTREEITEM item);
	
  //! EV_TREE_SELECT UWidget message handling
	ev_msg void OnTreeSelect(int cmd, int deselect, UTREEITEM item);
	
  //! EV_TREE_GETSEL UWidget message handling
	ev_msg void OnTreeGetSel(int cmd, UTREEITEM *item);
	
  //! EV_LIST_ADDCOLUMNS UWidget message handling
	ev_msg void OnListAddColumns(int cmd, int num);
	
  //! EV_LIST_SHOWCOLUMNS UWidget message handling
	ev_msg void OnListShowColumns(int cmd, int num);
	
  //! EV_LIST_DELCOLUMNS UWidget message handling
	ev_msg void OnListHideColumns(int cmd, int num);
	
  //! EV_LIST_SETCOLTITLE UWidget message handling
	ev_msg void OnListSetColTitle(int cmd, int num, const char *title);
	
  //! EV_LIST_SETCOLWIDTH UWidget message handling
	ev_msg void OnListSetColWidth(int cmd, int num, int width);
	
  //! EV_LIST_RESETALL UWidget message handling
	ev_msg void OnListResetAll(int cmd);
	
  //! EV_LIST_ADDSEL UWidget message handling
	ev_msg void OnListAddSel(int cmd, int entry);

  //! EV_LIST_POPUP UWidget message handling
	ev_msg void OnListPopup(int cmd, int entry);

  //! EV_LIST_DELROW UWidget message handling
	ev_msg void OnListDelRow(int cmd, int row);
	
  //! EV_LIST_SETFEEDBACK UWidget message handling
	ev_msg void OnListSetFeedback(int cmd, int turnOn);
	
  //! EV_LIST_NEWROW UWidget message handling
	ev_msg void OnListNewRow(int cmd, int *num);
	
  //! EV_LIST_ROWSETDATA UWidget message handling
	ev_msg void OnListRowSetData(int cmd, int row, void *data);
	
  //! EV_LIST_ROWGETDATA UWidget message handling
	ev_msg void OnListRowGetData(int cmd, int row, void **data);
	
  //! EV_LIST_INSERT UWidget message handling
	ev_msg void OnListInsert(int cmd, ULIST_INSERT *data);
	
  //! EV_LIST_GETINFO UWidget message handling
	ev_msg void OnListGetInfo(int cmd, ULIST_INFO *desc);

  //! EV_LIST_GETNEXTSEL UWidget message handling
	ev_msg int OnListGetNextSel(int cmd, int selectedCell);
	
  //! EV_LIST_GETNEXT UWidget message handling
	ev_msg int OnListGetNext(int cmd, int whichCell);
	
  //! EV_LIST_RESORT UWidget message handling
	ev_msg void OnListResort(int cmd, int column, UListSorter s);

  //! ON_LIST_SETSORTDIR UWidget message handling
	ev_msg void OnListSetSortDir(int cmd, int column, bool dir);
	
  //! EV_COMBO_RESETALL UWidget message handling
	ev_msg void OnComboResetAll(int cmd);
	
  //! EV_COMBO_APPEND UWidget message handling
	ev_msg void OnComboAppend(int cmd, const char *txt);
	
  //! EV_COMBO_SETSEL UWidget message handling
	ev_msg void OnComboSetPos(int cmd, int sel);
	
  //! EV_COMBO_GETSEL UWidget message handling
	ev_msg int OnComboGetPos(int cmd, std::string* txt);
	
  //! EV_COMBO_GETCOUNT UWidget message handling
	ev_msg int OnComboGetCount(int cmd);
	
  //! EV_COMBO_DELSEL UWidget message handling
	ev_msg void OnComboDelSel(int cmd, int sel);
	
  //! EV_COMBO_ENABLESEL UWidget message handling
	//! \warning not implemented on \b Win32
	ev_msg void OnComboEnableSel(int cmd, int sel, int state);
	
  //! EV_COMBO_ENABLESEL UWidget message handling
	ev_msg void OnComboGetMenu(int cmd, UMENU *menu);
	
  //! EV_SMARTCOMBO_SETFEATURE UWidget message handling
	ev_msg void OnSmartComboSetFeature(int cmd, int feature);
	
  //! EV_SMARTCOMBO_SETITEMS UWidget message handling
	ev_msg void OnSmartComboSetItems(int cmd, CMString* items);
	
  //! EV_SMARTCOMBO_SETREADONLY UWidget message handling
	ev_msg void OnSmartComboSetReadOnly(int cmd, bool readOnly);
	
  //! EV_UPDATEFOCUS UWidget message handling
	ev_msg int OnUpdateFocus(int leave, void *widget);
	
  //! EV_FETCHCMD UWidget message handling
	ev_msg int OnFetchCmd(int cmd, void *widget);
	
  //! EV_PAGE UWidget message handling
 	ev_msg void OnPageChanged(int cmd, int pagenum);

  //! EV_STATUSBAR_TEXT UWidget message handling
	ev_msg void OnStatusBarText(int cmd, const char *txt);
	
  //! EV_KEYDOWN UWidget message handling
	ev_msg void OnKeyDown(int keyval);
	
 UDECLARE_MESSAGE_MAP()

#if qMacPP
	friend class LPseudoWinAttachment;
#endif
#ifdef WIN32
	friend LRESULT UWinPseudoWindowProc(CWnd *wnd, UWidget *wid, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

#endif /* __cplusplus */

#endif /* UWIDGET_H */
