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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- June 2004
 */

// MMenuString.cpp: implementation of the CMMenuString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wincvs.h"
#include "MMenuString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;

/*!
	Constructor
	\param maxstr Maximum string
	\param uniqueName Unique name
	\param defaultStr NULL terminated set of strings
*/
CMMenuString::CMMenuString(unsigned int maxstr, const char* uniqueName, char* const* defaultStr /*= 0L*/)
	: CPersistent(uniqueName, kNoClass), m_maxStr(maxstr)
{
	if( defaultStr != NULL )
	{
		int i = 0;
		while( defaultStr[i] != NULL )
		{
			Insert(defaultStr[i++]);
		}
	}
}

CMMenuString::~CMMenuString()
{
}

unsigned int CMMenuString::SizeOf(void) const
{
	unsigned int thesize = sizeof(int);
	
	for(list_type::const_iterator i = m_allStrs.begin(); i != m_allStrs.end(); ++i)
	{
		thesize += (*i).length() + 1;
	}

	return thesize;
}

const void* CMMenuString::GetData(void) const
{
	unsigned int thesize = SizeOf(), tmpsize;
	((CMMenuString*)this)->m_buf.AdjustSize(thesize);
	
	char* tmp = ((CMMenuString*)this)->m_buf;
	*(int*)tmp = m_allStrs.size();
	tmp += sizeof(int) / sizeof(char);
	
	for(list_type::const_iterator i = m_allStrs.begin(); i != m_allStrs.end(); ++i)
	{
		tmpsize = i->length() + 1;
		memcpy(tmp, i->c_str(), tmpsize * sizeof(char));
		tmp += tmpsize;
	}
	
	return (char*)m_buf;
}

void CMMenuString::SetData(const void* ptr, unsigned int size)
{
	const char* tmp = (char*)ptr;

	m_allStrs.erase(m_allStrs.begin(), m_allStrs.end());

	unsigned int thesize = *(int *)tmp;
	tmp += sizeof(int) / sizeof(char);
	
	for(unsigned int i = 0; i < thesize && i < m_maxStr; i++)
	{
		m_allStrs.push_back(string(tmp));
		tmp += strlen(tmp) + 1;
	}
}

void CMMenuString::Insert(const char* newstr)
{
	TouchTimeStamp();
	m_allStrs.push_back(string(newstr));
}
