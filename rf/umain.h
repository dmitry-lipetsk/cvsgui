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

#ifndef UEVENT_H
#define UEVENT_H

#include "uobject.h"

/*
 * The routing of the messages is copied from the MFC scheme :
 * A message has a sort of "prototype" associated with it and
 * is dispatched using a virtual message map : the message goes
 * to the child first, then get up to the parents if it was not handled.
 */

typedef enum
{
	EV_NONE,

	/* system events */
	EV_CREATE,		/* void OnCreate(void) */
	EV_DESTROY,		/* void OnDestroy(void) */
	EV_ENTER,		/* void OnEnter(void *data) */
	EV_LEAVE,		/* void OnLeave(void) */
	EV_EXPOSE,		/* void OnExpose(void) */
	EV_MMOVE,		/* void OnMouseMove(int x, int y, int mod) */
	EV_BTNDOWN,		/* void OnBtnDown(int x, int y, int mod) */
	EV_BTNUP,		/* void OnBtnUp(int x, int y, int mod) */
	EV_BTNDBLDOWN,	/* void OnBtnDblDown(int x, int y, int mod) */
	EV_KEYDOWN,		/* void OnKeyDown(int c) */
	EV_SETCURSOR,	/* void OnSetCursor(void) */
	EV_CHGCURSOR,	/* void OnChgCursor(int cmd, UCURSOR cursor) */
	EV_RESIZE,		/* void OnResize(const URECT * r) */
	EV_GETLOC,		/* void OnGetLoc<Cmd>(int cmd, URECT *) */
	EV_SETLOC,		/* void OnSetLoc<Cmd>(int cmd, const URECT *) */

	/* generic */
	EV_CMD,			/* int On<Cmd>(void) */
	EV_UPDTCMD,		/* void OnUpdate<Cmd>(UCmdUI *pCmdUI) */
	EV_ENABLECMD,	/* void OnEnableCmd<Cmd>(int cmd, int enabled) */
	EV_FETCHCMD,	/* int OnFetchCmd(int cmd, void *widget) */
	EV_SETTOOLTIP,	/* void OnSetTooltip<Cmd>(int cmd, UCmdUI *pCmdUI) */
	EV_QUERYSTATE,	/* int OnQueryState<Cmd>(int cmd) */
	EV_GETINTEGER = EV_QUERYSTATE,
	EV_SETSTATE,	/* void OnSetState<Cmd>(int cmd, int state) */
	EV_SETINTEGER = EV_SETSTATE,
	EV_CHGFOCUS,	/* void OnChgFocus(int cmd) */
	EV_SETDEFAULT,	/* void OnSetDefault(int cmd) */
	EV_INVALIDATE,	/* void OnInvalidate(int cmd) */
	EV_REDRAW,		/* void OnRedraw(int cmd) */
	EV_GETTEXT,		/* void OnGetText(int cmd, CStr *text); */
	EV_SETTEXT,		/* void OnSetText(int cmd, const char *text); */
	EV_SETICON,		/* void OnSetIcon(int cmd, void *pixmap); */
	EV_GETFLOAT,	/* void OnGetFloat(int cmd, float *f); */
	EV_SETFLOAT,	/* void OnSetFloat(int cmd, const float *f); */
	EV_INIT_WIDGET,	/* void OnInitWidget(int cmd, void *widget); */
	EV_INIT_TAB_WIDGET,	/* void OnInitTabWidget(int cmd, int page, void *widget); */
	EV_SHOW_WIDGET,	/* void OnShowWidget<Cmd>(int cmd, int state) */
	EV_PAGE_CHANGED,	/* void OnPageChanged<Cmd>(int cmd, int pagenum) */
	EV_STATUSBAR_TEXT,	/* void OnStatusBarText(int cmd, const char *text) */


	/* notifying */
	EV_NOTIFY,			/* void OnNotify<Cmd>(void) */
	EV_EDITCHANGE,		/* void OnEditChange<Cmd>(void) */
	EV_EDITACTIVATE,	/* void OnEditActivate<Cmd>(void) */
	EV_TIMER,			/* void OnTimer<Cmd>(long time) */
	EV_UPDATEUI,		/* void OnUpdateUI(void) */
	EV_UPDATEFOCUS,		/* int OnUpdateFocus(int leave, void *widget) */

	/* drag'n drop */
	EV_DRAGQUERY,		/* void OnDragQuery<Cmd>(UDragable *) */
	EV_DROPQUERY,		/* void OnDropQuery<Cmd>(UDropable **) */
	EV_DROPIT,			/* void OnDropIt<Cmd>(const UDragable *) */
	EV_DROPQUERY_CMDID,	/* void OnDropQueryCmdID<Cmd>(int x, int y, int *cmdid) */
	EV_DROPREF,		/* void OnDropRef<Cmd>(int cmd, int nbref, long *ref) */

	/* toolbar manager */
	EV_TB_INIT,			/* void OnTbInit(UTB_INIT *info) */
	EV_TB_END,			/* void OnTbEnd(void) */
	EV_TB_BUTTON,		/* void OnTbButton(UTB_BUTTON *info) */
	EV_TB_DROPDOWN,		/* void OnTbDropDown(UTB_DROPDOWN *info) */
	EV_TB_CHECKBTN,		/* void OnTbCheckBtn(UTB_BUTTON *info) */
	EV_TB_SEPARATOR,	/* void OnTbSeparator(void) */
	EV_TB_GETCOLOR,		/* void OnTbGetColor(int cmd, float *color) */

	/* slider */
	EV_SET_SLIDER,  	/* void OnSetSlider(int cmd, int pos) */
	EV_GET_SLIDERPROP,	/* void OnGetSliderProperty(int cmd, USLIDER *prop) */
	EV_SET_SLIDERPROP,	/* void OnSetSliderProperty(int cmd, const USLIDER *prop) */
	EV_SCROLLCHANGE,	/* void OnScrollChange<Cmd>(int pos) */

	/* custom drawing */
	EV_CUSTOM_DRAW,  	/* void OnCustomDraw<Cmd>(void *drawInfo) */
	EV_CUSTOM_CLICK, 	/* void OnCustimClick<Cmd>(void *drawInfo) */
	
	/* tree */
	EV_TREE_RESETALL,	/* void OnTreeResetAll(int cmd) */
	EV_TREE_INSERT,		/* void OnTreeInsert(int cmd, UTREE_INSERT *data) */
	EV_TREE_GETINFO,	/* void OnTreeGetInfo(int cmd, UTREE_INFO *data) */
	EV_TREE_SETDATA,	/* void OnTreeSetData(int cmd, UTREE_INFO *data) */
	EV_TREE_SETICON,	/* void OnTreeSetIcon(int cmd, UTREE_INFO *data) */
	EV_TREE_DELETE,		/* void OnTreeDelete(int cmd, UTREEITEM item) */
	EV_TREE_EXPANDING,	/* void OnTreeExpanding<cmd>(int collapse, UTREEITEM item) */
	EV_TREE_SELECTING,	/* void OnTreeSelecting<cmd>(int deselecting, UTREEITEM item) */
	EV_TREE_EXPAND,		/* void OnTreeExpand(int cmd, int collapse, UTREEITEM item) */
	EV_TREE_SELECT,		/* void OnTreeSelect(int cmd, int deselect, UTREEITEM item) */
	EV_TREE_GETSEL,		/* void OnTreeGetSel(int cmd, UTREEITEM *item) */

	/* list */
	EV_LIST_POPUP,			/* void OnListChange<Cmd>(int entry, const char *txt) */
	EV_LIST_SELECTING,		/* void OnListChange<Cmd>(int entry, const char *txt) */
	EV_LIST_SELCOLUMN,		/* void OnListSelColumn<Cmd>(int entry) */
	EV_LIST_RESORT,			/* void OnListResort(int cmd, int column, UListSorter s) */
	EV_LIST_SETSORTDIR,		/* void OnListSetSortDir(int cmd, int column, bool dir) */
	EV_LIST_DBLCLICK,		/* void OnListDblClick<Cmd>(void) */
	EV_LIST_RESETALL,		/* void OnListResetAll(int cmd) */
	EV_LIST_SETFEEDBACK,	/* void OnListSetFeedback(int cmd, int turnOn) */
	EV_LIST_ADDCOLUMNS,		/* void OnListAddColumns(int cmd, int num) */
	EV_LIST_SHOWCOLUMNS,	/* void OnListShowColumns(int cmd, int num) */
	EV_LIST_HIDECOLUMNS,	/* void OnListHideColumns(int cmd, int num) */
	EV_LIST_SETCOLTITLE,	/* void OnListSetColTitle(int cmd, int num, const char *title) */
	EV_LIST_SETCOLWIDTH,	/* void OnListSetColWidth(int cmd, int num, int width) */
	EV_LIST_NEWROW,			/* void OnListNewRow(int cmd, int *num) */
	EV_LIST_ROWSETDATA,		/* void OnListRowSetData(int cmd, int row, void *data) */
	EV_LIST_ROWGETDATA,		/* void OnListRowGetData(int cmd, int row, void **data) */
	EV_LIST_INSERT,			/* void OnListInsert(int cmd, ULIST_INSERT *data) */
	EV_LIST_ADDSEL,			/* void OnListAddSel(int cmd, int entry) */
	EV_LIST_DELROW,			/* void OnListDelRow(int cmd, int row) */
	EV_LIST_GETNEXTSEL,		/* int OnListGetNextSel(int cmd, int selectedCell) */
	EV_LIST_GETINFO,		/* void OnListGetInfo(int cmd, ULIST_INFO *data) */
	EV_LIST_GETNEXT,		/* int OnListGetNext(int cmd, int whichCell) */

	/* combo */
	EV_COMBO_RESETALL,	/* void OnComboResetAll(int cmd) */
	EV_COMBO_APPEND,	/* void OnComboAppend(int cmd, const char *str) */
	EV_COMBO_SETSEL,	/* void OnComboSetPos(int cmd, int pos) */
	EV_COMBO_GETSEL,	/* int OnComboGetPos(int cmd, CStr *txt) */
	EV_COMBO_SEL,		/* void OnComboSel<Cmd>(int pos, const char *txt) */
	EV_COMBO_GETCOUNT,	/* int OnComboGetCount(int cmd) */
	EV_COMBO_DELSEL,	/* void OnComboDelSel(int cmd, int pos) */
	EV_COMBO_ENABLESEL,	/* void OnComboEnableSel(int cmd, int pos, int state) */
	EV_COMBO_GETMENU,	/* void OnComboGetMenu(int cmd, UMENU *menu) */
	
	/* smart combo (Win32 only at the moment)*/
	EV_SMARTCOMBO_SETFEATURE,	/* void OnSmartComboSetFeature(int cmd, int feature) */
	EV_SMARTCOMBO_SETITEMS	,	/* void OnSmartComboSetItems(int cmd, CMString* items) */
	EV_SMARTCOMBO_SETREADONLY,	/* void OnSmartComboSetReadOnly(int cmd, bool readOnly) */

	/* user defined */
	EV_COMMAND_START = 1000
} UEventType;

