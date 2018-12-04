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
 * Author : Jonathan M. Gilligan <jonathan.gilligan@vanderbilt.edu> --- July 2001
 */

/*
 * WinCvsDebug.cpp --- Miscellaneous debugging stuff.
 */

#include "stdafx.h"
#include "WinCvsDebug.h"

#if ENABLE_WINCVSDEBUG

namespace
{
	unsigned long wincvs_debug_mask = 0L;
}

unsigned long GetWinCvsDebugMask(bool clear_mask)
{
	unsigned long retval = wincvs_debug_mask;
	if ( clear_mask && (wincvs_debug_mask & wcvsdbg_one_shot) )
		wincvs_debug_mask = wcvsdbg_one_shot;
	return retval;
}

void SetWinCvsDebugMask(unsigned long umask)
{
	wincvs_debug_mask = umask;
}

void AppendWinCvsDebugMask(unsigned long umask)
{
	wincvs_debug_mask = wincvs_debug_mask | umask;
}

void SetWinCvsDebugMaskBit(unsigned char bit, bool val)
{
	if (bit > sizeof(wincvs_debug_mask) * 8)
	{
		return;
	}
	if (val) 
	{
		wincvs_debug_mask = wincvs_debug_mask | 1 << bit;
	}
	else
	{
		wincvs_debug_mask = wincvs_debug_mask & ~(1 << bit);
	}
}

bool GetWinCvsDebugMaskBit(unsigned char bit, bool clear_mask)
{
	bool retval = (wincvs_debug_mask & (1 << bit)) != 0;
	if ( clear_mask && (wincvs_debug_mask & wcvsdbg_one_shot) && (bit != wcvsdbg_one_shot_bit) )
	{
		wincvs_debug_mask = wincvs_debug_mask & ~(1 << bit);
	}
	return retval;
}

#endif	// ENABLE_WINCVSDEBUG
