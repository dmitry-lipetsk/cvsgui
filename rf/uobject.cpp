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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- May 1999
 */

#include "stdafx.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "uobject.h"

URuntimeClass *URuntimeClass::sAllURuntimeClass = 0L;

const URuntimeClass* UObject::_GetBaseClass()
	{ return 0L; }
const URuntimeClass UObject::classUObject(
	"UObject", sizeof(UObject), 0L, &UObject::_GetBaseClass);
const URuntimeClass *UObject::rtUObject()
	{ return &UObject::classUObject; }

UObject* UObject::CreateObject(const char *className)
{
	URuntimeClass *next = URuntimeClass::sAllURuntimeClass;
	while(next != 0L)
	{
		if(strcmp(next->m_lpszClassName, className) == 0)
			return next->CreateObject();
		
		next = next->m_pNextClass;
	}
	return 0L;
}

const URuntimeClass* UObject::GetRuntimeClass() const
{
	return URUNTIME_CLASS(UObject);
}

bool UObject::IsKindOf(const URuntimeClass* pClass) const
{
	// simple SI case
	const URuntimeClass* pClassThis = GetRuntimeClass();
	return pClassThis->IsDerivedFrom(pClass);
}

URuntimeClass::URuntimeClass(const char *lpszClassName, int nObjectSize,
		UObject* (*pfnCreateObject)(), const URuntimeClass* (*pfnGetBaseClass)())
{
	m_lpszClassName = lpszClassName;
	m_nObjectSize = nObjectSize;
	m_pfnCreateObject = pfnCreateObject;
	m_pfnGetBaseClass = pfnGetBaseClass;
	m_pNextClass = sAllURuntimeClass;
	
	sAllURuntimeClass = this;
}

URuntimeClass::~URuntimeClass()
{
	URuntimeClass *next = sAllURuntimeClass;
	URuntimeClass *prev = 0L;
	while(next != 0L)
	{
		if(next == this)
		{
			if(prev == 0L)
				sAllURuntimeClass = m_pNextClass;
			else
				prev->m_pNextClass = m_pNextClass;
			break;
		}
		
		prev = next;
		next = next->m_pNextClass;
	}
}

UObject* URuntimeClass::CreateObject()
{
	if (m_pfnCreateObject == 0L)
	{
		return 0L;
	}

	UObject* pObject = 0L;
	try
	{
		pObject = (*m_pfnCreateObject)();
	}
	catch(...)
	{
	}

	return pObject;
}

bool URuntimeClass::IsDerivedFrom(const URuntimeClass* pBaseClass) const
{
	// simple SI case
	const URuntimeClass* pClassThis = this;
	while (pClassThis != 0L)
	{
		if (pClassThis == pBaseClass)
			return true;
		pClassThis = (*pClassThis->m_pfnGetBaseClass)();
	}
	return false;       // walked to the top, no match
}
