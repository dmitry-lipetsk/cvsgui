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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 * Modified: Jerzy Kaczorowski <kaczoroj@hotmail.com> - 27 Jan 2001 - fix warning C4800 (Add "0 != " for all the int variables forced to bool value)
 */

/*
 * CvsEntries.h --- adaptation from cvs/src/entries.c
 */

#ifndef CVSENTRIES_H
#define CVSENTRIES_H

#include "SortList.h"
#include "CvsIgnore.h"
#include "cvsgui_i18n.h"
#include "CvsPrefs.h"

#ifdef TARGET_OS_MAC
# include "TextBinary.h" // for definition of UFSSpec
#endif

#ifndef _countof
        /// Get the size of an array
#       define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

time_t GetDateTime(const char* strTime);
int GetStateOrder(const char* state);

class KoColumnContext;

/// Entry type enum
typedef enum
{
	ENT_FILE,	/*!< File entry */
	ENT_SUBDIR	/*!< Directory entry */
} ent_type;

/// Column index class
class ColumnIndex
{
private:
	// Construction
	ColumnIndex();

public:
	/// Column index enum
	enum
	{
		kName = 0,	/*!< Name */
		kExtention,	/*!< Extension */
		kRevision,	/*!< Revision number */
		kOption,	/*!< Keyword expansion option */
		kState,		/*!< File state */
		kEncoding,	/*!< File Encoding */
		kStickyTag,	/*!< Sticky tag */
		kTimeStamp,	/*!< Time stamp */
		kConflict,	/*!< Conflict */
		kInfo,		/*!< Info */
		kPath,		/*!< Path */
		kModTime,	/*!< Modification time */
		kEnd		/*!< Marker of the last index only */
	};

	static const char* GetColumnName(const int index, const bool shortName = true);
};

#define STR_STATE_IGNORED_T		"Ignored"
#define STR_STATE_UNKNOWN_T		"Unknown"
#define STR_STATE_CONFLICT_T	"Conflict"
#define STR_STATE_RESOLVED_T	"Resolved"
#define STR_STATE_MODIFIED_T	"Modified"
#define STR_STATE_REMOVED_T		"Removed"
#define STR_STATE_ADDED_T		"Added"
#define STR_STATE_MISSING_T		"Missing"

#define STR_STATE_IGNORED		_i18n(STR_STATE_IGNORED_T)
#define STR_STATE_UNKNOWN		_i18n(STR_STATE_UNKNOWN_T)
#define STR_STATE_CONFLICT		_i18n(STR_STATE_CONFLICT_T)
#define STR_STATE_RESOLVED		_i18n(STR_STATE_RESOLVED_T)
#define STR_STATE_MODIFIED		_i18n(STR_STATE_MODIFIED_T)
#define STR_STATE_REMOVED		_i18n(STR_STATE_REMOVED_T)
#define STR_STATE_ADDED  		_i18n(STR_STATE_ADDED_T)
#define STR_STATE_MISSING		_i18n(STR_STATE_MISSING_T)

/*!
	Class to support reference counted full path description
	\note Reference-counted, we don't use delete on it
*/
class EntnodePath
{
public:
	// Construction
	EntnodePath(const char* path);
protected:
	virtual ~EntnodePath();

public:
	// Interface
	EntnodePath* Ref();
	EntnodePath* UnRef();
	const char* GetFullPathName(std::string& resPath, const char* name) const;
	const char* GetFolderPath() const;

protected:
	// Data members
	std::string m_fullpath;	/*!< Fully qualified path of the folder containing the item */
	int m_ref;				/*!< Reference count */
};

/*!
	Abstract class to hold the node data
	\note Reference-counted, we don't use delete on it
*/
class EntnodeData
{
protected:
	// Construction
	EntnodeData(const char* name, EntnodePath* path);
	virtual ~EntnodeData();

public:
	/// Enum node types
	enum
	{
		kName = ColumnIndex::kName,
		kState = ColumnIndex::kState,
		kPath = ColumnIndex::kPath
	};

	// Interface
	
	/// Get the entry type
	virtual ent_type GetType() const = 0;

	EntnodeData* Ref();
	EntnodeData* UnRef();
	
	static int Compare(const EntnodeData& node1, const EntnodeData& node2);
	virtual const std::string& operator[](const int index) const;

	void SetMissing(bool state);
	bool IsMissing() const;
	void SetVisited(bool state);
	bool IsVisited() const;
	void SetUnknown(bool state);
	bool IsUnknown() const;
	void SetIgnored(bool state);
	bool IsIgnored() const;
	void SetLocked(bool state);
	bool IsLocked() const;
	void SetRemoved(bool state);
	bool IsRemoved() const;
	void SetState(const char* state);
	const char* GetState() const;

