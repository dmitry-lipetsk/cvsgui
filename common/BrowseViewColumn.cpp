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

#include "stdafx.h"

#include "FileTraversal.h"
#include "BrowseViewColumn.h"
#include "CvsEntries.h"
#include "cvsgui_i18n.h"

#if TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
#elif HAVE_SYS_TIME_H
#	include <sys/time.h>
#else
#	include <time.h>
#endif

#if qMacPP
#	include "MacMisc.h"
#endif

using namespace std;

#if qMacPP
# define PLATFORM_COLUMN_WIDTH(mac, other) mac
#else
# define PLATFORM_COLUMN_WIDTH(mac, other) other
#endif

#ifndef MAX_PATH
#define MAX_PATH 2048
#endif

#if TARGET_OS_MAC
static int CFStrOrdICmp(const char* s1, const char* s2)
{
  CFStringRef         str1 = CFStringCreateWithCStringNoCopy(NULL, s1, kCFStringEncodingUTF8, kCFAllocatorNull);
  CFStringRef         str2 = CFStringCreateWithCStringNoCopy(NULL, s2, kCFStringEncodingUTF8, kCFAllocatorNull);
  int                 rc;
  
  if ( str1 != NULL  &&  str2 != NULL )
  {
    rc = CFStringCompare(str1, str2, kCFCompareCaseInsensitive|kCFCompareNonliteral|kCFCompareLocalized);
    if ( rc == 0 ) 
      rc = CFStringCompare(str1, str2, kCFCompareNonliteral|kCFCompareLocalized);
  }
  else rc = StrOrdICmp(s1, s2);
   
  if ( str1 ) CFRelease(str1);
  if ( str2 ) CFRelease(str2);
  
  return rc;
}
#endif


/*!
	Compare callback function
	\param data1 First node data
	\param data2 Second node data
	\param data Sort parameters data
	\return -1, 0 or +1
*/
#ifdef WIN32
int CALLBACK _Compare(LPARAM data1, LPARAM data2, LPARAM data)
#else
int _Compare(void* data1, void* data2, void* data)
#endif
{
	int res = 0;
	
	const EntnodeData* d1 = (const EntnodeData*)data1;
	const EntnodeData* d2 = (const EntnodeData*)data2;
	
	if( EntnodeData::IsUpOneNode(d1) || EntnodeData::IsUpOneNode(d2) )
	{
		return EntnodeData::IsUpOneNode(d1) ? -1 : 1;
	}

	const CSortParam* pSortParam = (CSortParam*)data;
	
	bool overrideCompare = false;
	
	if( pSortParam->m_fGroupSubDir )
	{
		// Group sub-directories
		const int d1IsDir = d1->GetType() == ENT_SUBDIR ? 1 : 0;
		const int d2IsDir = d2->GetType() == ENT_SUBDIR ? 1 : 0;
		
		if( (d1IsDir + d2IsDir) == 1 )
		{
			// One (and only one of them) is a directory
			overrideCompare = true;
			res = d1IsDir ? -1 : 1;
		}
	}
	
	if( !overrideCompare )
	{
		const int columnIndex = pSortParam->m_column->GetColumnIndex();

		// Make the non-empty items show on top
		const string& s1 = (*d1)[columnIndex];
		const string& s2 = (*d2)[columnIndex];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			// One (and only one of them) is empty (if both are empty then compare function may choose different criteria to decide)
			overrideCompare = true;
			res = s1IsEmpty ? 1 : -1;
		}
	}
	
	if( !overrideCompare )
	{
		res = pSortParam->m_column->Compare(pSortParam->m_context, d1, d2);
		
		if( res == 0 && pSortParam->m_column != pSortParam->m_columnAlt )
		{
			res = pSortParam->m_columnAlt->Compare(pSortParam->m_context, d1, d2);
		}
		else if( pSortParam->m_fAccendant )
		{
			res = -res;
		}
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////
// KoColumnContext

KoColumnContext::KoColumnContext(const char* path)
	: m_path(path)
{
}	

/*!
	Get the current path
	\return The current path
*/
const std::string& KoColumnContext::GetPath() const
{
	return m_path;
}

//////////////////////////////////////////////////////////////////////////
// KoNameColumn

/// Name column
const class KoNameColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kName;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 150;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = PLATFORM_COLUMN_WIDTH(250, 210);
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, data->GetName(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
#if TARGET_OS_MAC
		return CFStrOrdICmp(d1->GetName(), d2->GetName());
#else
		return StrOrdICmp(d1->GetName(), d2->GetName());
#endif
	}

	virtual bool IsDefaultAscending() const
	{
		return false;
	}
} nameColumn;

