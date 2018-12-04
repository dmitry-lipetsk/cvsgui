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
 * Author : Kirill Müller <mail@kirill-mueller.de> --- February 2006
 */

/*
 * DiffParse.h : parse the cvs diff output
 */

#if !defined(AFX_DIFFPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_DIFFPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#	pragma once
#endif // _MSC_VER >= 1000

#include "MultiFiles.h"
#include "CvsArgs.h"
#include "AppGlue.h"

bool CvsDiffParse(const char* dir, const CvsArgs& args, MultiFiles& files);

/// Class to implement diff console that will collect lines
/// beginning with "Index: "
class CCvsDiffConsole : public CCvsFilterConsole
{
public:
	// Construction
	CCvsDiffConsole(MultiFiles* mf, const char* dir);

	// Interface
	virtual void OnFilter(const std::string& txt);

private:
	// Members
	MultiFiles* m_mf;		/*!< Selection */
	std::string m_dir;	/*!< Folder */
};

#endif // !defined(AFX_DIFFPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
