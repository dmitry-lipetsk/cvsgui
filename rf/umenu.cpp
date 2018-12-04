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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <vector>
#include <map>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "umenu.h"
#include "uconsole.h"

#ifdef WIN32
#include "dib.h"
#endif

#ifdef qMacAPP
#include "UMenuMgr.h"
#include "UDispatcher.h"
#endif

#if qMacPP
#include <UConditionalMenus.h>
#include <UAppearanceMenus.h>
#endif

using namespace std;

#ifdef qGTK
extern "C"
{
	// from glade's support.h
	GtkWidget *create_pixmap(GtkWidget *widget, const gchar *filename);
}
#endif

#if defined(_DEBUG) && defined(WIN32)
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

UIMPLEMENT_DYNAMIC(UMenu, UWidget)

UBEGIN_MESSAGE_MAP(UMenu, UWidget)
	ON_UPDATEUI(UMenu)
	ON_UCOMMAND_RANGE(0, 0xFFFF, UMenu::OnCommands)
UEND_MESSAGE_MAP()

UMenu::UMenu(int widid) : UWidget(widid)
#if qGTK
	, fAccelGroup(0L), fRadioGroup(0L)
#endif
{
#if qGTK
	fAccelGroup = gtk_accel_group_new ();
#endif
}

UMenu::~UMenu()
{
}

#ifdef WIN32
// create a HBITMAP from a pixmap
HBITMAP UMakeBitmap(char **icon, int & width, int & height, COLORREF back)
{
	BYTE **src = (BYTE **)icon;

	// read the header
	int w, h, nc, cpp, i, x, y;
	if(sscanf((char *)*src++, "%d%d%d%d", &w, &h, &nc, &cpp) != 4)
		return 0L;

	width = w;
	height = h;

	HBITMAP hbitmap = CreateDIB(w, h, 8, 0L);
	LPBITMAPINFO header = (LPBITMAPINFO)GlobalLock(hbitmap);
	BYTE *image = (LPBYTE)header + header->bmiHeader.biSize + PaletteSize(header);
	RGBQUAD *pRgb = (RGBQUAD *)((LPBYTE)header + header->bmiHeader.biSize);

	// read the colormap
	map<short, BYTE> colors;
	for(i = 0; i < nc; i++)
	{
		const BYTE *line = *src++;
		short aref;
		if(cpp == 1)
		{
			aref = line[0];
			line += 4;
		}
		else if(cpp == 2)
		{
			aref = ((short)line[1] << 8)  | line[0];
			line += 5;
		}
		else
			ASSERT(0);

		COLORREF color;

		if(strcmp((char *)line, "None") == 0)
		{
			color = back;
		}
		else
		{
			if(*line++ != '#')
				return 0L;

			if(strlen((char *)line) < 6)
				return 0L;

			int comp[6], c;
			int j;
			for(j = 0; j < 6; j++)
			{
				c = *line++;
				if(c >= '0' && c <= '9')
					comp[j] = c - '0';
				else if(c >= 'a' && c <= 'f')
					comp[j] = c - 'a' + 10;
				else if(c >= 'A' && c <= 'F')
					comp[j] = c - 'A' + 10;
				else
					return 0L;
			}

			color = RGB(comp[0]*16+comp[1],comp[2]*16+comp[3],comp[4]*16+comp[5]);
		}

		colors.insert(make_pair(aref, i));

		pRgb->rgbRed = GetRValue(color);
		pRgb->rgbGreen = GetGValue(color);
		pRgb->rgbBlue = GetBValue(color);
		pRgb->rgbReserved = 0;
		pRgb++;
	}

	if(nc != colors.size() || nc > 256)
		return 0L;

	// make the bitmap
	for(y = 0; y < h; y++)
	{
		const BYTE *line = *src++;
		BYTE *ptr = image + WIDTHBYTES(w * 8) * (h - y - 1);
		for(x = 0; x < w; x++)
		{
			short aref;
			if(cpp == 1)
			{
				aref = line[0];
				line++;
			}
			else if(cpp == 2)
			{
				aref = ((short)line[1] << 8)  | line[0];
				line += 2;
			}
			map<short, BYTE>::iterator pos = colors.find(aref);
			if(pos == colors.end())
				return 0L;
			*ptr++ = (*pos).second;
		}
	}

	GlobalUnlock(hbitmap);

	HBITMAP ddbbmap = DIBToBitmap(hbitmap);
	GlobalFree((HGLOBAL)hbitmap);

	return ddbbmap;
}