#ifdef __cplusplus

#ifndef STD
#	define STD std::
#endif

class UCmdTarget;
class UWidget;
class UDragable;
class UDropable;

typedef void (UCmdTarget::*UPMSG)(void);
//{ #define ON_*
#define ON_UCREATE(baseClass) \
	{ EV_CREATE, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnCreate },

#define ON_UDESTROY(baseClass) \
	{ EV_DESTROY, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnDestroy },

#define ON_UENTER(baseClass) \
	{ EV_ENTER, epV_VP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void *))&baseClass::OnEnter },

#define ON_ULEAVE(baseClass) \
	{ EV_LEAVE, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnLeave },

#define ON_UEXPOSE(baseClass) \
	{ EV_EXPOSE, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnExpose },

#define ON_UMMOVE(baseClass) \
	{ EV_MMOVE, epV_iii, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(int x, int y, int mod))&baseClass::OnMouseMove },

#define ON_UBTNDOWN(baseClass) \
	{ EV_BTNDOWN, epV_iii, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(int x, int y, int mod))&baseClass::OnBtnDown },

#define ON_UBTNUP(baseClass) \
	{ EV_BTNUP, epV_iii, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(int x, int y, int mod))&baseClass::OnBtnUp },

#define ON_UBTNDBLDOWN(baseClass) \
	{ EV_BTNDBLDOWN, epV_iii, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(int x, int y, int mod))&baseClass::OnBtnDblDown },

