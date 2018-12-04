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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * ImportFilter.h : filter the imported files before importing
 */

#if !defined(AFX_IMPORTFILTER_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
#define AFX_IMPORTFILTER_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <vector>
#include "FileTraversal.h"
#include "ProgressDlg.h"
#include "CvsIgnore.h"

class ReportWarning;
class ReportConflict;
class ImportReport;

kTraversal list_all_types_recur(const char* path, ReportWarning*& warnings, ReportConflict*& conflicts);
void free_list_types(ReportWarning*& warnings, ReportConflict*& conflicts);
void copy_list_types(const ReportWarning* srcWarnings, const ReportConflict* srcConflicts, ReportWarning*& warnings, ReportConflict*& conflicts);

#if qCvsDebug
	bool list_all_types(const char* path);
#endif /* qCvsDebug */

/// Class to represent file type and help handle possible import conflicts
class ReportConflict
{
public:
	ReportConflict(int mac_type, int mac_creator);
	virtual ~ReportConflict();

	ReportConflict* next;	/*!< Next element */

	/// Status enum
	enum
	{
		noConflict =				0x0000,	/*!< No conflict */
		hasConflict =				0x0001,	/*!< There is a conflict */
		hasExtension =				0x0002,	/*!< Has an extension */
		isBinary =					0x0004,	/*!< Binary */
		forceBinary =				0x0008,	/*!< Force binary */
		forceText =					0x0010,	/*!< Force text */
		hasIgnore =					0x0020,	/*!< Ignore */
		hasTypeCreatorConflict =	0x0040, /*!< Type creator conflict, mac only */
		forceNoKeywords =			0x0080,	/*!< For no keyword expansion */
		isUnicode =					0x0100,	/*!< Unicode */
		forceUnicode =				0x0200,	/*!< Force unicode */
		userFlags =	forceText | forceBinary | forceUnicode | hasIgnore | forceNoKeywords	/*!< Mask of all user flags */
	};

	void AddPattern(const char* newpat, kFileType fileType, bool isExtension, const char* sample);
	void SetConflict(const char* sample);
	void SetTypeCreatorConflict(const char* sample); /* mac only */
	bool Match(const char* pat);

	bool HasConflict(void) const;
	bool HasTypeCreatorConflict(void) const;
	bool IsBinary(void) const;
	bool IsUnicode(void) const;
	bool IsExtension(void) const;
	const char* GetPattern(void) const;
	int GetMacType(void) const;
	int GetMacCreator(void) const;

	void ForceBinary(void);
	void ForceText(void);
	void ForceUnicode(void);
	void Ignore(void);
	void ForceNoKeywords(void);
	bool HasForceBinary(void) const;
	bool HasForceText(void) const;
	bool HasForceUnicode(void) const;
	bool HasIgnore(void) const;
	bool HasForceNoKeywords(void) const;

	void DisableUserSettings(void);

	std::string GetStateDesc() const;
	std::string GetDesc() const;
	std::string GetKindDesc() const;

	void PrintOut(std::string& out) const;

#if qCvsDebug
	void PrintOut(void) const;
#endif /* qCvsDebug */

protected:
	int status;				/*!< Status */
	std::string pattern;	/*!< An extension or a whole name (hasExtension) */
	std::string sample1;	/*!< Sample for this entry */
	std::string sample2;	/*!< Another sample (if hasConflict) */
	int ftype;				/*!< File type, mac only */
	int fcreator;			/*!< File creator, mac only */
};

/// Class to represent a warning (wrong line feed, escape characters etc.) and a set of files which illustrates this warning
class ReportWarning
{
public:
	ReportWarning(kTextBinTYPE akind);
	virtual ~ReportWarning();

	ReportWarning* next;				/*!< Next element */
	static const size_t maxWarnings;	/*!< Maximum number of wanings */

	void AddWarning(const char* sample);
	kTextBinTYPE Kind(void) const;

	bool IsError() const;
	std::string GetStateDesc() const;
	std::string GetDesc() const;

	void PrintOut(std::string& out) const;

#if qCvsDebug
	void PrintOut(void) const;
#endif /* qCvsDebug */

protected:
	kTextBinTYPE kind;

	std::vector<std::string> samples;	/*!< Samples container */
	bool tooManyWarnings;				/*!< Flag to indicate too many warnings */
};

/// Class to traverse and detect conflicts or warnings for import
class ImportReport : public TraversalReport
{
public:
	// Construction
	ImportReport(const char* path, ReportWarning*& w, ReportConflict*& c);
	virtual ~ImportReport();

private:
	// Data members
	ReportWarning*& warnings;		/*!< Warnings list */
	ReportConflict*& conflicts;		/*!< Conflicts list */
	ignored_list_type m_ignlist;	/*!< Ignore list */

	CProgress m_progress;			/*!< Progress dialog to allow abort filtering */

public:
	// Interface

	/// Get the ignore list
	ignored_list_type& GetIgnoreList() { return m_ignlist; }

	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const UFSSpec* macspec);
	virtual kTraversal ExitDirectory(const char* fullpath);
	virtual kTraversal OnError(const char* err, int errcode);
	virtual kTraversal OnIdle(const char* fullpath);

	virtual kTraversal OnDirectory(const char* fullpath, const char* fullname, const char* name,
		const struct stat& dir, const UFSSpec* macspec);

	virtual kTraversal OnFile(const char* fullpath, const char* fullname, const char* name,
		const struct stat& dir, const UFSSpec* macspec);
	
	virtual kTraversal OnAlias(const char*fullpath, const char* fullname, const char* name,
		const struct stat& dir, const UFSSpec* macspec);
};

#endif // !defined(AFX_IMPORTFILTER_H__98CCBD22_845B_11D1_8949_444553540000__INCLUDED_)