HBITMAP UMakeBitmap(char **icon)
{
	int w, h;
	return UMakeBitmap(icon, w, h, ::GetSysColor(COLOR_BTNFACE));
}

UEXPORT HBITMAP UMakeBitmap(const char *icon)
{
	vector<string> icstr;
	const char *tmp;
	while( (tmp = strchr(icon, '\n')) != NULL )
	{
		string str;
		str.assign(icon, tmp - icon);
		icon = tmp + 1;
		icstr.push_back(str);
	}

	if(icon[0] != '\0')
	{
		icstr.push_back(string(icon));
	}

	vector<string>::const_iterator i;
	vector<const char*> ic;
	for(i = icstr.begin(); i != icstr.end(); ++i)
	{
		ic.push_back((*i).c_str());
	}
	return UMakeBitmap((char**)&ic[0]);
}
#endif

#define CTRLALT_STROKE	"Ctrl+Alt+"
#define CTRL_STROKE		"Ctrl+"
#define ALT_STROKE		"Alt+"

static void GetMenuTitle(const char* name, std::string& title, int cmd, char& mnemonic, int& mod, int& key)
{
	const char* tmp;

	title = "";
	mod = 0;
	key = 0;
	mnemonic = '\0';

	// Get the accelerator
	string copy(name);

	if( tmp = strchr(name, '\t') )
	{
		copy = copy.substr(0, tmp - name);
		tmp++;

		if( strncmp(tmp, CTRLALT_STROKE, strlen(CTRLALT_STROKE)) == 0 )
		{
			mod = UK_CONTROL | UK_ALT;
			key = tmp[strlen(CTRLALT_STROKE)];
		}
		else if( strncmp(tmp, CTRL_STROKE, strlen(CTRL_STROKE)) == 0 )
		{
			mod = UK_CONTROL;
			key = tmp[strlen(CTRL_STROKE)];
		}
		else if( strncmp(tmp, ALT_STROKE, strlen(ALT_STROKE)) == 0 )
		{
			mod = UK_ALT;
			key = tmp[strlen(ALT_STROKE)];
		}

		name = copy.c_str();
	}

	title = name;

#ifndef WIN32
	if((tmp = strchr(title.c_str(), '&')) != 0L)
	{
		size_t l = tmp - (const char *)title.c_str();
		memmove((char *)&title[l], &title[l + 1], title.length() - l);
	}
#endif
}

