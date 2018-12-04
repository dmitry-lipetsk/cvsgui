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

#include "stdafx.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "uwidget.h"
#include "uconsole.h"
#include "udraw.h"
#include "umenu.h"
#include "SmartComboBox.h"

#if qMacAPP
#	include "UPseudoSupport.h"
#	include "UButton.h"
#	include "UDialogBehavior.h"
#	include "URadio.h"
#	include "UCheckbox.h"
#	include "UNumberText.h"
#	include "UPopup.h"
#	include "UWindow.h"
#	include "UViewServer.h"
#	include "USlider.h"
#	include "UDispatcher.h"
#	include "UTabbedView.h"
#	include "UProgressIndicator.h"
#endif

#ifdef qMacPP
#	include "UPseudoSupport.h"
#	include <LCheckBox.h>
#	include <LRadioButton.h>
#	include <LEditText.h>
#	include <LStaticText.h>
#	include <UModalDialogs.h>
#	include <LPopupButton.h>
#	include <LOutlineTable.h>
#	include <LOutlineItem.h>
#	include <UTableHelpers.h>
#	include <LTextTableView.h>
#	include <LCheckBoxGroupBox.h>
#	include <LPageController.h>
#	include <LTabsControl.h>
#endif

#ifdef WIN32
#	include "PseudoDlg.h"
#	include "PseudoButton.h"
#endif

using namespace std;

#if defined(_DEBUG) && defined(WIN32)
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

#ifndef NEW
#	define NEW new
#endif

UIMPLEMENT_DYNAMIC(UWidget, UCmdTarget)

UBEGIN_MESSAGE_MAP(UWidget, UCmdTarget)
	ON_UPDATEUI(UWidget)
	ON_UPDATEFOCUS(UWidget)
	ON_INIT_WIDGET(UWidget, 0, 0xFFFF)
	ON_INIT_TAB_WIDGET(UWidget, 0, 0xFFFF)
	ON_PAGE_CHANGED(UWidget, 0, 0xFFFF)
	ON_FETCHCMD(UWidget, 0, 0xFFFF)
	ON_QUERYSTATE(0, 0xFFFF, UWidget::OnGetState)
	ON_SETSTATE(0, 0xFFFF, UWidget::OnSetState)
	ON_ENABLECMD(0, 0xFFFF, UWidget::OnEnableCmd)
	ON_CHGFOCUS(UWidget, 0, 0xFFFF)
	ON_SETDEFAULT(UWidget, 0, 0xFFFF)
	ON_INVALIDATE(UWidget, 0, 0xFFFF)
	ON_REDRAW(UWidget, 0, 0xFFFF)
	ON_SHOW_WIDGET_RANGE(0, 0xFFFF, UWidget::OnShowWidget)
	ON_SETICON(0, 0xFFFF, UWidget::OnSetIcon)
	ON_SETTEXT(0, 0xFFFF, UWidget::OnSetText)
	ON_GETTEXT(0, 0xFFFF, UWidget::OnGetText)
	ON_SETFLOAT(UWidget, 0, 0xFFFF)
	ON_GETFLOAT(UWidget, 0, 0xFFFF)
	ON_SET_SLIDER(UWidget, 0, 0xFFFF)
	ON_GET_SLIDERPROP(UWidget, 0, 0xFFFF)
	ON_SET_SLIDERPROP(UWidget, 0, 0xFFFF)
	ON_CHGCURSOR(UWidget, 0, 0xFFFF)
	ON_USETTOOLTIP(0, 0xFFFF, UWidget::OnSetTooltip)
	ON_USETLLOC(0, 0xFFFF, UWidget::OnSetLoc)
	ON_UGETLLOC(0, 0xFFFF, UWidget::OnGetLoc)
	ON_TREE_RESETALL(UWidget, 0, 0xFFFF)
	ON_TREE_INSERT(UWidget, 0, 0xFFFF)
	ON_TREE_GETINFO(UWidget, 0, 0xFFFF)
	ON_TREE_DELETE(UWidget, 0, 0xFFFF)
	ON_TREE_EXPAND(UWidget, 0, 0xFFFF)
	ON_TREE_SELECT(UWidget, 0, 0xFFFF)
	ON_TREE_SETDATA(UWidget, 0, 0xFFFF)
	ON_TREE_SETICON(UWidget, 0, 0xFFFF)
	ON_TREE_GETSEL(UWidget, 0, 0xFFFF)
	ON_LIST_SETCOLTITLE(UWidget, 0, 0xFFFF)
	ON_LIST_SETCOLTITLE(UWidget, 0, 0xFFFF)
	ON_LIST_SETCOLWIDTH(UWidget, 0, 0xFFFF)
	ON_LIST_RESETALL(UWidget, 0, 0xFFFF)
	ON_LIST_ADDSEL(UWidget, 0, 0xFFFF)
	ON_LIST_DELROW(UWidget, 0, 0xFFFF)
	ON_LIST_SETFEEDBACK(UWidget, 0, 0xFFFF)
	ON_LIST_NEWROW(UWidget, 0, 0xFFFF)
	ON_LIST_INSERT(UWidget, 0, 0xFFFF)
	ON_LIST_ROWSETDATA(UWidget, 0, 0xFFFF)
	ON_LIST_ROWGETDATA(UWidget, 0, 0xFFFF)
	ON_LIST_GETNEXTSEL(UWidget, 0, 0xFFFF)
	ON_LIST_GETNEXT(UWidget, 0, 0xFFFF)
	ON_LIST_GETINFO(UWidget, 0, 0xFFFF)
	ON_LIST_ADDCOLUMNS(UWidget, 0, 0xFFFF)
	ON_LIST_SHOWCOLUMNS(UWidget, 0, 0xFFFF)
	ON_LIST_HIDECOLUMNS(UWidget, 0, 0xFFFF)
	ON_LIST_RESORT(UWidget, 0, 0xFFFF)
	ON_LIST_SETSORTDIR(UWidget, 0, 0xFFFF)
//	ON_LIST_POPUP(UWidget, 0, 0xFFFF)
	ON_COMBO_RESETALL(UWidget, 0, 0xFFFF)
	ON_COMBO_APPEND(UWidget, 0, 0xFFFF)
	ON_COMBO_SETSEL(UWidget, 0, 0xFFFF)
	ON_COMBO_GETSEL(UWidget, 0, 0xFFFF)
	ON_COMBO_GETCOUNT(UWidget, 0, 0xFFFF)
	ON_COMBO_DELSEL(UWidget, 0, 0xFFFF)
	ON_COMBO_ENABLESEL(UWidget, 0, 0xFFFF)
	ON_COMBO_GETMENU(UWidget, 0, 0xFFFF)
	ON_SMARTCOMBO_SETFEATURE(UWidget, 0, 0xFFFF)
	ON_SMARTCOMBO_SETITEMS(UWidget, 0, 0xFFFF)
	ON_SMARTCOMBO_SETREADONLY(UWidget, 0, 0xFFFF)
	ON_STATUSBAR_TEXT(UWidget, 0, 0xFFFF)
	ON_KEYDOWN(UWidget)
UEND_MESSAGE_MAP()

bool UWidget::sValidMenu = false;
int UWidget::sFocusWidID = -1;
int UWidget::sFocusCmdID;

#if qGTK
guint UWidget::lastKey = GDK_VoidSymbol;
#endif

bool UWidget::IsUIValid() { return sValidMenu; }
void UWidget::DoValidUI(bool how) { sValidMenu = how; }
int UWidget::GetFocus(void) { return sFocusWidID; }
int UWidget::GetFocusCmd(void) { return sFocusCmdID; }

#if qGTK
extern "C"
{
	static void gtkdestroy(GtkWidget *widget, gpointer user_data)
	{
		UWidget *wid = (UWidget *)user_data;
		UEventSendMessage(wid->GetWidID(), EV_DESTROY, 0, 0L);
	}

	static int gtkkeypress(GtkWidget *widget, GdkEventKey * event, gpointer user_data)
	{
		UWidget *wid = (UWidget *)user_data;
		UEventSendMessage(wid->GetWidID(), EV_KEYDOWN, event->keyval, 0L);
		return FALSE;
	}

	static guint focus_in_signal_id;
	static guint focus_out_signal_id;

	static gboolean gtkeventwatcher(GtkObject *object, guint signal_id, guint n_params, GtkArg *params, gpointer data)
	{
#if 0
		g_print ("Watch: \"%s\" emitted for %s\n",
				 gtk_signal_name (signal_id),
				 gtk_type_name (GTK_OBJECT_TYPE (object)));
#endif

		// for a tree, we are interested to know about the tree, not just the item
		if(GTK_IS_TREE_ITEM(object))
		{
			GtkTree *parent = GTK_TREE(GTK_WIDGET(object)->parent);
			// now find the upper tree
			while(1)
			{
				gpointer data = gtk_object_get_data(GTK_OBJECT(parent), "rf_treeparent");
				if(data != 0L)
					parent = GTK_TREE(GTK_WIDGET(data)->parent);
				else
					break;
			}
			object = GTK_OBJECT(parent);
		}

		if(signal_id == focus_out_signal_id)
		{
			UEventNotifyMessage(EV_UPDATEFOCUS, 1, object);
		}
		else if(signal_id == focus_in_signal_id)
		{
			UEventNotifyMessage(EV_UPDATEFOCUS, 0, object);
		}

		return TRUE;
	}

	static void gtkmaindoevent(GdkEvent *event)
	{
		gtk_main_do_event(event);

		switch(event->type)
		{
		case GDK_BUTTON_PRESS:
		case GDK_2BUTTON_PRESS:
		case GDK_3BUTTON_PRESS:
		case GDK_BUTTON_RELEASE:
		case GDK_KEY_PRESS:
		case GDK_KEY_RELEASE:
		case GDK_FOCUS_CHANGE:
			UWidget::DoValidUI(false);
			break;
		default:
			break;
		}
	}

	static void gtkclicked(GtkButton *button, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		UEventSendMessage(widid, EV_CMD, cmdid, 0L);
	}
	
	static void gtkmenuitemactivate(GtkMenuItem *menuitem, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		UEventSendMessage(widid, EV_CMD, cmdid, 0L);
	}

	static void gtkselectclist(GtkWidget *clist, gint row, gint column,
							   GdkEventButton *event, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		gchar *text;
		gtk_clist_get_text(GTK_CLIST(clist), row, column, &text);
		UEventSendMessage(widid, EV_LIST_SELECTING, UMAKEINT(cmdid, row), text);
		//if(event) fprintf(stderr, "Button %d\n", event->button);
	}

	static void gtkselectclistcolumn(GtkCList *clist, gint column, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		UEventSendMessage(widid, EV_LIST_SELCOLUMN, UMAKEINT(cmdid, column), 0L);
	}

	static gint gtkclistclick(GtkWidget *widget, GdkEventButton *event, gpointer data)
	{
		int widid = ULOWORD((int)data);
		int cmdid = UHIWORD((int)data);
					
		//fprintf(stderr, "Button %d\n", event->button);
		
//		if (event->button == 3)
//		{
//			UEventSendMessage(widid, EV_LIST_POPUP, cmdid, 0L);
//		}
		

		switch(event->type) {
			case GDK_BUTTON_PRESS:	// ignored; in lists a singleit's a selection ..
				break; 
			
			case GDK_2BUTTON_PRESS:
				UEventSendMessage(widid, EV_LIST_DBLCLICK, UMAKEINT(cmdid, event->button), 0L);
				return FALSE;
				break;
			
			default: 
				break;
		}
		
		return TRUE;
	}

	static void gtkcomboclicked(GtkWidget *widget, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);

		GtkList *list = GTK_LIST(widget->parent);
		GList *selection = list->selection;
		int count = -1;
		string selTxt;
		if (selection)
		{
			GtkBin *bin = GTK_BIN( selection->data );
			gchar *txt;
			gtk_label_get(GTK_LABEL(bin->child), &txt);
			selTxt = txt;
			GList *child = list->children;
			count = 0;
			while (child)
			{
				if (child->data == selection->data)
					break;
				count++;
				child = child->next;
			}
			g_assert(child != 0L);
		}
		UEventSendMessage(widid, EV_COMBO_SEL, UMAKEINT(cmdid, count), (void *)(const char *)selTxt.c_str());
	}

	GtkLabel *getLabelFromContainer(GtkContainer *w)
	{
		GList *list = gtk_container_children(w);
		GtkLabel *label = 0L;
		while(list != 0L)
		{
			GtkWidget *wid = GTK_WIDGET(list->data);
			if(GTK_IS_LABEL(wid))
			{
				label = GTK_LABEL(wid);
				break;
			}
			list = (GList *)g_list_next(list);
		}
		return label;
	}

	static void gtkmenuactivated(GtkWidget *widget, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);

		string selTxt;

		// fix me: where to find the label of the item ?
#if 1
		selTxt = (char *)gtk_object_get_data(GTK_OBJECT(widget), "rf_menuitemtext");
