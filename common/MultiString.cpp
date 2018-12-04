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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- August 1998
 */

/*
 * MultiString.cpp --- remember several unique persistents strings
 */

#include "stdafx.h"
#include "MultiString.h"

#if qUnix
#	include "uwidget.h"
#endif

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#	pragma warning (disable : 4660)
#endif /* WIN32 */

using namespace std;

UIMPLEMENT_DYNAMIC(CMString, CPersistent)

#if qUnix
void DoDataExchange(bool fill, int widid, int cmdid, CMString& mstr)
{
	if(fill)
	{
		UEventSendMessage(widid, EV_COMBO_RESETALL, cmdid, 0L);
		const vector<std::string> & list = mstr.GetList();
		vector<std::string>::const_iterator i;
		for(i = list.begin(); i != list.end(); ++i)
		{
			UEventSendMessage(widid, EV_COMBO_APPEND, cmdid, (void*)(const char*)(*i).c_str());
		}
	}
	else
	{
		std::string value;
		UEventSendMessage(widid, EV_GETTEXT, cmdid, &value);
		if(!value.empty())
		{
			mstr.Insert(value.c_str());
		}
	}
}
#endif // qUnix

//////////////////////////////////////////////////////////////////////////
// TMPersistent

//////////////////////////////////////////////////////////////////////////
// TMString

/// defaultStr is a null terminated set of strings
template<class T>
TMString<T>::TMString(unsigned int maxstr, const char* uniqueName, 
					  char* const* defaultStr /*= 0L*/, kClassPersistent pclass /*= kNoClass*/) 
	: CPersistent(uniqueName, pclass), m_maxStr(maxstr)
{
	if( defaultStr != 0L )
	{
		int i = 0;
		while( defaultStr[i] != 0L )
		{
			Insert(defaultStr[i++]);
		}
	}
}

template<class T>
TMString<T>::~TMString()
{
}

template<class T>
unsigned int TMString<T>::SizeOf(void) const
{
	unsigned int thesize = sizeof(int);
	
	for(typename list_t::const_iterator i = m_allStrs.begin(); i != m_allStrs.end(); ++i)
	{
		thesize += i->length() + 1;
	}

	return thesize;
}

template<class T>
const void* TMString<T>::GetData(void) const
{
	unsigned int thesize = SizeOf(), tmpsize;
	((TMString*)this)->m_buffer.AdjustSize(thesize);
	
	char* tmp = ((TMString*)this)->m_buffer;
	*(int*)tmp = m_allStrs.size();
	tmp += sizeof(int) / sizeof(char);
	
	for(typename list_t::const_iterator i = m_allStrs.begin(); i != m_allStrs.end(); ++i)
	{
		tmpsize = i->length() + 1;
		memcpy(tmp, i->c_str(), tmpsize * sizeof(char));
		tmp += tmpsize;
	}
	
	return (char*)m_buffer;
}

template<class T>
void TMString<T>::SetData(const void* ptr, unsigned int size)
{
	const char* tmp = (char*)ptr;
	unsigned int thesize;
	
	m_allStrs.erase(m_allStrs.begin(), m_allStrs.end());
	thesize = *(int*)tmp;
	tmp += sizeof(int) / sizeof(char);
	
	for(unsigned int i = 0; i < thesize && i < m_maxStr; i++)
	{
		m_allStrs.push_back(T(tmp));
		tmp += strlen(tmp) + 1;
	}
}

/*!
	Find a string
	\param instr String to be found
	\return const Iterator to the string found
	\note The string classes have too many operators to use std::find
*/
template<class T>
#if (_MSC_VER > 1200)
typename std::vector<T>::const_iterator TMString<T>::Find(const char *instr) const
#else
typename TMString<T>::list_t::const_iterator TMString<T>::Find(const char *instr) const
#endif
{
#if (__GNUC__ > 2)
	// prevent compiler warnings if gcc 3.x is used
	typename TMString<T>::list_t::const_iterator i;
#else
	TMString<T>::list_t::const_iterator i;
#endif
	
	for(i = m_allStrs.begin(); i != m_allStrs.end(); ++i)
	{
		if( Compare(i->c_str(), instr) == 0 )
			break;
	}
		
	return i;
}

