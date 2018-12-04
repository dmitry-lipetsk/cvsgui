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
 * MultiString.h --- remember several unique persistents strings
 */

#ifndef MULTISTRING_H
#define MULTISTRING_H

#include <vector>
#include "ustr.h"

#if qUnix
	class CMString;
	void DoDataExchange(bool fill, int widid, int cmdid, CMString& mstr);
#endif // qUnix

///	Persistent array
template<class T>
class TMPersistent : public CPersistent
{
protected:
	typedef	std::vector<T> list_t;
public:
	// Construction
	TMPersistent(const char* uniqueName, kClassPersistent pclass = kNoClass);
	virtual ~TMPersistent();
	
protected:
	// Methods
	virtual unsigned int SizeOf(void) const;
	virtual const void* GetData(void) const;
	virtual void SetData(const void* ptr, unsigned int size);
	
	// Interface
	inline const list_t& GetList(void) const;
	inline list_t& GetList(void);

private:
	// Data members
	list_t m_allItems;
	mutable	CStaticAllocT<char> m_buffer;
};

template<class T>
class TMString : public CPersistent
{
public:
	typedef	std::vector<T> list_t;
	TMString(unsigned int maxstr, const char* uniqueName,
		char* const* defaultStr = 0L, kClassPersistent pclass = kNoClass);
	
	virtual ~TMString();
	
protected:
	// Methods
	virtual unsigned int SizeOf(void) const;
	virtual const void* GetData(void) const;
	virtual void SetData(const void* ptr, unsigned int size);

	virtual int Compare(const char* s1, const char* s2) const;
	
public:
	// Interface
	void Insert(const char* str);
	bool Remove(const char* str);
	void RemoveAll();
	
	typename list_t::const_iterator Find(const char* instr) const;
	
	inline const list_t& GetList(void) const;
	inline void SetList(const list_t& List);
	inline typename list_t::const_iterator begin() const;
	inline typename list_t::const_iterator end() const;
	
protected:
	// Data members
	list_t m_allStrs;
	mutable	CStaticAllocT<char> m_buffer;
	unsigned int m_maxStr;
};

/// std::string specialization of TMString
class CMString : public TMString<std::string>
{
	UDECLARE_DYNAMIC(CMString)
public:
	// Construction
	CMString(unsigned int maxstr, const char* uniqueName,
		char* const* defaultStr = 0L, kClassPersistent pclass = kNoClass);
	
	virtual ~CMString();
};

/// Multi-string for storing map (e.g. dictionary)
template <char delimiter>
class TKeyString : public CMString
{
public:
	// Construction
	TKeyString(unsigned int maxstr, const char* uniqueName,
		char* const* defaultStr = 0L, kClassPersistent pclass = kNoClass);

	virtual ~TKeyString();

protected:
	// Methods
	virtual int Compare(const char* s1, const char* s2) const;

public:
	// Interface
	virtual void Concatenate(std::string& res, const char* key, const char* value) const;
	virtual void Split(const char* str, std::string& key, std::string& value) const;

	const char GetDelimiter() const;
};
					   
/// Dictionary specialization of TKeyString ('@')
class CKeyString : public TKeyString<'@'>
{
public:
	// Construction
	CKeyString(unsigned int maxstr, const char* uniqueName,
		char* const* defaultStr = 0L, kClassPersistent pclass = kNoClass);

	virtual ~CKeyString();
};

/// Multi-key specialization of TKeyString ('x01')
class CMKeyString : public TKeyString<'\x01'>
{
public:
	// Construction
	CMKeyString(unsigned int maxstr, const char* uniqueName,
		char* const* defaultStr = 0L, kClassPersistent pclass = kNoClass);
	
	virtual ~CMKeyString();

protected:
	// Methods
	virtual int Compare(const char* s1, const char* s2) const;
};

//////////////////////////////////////////////////////////////////////////
// TMPersistent	inline implementations

template<class T>
inline const typename TMPersistent<T>::list_t& TMPersistent<T>::GetList(void) const
{
	return m_allItems;
}

template<class T>
inline typename TMPersistent<T>::list_t& TMPersistent<T>::GetList(void) 
{
	return m_allItems;
}

template<class T>
TMPersistent<T>::TMPersistent(const char* uniqueName, kClassPersistent pclass /*= kNoClass*/)
	: CPersistent(uniqueName, pclass)
{
}

template<class T>
TMPersistent<T>::~TMPersistent()
{
}

template<class T>
inline unsigned int TMPersistent<T>::SizeOf(void) const
{
	return sizeof(int) + sizeof(T) * m_allItems.size();
}

template<class T>
const void* TMPersistent<T>::GetData(void) const
{
	m_buffer.AdjustSize(SizeOf());
	char* tmp = m_buffer;
	*(int*) tmp = m_allItems.size();
	tmp += sizeof(int) / sizeof(char);
	
	for(typename list_t::const_iterator i = m_allItems.begin(); i != m_allItems.end(); ++i)
	{
		memcpy(tmp, (const char*)(&*i), sizeof(T));
		tmp += sizeof(T);
	}
	
	return (char*)m_buffer;
}

template<class T>
void TMPersistent<T>::SetData(const void* ptr, unsigned int size)
{
	const char* tmp = (char*)ptr;
	m_allItems.erase(m_allItems.begin(), m_allItems.end());
	unsigned int thesize = *(int*)tmp;
	
	tmp += sizeof(int) / sizeof(char);
	for(unsigned int i = 0; i < thesize; ++i)
	{
		m_allItems.push_back(*((T*)tmp));
		tmp += sizeof(T);
	}
}

//////////////////////////////////////////////////////////////////////////
// TMString	inline implementations

template<class T>
inline const typename TMString<T>::list_t& TMString<T>::GetList(void) const
{
	return m_allStrs;
}

template<class T>
inline void TMString<T>::SetList(const list_t& List)
{
	m_allStrs = List;
}

template<class T>
#if (_MSC_VER > 1200)
typename std::vector<T>::const_iterator TMString<T>::begin() const
#else
typename TMString<T>::list_t::const_iterator TMString<T>::begin() const
#endif
{
	return GetList().begin();
}

template<class T>
#if (_MSC_VER > 1200)
typename std::vector<T>::const_iterator TMString<T>::end() const
#else
typename TMString<T>::list_t::const_iterator TMString<T>::end() const
#endif
{
	return GetList().end();
}

template<class T>
int TMString<T>::Compare(const char* s1, const char* s2) const
{
	return strcmp(s1, s2);
}

#endif /* AUTHEN_H */