#else
		GtkLabel *label = getLabelFromContainer(GTK_CONTAINER(widget));
		gchar *txt;
		gtk_label_get(label, &txt);
		selTxt = txt;
#endif


		GtkMenu *menu = GTK_MENU(widget->parent);
		GList *list = GTK_MENU_SHELL(menu)->children;
		int count = 0;
		while(list != 0L)
		{
			GtkWidget *wid = GTK_WIDGET(list->data);
			if(wid == widget)
				break;
			list = (GList *)g_list_next(list);
			count++;
		}
		g_assert(list != 0L);
		UEventSendMessage(widid, EV_COMBO_SEL, UMAKEINT(cmdid, count), (void *)(const char *)selTxt.c_str());
	}

	static gint gtkcancel(GtkWidget *widget, GdkEvent* event, gpointer user_data)
	{
		static bool sSemaphore = false;
		USemaphore policeman(sSemaphore);
		if(policeman.IsEnteredTwice())
			return TRUE;

		UWidget *wid = (UWidget *)user_data;
		wid->EndModal(false);
		return TRUE;
	}

	static bool matchToken(const char *str, int &cmd)
	{
		if(str == 0L)
			return false;

		const char *origStr = str;
		size_t len = strlen(kUgtkPrefixToken);
		if(strncmp(str, kUgtkPrefixToken, len) != 0)
			return false;
		
		str += len;
		if(*str && *str != '_')
			str++;
		if(*str && *str == '_')
			str++;

		if(sscanf(str, "%d", &cmd) == 1 && cmd >=0 && cmd < (32767 - EV_COMMAND_START))
		{
			cmd += EV_COMMAND_START;
			return true;
		}
		else
			g_warning("The widget '%s' has a wrong pseudo command '%d'\n", origStr, cmd);

		return false;
	}

	static void dataForeach(GQuark key_id, gpointer data, gpointer user_data)
	{
		const char *str = g_quark_to_string(key_id);
		int id = (int)user_data;
		int cmd;
		if(matchToken(str, cmd))
		{
			UASSERT(GTK_IS_WIDGET(data));

			GtkWidget *w = (GtkWidget *)data;
			w = w->parent;
			while(w != 0L)
			{
				if(GTK_IS_NOTEBOOK(w))
				{
					// don't extract widgets children of a notebook at this point
					return;
				}
				w = w->parent;
			}

			UEventSendMessage(id, EV_INIT_WIDGET, cmd, data);
		}
	}

	typedef struct
	{
		int widid;
		int pageid;
		int pagenum;
	} _dataTabForeach;

	static void dataTabForeach(GQuark key_id, gpointer data, gpointer user_data)
	{
		const char *str = g_quark_to_string(key_id);
		_dataTabForeach *mydata = (_dataTabForeach *)user_data;
		int cmd;
		if(matchToken(str, cmd))
		{
			UASSERT(GTK_IS_WIDGET(data));
			GtkWidget *w = (GtkWidget *)data;
			w = w->parent;
			while(w != 0L)
			{
				if(GTK_IS_NOTEBOOK(w))
				{
					break;
				}
				w = w->parent;
			}
			if(w == 0L)
			{
				// this is not a child of a notebook
				return;
			}

			gchar *name = gtk_widget_get_name(w);
			int pagecmd;
			if(!matchToken(name, pagecmd) || pagecmd != mydata->pageid)
				return;

			GtkNotebook *n = GTK_NOTEBOOK(w);
			w = (GtkWidget *)data;
			int pagenum = -1;
			while(w != 0L && pagenum == -1)
			{
				int i = 0;
				GtkWidget *p;
				while((p = gtk_notebook_get_nth_page(n, i)) != 0L)
				{
					if(w == p)
					{
						pagenum = i;
						break;
					}
					i++;
				}
				w = w->parent;
			}

			if(pagenum == -1)
			{
				g_warning("Could not find the page of the widget '%s'\n", str);
				return;
			}

			if(pagenum != mydata->pagenum)
				return;

			UEventSendMessage(mydata->widid, EV_INIT_WIDGET, cmd, data);
		}
	}

	static void gtkpacktreeitem(GtkBox *m_box, const char *title, GtkPixmap *icon)
	{
		if(icon != 0L)
		{
			gtk_misc_set_alignment (GTK_MISC (icon), 0.0, 0.5);
			gtk_box_pack_start(m_box, GTK_WIDGET(icon), FALSE, FALSE, 0);
			gtk_widget_show (GTK_WIDGET(icon));
		}

		GtkLabel *m_label_widget = GTK_LABEL(gtk_label_new (title != 0L ? title : ""));
		gtk_misc_set_alignment (GTK_MISC (m_label_widget), 0.5, 0.5);
		gtk_box_pack_start(m_box, GTK_WIDGET(m_label_widget), FALSE, FALSE, 0);
		gtk_widget_show (GTK_WIDGET(m_label_widget));
		
		gtk_widget_show(GTK_WIDGET(m_box));
	}

	static GtkTreeItem *gtknewtreeitem(const char *title, GtkPixmap *icon)
	{
		GtkTreeItem *item;
		item = GTK_TREE_ITEM(gtk_tree_item_new());
		
		GtkHBox *m_box = GTK_HBOX(gtk_hbox_new(FALSE, 2));
		gtk_container_add (GTK_CONTAINER (item), GTK_WIDGET(m_box));

		gtkpacktreeitem(GTK_BOX(m_box), title, icon);
		
		return item;
	}

	static void gtktreeexpand(GtkWidget *item, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		g_assert(GTK_IS_TREE_ITEM(item));
		UEventSendMessage(widid, EV_TREE_EXPANDING, UMAKEINT(cmdid, 0), item);
	}

	static void gtktreecollapse(GtkWidget *item, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		g_assert(GTK_IS_TREE_ITEM(item));
		UEventSendMessage(widid, EV_TREE_EXPANDING, UMAKEINT(cmdid, 1), item);
	}

	static void gtktreeselect(GtkWidget *item, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		g_assert(GTK_IS_TREE_ITEM(item));
		UEventSendMessage(widid, EV_TREE_SELECTING, UMAKEINT(cmdid, 0), item);
	}

	static void gtktreedeselect(GtkWidget *item, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		g_assert(GTK_IS_TREE_ITEM(item));
		UEventSendMessage(widid, EV_TREE_SELECTING, UMAKEINT(cmdid, 1), item);
	}
   
	static GtkLabel *gtktreegetlabel(GtkWidget *item)
	{
		GtkBox *box = GTK_BOX (GTK_BIN (item)->child);
		GList *list = box->children;
		GtkLabel *label = 0L;
		while(list != 0L)
		{
			GtkWidget *wid = ((GtkBoxChild *)list->data)->widget;
			if(GTK_IS_LABEL(wid))
			{
				label = (GtkLabel *)wid;
				break;
			}
			list = (GList *)g_list_next(list);
		}
		return label;
	}
  
  static gint treeListSort(gconstpointer ptr1, gconstpointer ptr2)
  {
    GtkLabel *label1, *label2;
    gchar *title1, *title2;
		
    label1 = gtktreegetlabel((GtkWidget*)ptr1);
    label2 = gtktreegetlabel((GtkWidget*)ptr2);
    
    gtk_label_get(label1, &title1);
    gtk_label_get(label2, &title2);
          
#ifdef qUnix
    return strcmp(title1, title2);
#else
    return _stricmp(title1, title2);
#endif
  }

  static void gtktreesort(GtkTree *w)
  {
    if(w == 0L)
      return;

# if qGTK
    if(!GTK_IS_TREE(w))
      return;

    GtkTree *parent = GTK_TREE(w);

    GList *list = GTK_TREE(parent)->children;
   
    g_list_sort(list, treeListSort);

    parent->children = g_list_first(list);
#endif
  }
  
	static void gtksettreeitemicon(GtkTreeItem *item, GtkPixmap *icon)
	{
		gchar *title = 0L;
		GtkLabel *label = gtktreegetlabel(GTK_WIDGET(item));
		if(label != 0L)
			gtk_label_get(label, &title);
		string str(title);

		GtkBox *m_box = GTK_BOX(GTK_BIN (item)->child);
		GList *list = g_list_copy(m_box->children);
		while(list != 0L)
		{
			GtkWidget *wid = ((GtkBoxChild *)list->data)->widget;
			gtk_container_remove(GTK_CONTAINER(m_box), wid);
							  
			list = (GList *)g_list_next(list);
		}
		g_list_free(list);
		
		gtkpacktreeitem(m_box, str.c_str(), icon);
	}

	// TODO: remove that when the GTK one is working
	static gint gtk_clist_get_text_is_buggy(GtkCList  *clist,
											gint       row,
											gint       column,
											gchar    **text)
	{
		GtkCListRow *clist_row;
		
		g_return_val_if_fail (clist != NULL, 0);
		g_return_val_if_fail (GTK_IS_CLIST (clist), 0);
		
		if (row < 0 || row >= clist->rows)
			return 0;
		if (column < 0 || column >= clist->columns)
			return 0;
		
		clist_row = (GtkCListRow *)(g_list_nth (clist->row_list, row))->data;
		
		if (clist_row->cell[column].type == GTK_CELL_TEXT)
		{
			if (text)
				*text = GTK_CELL_TEXT (clist_row->cell[column])->text;
		}
		else if (clist_row->cell[column].type == GTK_CELL_PIXTEXT)
		{
			if (text)
				*text = GTK_CELL_PIXTEXT(clist_row->cell[column])->text;
		}
		else
			return 0;
		
		return 1;
	}

	static void
	gtkswitchpage(GtkWidget       *w,
				  GtkNotebookPage *page,
				  gint             page_num,
				  gpointer         data)
	{
		int widid = ULOWORD((int)data);
		int cmdid = UHIWORD((int)data);
		
		UEventSendMessage(widid, EV_PAGE_CHANGED, UMAKEINT(cmdid, page_num), 0L);
	}

	static UListSorter sGtkClistDiff = 0L;
	static UWidget *sGtkClistWid = 0L;

	static gint gtkclistdefcompare(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
	{
		if(sGtkClistDiff == 0L)
			return 0;

		GtkCListRow *row1 = (GtkCListRow *) ptr1;
		GtkCListRow *row2 = (GtkCListRow *) ptr2;
		return sGtkClistDiff(sGtkClistWid, row1->data, row2->data);
	}

	static gint gtkexposeevent(GtkWidget *widget, GdkEvent *event, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		
		UEventSendMessage(widid, EV_CUSTOM_DRAW, cmdid, event);
		return TRUE;
	}
	
	static gint gtklayoutclicked(GtkWidget *widget, GdkEvent *event, gpointer user_data)
	{
		int widid = ULOWORD((int)user_data);
		int cmdid = UHIWORD((int)user_data);
		
		UEventSendMessage(widid, EV_CUSTOM_CLICK, cmdid, event);
		return TRUE;
	}
} // extern "C"

#endif /* qGTK */

#if qMacAPP
	static bool macapp_getcmdid(TView *view, int & cmdid)
	{
		IDType atype = view->GetIdentifier();
		const char *atypestr = (char *)&atype;
		if(atypestr[0] != 'p' && atypestr[1] != 's')
			return false;
		
		char cmd[3] = {atypestr[2], atypestr[3], 0};
		return sscanf(cmd, "%d", &cmdid) == 1;
	}
	
	static void macapp_fecthview(int widid, TView *view)
	{
		if(view == 0L)
			return;
		
		int cmdid;
		if(macapp_getcmdid(view, cmdid))
		{
			UEventSendMessage(widid, EV_INIT_WIDGET, cmdid + EV_COMMAND_START, view);
		}

		if(MA_MEMBER(view, TTabbedView))
		{
			TTabbedView *tab = (TTabbedView *)view;
			TView *tabview;
			SInt32 tabIndex = 1;
			while((tabview = tab->GetPanelView(tabIndex++)) != 0L)
			{
				macapp_fecthview(widid, tabview);
			}
		}
		
		macapp_fecthview(widid, view->GetNextView());
		macapp_fecthview(widid, view->GetSubView());
	}
#endif /* qMacAPP */

UWidget::UWidget(int widid) : UCmdTarget(widid), m_currentPage(-1)
#if qGTK || qMacPP
	, m_modalLoop(false)
#endif
#ifdef WIN32
	, m_curModal(0L), m_noDispatch(false)
#endif
{
#if qMacAPP
	static bool firstTime = true;
	if(firstTime)
	{
		firstTime = false;
		gDispatcher->AddBehavior(new TPseudoAppBehavior);
			// this behavior takes care about menu routing and giving time
			// to the pseudo-events.
		new UMacAppMenu();
			// for MacApp, we can create automatically the pseudo menu handling.
	}
#endif
#if qMacPP
	static bool firstTime = true;
	if(firstTime && LCommander::GetTopCommander() != 0L)
	{
		firstTime = false;
		LCommander::GetTopCommander()->AddAttachment(NEW LMainPseudoAttachment());
	}
#endif
}

