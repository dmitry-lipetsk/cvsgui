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
 * ImportFilter.cpp : filter the imported files before importing
 */

#include "stdafx.h"

#ifdef WIN32
#	include "resource.h"
#endif /* WIN32 */

#ifdef TARGET_OS_MAC
#	include "GUSIInternal.h"
#	include "GUSIFileSpec.h"
#	include "MacMisc.h"
#endif /* TARGET_OS_MAC */

#include "ImportFilter.h"
#include "AppConsole.h"

//
// MN - Apply $HOME/.cvsignore to import filter
//
// list_add_type function uses ignore filter to decided whether
// a file should be ignored in importing (user can change this
// in import dialogbox, but this is a default). If user imports
// a whole subdirectory instead of a file then this will
// eliminate ignored files.
//
#include "CvsIgnore.h"	

#if qCvsDebug
#	include "CvsAlert.h"
#endif /* qCvsDebug */

using namespace std;

#ifdef TARGET_OS_MAC
	static const char *lf = "\r";
#elif defined(WIN32)
	static const char *lf = "\r\n";
#else
	static const char *lf = "\n";
#endif

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

/*!
	Add new warning to the list	or add the sample to already existing warning
	\param warnings Warning list
	\param warn Warning type
	\param sample Sample
*/
static void list_add_warning(ReportWarning*& warnings, kTextBinTYPE warn, const char* sample)
{
	// Find if it exists already a same kind of warning
	ReportWarning* tmp = warnings;
	while( tmp != 0L )
	{
		if( tmp->Kind() == warn )
			break;

		tmp = tmp->next;
	}

	if( tmp != 0L && tmp->Kind() == warn )
	{
		tmp->AddWarning(sample);
		return;
	}

	// Insert new one
	tmp = new ReportWarning(warn);
	if( tmp == 0L )
	{
		cvs_err("Impossible to allocate %d bytes !\n", sizeof(ReportWarning));
		return;
	}

	tmp->AddWarning(sample);

	if( warnings == 0L )
	{
		warnings = tmp;
	}
	else
	{
		tmp->next = warnings;
		warnings = tmp;
	}
}

/*!
	Add file to a warning/conflict list
	\param impReport Import report
	\param conflicts Conflicts list
	\param fileType File type
	\param sample Sample
	\param fullname Full name
	\param mac_type Mac specific type
	\param mac_creator Mac specific crator
	\note When a file is added to the list, it is also processed through ignore filter. 
	If file should be ignored then the initial state is "IGNORE" in import dialog box.
*/
static void list_add_type(ImportReport* impReport, ReportConflict*& conflicts, kFileType fileType,
						  const char* sample, const char* fullname, 
						  int mac_type, int mac_creator)
{
	// Look for a matching entry
	ReportConflict* entry = conflicts;
	while( entry != 0L )
	{
		if( entry->Match(sample) )
		{
			if( entry->HasConflict() )
				return;
			
			if( entry->IsBinary() ^ (kFileTypeBin == fileType) )
				entry->SetConflict(fullname);
			
			if( entry->IsUnicode() ^ (kFileTypeUnicode == fileType) )
				entry->SetConflict(fullname);
			
			// Mac only
			if( entry->GetMacType() != mac_type || entry->GetMacCreator() != mac_creator )
				entry->SetTypeCreatorConflict(fullname);
			
			// It matches perfectly
			return;
		}
		
		entry = entry->next;
	}

	// Find the extension
	const char* tmp = sample, *ext = 0L;

	while( (tmp = strchr(tmp, '.')) != 0L )
	{
		ext = tmp++;
	}

	// Add a new entry
	ReportConflict* newc = new ReportConflict(mac_type, mac_creator);
	if( newc == 0L )
	{
		cvs_err("Impossible to allocate %d bytes !\n", sizeof(ReportConflict));
		return;
	}

	newc->AddPattern(ext != 0L ? ext : sample, fileType, ext != 0L, fullname);

	if( conflicts == 0L )
	{
		conflicts = newc;
	}
	else
	{
		newc->next = conflicts;
		conflicts = newc;
	}

	// MN - Apply ignore filter. If $HOME/.cvsignore tells that
	// file should be ignored then set object to ignored state.
	if( MatchIgnoredList(sample, impReport->GetIgnoreList()) )
		newc->Ignore();
}