	const char* GetName() const;
	const char* GetFullPathName(std::string& resPath) const;

	const char* GetFolderPath() const;

	virtual const char* GetExtension() const;
	virtual const char* GetConflict() const;

	void SetUnmodified(bool state);
	bool IsUnmodified() const;

	void SetResolved(bool state);
	bool IsResolved() const;
	
	void SetAdded(bool state);
	bool IsAdded() const;
	
#ifdef TARGET_OS_MAC
	UFSSpec& MacSpec();
	const UFSSpec& GetMacSpec() const;
#endif /* TARGET_OS_MAC */

#ifdef WIN32
	static inline bool IsUpOneNode(const EntnodeData* data);
#endif

protected:
	// Data members
	int m_ref;	/*!< Reference counter */

	int m_missing		: 1;		/*!< Missing flag */
	int m_visited		: 1;		/*!< Visited flag */
	int m_unknown		: 1;		/*!< Unknown flag */
	int m_unmodified	: 1;		/*!< Unmodified flag */
	int m_ignored		: 1;		/*!< Ignored flag */
	int m_locked		: 1;		/*!< Locked flag (readonly) */
	int m_removed		: 1;		/*!< Removed flag */
	int m_added			: 1;		/*!< Added flag */
	int m_resolved		: 1;		/*!< Conflict resolved flag */

#ifdef TARGET_OS_MAC
	UFSSpec macspec;	/*!< Macspec */
#endif /* TARGET_OS_MAC */

private:
	// Data members
	std::string m_name;				/*!< Name of this item */
	std::string m_state;			/*!< Item's state*/
	EntnodePath* m_fullPathNode;	/*!< Fully qualified path of the folder containing the item */
	mutable std::string m_fullPathName;	/*!< Full path */
};

/*!
	Abstract class to hold the file node data
	\note Reference-counted, we don't use delete on it
*/
class EntnodeFile : public EntnodeData
{
public:
	// Construction
	EntnodeFile(const char* name, EntnodePath* path, const char* vn = NULL,
		const char* ts = NULL, const char* option = NULL,
		const char* tag = NULL, const char* date = NULL,
		const char* conflict = NULL);
protected:
	virtual ~EntnodeFile();

public:
	/// Enum file node types
	enum
	{
		kExt = ColumnIndex::kExtention,
		kVN = ColumnIndex::kRevision,
		kTS = ColumnIndex::kTimeStamp,
		kOption = ColumnIndex::kOption,
		kEncoding = ColumnIndex::kEncoding,
		kTag = ColumnIndex::kStickyTag,
		kConflict = ColumnIndex::kConflict,
		kInfo = ColumnIndex::kInfo,
		kModTime = ColumnIndex::kModTime
	};

	// Interface
	virtual const std::string& operator[](const int index) const;
	virtual ent_type GetType() const;
	virtual const char* GetExtension() const;
	virtual const char* GetConflict() const;
	
	virtual const char* GetRelativePath(const KoColumnContext* context, const bool forceCreate = false) const;

	void SetEncoding(const char* encoding);
	void SetModTime(const time_t modTime);
	time_t GetModTime() const;

protected:
	// Data members
	std::string m_ext;		/*!< Extension */
	std::string m_vn;		/*!< Revision */
	std::string m_ts;		/*!< Timestamp */
	std::string m_option;	/*!< Option */
	std::string m_encoding;	/*!< Encoding */
	std::string m_tag;		/*!< Tag, may be empty */
	std::string m_date;		/*!< Date, may be empty */
	std::string m_conflict;	/*!< Conflict, may be empty */
	
	std::string m_info;		/*!< Info, if it isn't a valid datetime then it will point to the timestamp and so it should not be freed */

	mutable std::string m_relativePath;	/*!< Path relative to the column context path */
	time_t m_modTime;		/*!< Modification time */
	std::string m_modified;	/*!< Modification time as string, may be empty */
};

/*!
	Abstract class to hold the folder node data
	\note Reference-counted, we don't use delete on it
*/
class EntnodeFolder : public EntnodeData
{
public:
// Construction
	EntnodeFolder(const char* name, EntnodePath* path, const char* vn = NULL,
		const char* ts = NULL, const char* option = NULL, const char* tag = NULL,
		const char* date = NULL, const char* conflict = NULL);
protected:
	virtual ~EntnodeFolder();

public:
	// Interface
	virtual const std::string& operator[](const int index) const;
	virtual ent_type GetType() const;
};