UWidget::~UWidget()
{
	vector<UCmdTarget *>::iterator i;
	for(i = m_listeners.begin(); i != m_listeners.end(); ++i)
	{
		delete *i;
	}
	map<int, UWidget *>::iterator j;
	for(j = m_tabs.begin(); j != m_tabs.end(); ++j)
	{
		delete (*j).second;
	}
	if(HasFocus())
		sFocusWidID = -1;
}

void UWidget::AddListener(UCmdTarget * listener)
{
	m_listeners.push_back(listener);
}

int UWidget::UDispatchMessage(int/*UEventType*/ evt, int arg1, void *arg2, UDispatchInfo & dispatchInfo)
{
	vector<UCmdTarget *>::iterator i;
	for(i = m_listeners.begin(); i != m_listeners.end(); ++i)
	{
		(*i)->UDispatchMessage(evt, arg1, arg2, dispatchInfo);
	}
	return UCmdTarget::UDispatchMessage(evt, arg1, arg2, dispatchInfo);
}

void *UWidget::GetWidget(int cmd) const
{
	map<int, void *>::const_iterator i = m_widgets.find(cmd);
	if(i == m_widgets.end())
	{
		UAppConsole("Unable to find widget id : %d (file %s:%d)\n", cmd, __FILE__, __LINE__);
#ifdef DEBUG
		printf( "Valid Ids are:\n");
		for ( i = m_widgets.begin(); i != m_widgets.end(); ++i) {
			printf( "    Id: %d, Addr: 0x%x\n", (*i).first, (unsigned int)(*i).second);
		} 
#endif
		return 0L;
	}
	return (*i).second;
}

const char *UWidget::GetTooltip(int cmd) const
{
	map<int, string>::const_iterator i = m_tooltips.find(cmd);
	if(i == m_tooltips.end())
	{
		UAppConsole("Unable to find widget id : %d (file %s:%d)\n", cmd, __FILE__, __LINE__);
#ifdef DEBUG
		printf( "Valid Ids are:\n");
		for ( i = m_tooltips.begin(); i != m_tooltips.end(); ++i) {
			printf( "    Id: %d, Str: '%s'\n", (*i).first, (*i).second.c_str());
		} 
#endif
		return 0L;
	}
	return (*i).second.c_str();
}

void UWidget::OnGetLoc(int cmd, URECT * r)
{
	r->left = 0; // sanity
	r->right = 0;
	r->top = 0;
	r->bottom = 0;

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	CViewRect mr = view->GetFrame();
	view->SuperViewToLocal(mr);
	view->LocalToRootView(mr);
	r->left = mr.left;
	r->right = mr.right;
	r->top = mr.top;
	r->bottom = mr.bottom;
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	SPoint32 outLocation;
	SDimension16 outSize;
	view->GetFrameLocation(outLocation);
	view->GetFrameSize(outSize);

	*r = URect(UPoint(outLocation.h, outLocation.v), USize(outSize.width, outSize.height));
#endif
#ifdef WIN32
	CRect mr;
	CWnd *wnd = (CWnd *)w;
	wnd->GetClientRect(mr);
	CWnd *parent = wnd->GetParent();
	if(parent != 0L)
	{
		wnd->ClientToScreen(mr);
		parent->ScreenToClient(mr);
	}
	r->left = mr.left;
	r->right = mr.right;
	r->top = mr.top;
	r->bottom = mr.bottom;
#endif
}

void UWidget::OnSetLoc(int cmd, const URECT * r)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	CViewRect mr(r->left, r->top, r->right, r->bottom);
	if(mr.right < mr.left)
		mr.right = mr.left;
	if(mr.bottom < mr.top)
		mr.bottom = mr.top;
	view->RootViewToLocal(mr);
	view->LocalToSuperView(mr);
	view->SetFrame(mr, true);
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	SPoint32 oldLocation;
	view->GetFrameLocation(oldLocation);
	URect newDim(*r);
	UPoint offset = newDim.TopLeft() - UPoint(oldLocation.h, oldLocation.v);
	view->ResizeFrameTo(newDim.Width(), newDim.Height(), true);
	view->MoveBy(offset.x, offset.y, true);
#endif
#ifdef WIN32
	CRect wr(r->left, r->top, r->right, r->bottom);
	CWnd *wnd = (CWnd *)w;
	wnd->MoveWindow(wr, TRUE);
#endif
}

void UWidget::OnSetTooltip(int cmd, UCmdUI *pCmdUI)
{	
	const char *tooltip = GetTooltip(cmd);
	if(tooltip == 0L)
		return;

	pCmdUI->SetTooltip(tooltip);
}

bool UWidget::WidgetIsActive(int cmd) const
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return false;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	bool res = false;

	CWnd* parent = wnd->GetOwner();
	if(parent && (HWND)*parent->GetFocus() == (HWND)*wnd)
		res = true;

	return res;
#endif
	return false;
}

void UWidget::AddTooltip(int cmd, const char* tooltipID)
{
	map<int, string>::value_type val(cmd, string(tooltipID));
	m_tooltips.insert(val);
}

void UWidget::OnInitWidget(int cmd, void *widget)
{
	map<int, void *>::value_type val(cmd, widget);
	m_widgets.insert(val);

#ifdef WIN32

#endif
#if qMacPP
	LPane *view = (LPane *)widget;
	if(view != 0L)
	{
		// fetch the inside widgets and add them the the widget list
		static bool sSemaphore = false;
		USemaphore policeman(sSemaphore);
		if(!policeman.IsEnteredTwice())
		{
			macpp_fecthview(GetWidID(), view);
		}
		
		LWindow *wind = dynamic_cast<LWindow*>(view);
		if(wind != 0L)
		{
			LPseudoWinAttachment *attach = NEW LPseudoWinAttachment(GetWidID());
			wind->AddAttachment(attach);
			macpp_linkbroadcasters(wind, attach);
			
			SPaneInfo inPaneInfo;
			memset(&inPaneInfo, 0, sizeof(SPaneInfo));
			inPaneInfo.superView = wind;
			NEW UPseudoFakePane(inPaneInfo, attach);

			// call the create callback
			UEventSendMessage(GetWidID(), EV_CREATE, 0, 0L);
		}
	}
#endif
#if qMacAPP
	TView *view = (TView *)widget;
	if(MA_MEMBER(view, TView))
	{
		// fetch the inside widgets and add them the the widget list
		static bool sSemaphore = false;
		USemaphore policeman(sSemaphore);
		if(!policeman.IsEnteredTwice())
		{
			macapp_fecthview(GetWidID(), view);
			//macapp_workaround_adlib_bug(view);
		}
		
		if(MA_MEMBER(view, TWindow))
		{
			// add the pseudo behaviour which re-routes the events to the pseudo-widget
			TPseudoWinBehavior *psbehavior = dynamic_cast_AC(TPseudoWinBehavior*,
				view->GetBehaviorWithIdentifier(TPseudoWinBehavior::kPseudoWinBehavior));
			
			if(psbehavior == 0L)
			{
				psbehavior = TH_new TPseudoWinBehavior;
				view->AddBehavior(psbehavior);
			}
			psbehavior->SetPseudoWidget(GetWidID());
			
			// add the pseudo adorner which intercepts the frame size changes
			TPseudoWinAdorner *psadorner = dynamic_cast_AC(TPseudoWinAdorner*,
				view->AdornerWithID(TPseudoWinAdorner::kPseudoWinAdorner));
			
			if(psadorner == 0L)
			{
				psadorner = TH_new TPseudoWinAdorner;
				view->AddAdorner(psadorner, kAdornLast, false);
			}
			psadorner->SetPseudoWidget(GetWidID());
			
			// call the create callback
			UEventSendMessage(GetWidID(), EV_CREATE, 0, 0L);
		}
	}
#endif
#if qGTK
	static bool firstTime = true;
	if(firstTime)
	{
		// for the disable/enable handling notification (EV_UPDATEUI)
		gdk_event_handler_set((GdkEventFunc)gtkmaindoevent, NULL, NULL);

		// listen focus events
		focus_in_signal_id = gtk_signal_lookup ("focus_in_event", GTK_TYPE_WIDGET);
		gtk_signal_add_emission_hook (focus_in_signal_id, gtkeventwatcher, NULL);
		focus_out_signal_id = gtk_signal_lookup ("focus_out_event", GTK_TYPE_WIDGET);
		gtk_signal_add_emission_hook (focus_out_signal_id, gtkeventwatcher, NULL);

		firstTime = false;
	}

	// so for every widget mapped, we'll be able to find the pseudo-widget associated with.
	gtk_object_set_data(GTK_OBJECT(widget), kUgtkDataToken, (gpointer)UMAKEINT(GetWidID(), cmd));

	// we are only interesting about the main widgets here
	if(GTK_IS_WIDGET(widget))
	{
		if(cmd == kUMainWidget)
		{
			// extract and map to the pseudo widget the sub-widgets begining with 'kUgtkPrefixToken'
			g_datalist_foreach (&GTK_OBJECT(widget)->object_data, dataForeach, (gpointer)GetWidID());
			
			// map the destroy callback
			gtk_signal_connect (GTK_OBJECT(widget), "destroy",
								GTK_SIGNAL_FUNC(gtkdestroy), (gpointer)this);

			gtk_signal_connect (GTK_OBJECT(widget), "key_press_event",
								GTK_SIGNAL_FUNC(gtkkeypress), (gpointer)this);
			
			ResetKeyState();
			
			// call the create callback
			UEventSendMessage(GetWidID(), EV_CREATE, 0, 0L);
		}
	}

	if(GTK_IS_NOTEBOOK(widget))
	{
		gtk_signal_connect (GTK_OBJECT(widget), "switch_page",
							GTK_SIGNAL_FUNC(gtkswitchpage),
							(gpointer)UMAKEINT(GetWidID(), cmd));
	}

	if(GTK_IS_BUTTON(widget))
	{
		gtk_signal_connect (GTK_OBJECT(widget), "clicked",
			GTK_SIGNAL_FUNC(gtkclicked), (gpointer)UMAKEINT(GetWidID(), cmd));
	}

	if(GTK_IS_MENU_ITEM(widget))
	{
		gtk_signal_connect(GTK_OBJECT(widget), "activate",
			GTK_SIGNAL_FUNC(gtkmenuitemactivate), (gpointer)UMAKEINT(GetWidID(), cmd));
	}

	if(GTK_IS_COMBO(widget))
	{
		gtk_signal_connect(GTK_OBJECT(GTK_COMBO(widget)->entry), "activate",
			GTK_SIGNAL_FUNC(gtkclicked), (gpointer)UMAKEINT(GetWidID(), cmd));
	}
	
	if(GTK_IS_CLIST(widget))
	{
		gtk_signal_connect (GTK_OBJECT(widget), "select_row",
			GTK_SIGNAL_FUNC(gtkselectclist), (gpointer)UMAKEINT(GetWidID(), cmd));
		
		gtk_signal_connect (GTK_OBJECT(widget), "click_column",
			GTK_SIGNAL_FUNC(gtkselectclistcolumn), (gpointer)UMAKEINT(GetWidID(), cmd));
		
		gtk_signal_connect_after (GTK_OBJECT (widget), "button_press_event",
								  GTK_SIGNAL_FUNC (gtkclistclick), (gpointer)UMAKEINT(GetWidID(), cmd));
	}

	if(GTK_IS_DRAWING_AREA(widget))
	{
		gtk_signal_connect_after (GTK_OBJECT(widget), "button_press_event",
								  GTK_SIGNAL_FUNC(gtklayoutclicked), (gpointer)UMAKEINT(GetWidID(), cmd));
		gtk_signal_connect (GTK_OBJECT(widget), "expose_event",
							GTK_SIGNAL_FUNC (gtkexposeevent), (gpointer)UMAKEINT(GetWidID(), cmd));
		gtk_widget_set_events (GTK_WIDGET(widget), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
	}

	if(GTK_IS_LAYOUT(widget))
	{
		gtk_signal_connect_after (GTK_OBJECT(widget), "button_press_event",
								  GTK_SIGNAL_FUNC(gtklayoutclicked), (gpointer)UMAKEINT(GetWidID(), cmd));
		gtk_signal_connect (GTK_OBJECT(widget), "expose_event",
							GTK_SIGNAL_FUNC (gtkexposeevent), (gpointer)UMAKEINT(GetWidID(), cmd));
		gtk_widget_set_events (GTK_WIDGET(widget), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
	}
#endif /* qGTK */
}

void UWidget::OnInitTabWidget(int cmd, int page, void *widget)
{
#if qGTK
	if(GTK_IS_WIDGET(widget))
	{
		// extract and map to the pseudo widget the sub-widgets begining with 'kUgtkPrefixToken'
		_dataTabForeach data = {GetWidID(), cmd, page};
		g_datalist_foreach (&GTK_OBJECT(widget)->object_data, dataTabForeach, (gpointer)&data);
	}
#endif /* qGTK */
}

void UWidget::OnBlockHandler(void* widget, int cmd, int event)
{
#ifdef qGTK
	if(event == GDK_EXPOSE) {
		gtk_signal_handler_block_by_func( GTK_OBJECT(widget),
                                          GTK_SIGNAL_FUNC (gtkexposeevent),
                                          (gpointer)UMAKEINT(GetWidID(), cmd) );
	}
#endif
}

void UWidget::OnUnblockHandler(void* widget, int cmd, int event)
{
#ifdef qGTK
	if(event == GDK_EXPOSE) {
		gtk_signal_handler_unblock_by_func( GTK_OBJECT(widget),
                                            GTK_SIGNAL_FUNC (gtkexposeevent),
                                            (gpointer)UMAKEINT(GetWidID(), cmd));
		gtk_widget_show(GTK_WIDGET(widget));
	}
#endif
}

int UWidget::OnGetState(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return 0;

	int state = 0; // sanity

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CPseudoRotButton)))
	{
		CPseudoRotButton *btn = (CPseudoRotButton *)wnd;
		state = btn->GetCurrent();
	}
 	else if(wnd->IsKindOf(RUNTIME_CLASS(CPseudoToggle)))
	{
		CPseudoToggle *btn = (CPseudoToggle *)wnd;
		state = btn->GetState();
	}
	else if(wnd->IsKindOf(RUNTIME_CLASS(CButton)))
	{
		CButton *btn = (CButton *)wnd;
		state = btn->GetCheck() != 0;
	}
	else
	{
		CString s;
		wnd->GetWindowText(s);
		sscanf(s, "%d", &state);
	}