#define ON_KEYDOWN(baseClass) \
	{ EV_KEYDOWN, epV_i, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(int c))&baseClass::OnKeyDown },

#define ON_RESIZE(cmd, hook) \
	{ EV_RESIZE, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(const URECT * r))&hook },

#define ON_SETCURSOR(baseClass) \
	{ EV_SETCURSOR, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnSetCursor },

#define ON_CHGCURSOR(baseClass, cmdmin, cmdmax) \
	{ EV_CHGCURSOR, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UCURSOR cursor))&baseClass::OnChgCursor },

#define ON_TB_INIT(baseClass) \
	{ EV_TB_INIT, epV_VP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(UTB_INIT *info))&baseClass::OnTbInit },

#define ON_TB_END(baseClass) \
	{ EV_TB_END, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnTbEnd },

#define ON_TB_BUTTON(baseClass) \
	{ EV_TB_BUTTON, epV_VP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(UTB_BUTTON *info))&baseClass::OnTbButton },

#define ON_TB_DROPDOWN(baseClass) \
	{ EV_TB_DROPDOWN, epV_VP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(UTB_DROPDOWN *info))&baseClass::OnTbDropDown },

#define ON_TB_CHECKBTN(baseClass) \
	{ EV_TB_CHECKBTN, epV_VP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(UTB_BUTTON *info))&baseClass::OnTbCheckBtn },

#define ON_TB_GETCOLOR(baseClass) \
	{ EV_TB_GETCOLOR, epV_iVP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, float *color))&baseClass::OnTbGetColor },

#define ON_TB_SEPARATOR(baseClass) \
	{ EV_TB_SEPARATOR, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnTbSeparator },

#define ON_UCOMMAND(cmd, hook) \
	{ EV_CMD, epi_V, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(int (UCmdTarget::*)(void))&hook },

#define ON_UCOMMAND_RANGE(cmdmin, cmdmax, hook) \
	{ EV_CMD, epi_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd))&hook },

#define ON_UCOMMAND_RANGEEX(cmdmin, cmdmax, hook) \
	{ EV_CMD, epi_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd, void *data))&hook },

#define ON_UUPDATECMD(cmd, hook) \
	{ EV_UPDTCMD, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(UCmdUI *pCmdUI))&hook },

#define ON_FETCHCMD(baseClass, cmdmin, cmdmax) \
	{ EV_FETCHCMD, epi_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd, void *widget))&baseClass::OnFetchCmd },

#define ON_UUPDATECMD_RANGE(cmdmin, cmdmax, hook) \
	{ EV_UPDTCMD, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UCmdUI *pCmdUI))&hook },

#define ON_USETTOOLTIP(cmdmin, cmdmax, hook) \
	{ EV_SETTOOLTIP, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UCmdUI *pCmdUI))&hook },

#define ON_UGETLLOC(cmdmin, cmdmax, hook) \
	{ EV_GETLOC, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, URECT *r))&hook },

#define ON_USETLLOC(cmdmin, cmdmax, hook) \
	{ EV_SETLOC, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, const URECT *r))&hook },

#define ON_QUERYSTATE(cmdmin, cmdmax, hook) \
	{ EV_QUERYSTATE, epi_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd))&hook },

#define ON_EDITCHANGE(cmd, hook) \
	{ EV_EDITCHANGE, epV_V, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(void))&hook },

