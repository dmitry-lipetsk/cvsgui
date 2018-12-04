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

#include "stdafx.h"

#include "UCvsStatusbar.h"
#include "UCvsCommands.h"
#include "MacrosSetup.h"

UIMPLEMENT_DYNAMIC(UCvsStatusbar, UWidget)

UBEGIN_MESSAGE_MAP(UCvsStatusbar, UWidget)
	ON_UDESTROY(UCvsStatusbar)
	ON_UCREATE(UCvsStatusbar)
UEND_MESSAGE_MAP()

UCvsStatusbar::UCvsStatusbar() : UWidget(kUCvsStatusbarID)
{
}

UCvsStatusbar::~UCvsStatusbar()
{
}

void UCvsStatusbar::OnDestroy(void)
{
	delete this;
}

void UCvsStatusbar::OnCreate(void)
{
}
