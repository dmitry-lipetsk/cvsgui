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
 * UpdateToTempParse.h : parse the CVS update output
 */

#if !defined(AFX_UPDATETOTEMPPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_UPDATETOTEMPPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#	pragma once
#endif // _MSC_VER >= 1000

#include "MultiFiles.h"
#include "CvsArgs.h"
#include "AppGlue.h"

/// Class to implement update console that will look for lines
/// beginning with "Checking out " on stderr and redirect
/// stdout to a corresponding temp file
class CCvsUpdateToTempConsole : public CCvsFilterConsole
{
	typedef CCvsFilterConsole PARENT;

public:
	/// Multiline entry type enum
	typedef enum {
		kUpdateFilter = 0,	/*!< Update */
		kAnnotateFilter,	/*!< Annotate */
	}kFilterType;

	// Construction
	CCvsUpdateToTempConsole(kFilterType filterType, const MultiFilesEntry* mfe,	const std::string& rev,
		std::vector<std::string>& tempFiles);
	virtual ~CCvsUpdateToTempConsole();

	// Interface
	void Abort();

private:
	// Methods
	virtual long cvs_out(const char* txt, long len);
	virtual long cvs_err(const char* txt, long len);
	
	virtual void OnFilter(const std::string& txt);
	
	void CloseFile();
	FILE* CreateTempFile(const std::string& fileName, const std::string& currRev, std::string& tmpFile);

	static const char* GetFilterText(kFilterType filterType);

	// Members
	kFilterType m_filterType;		/*!< Filter type */
	FILE* m_currentFile;			/*!< Current file */
	std::string m_currentFileName;	/*!< Current file name */
	const MultiFilesEntry* m_mfe;	/*!< MultiFiles for this diff command */
	std::string m_rev;
	std::string m_revNorm;
	std::vector<std::string>& m_tempFiles;
	bool m_bAbort;					/*!< Abort request, avoid creating empty files */
};

#endif // !defined(AFX_UPDATETOTEMPPARSE_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