#define ON_EDITCHANGE_RANGE(cmdmin, cmdmax, hook) \
	{ EV_EDITCHANGE, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&hook },

#define ON_EDITACTIVATE(cmd, hook) \
	{ EV_EDITACTIVATE, epV_V, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(void))&hook },

#define ON_EDITACTIVATE_RANGE(cmdmin, cmdmax, hook) \
	{ EV_EDITACTIVATE, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&hook },

#define ON_SCROLLCHANGE(cmd, hook) \
	{ EV_SCROLLCHANGE, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int pos))&hook },

#define ON_SCROLLCHANGE_RANGE(cmdmin, cmdmax, hook) \
	{ EV_SCROLLCHANGE, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int pos))&hook },

#define ON_TIMER(cmd, hook) \
	{ EV_TIMER, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(long time))&hook },

#define ON_UPDATEUI(baseClass) \
	{ EV_UPDATEUI, epV_V, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(void))&baseClass::OnUpdateUI },

#define ON_UPDATEFOCUS(baseClass) \
	{ EV_UPDATEFOCUS, epi_iVP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(int (UCmdTarget::*)(int leave, void *widget))&baseClass::OnUpdateFocus },

#define ON_SETSTATE(cmdmin, cmdmax, hook) \
	{ EV_SETSTATE, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int state))&hook },

#define ON_ENABLECMD(cmdmin, cmdmax, hook) \
	{ EV_ENABLECMD, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int state))&hook },

#define ON_CHGFOCUS(baseClass, cmdmin, cmdmax) \
	{ EV_CHGFOCUS, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&baseClass::OnChgFocus },

#define ON_SETDEFAULT(baseClass, cmdmin, cmdmax) \
	{ EV_SETDEFAULT, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&baseClass::OnSetDefault },

#define ON_INVALIDATE(baseClass, cmdmin, cmdmax) \
	{ EV_INVALIDATE, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&baseClass::OnInvalidate },

#define ON_REDRAW(baseClass, cmdmin, cmdmax) \
	{ EV_REDRAW, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&baseClass::OnRedraw },

#define ON_SHOW_WIDGET(cmd, hook) \
	{ EV_SHOW_WIDGET, epV_I, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int state))&hook },

#define ON_PAGE_CHANGED(baseClass, cmdmin, cmdmax) \
	{ EV_PAGE_CHANGED, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int pagenum))&baseClass::OnPageChanged },

#define ON_SHOW_WIDGET_RANGE(cmdmin, cmdmax, hook) \
	{ EV_SHOW_WIDGET, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int state))&hook },

#define ON_DROPREF(cmdmin, cmdmax, hook) \
	{ EV_DROPREF, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int numref, long *refs))&hook },

#define ON_UNOTIFY(cmd, hook) \
	{ EV_NOTIFY, epV_V, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(void))&hook },

#define ON_INIT_WIDGET(baseClass, cmdmin, cmdmax) \
	{ EV_INIT_WIDGET, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, void *))&baseClass::OnInitWidget },

#define ON_INIT_TAB_WIDGET(baseClass, cmdmin, cmdmax) \
	{ EV_INIT_TAB_WIDGET, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, int, void *))&baseClass::OnInitTabWidget },

#define ON_SETICON(cmdmin, cmdmax, hook) \
	{ EV_SETICON, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, void *))&hook },

#define ON_SETTEXT(cmdmin, cmdmax, hook) \
	{ EV_SETTEXT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, const char *))&hook },

#define ON_GETTEXT(cmdmin, cmdmax, hook) \
	{ EV_GETTEXT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, std::string*))&hook },

#define ON_SETFLOAT(baseClass, cmdmin, cmdmax) \
	{ EV_SETFLOAT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, const float *f))&baseClass::OnSetFloat },

#define ON_GETFLOAT(baseClass, cmdmin, cmdmax) \
	{ EV_GETFLOAT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, float *f))&baseClass::OnGetFloat },

#define ON_DRAGQUERY(cmd, hook) \
	{ EV_DRAGQUERY, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(UDragable *))&hook },

#define ON_DROPQUERY(cmd, hook) \
	{ EV_DROPQUERY, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(UDropable **))&hook },

#define ON_DROPIT(cmd, hook) \
	{ EV_DROPIT, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(const UDragable *))&hook },

#define ON_DRAGQUERY_RANGE(cmdmin, cmdmax, hook) \
	{ EV_DRAGQUERY, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, UDragable *))&hook },

#define ON_DROPQUERY_RANGE(cmdmin, cmdmax, hook) \
	{ EV_DROPQUERY, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, UDropable **))&hook },

#define ON_DROPIT_RANGE(cmdmin, cmdmax, hook) \
	{ EV_DROPIT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int, const UDragable *))&hook },

#define ON_DROPQUERY_CMDID(baseClass) \
	{ EV_DROPQUERY_CMDID, epV_iiVP, UMSG_NOEXT, 0, 0, \
		(UPMSG)(void (UCmdTarget::*)(int, int, int *))&baseClass::OnDropQueryCmdID },

#define ON_SET_SLIDER(baseClass, cmdmin, cmdmax) \
	{ EV_SET_SLIDER, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int pos))&baseClass::OnSetSlider },

#define ON_GET_SLIDERPROP(baseClass, cmdmin, cmdmax) \
	{ EV_GET_SLIDERPROP, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, USLIDER *prop))&baseClass::OnGetSliderProperty },

