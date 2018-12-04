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
 * LRadioTabEnabler.h --- enable/disable a view when activated/deactivated
 */

#ifndef LCVSWASTEEDIT_H
#define LCVSWASTEEDIT_H
#pragma once

#include "WTextView.h"

class	CCvsWasteEdit : public WTextView
{
public:
	enum {class_ID = 'CWST'};

	CCvsWasteEdit(LStream *inStream);
	virtual ~CCvsWasteEdit();

	virtual Boolean HandleKeyPress(const EventRecord & inKeyEvent);
		// handle PageUp/PageDown and sone others things

protected:
	bool HandleEnterKey(void);
		// execute the selection

	virtual void DoDragReceive(DragReference inDragRef);
	virtual Boolean DragIsAcceptable(DragReference inDragRef);
		// from LDragAndDrop
};

#endif /* LCVSWASTEEDIT_H */
