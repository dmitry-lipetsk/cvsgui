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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- January 1997
 */

#include "stdafx.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <new>
#include <string>
#include <sstream>
#include <algorithm>
#include <stack>

#if defined (WIN32)
#	include <io.h>
#endif

#if defined (TARGET_OS_MAC)
#	include "MacMisc.h"
#endif

#include "cvsgui_i18n.h"

#include "ustr.h"
#include "uconsole.h"
#include "FileTraversal.h"
#include "AppConsole.h"

#if qMacAPP
#	include <Errors.h>
#endif

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#if TARGET_RT_MAC_MACHO || qUnix
#	ifndef HAVE_STRICMP
#		define stricmp strcasecmp
#	endif
#	ifndef HAVE_STRNICMP
#		define strnicmp strncasecmp
#	endif
#endif

using namespace std;

/// Functor class to eliminate characters which can't be used in a filename
class ChrForbiddenInFilename_Test : public unary_function<const char, bool>
{
public:
	result_type operator()(argument_type c) const
	{
		return c == '*' ||
			c == '?' ||
			c == ':' ||
			c == '/' ||
			c == '\\' ||
			c == '"' ||
			c == '$';
	}
};

/// Functor class to find non-white space characters
class ChrNotWhiteSpace_Test : public unary_function<const char, bool>
{
public:
	result_type operator()(argument_type c) const
	{
		return !isspace(c);
	}
};

/*!
	Normalize the log message for the server
	\param msg Message to be normalized
	\return Returns reference to normalized log message
	\todo mac : ISO conversion
*/
std::string& NormalizeLogMsg(std::string& msg)
{
#ifdef WIN32
	Replace(msg, "\r", "");
#endif /* WIN32 */
#ifdef TARGET_OS_MAC
	Replace(msg, '\r', '\n');
#endif /* TARGET_OS_MAC */

	return msg;
}

/*!
	Format XML string, replacing any special characters with it's reference
	\param xmlItem String to format
	\return Formatted XML string
	\note Reference: http://support.microsoft.com/kb/316063
*/
std::string FormatXmlString(const char* xmlItem)
{
	std::string res(xmlItem ? xmlItem : "");

	if( !res.empty() )
	{
		Replace(res, "&", "&amp;");		// Ampersand
		Replace(res, "<", "&lt;");		// Left angle bracket
		Replace(res, ">", "&gt;");		// Right angle bracket
		Replace(res, "\"", "&quot;");	// Straight quotation mark
		Replace(res, "'", "&apos;");	// Apostrophe
	}

	return res;
}

/*!
	Find a unique name for a temporary file
	\param file Return temporary file name
	\param prefix Prefix of the filename
	\param extension Extension of the filename
	\param create Set to true to create the file
	\return true on success, false otherwise
*/
bool MakeTmpFile(std::string& file, const char* prefix, const char* extension /*= NULL*/, bool create /*= false*/)
{
	static int count = 0;
	
	string tmpPath;
	string ext;
	string prf;
	
	if( extension && *extension != '\0' )
	{
		if( extension[0] != '.' )
			ext = ".";
		
		ext += extension;
	}
	
	if( prefix && *prefix != '\0' )
	{
		prf = prefix;
	}
	else
	{
		prf = "cvsgui";
	}
	
#ifdef WIN32
	const char* ptr = getenv(_T("TMP"));
	if( ptr && _access(ptr, 0) != -1 )
	{
		tmpPath = ptr;
	}
	else
	{
		tmpPath = ".";
	}
#endif /* !WIN32 */
#ifdef TARGET_OS_MAC
	UFSSpec theFolder;
	OSErr err;
	
	if( (err = MacGetTempFolder(theFolder, tmpPath)) != noErr )
	{
		cvs_err(_i18n("Unable to locate the preferences folder (error %d) !\n"), err);
		return 0L;
	}
#endif /* !TARGET_OS_MAC */
#if qUnix
	tmpPath = "/tmp";
#endif
	
	struct stat sb;
	if( stat(tmpPath.c_str(), &sb) == -1 || !S_ISDIR(sb.st_mode) )
	{
		cvs_err(_i18n("Unable to access '%s' (error %d) !\n"), tmpPath.c_str(), errno);
		return false;
	}
	
	NormalizeFolderPath(tmpPath);
	
	int maxtry = 10000;
	while( true )
	{
		stringstream name;
		name << prf.c_str() << count++ << ext.c_str();
		
		file = tmpPath;
		string filename = name.str();
		file += NormalizeFileName(filename);
		
		if( stat(file.c_str(), &sb) == -1 && errno == ENOENT )
		{
			if( create )
			{
				if( FILE* stream = fopen(file.c_str(), "w") )
				{
					fclose(stream);
				}
			}
			
			return true;
		}
		
		if( maxtry-- <= 0 )
		{
			cvs_err(_i18n("Time out to access '%s' !\n"), tmpPath.c_str());
			break;
		}
	}
	
	return false;
}