#define ON_SET_SLIDERPROP(baseClass, cmdmin, cmdmax) \
	{ EV_SET_SLIDERPROP, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, const USLIDER *prop))&baseClass::OnSetSliderProperty },

#define ON_TREE_RESETALL(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_RESETALL, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&baseClass::OnTreeResetAll },

#define ON_TREE_INSERT(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_INSERT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UTREE_INSERT *data))&baseClass::OnTreeInsert },

#define ON_TREE_GETINFO(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_GETINFO, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UTREE_INFO *data))&baseClass::OnTreeGetInfo },

#define ON_TREE_SETDATA(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_SETDATA, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UTREE_INFO *data))&baseClass::OnTreeSetData },

#define ON_TREE_SETICON(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_SETICON, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UTREE_INFO *data))&baseClass::OnTreeSetIcon },

#define ON_TREE_DELETE(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_DELETE, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UTREEITEM item))&baseClass::OnTreeDelete },

#define ON_TREE_EXPAND(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_EXPAND, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int collapse, UTREEITEM item))&baseClass::OnTreeExpand },

#define ON_TREE_SELECT(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_SELECT, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int deselect, UTREEITEM item))&baseClass::OnTreeSelect },

#define ON_TREE_GETSEL(baseClass, cmdmin, cmdmax) \
	{ EV_TREE_GETSEL, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UTREEITEM *item))&baseClass::OnTreeGetSel },

#define ON_TREE_EXPANDING(cmd, hook) \
	{ EV_TREE_EXPANDING, epV_IVP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int collapse, UTREEITEM item))&hook },

#define ON_TREE_SELECTING(cmd, hook) \
	{ EV_TREE_SELECTING, epV_IVP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int deselect, UTREEITEM item))&hook },

#define ON_LIST_SELECTING(cmd, hook) \
	{ EV_LIST_SELECTING, epV_IVP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int entry, const char *txt))&hook },

#define ON_LIST_SELCOLUMN(cmd, hook) \
	{ EV_LIST_SELCOLUMN, epV_I, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int entry))&hook },

#define ON_LIST_RESORT(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_RESORT, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int column, UListSorter s))&baseClass::OnListResort },

#define ON_LIST_SETSORTDIR(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_RESORT, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int column, bool dir))&baseClass::OnListSetSortDir },

#define ON_LIST_DBLCLICK(cmd, hook) \
	{ EV_LIST_DBLCLICK, epV_V, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int btn))&hook },

#define ON_LIST_POPUP(cmd, hook) \
	{ EV_LIST_POPUP, epV_IVP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(void))&hook },

#define ON_LIST_ADDSEL(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_ADDSEL, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int entry))&baseClass::OnListAddSel },

#define ON_LIST_DELROW(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_DELROW, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int row))&baseClass::OnListDelRow },

#define ON_LIST_RESETALL(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_RESETALL, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&baseClass::OnListResetAll },

#define ON_LIST_ADDCOLUMNS(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_ADDCOLUMNS, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int num))&baseClass::OnListAddColumns },

#define ON_LIST_SHOWCOLUMNS(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_SHOWCOLUMNS, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int num))&baseClass::OnListShowColumns },

#define ON_LIST_HIDECOLUMNS(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_HIDECOLUMNS, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int num))&baseClass::OnListHideColumns },

#define ON_LIST_SETFEEDBACK(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_SETFEEDBACK, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int turnOn))&baseClass::OnListSetFeedback },

#define ON_LIST_SETCOLTITLE(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_SETCOLTITLE, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int num, const char *title))&baseClass::OnListSetColTitle },

#define ON_LIST_SETCOLWIDTH(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_SETCOLWIDTH, epV_iii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int num, int width))&baseClass::OnListSetColWidth },

#define ON_LIST_NEWROW(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_NEWROW, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int *num))&baseClass::OnListNewRow },

#define ON_LIST_ROWSETDATA(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_ROWSETDATA, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int row, void *data))&baseClass::OnListRowSetData },

#define ON_LIST_ROWGETDATA(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_ROWGETDATA, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int row, void **data))&baseClass::OnListRowGetData },

#define ON_LIST_INSERT(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_INSERT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, ULIST_INSERT *data))&baseClass::OnListInsert },

#define ON_LIST_GETNEXTSEL(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_GETNEXTSEL, epi_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd, int selectedCell))&baseClass::OnListGetNextSel },

#define ON_LIST_GETNEXT(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_GETNEXT, epi_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd, int whichCell))&baseClass::OnListGetNext },

#define ON_LIST_GETINFO(baseClass, cmdmin, cmdmax) \
	{ EV_LIST_GETINFO, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, ULIST_INFO *data))&baseClass::OnListGetInfo },

#define ON_COMBO_RESETALL(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_RESETALL, epV_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd))&baseClass::OnComboResetAll },

#define ON_COMBO_APPEND(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_APPEND, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, const char *txt))&baseClass::OnComboAppend },

#define ON_COMBO_SETSEL(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_SETSEL, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int sel))&baseClass::OnComboSetPos },

#define ON_COMBO_DELSEL(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_DELSEL, epV_ii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int sel))&baseClass::OnComboDelSel },

#define ON_COMBO_ENABLESEL(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_ENABLESEL, epV_iii, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int sel, int state))&baseClass::OnComboEnableSel },