UMENU UMenuAddPulldown(int widid, const char* name, UMENU top, int cmd /*= 0*/)
{
	string title;
	int  mod, key;
	char mnemonic;
	GetMenuTitle(name, title, 0, mnemonic, mod, key);

#ifdef qMacAPP
	return UMENU_NULL;
#endif
#ifdef qMacPP
	if(top == UMENU_NULL)
	{
		Assert_(0); // TODO
	}
	else
	{
		LMenuBar *menuBar = LMenuBar::GetCurrentMenuBar();
		LMenu *menu = menuBar->FetchMenu(top);
		if(menu == 0L)
			return UMENU_NULL;
		
		LStr255 pstr(name);
		LMenu *newMenu = new LMenu(::UniqueID('MENU'), pstr);
		menuBar->InstallMenu(newMenu, hierMenu);

		menu->InsertCommand(pstr, cmd_UseMenuItem, max_Int16 - 1);
		UAppearanceMenus::SetItemHierID(menu->GetMacMenuH(), ::CountMenuItems(menu->GetMacMenuH()),
			newMenu->GetMenuID());
		return newMenu->GetMenuID();
	}
	return UMENU_NULL;
#endif
#ifdef qGTK
	GtkWidget *parent = 0L;

	if(top == UMENU_NULL)
	{
		// try to find the menubar : the menu bar should be mapped
		// to a UMenu object as the main widget
		const UCmdTarget *target = UEventQueryWidget(widid);
		UMenu *menu = 0L;
		if(target == 0L || !target->IsKindOf(URUNTIME_CLASS(UMenu)))
			return UMENU_NULL;

		menu = (UMenu *)target;
		if(menu == 0L)
			return UMENU_NULL;

		GtkWidget *wid = (GtkWidget *)menu->GetWidget(kUMainWidget);
		if(wid == 0L || !GTK_IS_MENU_SHELL(wid))
			return UMENU_NULL;
		
		parent = wid;
	}
	else
	{
		if(!GTK_IS_MENU_SHELL(top))
			return UMENU_NULL;
		parent = (GtkWidget *)top;
	}

	GtkWidget *menuitem = gtk_menu_item_new_with_label(title.c_str());
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER (parent), menuitem);
	
	GtkWidget *menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem), menu);
	gtk_menu_ensure_uline_accel_group (GTK_MENU (menu));
	UEventSendMessage(widid, EV_INIT_WIDGET, cmd, menu);
	return (UMENU)menu;
#endif
#ifdef WIN32
	if(top == UMENU_NULL)
	{
		CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
		CMenu menu;
		menu.CreatePopupMenu();
		pTopMenu->AppendMenu(MF_POPUP, (UINT)menu.GetSafeHmenu(), name);

		static bool firsttime = true;
		if(firsttime)
		{
			pTopMenu->DeleteMenu(0, MF_BYPOSITION);
			firsttime = false;
		}
		return (UMENU)menu.Detach();
	}
	else
	{
		HMENU menuhdl = (HMENU)top;
		CMenu parent;
		CMenu menu;
		menu.CreatePopupMenu();
		VERIFY(parent.Attach(menuhdl));
		parent.AppendMenu(MF_POPUP, (UINT)menu.GetSafeHmenu(), name);
		parent.Detach();
		return (UMENU)menu.Detach();
	}
#endif
}