/*!
	Trim white-space characters from the left side of the string
	\param str string
	\return Reference to the trimmed string
*/
std::string& TrimLeft(std::string& str)
{
	str.erase(str.begin(), find_if(str.begin(), str.end(), ChrNotWhiteSpace_Test()));

	return str;
}

/*!
	Trim given character from the left side of the string
	\param str string
	\param chr Character to trim
	\return Reference to the trimmed string
*/
std::string& TrimLeft(std::string& str, const char chr)
{
	const string::size_type pos = str.find_first_not_of(chr);
	
	if( pos != string::npos )
	{
		str.erase(0, pos);
	}

	return str;
}

/*!
	Trim any of given characters from the left side of the string
	\param str string
	\param strTrim Characters to trim
	\return Reference to the trimmed string
*/
std::string& TrimLeft(std::string& str, const char* strTrim)
{
	const string::size_type pos = str.find_first_not_of(strTrim);
	
	if( pos != string::npos )
	{
		str.erase(0, pos);
	}

	return str;
}

/*!
	Trim white-space characters from the right side of the string
	\param str string
	\return Reference to the trimmed string
*/
std::string& TrimRight(std::string& str)
{
	str.erase(find_if(str.rbegin(), str.rend(), ChrNotWhiteSpace_Test()).base(), str.end());
	
	return str;
}

/*!
	Trim given character from the right side of the string
	\param str string
	\param chr Character to trim
	\return Reference to the trimmed string
*/
std::string& TrimRight(std::string& str, const char chr)
{
	const string::size_type pos = str.find_last_not_of(chr);
	
	if( pos != string::npos )
	{
		str.erase(pos + 1);
	}
	
	return str;
}

/*!
	Trim any of given characters from the right side of the string
	\param str string
	\param strTrim Characters to trim
	\return Reference to the trimmed string
*/
std::string& TrimRight(std::string& str, const char* strTrim)
{
	const string::size_type pos = str.find_last_not_of(strTrim);
	
	if( pos != string::npos )
	{
		str.erase(pos + 1);
	}
	
	return str;
}

/*!
	Trim white-space characters from both sides of the string
	\param str string
	\return Reference to the trimmed string
*/
std::string& Trim(std::string& str)
{
	return TrimLeft(TrimRight(str));
}

/*!
	Trim given character from both sides of the string
	\param str string
	\param chr Character to trim
	\return Reference to the trimmed string
*/
std::string& Trim(std::string& str, const char chr)
{
	return TrimLeft(TrimRight(str, chr), chr);
}

/*!
	Trim any of given characters from both sides of the string
	\param str string
	\param strTrim Characters to trim
	\return Reference to the trimmed string
*/
std::string& Trim(std::string& str, const char* strTrim)
{
	return TrimLeft(TrimRight(str, strTrim), strTrim);
}

/*!
	Parse a command like 'user=whohost=cvs'
	\param cmd Command
	\param key Key
	\param value Return value
	\return	true on success, false otherwise
*/
bool GetEnvValue(const char* cmd, const char* key, std::string& value)
{
	if( !cmd || !key || 
		*cmd == '\0' || *key == '\0' )
	{
		// Bad pointer or empty strings
		return false;
	}

	string strCmd = cmd;

	const string::size_type posKey = strCmd.find(key);
	if( posKey == string::npos )
	{
		// No assignment found
		return false;
	}

	const string::size_type posAssign = strCmd.find('=', posKey);
	if( posAssign == string::npos )
	{
		// No assignment found
		return false;
	}

	const string::size_type posAmp = strCmd.find('&', posAssign);
	if( posAmp != string::npos )
	{
		value = strCmd.substr(posAssign + 1, posAmp - 1 - posAssign);
	}
	else
	{
		value = strCmd.substr(posAssign + 1);
	}

	return true;
}

/*!
	Normalize given file name
	\param fileName Return normalized file name
	\return Normalized file name reference
*/
std::string& NormalizeFileName(std::string& fileName)
{
	std::replace_if(fileName.begin(), fileName.end(), ChrForbiddenInFilename_Test(), '_');

	return fileName;
}