#define ON_COMBO_GETSEL(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_GETSEL, epi_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd, std::string* txt))&baseClass::OnComboGetPos },

#define ON_COMBO_GETMENU(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_GETMENU, epi_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, UMENU *menu))&baseClass::OnComboGetMenu },

#define ON_COMBO_GETCOUNT(baseClass, cmdmin, cmdmax) \
	{ EV_COMBO_GETCOUNT, epi_i, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(int (UCmdTarget::*)(int cmd))&baseClass::OnComboGetCount },

#define ON_COMBO_SEL(cmd, hook) \
	{ EV_COMBO_SEL, epV_IVP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(int pos, const char *txt))&hook },

#define ON_COMBO_SEL_RANGE(cmdmin, cmdmax, hook) \
	{ EV_COMBO_SEL, epV_iiVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int pos, const char *txt))&hook },

#define ON_SMARTCOMBO_SETFEATURE(baseClass, cmdmin, cmdmax) \
	{ EV_SMARTCOMBO_SETFEATURE, epi_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, int feature))&baseClass::OnSmartComboSetFeature },

#define ON_SMARTCOMBO_SETITEMS(baseClass, cmdmin, cmdmax) \
	{ EV_SMARTCOMBO_SETITEMS, epi_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, CMString* items))&baseClass::OnSmartComboSetItems },

#define ON_SMARTCOMBO_SETREADONLY(baseClass, cmdmin, cmdmax) \
	{ EV_SMARTCOMBO_SETREADONLY, epi_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, bool readOnly))&baseClass::OnSmartComboSetReadOnly },

#define ON_CUSTOM_CLICK(cmd, hook) \
	{ EV_CUSTOM_CLICK, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(void *drawInfo))&hook },
#define ON_CUSTOM_DRAW(cmd, hook) \
	{ EV_CUSTOM_DRAW, epV_VP, UMSG_RANGE, cmd, cmd, \
		(UPMSG)(void (UCmdTarget::*)(void *drawInfo))&hook },

#define ON_CUSTOM_DRAW_RANGE(cmdmin, cmdmax, hook) \
	{ EV_CUSTOM_DRAW, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, void *drawInfo))&hook },
		
#define ON_STATUSBAR_TEXT(baseClass, cmdmin, cmdmax) \
	{ EV_STATUSBAR_TEXT, epV_iVP, UMSG_RANGE, cmdmin, cmdmax, \
		(UPMSG)(void (UCmdTarget::*)(int cmd, const char *txt))&baseClass::OnStatusBarText },
//}

typedef enum
{
	epNone,
	epV_V,		// void (void)
	epV_VP,		// void (arg2)
	epV_iVP,	// void (arg1, arg2)
	epV_IVP,	// void (HI(arg1), arg2)
	epV_i,		// void (arg1)
	epV_I,		// void (HI(arg1))
	epV_ii,		// void (LO(arg1), HI(arg1))
	epV_iii,	// void (LO(arg1), HI(arg1), (int)arg2)
	epV_iiVP,	// void (LO(arg1), HI(arg1), arg2)
	epi_i,		// int (arg1)
	epi_V,		// int (void)
	epi_ii,		// int (LO(arg1), HI(arg1))
	epi_iVP,	// int (arg1, arg2)
	epi_VP		// int (arg2)
} UEventParam;

typedef enum
{
	UMSG_NOEXT,
	UMSG_RANGE
} UMSG_EXT;

struct UMSGMAP_ENTRY
{
	int/*UEventType*/ nMessage;   // message
	UEventParam nSig;       // signature type (parameters)
	UMSG_EXT ext;
	int ext1;
	int ext2;
	UPMSG pfn;    // routine to call
};

struct UMSGMAP
{
	const UMSGMAP* pBaseMap;
	const UMSGMAP_ENTRY* lpEntries;
};

//{ #define UDECLARE_MESSAGE_MAP() 
#define UDECLARE_MESSAGE_MAP() \
private: \
	static const UMSGMAP_ENTRY _messageEntries[]; \
protected: \
	static const UMSGMAP messageMap; \
	static const UMSGMAP* GetBaseMessageMap(); \
	static const UMSGMAP* GetThisMessageMap(); \
	virtual const UMSGMAP* GetMessageMap() const; \

//}

//{ #define UBEGIN_MESSAGE_MAP(theClass, baseClass)
#define UBEGIN_MESSAGE_MAP(theClass, baseClass) \
	const UMSGMAP* theClass::GetBaseMessageMap() \
		{ return baseClass::GetThisMessageMap(); } \
	const UMSGMAP* theClass::GetThisMessageMap() \
		{ return &theClass::messageMap; } \
	const UMSGMAP* theClass::GetMessageMap() const \
		{ return &theClass::messageMap; } \
	const UMSGMAP theClass::messageMap = \
	{ baseClass::GetThisMessageMap(), &theClass::_messageEntries[0] }; \
	const UMSGMAP_ENTRY theClass::_messageEntries[] = \
	{ \

//}

//{ #define UEND_MESSAGE_MAP()
#define UEND_MESSAGE_MAP() \
		{EV_NONE, epNone, UMSG_NOEXT, 0, 0, (UPMSG)0 } \
	}; \
//}
	
#define ev_msg // intentional placeholder