#ifdef WIN32
static void addAccel(int cmd, int mod, int key)
{
	if(key == 0)
		return;

	// Save the current accelerator table. 
	CWnd *wnd = AfxGetMainWnd();
	CFrameWnd *frame = 0L;
	if(wnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		frame = (CFrameWnd *)wnd;
	else
		frame = wnd->GetParentFrame();
	if(frame == 0L)
		return;

	HACCEL haccelOld = frame->m_hAccelTable;

	// Count the number of entries in the current 
	// table, allocate a buffer for the table, and 
	// then copy the table into the buffer. 

	int cAccelerators = CopyAcceleratorTable(haccelOld, NULL, 0); 
	LPACCEL lpaccelNew = (LPACCEL) LocalAlloc(LPTR, (cAccelerators + 1) * sizeof(ACCEL)); 
	if (lpaccelNew == NULL) 
		return;

	CopyAcceleratorTable(haccelOld, lpaccelNew, cAccelerators); 

	// Find the accelerator that the user modified 
	// and change its flags and virtual-key code 
	// as appropriate. 

	int i = 0;
	for (i = 0; i < cAccelerators; i++) 
	{
		if(lpaccelNew[i].cmd == cmd)
		{
			lpaccelNew[i].fVirt = FVIRTKEY;
			if(mod & UK_ALT)
				lpaccelNew[i].fVirt |= FALT;
			if(mod & UK_CONTROL)
				lpaccelNew[i].fVirt |= FCONTROL;
			lpaccelNew[i].key = key;
			break;
		}
	} 

	if(i == cAccelerators)
	{
		lpaccelNew[i].cmd = cmd;
		lpaccelNew[i].fVirt = FVIRTKEY;
		if(mod & UK_ALT)
			lpaccelNew[i].fVirt |= FALT;
		if(mod & UK_CONTROL)
			lpaccelNew[i].fVirt |= FCONTROL;
		lpaccelNew[i].key = key;
		cAccelerators++;
	}

	// Create the new accelerator table, and 
	// destroy the old one. 
	DestroyAcceleratorTable(haccelOld); 
	frame->m_hAccelTable = CreateAcceleratorTable(lpaccelNew, cAccelerators); 
	LocalFree(lpaccelNew);
}
#endif

#ifdef qGTK
static void addToAccelGroup(GtkAccelGroup *accelGroup, GtkWidget *menuitem, int mod, int key)
{
	if(key == 0 || accelGroup == 0L)
		return;

	int gdkmod = 0;
	if(mod & UK_CONTROL)
		gdkmod |= GDK_CONTROL_MASK;
	if(mod & UK_ALT)
		gdkmod |= GDK_MOD1_MASK;

  	gtk_widget_add_accelerator (menuitem, "activate",
								accelGroup, key, gdkmod, GTK_ACCEL_VISIBLE);
}

static GtkWidget* gtk_menu_item_new_with_label_pixmap(const gchar *label, const void *pixmap)
{
// FIX ME !
#if 1
	return gtk_menu_item_new_with_label(label);
#else
	if(pixmap == 0L)
		return gtk_menu_item_new_with_label(label);

	GtkWidget *menu_item = gtk_menu_item_new ();

	GtkPixmap *gtkpix = GTK_PIXMAP(create_pixmap(menu_item, (const gchar *)pixmap));
	gtk_misc_set_alignment (GTK_MISC (gtkpix), 0.0, 0.5);
	gtk_container_add (GTK_CONTAINER (menu_item), GTK_WIDGET(gtkpix));
	gtk_widget_show (GTK_WIDGET(gtkpix));

	GtkWidget *accel_label = gtk_accel_label_new (label);
	gtk_misc_set_alignment (GTK_MISC (accel_label), 0.0, 0.5);
	
	gtk_container_add (GTK_CONTAINER (menu_item), accel_label);
	gtk_accel_label_set_accel_widget (GTK_ACCEL_LABEL (accel_label), menu_item);
	gtk_widget_show (accel_label);
	
	return menu_item;
#endif
}
#endif // qGTK

void UMenuDelete(UMENU top, int afterIndex)
{
	UASSERT(top != UMENU_NULL);

#ifdef qMacAPP
	MenuRef menu = MAGetMenu(top);
	if(menu)
	{
		short numItems = ::CountMenuItems(menu);
		afterIndex++;
		while (numItems >= afterIndex)
		{
			::DeleteMenuItem(menu, numItems);
			--numItems;
		}
	}
#endif
#ifdef qMacPP
	LMenuBar *menuBar = LMenuBar::GetCurrentMenuBar();
	LMenu *menu = menuBar->FetchMenu(top);
	if(menu)
	{
		short numItems = ::CountMenuItems(menu->GetMacMenuH());
		afterIndex++;
		while (numItems >= afterIndex)
		{
			menu->RemoveItem(numItems);
			--numItems;
		}
	}
	else
	{
		MenuHandle mhdl = ::GetMenu(top);
		if(mhdl != 0L)
		{
			short numItems = ::CountMenuItems(mhdl);
			afterIndex++;
			while (numItems >= afterIndex)
			{
				::DeleteMenuItem(mhdl, numItems);
				--numItems;
			}
		}
	}
#endif
#ifdef WIN32
	CMenu parent;
	HMENU menuhdl = (HMENU)top;
	VERIFY(parent.Attach(menuhdl));

	int numItems = (int)parent.GetMenuItemCount() - 1;
	while (numItems >= afterIndex)
	{
		VERIFY(parent.DeleteMenu(numItems, MF_BYPOSITION));
		--numItems;
	}
	parent.Detach();
#endif
#ifdef qUnix
	if(!GTK_IS_MENU_SHELL(top))
		return;
	
	GtkWidget *parent = (GtkWidget *)top;

	GList *list = gtk_container_children(GTK_CONTAINER (parent));
	GList *items = g_list_nth(list, afterIndex);
	
	while(items != 0L)
	{
		GtkWidget *wid = GTK_WIDGET(items->data);
		gtk_container_remove(GTK_CONTAINER(parent), wid);
		items = (GList *)g_list_next(items);
	}
#endif
}

int UMenuCount(UMENU top)
{
	UASSERT(top != UMENU_NULL);

#ifdef qMacAPP
	MenuRef menu = MAGetMenu(top);
	if(menu)
	{
		return ::CountMenuItems(menu);
	}
#endif
#ifdef qMacPP
	LMenuBar *menuBar = LMenuBar::GetCurrentMenuBar();
	LMenu *menu = menuBar->FetchMenu(top);
	if(menu)
	{
		return ::CountMenuItems(menu->GetMacMenuH());
	}
	else
	{
		MenuHandle mhdl = ::GetMenu(top);
		if(mhdl != 0L)
		{
			return ::CountMenuItems(mhdl);
		}
	}
#endif
#ifdef WIN32
	CMenu parent;
	HMENU menuhdl = (HMENU)top;
	VERIFY(parent.Attach(menuhdl));

	UINT numItems = parent.GetMenuItemCount();
	parent.Detach();

	return numItems;
#endif
#ifdef qUnix
	if(!GTK_IS_MENU_SHELL(top))
		return 0;
	
	GtkWidget *parent = (GtkWidget *)top;

	GList* items = gtk_container_children(GTK_CONTAINER (parent));
	
	return g_list_length(items);
#endif

	return 0;
}

void UMenuDeleteByCmd(UMENU top, int cmd)
{
	UASSERT(top != UMENU_NULL);

#ifdef qMacAPP
	MenuID menuNo;
	short itemNo;
	MenuRef menu = CommandToComponents(cmd, menuNo, itemNo);
	if(menu)
	{
		::DeleteMenuItem(menu, itemNo);
	}
#endif
#ifdef qMacPP
	LMenuBar *menuBar = LMenuBar::GetCurrentMenuBar();
	LMenu *menu = menuBar->FetchMenu(top);
	if(menu)
	{
		menu->RemoveCommand(cmd);
	}
#endif
#ifdef WIN32
	CMenu parent;
	HMENU menuhdl = (HMENU)top;
	VERIFY(parent.Attach(menuhdl));

	VERIFY(parent.DeleteMenu(cmd, MF_BYCOMMAND));

	parent.Detach();
#endif
#ifdef qUnix
	if(!GTK_IS_MENU_SHELL(top))
		return;
	
	GtkWidget *parent = (GtkWidget *)top;

	GList *list = gtk_container_children(GTK_CONTAINER (parent));
		
	while(list != 0L)
	{
		GtkWidget *wid = GTK_WIDGET(list->data);

		if(UHIWORD(gtk_object_get_data(GTK_OBJECT(wid), kUgtkDataToken)) == cmd) {			
			gtk_container_remove(GTK_CONTAINER(parent), wid);
			break;
		}
	
		list = (GList *)g_list_next(list);
	}
#endif
}

UMENU UMenuAddButton(int widid, const char* name, UMENU top, const void* pixmap, int cmd)
{
	string title;
	int  mod, key;
	char mnemonic;
	GetMenuTitle(name, title, cmd, mnemonic, mod, key);
	UASSERT(top != UMENU_NULL);

#ifdef qMacPP
	LMenuBar *menuBar = LMenuBar::GetCurrentMenuBar();
	LMenu *menu = menuBar->FetchMenu(top);
	if(menu)
	{
		LStr255 pstr(title);
		menu->InsertCommand(pstr, cmd, max_Int16 - 1);
	}
	else
	{
		MenuHandle mhdl = ::GetMenu(top);
		if(mhdl != 0L)
		{
			SInt16	menuLength = ::CountMenuItems(mhdl);
			LStr255 pstr(title);
			::MacInsertMenuItem(mhdl, pstr, menuLength);
		}
	}
	
	return UMENU_NULL;
#endif
#ifdef qMacAPP
	MenuRef menu = MAGetMenu(top);
	if(menu)
	{
		CStr255_AC pstr(title);
		gDispatcher->DoMakeMenuBarManager();
		TMenuBarManager::fgMenuBarManager->AddMenuItem(pstr, top, ::CountMenuItems(menu), cmd);
	}
	
	return UMENU_NULL;
#endif
#ifdef qGTK
	UCmdTarget *target = UEventQueryWidget(widid);
	if(target == 0L || !target->IsKindOf(URUNTIME_CLASS(UMenu)))
		return UMENU_NULL;
	UMenu *pmenu = (UMenu *)target;

	if(!GTK_IS_MENU_SHELL(top))
		return UMENU_NULL;
	GtkWidget *parent = (GtkWidget *)top;

	GtkWidget *menuitem = gtk_menu_item_new_with_label_pixmap(title.c_str(), pixmap);
	gtk_widget_show (menuitem);
	gtk_container_add (GTK_CONTAINER (parent), menuitem);
	addToAccelGroup((GtkAccelGroup *)pmenu->GetAccelGroup(), menuitem, mod, key);
	UEventSendMessage(widid, EV_INIT_WIDGET, cmd, menuitem);
	return UMENU_NULL;
#endif
#ifdef WIN32
	HMENU menuhdl = (HMENU)top;
	CMenu parent;
	VERIFY(parent.Attach(menuhdl));

	parent.AppendMenu(MF_STRING, UCMD_to_WINCMD(cmd), name);
	addAccel(UCMD_to_WINCMD(cmd), mod, key);

	if(pixmap != 0L && strlen((const char *)pixmap))
	{
		HBITMAP bmap = UMakeBitmap((const char *)pixmap);
		CBitmap b;
		VERIFY(b.Attach(bmap));
		parent.SetMenuItemBitmaps(UCMD_to_WINCMD(cmd), MF_BYCOMMAND, &b, &b);
		b.Detach();
	}

	parent.Detach();
	return UMENU_NULL;
#endif
}

UMENU UMenuAddToggle(int widid, const char* name, UMENU top, const void* pixmap, int cmd)
{
	string title;
	int  mod, key;
	char mnemonic;
	GetMenuTitle(name, title, cmd, mnemonic, mod, key);

#ifdef qMacPP
	LMenuBar *menuBar = LMenuBar::GetCurrentMenuBar();
	LMenu *menu = menuBar->FetchMenu(top);
	if(menu)
	{
		LStr255 pstr(title);
		menu->InsertCommand(pstr, cmd, max_Int16 - 1);
	}
	else
	{
		MenuHandle mhdl = ::GetMenu(top);
		if(mhdl != 0L)
		{
			SInt16	menuLength = ::CountMenuItems(mhdl);
			LStr255 pstr(title);
			::MacInsertMenuItem(mhdl, pstr, menuLength);
		}
	}
	
	return UMENU_NULL;
#endif
#ifdef qMacAPP
	MenuRef menu = MAGetMenu(top);
	if(menu)
	{
		CStr255_AC pstr(title);
		gDispatcher->DoMakeMenuBarManager();
		TMenuBarManager::fgMenuBarManager->AddMenuItem(pstr, top, ::CountMenuItems(menu), cmd);
	}
	
	return UMENU_NULL;
#endif
#ifdef qGTK
	UCmdTarget *target = UEventQueryWidget(widid);
	if(target == 0L || !target->IsKindOf(URUNTIME_CLASS(UMenu)))
		return UMENU_NULL;
	UMenu *pmenu = (UMenu *)target;

	if(!GTK_IS_MENU_SHELL(top))
		return UMENU_NULL;
	GtkWidget *parent = (GtkWidget *)top;

	GtkWidget *menuitem = gtk_check_menu_item_new_with_label(title.c_str());
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER (parent), menuitem);
	addToAccelGroup((GtkAccelGroup *)pmenu->GetAccelGroup(), menuitem, mod, key);
//  	gtk_tooltips_set_tip (tooltips, quit1, "Quit the application", NULL);
	UEventSendMessage(widid, EV_INIT_WIDGET, cmd, menuitem);

	return UMENU_NULL;
#endif
#ifdef WIN32
	ASSERT(top != UMENU_NULL);

	HMENU menuhdl = (HMENU)top;
	CMenu parent;
	VERIFY(parent.Attach(menuhdl));

	parent.AppendMenu(MF_STRING, UCMD_to_WINCMD(cmd), name);
	addAccel(UCMD_to_WINCMD(cmd), mod, key);

	if(pixmap != 0L && strlen((const char *)pixmap))
	{
		HBITMAP bmap = UMakeBitmap((const char *)pixmap);
		CBitmap b;
		VERIFY(b.Attach(bmap));
		parent.SetMenuItemBitmaps(UCMD_to_WINCMD(cmd), MF_BYCOMMAND, &b, &b);
		b.Detach();
	}

	parent.Detach();
	return UMENU_NULL;
#endif
}

