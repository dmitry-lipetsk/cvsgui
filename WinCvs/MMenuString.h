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

// MMenuString.h: interface for the CMMenuString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMENUSTRING_H__6C55ACE9_20FB_42DE_B678_7EA6A3AF99B3__INCLUDED_)
#define AFX_MMENUSTRING_H__6C55ACE9_20FB_42DE_B678_7EA6A3AF99B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/// Menu string
class CMMenuString : public CPersistent
{
public:
	// Construction
	CMMenuString(unsigned int maxstr, const char* uniqueName, char* const* defaultStr = 0L);
	virtual ~CMMenuString();
	
	virtual unsigned int SizeOf(void) const;
	virtual const void* GetData(void) const;
	virtual void SetData(const void* ptr, unsigned int size);
	
	void Insert(const char* newstr);
	
	typedef std::vector<std::string> list_type; /*!< List type */

	inline const list_type& GetList(void) const;
	inline list_type& GetList(void);
	
protected:
	// Data members
	list_type m_allStrs;		/*!< String list */
	CStaticAllocT<char> m_buf;	/*!< Buffer */
	unsigned int m_maxStr;		/*!< Maximum number of strings */
};

//////////////////////////////////////////////////////////////////////////
// CMMenuString inline implementation

/// Get the const list
inline const CMMenuString::list_type& CMMenuString::GetList(void) const
{ 
	return m_allStrs; 
}

/// Get the list
inline CMMenuString::list_type& CMMenuString::GetList(void)
{
	TouchTimeStamp(); 
	
	return m_allStrs; 
}


#endif // !defined(AFX_MMENUSTRING_H__6C55ACE9_20FB_42DE_B678_7EA6A3AF99B3__INCLUDED_)
