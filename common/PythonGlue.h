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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- August 2001
 */

/*
 * PythonGlue.h --- glue to the Python language
 */

#ifndef PYTHONGLUE_H
#define PYTHONGLUE_H

#include "umain.h"

class UCmdUI;
class EntnodeData;


typedef struct _ts PyThreadState;

/// Macro type enum
typedef enum 
{
	kSelection = 0,
	kAdmin,
	kMacroTypeEnd /*!< End marker */
} kMacroType;

void PyMacrosReloadAll(void);
void PyDoCmdUI(UCmdUI* ucmdui);
void PyDoPython(int cmd);

bool PyIsUICacheValid();
void PyValidUICache();
void PyInvalUICache();
void PyAppendCache(EntnodeData* data);

UMENU GetTopLevelMacroMenu(kMacroType type, bool detach);

/// Python interpreter wrapper
class CPython_Interp
{
public:
	// Construction
	CPython_Interp(bool mainInterp = true);
	~CPython_Interp();

private:
	// Data members
	PyThreadState* fInterp;						/*!< Python interpreter */
	
	static PyThreadState* m_mainInterpreter;	/*!< Main  interpreter */

public:
	// Interface
	static bool IsAvail(const bool forceReload = false);

	int DoScriptVar(const char* format, ...);
	int DoScript(const char* script);
	int DoFile(const char* file);
	
	static void InitPythonSupport(const bool forceReload = false);
	static void ClosePythonSupport(void);

	inline operator PyThreadState*(void);
};

//////////////////////////////////////////////////////////////////////////
// Inline implementation

/// Get the Python interpreter
inline CPython_Interp::operator PyThreadState*(void)
{
	return fInterp;
}

#endif /* PYTHONGLUE_H */