#endif
#if qGTK
	if(GTK_IS_TOGGLE_BUTTON(w))
	{
		state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)) != 0;
	}
	else if(GTK_IS_ENTRY(w))
	{
		gchar *text = gtk_entry_get_text(GTK_ENTRY(w));
		sscanf(text, "%d", &state);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LCheckBox *check;
	LRadioButton *rad;
	LEditText *edit;
	LStaticText *stat;
	LBevelButton *bevel;
	UPseudoRotButton *rot;
	LCheckBoxGroupBox *groupbox;
	LPageController *tabctrl;
	LTabsControl *tabctrl2;
	LControl *ctrl;
	if((check = dynamic_cast<LCheckBox *>(view)) != 0L)
	{
		state = check->GetValue() == Button_On ? 1 : 0;
	}
	else if((rad = dynamic_cast<LRadioButton *>(view)) != 0L)
	{
		state = rad->GetValue() == Button_On ? 1 : 0;
	}
	else if((edit = dynamic_cast<LEditText *>(view)) != 0L)
	{
		LStr255 pstr;
		edit->GetText(pstr);
		state = (SInt32)pstr;
	}
	else if((stat = dynamic_cast<LStaticText *>(view)) != 0L)
	{
		LStr255 pstr;
		stat->GetText(pstr);
		state = (SInt32)pstr;
	}
	else if((groupbox = dynamic_cast<LCheckBoxGroupBox *>(view)) != 0L)
	{
		state = groupbox->GetValue() == Button_On ? 1 : 0;
	}
	else if((rot = dynamic_cast<UPseudoRotButton *>(view)) != 0L)
	{
		state = rot->GetCurrentState();
	}
	else if((bevel = dynamic_cast<LBevelButton *>(view)) != 0L)
	{
		state = bevel->GetValue() == Button_On ? 1 : 0;
	}
	else if((tabctrl = dynamic_cast<LPageController *>(view)) != 0L)
	{
		state = tabctrl->GetCurrentPageIndex() - 1;
	}
	else if((tabctrl2 = dynamic_cast<LTabsControl *>(view)) != 0L)
	{
		state = tabctrl2->GetValue() - 1;
	}
	else if((ctrl = dynamic_cast<LControl *>(view)) != 0L)
	{
		state = ctrl->GetValue();
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TCheckBox))
	{
		TCheckBox *check = (TCheckBox *)view;
		state = check->GetState();
	}
	else if(MA_MEMBER(view, TRadio))
	{
		TRadio *rad = (TRadio *)view;
		state = rad->GetState();
	}
	else if(MA_MEMBER(view, TNumberText))
	{
		TNumberText *num = (TNumberText *)view;
		state = num->GetValue();
	}
#endif

	return state;
}

void UWidget::OnGetFloat(int cmd, float *f)
{
	*f = 0.0; // sanity

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacPP
	LPane *view = (LPane *)w;
	LEditText *edit;
	LStaticText *stat;
	if((edit = dynamic_cast<LEditText *>(view)) != 0L)
	{
		LStr255 pstr;
		edit->GetText(pstr);
		*f = (double)pstr;
	}
	else if((stat = dynamic_cast<LStaticText *>(view)) != 0L)
	{
		LStr255 pstr;
		stat->GetText(pstr);
		*f = (double)pstr;
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoDoubleNumberText))
	{
		TPseudoDoubleNumberText *num = (TPseudoDoubleNumberText *)view;
		*f = num->GetValue();
	}
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	CString s;
	wnd->GetWindowText(s);
	sscanf(s, "%f", f);
#endif
}

void UWidget::OnSetState(int cmd, int state)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CPseudoRotButton)))
	{
		CPseudoRotButton *btn = (CPseudoRotButton *)wnd;
		btn->DoRotate(state);
	}
	else if(wnd->IsKindOf(RUNTIME_CLASS(CPseudoToggle)))
	{
		CPseudoToggle *btn = (CPseudoToggle *)wnd;
		btn->SetState(state);
	}
	else if(wnd->IsKindOf(RUNTIME_CLASS(CButton)))
	{
		CButton *btn = (CButton *)wnd;
		btn->SetCheck(state != 0);
	}
	else
	{
		USemaphore policeman(m_noDispatch);
		char tmp[100];
		sprintf(tmp, "%d", state);
		wnd->SetWindowText(tmp);
	}
#endif
#if qGTK
	if(GTK_IS_TOGGLE_BUTTON(w))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), state != 0);
	}
	else if(GTK_IS_ENTRY(w))
	{
		char tmp[20];
		sprintf(tmp, "%d", state);
		gtk_entry_set_text(GTK_ENTRY(w), tmp);
	}
#endif
#if qMacPP
	USemaphore policeman(gTurnOffBroadcast);

	LPane *view = (LPane *)w;
	LCheckBox *check;
	LRadioButton *rad;
	LEditText *edit;
	LStaticText *stat;
	UPseudoRotButton *rot;
	LCheckBoxGroupBox *groupbox;
	LPageController *tabctrl;
	LTabsControl *tabctrl2;
	LControl *ctrl;
	if((check = dynamic_cast<LCheckBox *>(view)) != 0L)
	{
		check->SetValue(state ? Button_On : Button_Off);
	}
	else if((rad = dynamic_cast<LRadioButton *>(view)) != 0L)
	{
		rad->SetValue(state ? Button_On : Button_Off);
	}
	else if((groupbox = dynamic_cast<LCheckBoxGroupBox *>(view)) != 0L)
	{
		groupbox->SetValue(state ? Button_On : Button_Off);
	}
	else if((edit = dynamic_cast<LEditText *>(view)) != 0L)
	{
		LStr255 pstr((SInt32)state);
		edit->SetText(pstr);
	}
	else if((stat = dynamic_cast<LStaticText *>(view)) != 0L)
	{
		LStr255 pstr((SInt32)state);
		stat->SetText(pstr);
	}
	else if((rot = dynamic_cast<UPseudoRotButton *>(view)) != 0L)
	{
		rot->DoRotate(state);
	}
	else if((tabctrl = dynamic_cast<LPageController *>(view)) != 0L)
	{
		tabctrl->SetSelectedPageButtonByIndex(state + 1, false);
	}
	else if((tabctrl2 = dynamic_cast<LTabsControl *>(view)) != 0L)
	{
		tabctrl2->SetValue(state + 1);
	}
	else if((ctrl = dynamic_cast<LControl *>(view)) != 0L)
	{
		ctrl->SetValue(state);
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TCheckBox))
	{
		TCheckBox *check = (TCheckBox *)view;
		check->SetState(state, true);
	}
	else if(MA_MEMBER(view, TRadio))
	{
		TRadio *rad = (TRadio *)view;
		rad->SetState(state, true);
	}
	else if(MA_MEMBER(view, TNumberText))
	{
		TNumberText *num = (TNumberText *)view;
		num->SetValue(state, true);
	}
	else if(MA_MEMBER(view, TPseudoRotateIconButton))
	{
		TPseudoRotateIconButton *rot = (TPseudoRotateIconButton *)view;
		rot->DoRotate(state);
	}
	else if(MA_MEMBER(view, TProgressIndicator))
	{
		TProgressIndicator *progress = (TProgressIndicator *)view;
		progress->SetValue(state, true);
	}
#endif
}

void UWidget::OnSetFloat(int cmd, const float *f)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoDoubleNumberText))
	{
		TPseudoDoubleNumberText *num = (TPseudoDoubleNumberText *)view;
		num->SetValue(*f, true);
	}
#endif
#if qMacPP
	USemaphore policeman(gTurnOffBroadcast);

	LPane *view = (LPane *)w;
	LEditText *edit;
	LStaticText *stat;
	if((edit = dynamic_cast<LEditText *>(view)) != 0L)
	{
		LStr255 pstr((long double)*f, "\p#####.##");
		edit->SetText(pstr);
	}
	else if((stat = dynamic_cast<LStaticText *>(view)) != 0L)
	{
		LStr255 pstr((long double)*f, "\p#####.##");
		stat->SetText(pstr);
	}
#endif
#ifdef WIN32
	USemaphore policeman(m_noDispatch);
	CWnd *wnd = (CWnd *)w;

	char tmp[100];
	sprintf(tmp, "%f", *f);
	wnd->SetWindowText(tmp);
#endif
}

void UWidget::OnEnableCmd(int cmd, int state)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacPP
	LPane *view = (LPane *)w;
	if(state)
		view->Enable();
	else
		view->Disable();
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TView))
	{
		view->SetActiveState(state != 0, true);
	}
#endif
#if qGTK
	if(GTK_IS_WIDGET(w))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(w), state != 0);
	}
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	wnd->EnableWindow(state != 0);
#endif
}

bool UWidget::IsEnabled(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return false;

#ifdef qGTK	
	return GTK_WIDGET_SENSITIVE(GTK_WIDGET(w));
#else
	return true;
#endif

}

void UWidget::OnChgFocus(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacPP
	LPane *pane = (LPane *)w;
	LCommander *target = dynamic_cast<LCommander *>(pane);
	if(target != 0L)
	{
		LCommander::SwitchTarget(target);
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TView))
	{
		if(view->GetWindow() != 0L)
			view->GetWindow()->SetWindowTarget(view);
	}
#endif
#if qGTK
	if(GTK_IS_WIDGET(w))
	{
		UEventNotifyMessage(EV_UPDATEFOCUS, 0, GTK_OBJECT(w));
	}
	
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	wnd->SetFocus();
#endif
}

void UWidget::OnSetDefault(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacPP
#endif
#if qMacAPP
#endif
#if qGTK
	if(GTK_IS_WIDGET(w))
	{
		gtk_widget_grab_default(GTK_WIDGET(w));
	}
#endif
#ifdef WIN32
#endif
}
void UWidget::OnInvalidate(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TView))
	{
		view->ForceRedraw();
	}
#endif
#if qMacPP
	LPane *pane = (LPane *)w;
	pane->Refresh();
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	wnd->Invalidate();
#endif
}

void UWidget::OnRedraw(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TView))
	{
		view->ForceRedraw();
	}
#endif
#if qMacPP
	LPane *pane = (LPane *)w;
	pane->Draw(nil);
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	wnd->Invalidate();
#endif
}


void UWidget::OnSetIcon(int cmd, void *p)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef qUnix
	if(GTK_IS_PIXMAP(w))
	{
		GtkPixmap *widget = GTK_PIXMAP(w);
		GtkPixmap *pix = GTK_PIXMAP(p);
		gtk_pixmap_set(widget, pix->pixmap, pix->mask);
	}
#endif
}

void UWidget::OnSetText(int cmd, const char *text)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	USemaphore policeman(m_noDispatch);
	CWnd *wnd = (CWnd *)w;
	wnd->SetWindowText(text);
#endif
#if qGTK
	if(GTK_IS_WINDOW(w))
	{
		gtk_window_set_title(GTK_WINDOW(w), text);
	}
	else if(GTK_IS_BUTTON(w))
	{
		GtkLabel *label = getLabelFromContainer(GTK_CONTAINER(w));
		if(label != 0L)
			gtk_label_set_text(label, text);
		else
			g_warning("Cannot find the label of '%s'\n", gtk_widget_get_name(GTK_WIDGET(w)));
	}
	else if(GTK_IS_TEXT(w))
	{
		GtkText *t = GTK_TEXT(w);
		gtk_text_set_point(t, 0);
		gtk_text_forward_delete(t, gtk_text_get_length(t));
		gtk_text_insert(t, 0L, 0L, 0L, text, -1);
	}
	else if(GTK_IS_ENTRY(w))
	{
		gtk_entry_set_text(GTK_ENTRY(w), text);
	}
	else if(GTK_IS_COMBO(w))
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(w)->entry), text);
	}
	else if(GTK_IS_LABEL(w))
	{
		GtkLabel *t = GTK_LABEL(w);
		gtk_label_set_text(t, text);
	}