/*!
	Traverse path and add files to the warning and conflict lists
	\param path Path to traverse
	\param warnings Warnings list
	\param conflicts Conflicts list
	\return Traverse command as kTraversal
*/
kTraversal list_all_types_recur(const char* path, ReportWarning*& warnings, ReportConflict*& conflicts)
{
	ImportReport report(path, warnings, conflicts);
	return FileTraverse(path, report);
}

/*!
	Release memory allocated for warnings and conflicts lists
	\param warnings Warnings list
	\param conflicts Conflicts list
*/
void free_list_types(ReportWarning*& warnings, ReportConflict*& conflicts)
{
	ReportWarning* warn = warnings;
	while( warn != 0L )
	{
		ReportWarning* tmpwarn = warn;
		warn = warn->next;
		delete tmpwarn;
	}

	ReportConflict* conf = conflicts;
	while( conf != 0L )
	{
		ReportConflict* tmpconf = conf;
		conf = conf->next;
		delete tmpconf;
	}
	
	warnings = NULL;
	conflicts = NULL;
}

/*!
	Copy warnings and conflicts lists
	\param srcWarnings Source warnings list
	\param srcConflicts Source conflicts list
	\param warnings Warnings list
	\param conflicts Conflicts list
*/
void copy_list_types(const ReportWarning* srcWarnings, const ReportConflict* srcConflicts, ReportWarning*& warnings, ReportConflict*& conflicts)
{
	const ReportWarning* warn = srcWarnings;
	ReportWarning* lastWarn = NULL;
	while( warn != 0L )
	{
		ReportWarning* neww = new ReportWarning(kFileIsOK);
		if( neww == 0L )
		{
			cvs_err("Impossible to allocate %d bytes !\n", sizeof(ReportWarning));
			return;
		}
		
		*neww = *warn;
		neww->next = NULL;
		
		if( warnings == 0L )
		{
			warnings = neww;
		}
		else
		{
			lastWarn->next = neww;
		}

		lastWarn = neww;
		warn = warn->next;
	}
	
	const ReportConflict* conf = srcConflicts;
	ReportConflict* lastConf = NULL;
	while( conf != 0L )
	{
		ReportConflict* newc = new ReportConflict(0, 0);
		if( newc == 0L )
		{
			cvs_err("Impossible to allocate %d bytes !\n", sizeof(ReportConflict));
			return;
		}
		
		*newc = *conf;
		newc->next = NULL;
		
		if( conflicts == 0L )
		{
			conflicts = newc;
		}
		else
		{
			lastConf->next = newc;
		}
		
		lastConf = newc;
		conf = conf->next;
	}
}

#if qCvsDebug
bool list_all_types(const char* path)
{
	ReportWarning* warnings = 0L;
	ReportConflict* conflicts = 0L;
	list_all_types_recur(path, warnings, conflicts);
	
	cvs_out("Reporting warnings :\n");
	ReportWarning* warn = warnings;
	
	while( warn != 0L )
	{
		warn->PrintOut();
		warn = warn->next;
	}

	cvs_out("Reporting types :\n");
	ReportConflict* conf = conflicts;

	while( conf != 0L )
	{
		conf->PrintOut();
		conf = conf->next;
	}

	free_list_types(warnings, conflicts);

	CvsAlert cvsAlert(kCvsAlertQuestionIcon, 
		_i18n("Continue importing?"), NULL, 
		BUTTONTITLE_CONTINUE, BUTTONTITLE_CANCEL);

	return cvsAlert.ShowAlert() == kCvsAlertOKButton;
}
#endif /* qCvsDebug */

//////////////////////////////////////////////////////////////////////////
// ReportConflict

ReportConflict::ReportConflict(int mac_type, int mac_creator)
{
	next = 0L;
	status = noConflict;
	ftype = mac_type;
	fcreator = mac_creator;
}

