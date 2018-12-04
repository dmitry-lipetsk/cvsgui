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
 * Author : Kirill Müller <mail@kirill-mueller.de> --- March 2006
 */

/*
 * UpdateToTempParse.cpp : parse the CVS update output
 */

#include "stdafx.h"

#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef TARGET_OS_MAC
#	include <unistd.h>
#endif /* TARGET_OS_MAC */

#include "UpdateToTempParse.h"
#include "AppConsole.h"
#include "getline.h"
#include <algorithm>

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

//////////////////////////////////////////////////////////////////////////
// 	CCvsUpdateToTempConsole

CCvsUpdateToTempConsole::CCvsUpdateToTempConsole(kFilterType filterType, const MultiFilesEntry* mfe, const std::string& rev,
												 std::vector<std::string>& tempFiles) 
	: CCvsFilterConsole(CCvsUpdateToTempConsole::GetFilterText(filterType), CCvsFilterConsole::FILTER_ERR),
	m_tempFiles(tempFiles)
{
	m_filterType = filterType;
	m_currentFile = NULL;
	m_mfe = mfe;
	m_rev = rev;
	m_revNorm = rev;

#ifdef WIN32
	NormalizeFileName(m_revNorm);
#endif

	m_tempFiles.resize(mfe->NumFiles());

	m_bAbort = false;
}

CCvsUpdateToTempConsole::~CCvsUpdateToTempConsole()
{
	CloseFile();

	// Check if files were missing, create empty files
	bool bWriteNewline = false;

	if( !m_bAbort) 
	{
		for(int i = 0; ; i++)
		{
			string path, filename, currRev;
			if( !m_mfe->get(i, path, filename, currRev) )
				break;
			
			if( m_tempFiles[i].length() > 0 )
				continue;
			
			string tmpFile;
			FILE* file = CreateTempFile(filename, "0", tmpFile);
			
			if( !file )
				continue;
			
			fclose(file);
			
			m_tempFiles[i] = tmpFile;
			
			static const char* message = _i18n("Created empty file ");
			static const int messageLen = strlen(message);

			cvs_outstr(message, messageLen);
			cvs_outstr(tmpFile.c_str(), tmpFile.length());
			cvs_outstr("\n", 1);
			
			bWriteNewline = true;
		}
	}

	if( bWriteNewline )
		cvs_outstr("\n", 1);
}

long CCvsUpdateToTempConsole::cvs_out(const char* txt, long len)
{
	if( !m_currentFile )
		return 0;

	if( !m_currentFileName.empty() )
	{
		static const char* message = _i18n("Written to ");
		static const int messageLen = strlen(message);

		cvs_outstr(message, messageLen);
		cvs_outstr(m_currentFileName.c_str(), m_currentFileName.length());
		cvs_outstr("\n", 1);

		m_currentFileName.erase();
	}

	return fwrite(txt, 1, len, m_currentFile);
}

long CCvsUpdateToTempConsole::cvs_err(const char* txt, long len)
{
	cvs_errstr(txt, len);
	return PARENT::cvs_err(txt, len);
}

void CCvsUpdateToTempConsole::OnFilter(const std::string& txt)
{
	CloseFile();

	string txtBS = txt;

	if( kPathDelimiter != '/' )
	{
		replace(txtBS.begin(), txtBS.end(), '/', kPathDelimiter);
	}

	string path, filename, currRev;
	int i;
	for(i = 0; ; i++)
	{
		if( !m_mfe->get(i, path, filename, currRev) )
			break;

		if( filename == txtBS.c_str() )
			break;

		filename = "";
	}

	if( filename.length() == 0 )
	{
		::cvs_err(_i18n("Didn't find file '%s' among requested files\n"), txtBS.c_str());
		return;
	}

	string tmpFile;
	m_currentFile = CreateTempFile(filename, currRev, tmpFile);

	if( !m_currentFile )
		return;

	m_tempFiles[i] = tmpFile.c_str();
	registerTempFile(tmpFile.c_str());
	m_currentFileName = tmpFile;
}

void CCvsUpdateToTempConsole::Abort()
{
	m_bAbort = true;
}

void CCvsUpdateToTempConsole::CloseFile()
{
	if( m_currentFile ) 
	{
		fclose(m_currentFile);
		m_currentFile = NULL;
	}
}

/*!
	Create temporary file
	\param fileName Filename
	\param currRev Current revision
	\param tmpFile Return temporary file name 
	\return The temporary file handle on success, NULL on error
*/
FILE* CCvsUpdateToTempConsole::CreateTempFile(const std::string& fileName, const std::string& currRev, std::string& tmpFile)
{
	// note that filename can contain subfolders
	string ext, base;
	SplitFilename(fileName.c_str(), base, ext);

	string prefix(base);

	switch( m_filterType )
	{
	case kUpdateFilter:
		prefix += '_';
		break;
	case kAnnotateFilter:
		prefix += "_ann_";
		break;
	default:
		break;
	}
	
	if( !m_rev.empty() )
		prefix += m_revNorm;
	else
		prefix += currRev;
	
	prefix += '_';

	// convert subfolder names to regular name
	FlatenSubFolders(prefix);

	return createCVSTempFile(tmpFile, prefix.c_str(), ext.c_str());
}

/*!
	Get the filter text
	\param filterType Filter type to get the text for
	\return Filter text
*/
const char* CCvsUpdateToTempConsole::GetFilterText(kFilterType filterType)
{
	const char* res = "";

	switch( filterType )
	{
	case kUpdateFilter:
		res = "Checking out ";
		break;
	case kAnnotateFilter:
		res = "Annotations for ";
		break;
	default:
		// Fall thru
		break;
	}

	return res;
}