/*!
	Add a new string
	\param str String to insert
*/
template<class T>
void TMString<T>::Insert(const char* str)
{
	TouchTimeStamp();

	for(typename list_t::iterator i = m_allStrs.begin(); i != m_allStrs.end(); ++i)
	{
		if( Compare(i->c_str(), str) == 0 )
		{
			m_allStrs.erase(i);
			m_allStrs.insert(m_allStrs.begin(), T(str));
			return;
		}
	}		

	m_allStrs.insert(m_allStrs.begin(), T(str));

	if( m_allStrs.size() > m_maxStr )
	{
		m_allStrs.erase(m_allStrs.begin() + m_maxStr, m_allStrs.end());
	}
}

/*!
	Remove a string, true if it was found
	\param str String to remove
	\return true if a string was found and remove, false otherwise
*/
template<class T>
bool TMString<T>::Remove(const char* str)
{
	for(typename list_t::iterator i = m_allStrs.begin(); i != m_allStrs.end(); ++i)
	{
		if( Compare(i->c_str(), str) == 0 )
		{
			TouchTimeStamp();
			m_allStrs.erase(i);
			return true;
		}
	}
	
	return false;
}

/*!
	Remove all strings
*/
template <class T>
void TMString<T>::RemoveAll()
{
	TouchTimeStamp();
	m_allStrs.clear();
}

//////////////////////////////////////////////////////////////////////////
// CMString

CMString::CMString(unsigned int maxstr, const char* uniqueName,
		 char* const* defaultStr /*= 0L*/, kClassPersistent pclass /*= kNoClass*/) 
		 : TMString<string>(maxstr, uniqueName, defaultStr, pclass)
{
}
	
CMString::~CMString()
{
}

//////////////////////////////////////////////////////////////////////////
// TKeyString

template<char delimiter>
TKeyString<delimiter>::TKeyString(unsigned int maxstr, const char* uniqueName,
								  char* const* defaultStr /*= 0L*/, kClassPersistent pclass /*= kNoClass*/) 
								  : CMString(maxstr, uniqueName, defaultStr, pclass)
{
}

template<char delimiter>
TKeyString<delimiter>::~TKeyString()
{
}

/*!
	Concatenate the key and value strings into a single delimited string
	\param res Return result string
	\param key Key
	\param value Value
*/
template<char delimiter>
void TKeyString<delimiter>::Concatenate(std::string& res, const char* key, const char* value) const
{
	char c;
	while( (c = *key++) != '\0' )
	{
		if( c == delimiter || c == '\\' )
			res += '\\';

		res += c;
	}
	
	res += delimiter;
	while( (c = *value++) != '\0' )
	{
		if( c == delimiter || c == '\\' )
			res += '\\';
		
		res += c;
	}
}

/*!
	Split a single delimited string into the key and value strings
	\param str String to split
	\param key Return key
	\param value Return value
*/
template<char delimiter>
void TKeyString<delimiter>::Split(const char* str, std::string& key, std::string& value) const
{
	key.erase();
	value.erase();
	
	bool inKey = true;
	
	char c;
	while( (c = *str++) != 0 )
	{
		if( c == '\\' )
		{
			c = *str++;
		}
		else if( c == delimiter )
		{
			inKey = false;
			continue;
		}
		
		if( inKey )
			key += c;
		else
			value += c;
	}
}

template<char delimiter>
int TKeyString<delimiter>::Compare(const char* s1, const char* s2) const
{
	string key1, key2, val1, val2;

	Split(s1, key1, val1);
	Split(s2, key2, val2);

	return strcmp(key1.c_str(), key2.c_str());
}

/*!
	Get the delimiter
	\return Keyword-value delimiter
*/
template<char delimiter>
const char TKeyString<delimiter>::GetDelimiter() const
{
	return delimiter;
}

//////////////////////////////////////////////////////////////////////////
// CKeyString

CKeyString::CKeyString(unsigned int maxstr, const char* uniqueName,
					   char* const* defaultStr /*= 0L*/, kClassPersistent pclass /*= kNoClass*/)
					   : TKeyString<'@'>(maxstr, uniqueName, defaultStr, pclass)
{
}

CKeyString::~CKeyString()
{
}

//////////////////////////////////////////////////////////////////////////
// CMKeyString

CMKeyString::CMKeyString(unsigned int maxstr, const char* uniqueName,
					   char* const* defaultStr /*= 0L*/, kClassPersistent pclass /*= kNoClass*/)
					   : TKeyString<'\x01'>(maxstr, uniqueName, defaultStr, pclass)
{
}

CMKeyString::~CMKeyString()
{
}

int CMKeyString::Compare(const char* s1, const char* s2) const
{
	return CMString::Compare(s1, s2);
}

// this needs to be at the end for Metrowerks
template class TMString<string>;