void ReportConflict::PrintOut(std::string& out) const
{
	out = "";
	if( HasConflict() )
	{
		out += "Text/Binary conflict, at least these files :";
		out += lf;
		
		if( !sample1.empty() && !sample2.empty() )
		{
			out += "    ";
			out += sample1;
			out += " and ";
			out += sample2;
			out += lf;
		}
	}
	else if( HasTypeCreatorConflict() )
	{
		out += "Mac signature mismatch for this type, at least these files :";
		out += lf;
		
		if( !sample1.empty() && !sample2.empty() )
		{
			out += "    ";
			out += sample1;
			out += " and ";
			out += sample2;
			out += lf;
		}
	}
}

#if qCvsDebug
void ReportConflict::PrintOut(void) const
{
	if( pattern.empty() )
		return;
	
	cvs_out("  New type : %s%s (%s)\n",
		(status & hasExtension) ? "*" : "",
		pattern.c_str(),
		(status & isBinary) ? "BINARY" : "TEXT");
	
	if( !sample1.empty() && !sample2.empty() )
	{
		if( HasConflict() )
			cvs_out("    Text/Binary CONFLICT ! %s and %s\n",
			sample1.c_str(), sample2.c_str());

		else if( HasTypeCreatorConflict() )
			cvs_out("    Mac signature mismatch ! %s and %s\n",
			sample1.c_str(), sample2.c_str());
	}
}
#endif /* qCvsDebug */

ReportConflict::~ReportConflict()
{
}

/*!
	Add new pattern
	\param newpat Pattern to add
	\param fileType File type
	\param isExt Flag indicating whether the file has an extension
	\param sample Sample
*/
void ReportConflict::AddPattern(const char* newpat, kFileType fileType, bool isExt, const char* sample)
{
	pattern = newpat;
	sample1 = sample;
	
	switch( fileType )
	{
	case kFileTypeBin:
		status |= isBinary;
		break;
	case kFileTypeUnicode:
		status |= isUnicode;
		break;
	default:
		// nothing to do
		break;
	}

	if( isExt )
		status |= hasExtension;
}

/*!
	Match the pattern
	\param pat
	\return true if pattern matches, false otherwise
	\note Since it is the server who compares against the pattern that might be a case compare
*/
bool ReportConflict::Match(const char* pat)
{
	if( pat == 0L || pattern.empty() )
		return false;

	int len = strlen(pat);
	int patlen = pattern.length();

	if( status & hasExtension )
	{
		if( len < patlen )
			return false;

		return strcmp(pattern.c_str(), pat + len - patlen) == 0;
	}

	return strcmp(pattern.c_str(), pat) == 0;
}

/*!
	Mark conflict
	\param sample Sample
*/
void ReportConflict::SetConflict(const char* sample)
{
	// one is enough !
	if( HasConflict() )
		return;

	sample2 = sample;
	status |= hasConflict;
}

/*!
	Set type creator conflict
	\param sample Sample
*/
void ReportConflict::SetTypeCreatorConflict(const char* sample)
{
	// one is enough !
	if( HasTypeCreatorConflict() || HasConflict() )
		return;

	sample2 = sample;
	status |= hasTypeCreatorConflict;
}

/*!
	Get the conflist status
	\return true if there is a conflict, false otherwise
*/
bool ReportConflict::HasConflict(void) const
{
	return (status & hasConflict) != 0;
}

/*!
	Get the type creator conflist status
	\return true if there is a type creator conflict, false otherwise
	\note Mac only
*/
bool ReportConflict::HasTypeCreatorConflict(void) const
{
	return (status & hasTypeCreatorConflict) != 0;
} 

/*!
	Get the binary status
	\return true if the status is binary, false otherwise
*/
bool ReportConflict::IsBinary(void) const
{
	return (status & isBinary) != 0;
}

/*!
	Get the unicode status
	\return true if the status is unicode, false otherwise
*/
bool ReportConflict::IsUnicode(void) const
{
	return (status & isUnicode) != 0;
}

/*!
	Check whether conflict has an extension
	\return true if conflict has an extension
*/
bool ReportConflict::IsExtension(void) const
{
	return (status & hasExtension) != 0;
}

/*!
	Get the pattern
	\return The pattern
*/
const char* ReportConflict::GetPattern(void) const
{
	return pattern.c_str();
}

