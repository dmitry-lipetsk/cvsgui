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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * LCmdHelpBevelButton.h --- extends the CmdBevelButton with a help string
 */

#ifndef LCMDHELPBEVELBTN_H
#define LCMDHELPBEVELBTN_H
#pragma once

#include <LCmdBevelButton.h>

class LStaticText;

class LCmdHelpBevelButton : public LCmdBevelButton
{
public:
	enum {class_ID = 'CHBB'};
#	define MAX_CMDHLPSTR 128

	LCmdHelpBevelButton(LStream *inStream);
	virtual ~LCmdHelpBevelButton();

	virtual void MouseEnter(Point inPortPt, const EventRecord &inMacEvent);
	virtual void MouseLeave();
#if __PowerPlant__	< 0x02118000
	virtual void AdjustCursorSelf(Point inPortPt, const EventRecord & inMacEvent);
#else
	virtual void		AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);
#endif
		// updates a targeted field with a help string
		// when the mouse goes in/out.

protected:
	LStaticText *fTarget;
	PaneIDT fTargetID;
	unsigned char fHelp[MAX_CMDHLPSTR];
};

#endif /* LCMDHELPBEVELBTN_H */
