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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

// AskYesNo.h : ask yes or no !

#if !defined(AFX_ASKYESNO_H__5946A8C4_8517_11D1_8949_444553540000__INCLUDED_)
#define AFX_ASKYESNO_H__5946A8C4_8517_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

bool PromptEditMessage(const char* msg, std::string& edit, const char* title = "Message",
					   const char* def = "OK", const char* cncl = "Cancel");

#endif // !defined(AFX_ASKYESNO_H__5946A8C4_8517_11D1_8949_444553540000__INCLUDED_)