/*!
	Get the mac type
	\return The mac type
	\note Mac only
*/
int ReportConflict::GetMacType(void) const
{
	return ftype;
}

/*!
	Get the mac creator
	\return The mac creator
	\note Mac only
*/
int ReportConflict::GetMacCreator(void) const
{
	return fcreator;
} 

/*!
	Force binary status
*/
void ReportConflict::ForceBinary(void)
{
	status &= ~(int)userFlags;
	status |= (int)forceBinary;
}

/*!
	Force text status
*/
void ReportConflict::ForceText(void)
{
	status &= ~(int)userFlags;
	status |= (int)forceText;
}

/*!
	Force unicode status
*/
void ReportConflict::ForceUnicode(void)
{
	status &= ~(int)userFlags;
	status |= (int)forceUnicode;
}

/*!
	Mark conflict for ignore
*/
void ReportConflict::Ignore(void)
{
	status &= ~(int)userFlags;
	status |= (int)hasIgnore;
}

/*!
	Force no keyword expansion status
*/
void ReportConflict::ForceNoKeywords(void)
{
	status &= ~(int)userFlags;
	status |= (int)forceNoKeywords;
}

/*!
	Get the force binary status
	\return true if force binary is set
*/
bool ReportConflict::HasForceBinary(void) const
{
	return (status & forceBinary) != 0;
}

/*!
	Get the force text status
	\return true if force text is set
*/
bool ReportConflict::HasForceText(void) const
{
	return (status & forceText) != 0;
}

/*!
	Get the force unicode status
	\return true if force unicode is set
*/
bool ReportConflict::HasForceUnicode(void) const
{
	return (status & forceUnicode) != 0;
}

/*!
	Get the ignore status
	\return true if ignore is set
*/
bool ReportConflict::HasIgnore(void) const
{
	return (status & hasIgnore) != 0;
}

/*!
	Get the force no keyword expansion status
	\return true if force no keyword expansion is set
*/
bool ReportConflict::HasForceNoKeywords(void) const
{
	return (status & forceNoKeywords) != 0;
}

/*!
	Erase any user settings (force binary, force text etc.)
*/
void ReportConflict::DisableUserSettings(void)
{
	status &= ~(int)userFlags;
}

/*!
	Get the state description
	\return The state description as CPStr
*/
std::string ReportConflict::GetStateDesc() const
{
	return HasConflict() ? "Error" : "OK";
}

/*!
	Get the description
	\return The description as CPStr
*/
std::string ReportConflict::GetDesc() const
{
	string desc;
	
	desc += "Type ";
	desc += (IsExtension() ? "*" : "");
	desc += GetPattern();

	return desc;
}

/*!
	Get the kind description
	\return The kind description as CPStr
*/
std::string ReportConflict::GetKindDesc() const
{
	if( HasForceBinary() )
	{
		return "FORCE BINARY";
	}
	else if( HasForceUnicode() )
	{
		return "FORCE UNICODE";
	}
	else if( HasForceText() )
	{
		return "FORCE TEXT";
	}
	else if( HasForceNoKeywords() )
	{
		return "FORCE NO KEYWORDS";
	}
	else if( HasIgnore() )
	{
		return "IGNORE";
	}
	else if( IsBinary() )
	{
		return "BINARY";
	}
	else if( IsUnicode() )
	{
		return "UNICODE";
	}

	return "TEXT";
}

//////////////////////////////////////////////////////////////////////////
// ReportWarning

const size_t ReportWarning::maxWarnings = 10;

ReportWarning::ReportWarning(kTextBinTYPE akind)
{
	kind = akind;
	next = 0L;
	tooManyWarnings = false;
}

ReportWarning::~ReportWarning()
{
}