UMENU UMenuAddRadio(int widid, const char* name, UMENU top, const void* pixmap, int cmd)
{
	string title;
	int  mod, key;
	char mnemonic;
	GetMenuTitle(name, title, cmd, mnemonic, mod, key);

#ifdef qMacPP
	return UMENU_NULL;
#endif
#ifdef qMacAPP
	return UMENU_NULL;
#endif
#ifdef qGTK
	UCmdTarget *target = UEventQueryWidget(widid);
	if(target == 0L || !target->IsKindOf(URUNTIME_CLASS(UMenu)))
		return UMENU_NULL;
	UMenu *pmenu = (UMenu *)target;

	if(!GTK_IS_MENU_SHELL(top))
		return UMENU_NULL;
	GtkWidget *parent = (GtkWidget *)top;
	GSList* group = (GSList*)pmenu->GetRadioGroup();
	
	GtkWidget *menuitem = gtk_radio_menu_item_new_with_label(group, title.c_str());
	
	if(group == 0L) {
		group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menuitem));
		pmenu->SetRadioGroup(group);
	}
	
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER (parent), menuitem);
	addToAccelGroup((GtkAccelGroup *)pmenu->GetAccelGroup(), menuitem, mod, key);
	UEventSendMessage(widid, EV_INIT_WIDGET, cmd, menuitem);

	return UMENU_NULL;
