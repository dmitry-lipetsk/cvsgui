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

#ifndef UOBJECT_H
#define UOBJECT_H

#include "uconfig.h"

#ifdef __cplusplus

class UObject;

class UEXPORT URuntimeClass
{
public:
	URuntimeClass(const char *lpszClassName, int nObjectSize,
		UObject* (*pfnCreateObject)(), const URuntimeClass* (*pfnGetBaseClass)());
	~URuntimeClass();
	
// Attributes
	const char *m_lpszClassName;
	int m_nObjectSize;
	UObject* (* m_pfnCreateObject)(); // 0L => abstract class
	const URuntimeClass* (* m_pfnGetBaseClass)();

// Operations
	UObject* CreateObject();
	bool IsDerivedFrom(const URuntimeClass* pBaseClass) const;

	// CRuntimeClass objects linked together in simple list
	URuntimeClass* m_pNextClass;       // linked list of registered classes

	// linked list of all the registred runtime classes
	static URuntimeClass *sAllURuntimeClass;
};

#define URUNTIME_CLASS(class_name) (class_name::rt##class_name())

#define UDECLARE_DYNAMIC(class_name) \
protected: \
	static const URuntimeClass* _GetBaseClass(); \
	static const URuntimeClass class##class_name; \
public: \
	static const URuntimeClass *rt##class_name(void); \
	virtual const URuntimeClass* GetRuntimeClass() const; \

// dynamically constructable
#define UDECLARE_DYNCREATE(class_name) \
	UDECLARE_DYNAMIC(class_name) \
	static UObject* CreateObject(); \

#define UIMPLEMENT_RUNTIMECLASS(class_name, base_class_name, pfnNew) \
	const URuntimeClass *class_name::rt##class_name(void) \
		{ return &class_name::class##class_name; } \
	const URuntimeClass* class_name::_GetBaseClass() \
		{ return URUNTIME_CLASS(base_class_name); } \
	const URuntimeClass class_name::class##class_name( \
		#class_name, sizeof(class class_name), pfnNew, \
			&class_name::_GetBaseClass); \
	const URuntimeClass* class_name::GetRuntimeClass() const \
		{ return URUNTIME_CLASS(class_name); } \

#define UIMPLEMENT_DYNAMIC(class_name, base_class_name) \
	UIMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0L)

#define UIMPLEMENT_DYNCREATE(class_name, base_class_name) \
	UObject* class_name::CreateObject() \
		{ return new class_name; } \
	UIMPLEMENT_RUNTIMECLASS(class_name, base_class_name, \
		class_name::CreateObject)

class UEXPORT UObject
{
public:
	virtual const URuntimeClass* GetRuntimeClass() const;
	virtual ~UObject() {}

	static const URuntimeClass *rtUObject();
	static const URuntimeClass* _GetBaseClass();
	static UObject* CreateObject(const char *className);

	bool IsKindOf(const URuntimeClass* pClass) const;
protected:
	static const URuntimeClass classUObject;
	UObject() {}
};

#define UDYNAMIC_CAST(TYPE, EXPR) \
	((TYPE *)((EXPR) == 0L ? 0L : ((EXPR)->IsKindOf(URUNTIME_CLASS(TYPE)) ? (TYPE *)EXPR : 0L)))

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* UOBJECT_H */