/// Allow reference of a file or a folder node
class ENTNODE
{
public:
	// Construction
	ENTNODE();
	ENTNODE(EntnodeData* data);
	ENTNODE(EntnodeData& data);
	ENTNODE(const ENTNODE& anode);
	virtual ~ENTNODE();

	// Interface
	static int Compare(const ENTNODE& node1, const ENTNODE& node2);
	ENTNODE& operator=(const ENTNODE& anode);
	EntnodeData* Data() const;
	operator EntnodeData*() const;

protected:
	// Data members
	EntnodeData* shareData; /*!< Data */
};

bool Entries_Open(CSortList<ENTNODE>& entries, const char* fullpath);

EntnodeData* Entries_SetVisited(const char* path, CSortList<ENTNODE>& entries, const char* name,
	const struct stat& finfo, bool isFolder, const ignored_list_type* ignlist = NULL);

void Entries_SetMissing(CSortList<ENTNODE>& entries);
bool Tag_Open(std::string& tag, const char* fullpath);

//////////////////////////////////////////////////////////////////////////
// EntnodePath

/// Increase reference count
inline EntnodePath* EntnodePath::Ref()
{
	++m_ref; 
	return this;
}

/// Decrease reference count, delete self if reference count reached zero
inline EntnodePath* EntnodePath::UnRef()
{
	if( --m_ref == 0 )
	{
		delete this;
		return NULL;
	}
	
	return this;
}

/*!
	Get the folder path
	\return Folder path
*/
inline const char* EntnodePath::GetFolderPath() const
{
	return m_fullpath.c_str();
}

//////////////////////////////////////////////////////////////////////////
// EntnodeData

/// Increase reference count
inline EntnodeData* EntnodeData::Ref()
{
	++m_ref;
	return this;
}

/// Decrease reference count, delete self if reference count reached zero
inline EntnodeData* EntnodeData::UnRef()
{
	if( --m_ref == 0 )
	{
		delete this;
		return NULL;
	}
	
	return this;
}

/*!
	Compare two nodes data
	\param node1 First node
	\param node2 Second node
	\return Less than zero if node1 is less than node2, zero if nodes are equal, more than zero if node2 is less than node1
	\note For windows comparison is not case sensitive
*/
inline int EntnodeData::Compare(const EntnodeData& node1, const EntnodeData& node2)
{
#if TARGET_OS_MAC
  CFOptionFlags   comparisonOptions(kCFCompareNonliteral|kCFCompareLocalized);
  if ( !gCvsPrefs.MatchEntriesCase() ) comparisonOptions |= kCFCompareCaseInsensitive;
  CFStringRef   s1 = CFStringCreateWithCStringNoCopy(NULL, node1.m_name, kCFStringEncodingUTF8, kCFAllocatorNull);
  CFStringRef   s2 = CFStringCreateWithCStringNoCopy(NULL, node2.m_name, kCFStringEncodingUTF8, kCFAllocatorNull);
  if ( s1 && s2 ) {
    CFComparisonResult  rc = CFStringCompare(s1, s2, comparisonOptions);
    CFRelease(s1);
    CFRelease(s2);
    return (int)rc;
  }
  if ( s1 ) CFRelease(s1);
  if ( s2 ) CFRelease(s2);
#endif
	return gCvsPrefs.MatchEntriesCase() ? strcmp(node1.m_name.c_str(), node2.m_name.c_str()) : stricmp(node1.m_name.c_str(), node2.m_name.c_str());
}

/// Set the missing flag
inline void EntnodeData::SetMissing(bool state)
{
	m_missing = state;
}

/// Get the missing flag
inline bool EntnodeData::IsMissing() const
{
	return m_missing != 0L;
}

/// Set the visited flag
inline void EntnodeData::SetVisited(bool state)
{
	m_visited = state;
}

/// Get the visited flag
inline bool EntnodeData::IsVisited() const
{
	return m_visited != 0L;
}

/// Set the unknown flag
inline void EntnodeData::SetUnknown(bool state)
{
	m_unknown = state;
}

/// Get the unknown flag
inline bool EntnodeData::IsUnknown() const
{
	return m_unknown != 0L;
}

/// Set the ignored flag
inline void EntnodeData::SetIgnored(bool state)
{
	m_ignored = state;
}

/// Get the ignored flag
inline bool EntnodeData::IsIgnored() const
{
	return m_ignored != 0L;
}

/// Set the locked flag
inline void EntnodeData::SetLocked(bool state)
{
	m_locked = state;
}

/// Get the locked flag
inline bool EntnodeData::IsLocked() const
{
	return m_locked != 0L;
}

