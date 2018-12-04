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

#include "stdafx.h"

#include "PythonImpl.h"
#include "CvsPrefs.h"
#include "umenu.h"

using namespace std;

#if USE_PYTHON

PY_IMPL_CLASS(CvsEntry, "Class for an entry in CVS/Entries")

PY_BEGIN_IMPL_METHOD(CvsEntry)
PY_END_IMPL_METHOD
	
PyObject* PyCvsEntry::Create(PyObject* selfptr, PyObject* args)
{
	if( !PyArg_ParseTuple(args, ":CCvsEntry") )
		return NULL;

	PyAutoObject<PyCvsEntry::obj_t> self = PyCvsEntry::New();
	if( !self.IsValid() )
		return NULL;

	TRY
	{
		self->m_object = new PyCvsEntry(0L);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return self.CopyReturn();
}

void PyCvsEntry::InitValues(PyObject* d)
{
}

PyObject* PyCvsEntry::GetAttr(char* name)
{
	string buf;
	EntnodeData* data = m_data;

	if( strcmp(name, "name") == 0 )
		return PyString_FromString(data->GetName());
	else if( strcmp(name, "fullname") == 0 )
		return PyString_FromString(data->GetFullPathName(buf));
	else if( strcmp(name, "version") == 0 )
		return PyString_FromString(((*data)[EntnodeFile::kVN]).c_str());
	else if( strcmp(name, "timeutc") == 0 )
		return PyString_FromString(((*data)[EntnodeFile::kTS]).c_str());
	else if( strcmp(name, "keyword") == 0 )
		return PyString_FromString(((*data)[EntnodeFile::kOption]).c_str());
	else if( strcmp(name, "tag") == 0 )
		return PyString_FromString(((*data)[EntnodeFile::kTag]).c_str());
	else if( strcmp(name, "isfile") == 0 )
		return data->GetType() == ENT_FILE ? True() : False();
	else if( strcmp(name, "modified") == 0 )
		return data->IsUnmodified() ? False() : True();
	else if( strcmp(name, "missing") == 0 )
		return data->IsMissing() ? True() : False();
	else if( strcmp(name, "unknown") == 0 )
		return data->IsUnknown() ? True() : False();
	else if( strcmp(name, "ignored") == 0 )
		return data->IsIgnored() ? True() : False();
	else if( strcmp(name, "readonly") == 0 )
		return data->IsLocked() ? True() : False();
	else if( strcmp(name, "removed") == 0 )
		return data->IsRemoved() ? True() : False();
	else if( strcmp(name, "added") == 0 )
		return data->IsAdded() ? True() : False();
	else if( strcmp(name, "state") == 0 )
		return PyString_FromString(data->GetState());
	else if( strcmp(name, "encoding") == 0 )
		return PyString_FromString(((*data)[EntnodeFile::kEncoding]).c_str());
	else if( strcmp(name, "conflict") == 0 )
		return PyString_FromString(data->GetConflict());

	return NULL;
}

PY_IMPL_CLASS(ColorConsole, "Class printing in color in the console")

PY_BEGIN_IMPL_METHOD(ColorConsole)
	PY_IMPL_METHOD(ColorConsole, Print, "Print in color")
PY_END_IMPL_METHOD
	
PyObject* PyColorConsole::Create(PyObject* selfptr, PyObject* args)
{
	if( !PyArg_ParseTuple(args, ":CColorConsole") )
		return NULL;

	PyAutoObject<PyColorConsole::obj_t> self = PyColorConsole::New();
	if( !self.IsValid() )
		return NULL;

	TRY
	{
		self->m_object = new PyColorConsole();
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return self.CopyReturn();
}


void PyColorConsole::InitValues(PyObject* d)
{
	RegisterValue(d, "kNormal", kNormal);
	RegisterValue(d, "kBrown", kBrown);
	RegisterValue(d, "kGreen", kGreen);
	RegisterValue(d, "kMagenta", kMagenta);
	RegisterValue(d, "kRed", kRed);
	RegisterValue(d, "kBlue", kBlue);
	RegisterValue(d, "kBold", kBold);
	RegisterValue(d, "kItalic", kItalic);
	RegisterValue(d, "kUnderline", kUnderline);
	RegisterValue(d, "kNL", kNL);
}

PyObject* PyColorConsole::Print(PyObject* args)
{
	int style;

	if( PyArg_ParseTuple(args, "i", &style) )
	{
		m_console << kConsoleColor(style);
		return Void();
	}

	PyErr_Clear();

	char* str;
	if( PyArg_ParseTuple(args, "s", &str) )
	{
		m_console << str;
		return Void();
	}

	return NULL;
}

PY_IMPL_CLASS(MenuMgr, "Class to manipulate menus")

PY_BEGIN_IMPL_METHOD(MenuMgr)
	PY_IMPL_METHOD(MenuMgr, AddPulldown, "Add a new menu")
	PY_IMPL_METHOD(MenuMgr, AddToggle, "Add a toggle menu entry")
	PY_IMPL_METHOD(MenuMgr, AddButton, "Add a regular menu entry")
	PY_IMPL_METHOD(MenuMgr, AddSeparator, "Add a separator menu entry")
	PY_IMPL_METHOD(MenuMgr, Delete, "Delete a menu entry by index")
	PY_IMPL_METHOD(MenuMgr, DeleteByCmd, "Delete a menu entry by command")
	PY_IMPL_METHOD(MenuMgr, Count, "Count the number of entries in the menu")
PY_END_IMPL_METHOD

static int ParseUMENU(PyObject* v, UMENU* menu)
{
	if(v->ob_type == PyMenuMgr::GetType())
	{
		*menu = *((PyMenuMgr::obj_t*)v)->m_object;
		return 1;
	}

	return 0;
}
	
PyObject* PyMenuMgr::Create(PyObject* selfptr, PyObject* args)
{
	UMENU menu;

	if( !PyArg_ParseTuple(args, "i:CMenuMgr", &menu) )
		return NULL;

	PyAutoObject<PyMenuMgr::obj_t> self = PyMenuMgr::New();
	if( !self.IsValid() )
		return NULL;

	TRY
	{
		self->m_object = new PyMenuMgr(menu);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return self.CopyReturn();
}

void PyMenuMgr::InitValues(PyObject* d)
{
}

PyObject* PyMenuMgr::AddPulldown(PyObject* args)
{
	int widid;
	char* name;

	if( !PyArg_ParseTuple(args, "is", &widid, &name) )
		return NULL;

	PyAutoObject<PyMenuMgr::obj_t> res = PyMenuMgr::New();
	if( !res.IsValid() )
		return NULL;

	res->m_object = new PyMenuMgr(UMenuAddPulldown(widid, name, m_menu));

	return res.CopyReturn();
}

PyObject* PyMenuMgr::AddToggle(PyObject* args)
{
	int widid, cmd;
	char *name, *pixmap = 0L;

	if( !PyArg_ParseTuple(args, "isi|s", &widid, &name, &cmd, &pixmap) )
		return NULL;

	UMenuAddToggle(widid, name, m_menu, pixmap, cmd);

	return Void();
}

PyObject* PyMenuMgr::AddButton(PyObject* args)
{
	int widid, cmd;
	char *name, *pixmap = 0L;

	if( !PyArg_ParseTuple(args, "isi|s", &widid, &name, &cmd, &pixmap) )
		return NULL;

	UMenuAddButton(widid, name, m_menu, pixmap, cmd);

	return Void();
}

PyObject* PyMenuMgr::AddSeparator(PyObject* args)
{
	int widid;

	if( !PyArg_ParseTuple(args, "i", &widid) )
		return NULL;

	UMenuAddSeparator(widid, m_menu);

	return Void();
}

PyObject* PyMenuMgr::Delete(PyObject* args)
{
	int afterIndex;

	if( !PyArg_ParseTuple(args, "i", &afterIndex) )
		return NULL;

	UMenuDelete(m_menu, afterIndex);

	return Void();
}

PyObject* PyMenuMgr::DeleteByCmd(PyObject* args)
{
	int cmd;

	if( !PyArg_ParseTuple(args, "i", &cmd) )
		return NULL;

	UMenuDeleteByCmd(m_menu, cmd);

	return Void();
}

PyObject* PyMenuMgr::Count(PyObject* args)
{
	int res;

	if( !PyArg_ParseTuple(args, "") )
		return NULL;

	res = UMenuCount(m_menu);

	return PyInt_FromLong(res);
}

PY_IMPL_CLASS(CmdUI, "Class to update an interface object state")

PY_BEGIN_IMPL_METHOD(CmdUI)
	PY_IMPL_METHOD(CmdUI, Enable, "Enable/Disable the object")
	PY_IMPL_METHOD(CmdUI, SetCheck, "Check/uncheck the object")
	PY_IMPL_METHOD(CmdUI, SetText, "Rename the object")
PY_END_IMPL_METHOD
	
PyObject* PyCmdUI::Create(PyObject* selfptr, PyObject* args)
{
	UCmdUI *cmdui;

#if defined(SIZEOF_VOID_P) && (SIZEOF_VOID_P == 8)
	if( !PyArg_ParseTuple(args, "K:CCmdUI", &cmdui) )
		return NULL;
#else
	if( !PyArg_ParseTuple(args, "i:CCmdUI", &cmdui) )
		return NULL;
#endif
	PyAutoObject<PyCmdUI::obj_t> self = PyCmdUI::New();
	if( !self.IsValid() )
		return NULL;

	TRY
	{
		self->m_object = new PyCmdUI(cmdui);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return self.CopyReturn();
}

void PyCmdUI::InitValues(PyObject* d)
{
}

PyObject* PyCmdUI::Enable(PyObject* args)
{
	int state;

	if( !PyArg_ParseTuple(args, "i", &state) )
		return NULL;

	m_cmdui->Enable(state != 0);

	return Void();
}

PyObject* PyCmdUI::SetCheck(PyObject* args)
{
	int state;

	if( !PyArg_ParseTuple(args, "i", &state) )
		return NULL;

	m_cmdui->Check(state != 0);

	return Void();
}

PyObject* PyCmdUI::SetText(PyObject* args)
{
	char* text;

	if( !PyArg_ParseTuple(args, "s", &text) )
		return NULL;

	m_cmdui->SetText(text);

	return Void();
}

PY_IMPL_CLASS(Persistent, "Class to store persistent values on disk")

PY_BEGIN_IMPL_METHOD(Persistent)
	PY_IMPL_METHOD(Persistent, GetValue, "Get the persistent value")
	PY_IMPL_METHOD(Persistent, SetValue, "Set the persistent value")
PY_END_IMPL_METHOD
	
PyObject* PyPersistent::Create(PyObject* selfptr, PyObject* args)
{
	char *uniqueName, *defValue = 0L;
	int addToSettings = 0;
	
	if( !PyArg_ParseTuple(args, "s|si:CPersistent", &uniqueName, &defValue, &addToSettings) )
		return NULL;

	PyAutoObject<PyPersistent::obj_t> self = PyPersistent::New();
	if( !self.IsValid() )
		return NULL;

	TRY
	{
		self->m_object = new PyPersistent(uniqueName, defValue, addToSettings ? kAddSettings : kNoClass);

		CPersistent::LoadAll(uniqueName);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return self.CopyReturn();
}

void PyPersistent::InitValues(PyObject* d)
{
}

PyObject* PyPersistent::SetValue(PyObject* args)
{
	char* s;

	if( !PyArg_ParseTuple(args, "s", &s) )
		return NULL;

	m_cstr = s;

	gCvsPrefs.save(true);

	return Void();
}

PyObject* PyPersistent::GetValue(PyObject* args)
{
	if( !PyArg_ParseTuple(args, "") )
		return NULL;

	return PyString_FromString(m_cstr.c_str());
}

#if (defined(WIN32) && !qCvsDebug) || TARGET_RT_MAC_MACHO

#undef PYTHONGLUE_IMPL_H
#undef PYTHONGLUE_EXTERN
#define PYTHONGLUE_EXTERN
#include "PythonGlue.impl.h"

PyTypeObject* GetPyType_Type()
{
	return PyType_Type_glue;
}

PyTypeObject* GetPyFile_Type()
{
	return PyFile_Type_glue;
}

PyTypeObject* GetPyString_Type()
{
	return PyString_Type_glue;
}

PyTypeObject* GetPyTuple_Type()
{
	return PyTuple_Type_glue;
}

PyTypeObject* GetPyList_Type()
{
	return PyList_Type_glue;
}

PyTypeObject* GetPyInt_Type()
{
	return PyInt_Type_glue;
}

PyObject* GetPyNone()
{
	return _Py_NoneStruct_glue;
}

PyObject* GetPyTrue()
{
	return _Py_TrueStruct_glue;
}

PyObject* GetPyFalse()
{
	return _Py_ZeroStruct_glue;
}
#endif // WIN32 && !qCvsDebug

#endif // USE_PYTHON