//////////////////////////////////////////////////////////////////////////
// KoExtColumn

/// Extension column
const class KoExtColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kExtention;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 50;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = PLATFORM_COLUMN_WIDTH(150, 60);
#endif
	}
	
	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, data->GetExtension(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res = 0;

		// Compare extensions
		const char* s1 = d1->GetExtension();
		const char* s2 = d2->GetExtension();
		const int s1IsEmpty = IsNullOrEmpty(s1) ? 1 : 0;
		const int s2IsEmpty = IsNullOrEmpty(s2) ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			res = stricmp(s1, s2);
			if( res == 0 ) 
			{
				// Compare filenames, if ext. are identical
				res = stricmp(d1->GetName(), d2->GetName());
			}
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			 res = s1IsEmpty ? 1 : -1;
		}
		else
		{
			// If there are no extensions then compare filenames
			res = stricmp(d1->GetName(), d2->GetName());
		}
		
		return res;
	}

	virtual bool IsDefaultAscending() const
	{
		return false;
	}
} extColumn;

//////////////////////////////////////////////////////////////////////////
// KoPathColumn

/// Path column
const class KoPathColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kPath;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 150;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = PLATFORM_COLUMN_WIDTH(340, 300);
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{				
		const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data);
		StrSafeCopy(buffer, node ? node->GetRelativePath(context) : "", bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		char text1[MAX_PATH];
		GetText(context, d1, text1, MAX_PATH);
		
		char text2[MAX_PATH];
		GetText(context, d2, text2, MAX_PATH);
		
#if TARGET_OS_MAC
		return CFStrOrdICmp(text1, text2);
#else
		return StrOrdICmp(text1, text2);
#endif
	}

	virtual bool IsDefaultAscending() const
	{
		return false;
	}
} pathColumn;

//////////////////////////////////////////////////////////////////////////
// KoRevisionColumn

/// Revision column	
const class KoRevisionColumn : public KiColumn 
{
private:
	// Methods

	/*!
		Compare two revisions
		\param rev1 First revision
		\param rev2 Second revision
		\return -1, 0 or 1
	*/
	static int CompareRevisions(const char* rev1, const char* rev2)
	{
		const int rev1IsEmpty = IsNullOrEmpty(rev1) ? 1 : 0;
		const int rev2IsEmpty = IsNullOrEmpty(rev2) ? 1 : 0;

		if( (rev1IsEmpty + rev2IsEmpty) == 2 )
		{
			// Both empty
			return 0;
		}
		else if( (rev1IsEmpty + rev2IsEmpty) == 1 )
		{
			// One empty
			return rev1IsEmpty ? -1 : 1;
		}

		const int v1 = atoi(rev1);
		const int v2 = atoi(rev2);

		if( v1 == v2 )
		{
			string q1;
			if( const char* pos = strchr(rev1, '.') )
			{
				q1 = pos + 1;
			}
			
			string q2;
			if( const char* pos = strchr(rev2, '.') )
			{
				q2 = pos + 1;
			}
			
			return CompareRevisions(q1.c_str(), q2.c_str());
		}

		return v1 < v2 ? -1 : 1;
	}

public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kRevision;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 50;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = PLATFORM_COLUMN_WIDTH(82, 70);
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res;

		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			res = CompareRevisions(s1.c_str(), s2.c_str());
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}

		return res;
	}
} revisionColumn;

//////////////////////////////////////////////////////////////////////////
// KoOptionColumn

/// Option column
const class KoOptionColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kOption;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 50;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = 55;
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const 
	{
		int res;

		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			res = strcmp(s1.c_str(), s2.c_str());
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}

		return res;
	}
} optionColumn;

