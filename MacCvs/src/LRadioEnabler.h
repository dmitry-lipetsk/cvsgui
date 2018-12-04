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

#ifndef LRADIO_ENABLER_H
#define LRADIO_ENABLER_H
#pragma once

#include <LRadioButton.h>

class	LRadioEnabler : public LRadioButton
{
public:
	enum {class_ID = 'RADE'};

	LRadioEnabler(LStream *inStream);
	virtual ~LRadioEnabler();

	virtual void		SetValue(SInt32 inValue);

protected:
	PaneIDT mEnableView;
};

#endif /* LRADIO_ENABLER_H */