#endif
#ifdef WIN32
	return UMENU_NULL;
#endif
}

UMENU UMenuAddSeparator(int widid, UMENU top)
{
	UASSERT(top != UMENU_NULL);

#ifdef qMacPP
	LMenuBar *menuBar = LMenuBar::GetCurrentMenuBar();
	LMenu *menu = menuBar->FetchMenu(top);
	if(menu)
	{
		menu->InsertCommand("\p-", 0, max_Int16 - 1);
	}
	else
	{
		MenuHandle mhdl = ::GetMenu(top);
		if(mhdl != 0L)
		{
			SInt16	menuLength = ::CountMenuItems(mhdl);
			::MacInsertMenuItem(mhdl, "\p-", menuLength);
		}
	}
	
	return UMENU_NULL;
#endif
#ifdef qMacAPP
	MenuRef menu = MAGetMenu(top);
	if(menu)
	{
		CStr255_AC pstr("-");
		gDispatcher->DoMakeMenuBarManager();
		TMenuBarManager::fgMenuBarManager->AddMenuItem(pstr, top, ::CountMenuItems(menu), 0);
	}
	
	return UMENU_NULL;
#endif
#ifdef qGTK
	if(!GTK_IS_MENU_SHELL(top))
		return UMENU_NULL;
	GtkWidget *parent = (GtkWidget *)top;

	GtkWidget *menuitem = gtk_menu_item_new();
	gtk_widget_show(menuitem);
	gtk_container_add(GTK_CONTAINER (parent), menuitem);
	gtk_widget_set_sensitive(menuitem, FALSE);
	return UMENU_NULL;
#endif
#ifdef WIN32
	HMENU menuhdl = (HMENU)top;
	CMenu parent;
	VERIFY(parent.Attach(menuhdl));
	parent.AppendMenu(MF_SEPARATOR);
	parent.Detach();
	return UMENU_NULL;
#endif
}

