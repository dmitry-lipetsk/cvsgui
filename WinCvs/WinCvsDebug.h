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
 * WinCvsDebug.h --- Miscellaneous debugging stuff.
 */

#pragma once

#if !defined(WINCVSDEBUG_H_INCLUDED)
#define WINCVSDEBUG_H_INCLUDED

#if !defined(ENABLE_WINCVSDEBUG)

/// WinCvs debugging disabled by default
#	define ENABLE_WINCVSDEBUG 0

#endif

#if ENABLE_WINCVSDEBUG

enum wcvs_debug_mask_bits 
{
	wcvsdbg_one_shot_bit = 0,
	wcvsdbg_verbose_bit,
	wcvsdbg_trace_dst_checks_bit,
	wcvsdbg_trace_modified_checks_bit,
	wcvsdbg_trace_stat_bit
};

enum wcvs_debug_mask_flags 
{
	wcvsdbg_one_shot = 1L << wcvsdbg_one_shot_bit,
	wcvsdbg_verbose = 1L << wcvsdbg_verbose_bit,
	wcvsdbg_trace_dst_checks = 1L << wcvsdbg_trace_dst_checks_bit,
	wcvsdbg_trace_modified_checks = 1L << wcvsdbg_trace_modified_checks_bit,
	wcvsdbg_trace_stat = 1L << wcvsdbg_trace_stat_bit
};

unsigned long GetWinCvsDebugMask(bool clear_mask = true);
bool GetWinCvsDebugMaskBit(unsigned char bit, bool clear_mask = true);

void SetWinCvsDebugMask(unsigned long umask);
void AppendWinCvsDebugMask(unsigned long umask);
void SetWinCvsDebugMaskBit(unsigned char bit, bool val);

#endif	// ENABLE_WINCVSDEBUG

#endif	// !defined(WINCVSDEBUG_H_INCLUDED)