/*!
	Normalize given folder path
	\param folder Return normalized folder path
	\param delimLast true to assure that path ends with delimiter, false to trim delimiter
	\return Normalized folder path
*/
std::string& NormalizeFolderPath(std::string& folder, bool delimLast /*= true*/)
{
	if( !folder.empty() )
	{
		if( delimLast  )
		{
			if( folder.at(folder.length() - 1) != kPathDelimiter )
			{
				folder += kPathDelimiter;
			}
		}
		else
		{
			TrimRight(folder, kPathDelimiter);
#ifdef WIN32
			// Special treatment for drive root folder on windows
			if( folder.length() == 2 && ':' == folder[1] )
			{
				folder += kPathDelimiter;
			}
#endif
		}
	}

	return folder;
}

/*!
	Split the path into the up-directory and the folder
	\param dir Path to be split
	\param uppath Return up-directory
	\param folder Return folder or file
	\param dontSkipDelimiter Set to true to prevent skipping the delimiter if the path to be split ends with one
	\return true on success, false otherwise
	\note Passing NULL is considered an error, passing empty string returns true
*/
bool SplitPath(const char* dir, std::string& uppath, std::string& folder, bool dontSkipDelimiter /*= false*/)
{
	if( !dir )
	{
		// NULL pointer
		return false;
	}

	if( *dir == '\0' )
	{
		// Empty string
		if( !uppath.empty() )
			uppath.erase();

		if( !folder.empty() )
			folder.erase();

		return true;
	}
	
	uppath = dir;

	string::size_type pos = uppath.find_last_of(kPathDelimiter);
	if( string::npos == pos )
	{
		// No path delimiter
		folder = uppath;
		uppath.erase();

		return true;
	}

	const bool delIsLast = uppath.length() - 1 == pos;
	if( delIsLast && !dontSkipDelimiter )
	{
		// Skip delimiter
		uppath.erase(pos);
		pos = uppath.find_last_of(kPathDelimiter);
	}

	if( pos != string::npos )
	{
		folder = uppath.substr(pos + 1, delIsLast ? uppath.length() - 1 : string::npos);
		uppath.erase(pos + 1);
	}
	else
	{
		folder.erase();
	}

#ifdef WIN32
	// Special treatment for drive root folder on windows
	if( folder.empty() && 
		uppath.length() == 2 && ':' == uppath[1] )
	{
		uppath += kPathDelimiter;
	}
#endif

	return true;
}

/*!
	Split filename into base and extension
	\param file File name to split
	\param base Return the base of the filename
	\param ext Return the file's extension
	\return true on success, false otherwise
	\note Passing NULL is considered an error, passing empty string returns true
*/
bool SplitFilename(const char* file, std::string& base, std::string& ext)
{
	if( !file )
	{
		// NULL pointer
		return false;
	}

	if( *file == '\0' )
	{
		// Empty string
		if( !base.empty() )
			base.erase();

		if( !ext.empty() )
			ext.erase();

		return true;
	}
	
	base = file;
	
	const string::size_type pos = base.find_last_of(kExtensionDelimiter);
	if( string::npos == pos )
	{
		// No extension delimiter
		if( !ext.empty() )
			ext.erase();
		
		return true;
	}

	const bool delIsLast = base.length() - 1 == pos;
	if( delIsLast )
	{
		if( !ext.empty() )
			ext.erase();
		
		base.erase(pos);

		return true;
	}

	ext = base.substr(pos + 1);
	base.erase(pos);

	return true;
}

/*!
	Replace all instance of string with another string
	\param strRes String to replace strings in
	\param strFind String to be replaced
	\param strReplace Replacement string
	\return Returns reference to strRes
*/
std::string& Replace(std::string& strRes, const char* strFind, const char* strReplace)
{
	if( !strRes.empty() && strFind && strReplace )
	{
		const int findLen = strlen(strFind);
		const int replaceLen = strlen(strReplace);
		
		std::string::size_type pos = 0;
		while( (pos = strRes.find(strFind, pos)) != std::string::npos )
		{
			strRes.replace(pos, findLen, strReplace);
			pos += replaceLen;
		}
	}
	
	return strRes;
}

