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
 * TclGlue.h --- glue to the TCL language
 */

#ifndef TCLGLUE_H
#define TCLGLUE_H

class EntnodeData;

#ifndef _TCL
#	define TCL_OK		0
#	define TCL_ERROR	1
#	define TCL_RETURN	2
#	define TCL_BREAK	3
#	define TCL_CONTINUE	4
#endif

typedef int kTclRes;

#define kTCLPathDelimiter '/'

struct Tcl_Interp;

void TclBrowserReset(void);
void TclBrowserAppend(const char* path, EntnodeData* data);
	// TO_OBSOLETE

/// TCL interpreter wrapper
class CTcl_Interp
{
public:
	// Construction
	CTcl_Interp();
	~CTcl_Interp();

private:
	// Data members
	Tcl_Interp* fInterp;	/*!< TCL interpreter */

	int fLastErrorCode;		/*!< ErrorCode value from the last CVS command (cvslaunch) */
	bool fExitOnError;		/*!< Exit on error flag (TRUE = Exit TCL macro automatically in error, FALSE = Do not exit) */

public:
	// Interface
	static bool IsAvail(const bool forceReload = false);
	
	kTclRes DoScriptVar(const char* format, ...);
	kTclRes DoScript(const char* script);
	kTclRes DoFile(const char* file);
	
	inline operator Tcl_Interp*(void);
	
	inline int  GetCvsLastErrorCode() const;
	inline void SetCvsLastErrorCode(const int value);

	inline bool GetCvsExitOnError() const;
	inline void SetCvsExitOnError(const bool value);

	static void Unixfy(std::string& path);
	static void Deunixfy(std::string& path);
};

//////////////////////////////////////////////////////////////////////////
// Inline implementation

/// Get the TCL interpreter
inline CTcl_Interp::operator Tcl_Interp*(void)
{
	return fInterp;
}

/// Get the last error code
inline int CTcl_Interp::GetCvsLastErrorCode() const
{
	return fLastErrorCode;
}

/// Set the last error code
inline void CTcl_Interp::SetCvsLastErrorCode(const int value)
{
	fLastErrorCode = value;
}

/// Get the exit on error flag
inline bool CTcl_Interp::GetCvsExitOnError() const
{
	return fExitOnError;
}

/// Set the exit on error flag
inline void CTcl_Interp::SetCvsExitOnError(const bool value)
{
	fExitOnError = value;
}

#endif /* TCLGLUE_H */
