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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

/*
 * 
 */

#ifndef UCVSMENU_H
#define UCVSMENU_H

#include "umenu.h"

class UCvsMenubar : public UMenu
{
	UDECLARE_DYNAMIC(UCvsMenubar)
public:
	UCvsMenubar();
	virtual ~UCvsMenubar();

protected:

	ev_msg void OnDestroy();
	ev_msg void OnCreate();
	//ev_msg void GetMenu();

	UDECLARE_MESSAGE_MAP()
};

void CreateMainMenubar(int widid);

#endif
