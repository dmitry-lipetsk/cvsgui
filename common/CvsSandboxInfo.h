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
 * Author : Oliver Giesen <ogware@gmx.net>
 */

#if !defined(AFX_CVSSANDBOXINFO_H__06D1CA4F_06C6_4F57_9787_C91C53E372D6__INCLUDED_)
#define AFX_CVSSANDBOXINFO_H__06D1CA4F_06C6_4F57_9787_C91C53E372D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CvsSandboxInfo.h : header file
//

#include <string>

using std::string;

/// CVS sandbox info
class CCvsSandboxInfo
{
public:
	// Construction
	CCvsSandboxInfo(const char* path);
	virtual ~CCvsSandboxInfo();

private:
	// Data members
	string m_path;

protected:
	// Methods
	string ReadAdminFile(const char* name) const;

public:
	// Interface
	string GetRoot() const;
	string GetRepository() const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_CVSSANDBOXINFO_H__06D1CA4F_06C6_4F57_9787_C91C53E372D6__INCLUDED_