/// Set the removed flag
inline void EntnodeData::SetRemoved(bool state)
{
	m_removed = state;
}

/// Get the removed flag
inline bool EntnodeData::IsRemoved() const
{
	return m_removed != 0L;
}


/// Set the state
inline void EntnodeData::SetState(const char* state)
{
	m_state = state;
}

/// Get the state
inline const char* EntnodeData::GetState() const
{
	return m_state.c_str();
}

/// Set the encoding
inline void EntnodeFile::SetEncoding(const char* encoding)
{
	m_encoding = encoding;
}

/// Set the modification time
inline void EntnodeFile::SetModTime(const time_t modTime)
{
	m_modTime = modTime;
	
	if( m_modTime != (time_t)-1 )
	{
#ifdef WIN32
		// Formatting according to the system settings
		COleDateTime fileTime(modTime);

		m_modified = fileTime.Format();
#else
		char* cp;
		
		if( struct tm* tm_p = gmtime(&m_modTime) )
		{
			struct tm local_tm = { 0 };
			
			memcpy(&local_tm, tm_p, sizeof(local_tm));
			cp = asctime(&local_tm);
		}
		else
			cp = ctime(&m_modTime);
		
		cp[24] = 0;
		
		m_modified = cp;
#endif
	}
}

/// Get the modification time
inline time_t EntnodeFile::GetModTime() const
{
	return m_modTime;
}

/// Get the item name
inline const char* EntnodeData::GetName() const 
{
	return m_name.c_str();
}

/// Get the fully qualified item name
inline const char* EntnodeData::GetFullPathName(std::string& resPath) const 
{
	return m_fullPathNode->GetFullPathName(resPath, m_name.c_str());
}

/// Get the fully qualified folder name of the item
inline const char* EntnodeData::GetFolderPath() const
{
	return m_fullPathNode->GetFolderPath();
}

/// Get the conflict marker
inline const char* EntnodeData::GetConflict() const
{
	return "";
}

/// Get the extension
inline const char* EntnodeData::GetExtension() const
{
	return "";
}

/// Set the unmodified flag
inline void EntnodeData::SetUnmodified(bool state)
{
	m_unmodified = state;
}

/// Get the unmodified flag
inline bool EntnodeData::IsUnmodified() const
{
	return m_unmodified != 0L;
}

/// Set the resolved flag
inline void EntnodeData::SetResolved(bool state)
{
	m_resolved = state;
}

/// Get the resolved flag
inline bool EntnodeData::IsResolved() const
{
	return m_resolved != 0;
}

/// Set the added flag
inline void EntnodeData::SetAdded(bool state)
{
	m_added = state;
}

/// Get the added flag
inline bool EntnodeData::IsAdded() const
{
	return m_added != 0;
}

#ifdef TARGET_OS_MAC
/// Access macspec
inline UFSSpec& EntnodeData::MacSpec()
{
	return macspec;
}

/// Get macspec
inline const UFSSpec& EntnodeData::GetMacSpec() const
{
	return macspec;
}
#endif /* TARGET_OS_MAC */

#ifdef WIN32
/*!
	Check whether node data represents an "up one" item
	\param data Node data
	\return true if item is an up-one node, false otherwise
*/
inline bool EntnodeData::IsUpOneNode(const EntnodeData* data)
{
	return !data;
}
#endif

//////////////////////////////////////////////////////////////////////////
// ENTNODE

inline ENTNODE::ENTNODE() : shareData(NULL)
{
}

inline ENTNODE::ENTNODE(EntnodeData* data) : shareData(data->Ref())
{
}

inline ENTNODE::ENTNODE(EntnodeData& data) : shareData(data.Ref())
{
}

inline ENTNODE::ENTNODE(const ENTNODE& anode) : shareData(NULL)
{
	*this = anode;
}

inline ENTNODE::~ENTNODE()
{
	if( shareData != NULL )
		shareData->UnRef();
}

/*!
	Compare two ENTNODE items
	\param node1 First item
	\param node2 Second item
	\return Less than zero if node1 is less than node2, zero if nodes are equal, more than zero if node2 is less than node1
*/
inline int ENTNODE::Compare(const ENTNODE& node1, const ENTNODE& node2)
{
	return EntnodeData::Compare(*node1.shareData, *node2.shareData);
}

/// Access data
inline EntnodeData* ENTNODE::Data() const
{
	return shareData;
}

/// Operator EntnodeData*
inline ENTNODE::operator EntnodeData*() const
{
	return shareData;
}

#endif /* CVSENTRIES_H */
