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
 * AppGlue.h --- glue code to access CVS services
 */

#ifndef APPGLUE_H
#define APPGLUE_H

#include <stdio.h>
#include <string>
#include <vector>

#ifdef WIN32
	#include <fcntl.h>
	#include <io.h>
#endif

typedef struct _CvsProcess CvsProcess;

// FIXME: To be removed and replaced by value in CvsProcessErrorInfo once the sLastExitCode is eliminated
#define errInternal -99
#define errThreadRunning -98
#define errNoCvsguiProtocol -97

// Hooks for CVS
extern "C" {
	extern long glue_consoleout(const char* txt, const long len, const CvsProcess* process);
	extern long glue_consoleerr(const char* txt, const long len, const CvsProcess* process);
	extern long glue_consolein(const char* txt, const long len, const CvsProcess* process);
	extern const char* glue_getenv(const char* name, const CvsProcess* process);
	extern void glue_exit(const int code, const CvsProcess* process);
	extern void glue_ondestroy(const CvsProcess* process);
}

class CCvsConsole;
class CvsArgs;

int testCVS(const char* path, int argc, char* const* argv);
int launchCVS(const char* path, int argc, char* const* argv, CCvsConsole* console = 0L);
int launchCVS(int test, const char* path, int argc, char* const* argv, CCvsConsole* console = 0L, const char* cvsfile = 0L);
void stopCVS();

FILE* createCVSTempFile(std::string& tmpFile, const char* prefix, const char* extension);
const char* registerTempFile(const char* filename);

FILE* launchCVS(const char* dir, const CvsArgs& args, std::string& tmpFile, 
				const char* prefix = 0L, const char* extension = 0L);

const char* launchCVS(const char* dir, const CvsArgs& args, 
					  const char* prefix = 0L, const char* extension = 0L);

#if defined(WIN32) || TARGET_RT_MAC_MACHO
	/// Wait for the CVS thread to terminate
	void WaitForCvs(void);
#else
#	define WaitForCvs()
#endif

/// Application data holder passed to the cvsgui protocol callbacks
class CCvsProcessData
{
public:
	// Construction
	CCvsProcessData();

private:
	// Data members
	CCvsConsole* m_pCvsConsole;

public:
	// Interface
	void SetCvsConsole(CCvsConsole* pCvsConsole);
	CCvsConsole* GetCvsConsole() const;
	CCvsConsole* GetCvsConsole();
};

/// Abstract base class for CVS process console
class CCvsConsole
{
public:
	virtual ~CCvsConsole() {};

	/*!
		Stdout handler
		\param txt Text
		\param len Text length
		\return The character count written to the console
	*/
	virtual long cvs_out(const char* txt, long len) = 0L;

	/*!
		Stderr handler
		\param txt Text
		\param len Text length
		\return The character count written to the console
	*/
	virtual long cvs_err(const char* txt, long len) = 0L;
};

/// Class to implement filter console that looks for a specific
/// string at the beginning of a line and calls a handler
/// with what comes after that string
class CCvsFilterConsole : public CCvsConsole
{
public:
	/// Filter options (bitwise OR)
	enum kFilterOptions
	{
		FILTER_OUT = 1,		/*!< Filter stdout */
		FILTER_ERR = 2,		/*!< Filter stderr */
		PRINT_MATCH = 4,	/*!< Print match to console */
	};

	CCvsFilterConsole(const char* filter, int options);

	// Interface
	virtual long cvs_out(const char* txt, long len);
	virtual long cvs_err(const char* txt, long len);

	/*!
		Filter text 
		\param txt Text
	*/
	virtual void OnFilter(const std::string& txt) = 0;

private:
	/*!
		Filter handler
		\param txt Text
		\param len Text length
		\return The character count written to the console
	*/
	long cvs_filter(const char* txt, long len);

	/// Parse states
	enum kStates 
	{
		AFTER_NL,		/*!< After new line */
		AFTER_FILTER,	/*!< After file marker */
		NONE,			/*!< Neutral */
	};

	// Members
	kStates m_state;			/*!< State */
	std::string m_afterFilter;	/*!< Text after filter */
	std::string m_filter;		/*!< Filter */
	int m_options;				/*!< Filter options */
};

/// Base class for CVS stream console
class CCvsStreamConsole : public CCvsConsole
{
public:
	// Construction
	CCvsStreamConsole(FILE* fileOut = 0L, bool autoClose = true);
	virtual ~CCvsStreamConsole();

protected:
	// Members
	bool m_autoClose;	/*!< Flag to indicate that output file should be closed by the destructor */
	bool m_modeSet;		/*!< Flag to indicate that output file mode is set */
	FILE* m_fileOut;	/*!< Output file handle */

	// Methods
	virtual void SetMode(bool binary);
	virtual bool IsModeSet() const;

public:
	// Interface
	virtual bool Open(const char* fileName);

	virtual long cvs_out(const char* txt, long len);
	virtual long cvs_err(const char* txt, long len);
};


/// Class to implement stream console
class CStreamConsole : public CCvsStreamConsole
{
public:
	// Construction
	CStreamConsole(FILE* fileOut);
};

#if INTERNAL_AUTHEN
/// Class to setup authentication model and proper environment
class CAuthenSetUp
{
public:
	CAuthenSetUp();
	virtual ~CAuthenSetUp();
};
#endif /* INTERNAL_AUTHEN */

/// Class to support the clean up of any temporary files created
class CTmpFiles
{
public:
	// Construction
	CTmpFiles();
	virtual ~CTmpFiles();

protected:
	// Data members
	std::vector<std::string> m_allfiles;	/*!< Files collection */

#ifdef WIN32
	bool m_removeOnReboot;	/*!< true to schedule files for removal at the next boot instead of removing at object destruction */
#endif

public:
	// Interface
	const char* push_back(const char* file);
};

#endif /* APPGLUE_H */
