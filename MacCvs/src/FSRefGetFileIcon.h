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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2002
 */
/* ------------------ GetFileIcon.h ------------------- */

#ifndef __GETFILEICON__
#define __GETFILEICON__

#ifndef __ICONS__
#include <Icons.h>
#endif
#ifndef __FINDER__
#include <Finder.h>
#endif

// store a 'IconRef' reference along with the 'IconSuiteRef'
// associated with.
class CIconRefData
{
public:
	CIconRefData(IconSuiteRef suite, IconRef icon);

protected:
	// cannot use 'delete', use UnRef() instead.
	~CIconRefData();

public:
	void SetIconSuite(IconSuiteRef suite);
	void SetIcon(IconRef icon);

	inline IconSuiteRef GetIconSuite(void) const { return m_suite; }
	inline IconRef GetIconRef(void) const { return m_icon; }

	inline static int Compare(const CIconRefData & node1, const CIconRefData & node2)
	{
		return node1.m_icon < node2.m_icon ? -1 : (node1.m_icon > node2.m_icon ? 1 : 0);
	}
	inline CIconRefData *Ref(void) {++m_ref; return this;}
	inline CIconRefData *UnRef(void)
	{
		if(--m_ref == 0)
		{
			delete this;
			return 0L;
		}
		return this;
	}

protected:
	int m_ref;
	IconSuiteRef m_suite;
	IconRef m_icon;
};

class CIconRef
{
public:
	CIconRef() : m_data(0L) {}
	CIconRef(CIconRefData *data) : m_data(data->Ref()) {}
	CIconRef(CIconRefData & data) : m_data(data.Ref()) {}
	CIconRef(const CIconRef & anode) : m_data(0L)
	{
		*this = anode;
	}
	~CIconRef()
	{
		if(m_data != 0L)
			m_data->UnRef();
	}

	inline static int Compare(const CIconRef & node1, const CIconRef & node2)
	{
		return CIconRefData::Compare(*node1.m_data, *node2.m_data);
	}

	inline CIconRef & operator=(const CIconRef & anode)
	{
		CIconRefData* oldData = m_data;
		m_data = anode.m_data;
		if(m_data != 0L)
			m_data->Ref();
		if(oldData != 0L)
			oldData->UnRef();			
		return *this;
	}

	inline IconSuiteRef GetIconSuite(void) const { return m_data != 0L ? m_data->GetIconSuite() : 0L; }
	inline IconRef GetIconRef(void) const { return m_data != 0L ? m_data->GetIconRef() : 0L; }

protected:
	CIconRefData *m_data;
};

OSStatus GetSysIcon(OSType inIconType, IconSelectorValue inIconSelector, CIconRef & res);
OSStatus GetFileIcon( FSRef *thing, IconSelectorValue iconSelector, CIconRef & res);

#endif // __GETFILEICON__