#endif
#if qMacPP
	USemaphore policeman(gTurnOffBroadcast);

	LPane *view = (LPane *)w;
	LTextEditView *txt;
	if((txt = dynamic_cast<LTextEditView *>(view)) != 0L)
	{
		txt->SetTextPtr((Ptr)text, strlen(text));	
	}
	else
	{
		LStr255 pstr(text);
		LStr255 oldstr;
		view->GetDescriptor(oldstr);	
		if(oldstr != pstr)
			view->SetDescriptor(pstr);
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TCtlMgr))
	{
		TCtlMgr *ctrl = (TCtlMgr *)view;
		CStr255_AC pstr(text);
		ctrl->SetText(pstr, true);
	}
	else if(MA_MEMBER(view, TStaticText))
	{
		TStaticText *ctrl = (TStaticText *)view;
		CStr255_AC pstr(text);
		ctrl->SetText(pstr, true);
	}
	else if(MA_MEMBER(view, TWindow))
	{
		TWindow *ctrl = (TWindow *)view;
		CStr255_AC pstr(text);
		ctrl->SetTitle(pstr);
	}
#endif
}

void UWidget::OnGetText(int cmd, std::string* text)
{
	*text = ""; // sanity

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	CString s;
	wnd->GetWindowText(s);
	*text = s;
#endif
#if qGTK
	if(GTK_IS_WINDOW(w))
	{
		*text = GTK_WINDOW(w)->title;
	}
	else if(GTK_IS_BUTTON(w))
	{
		GtkLabel *label = getLabelFromContainer(GTK_CONTAINER(w));
		if(label != 0L)
		{
			gchar *str;
			gtk_label_get(label, &str);

 			if(str != 0L)
                                *text = str;
		}
		else
			g_warning("Cannot find the label of '%s'\n", gtk_widget_get_name(GTK_WIDGET(w)));
	}
	else if(GTK_IS_ENTRY(w))
	{
		*text = gtk_entry_get_text(GTK_ENTRY(w));
	}
	else if(GTK_IS_COMBO(w))
	{
		*text = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(w)->entry));
	}
	else if(GTK_IS_TEXT(w))
	{
		GtkText *t = GTK_TEXT(w);
		size_t size = gtk_text_get_length(t);
		size_t idx;

		for (idx = 0; idx < size; idx++) {
			*text += (char)GTK_TEXT_INDEX(t,idx);
		}
	}
	else if(GTK_IS_LABEL(w))
	{
		GtkLabel *t = GTK_LABEL(w);
		gchar *str;
		gtk_label_get(t, &str);
		if(str != 0L)
			*text = str;
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LTextEditView *txt;
	if((txt = dynamic_cast<LTextEditView *>(view)) != 0L)
	{
		LStr255 pstr;
		Handle hdl = txt->GetTextHandle();
		text->set((const char *)*hdl, ::GetHandleSize(hdl));
	}
	else
	{
		LStr255 pstr;
		view->GetDescriptor(pstr);	
		*text = pstr;
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TCtlMgr))
	{
		TCtlMgr *ctrl = (TCtlMgr *)view;
		CStr255_AC pstr = ctrl->GetText();
		*text = pstr;
	}
	else if(MA_MEMBER(view, TStaticText))
	{
		TStaticText *ctrl = (TStaticText *)view;
		CStr255_AC pstr = ctrl->GetText();
		*text = pstr;
	}
	else if(MA_MEMBER(view, TWindow))
	{
		TWindow *ctrl = (TWindow *)view;
		CStr255_AC pstr = ctrl->GetTitle();
		*text = pstr;
	}
#endif
}

void UWidget::OnShowWidget(int cmd, int state)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	wnd->ShowWindow(state ? SW_SHOWNORMAL : SW_HIDE);
#endif
#if qGTK
	GtkWidget *wid = (GtkWidget *)w;
	if(state)
		gtk_widget_show(wid);
	else
		gtk_widget_hide(wid);
#endif
#if qMacAPP
	TView *view = (TView *)w;
	view->Show(state != 0, true);
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	if(state)
		view->Show();
	else
		view->Hide();
#endif
}


//{ functions for slider
void UWidget::OnSetSlider(int cmd, int pos)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CSliderCtrl)))
	{
		CSliderCtrl *slider = (CSliderCtrl *)wnd;
		slider->SetPos(pos);
	}
	else if(wnd->IsKindOf(RUNTIME_CLASS(CScrollBar)))
	{
		CScrollBar *scroll = (CScrollBar *)wnd;
		scroll->SetScrollPos(pos);
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TSlider) || MA_MEMBER(view, TScrollBar))
	{
		TCtlMgr *ctrl = (TCtlMgr *)view;
		ctrl->SetValue(pos, true);
	}
#endif
#if qMacPP
	USemaphore policeman(gTurnOffBroadcast);

	LPane *view = (LPane *)w;
	LControl *ctrl;
	if((ctrl = dynamic_cast<LControl *>(view)) != 0L)
	{
		ctrl->SetValue(pos);
	}
#endif
}


void UWidget::OnGetSliderProperty(int cmd, USLIDER *prop)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CSliderCtrl)))
	{
		CSliderCtrl *slider = (CSliderCtrl *)wnd;
		prop->max = slider->GetRangeMax();
		prop->min = slider->GetRangeMin();
		prop->inc = slider->GetPageSize();
		prop->pos = slider->GetPos();
	}
	else if(wnd->IsKindOf(RUNTIME_CLASS(CScrollBar)))
	{
		SCROLLINFO info;
		CScrollBar *scroll = (CScrollBar *)wnd;
		VERIFY(scroll->GetScrollInfo(&info));
		prop->max = info.nMax;
		prop->min = info.nMin;
		prop->inc = info.nPage;
		prop->pos = info.nPos;
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TSlider) || MA_MEMBER(view, TScrollBar))
	{
		TCtlMgr *ctrl = (TCtlMgr *)view;
		prop->max = ctrl->GetMaximum();
		prop->min = ctrl->GetMinimum();
		if(MA_MEMBER(ctrl, TPseudoScrollBar))
			prop->inc = ((TPseudoScrollBar *)ctrl)->fPageStep;
		else
			prop->inc = 1;
		prop->pos = ctrl->GetValue();
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LControl *ctrl;
	if((ctrl = dynamic_cast<LControl *>(view)) != 0L)
	{
		prop->max = ctrl->GetMaxValue();
		prop->min = ctrl->GetMinValue();
		LPseudoWinAttachment *attach = macpp_getattachwindow(this);
		prop->inc = attach != 0L ? attach->GetPageValue(cmd) : 1;
		prop->pos = ctrl->GetValue();
	}
#endif
}

void UWidget::OnSetSliderProperty(int cmd, const USLIDER *prop)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CSliderCtrl)))
	{
		CSliderCtrl *slider = (CSliderCtrl *)wnd;
		slider->SetRange(prop->min, prop->max);
		slider->SetPos(prop->pos);
		slider->SetPageSize(prop->inc);
	}
	else if(wnd->IsKindOf(RUNTIME_CLASS(CScrollBar)))
	{
		SCROLLINFO info;
		memset(&info, 0, sizeof(info));
		info.cbSize = sizeof(info);
		info.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
		info.nMax = prop->max;
		info.nMin = prop->min;
		info.nPos = prop->pos;
		info.nPage = prop->inc;
		if(info.nPage > (UINT)(info.nMax - info.nMin))
			info.nPage = (UINT)(info.nMax - info.nMin) / 2;
		CScrollBar *scroll = (CScrollBar *)wnd;
		VERIFY(scroll->SetScrollInfo(&info));
		if(prop->max != prop->min)
			scroll->EnableWindow(TRUE);
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TSlider) || MA_MEMBER(view, TScrollBar))
	{
		TCtlMgr *ctrl = (TCtlMgr *)view;
		ctrl->SetValues(prop->pos, prop->min, prop->max, true);
		if(MA_MEMBER(ctrl, TPseudoScrollBar))
			((TPseudoScrollBar *)ctrl)->fPageStep = prop->inc;
	}
#endif
#if qMacPP
	USemaphore policeman(gTurnOffBroadcast);

	LPane *view = (LPane *)w;
	LControl *ctrl;
	if((ctrl = dynamic_cast<LControl *>(view)) != 0L)
	{
		ctrl->SetMaxValue(prop->max);
		ctrl->SetMinValue(prop->min);
		ctrl->SetValue(prop->pos);
		LPseudoWinAttachment *attach = macpp_getattachwindow(this);
		if(attach != 0L)
			attach->SetPageValue(cmd, prop->inc);
	}
#endif
}

//}
//{ functions for timer
void UWidget::AddTimer(long lap, int cmd)
{
	UTimer timer(lap, ::UTickCount(), cmd);
	m_timers.push_back(timer);
}

void UWidget::RemoveTimer(int cmd)
{
	vector<UTimer>::iterator i;
	for(i = m_timers.begin(); i != m_timers.end(); i++)
	{
		if((*i).m_cmd == cmd)
		{
			(*i).m_cmd = 0; // delay removing
			break;
		}
	}
}

std::vector<UTimer> & UWidget::GetTimers(void)
{
	// removed the delayed items
	vector<UTimer>::iterator i;
	for(i = m_timers.begin(); i != m_timers.end(); ++i)
	{
		if((*i).m_cmd == 0)
		{
			m_timers.erase(i);
			break;
		}
	}

	return m_timers;
}

//}
void UWidget::OnUpdateUI()
{
	map<int, void *>::const_iterator i;
	for(i = m_widgets.begin(); i != m_widgets.end(); ++i)
	{
		UCmdUI pUI((*i).first);
		pUI.m_widget = (*i).second;
		UEventSendMessage(GetWidID(), EV_UPDTCMD, pUI.m_nID, &pUI);
	}
}

int UWidget::DoModal(int id)
{
	bool result = false;

#if !defined(qMacAPP) && !defined(qMacPP) && !defined(WIN32)
	void *w = GetWidget(kUMainWidget);
	if(w == 0L)
	{
		UAppConsole("Unable to find widget id : %d (file %s:%d)\n", kUMainWidget, __FILE__, __LINE__);
		return result;
	}
#endif // !qMacAPP && !WIN32

#if qGTK || qMacPP
	if(m_modalLoop)
	{
		UAppConsole("Trying to break a modal state\n");
		return result;
	}
#endif

#ifdef WIN32
	CPseudoDlg dlg(id, this);
	m_curModal = &dlg;
	result = dlg.DoModal() == IDOK;
	if(!result)
		Dismiss();
	m_curModal = 0L;
#endif
#if qMacAPP
	TWindow *view = gViewServer->NewTemplateWindow(id, nil);
	ThrowIfNULL_AC(view);
	
	UEventSendMessage(GetWidID(), EV_INIT_WIDGET, kUMainWidget, view);

	DoDataExchange(true);
	IDType resid = view->PoseModally();

	TDialogBehavior* dlgb;
	if((dlgb = view->GetDialogBehavior()) != 0L)
	{
		result = resid == dlgb->fDefaultItem;
	}

	if(result)
		DoDataExchange(false);
	else
		Dismiss();

	view->Close();		
#endif
#if qMacPP
	StDialogHandler	theHandler(id, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	
	if(theDialog != 0L)
	{
		UEventSendMessage(GetWidID(), EV_INIT_WIDGET, kUMainWidget, theDialog);

		DoDataExchange(true);
		theDialog->Show();
		theHandler.SetSleepTime(6);
		
		m_modalLoop = true;
		MessageT hitMessage;
		while (m_modalLoop)
		{		// Let DialogHandler process events
			hitMessage = theHandler.DoDialog();
			
			if (hitMessage == msg_OK || hitMessage == msg_Cancel)
				break;
		}
		
		result = m_modalLoop ? hitMessage == msg_OK : m_modalRes;

		if(result)
			DoDataExchange(false);
		else
			Dismiss();

		//theDialog->Hide();
		//theDialog->DoClose();
	}	
#endif
#if qGTK
	if(GTK_IS_WINDOW(w))
	{
		// emulate the click in tab 1
		map<int, UWidget *>::iterator i;
		for(i = m_tabs.begin(); i != m_tabs.end(); ++i)
		{
			int cmdid = ULOWORD((*i).first);
			int pagenum = UHIWORD((*i).first);
			if(pagenum == 0)
			{
				UEventSendMessage(GetWidID(), EV_PAGE_CHANGED, UMAKEINT(cmdid, pagenum), 0L);
				break;
			}
		}
		gtk_signal_connect(GTK_OBJECT(w),"delete_event", GTK_SIGNAL_FUNC(gtkcancel), this);
		DoDataExchange(true);
		gtk_widget_show(GTK_WIDGET(w));
		m_modalLoop = true;
		gtk_grab_add (GTK_WIDGET(w));
		while (m_modalLoop)
		{
			if( gtk_main_iteration_do(TRUE))
				break;
		}
		gtk_grab_remove (GTK_WIDGET(w));
		result = m_modalRes;
		m_modalLoop = false; // sanity...
		if(result)
		{
			UEventSendMessage(GetWidID(), EV_PAGE_CHANGED, UMAKEINT(0, -1), 0L);
			DoDataExchange(false);
		}
		else 
		{
			Dismiss();
		}
		gtk_widget_destroy(GTK_WIDGET(w));
	}
	else
		g_warning("Trying to modal on a non-window\n");
#endif

	return result;
}

void UWidget::EndDialog(int result)
{
	m_modalRes = result;
#if qGTK || qMacPP
	if(!m_modalLoop)
		UAppConsole("EndModal called twice\n");

	m_modalLoop = false;
#endif
#if qMacAPP
	void *w = GetWidget(kUMainWidget);
	if(w == 0L)
		return;
	
	TWindow *wind = (TWindow *)w;
	TDialogBehavior* dlgb;
	if(MA_MEMBER(wind, TWindow) && (dlgb = wind->GetDialogBehavior()) != 0L)
	{
		wind->Dismiss(result ? dlgb->fDefaultItem : dlgb->fCancelItem, result);
	}
#endif
#ifdef WIN32
	if(m_curModal != 0L)
	{
		if (result && !m_curModal->UpdateData(TRUE))
		{
			TRACE0("UpdateData failed during dialog termination.\n");
			// the UpdateData routine will set focus to correct item
			return;
		}
		m_curModal->EndDialog(result ? IDOK : IDCANCEL);
	}
#endif
}

void UWidget::EndModal(bool result)
{
	EndDialog((int)result);
}

void UWidget::OnChgCursor(int cmd, UCURSOR cursor)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(GTK_IS_WIDGET(w) && GTK_WIDGET(w)->window != 0L)
	{
		static GdkCursor *cursor_arrow=0L;
		static GdkCursor *cursor_watch=0L;
		if(cursor_arrow == 0L)
		{
			cursor_arrow = gdk_cursor_new (GDK_TOP_LEFT_ARROW);
			cursor_watch = gdk_cursor_new (GDK_WATCH);
		}
		if(cursor == uCursArrow)
			gdk_window_set_cursor (GTK_WIDGET(w)->window, cursor_arrow);
		else if(cursor == uCursWatch)
			gdk_window_set_cursor (GTK_WIDGET(w)->window, cursor_watch);
	}
#endif
}