//////////////////////////////////////////////////////////////////////////
// KoEncodingColumn

/// Encoding column
const class KoEncodingColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kEncoding;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 60;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = 60;
#endif
	}
	
	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}
	
	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res;
		
		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			res = stricmp(s1.c_str(), s2.c_str());
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}
		
		return res;
	}
} encodingColumn;

//////////////////////////////////////////////////////////////////////////
// KoStateColumn

/// State column
const class KoStateColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kState;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 70;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = PLATFORM_COLUMN_WIDTH(76, 70);
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res;
		
		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			if( gCvsPrefs.SmartSorting() )
			{
				const int i1 = GetStateOrder(s1.c_str());
				const int i2 = GetStateOrder(s2.c_str());

				if( i1 >= 0 || i2 >= 0 )
				{
					res = i2 - i1;
				}
				else 
				{
#ifdef WIN32
					ASSERT(FALSE); // Unknown state
#endif
					res = stricmp(s1.c_str(), s2.c_str());
				}
			}
			else
			{
				res = stricmp(s1.c_str(), s2.c_str());
			}
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}
		
		return res;
	}
} stateColumn;

//////////////////////////////////////////////////////////////////////////
// KoTagColumn

/// Tag column
const class KoTagColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kStickyTag;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 150;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = 90;
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res = 0;

		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			res = strcmp(s1.c_str(), s2.c_str());
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}

		return res;

	}
} tagColumn;

//////////////////////////////////////////////////////////////////////////
// KoDateColumn

/// Date column
const class KoDateColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kTimeStamp;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = (IsLargeFonts()) ? 170 : 150;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = PLATFORM_COLUMN_WIDTH(172, 135);
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res = 0;
		
		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			const time_t t1 = GetDateTime(s1.c_str());
			const time_t t2 = GetDateTime(s2.c_str());

			res = t1 < t2 ? -1 : (t1 > t2 ? 1 : 0);
		} 
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}

		return res;
	}
} dateColumn;

//////////////////////////////////////////////////////////////////////////
// KoConflictColumn

/// Conflict column
const class KoConflictColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kConflict;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 150;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = 100;
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, data->GetConflict(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res;

		const char* s1 = d1->GetConflict();
		const char* s2 = d2->GetConflict();
		const int s1IsEmpty = IsNullOrEmpty(s1) ? 1 : 0;
		const int s2IsEmpty = IsNullOrEmpty(s2) ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			res = strcmp(s1, s2);
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}

		return res;
	}
} conflictColumn;

//////////////////////////////////////////////////////////////////////////
// KoInfoColumn

/// Info column
const class KoInfoColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kInfo;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = 150;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = 100;
#endif
	}
	
	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}
	
	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res;
		
		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			res = stricmp(s1.c_str(), s2.c_str());
		}
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}
		
		return res;
	}
} infoColumn;

//////////////////////////////////////////////////////////////////////////
// KoModifiedColumn

/// Modified column
const class KoModifiedColumn : public KiColumn 
{
public:
	// Interface
	virtual int GetColumnIndex() const
	{
		return ColumnIndex::kModTime;
	}
	
	virtual void GetSetupData(LV_COLUMN* lvc) const
	{
#ifdef WIN32
		lvc->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvc->pszText = (char*)ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->cx = (IsLargeFonts()) ? 170 : 150;
		lvc->fmt = LVCFMT_LEFT;
#else
		lvc->name = ColumnIndex::GetColumnName(GetColumnIndex());
		lvc->width = PLATFORM_COLUMN_WIDTH(172, 135);
#endif
	}

	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const
	{
		StrSafeCopy(buffer, ((*data)[GetColumnIndex()]).c_str(), bufferSize);
	}

	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const
	{
		int res = 0;
		
		const string& s1 = (*d1)[GetColumnIndex()];
		const string& s2 = (*d2)[GetColumnIndex()];
		const int s1IsEmpty = s1.empty() ? 1 : 0;
		const int s2IsEmpty = s2.empty() ? 1 : 0;
		
		if( (s1IsEmpty + s2IsEmpty) == 0 ) 
		{
			const EntnodeFile* f1 = dynamic_cast<const EntnodeFile*>(d1);
			const EntnodeFile* f2 = dynamic_cast<const EntnodeFile*>(d2);

			if( f1 && f2 )
			{
				const time_t t1 = f1->GetModTime();
				const time_t t2 = f2->GetModTime();
				
				res = t1 < t2 ? -1 : (t1 > t2 ? 1 : 0);
			}
			else
			{
				res = (long)f1 < (long)f2 ? -1 : ((long)f1 > (long)f2 ? 1 : 0);
			}
		} 
		else if( (s1IsEmpty + s2IsEmpty) == 1 )
		{
			res = s1IsEmpty ? -1 : 1;
		}
		else
		{
			res = 0;
		}

		return res;
	}
} modifiedColumn;

