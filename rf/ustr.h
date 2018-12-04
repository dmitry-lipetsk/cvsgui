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

#ifndef USTR_H
#define USTR_H

#include <stddef.h>
#include <string>
#include "uconfig.h"
#include "Persistent.h"

#ifdef qUnix
#	include <string>
#endif

#ifdef __GNUC__
#	define EGCS_CONST const
#else
#	define EGCS_CONST
#endif

#if TARGET_RT_MAC_MACHO || qUnix
#	ifndef HAVE_STRICMP
#		define stricmp strcasecmp
#	endif
#	ifndef HAVE_STRNICMP
#		define strnicmp strncasecmp
#	endif
#endif

/// Path delimiter, platform-dependent
#ifdef WIN32
static const char kPathDelimiter = '\\';
#else
static const char kPathDelimiter = '/';
#endif

/// All platforms path delimiters, use when trimming mixed paths (e.g. repository)
static const char* kUnivPathDelimiters = "/\\";

static const char kCmdLineQuote = '"';
static const char kCmdLineEscape = '\\';

/// Filename extension delimiter
static const char kExtensionDelimiter = '.';

inline const char* StrGetSafeStr(const char* str);
inline bool IsNullOrEmpty(const char* str);

int StrOrdICmp(const char* s1, const char* s2);
int StrPathCmp(const char* s1, const char* s2);
void StrSafeCopy(char* buffer, const char* value, const int bufferSize);
std::string& NormalizeLogMsg(std::string& msg);

std::string& TrimLeft(std::string& str);
std::string& TrimLeft(std::string& str, const char chr);
std::string& TrimLeft(std::string& str, const char* strTrim);

std::string& TrimRight(std::string& str);
std::string& TrimRight(std::string& str, const char chr);
std::string& TrimRight(std::string& str, const char* strTrim);

std::string& Trim(std::string& str);
std::string& Trim(std::string& str, const char chr);
std::string& Trim(std::string& str, const char* strTrim);

bool GetEnvValue(const char* cmd, const char* key, std::string& value);
std::string& Replace(std::string& strRes, const char* strFind, const char* strReplace);
bool ParseCommandLine(const char* cmdLine, std::vector<std::string>& args);

std::string& FlatenSubFolders(std::string& path);
std::string& NormalizeFileName(std::string& fileName);
std::string& NormalizeFolderPath(std::string& folder, bool delimLast = true);
bool SplitPath(const char* dir, std::string& uppath, std::string& folder, bool dontSkipDelimiter = false);
bool SplitFilename(const char* file, std::string& base, std::string& ext);
bool MakeTmpFile(std::string& file, const char* prefix, const char* extension = NULL, bool create = false);

std::string FormatXmlString(const char* xmlItem);

/// Class to use when fed-up with realloc, malloc...
template <class T>
class CStaticAllocT
{
public:
	// Construction
	CStaticAllocT();
	~CStaticAllocT();
	
protected:
	// Data members
	T* m_buf;			/*!< Buffer */
	size_t m_bufsize;	/*!< Buffer size */
	
public:
	// Interface
	void AdjustSize(size_t size);
	
	inline operator T*(void) const /* egcs const */;
	inline operator const T*(void) const;
	
#ifdef __GNUC__
	inline const T& operator[](int i) const;
	inline T& operator[](int i);
#endif
	
	inline size_t size() const;
};

/// Persistent string
class CPersistentString : public std::string, public CPersistent
{
	UDECLARE_DYNAMIC(CPersistentString)
public:
	// Construction
	inline CPersistentString(const char* uniqueName, const char* defValue = 0L, kClassPersistent pclass = kNoClass);
	virtual ~CPersistentString();
	
public:
	// Interface
	virtual unsigned int SizeOf(void) const;
	virtual const void* GetData(void) const;
	virtual void SetData(const void* ptr, unsigned int size);
	
	inline CPersistentString& operator=(const char* newstr);
};

//////////////////////////////////////////////////////////////////////////
// Inline functions implementation

/*!
	Test whether given string is a NULL pointer or and empty string
	\param str String to test
	\return true for empty string, false otherwise
	\note To be used safely with char*
*/
inline bool IsNullOrEmpty(const char* str)
{
	return str == NULL || *str == '\0';
}

/*!
	Get safe character array
	\param str Character array
	\return Safe character array
	\note This is required due to std::string vulnerability to NULL's
*/
inline const char* StrGetSafeStr(const char* str)
{
	return str ? str : "";
}

//////////////////////////////////////////////////////////////////////////
// CStaticAllocT inline implementation

template <class T>
CStaticAllocT<T>::CStaticAllocT()
{
	m_buf = 0L;
	m_bufsize = 0;
}

template <class T>
CStaticAllocT<T>::~CStaticAllocT()
{
	if( m_buf != 0L )
		free(m_buf);
}

/*!
	Adjust the buffer size
	\param size New buffer size
*/
template <class T>
void CStaticAllocT<T>::AdjustSize(size_t size)
{
	if( m_bufsize < size )
	{
		if( m_buf == 0L )
			m_buf = (T*)malloc(size * sizeof(T));
		else
			m_buf = (T*)realloc(m_buf, size * sizeof(T));
		
		if( m_buf == 0L )
			throw std::bad_alloc();
		
		m_bufsize = size;
	}
}

/// Buffer access
template <class T>
inline CStaticAllocT<T>::operator T*(void) const /* egcs const */
{
	return m_buf;
}

/// Buffer access
template <class T>
inline CStaticAllocT<T>::operator const T*(void) const
{
	return m_buf;
}

#ifdef __GNUC__
template <class T>
inline const T& CStaticAllocT<T>::operator[](int i) const
{
	return m_buf[i];
}

template <class T>
inline T& CStaticAllocT<T>::operator[](int i)
{
	return m_buf[i];
}
#endif

/*!
	Get the buffer size
	\return The buffer size
*/
template <class T>
inline size_t CStaticAllocT<T>::size() const
{
	return m_bufsize;
}

//////////////////////////////////////////////////////////////////////////
// CPersistentString inline implementation

inline CPersistentString::CPersistentString(const char* uniqueName, const char* defValue /*= 0L*/, kClassPersistent pclass /*= kNoClass*/)
	: CPersistent(uniqueName, pclass)
{
	if( defValue != 0L )
		*(std::string*)this = defValue;
}

/// Set from a C string
inline CPersistentString& CPersistentString::operator=(const char* newstr)
{
	TouchTimeStamp();
	if( newstr != 0L )
		*(std::string*)this = newstr;
	
	return *this;
}

#endif /* USTR_H */