//{ functions for tree
void UWidget::OnTreeResetAll(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_TREE(w))
		return;

	GtkTree *tree = GTK_TREE(w);
	GList *list = g_list_copy(tree->children);
	GList *head = list;
	while(list != 0L)
	{
		GtkWidget *wid = GTK_WIDGET(list->data);
		gtk_container_remove(GTK_CONTAINER(tree), wid);
		list = (GList *)g_list_next(list);
	}
	g_list_free(head);
#endif
}

void UWidget::OnTreeInsert(int cmd, UTREE_INSERT *data)
{
	data->result = 0L; // sanity

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
  if(!GTK_IS_TREE(w))
		return;

	GtkTree *root;
	if(data->parent == 0L)
		root = GTK_TREE(w);
	else
	{
		GtkTreeItem *item = GTK_TREE_ITEM(data->parent);
		
		if(item->subtree != 0L) {
			root = GTK_TREE(item->subtree);
		}
		else
		{
			root = GTK_TREE(gtk_tree_new());
			gtk_object_set_data(GTK_OBJECT(root), "rf_treeparent", item);
			gtk_tree_set_selection_mode (root, GTK_SELECTION_BROWSE); // TODO : find out it with the root
			gtk_tree_set_view_mode (root, GTK_TREE_VIEW_ITEM); // TODO : find out it with the root
			gtk_tree_item_set_subtree (item, GTK_WIDGET(root));
		}
	}
  
  GtkTreeItem *item = gtknewtreeitem(data->title, data->icon == 0L ? 0L : GTK_PIXMAP(data->icon));
	gtk_object_set_data(GTK_OBJECT(item), "rf_treeitemdata", data->data);
	gtk_signal_connect(GTK_OBJECT(item), "select",
					   GTK_SIGNAL_FUNC(gtktreeselect), (gpointer)UMAKEINT(GetWidID(), cmd));
	gtk_signal_connect(GTK_OBJECT(item), "deselect",
					   GTK_SIGNAL_FUNC(gtktreedeselect), (gpointer)UMAKEINT(GetWidID(), cmd));
	gtk_signal_connect (GTK_OBJECT(item), "expand",
						GTK_SIGNAL_FUNC(gtktreeexpand), (gpointer)UMAKEINT(GetWidID(), cmd));
	gtk_signal_connect (GTK_OBJECT(item), "collapse",
						GTK_SIGNAL_FUNC(gtktreecollapse), (gpointer)UMAKEINT(GetWidID(), cmd));

	gtk_tree_append(GTK_TREE(root), GTK_WIDGET(item));
  gtk_widget_show (GTK_WIDGET(item));

 	data->result = (UTREEITEM)item;
#endif
}

void UWidget::OnTreeSetData(int cmd, UTREE_INFO *data)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_TREE(w))
		return;
  
	gtk_object_set_data(GTK_OBJECT(data->item), "rf_treeitemdata", data->data);
#endif
}

void UWidget::OnTreeSetIcon(int cmd, UTREE_INFO *data)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_TREE(w))
		return;

	gtksettreeitemicon(GTK_TREE_ITEM(data->item), GTK_PIXMAP(data->icon));
#endif
}

void UWidget::OnTreeGetSel(int cmd, UTREEITEM *item)
{
	*item = 0L; // sanity

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_TREE(w))
		return;

	GList *sel = GTK_TREE(w)->selection;
	if(sel == 0L || !GTK_IS_TREE_ITEM(sel->data))
		return;

	*item = sel->data;
#endif
}

void UWidget::OnTreeGetInfo(int cmd, UTREE_INFO *desc)
{
	// sanity
	desc->parent = 0L;
	desc->next = 0L;
	desc->child = 0L;
	desc->data = 0L;
	desc->title = 0L;
	desc->icon = 0L;
	desc->expanded = 0;

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_TREE(w))
		return;

	GtkTreeItem *item;
	if(desc->item == 0L)
	{
		GList *list = g_list_first(GTK_TREE(w)->children);
		GList *next = g_list_next(list);
		item = list != 0L ? GTK_TREE_ITEM(list->data) : 0L;
		desc->item = item;
		desc->next = next != 0L ? GTK_TREE_ITEM(next->data) : 0L;
	}
	else
	{
		GtkTree *parent = GTK_TREE(GTK_WIDGET(desc->item)->parent);
		GList *list = g_list_find(GTK_TREE(parent)->children, desc->item);
		GList *next = g_list_next(list);
		item = list != 0L ? GTK_TREE_ITEM(list->data) : 0L;
		desc->item = item;
		// the only way I found to guess the TREE_ITEM parent
		desc->parent = parent == w ? 0L : GTK_TREE_ITEM(gtk_object_get_data(GTK_OBJECT(parent), "rf_treeparent"));
		g_assert(desc->parent == 0L || GTK_IS_TREE_ITEM(desc->parent));
		desc->next = next != 0L ? GTK_TREE_ITEM(next->data) : 0L;
	}
	if(item != 0L)
	{
		if(item->subtree != 0L)
		{
			GtkTree *subtree = GTK_TREE(item->subtree);
			GList *list = g_list_first(GTK_TREE(subtree)->children);
			desc->child = list != 0L ? GTK_TREE_ITEM(list->data) : 0L;
		}
		desc->data = gtk_object_get_data(GTK_OBJECT(item), "rf_treeitemdata");
		GtkLabel *label = gtktreegetlabel(GTK_WIDGET(item));
		if(label != 0L)
			gtk_label_get(label, &desc->title);
		desc->expanded = item->expanded;
	}
#endif
}

void UWidget::OnTreeDelete(int cmd, UTREEITEM item)
{
	// sanity
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_TREE(w) || item == 0L)
		return;

	GtkTreeItem *i = GTK_TREE_ITEM(item);
	
	GtkTree *parent = GTK_TREE(GTK_WIDGET(i)->parent);
	if (parent == 0L)
		return;

	gtk_container_remove(GTK_CONTAINER(parent), GTK_WIDGET(i));
#endif
}

void UWidget::OnTreeExpand(int cmd, int collapse, UTREEITEM item)
{
	// sanity
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
  if(!GTK_IS_TREE(w) || item == 0L)
		return;

  GtkTreeItem *i = GTK_TREE_ITEM(item);
  
  gtktreesort(GTK_TREE(i->subtree));
  
	int collapsed = !i->expanded;
	if((collapsed != 0) ^ (collapse != 0))
	{
		if(!collapse)
			gtk_tree_item_expand(i);
		else
			gtk_tree_item_collapse(i);
	}
#endif
}

void UWidget::OnTreeSelect(int cmd, int deselect, UTREEITEM item)
{
	// sanity
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_TREE(w) || item == 0L)
		return;

	GList *sel = GTK_TREE(w)->selection;
	while(sel)
	{
		gtk_tree_item_deselect(GTK_TREE_ITEM(sel->data));
		sel = g_list_next(sel);
	}

	GtkWidget *i = GTK_WIDGET(item);
	if(!deselect)
		gtk_tree_select_child(GTK_TREE(w), i);
	else
		gtk_tree_unselect_child(GTK_TREE(w), i);
#endif
}

//}
//{ functions for comboboxes
void UWidget::OnComboResetAll(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		CComboBox *combo = (CComboBox *)wnd;
		combo->ResetContent();
	}
#endif
#if qGTK
	if(GTK_IS_COMBO(w))
	{
		GtkList *listbox = GTK_LIST( GTK_COMBO(w)->list );
		if(listbox && listbox->children != 0L)
			gtk_list_remove_items( listbox, g_list_copy(listbox->children));
	}
	else if(GTK_IS_OPTION_MENU(w))
	{
		GtkWidget *menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(w));
		GList *list = g_list_copy(GTK_MENU_SHELL(menu)->children);
		GList *head = list;
		while(list != 0L)
		{
			GtkWidget *wid = GTK_WIDGET(list->data);
			gtk_container_remove(GTK_CONTAINER(menu), wid);
			list = (GList *)g_list_next(list);
		}
		g_list_free(head);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		USemaphore policeman(gTurnOffBroadcast);

		popup->DeleteAllMenuItems();
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPopup))
	{
		TPopup *popup = (TPopup *)view;
		while(popup->GetNumberOfItems() > 0)
			popup->DeleteItem(1);
	}
#endif
}

void UWidget::OnComboAppend(int cmd, const char *txt)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		CComboBox *combo = (CComboBox *)wnd;
		combo->InsertString(-1, txt);
	}
#endif
#if qGTK
	if(GTK_IS_COMBO(w))
	{
		GtkWidget *list = GTK_COMBO(w)->list;
		GtkWidget *list_item = gtk_list_item_new_with_label(txt);
		gtk_container_add( GTK_CONTAINER(list), list_item );
		gtk_widget_show( list_item );
		
		gtk_signal_connect( GTK_OBJECT(list_item), "select",
							GTK_SIGNAL_FUNC(gtkcomboclicked), (gpointer)UMAKEINT(GetWidID(), cmd) );
	}
	else if(GTK_IS_OPTION_MENU(w))
	{
		GtkWidget *menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(w));
		GtkWidget *item = gtk_menu_item_new_with_label(txt);
		// fix me: where to find the label of the item ?
		gtk_object_set_data_full(GTK_OBJECT(item), "rf_menuitemtext", strdup(txt), (GtkDestroyNotify)free);
		gtk_signal_connect (GTK_OBJECT(item), "activate",
			GTK_SIGNAL_FUNC(gtkmenuactivated), (gpointer)UMAKEINT(GetWidID(), cmd));
		gtk_widget_show(item);
		gtk_menu_append(GTK_MENU(menu), item);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		USemaphore policeman(gTurnOffBroadcast);

		if(strcmp(txt, "-") == 0)
			popup->InsertMenuItem("\p(-", max_Int16, false);
		else
			popup->InsertMenuItem(LStr255(txt), max_Int16);
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPopup))
	{
		TPopup *popup = (TPopup *)view;
		if(strcmp(txt, "-") == 0)
			popup->AddSeparator();
		else
			popup->AddItem(CStr255_AC(txt));
	}
#endif
}

void UWidget::OnComboSetPos(int cmd, int sel)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		CComboBox *combo = (CComboBox *)wnd;
		combo->SetCurSel(sel);
	}
