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
 * LCmdHelpBevelButton.cpp --- extends the CmdBevelButton with a help string
 */

#include "LCmdHelpBevelBtn.h"
#include <LStaticText.h>
#include <string.h>

LCmdHelpBevelButton::LCmdHelpBevelButton(LStream *inStream) : LCmdBevelButton(inStream)
{
	Str255 helpstr;
	inStream->ReadPString(helpstr);
	memcpy(fHelp, helpstr, MAX_CMDHLPSTR * sizeof(char));
	inStream->ReadData(&fTargetID, sizeof(PaneIDT));
	fTarget = (LStaticText *)-1;
}

LCmdHelpBevelButton::~LCmdHelpBevelButton()
{
}

#if __PowerPlant__	< 0x02118000
void LCmdHelpBevelButton::AdjustCursorSelf(Point inPortPt, const EventRecord & inMacEvent)
#else
void		LCmdHelpBevelButton::AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn)
#endif
{
	MouseEnter(inPortPt, inMacEvent);
	
#if __PowerPlant__	< 0x02118000
	LCmdBevelButton::AdjustCursorSelf(inPortPt, inMacEvent);
#else
	LCmdBevelButton::AdjustMouseSelf(inPortPt, inMacEvent, outMouseRgn);
#endif
}

void LCmdHelpBevelButton::MouseEnter(Point /*inPortPt*/, const EventRecord &/*inMacEvent*/)
{
	if(fTarget == 0L)
		return;

	// find the target the first time
	if(fTarget == (LStaticText *)-1)
	{
		LPane * aPane = this;
		while(aPane != 0L && aPane->GetSuperView() != 0L)
			aPane = aPane->GetSuperView();

		Assert_(aPane != 0L);
		if(aPane != 0L)
		{
			fTarget = dynamic_cast<LStaticText*>
				(aPane->FindPaneByID(fTargetID));
			Assert_(fTarget != 0L);
			if(fTarget == 0L)
				return;
		}
	}
	
	Str255 outBuffer;
	Size outTextLength;
	fTarget->GetText((char *)outBuffer, sizeof(Str255), &outTextLength);
	if(outTextLength == fHelp[0] &&
		memcmp(&fHelp[1], outBuffer, outTextLength * sizeof(char)) == 0)
			return;

	fTarget->SetText((Ptr)&fHelp[1], fHelp[0]);

}

void LCmdHelpBevelButton::MouseLeave()
{
	if(fTarget == 0L)
		return;

	char txt[2] = {'\0', '\0'};
	fTarget->SetText((Ptr)&txt[1], txt[0]);
}