void UMenu::OnUpdateUI()
{
	// for WIN32, the UpdateUI on the menus is better handled by MFC
#ifndef WIN32
	static bool sSemaphore = false;
	USemaphore policeman(sSemaphore);
	if(policeman.IsEnteredTwice())
		return;

	// this is the interesting thing about the toolbar : it resends the
	// update UI to other widgets according to the focus information and
	// allows the update to be handled several times if the handler asks for it
	// (by using UCmdUI::SetContinue)

	const STD map<int, UCmdTarget *> & targets = UEventGetTargets();
	map<int, void *>::iterator w;
	int cmdid;

	for(w = m_widgets.begin(); w != m_widgets.end(); ++w)
	{
		void *widget = (*w).second;
		cmdid = (*w).first;

		if(cmdid == kUMainWidget)
			continue;

#if qGTK
		if(!GTK_IS_MENU_ITEM(widget))
			continue;
#endif

		// first forward the command to the focused widget : if it doesn't want it
		// we give it to the next pseudo widget.
		if(sFocusWidID != -1 && sFocusWidID != GetWidID())
		{
			UCmdUI pUI(cmdid);
			pUI.m_widget = widget;
			UEventSendMessage(sFocusWidID, EV_UPDTCMD, pUI.m_nID, &pUI);
			if(!pUI.CanContinue())
				continue;
		}

		STD map<int, UCmdTarget *>::const_iterator i;
		for(i = targets.begin(); i != targets.end(); ++i)
		{
			int widid = (*i).first;
			if(widid == sFocusWidID || widid == GetWidID())
				continue;
			
			UCmdUI pUI(cmdid);
			pUI.m_widget = widget;
			UEventSendMessage(widid, EV_UPDTCMD, pUI.m_nID, &pUI);
			if(!pUI.CanContinue())
				break;
		}
		if(i == targets.end())
		{
			//UAppConsole("The cmd %d of the menubar has no update handler, it's gonna be disabled\n", cmdid);
			UCmdUI pUI(cmdid);
			pUI.m_widget = widget;
			pUI.Enable(false);
		}
	}
#endif
}

