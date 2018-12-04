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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- October 2002
 */

/*
 * CCVSToolbar.h --- toolbar for MacCvs
 */

#pragma once

#include <LWindow.h>

class TCVSToolbar : public LWindow, public LPeriodical
{
public:
	enum { class_ID = FOUR_CHAR_CODE('CVTB') };

	enum
	{
		kMovieItem	=	'moov'
	};


	TCVSToolbar(LStream*			inStream);
	virtual ~TCVSToolbar();

	virtual	void		SpendTime(
								const EventRecord&	inMacEvent);
	
	void HaveMovie(Movie moov);
	void Start(void);
	void Stop(void);

	virtual void		AttemptClose();
	
	void CanCloseNow(bool inAllowClose) { fCanClose = inAllowClose; }
	
protected:
	virtual void		DrawSelf();
	
	MovieController fPlay;
	Movie fWindowMovie;
	CGrafPtr fOldPort;
	GDHandle fOldGD;
	bool fStart;
	bool  fCanClose;
};
