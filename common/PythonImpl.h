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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- September 2001
 */

#ifndef __PYIMPL
#define __PYIMPL

#include "PythonContainer.h"

#if USE_PYTHON
#include "CvsEntries.h"
#include "AppConsole.h"
#include "umain.h"

/// Cvs Entry Python wrapper
class PyCvsEntry : public PyContainer
{
	PY_DECL_CLASS(CvsEntry)

public:
	PyCvsEntry(EntnodeData* data) : m_data(data)
	{
	}

	virtual ~PyCvsEntry()
	{
	}

	virtual PyObject* GetAttr(char* name);

	/// EntnodeData* operator 
	inline operator EntnodeData* (void) const { return m_data; }

protected:
	ENTNODE m_data; /*!< Node data */
};

/// Color console Python wrapper
class PyColorConsole : public PyContainer
{
	PY_DECL_CLASS(ColorConsole)
	PY_DECL_METHOD(ColorConsole, Print)

public:
	PyColorConsole()
	{
	}

	virtual ~PyColorConsole()
	{
	}

protected:
	CColorConsole m_console;	/*!< Console */
};

/// Menu Python wrapper
class PyMenuMgr : public PyContainer
{
	PY_DECL_CLASS(MenuMgr)
	PY_DECL_METHOD(MenuMgr, AddPulldown)
	PY_DECL_METHOD(MenuMgr, AddToggle)
	PY_DECL_METHOD(MenuMgr, AddButton)
	PY_DECL_METHOD(MenuMgr, AddSeparator)
	PY_DECL_METHOD(MenuMgr, Delete)
	PY_DECL_METHOD(MenuMgr, DeleteByCmd)
	PY_DECL_METHOD(MenuMgr, Count)

public:
	PyMenuMgr(UMENU menu) : m_menu(menu)
	{
	}

	virtual ~PyMenuMgr()
	{
	}

	/// UMENU operator
	inline operator UMENU() const { return m_menu; }

protected:
	UMENU m_menu;	/*!< Menu */
};

/// Cmd UI Python wrapper
class PyCmdUI : public PyContainer
{
	PY_DECL_CLASS(CmdUI)
	PY_DECL_METHOD(CmdUI, Enable)
	PY_DECL_METHOD(CmdUI, SetCheck)
	PY_DECL_METHOD(CmdUI, SetText)

public:
	PyCmdUI(UCmdUI* cmdui) : m_cmdui(cmdui)
	{
	}

	virtual ~PyCmdUI()
	{
	}

protected:
	UCmdUI* m_cmdui;	/*!< Cmd UI */
};

class PyPersistent : public PyContainer
{
	PY_DECL_CLASS(Persistent)
	PY_DECL_METHOD(Persistent, GetValue)
	PY_DECL_METHOD(Persistent, SetValue)

public:
	PyPersistent(const char* uniqueName, const char* defValue, kClassPersistent pclass) :
		m_cstr(uniqueName, defValue, pclass)
	{
	}

	virtual ~PyPersistent()
	{
	}

protected:
	CPersistentString m_cstr;	/*!< Command string */
};
#endif // USE_PYTHON

#endif