#endif
#if qGTK
	if(GTK_IS_COMBO(w))
	{
		GtkWidget *list = GTK_COMBO(w)->list;
		gtk_list_select_item(GTK_LIST(list), sel);
	}
	else if(GTK_IS_OPTION_MENU(w))
	{
		gtk_option_menu_set_history (GTK_OPTION_MENU(w), sel);
	}

#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPopup))
	{
		TPopup *popup = (TPopup *)view;
		popup->SetCurrentItem(sel + 1, true);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		USemaphore policeman(gTurnOffBroadcast);

		popup->SetCurrentMenuItem(sel + 1);
	}
#endif
}

void UWidget::OnComboDelSel(int cmd, int sel)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		CComboBox *combo = (CComboBox *)wnd;
		combo->DeleteString(sel);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		USemaphore policeman(gTurnOffBroadcast);

		popup->DeleteMenuItem(sel + 1);
	}
#endif
}

void UWidget::OnComboEnableSel(int cmd, int sel, int state)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		if(state)
			::EnableMenuItem(popup->GetMacMenuH(), sel + 1);
		else
			::DisableMenuItem(popup->GetMacMenuH(), sel + 1);
	}
#endif
}

void UWidget::OnComboGetMenu(int cmd, UMENU *menu)
{
	*menu = UMENU_NULL; // sanity
	
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		MenuHandle hdl = popup->GetMacMenuH();
		if(hdl != 0L)
#	if ACCESSOR_CALLS_ARE_FUNCTIONS
			*menu = GetMenuID(hdl);
#	else
			*menu = (**hdl).menuID;
#	endif
	}
#endif
}

int UWidget::OnComboGetPos(int cmd, std::string* txt)
{
	int res = -1; // sanity
	
	if( txt != 0L )
		*txt = "";
	
	void *w = GetWidget(cmd);
	if(w == 0L)
		return res;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		CComboBox *combo = (CComboBox *)wnd;
		res = combo->GetCurSel();
		if(res != -1 && txt != 0L)
		{
			CString s;
			combo->GetLBText(res, s);
			*txt = s;
		}
	}
#endif
#ifdef qGTK
	if(GTK_IS_COMBO(w))
	{
		GtkList *list = (GtkList*)GTK_COMBO(w)->list;
		GtkWidget *data = (GtkWidget*) list->selection->data;
		
		res = gtk_list_child_position(GTK_LIST(list), (GtkWidget*)data);
	}
	else if(GTK_IS_OPTION_MENU(w))
	{
		
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPopup))
	{
		TPopup *popup = (TPopup *)view;
		res = popup->GetCurrentItem() - 1;
		if(txt != 0L)
		{
			CStr255_AC pstr = popup->GetItemText(res + 1);
			*txt = pstr;
		}
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		res = popup->GetCurrentMenuItem() - 1;
		if(txt != 0L)
		{
			LStr255 pstr;
			popup->GetMenuItemText(res + 1, pstr);
			*txt = pstr;
		}
	}
#endif

	return res;
}



int UWidget::OnComboGetCount(int cmd)
{
	int res = 0; // sanity
	
	void *w = GetWidget(cmd);
	if(w == 0L)
		return res;

#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		CComboBox *combo = (CComboBox *)wnd;
		res = combo->GetCount();
	}
#endif
#if qGTK
	if(GTK_IS_COMBO(w))
	{
		GtkList *list = GTK_LIST(GTK_COMBO(w)->list);
		if ( list) 
		{
			GList *tmp_list;
			for( tmp_list = list->children; tmp_list; tmp_list = tmp_list->next)
				++res;
		}
	}
	else if(GTK_IS_OPTION_MENU(w))
	{
		GtkMenu *mm = GTK_MENU(gtk_option_menu_get_menu( GTK_OPTION_MENU(w)));
		if ( mm) 
		{ 
			GList *tmp_list;
			for( tmp_list = mm->menu_shell.children; tmp_list; tmp_list = tmp_list->next)
				++res;
		}
	}

#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPopup))
	{
		TPopup *popup = (TPopup *)view;
		res = popup->GetNumberOfItems();
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LMenuController *popup;
	if((popup = dynamic_cast<LMenuController *>(view)) != 0L)
	{
		res = ::CountMenuItems(popup->GetMacMenuH());
	}
#endif

	return res;
}

void UWidget::OnSmartComboSetFeature(int cmd, int feature)
{
	void *w = GetWidget(cmd);
	if( w == 0L )
		return;

#ifdef WIN32
	CWnd* wnd = (CWnd*)w;

	if( wnd->IsKindOf(RUNTIME_CLASS(CComboBox)) )
	{
		CComboBox* combo = (CComboBox*)wnd;
		combo->SendMessage(WM_SMCB_SETFEATURE, WPARAM(feature));
	}
#endif
#if qMacPP
#endif
}

void UWidget::OnSmartComboSetItems(int cmd, CMString* items)
{
	void *w = GetWidget(cmd);
	if( w == 0L )
		return;

#ifdef WIN32
	CWnd* wnd = (CWnd*)w;

	if( wnd->IsKindOf(RUNTIME_CLASS(CComboBox)) )
	{
		CComboBox* combo = (CComboBox*)wnd;
		combo->SendMessage(WM_SMCB_SETITEMS, WPARAM(items));
	}
#endif
#if qMacPP
#endif
}

void UWidget::OnSmartComboSetReadOnly(int cmd, bool readOnly)
{
	void *w = GetWidget(cmd);
	if( w == 0L )
		return;

#ifdef WIN32
	CWnd* wnd = (CWnd*)w;

	if( wnd->IsKindOf(RUNTIME_CLASS(CComboBox)) )
	{
		CComboBox* combo = (CComboBox*)wnd;
		combo->SendMessage(WM_SMCB_SETREADONLY, WPARAM(readOnly));
	}
#endif
#if qMacPP
#endif
}

//}
//{ functions for lists
void UWidget::OnListHideColumns(int cmd, int num)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

	GtkCList *list = GTK_CLIST(w);
	
	// one must always be visible. We use the first one
	for(int i=1; i< num; i++)
		gtk_clist_set_column_visibility (list, i, false);
#endif
}

void UWidget::OnListShowColumns(int cmd, int num)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

	GtkCList *list = GTK_CLIST(w);
	
	// one must always be visible. We use the first one
	for(int i=1; i< num; i++)
		gtk_clist_set_column_visibility (list, i, true);
#endif
}

void UWidget::OnListAddColumns(int cmd, int num)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;
 
	//GtkCList *list = GTK_CLIST(w);
		// don't know how to do that, for now it relies on glade settings

#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		DWORD dwExStyle = ListView_GetExtendedListViewStyle(list->m_hWnd);
		dwExStyle |= LVS_EX_FULLROWSELECT;
		ListView_SetExtendedListViewStyle(list->m_hWnd, dwExStyle);

		while(num--)
			list->InsertColumn(0, "", LVCFMT_LEFT, 80);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	LTextTableView *tabtext;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		macpp_preparetable(table);
		table->InsertCols( num, 0, nil, nil, false );
	}
	else if((tabtext = dynamic_cast<LTextTableView *>(view)) != 0L)
	{
		macpp_preparetable(GetWidget(kUMainWidget), tabtext);
		tabtext->InsertCols( num, 0, nil, nil, false );
	}
#endif
}

void UWidget::OnListSetColTitle(int cmd, int num, const char *title)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	gtk_clist_set_column_title (list, num, title);
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		LVCOLUMN col;
		memset(&col, 0, sizeof(col));
		col.mask = LVCF_TEXT;
		col.pszText = (char *)title;
		VERIFY(list->SetColumn(num, &col));
	}
#endif
}

void UWidget::OnListSetColWidth(int cmd, int num, int width)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	gtk_clist_set_column_width(list, num, width);
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		LVCOLUMN col;
		memset(&col, 0, sizeof(col));
		col.mask = LVCF_WIDTH;
		col.cx = width;
		VERIFY(list->SetColumn(num, &col));
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	LTextTableView *tabtext;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		macpp_preparetable(table);
		LTableGeometry *geom = table->GetTableGeometry();
		if(geom != 0L)
			geom->SetColWidth(width, num + 1, num + 1);
	}
	else if((tabtext = dynamic_cast<LTextTableView *>(view)) != 0L)
	{
		macpp_preparetable(GetWidget(kUMainWidget), tabtext);
		LTableGeometry *geom = tabtext->GetTableGeometry();
		if(geom != 0L)
			geom->SetColWidth(width, num + 1, num + 1);
	}
#endif
}

void UWidget::OnListResetAll(int cmd)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	gtk_clist_clear(list);
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoTextListView))
	{
		TPseudoTextListView *txt = (TPseudoTextListView *)view;
		txt->RemoveAll();
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	LTextTableView *tabtext;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		macpp_preparetable(table);
		LArrayIterator iter(table->GetFirstLevelItems());
		LOutlineItem* item;
		while (iter.Next(&item))
		{
			table->RemoveItem(item, false, false);
		}
		table->AdjustImageSize(false);
		table->Refresh();
	}
	else if((tabtext = dynamic_cast<LTextTableView *>(view)) != 0L)
	{
		macpp_preparetable(GetWidget(kUMainWidget), tabtext);
		tabtext->RemoveAllRows(false);
		tabtext->Refresh();
	}
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		VERIFY(list->DeleteAllItems());
	}
#endif
}

void UWidget::OnListAddSel(int cmd, int entry)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoTextListView))
	{
		TPseudoTextListView *txt = (TPseudoTextListView *)view;
		txt->SelectItem(entry + 1, false, true, true);
	}
#endif
#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	gtk_clist_select_row(list, entry, 0);
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		VERIFY(list->SetItemState(entry, LVIS_SELECTED, LVIS_SELECTED));
		VERIFY(list->EnsureVisible(entry, true));
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LTableView *table;
	if((table = dynamic_cast<LTableView *>(view)) != 0L)
	{
		STableCell cell(entry + 1, 1);
		table->SelectCell(cell);
		table->ScrollCellIntoFrame(cell);
	}
#endif
}

void UWidget::OnListDelRow(int cmd, int entry)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoTextListView))
	{
		TPseudoTextListView *txt = (TPseudoTextListView *)view;
		txt->DelItemAt(entry + 1, 1);
	}
#endif
#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	gtk_clist_remove(list, entry);
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		VERIFY(list->DeleteItem(entry));
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	LTextTableView *tabtext;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		LOutlineItem* item = table->FindItemForRow(entry + 1);
		if(item != 0L)
		{
			table->RemoveItem(item);
		}
	}
	else if((tabtext = dynamic_cast<LTextTableView *>(view)) != 0L)
	{
		tabtext->RemoveRows(1, entry + 1, true);
	}
#endif
}

void UWidget::OnListSetFeedback(int cmd, int turnOn)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	if(turnOn)
		gtk_clist_thaw(list);
	else
		gtk_clist_freeze(list);
#endif
}

void UWidget::OnListNewRow(int cmd, int *num)
{
	*num = 0; // sanity
	
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

	gchar *emptyTxts[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};

    GtkCList *list = GTK_CLIST(w);
	*num = gtk_clist_append  (list, emptyTxts);
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoTextListView))
	{
		TPseudoTextListView *txt = (TPseudoTextListView *)view;
		*num = txt->GetNumOfRows();
	}
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		*num = list->GetItemCount();
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	LTextTableView *tabtext;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		// Store this information in the list view
		CPseudoItem *theItem = NEW CPseudoItem(GetWidID(), cmd);
		if(theItem != 0L)
		{
			TableIndexT outRows, outCols;
			table->GetTableSize(outRows, outCols);
			// and insert it at the end of the table
			LOutlineItem *lastItem = table->FindItemForRow(outRows);
			table->InsertItem( theItem, nil, lastItem );
			*num = outRows;
		}
	}
	else if((tabtext = dynamic_cast<LTextTableView *>(view)) != 0L)
	{
		TableIndexT outRows, outCols;
		tabtext->GetTableSize(outRows, outCols);
		tabtext->InsertRows(1, outRows);
		tabtext->AdjustImageSize(false);
		*num = outRows;
	}
#endif
}

void UWidget::OnListRowSetData(int cmd, int num, void *data)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	gtk_clist_set_row_data(list, num, data);
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		// Store this information in the list view
		LOutlineItem *item = table->FindItemForRow(num + 1);
		CPseudoItem *theItem;
		if((theItem = dynamic_cast<CPseudoItem *>(item)) != 0L)
		{
			theItem->m_data = data;
		}
	}
#endif
}

void UWidget::OnListRowGetData(int cmd, int row, void **data)
{
	*data = 0L; // sanity

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	*data = gtk_clist_get_row_data(list, row);
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		// Store this information in the list view
		LOutlineItem *item = table->FindItemForRow(row + 1);
		CPseudoItem *theItem;
		if((theItem = dynamic_cast<CPseudoItem *>(item)) != 0L)
		{
			*data = theItem->m_data;
		}
	}
#endif
}

