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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * Persistent.h --- utilities to store persistent values
 */

#ifndef PERSISTENT_H
#define PERSISTENT_H

#include <string.h>
#include <vector>
#include <map>
#include "uobject.h"

#ifdef TARGET_OS_MAC
#	include <Files.h>
#endif /* TARGET_OS_MAC */

#if TARGET_OS_MAC
#	define kCVS_prefID 'cprf'
#	define kCVS_prefName "\pMacCvs Prefs"
#endif

#if defined(WIN32)
/// Disable internal authentication model in favor of dynamic CVSROOT string creation using cvs info command
#	define INTERNAL_AUTHEN 0
#else
/// Enable internal authentication model, to be phased out
#	define INTERNAL_AUTHEN 1
#endif /* !WIN32 */

#ifdef WIN32
	/// CVS settings profile name
#	define PROFILE_NAME "CVS settings"
#endif /* WIN32 */

/// Casting macro, good for most persistent values
#define PERSISTENT_CAST(T) \
	inline operator const T&() const {return m_fValue;} \
	inline T& operator=(const T& val) { TouchTimeStamp(); return m_fValue = val;} \
	inline bool operator==(const T& val) const {return m_fValue == val;}
	/* inline const T * operator ->() const {return &fValue;} */ \

/// Persistent classes enum
typedef enum
{
	kNoClass = 0x00,	/*!< No class */
	kAddSettings = 0x01	/*!< Add to the saved settings */
} kClassPersistent;

// Persistent settings API
void LoadPersistentSettings(const char* path, bool quiet = false);
bool HasPersistentSettings(const char* path);
void AskCreatePersistentSettings(const char* path);
void SavePersistentSettings(const char* path, bool quiet = false);
bool FormatDefaultSettingsLoc(std::string& path);

bool GetAllPersistentSettings(std::map<std::string, std::string>& res);

/// Abstract class to access the persistent data to load/save
class CPersistent : public UObject
{
	UDECLARE_DYNAMIC(CPersistent)
public:
	CPersistent(const char* uniqueName, kClassPersistent pclass);
	virtual ~CPersistent();

protected:
	/// Get the size of the data
	virtual unsigned int SizeOf(void) const = 0L;

	/// Get the data
	virtual const void* GetData(void) const = 0L;

	/// Set the data
	virtual void SetData(const void* ptr, unsigned int size) = 0L;
	
public:
	static void SaveAll(void);
	static bool LoadAll(const char* only = 0L);

	static bool SaveAllSettings(const char* settingFileName, const char* filePath);
	static bool LoadAllSettings(const char* settingFileName);

	/// Entry description internal structure
	struct CPersistentEntry
	{
		const char* m_fUniqueName;
		CPersistent* m_fValue;
		kClassPersistent m_fClass;
	};

	static CPersistentEntry* Find(const char* uniqueName);

	/// Reset the time stamp
	inline void ResetTimeStamp() { m_fTimeStamp = 0; }

	/// Get the time stamp
	inline long GetTimeStamp() const { return m_fTimeStamp; }

	/// Update time stamp
	inline void TouchTimeStamp() { m_fTimeStamp++; }

	static bool NeedSave(bool onlySettings = false);

private:
	static void Register(CPersistent* value, const char* uniqueName, kClassPersistent pclass);
	static void UnRegister(CPersistent* value);
	
  static bool LoadPersistentSettings(const char* token, const char* value, void* data);

protected:
	long m_fTimeStamp;										/*!< Time stamp */
	static std::vector<CPersistentEntry>* m_fAllEntries;	/*!< Entries container */
};

/// Template class for persistent values
template <class T>
class CPersistentT : public CPersistent
{
public:
	CPersistentT(const char* uniqueName, T defValue, kClassPersistent pclass) : CPersistent(uniqueName, pclass)
	{
		m_fValue = m_fDefValue = defValue;
	}

	virtual ~CPersistentT()
	{
	}

	PERSISTENT_CAST(T)

	virtual unsigned int SizeOf(void) const {return sizeof(T);}
	virtual const void* GetData(void) const {return &m_fValue;}
	virtual void SetData(const void* ptr, unsigned int size)
	{
		if( size == sizeof(T) )
			memcpy(&m_fValue, ptr, size);
	}
	
protected:
	T m_fDefValue;	/*!< Default value */
	T m_fValue;		/*!< Value */
};

/// <b>bool</b> type persistent specialization
class CPersistentBool : public CPersistentT<bool>
{
	UDECLARE_DYNAMIC(CPersistentBool)
public:
	CPersistentBool(const char* uniqueName, bool defValue,
		kClassPersistent pclass = kNoClass) :
	  CPersistentT<bool>(uniqueName, defValue, pclass)
	{
	}

	virtual ~CPersistentBool()
	{
	}

	PERSISTENT_CAST(bool)
};

/// <b>int</b> type persistent specialization
class CPersistentInt : public CPersistentT<int>
{
	UDECLARE_DYNAMIC(CPersistentInt)
public:
	CPersistentInt(const char* uniqueName, int defValue,
		kClassPersistent pclass = kNoClass) :
			CPersistentT<int>(uniqueName, defValue, pclass)
	{
	}

	virtual ~CPersistentInt()
	{
	}

	PERSISTENT_CAST(int)
};

#endif /* PERSISTENT_H */