int UMenu::OnCommands(int cmd)
{
#if 0
	static bool sSemaphore = false;
	USemaphore policeman(sSemaphore);
	if(policeman.IsEnteredTwice())
		return 1;
#endif

	// this is the interesting thing about the toolbar : it resends the
	// commands to other widgets according to the focus information and
	// allows the command to be handled several times if the handler return 1

	const STD map<int, UCmdTarget *> & targets = UEventGetTargets();
	int res = 0;
	int handledFlag;

	// first forward the command to the focused widget : if it doesn't want it
	// we give it to the next pseudo widget.
	if(sFocusWidID != -1 && sFocusWidID != GetWidID())
	{
		res = UEventSendMessageExt(sFocusWidID, EV_CMD, cmd, 0L, &handledFlag);
		if(/*res == 0 &&*/ handledFlag)
			return 0;// may be we should retuen res here?
	}

	STD map<int, UCmdTarget *>::const_iterator i;
	for(i = targets.begin(); i != targets.end(); ++i)
	{
		int widid = (*i).first;
		if(widid == sFocusWidID || widid == GetWidID())
			continue;
		res = UEventSendMessageExt(widid, EV_CMD, cmd, 0L, &handledFlag);
		if(/*res == 0 &&*/ handledFlag)
			return 0;
	}

	//UAppConsole("The cmd %d of the menubar has no command handler\n", cmd);

	return 1;
}