int UWidget::OnListGetNextSel(int cmd, int selectedCell)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return -1;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return -1;

	GList *sel = GTK_CLIST(w)->selection;
	if(sel == 0L)
		return -1;

	while(sel != 0L)
	{
		gint row = GPOINTER_TO_INT(sel->data);
		if(selectedCell == -1)
			return row;

		sel = (GList *)g_list_next(sel);

		if(selectedCell == row)
		{
			return sel == 0L ? -1 : GPOINTER_TO_INT(sel->data);
		}
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoTextListView))
	{
		TPseudoTextListView *txt = (TPseudoTextListView *)view;
		if(txt->FirstSelectedItem() == 0)
			return -1;
		
		for(GridCoordinate c = 0; c < txt->GetNumOfRows(); ++c)
		{
			if(!txt->IsItemSelected(c + 1))
				continue;
			
			if(selectedCell == -1)
				return c;

			if(selectedCell == c)
				selectedCell = -1; // so the next pick is the one
		}
	}
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		return list->GetNextItem(selectedCell, LVNI_SELECTED);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LTableView *table;
	if((table = dynamic_cast<LTableView *>(view)) != 0L)
	{
		STableCell sel;
		if(selectedCell == -1)
		{
			sel = table->GetFirstSelectedCell();
			return sel.row - 1;
		}
		
		sel.row = selectedCell + 1;
		sel.col = 1;
		while(table->GetNextSelectedCell(sel))
		{
			if(sel.row != selectedCell + 1)
				return sel.row - 1;
		}
	}
#endif

	return -1;
}

int UWidget::OnListGetNext(int cmd, int whichCell)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return -1;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return -1;

	if(whichCell == -1)
		whichCell = 0;
	else
		whichCell++;

	if(whichCell >= GTK_CLIST(w)->rows)
		whichCell = -1;

	return whichCell;
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		return list->GetNextItem(whichCell, LVNI_ALL);
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LTableView *table;
	if((table = dynamic_cast<LTableView *>(view)) != 0L)
	{
		STableCell sel;

		if(whichCell != -1)
		{
			sel.row = whichCell + 1;
			sel.col = 1;
		}
		
		while(table->GetNextCell(sel))
		{
			if(sel.row != whichCell + 1)
				return sel.row - 1;
		}
	}
#endif

	return -1;
}

void UWidget::OnListInsert(int cmd, ULIST_INSERT *data)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);

	if(data->title == 0L && data->icon == 0L)
		g_warning("Got an empty cell !\n");
	else if(data->title == 0L)
	{
		GtkPixmap *pix = GTK_PIXMAP(data->icon);
		gtk_clist_set_pixmap(list, data->row, data->col, pix->pixmap, pix->mask);
	}
	else if(data->icon == 0L)
	{
		gtk_clist_set_text(list, data->row, data->col, data->title);
	}
	else
	{
		GtkPixmap *pix = GTK_PIXMAP(data->icon);
		gtk_clist_set_pixtext(list, data->row, data->col, data->title, 2, pix->pixmap, pix->mask);
	}
#endif
#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoTextListView))
	{
		TPseudoTextListView *txt = (TPseudoTextListView *)view;
		//txt->InsStringBefore(data->row + 1, data->title);
		txt->InsStringLast(data->title);
	}
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;
		if(data->col == 0)
		{
			LV_ITEM lvi;
			memset(&lvi, 0, sizeof(lvi));
			lvi.mask = LVIF_TEXT;
			lvi.iItem = data->row;
			lvi.iSubItem = 0;
			lvi.pszText = (char *)data->title;

			list->InsertItem(&lvi);
		}
		else
		{
			list->SetItemText(data->row, data->col, data->title);
		}
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	LTextTableView *tabtext;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		LOutlineItem *item = table->FindItemForRow(data->row + 1);
		CPseudoItem *theItem;
		if((theItem = dynamic_cast<CPseudoItem *>(item)) != 0L)
		{
			map<int, CStr>::value_type val(data->col + 1, CStr(data->title));
			theItem->m_texts.insert(val);
		}
	}
	else if((tabtext = dynamic_cast<LTextTableView *>(view)) != 0L)
	{
		STableCell cell(data->row + 1, data->col + 1);
		tabtext->SetCellData(cell, data->title, strlen(data->title));
	}
#endif
}

void UWidget::OnListGetInfo(int cmd, ULIST_INFO *desc)
{
	// sanity
	desc->data = 0L;
	desc->title = 0L;

	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qMacAPP
	TView *view = (TView *)w;
	if(MA_MEMBER(view, TPseudoTextListView))
	{
		static CStr cstr; // no good

		TPseudoTextListView *txt = (TPseudoTextListView *)view;
		CStr255_AC name = txt->GetItemText(desc->row + 1);
		cstr = name;
		desc->title = cstr;
	}
#endif
#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	desc->data = gtk_clist_get_row_data(list, desc->row);
	gtk_clist_get_text_is_buggy(list, desc->row, desc->col, (gchar **)&desc->title);
#endif
#ifdef WIN32
	CWnd *wnd = (CWnd *)w;

	if(wnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
	{
		CListCtrl *list = (CListCtrl *)wnd;

		desc->data = (void *)list->GetItemData(desc->row);
		static CString s;
		s = list->GetItemText(desc->row, desc->col);
		desc->title = s;
	}
#endif
#if qMacPP
	LPane *view = (LPane *)w;
	LOutlineTable *table;
	LTextTableView *tabtext;
	if((table = dynamic_cast<LOutlineTable *>(view)) != 0L)
	{
		LOutlineItem *item = table->FindItemForRow(desc->row + 1);
		CPseudoItem *theItem;
		if((theItem = dynamic_cast<CPseudoItem *>(item)) != 0L)
		{
			map<int, CStr>::iterator i = theItem->m_texts.find(desc->col + 1);
			if(i != theItem->m_texts.end())
			{
				desc->title = (*i).second;
			}
			desc->data = theItem->m_data;
		}
	}
	else if((tabtext = dynamic_cast<LTextTableView *>(view)) != 0L)
	{
		STableCell cell(desc->row + 1, desc->col + 1);
		UInt32 ioDataSize = 255;
		char tmp[255];
		tabtext->GetCellData(cell, tmp, ioDataSize);
		static CStr res;
		res.set(tmp, ioDataSize);
		desc->title = res;
	}
#endif
}

void UWidget::OnListResort(int cmd, int column, UListSorter s)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);

	sGtkClistDiff = s;
	sGtkClistWid = this;
	gtk_clist_set_compare_func(list, gtkclistdefcompare);
	gtk_clist_set_sort_column(list, column);
	gtk_clist_sort(list);
	gtk_clist_set_compare_func(list, 0L);
#endif
}

void UWidget::OnListSetSortDir(int cmd, int column, bool dir)
{
	void *w = GetWidget(cmd);
	if(w == 0L)
		return;

#if qGTK
	if(!GTK_IS_CLIST(w))
		return;

    GtkCList *list = GTK_CLIST(w);
	gchar *text;
	guint8 spacing;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	
	gtk_clist_get_pixtext (list, 0, column, &text, &spacing, &pixmap, &mask);
	gtk_clist_set_pixtext (list, 0, column, text, spacing, pixmap, mask);
#endif
}

//}
void UWidget::OnStatusBarText(int cmd, const char *txt)
{
	void *w = GetWidget(cmd);
	if( (w == 0L) || (txt == 0L) )
		return;

#if qGTK
	if(!GTK_IS_STATUSBAR(w))
		return;

    GtkStatusbar *bar = GTK_STATUSBAR(w);
	
	gtk_statusbar_pop(bar, 1);
	gtk_statusbar_push(bar, 1, txt);
#endif
}

int UWidget::OnUpdateFocus(int leave, void *widget)
{
	map<int, void *>::const_iterator i;

	for(i = m_widgets.begin(); i != m_widgets.end(); ++i)
	{
		if((*i).second == widget)
			break;
	}
	if(i == m_widgets.end())
		return 0; // continue notification

	if(leave)
	{
		if(sFocusWidID == GetWidID())
			sFocusWidID = -1;
	}
	else
	{
		sFocusWidID = GetWidID();
		sFocusCmdID = (*i).first;
	}

	return 1; // stop notification
}

bool UWidget::HasModal(void)
{
#if qGTK
	const STD map<int, UCmdTarget *> & targets = UEventGetTargets();
	STD map<int, UCmdTarget *>::const_iterator i;
	for(i = targets.begin(); i != targets.end(); ++i)
	{
		UCmdTarget *target = (*i).second;
		if(!target->IsKindOf(URUNTIME_CLASS(UWidget)))
			continue;

		UWidget *wid = (UWidget *)target;
		if(wid->m_modalLoop)
			return true;
	}
#endif
	return false;
}

int UWidget::OnFetchCmd(int cmd, void *widget)
{
	map<int, void *>::const_iterator i;
	for(i = m_widgets.begin(); i != m_widgets.end(); ++i)
	{
		if(widget == (*i).second)
			return (*i).first;
	}
	return -1;
}

void UWidget::AddPage(UWidget *page, int pagecmd, int num_page)
{
	void *wid = GetWidget(kUMainWidget);
	if(wid == 0L)
		return;

	UEventSendMessage(page->GetWidID(), EV_INIT_TAB_WIDGET, UMAKEINT(pagecmd, num_page), wid);
	map<int, UWidget *>::value_type val(UMAKEINT(pagecmd, num_page), page);
	m_tabs.insert(val);

#if qMacPP
	// emulate the click in tab 1
	if(num_page == 0)
	{
		// emulate the click in tab 1
		UEventSendMessage(GetWidID(), EV_PAGE_CHANGED, UMAKEINT(pagecmd, num_page), 0L);
	}
#endif /* qMacPP */
}

void UWidget::OnPageChanged(int cmdid, int num_page)
{
	if(m_currentPage == UMAKEINT(cmdid, num_page))
		return;

	map<int, UWidget *>::iterator i;
	if(m_currentPage != -1)
	{
		i = m_tabs.find(m_currentPage);
		if(i != m_tabs.end())
		{
			UWidget *tab = (*i).second;
			tab->DoDataExchange(false);

#if qMacPP
			LPseudoWinAttachment *attach = macpp_getattachwindow(this);
			if(attach != 0L)
			{
				attach->InvalidCache(tab);
			}
			
			tab->m_widgets.erase(tab->m_widgets.begin(), tab->m_widgets.end());
#endif /* qMacPP */
		}
	}

	if(num_page == -1)
	{
		m_currentPage = -1;
		return;
	}

	i = m_tabs.find(UMAKEINT(cmdid, num_page));
	if(i != m_tabs.end())
	{
		UWidget *tab = (*i).second;

#if qMacPP
		UPseudoPlaceHolder *holder = dynamic_cast<UPseudoPlaceHolder *>((LPane *)GetWidget(cmdid));
		if(holder != 0L)
		{
			macpp_fecthview(tab->GetWidID(), holder->GetCurrentView());
			
			LPseudoWinAttachment *attach = macpp_getattachwindow(this);
			if(attach != 0L)
			{
				macpp_linkbroadcasters(holder->GetCurrentView(), attach);
			}
		}
#endif /* qMacPP */

		tab->DoDataExchange(true);
	}
	m_currentPage = UMAKEINT(cmdid, num_page);
}

#ifdef WIN32
void UWidget::OverideMapWidget(int cmdid, CRuntimeClass *widClass)
{
	map<int, CRuntimeClass *>::value_type val(cmdid, widClass);
	m_overide.insert(val);
}
#endif

#if qMacPP || qMacAPP
SInt32 UWidget::GetPaneID(int cmdid)
{
	cmdid -= EV_COMMAND_START;
	unsigned char tmp[50];
	unsigned char res[5] = {'p', 's', '\0', '\0', '\0'};
	sprintf((char *)tmp, "%d", cmdid);
	if(strlen((char *)tmp) > 2 || cmdid < 0)
	{
		UAppConsole("cmdid too big (%d)\n", cmdid);
	}
	else if(strlen((char *)tmp) == 2)
	{
		res[2] = tmp[0];
		res[3] = tmp[1];
	}
	else
	{
		res[2] = '0';
		res[3] = tmp[0];
	}
	
	SInt32 num = (res[0] << 24) | (res[1] << 16) | (res[2] << 8) | res[3];
	return num;
}
#endif

void UWidget::OnKeyDown(int keyval)
{
#if qGTK
	lastKey = keyval;
#endif
}

void UWidget::ResetKeyState()
{
#if qGTK
	lastKey = GDK_VoidSymbol;
#endif
}

short UWidget::GetAsyncKeyState(unsigned int keyFlag)
{
#if qGTK
	short ret = (lastKey != GDK_VoidSymbol) && (lastKey == keyFlag);

	if(ret)
		ResetKeyState();

	return ret;
#else
	return 0;
#endif
}

bool UWidget::IsEscKeyPressed()
{
#if qGTK
	return GetAsyncKeyState(GDK_Escape);
#else
	return false;
#endif
}