class UEXPORT UCmdTarget : public UObject
{
	UDECLARE_DYNAMIC(UCmdTarget)
public:
	UCmdTarget(int widid);
	virtual ~UCmdTarget();

	typedef enum
	{
		kUNoDispatchInfo,
		kUDispatchHandled = 0x01
	} UDispatchInfo;

	//! dispatch an event using the static message table.
	virtual int UDispatchMessage(int/*UEventType*/ evt, int arg1, void *arg2, UDispatchInfo & dispatchInfo);

	inline int GetWidID(void) const { return m_widid; }
private:
	int m_widid;

	UDECLARE_MESSAGE_MAP()
};

// the interface will ask regularly the widget if they want to be
// disabled, checked (EV_UPDTCMD)...
class UCmdUI
{
public:
	UCmdUI(int id) : m_nID(id)
#ifdef qUnix
	, pCmdUI(0L)
#endif
#ifdef WIN32
	, pCmdUI(0L)
	, pNMHDR(0L)
#endif
#ifdef qMacPP
	, m_wrapper(0L)
#endif
	, m_widget(0L)
	, m_continue(true)
		{}
	virtual ~UCmdUI() {}

// Attributes
	int m_nID;

// Operations to do in ON_UUPDATECMD
	virtual void Enable(bool bOn = true);
	virtual void Check(bool bOn = true);
	virtual void SetText(const char *title);
		// by default this sets the continue state to false.

	inline void SetContinue(bool state) { m_continue = state; }
	inline bool CanContinue(void) { return m_continue; }

// Operations to do in ON_USETTOOLTIP
	virtual void SetTooltip(const char *tooltip);
#ifdef qUnix
	class UCmdUI *pCmdUI;
#endif
#ifdef WIN32
	class CCmdUI *pCmdUI;
	void *pNMHDR;
#endif
#ifdef qMacPP
	struct UPseudoCmdStatusWrapper *m_wrapper;
#endif
	void *m_widget;
	bool m_continue;
};

class USemaphore
{
public:
	USemaphore(bool & semaphore) : m_semaphore(semaphore), m_didSetSemaphore(false),
		m_enteredTwice(false)
	{
		if(m_semaphore)
		{
			m_enteredTwice = true;
			return;
		}
		m_semaphore = true;
		m_didSetSemaphore = true;
	}

	~USemaphore()
	{
		if(m_didSetSemaphore)
			m_semaphore = false;
	}

	inline bool IsEnteredTwice() const { return m_enteredTwice; }

protected:
	bool & m_semaphore;
	bool m_didSetSemaphore;
	bool m_enteredTwice;
};
#endif /* __cplusplus */

#define UMAKEINT(a, b)       ((int)(((short)(a)) | ((int)((short)(b))) << 16))
#define ULOWORD(l)           ((short)(l))
#define UHIWORD(l)           ((short)(((int)(l) >> 16) & 0xFFFF))

//{ #define UK_*
/* Modifier codes */
#define UK_LBUTTON        0x01
#define UK_MBUTTON        0x02
#define UK_RBUTTON        0x04
#define UK_CONTROL        0x08
#define UK_SHIFT          0x10
#define UK_ALT            0x20

/* Keyboard codes (non listed are ASCII codes) */
#define UK_CANCEL         0x03
#define UK_BACK           0x08
#define UK_TAB            0x09
#define UK_CLEAR          0x0C
#define UK_RETURN         0x0D
#define UK_ESCAPE         0x1B
#define UK_SPACE          0x20
#define UK_PRIOR          0x21
#define UK_NEXT           0x22
#define UK_END            0x23
#define UK_HOME           0x24
#define UK_LEFT           0x25
#define UK_UP             0x26
#define UK_RIGHT          0x27
#define UK_DOWN           0x28
#define UK_INSERT         0x2D
#define UK_DELETE         0x2E
#define UK_HELP           0x2F

#define UK_PLUS			  0x6B
#define UK_MINUS		  0x6D

#define UK_NUMPAD0        0x60
#define UK_NUMPAD1        0x61
#define UK_NUMPAD2        0x62
#define UK_NUMPAD3        0x63
#define UK_NUMPAD4        0x64
#define UK_NUMPAD5        0x65
#define UK_NUMPAD6        0x66
#define UK_NUMPAD7        0x67
#define UK_NUMPAD8        0x68
#define UK_NUMPAD9        0x69
#define UK_MULTIPLY       0x6A
#define UK_ADD            0x6B
#define UK_SEPARATOR      0x6C
#define UK_SUBTRACT       0x6D
#define UK_DECIMAL        0x6E
#define UK_DIVIDE         0x6F
#define UK_F1             0x70
#define UK_F2             0x71
#define UK_F3             0x72
#define UK_F4             0x73
#define UK_F5             0x74
#define UK_F6             0x75
#define UK_F7             0x76
#define UK_F8             0x77
#define UK_F9             0x78
#define UK_F10            0x79
#define UK_F11            0x7A
#define UK_F12            0x7B
#define UK_F13            0x7C
#define UK_F14            0x7D
#define UK_F15            0x7E
#define UK_F16            0x7F
#define UK_F17            0x80
#define UK_F18            0x81
#define UK_F19            0x82
#define UK_F20            0x83
#define UK_F21            0x84
#define UK_F22            0x85
#define UK_F23            0x86
#define UK_F24            0x87
//}