//////////////////////////////////////////////////////////////////////////
// Columns

/// All columns for regular view
static const KiColumn* columnsRegular[] = {
	&nameColumn, &extColumn, 
	&revisionColumn, &optionColumn, &encodingColumn, &stateColumn, 
	&tagColumn, &infoColumn, &modifiedColumn
	//, &dateColumn, &conflictColumn
};

/// All columns types for regular view
static const int typesRegular[] = {
	EntnodeData::kName, EntnodeFile::kExt, 
	EntnodeFile::kVN, EntnodeFile::kOption, EntnodeFile::kEncoding, EntnodeData::kState,
	EntnodeFile::kTag, EntnodeFile::kInfo, EntnodeFile::kModTime
	// , EntnodeFile::kTS, EntnodeFile::kConflict
};

/// All columns for flat view
static const KiColumn* columnsRecursive[] = {
	&nameColumn, &extColumn, &pathColumn, 
	&revisionColumn, &optionColumn, &encodingColumn, &stateColumn, 
	&tagColumn, &infoColumn, &modifiedColumn
	// , &dateColumn, &conflictColumn
};

/// All columns types for flat view
static const int typesRecursive[] = {
	EntnodeData::kName, EntnodeFile::kExt, EntnodeFile::kPath, 
	EntnodeFile::kVN, EntnodeFile::kOption, EntnodeFile::kEncoding, EntnodeData::kState,
	EntnodeFile::kTag, EntnodeFile::kInfo, EntnodeFile::kModTime
	// , EntnodeFile::kTS, EntnodeFile::kConflict
};

//////////////////////////////////////////////////////////////////////////
// KoRegularModel

/// Regular columnar model
class KoRegularModel : public KiColumnModel
{
public:
	// Interface
	virtual const KiColumn* GetAt(const int pos) const
	{
		return columnsRegular[pos];
	}
	
	virtual int GetType(const int pos) const
	{
		return typesRegular[pos];
	}

	virtual int GetCount() const
	{
		return _countof(columnsRegular);
	}
} modelRegular;

//////////////////////////////////////////////////////////////////////////
// KoRecursiveModel

/// Recursive columnar model
class KoRecursiveModel : public KiColumnModel
{	
public:
	// Interface
	virtual const KiColumn* GetAt(const int pos) const
	{
		return columnsRecursive[pos];
	}

	virtual int GetType(const int pos) const
	{
		return typesRecursive[pos];
	}

	virtual int GetCount() const
	{
		return _countof(columnsRecursive);
	}
} modelRecursive;

//////////////////////////////////////////////////////////////////////////
// KiColumnModel

/*!
	Get the recursive model
	\return The recursive model
*/
KiColumnModel* KiColumnModel::GetRecursiveModel()
{
	return &modelRecursive;
}
	
/*!
	Get the regular model
	\return The regular model
*/
KiColumnModel* KiColumnModel::GetRegularModel()
{
	return &modelRegular;
}

/*!
	Get the column index by its type
	\param type Column type
	\return Column index
*/
int KiColumnModel::GetColumn(const int type) const
{
	const int numColumns = GetCount();

	for(int i = 0; i < numColumns; i++)
	{
		if( type == GetType(i) )
		{
			return i;
		}
	}
	
	return 0;
}
