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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * CBrowserPopup.h --- popup prompted in the browser with Option/Click
 */

#ifndef LBROWSERPOPUP_H
#define LBROWSERPOPUP_H
#pragma once

#include "CPStr.h"
#include "TextBinary.h"

class CBrowserPopup
{
public:
	CBrowserPopup(const char * fullPath);
	~CBrowserPopup();
	
	void Track(Point inGlobalPoint);

  static void SendFinderOpenAE(UInt32 inFileCount, const UFSSpec *theSpecs);
	static void SendFinderOpenAE(const UFSSpec &spec, bool reveal = false);

  void InstallAsSubmenu(MenuHandle inMenu, MenuItemIndex inItemIndex);
  void RemoveMenu();
  bool ProcessMenuCommand(CommandT inCommand);
  
protected:
	bool BuildBrowserMenu(void);
	void OpenMenuItemSelection(SInt16 menuSelect, SInt16 itemSelect);
	
	MenuHandle mMenu;
	CStr mPath;
	short mMenuID;
};

#endif /* LBROWSERPOPUP_H */