#ifdef WIN32
#	define WINCMD_DLG_START 2000
#	define WINCMD_DLG_END 3000
#	define UCMD_to_WINCMD(cmd) (cmd)
#	define WINCMD_to_UCMD(cmd) (cmd)
#	define UCMD_to_WINCTL(cmd) ((cmd)-EV_COMMAND_START+WINCMD_DLG_START)
#	define WINCTL_to_UCMD(cmd) ((cmd)-WINCMD_DLG_START+EV_COMMAND_START)
#endif

/* see umenu.h */
typedef long UMENU;
#define UMENU_NULL ((UMENU)0L)

/* toolbar manager */
#define UTB_SEPARATOR (-1)
typedef struct
{
	int widthbtn, heightbtn;	/* dimensions of each icon */
	const char *title;			/* title bar */
#ifdef WIN32
	int winid;					/* if for the toolbar (WIN32 only) */
#endif
} UTB_INIT;

typedef enum
{
	DDV_NONE
} UTB_VAR;

typedef enum
{
	DDD_NONE = 0x00
} UTB_DROP;

typedef struct
{
	const void *icon;		/* icon data (XPM) */
	int cmd;				/* associated command */
	UTB_VAR varcode;		/* misc. option */
	UTB_DROP drop;			/* for drag'n drop */
	const char *tooltip;	/* status tooltip/nprompt tooltip */
} UTB_BUTTON;

typedef struct
{
	int initialcmd;			/* initial value */
	UTB_BUTTON *btns;		/* all the dropdown buttons */
	const char *tooltip;	/* status tooltip/nprompt tooltip */
} UTB_DROPDOWN;

typedef struct
{
	int max; /* maximum of the scroll range */
	int min; /* minimum of the scroll range */
	int inc; /* increment of the scroll */
	int pos; /* current position of the slider */
} USLIDER;

typedef enum
{
	uCursArrow,
	uCursWatch
} UCURSOR;

typedef struct
{
	int cx, cy;
} USIZE;

typedef struct
{
	int x, y;
} UPOINT;

typedef struct
{
	int left, top, right, bottom;
} URECT;

typedef void *UTREEITEM;

typedef struct
{
	const char *title;			/* tree item title */
	UTREEITEM parent;			/* 0L if root */
	UTREEITEM result;			/* pointer on the result item */
	void *icon;					/* icon of the item, 0L if any */
	void *data;					/* user data of the item, 0L if any */
} UTREE_INSERT;

typedef struct
{
	UTREEITEM item;			/* pointer on the item */
	UTREEITEM parent;		/* pointer on the parent (0L if any) */
	UTREEITEM next;			/* pointer on the next item (0L if any) */
	UTREEITEM child;		/* pointer on the first child item (0L if any) */
	void *data;				/* data of the item */
	char *title;			/* title of the item */
	void *icon;				/* icon of the item */
	int expanded;			/* item is expanded */
} UTREE_INFO;

typedef struct
{
	int row, col;				/* the coordinates of the entry */
	const char *title;			/* the title of the entry (0L if any) */
	void *icon;					/* the icon of the entry (0L if any) */
} ULIST_INSERT;

typedef int (*UListSorter)(UWidget *wid, void *data1, void *data2);

typedef struct
{
	int row, col;				/* the coordinates of the entry */
	const char *title;			/* the title of the entry (0L if any) */
	void *data;					/* the user data */
} ULIST_INFO;

#ifdef __cplusplus
extern "C" {
#endif

UEXPORT int UEventSendMessage(int wid, int/*UEventType*/ evt, int arg1, void *arg2);
	/* send a message to a pseudo-widget */

UEXPORT int UEventSendMessageExt(int wid, int/*UEventType*/ evt, int arg1, void *arg2, int * handledFlag);
	/* send a message to a pseudo-widget and tells if the message was handled */

UEXPORT int UEventPostMessage(int wid, int/*UEventType*/ evt, int arg1, void *arg2);
	/* post a message to a pseudo-widget (get executed later) */

UEXPORT int UEventNotifyMessage(int/*UEventType*/ evt, int arg1, void *arg2);
	/* send a message to all the current pseudo-widget */

UEXPORT int UEventGetWidID(void);
	/* create a new widget id */

UEXPORT void UEventRemoveWidID(int widid);
	/* remove the widget from the broadcast list */

UEXPORT void UEventSetTooltipHandler(void (*handler) (const char *));
	/* set the handler (usually a status bar) called when happens a tooltip call */

UEXPORT void UEventGiveTime(void);
	/* give some time to the event manager for handling internal timers */

UEXPORT long UTickCount(void);
	/* in milliseconds */

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#	include <map>

	UEXPORT UCmdTarget *UEventQueryWidget(int widid);
		/* search for a widget (0L if failed) */

	UEXPORT const STD map<int, UCmdTarget *> & UEventGetTargets(void);
		/* return the list of the current pseudo targets */

#	ifdef WIN32
		// XPM wrappers
		UEXPORT HBITMAP UMakeBitmap(const char *icon);
		UEXPORT HBITMAP UMakeBitmap(char **icon);
		UEXPORT HBITMAP UMakeBitmap(char **icon, int & width, int & height, COLORREF back);
#	else /* !WIN32 */
#		define UMakeBitmap(b) 0L
#	endif /* !WIN32 */
#endif

#endif /* UEVENT_H */
