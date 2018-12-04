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
 */

/*
 * CvsEntries.cpp --- adaptation from cvs/src/entries.c
 */

#include "stdafx.h"

#ifdef TARGET_OS_MAC
#	include "GUSIInternal.h"
#	include "GUSIFileSpec.h"
#	define S_IWRITE S_IWUSR
#endif

#include <stdlib.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

#include <sstream>
#include <algorithm>

#include "CvsEntries.h"
#include "AppConsole.h"
#include "getline.h"
#include "FileTraversal.h"
#include "BrowseViewColumn.h"

#ifdef WIN32
#	ifdef _DEBUG
#		define new DEBUG_NEW
#		undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
#	endif
#	include "WinCvsDebug.h"
#endif /* WIN32 */

#if TIME_WITH_SYS_TIME
#	include <sys/time.h>
#	include <time.h>
#elif HAVE_SYS_TIME_H
#	include <sys/time.h>
#else
#	include <time.h>
#endif

using namespace std;

/// Months table used to parse the date string
string gStrMonths[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

#define STR_ENCODING_TEXT			_i18n("Text")
#define STR_ENCODING_BINARY			_i18n("Binary")
#define STR_ENCODING_UNICODE		_i18n("Unicode")
#define STR_ENCODING_EXTENDED		_i18n("Extended")

const char* gPszStateOrder[] = {
	STR_STATE_MISSING_T,
	STR_STATE_CONFLICT_T,
	STR_STATE_ADDED_T,
	STR_STATE_REMOVED_T,
	STR_STATE_RESOLVED_T,
	STR_STATE_MODIFIED_T,
	STR_STATE_UNKNOWN_T,
	STR_STATE_IGNORED_T,
};

#define STR_INFO_CVSBACKUPCOPY	_i18n("<Backup copy>")

//////////////////////////////////////////////////////////////////////////
// Helper functions

/*!
	Extract the date-time information from the string
	\param strTime Date-time string as returned by <b>asctime</b> function (CVS/Entries file format)
	\return The date-time as time_t value or (time_t)-1 as returned by mktime
*/
time_t GetDateTime(const char* strTime)
{
	struct tm tmTime = { 0 };

	string stringTime = StrGetSafeStr(strTime);
	if( stringTime.empty() )
	{
		return -1;
	}

	// Replace the time separator with spaces to help with parsing
	replace(stringTime.begin(), stringTime.end(), ':', ' ');

	// Parse
	stringstream parser;
	parser << stringTime;
	
	string strDay;
	string strMonth;

	parser >> strDay;
	parser >> strMonth;
	{
		// Find out the month
		tmTime.tm_mon = -1;
		string* month = find(gStrMonths, gStrMonths + 12, strMonth);

		if( month != gStrMonths + 12 )
		{
			tmTime.tm_mon = month - gStrMonths;
		}
		else
		{
			return (time_t)(-1);
		}
	}

	parser >> tmTime.tm_mday;
	parser >> tmTime.tm_hour;
	parser >> tmTime.tm_min;
	parser >> tmTime.tm_sec;
	
	parser >> tmTime.tm_year;
	tmTime.tm_year -= 1900;

    tmTime.tm_isdst = -1;

	return mktime(&tmTime);
}

/*!
	Get the position of a textual state description in gPszStateOrder
	\param state Zero-terminated textual state description (STR_STATE_*)
	\return The position of this state in gPszStateOrder, or -1 if not found
*/
int GetStateOrder(const char* state)
{
	// Empty items at last
	if( IsNullOrEmpty(state) )
		return _countof(gPszStateOrder);

	for(int nIndex = 0; nIndex < _countof(gPszStateOrder); nIndex++)
	{
		if( strcmp(state, gPszStateOrder[nIndex]) == 0 )
			return nIndex;
	}

	return -1;
}

/*!
	Find the extension in the filename
	\param name File name
	\return The pointer to the extension, NULL if not found
	\note Folders and filenames starting with dot are not considered to have extensions
*/
static const char* FindExtension(const char* name)
{
	// Look for the last occurrence of '.'
	const char* ptr = strrchr(name, '.');
	
	// If Entry is a subfolder, there is no extension
	// If '.' is the first char, there is also no extension
	if( ptr == name )
	{
		ptr = NULL;
	}
	
	// Ignore '.'
	if( ptr ) 
	{
		++ptr;
	}

	return ptr;
}

//////////////////////////////////////////////////////////////////////////
// ColumnIndex

ColumnIndex::ColumnIndex()
{
}

/*!
	Get the name for a given column index
	\param index The column index
	\param shortName true to retrieve the short column name, false to get the long name
	\return The column name
*/
const char* ColumnIndex::GetColumnName(const int index, const bool shortName /*= true*/)
{
	switch( index )
	{
	case kName:
		return shortName ? _i18n("Name") : _i18n("Filename");
	case kExtention:
		return shortName ? _i18n("Ext") : _i18n("Extension");
	case kRevision:
		return shortName ? _i18n("Rev.") : _i18n("Revision");
	case kOption:
		return shortName ? _i18n("Option") : _i18n("Keyword Expansion Option");
	case kState:
		return _i18n("State");
	case kEncoding:
		return _i18n("Encoding");
	case kStickyTag:
		return shortName ? _i18n("Tag") : _i18n("Sticky Tag");
	case kTimeStamp:
		return _i18n("Timestamp");
	case kConflict:
		return _i18n("Conflict");
	case kInfo:
		return shortName ? _i18n("Info") : _i18n("Information");
	case kModTime:
		return shortName ? _i18n("Modified") : _i18n("Modification time");
	case kPath:
		return _i18n("Path");
	}

	return "";
}

//////////////////////////////////////////////////////////////////////////
// EntnodePath

EntnodePath::EntnodePath(const char* path) 
	: m_ref(1)
{
	m_fullpath = StrGetSafeStr(path);
}

EntnodePath::~EntnodePath()
{
}

/*!
	Get the full path for a given name
	\param resPath Return full path
	\param name File name
	\return Full path
*/
const char* EntnodePath::GetFullPathName(std::string& resPath, const char* name) const
{
	resPath = m_fullpath;
	NormalizeFolderPath(resPath);
	resPath += name;

	return resPath.c_str();
}

//////////////////////////////////////////////////////////////////////////
// EntnodeData

EntnodeData::EntnodeData(const char* name, EntnodePath* path)
	: m_ref(1), 
	m_missing(false), 
	m_visited(false), 
	m_unknown(false),
	m_unmodified(true), 
	m_ignored(false), 
	m_locked(false), 
	m_removed(false), 
	m_added(false), 
	m_resolved(false)
{
#if TARGET_RT_MAC_MACHO
	memset(&macspec, 0, sizeof(macspec));
#endif

	m_name = StrGetSafeStr(name);
	m_fullPathNode = path->Ref();
}

EntnodeData::~EntnodeData()
{
	if( m_fullPathNode )
		m_fullPathNode->UnRef();
}

/// Operator []
const std::string& EntnodeData::operator[](const int index) const
{
	const static string strEmpty = "";

	switch( index )
	{
	case kName:
		return m_name;
	case kState:
		return m_state;
	case kPath:
		if( m_fullPathName.empty() )
		{
			GetFullPathName(m_fullPathName);
		}

		return m_fullPathName;
	default:
		break;
	}

	return strEmpty;
}

//////////////////////////////////////////////////////////////////////////
// EntnodeFile

EntnodeFile::EntnodeFile(const char* name, EntnodePath* path, const char* vn /*= NULL*/,
	const char* ts /*= NULL*/, const char* option /*= NULL*/, 
	const char* tag /*= NULL*/,	const char* date /*= NULL*/, 
	const char* conflict /*= NULL*/) : EntnodeData(name, path)
{
	m_ext = StrGetSafeStr(FindExtension(name));
	m_vn = StrGetSafeStr(vn);
	m_ts = StrGetSafeStr(ts);
	m_option = StrGetSafeStr(option);
	m_tag = StrGetSafeStr(tag);
	m_date = StrGetSafeStr(date);
	m_conflict = StrGetSafeStr(conflict);

	if( vn != NULL && vn[0] == '-' )
		SetRemoved(true);

	// Select the info string
	if( !m_ts.empty() )
	{
		// If timestamp is not a valid date then show that as info
		const time_t dateTime = GetDateTime(m_ts.c_str());
		if( dateTime == (time_t)-1 )
		{
			m_info = m_ts;
		}
	}
	else if( name && '.' == name[0] && '#' == name[1] )
	{
		// Indicate that this file is a backup copy created by CVS command
		m_info = STR_INFO_CVSBACKUPCOPY;
	}

	m_modTime = (time_t)-1;
}

EntnodeFile::~EntnodeFile()
{
}

/// Operator [] to access fields
const std::string& EntnodeFile::operator[](const int index) const
{
	switch( index )
	{
	case kExt:
		return m_ext;
	case kVN:
		return m_vn;
	case kTS:
		return m_ts;
	case kOption:
		return m_option;
	case kEncoding:
		return m_encoding;
	case kTag:
		return !m_tag.empty() ? m_tag : m_date;
	case kConflict:
		return m_conflict;
	case kInfo:
		return m_info;
	case kModTime:
		return m_modified;
	default:
		break;
	}

	return EntnodeData::operator[](index);
}

/// Get the type
ent_type EntnodeFile::GetType() const
{
	return ENT_FILE;
}

/// Get the extension
const char* EntnodeFile::GetExtension() const
{
	return m_ext.c_str();
}

/// Get the conflict marker
const char* EntnodeFile::GetConflict() const
{
	return m_conflict.c_str();
}

/*!
	Get the relative path
	\param context Column context
	\param forceCreate true to force creation, false to reuse any previously extracted value
	\return The path extracted from full pathname
*/
const char* EntnodeFile::GetRelativePath(const KoColumnContext* context, const bool forceCreate /*= false*/) const
{
	if( m_relativePath.empty() || forceCreate )
	{
		char* folderPath = strdup(GetFolderPath());
		if( folderPath )
		{
			m_relativePath.erase();

			char* dupMem = folderPath;
			
			folderPath += context->GetPath().length();
			
			if( '\\' == *folderPath )
			{
				folderPath++;
			}

			const int lenPath = strlen(folderPath);
			m_relativePath = !lenPath ? "." : folderPath;
			
			free(dupMem);
		}
	}

	return m_relativePath.c_str();
}

//////////////////////////////////////////////////////////////////////////
// EntnodeFolder

EntnodeFolder::EntnodeFolder(const char* name, EntnodePath* path, const char* vn /*= NULL*/,
	const char* ts /*= NULL*/, const char* option /*= NULL*/, const char* tag /*= NULL*/,
	const char* date /*= NULL*/, const char* conflict /*= NULL*/) : EntnodeData(name, path)
{
}

EntnodeFolder::~EntnodeFolder()
{
}

/// Operator [] to access fields
const std::string& EntnodeFolder::operator[](const int index) const
{
	return EntnodeData::operator[](index);
}

/// Get the type
ent_type EntnodeFolder::GetType() const
{
	return ENT_SUBDIR;
}

//////////////////////////////////////////////////////////////////////////
// ENTNODE

/// Operator =
ENTNODE& ENTNODE::operator=(const ENTNODE& anode)
{
	EntnodeData* oldData = shareData;
	shareData = anode.shareData;
	
	if( shareData != NULL )
	{
		shareData->Ref();
	}
	
	if( oldData != NULL )
	{
		oldData->UnRef();			
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Static functions

/*!
	Get the next real Entries line
	\param fpin File
	\param path Path
	\param cmd Command
	\return The next real Entries line or NULL on end of file
	\note On error, prints an error message and returns NULL
*/
static EntnodeData* GetNextEntriesLine(FILE* fpin, const char* path, char* cmd = NULL)
{
	EntnodeData* ent = NULL;
	char* line = NULL;
	size_t line_chars_allocated = 0;
	register char* cp;
	ent_type type;
	char* l, *name, *vn, *ts, *option;
	char* tag_or_date, *tag, *date, *ts_conflict;
	int line_length;

	EntnodePath* thePath = new EntnodePath(path);

	while( (line_length = getline(&line, &line_chars_allocated, fpin)) > 0 )
	{
		l = line;

		/* If CMD is not NULL, we are reading an Entries.Log file.
		Each line in the Entries.Log file starts with a single
		character command followed by a space.	For backward
		compatibility, the absence of a space indicates an add
		command.  */
		if( cmd != NULL )
		{
			if( l[1] != ' ' )
				*cmd = 'A';
			else
			{
				*cmd = l[0];
				l += 2;
			}
		}

		type = ENT_FILE;

		if( l[0] == 'D' )
		{
			type = ENT_SUBDIR;
			++l;
			/* An empty D line is permitted; it is a signal that this
			Entries file lists all known subdirectories.  */
		}

		if( l[0] != '/' )
			continue;

		name = l + 1;
		if( (cp = strchr(name, '/')) == NULL )
			continue;

		*cp++ = '\0';
		vn = cp;
		if( (cp = strchr(vn, '/')) == NULL )
			continue;

		*cp++ = '\0';
		ts = cp;
		if( (cp = strchr(ts, '/')) == NULL )
			continue;

		*cp++ = '\0';
		option = cp;
		if( (cp = strchr(option, '/')) == NULL )
			continue;

		*cp++ = '\0';
		tag_or_date = cp;

		if( (cp = strchr(tag_or_date, '\n')) == NULL
#ifdef TARGET_OS_MAC
			&& (cp = strchr(tag_or_date, '\r')) == NULL
#endif
		)
			continue;

		*cp = '\0';
		tag = (char*)NULL;
		date = (char*)NULL;
		
		if( *tag_or_date == 'T' )
			tag = tag_or_date + 1;
		else if( *tag_or_date == 'D' )
			date = tag_or_date + 1;

		if( (ts_conflict = strchr(ts, '+')) )
		{
			*ts_conflict++ = '\0';
			
			// Fix reappearing conflicts with older cvs servers
			if( strstr(ts, "merge") == NULL )
				ts_conflict = ts;
		}

		if( !IsNullOrEmpty(name) )
		{
			if( type == ENT_SUBDIR )
				ent = new EntnodeFolder(name, thePath);
			else
				ent = new EntnodeFile(name, thePath, vn, ts, option, tag, date, ts_conflict);
		}
		else
		{
			continue;
		}

		break;
	}

	if( line_length < 0 && !feof(fpin) )
		cvs_err("Cannot read entries file (error %d)\n", errno);

	free(line);

	thePath->UnRef();

	return ent;
}

/*!
	Read the entries file into a list, hashing on the file name
	\param entries Return file list
	\param fullpath Path to read files from
	\return true on success, false otherwise
	\note UPDATE_DIR is the name of the current directory, for use in error
          messages, or NULL if not known (that is, no-one has gotten around
          to updating the caller to pass in the information)
*/
bool Entries_Open(CSortList<ENTNODE>& entries, const char* fullpath)
{
	string adminPath(fullpath);
	NormalizeFolderPath(adminPath);
	adminPath += "CVS";
	adminPath += kPathDelimiter;
	
	entries.Reset();

	string filename = adminPath;
	filename += "Entries";
	
	FILE* fpin = fopen(filename.c_str(), "r");
	if( fpin == NULL )
		return false;

	EntnodeData* ent;
	
	while( (ent = GetNextEntriesLine(fpin, fullpath)) != NULL )
	{
		ENTNODE newnode(ent);
		ent->UnRef();

		if( entries.InsideAndInsert(newnode) )
		{
			cvs_err("Warning : duplicated entry in the 'CVS/Entries' file in directory '%s'\n", fullpath);
		}
	}

	fclose(fpin);

	filename = adminPath;
	filename += "Entries.log";
	
	fpin = fopen(filename.c_str(), "r");
	if( fpin != NULL )
	{
		char cmd;

		while( (ent = GetNextEntriesLine(fpin, &cmd)) != NULL )
		{
			ENTNODE newnode(ent);
			ent->UnRef();

			switch (cmd)
			{
			case 'A':
				if(entries.InsideAndInsert(newnode))
				{
					// Don't care
				}
				break;
			case 'R':
				entries.Delete(newnode);
				break;
			default:
				/* Ignore unrecognized commands */
				break;
			}
		}

		fclose(fpin);
	}

	return true;
}

/*!
	Verify that file is not modified
	\param sb Stat information
	\param ts Timestamp
	\return true if file is unmodified, false otherwise
	\note Contains the work-around for DST bug
*/
static bool VerifyUnmodified(const struct stat& sb, const char* ts)
{
	char* cp;
	struct tm* tm_p;
	struct tm local_tm = { 0 };
	
	/* We want to use the same timestamp format as is stored in the
	st_mtime.  For unix (and NT I think) this *must* be universal
	time (UT), so that files don't appear to be modified merely
	because the timezone has changed.  For VMS, or hopefully other
	systems where gmtime returns NULL, the modification time is
	stored in local time, and therefore it is not possible to cause
	st_mtime to be out of sync by changing the timezone.  */
	
	tm_p = gmtime(&sb.st_mtime);
	if( tm_p )
	{
		memcpy(&local_tm, tm_p, sizeof(local_tm));
		cp = asctime(&local_tm);	/* copy in the modify time */
	}
	else
		cp = ctime(&sb.st_mtime);

	if( !cp) {
		/* we have an invalid date in CVS/Entries, or a date far behind Unix bounds */
		return false; /* we always assume this Modified! */
	}
	cp[24] = 0;

	/* Deal with the differences in timestamps - POSIX uses space filled, NT uses zero filled */
	if( cp[8]=='0' && ts[8]==' ' ) 
		cp[8]=' ';
	
	if( cp[8]==' ' && ts[8]=='0' ) 
		cp[8]='0';

	return strcmp(cp, ts) == 0;
}

/*!
	Set the file encoding description
	\param data File node data
*/
static void SetEncoding(EntnodeFile* data)
{
	if( data && !data->IsUnknown() )
	{
		const char* encoding = STR_ENCODING_TEXT;
		
		const string& info = (*data)[EntnodeFile::kOption];
		if( !info.empty() )
		{
			const static char* encodings = "tbBu{";
			const string::size_type pos = info.find_first_of(encodings);
			if( pos != string::npos )
			{
				switch( info[pos] )
				{
				case 'b':
				case 'B':
					encoding = STR_ENCODING_BINARY;
					break;
				case 'u':
					encoding = STR_ENCODING_UNICODE;
					break;
				case '{':
					encoding = STR_ENCODING_EXTENDED;
					break;
				}
			}
		}

		((EntnodeFile*)data)->SetEncoding(encoding);
	}
}

/*!
	Fill an ENTNODE when the file appears on the disk
	\param path Path
	\param entries Entries list
	\param name Filename
	\param finfo File stat info
	\param isFolder true for folder, false otherwise
	\param ignlist Ignore list
	\return The reference to the node
	\note It sets some flags like "visited", "unknown", "ignored" etc.
*/
EntnodeData* Entries_SetVisited(const char* path, CSortList<ENTNODE>& entries, const char* name,
	const struct stat& finfo, bool isFolder, const ignored_list_type* ignlist /*= NULL*/)
{
	int index;
	bool isCvs = false;

	EntnodePath* thePath = new EntnodePath(path);

	if( isFolder )
	{
		EntnodeFolder* data = new EntnodeFolder(name, thePath);
		ENTNODE node(data);
		
		data->UnRef();
		isCvs = entries.InsideAndInsert(node, &index);
	}
	else
	{
		EntnodeFile* data = new EntnodeFile(name, thePath);
		ENTNODE node(data);
		
		data->UnRef();
		isCvs = entries.InsideAndInsert(node, &index);
	}

	thePath->UnRef();

	const ENTNODE& theNode = entries.Get(index);
	EntnodeData* data = theNode.Data();
	
	data->SetVisited(true);
	
	if( !isCvs )
	{
		data->SetUnknown(true);

		if( ignlist != NULL && MatchIgnoredList(name, *ignlist) )
			data->SetIgnored(true);

		// The folder may have some cvs informations in it, despite the fact
		// that it is not referenced by the parent directory, so try to figure it out

		if( !data->IsIgnored() )
		{
			string cvsFile(path);
			NormalizeFolderPath(cvsFile);
			cvsFile += name;
			NormalizeFolderPath(cvsFile);
			cvsFile += "CVS";

			struct stat sb;
			if( stat(cvsFile.c_str(), &sb) != -1 && S_ISDIR(sb.st_mode) )
			{
				data->SetUnknown(false);
			}
		}
	}

	if( isFolder )
	{
		if( data->IsIgnored() )
		{
			data->SetState(STR_STATE_IGNORED);
		}
		else if( data->IsUnknown() )
		{
			data->SetState(STR_STATE_UNKNOWN);
		}
		else
		{
			data->SetState("");
		}
	}
	else
	{
		const string& ts = (*data)[EntnodeFile::kTS];
		if( ts.empty() )
		{
			data->SetUnmodified(true);
			data->SetResolved(false);
		}
		else
		{
			const string& vn = (*data)[EntnodeFile::kVN];

			data->SetAdded(strcmp(vn.c_str(), "0") == 0);
			data->SetUnmodified(!data->IsAdded() && VerifyUnmodified(finfo, ts.c_str()));
			
			if( !IsNullOrEmpty(data->GetConflict()) )
			{
				data->SetResolved(!VerifyUnmodified(finfo, data->GetConflict()));
			}
		}

#if defined(WIN32)
#	if ENABLE_WINCVSDEBUG
		if( GetWinCvsDebugMaskBit(wcvsdbg_trace_modified_checks_bit) )
		{
			char scratch[128];

			/* copied from VerifyUnmodified... */
			char* cp;
			struct tm* tm_p;
			struct tm local_tm;
			tm_p = gmtime(&finfo.st_mtime);
			if( tm_p )
			{
				memcpy(&local_tm, tm_p, sizeof(local_tm));
				cp = asctime(&local_tm);	/* copy in the modify time */
			}
			else
			{
				cp = "NULL";
			}

			_snprintf(scratch, 128, 
				"Checking timestamps for %s: Entries: %s, file: %s --> %s\n",
                name, ts, cp, 
                data->IsUnmodified() ? "unmodified" : "modified");
			
			cvs_err(scratch);
			OutputDebugString(scratch);
        }
#	endif	// ENABLE_WINCVSDEBUG
#endif

		data->SetLocked((finfo.st_mode & S_IWRITE) == 0);
		
		EntnodeFile* fileNode = dynamic_cast<EntnodeFile*>(data);
		if( fileNode )
		{
			fileNode->SetModTime(finfo.st_mtime);
		}

		if( data->IsRemoved() )
		{
			data->SetState(STR_STATE_REMOVED);
		}
		else if( data->IsAdded() )
		{
			data->SetState(STR_STATE_ADDED);
		}
		else if( data->IsIgnored() )
		{
			data->SetState(STR_STATE_IGNORED);
		}
		else if( data->IsUnknown() )
		{
			data->SetState(STR_STATE_UNKNOWN);
		}
		else if( !IsNullOrEmpty(data->GetConflict()) )
		{
			data->SetState(data->IsResolved() ? STR_STATE_RESOLVED : STR_STATE_CONFLICT);
		}
		else if( !data->IsUnmodified() )
		{
			data->SetState(STR_STATE_MODIFIED);
		}

		SetEncoding(fileNode);
	}

	return data;
}

/*!
	Mark the missing entries
	\param entries Entries list
	\return true on success, false otherwise
	\note Called after the traversal marks the entries that didn't have "Entries_SetVisited" call associated
*/
void Entries_SetMissing(CSortList<ENTNODE>& entries)
{
	int numEntries = entries.NumOfElements();
	for(int i = 0; i < numEntries; i++)
	{
		const ENTNODE& theNode = entries.Get(i);
		EntnodeData* data = theNode.Data();
		
		if( data->IsVisited() )
			continue;

		data->SetMissing(true);
		data->SetState(data->IsRemoved() ? STR_STATE_REMOVED : STR_STATE_MISSING);

		SetEncoding(dynamic_cast<EntnodeFile*>(data));
	}
}

/*!
	Read the tag file
	\param tag Return tag
	\param fullpath Path
	\return false if no CVS/Tag, true otherwise
*/
bool Tag_Open(std::string& tag, const char* fullpath)
{
	tag.erase();

	string tagFile(fullpath);
	NormalizeFolderPath(tagFile);
	tagFile += "Tag";

	FILE* fpin = fopen(tagFile.c_str(), "r");
	if( fpin == NULL )
		return false;

	char* line = NULL;
	size_t line_chars_allocated = 0;
	int line_length;
	if( (line_length = getline(&line, &line_chars_allocated, fpin)) > 0 )
	{
		char* newLinePos = strchr(line, '\n');
		if( newLinePos != NULL )
			*newLinePos = '\0';
		
		tag = line + 1;
	}

	if( line != NULL )
		free(line);

	if( line_length < 0 && !feof(fpin) )
	{
		cvs_err("Cannot open Tag file in '%s' (error %d)\n", fullpath, errno);
		return false;
	}

	fclose(fpin);

	return !tag.empty();
}
