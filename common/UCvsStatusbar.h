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

#ifndef UCVSSTATUS_H
#define UCVSSTATUS_H

#include "uwidget.h"

class UCvsStatusbar : public UWidget
{
	UDECLARE_DYNAMIC(UCvsStatusbar)
public:
	UCvsStatusbar();
	virtual ~UCvsStatusbar();

protected:

	ev_msg void OnDestroy();
	ev_msg void OnCreate();

	UDECLARE_MESSAGE_MAP()
};
#endif