/*!
	Get the text description for warning type
	\param out Return warning's description
*/
void ReportWarning::PrintOut(std::string& out) const
{
	out = "";

	if( samples.size() == 0 )
		return;

	out = "Warning : ";
	switch(kind)
	{
	default:
	case kFileIsOK:
		out += "Files are OK";
		break;
	case kFileMissing:
		out += "Files are missing or unreadable";
		break;
	case kFileIsAlias:
		out += "Files are aliases";
		break;
	case kFileInvalidName:
		out += "Files have invalid name";
		break;
	case kTextWrongLF:
		out += "Files have the wrong line feed";
		break;
	case kTextIsBinary:
		out += "Files are binary";
		break;
	case kTextEscapeChar:
		out += "Files have escape characters in it";
		break;
	case kTextWrongSig:
		out += "Text files have the wrong signature";
		break;
	case kBinIsText:
		out += "Files are text, should be binary";
		break;
	case kBinWrongSig:
		out += "Binary files have the wrong signature";
		break;
	case kTextIsUnicode:
		out += "Files are unicode, should be text";
		break;
	case kBinIsUnicode:
		out += "Files are unicode, should be binary";
		break;
	}

	out += lf;
	
	for(vector<string>::const_iterator i = samples.begin(); i != samples.end(); ++i)
	{
		out += "    ";
		out += i->c_str();
		out += lf;
	}
	
	if( tooManyWarnings )
	{
		out += "    More files follow";
		out += lf;
	}
}

#if qCvsDebug
void ReportWarning::PrintOut(void) const
{
	if( samples.size() == 0 )
		return;

	cvs_out("  Warning : ");
	switch(kind)
	{
	default:
	case kFileIsOK:
		cvs_out("Files are OK\n");
		break;
	case kFileMissing:
		cvs_out("Files are missing or unreadable\n");
		break;
	case kFileIsAlias:
		cvs_out("Files are aliases\n");
		break;
	case kFileInvalidName:
		cvs_out("Files have invalid name\n");
		break;
	case kTextWrongLF: 
		cvs_out("Files have the wrong line feed\n");
		break;
	case kTextIsBinary:
		cvs_out("Files are binary\n");
		break;
	case kTextEscapeChar:
		cvs_out("Files have escape characters in it\n");
		break;
	case kTextWrongSig:
		cvs_out("Text files have the wrong signature\n");
		break;
	case kBinIsText:
		cvs_out("Files are text, should be binary\n");
		break;
	case kBinWrongSig:
		cvs_out("Binary files have the wrong signature\n");
		break;
	case kTextIsUnicode:
		cvs_out("Files are unicode, should be text\n");
		break;
	case kBinIsUnicode:
		cvs_out("Files are unicode, should be binary\n");
		break;
	}

	for(vector<string>::const_iterator i = samples.begin(); i != samples.end(); ++i)
	{
		cvs_out("    %s\n", i->c_str());
	}
	
	if( tooManyWarnings )
		cvs_out("    More files follow\n");
}
#endif /* qCvsDebug */

/*!
	Add warning
	\param sample Sample
*/
void ReportWarning::AddWarning(const char* sample)
{
	// it's enough !
	if( samples.size() >= maxWarnings )
	{
		tooManyWarnings = true;
		return;
	}

	samples.push_back(string(sample));
}

/*!
	Get the warning type
	\return The warning type
*/
kTextBinTYPE ReportWarning::Kind(void) const
{
	return kind;
}

/*!
	Check whether item represents an error
	\return true if item is an error, false if it is warning
	\note Item kind kFileIsOK should not happen and is considered an error
*/
bool ReportWarning::IsError() const
{
	return Kind() == kFileIsOK || Kind() == kTextEscapeChar;
}

/*!
	Get the state description
	\return The state description as CPStr
*/
std::string ReportWarning::GetStateDesc() const
{
	if( IsError() )
	{
		return "Error";
	}

	return "Warning";
}

/*!
	Get the description
	\return The description as CPStr
*/
std::string ReportWarning::GetDesc() const
{
	string desc;

	switch( Kind() )
	{
	case kFileIsOK:
		desc = "File is OK";
		break;
	case kFileMissing:
		desc = "File is missing or unreadable";
		break;
	case kFileIsAlias:
		desc = "File is an alias";
		break;
	case kFileInvalidName:
		desc = "File has an invalid name";
		break;
	case kTextWrongLF:
		desc = "File has the wrong line feed for this machine";
		break;
	case kTextIsBinary:
		desc = "File seems to be binary, while expecting text";
		break;
	case kTextEscapeChar:
		desc = "File has some escape characters in it (0x00-0x20, 0x80-0xFF)";
		break;
	case kTextIsUnicode:
		desc = "File seems to be unicode, while expecting text";
		break;
	case kBinIsUnicode:
		desc = "File seems to be unicode, while expecting binary";
		break;
	case kTextWrongSig:
		desc = "Text File has a wrong mac signature";
		break;
	case kBinWrongSig:
		desc = "Binary File has a wrong mac signature";
		break;
	default:
		desc = "File has some unknown problem";
		break;
	}

	return desc;
}