/*!
	Parse command line into a vector of strings
	\param cmdLine Command line to be parsed
	\param args Return parsed command line as vector of strings
	\return true on success, false otherwise
*/
bool ParseCommandLine(const char* cmdLine, std::vector<std::string>& args)
{
	if( !cmdLine )
	{
		// Bad pointer
		return false;
	}

	/// Command line parser state
	typedef enum {
		Argument,
		Quote,
		Escape,
	}tCmdLineState;

	stack<int> state;
	const char* argStart = NULL;
	bool done = false;

	for(const char* pos = cmdLine; !done; pos++)
	{
		if( '\0' == *pos )
		{
			done = true;
		}

		if( state.empty() )
		{
			if( isspace(*pos) || done )
			{
				continue;
			}

			argStart = pos;
			state.push(Argument);
		}

		if( Argument == state.top() )
		{
			if( isspace(*pos) || done )
			{
				string strArg(argStart, pos - argStart);
				args.push_back(strArg);

				state.pop();
				argStart = NULL;
			}
			else
			{
				switch( *pos )
				{
				case kCmdLineQuote:
					state.push(Quote);
					break;
				case kCmdLineEscape:
					state.push(Escape);
					break;
				default:
					// Nothing to do
					break;
				}
			}
		}
		else if( Quote == state.top() )
		{
			switch( *pos )
			{
			case kCmdLineQuote:
				state.pop();
				break;
			case kCmdLineEscape:
				state.push(Escape);
				break;
			default:
				// Nothing to do
				break;
			}
		}
		else if( Escape == state.top() )
		{
			if( !done )
			{
				state.pop();
			}
		}
	}

	// If state unmatched then append as the last argument anyway
	if( !state.empty() )
	{
		if( argStart )
		{
			args.push_back(argStart);
		}
	}

	// Process quotes and escape characters
	for(vector<string>::iterator it = args.begin(); it != args.end(); it++ )
	{
		// Erase quotes from quoted argument
		if( !it->empty() )
		{
			if( kCmdLineQuote == it->at(0) && 
				it->length() > 1 && 
				kCmdLineQuote == it->at(it->length() - 1) )
			{
				it->erase(it->begin());
				it->erase(it->length() - 1);
			}
		}

		// Replace escaped characters
		if( !it->empty() )
		{
			static const char escapedQuote[] = {kCmdLineEscape, kCmdLineQuote, '\0'};
			static const char replaceQuote[] = {kCmdLineQuote, '\0'};
			
			static const char escapedEscape[] = {kCmdLineEscape, kCmdLineEscape, '\0'};
			static const char replaceEscape[] = {kCmdLineEscape, '\0'};
			
			Replace(*it, escapedQuote, replaceQuote);
			Replace(*it, escapedEscape, replaceEscape);
		}
	}

	return state.empty();
}

/*!
	Removes folder separators by replacing them with dots so that temporary file 
	could be created without creating matching subfolders in the temporary file folder
	\param path File with subfolder
	\return Flattened path reference
	\note Windows client may provide file names pointing into subfolders
*/
std::string& FlatenSubFolders(std::string& path)
{
#if defined(WIN32) || defined(qUnix)
	std::replace(path.begin(), path.end(), kPathDelimiter, '.');
#elif TARGET_OS_MAC
	char *tmp;
	if((tmp = strrchr(path, kPathDelimiter)) != NULL)
	{
		CStr newStr(++tmp);
		path = newStr;
	}
#else
	// no-op on other platforms
#endif
	
	return path;
}

/*!
	Make a "safe" copy of the string
	\param buffer Destination
	\param value Source
	\param bufferSize Destination buffer size
*/
void StrSafeCopy(char* buffer, const char* value, const int bufferSize)
{
	if( value && *value )
	{
		strncpy(buffer, value, bufferSize);
	}
	else
	{
		*buffer = 0;
	}
}
	
/*!
	Compare strings without regard to case but assuring the order if only case differs
	\param s1 First string
	\param s2 Second string
	\return -1, 0 and 1 as appropriate
*/
int StrOrdICmp(const char* s1, const char* s2)
{
	if( int rc = stricmp(s1, s2) )
		return rc;
	
	return strcmp(s1, s2);
}

/*!
	Compare paths ignoring trailing path delimiter
	\param s1 First path
	\param s2 Second path
	\return -1, 0 and 1 as appropriate
*/
int StrPathCmp(const char* s1, const char* s2)
{
	string str1(s1);
	string str2(s2);

	TrimRight(str1, kPathDelimiter);
	TrimRight(str2, kPathDelimiter);

#ifdef WIN32
	return stricmp(str1.c_str(), str2.c_str());
#else
	return strcmp(str1.c_str(), str2.c_str());
#endif
}

//////////////////////////////////////////////////////////////////////////
// CPersistentString

CPersistentString::~CPersistentString()
{
}

/*!
	Get the size of string
	\return The size of string
	\note One byte added to account for the terminating NULL character
*/
unsigned int CPersistentString::SizeOf(void) const
{
	return length() + 1;
}

/*!
	Get the string data
	\return The string data
*/
const void* CPersistentString::GetData(void) const
{
	return c_str();
}

/*!
	Set the string data
	\param ptr String data
	\param size String data size
*/
void CPersistentString::SetData(const void* ptr, unsigned int size)
{
	if( strlen((const char*)ptr) == (size - 1) )
		*(std::string*)this = (const char*)ptr;
}
