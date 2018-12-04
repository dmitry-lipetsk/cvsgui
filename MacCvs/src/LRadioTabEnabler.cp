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
 * LRadioTabEnabler.cpp --- enable/disable a view when activated/deactivated
 */

#include "LRadioTabEnabler.h"

LRadioTabEnabler::LRadioTabEnabler(LStream *inStream) : LRadioButton(inStream)
{
	inStream->ReadData(&mEnableView, sizeof(PaneIDT));
}

LRadioTabEnabler::~LRadioTabEnabler()
{
}

void LRadioTabEnabler::SetValue(SInt32 inValue)
{
	LRadioButton::SetValue(inValue);
	
	if(inValue != Button_On)
		return;
	
	LView *superView = dynamic_cast<LView*>(GetSuperView());
	if(superView == 0L)
	{
		Assert_(0);
		return;
	}

	// now enable/disable regarding the order
	TArrayIterator<LPane*> iterator(superView->GetSubPanes());
	LPane	*theSub = 0L;
	bool doEnable = true;
	while (iterator.Next(theSub))
	{
		LRadioTabEnabler* radio = dynamic_cast<LRadioTabEnabler*>(theSub);
		if(radio == 0L)
			continue;
		
		PaneIDT id = radio->GetAssociatedView();
		if(id != 0)
		{		
			LPane *view = dynamic_cast<LPane*>(superView->FindPaneByID(id));
			if(view == 0L)
			{
				Assert_(0);
			}
			else
			{
				if(doEnable)
					view->Enable();
				else
					view->Disable();
			}
		}
		
		if(theSub == (LPane*)this)
			doEnable = false;
	}
}
