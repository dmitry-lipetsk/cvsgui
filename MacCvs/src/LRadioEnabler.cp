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
 * LRadioEnabler.cpp --- enable/disable a view when activated/deactivated
 */

#include "LRadioEnabler.h"

LRadioEnabler::LRadioEnabler(LStream *inStream) : LRadioButton(inStream)
{
	inStream->ReadData(&mEnableView, sizeof(PaneIDT));
}

LRadioEnabler::~LRadioEnabler()
{
}

void LRadioEnabler::SetValue(
	SInt32	inValue)
{
	LRadioButton::SetValue(inValue);
	
	LView *superView = GetSuperView();
	if(superView == 0L)
	{
		Assert_(0);
		return;
	}

	LPane *view = dynamic_cast<LPane*>(superView->FindPaneByID(mEnableView));
	
	if(view == 0L)
	{
		Assert_(0);
		return;
	}
	
	if(GetValue() == Button_Off)
	{
		view->Disable();
	}
	else
	{
		view->Enable();
	}
}