//////////////////////////////////////////////////////////////////////////
// ImportReport

ImportReport::ImportReport(const char* path, ReportWarning*& w, ReportConflict*& c) 
	: warnings(w), conflicts(c), m_progress(true, "Filtering files to import...", kProgressFile)
{
	// MN - Apply ignore filter to import file list
	BuildIgnoredList(m_ignlist, path);	
}

ImportReport::~ImportReport()
{
}

kTraversal ImportReport::EnterDirectory(const char* fullpath, const char* dirname, const UFSSpec* macspec)
{
	cvs_out("Filtering \'%s\'...\n", fullpath);
	return kContinueTraversal;
}

kTraversal ImportReport::ExitDirectory(const char* fullpath)
{
	return kContinueTraversal;
}

kTraversal ImportReport::OnError(const char* err, int errcode)
{
	list_add_warning(warnings, kFileMissing, err);
	return kContinueTraversal;
}

kTraversal ImportReport::OnIdle(const char* fullpath)
{
	if( m_progress.HasAborted() )
	{
		return kStopTraversal;
	}

	m_progress.SetProgress(fullpath);

	return kContinueTraversal;
}

kTraversal ImportReport::OnDirectory(const char* fullpath, const char* fullname, const char* name,
									 const struct stat& dir, const UFSSpec* macspec)
{
#if defined(WIN32) || defined(TARGET_OS_MAC)
	if(stricmp(name, "CVS") == 0)
#else
	if(strcmp(name, "CVS") == 0)
#endif
		return kSkipFile;

	return kContinueTraversal;
}

kTraversal ImportReport::OnFile(const char* fullpath, const char* fullname, const char* name,
								const struct stat& dir, const UFSSpec* macspec)
{
	int mac_type = 0;
	int mac_creator = 0;

#ifdef TARGET_OS_MAC
#	define MACSPEC , &fsSpec
	GUSIFileSpec inspec(fullname);
	FInfo finfo;
	memcpy(&finfo, inspec.CatInfo()->Info().finderInfo, 2 * sizeof(OSType));
	mac_type = finfo.fdType;
	mac_creator = finfo.fdCreator;
	UFSSpec fsSpec = inspec;
#else /* !TARGET_OS_MAC */
#	define MACSPEC
#endif /* !TARGET_OS_MAC */

	kFileType newType = kFileTypeText;
	kTextBinTYPE filetype = FileIsText(name, fullpath MACSPEC);

	if( kTextIsBinary == filetype || kTextWrongSig == filetype )
	{
		kTextBinTYPE bintype = FileIsBinary(name, fullpath MACSPEC);
		if( kFileIsOK != bintype )
		{
			list_add_warning(warnings, bintype, fullname);
		}
		else if( kTextWrongSig == filetype )
		{
			list_add_warning(warnings, filetype, fullname);
			filetype = kTextIsBinary;
			newType = kFileTypeBin;
		}
		
		if( kFileIsOK == bintype )
		{
			newType = kFileTypeBin;
		}
	}
	else if( kTextIsUnicode == filetype )
	{
		newType = kFileTypeUnicode;
	}
	else if( kFileIsOK != filetype )
	{
		list_add_warning(warnings, filetype, fullname);
	}
	
	// Add the type
	list_add_type(this, conflicts, newType, name, fullname, mac_type, mac_creator);

	return kContinueTraversal;
}

kTraversal ImportReport::OnAlias(const char* fullpath, const char* fullname, const char* name, 
								 const struct stat& dir, const UFSSpec* macspec)
{
	list_add_warning(warnings, kFileIsAlias, fullname);
	return kContinueTraversal;
}
