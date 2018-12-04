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
 * PythonGlue.cpp --- glue to the Python language
 */

#include "stdafx.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#	include "resource.h"
#	include "NoPython.h"
#endif /* WIN32 */

#ifdef qUnix
#	include <iostream>
#	include <fstream>
#	include "UCvsApp.h"
#	include "UCvsMenubar.h"
#	include "UCvsCommands.h"
#	include "support.h"
#endif

#include "PythonGlue.h"
#include "CvsLog.h"
#include "AppConsole.h"
#include "MacrosSetup.h"
#include "umenu.h"
#include "PythonContainer.h"
#include "PythonImpl.h"
#include "TclGlue.h"
#include "AppGlue.h"
#include "CvsArgs.h"
#include "FileTraversal.h"
#include "AskYesNo.h"
#include "dll_loader.h"
#include "CvsPrefs.h"
#include "CvsAlert.h"

#if USE_PYTHON
#	include <osdefs.h>
#endif

#if qMacCvsPP
#	include "MacCvsApp.h"
#	include "MacCvsConstant.h"
#	include "MacMisc.h"
#endif

#if qCarbon
#	include "TclGlue.mac.h"
#endif

using namespace std;

#ifdef WIN32
#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

#if TARGET_RT_MAC_MACHO
#	define CompatLoadLibrary CarbonCompatLoadLibrary
#	define CompatCloseLibrary CarbonCompatCloseLibrary
#	define _CompatCallLibrary _CarbonCompatCallLibrary
#endif

/// Top level macro menu collection
static UMENU gTopLevelMacroMenu[] = {
	NULL,
	NULL
};

/*!
	Get the top level menu for macro of specified type
	\param type Type of macro menu
	\param detach true to detach the macro from the application menu if it wasn't detached before
	\return true on success, false otherwise
*/
UMENU GetTopLevelMacroMenu(kMacroType type, bool detach)
{
	UMENU& macroMenu = gTopLevelMacroMenu[type];

	if( macroMenu == 0L && detach )
	{
#ifdef WIN32
		int pos;
		CMenu* top = GetMenuEntry(type == kSelection ? ID_MACRO_SEL : ID_MACRO_ADMIN, pos);
		if(top)
		{
			top->DeleteMenu(pos, MF_BYPOSITION);
			macroMenu = (UMENU)top->GetSafeHmenu();
		}
#endif
#if qMacCvsPP
		LMenuBar *menuBar	= LMenuBar::GetCurrentMenuBar();
		LMenu *menu = menuBar->FetchMenu(type == kSelection ? menu_MacroSel : menu_MacroAdmin);
		if(menu)
			macroMenu = menu->GetMenuID();
#endif
#if qUnix
		UCvsMenubar *menuBar = (UCvsMenubar *)UEventQueryWidget(kUCvsMenubarID);
		UMenu *menu = (UMenu *)(menuBar->GetWidget(type == kSelection ? cmdMACROS : cmdADMINMACROS));
		
		if(menu) {
			UMenuDelete((UMENU)menu, 0);
			macroMenu = (UMENU)menu;
		}
#endif
	}

	return macroMenu;
}

#if USE_PYTHON

/// Alert data
struct CvsAlerData
{
	CvsAlerData(const bool isPrompt = false)
		: m_isPrompt(isPrompt)
	{
		m_value = "";
		m_icon = "";
		m_message = "";
		m_messageDetails = "";
		m_okButtonTitle = "";
		m_cancelButtonTitle = "";
		m_otherButtonTitle = "";
		m_defaultButton = "";
		m_alertTitle = "";
	}

	bool m_isPrompt;	/*!< true for the prompt, false for alert only */
	char* m_value;		/*!< For prompt only, suggested value */

	char* m_icon;
	char* m_message;
	char* m_messageDetails;
	char* m_okButtonTitle;
	char* m_cancelButtonTitle;
	char* m_otherButtonTitle;
	char* m_defaultButton;
	char* m_alertTitle;
};

/// Alert result
struct CvsAlerResult
{
	std::string m_strRes;	/*!< Button click as text */
	std::string m_strValue;	/*!< Entered prompt value, applicable for prompt dialog only */
};

/*!
	Show the alert or prompt
	\param cvsAlerData Alert data
	\return Alert result
*/
CvsAlerResult ShowAlert(CvsAlerData& cvsAlerData)
{
	CvsAlerResult res;

	kCvsAlertIconType alertIcon = kCvsAlertNoIcon;
	if( strcmp(cvsAlerData.m_icon, "NoIcon") == 0 )
	{
		alertIcon = kCvsAlertNoIcon;
	}
	else if( strcmp(cvsAlerData.m_icon, "Stop") == 0 )
	{
		alertIcon = kCvsAlertStopIcon;
	}
	else if( strcmp(cvsAlerData.m_icon, "Note") == 0 )
	{
		alertIcon = kCvsAlertNoteIcon;
	}
	else if( strcmp(cvsAlerData.m_icon, "Warning") == 0 )
	{
		alertIcon = kCvsAlertWarningIcon;
	}
	else if( strcmp(cvsAlerData.m_icon, "Question") == 0 )
	{
		alertIcon = kCvsAlertQuestionIcon;
	}
	
	if( strcmp(cvsAlerData.m_messageDetails, "") == 0 )
	{
		cvsAlerData.m_messageDetails = NULL;
	}
	
	std::auto_ptr<CvsAlert> alert(cvsAlerData.m_isPrompt ? new CvsPrompt(alertIcon, cvsAlerData.m_message, cvsAlerData.m_messageDetails) : new CvsAlert(alertIcon, cvsAlerData.m_message, cvsAlerData.m_messageDetails));
	
	if( strcmp(cvsAlerData.m_okButtonTitle, "") != 0 )
	{
		alert->SetButtonTitle(kCvsAlertOKButton, cvsAlerData.m_okButtonTitle);
	}
	
	if( strcmp(cvsAlerData.m_cancelButtonTitle, "") != 0 )
	{
		alert->SetButtonTitle(kCvsAlertCancelButton, cvsAlerData.m_cancelButtonTitle);
	}
	
	if( strcmp(cvsAlerData.m_otherButtonTitle, "") != 0 )
	{
		alert->SetButtonTitle(kCvsAlertOtherButton, cvsAlerData.m_otherButtonTitle);
	}
	
	if( cvsAlerData.m_isPrompt && strcmp(cvsAlerData.m_value, "") != 0 )
	{
		dynamic_cast<CvsPrompt*>(alert.get())->SetValue(cvsAlerData.m_value);
	}
	
	kCvsAlertButtonType alertDefaultButton = kCvsAlertOKButton;
	if( strcmp(cvsAlerData.m_defaultButton, "") != 0 )
	{
		if( strcmp(cvsAlerData.m_defaultButton, "IDOK") == 0 )
		{
			alertDefaultButton = kCvsAlertOKButton;
		}
		else if( strcmp(cvsAlerData.m_defaultButton, "IDCANCEL") == 0 )
		{
			alertDefaultButton = kCvsAlertCancelButton;
		}
		else if( strcmp(cvsAlerData.m_defaultButton, "IDOTHER") == 0 )
		{
			alertDefaultButton = kCvsAlertOtherButton;
		}
	}
	
	if( strcmp(cvsAlerData.m_alertTitle, "") != 0 )
	{
		alert->SetAlertTitle(cvsAlerData.m_alertTitle);
	}
	
	switch( alert->ShowAlert(alertDefaultButton) )
	{
	case kCvsAlertOKButton:
		res.m_strRes = "IDOK";
		break;
	default:
	case kCvsAlertCancelButton:
		res.m_strRes = "IDCANCEL";
		break;
	case kCvsAlertOtherButton:
		res.m_strRes = "IDOTHER";
		break;
	}

	if( cvsAlerData.m_isPrompt )
	{
		res.m_strValue = dynamic_cast<CvsPrompt*>(alert.get())->GetValue();
	}

	return res;
}

PyThreadState *CPython_Interp::m_mainInterpreter = 0L;
static bool gUICacheValid = false;
static std::vector<ENTNODE> gUICache;
static PyObject *gUICacheObj = NULL;

class CPyThreadState
{
public:
	CPyThreadState(PyThreadState *p) : m_p(p)
	{
		m_p = PyThreadState_Swap(p);
	}
	~CPyThreadState()
	{
		PyThreadState_Swap(m_p);
	}
protected:
	PyThreadState *m_p;
};

enum
{
	_stdout_style = 15,
	_stderr_style = 16,
	_trace_style = 17
};

static PyObject *cvsgui_write(PyObject *self, PyObject *args)
{
	char *str;
	int style;
	PyObject *shell;

	if(!PyArg_ParseTuple(args, "siO", &str, &style, &shell))
		return NULL;

	TRY
	{
		if(style == _stdout_style)
			cvs_outstr(str, strlen(str));
		else
			cvs_errstr(str, strlen(str));
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyContainer::Void();
}

static PyObject *cvsgui_GetMainMenuWidget(PyObject *self, PyObject *args)
{
	int res = 0;

	if(!PyArg_ParseTuple(args, ""))
		return NULL;

	TRY
	{
#ifdef qUnix
		UCvsMenubar *menuBar = (UCvsMenubar *)UEventQueryWidget(kUCvsMenubarID);
		
		res = menuBar->GetWidID();
#endif		
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyInt_FromLong(res);
}

static PyObject *cvsgui_GetTopMacroSelectionMenu(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, ""))
		return NULL;

	PyAutoObject<PyMenuMgr::obj_t> res = PyMenuMgr::New();
	if (!res.IsValid())
		return NULL;

	TRY
	{
		UMENU macroMenu = GetTopLevelMacroMenu(kSelection, true);

		res->m_object = new PyMenuMgr(macroMenu);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return res.CopyReturn();
}

static PyObject *cvsgui_GetTopMacroAdminMenu(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, ""))
		return NULL;

	PyAutoObject<PyMenuMgr::obj_t> res = PyMenuMgr::New();
	if (!res.IsValid())
		return NULL;

	TRY
	{
		UMENU macroMenu = GetTopLevelMacroMenu(kAdmin, true);

		res->m_object = new PyMenuMgr(macroMenu);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return res.CopyReturn();
}

static PyObject *cvsgui_GetSelection(PyObject *self, PyObject *args)
{
	if(!PyArg_ParseTuple(args, ""))
		return NULL;

	if(gUICacheObj != 0L)
	{
		Py_INCREF(gUICacheObj);
		return gUICacheObj;
	}

	gUICacheObj = PyTuple_New(gUICache.size());
	if(gUICacheObj == NULL)
		return NULL;

	std::vector<ENTNODE>::const_iterator i;
	int cnt;
	for(cnt = 0, i = gUICache.begin(); i != gUICache.end(); ++i, ++cnt)
	{
		PyAutoObject<PyCvsEntry::obj_t> res = PyCvsEntry::New();
		if (!res.IsValid())
			return NULL;

		res->m_object = new PyCvsEntry(*i);

		PyTuple_SET_ITEM(gUICacheObj, cnt, res.CopyReturn());
	}

	Py_INCREF(gUICacheObj);
	return gUICacheObj;
}

class CFillPyEntries : public TraversalReport
{
public:
	CSortList<ENTNODE>& m_entries;
	ignored_list_type m_ignlist;

	CFillPyEntries(CSortList<ENTNODE>& entries) :
		m_entries(entries) {}

	virtual ~CFillPyEntries() {}

	virtual kTraversal EnterDirectory(const char* fullpath, const char* dirname, const UFSSpec* macspec)
	{
		Entries_Open (m_entries, fullpath);
		BuildIgnoredList(m_ignlist, fullpath);
		
		return kContinueTraversal;
	}

	virtual kTraversal ExitDirectory(const char *fullpath)
	{
		m_ignlist.erase(m_ignlist.begin(), m_ignlist.end());
		
		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char *err, int errcode)
	{
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char *fullpath)
	{
		return kContinueTraversal;
	}

	virtual kTraversal OnDirectory(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const UFSSpec * macspec)
	{
#if defined(WIN32) || defined(TARGET_OS_MAC)
		if(stricmp(name, "CVS") == 0)
#else
		if(strcmp(name, "CVS") == 0)
#endif
			return kSkipFile;

		/*EntnodeData *data = */Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);

		return kSkipFile;
	}

	virtual kTraversal OnAlias(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const UFSSpec * macspec)
	{
		return OnFile(fullpath, fullname, name, dir, macspec);
	}

	virtual kTraversal OnFile(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const UFSSpec * macspec)
	{
		/*EntnodeData *data = */Entries_SetVisited(fullpath, m_entries, name, dir, false, &m_ignlist);

		return kContinueTraversal;
	}
};

static PyObject *cvsgui_GetCvsEntries(PyObject *self, PyObject *args)
{
	char *path;

	if(!PyArg_ParseTuple(args, "s", &path))
		return NULL;

	CSortList<ENTNODE> entries(200, ENTNODE::Compare);
	CFillPyEntries traverse(entries);
	/*kTraversal res = */FileTraverse(path, traverse);

	// add the missing files
	Entries_SetMissing(entries);

	int numEntries = entries.NumOfElements();
	PyObject *obj = PyTuple_New(numEntries);
	if(obj == NULL)
		return NULL;

	for(int i = 0; i < numEntries; i++)
	{
		const ENTNODE & theNode = entries.Get(i);
		EntnodeData *data = theNode.Data();

		PyAutoObject<PyCvsEntry::obj_t> res = PyCvsEntry::New();
		if (!res.IsValid())
			return NULL;

		res->m_object = new PyCvsEntry(data);

		PyTuple_SET_ITEM(obj, i, res.CopyReturn());
	}

	return obj;
}

#if defined(WIN32)
//
// Get correct (UTC) file modification time.
// Used for dealing with notorious DST bug.
//
static PyObject * cvsgui_GetFileModTime(PyObject *self, PyObject *args)
{
    struct wnt_stat sb;
    struct tm * p_utc = NULL;

	const char *path;

// PyObject_Length exists only for debugging version (ActiveState Python 2.1)
#if defined(_DEBUG)
    int len = PyObject_Length(args);
    if (len < 1)
    {
        char errstr[255];
        sprintf(errstr, "Too few arguments (%d, expected 1)", len);
        PyErr_SetString(gCvsguiErr, errstr);
        return NULL;
    }
    else if (len > 1)
    {
        char errstr[255];
        sprintf(errstr, "Too many arguments (%d, expected 1)", len);
        PyErr_SetString(gCvsguiErr, errstr);
        return NULL;
    }
#endif

	if(!PyArg_ParseTuple(args, "s", &path))
		return NULL;

    if (wnt_stat(path, &sb) != 0)
    {
        char errstr[255];
        sprintf(errstr, "Could not find file \"%s\"", path);
        PyErr_SetString(gCvsguiErr, errstr);
        return NULL;
    }

    p_utc = gmtime(& sb.st_mtime);
    return Py_BuildValue("s", asctime(p_utc));
}
#endif	// defined(_WIN32)

static PyObject *cvsgui_LaunchTclMacro(PyObject *self, PyObject *args)
{
	char *macroName;

	if(!PyArg_ParseTuple(args, "s", &macroName) || !CTcl_Interp::IsAvail())
		return NULL;

	std::vector<ENTNODE>::const_iterator i;
	TclBrowserReset();
	for(i = gUICache.begin(); i != gUICache.end(); ++i)
	{
		EntnodeData *data = *i;
		TclBrowserAppend(((*data)[EntnodeData::kPath]).c_str(), data);
	}

	CTcl_Interp interp;
	string path = macroName;
	CTcl_Interp::Unixfy(path);
	interp.DoScriptVar("source \"%s\"", path.c_str());

	return PyContainer::Void();
}

class CBuffer
{
public:
	CBuffer() : m_curSize(0)
	{
	}

	void append(const char *ptr, size_t s)
	{
		if((s + m_curSize) > m_buf.size())
		{
			size_t step = s > 1024 ? s : 1024;
			m_buf.AdjustSize(m_buf.size() + step);
		}
#ifdef WIN32
		char *tmp = (char *)m_buf + m_curSize;
		size_t incs = 0;
		while(s--)
		{
			char c = *ptr++;
			if(c == '\r')
				continue;
			*tmp++ = c;
			incs++;
		}
		m_curSize += incs;
#elif defined(TARGET_OS_MAC)
		char *tmp = (char *)m_buf + m_curSize;
		size_t cnt = s;
		while(cnt--)
		{
			char c = *ptr++;
			if(c == '\r')
				c = '\n';
			*tmp++ = c;
		}
		m_curSize += s;
#else
		memcpy((char *)m_buf + m_curSize, ptr, s * sizeof(char));
		m_curSize += s;
#endif
	}

	inline size_t size() const { return m_curSize; }
	inline operator char *(void) const { return m_buf; }

protected:
	size_t m_curSize;
	CStaticAllocT<char> m_buf;
};

class CPyCvsConsole : public CCvsConsole
{
public:
	CPyCvsConsole()	{}
	virtual ~CPyCvsConsole() {}
	
	virtual long cvs_out(const char *txt, long len)
	{
		m_out.append(txt, len);
		return len;
	}
	
	virtual long cvs_err(const char *txt, long len)
	{
		m_err.append(txt, len);
		return len;
	}

	CBuffer m_out;
	CBuffer m_err;
};

static PyObject *cvsgui_PromptMessage(PyObject *self, PyObject *args)
{
	char *msg, *title, *ok, *cncl;

	if( !PyArg_ParseTuple(args, "ssss", &msg, &title, &ok, &cncl) )
		return NULL;

	bool res;

	TRY
	{
		CvsAlert cvsAlert(kCvsAlertQuestionIcon, msg, NULL, ok, cncl);
		cvsAlert.SetAlertTitle(title);

		res = (cvsAlert.ShowAlert() == kCvsAlertOKButton);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	if( res )
	{
		return PyContainer::True();
	}
	
	return PyContainer::False();
}

static PyObject *cvsgui_PromptEditMessage(PyObject *self, PyObject *args)
{
	char *msg, *title, *ok, *cncl, *edit;

	if(!PyArg_ParseTuple(args, "sssss", &msg, &edit, &title, &ok, &cncl))
		return NULL;

	bool res;
	string ed;

	TRY
	{
		ed = edit;

#ifdef WIN32
		CvsPrompt cvsPrompt(kCvsAlertQuestionIcon, msg, NULL, ok, cncl);
		cvsPrompt.SetAlertTitle(title);
		cvsPrompt.SetValue(edit);

		res = (cvsPrompt.ShowAlert() == kCvsAlertOKButton);

		if( res )
		{
			ed = cvsPrompt.GetValue().c_str();
		}
#else
		res = PromptEditMessage(msg, ed, title, ok, cncl);
#endif
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return Py_BuildValue("(is)", res ? 1 : 0, ed.empty() ? "" : ed.c_str());
}

static PyObject *cvsgui_CvsAlert(PyObject *self, PyObject *args)
{
	CvsAlerData cvsAlerData;
	
	if( !PyArg_ParseTuple(args, "ssssssss", &cvsAlerData.m_icon, &cvsAlerData.m_message, &cvsAlerData.m_messageDetails, &cvsAlerData.m_okButtonTitle, &cvsAlerData.m_cancelButtonTitle, &cvsAlerData.m_otherButtonTitle, &cvsAlerData.m_defaultButton, &cvsAlerData.m_alertTitle) )
		return NULL;
	
	CvsAlerResult cvsAlerResult;

	TRY
	{
		cvsAlerResult = ShowAlert(cvsAlerData);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL
		
	return Py_BuildValue("s", cvsAlerResult.m_strRes.c_str());
}

static PyObject *cvsgui_CvsPrompt(PyObject *self, PyObject *args)
{
	CvsAlerData cvsAlerData(true);
	
	if( !PyArg_ParseTuple(args, "sssssssss", 
		&cvsAlerData.m_value, &cvsAlerData.m_icon, &cvsAlerData.m_message, &cvsAlerData.m_messageDetails, 
		&cvsAlerData.m_okButtonTitle, &cvsAlerData.m_cancelButtonTitle, &cvsAlerData.m_otherButtonTitle, 
		&cvsAlerData.m_defaultButton, &cvsAlerData.m_alertTitle) )
	{
		return NULL;
	}
	
	CvsAlerResult cvsAlerResult;

	TRY
	{
		cvsAlerResult = ShowAlert(cvsAlerData);
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return Py_BuildValue("(ss)", cvsAlerResult.m_strRes.c_str(), cvsAlerResult.m_strValue.c_str());
}

static PyObject *cvsgui_LaunchBrowser(PyObject *self, PyObject *args)
{
	char *url;

	if(!PyArg_ParseTuple(args, "s", &url))
		return NULL;

#ifdef WIN32
	ShellExecute(NULL, "open", url, /*postData*/0L, NULL, SW_SHOWNORMAL);
#endif

	return PyContainer::Void();
}

static PyObject *cvsgui_Exit(PyObject *self, PyObject *args)
{
	int exitCode;
	
	if( !PyArg_ParseTuple(args, "i", &exitCode) )
		return NULL;

#ifdef WIN32
	PostQuitMessage(exitCode);
#else
	exit(exitCode);
#endif

	return PyContainer::Void();
}

/*!
	Write node as XML into a file
	\param out File to write to
	\param node Node to write
*/
static void outputXML(FILE* out, CLogNode* node)
{
	switch( node->GetType() )
	{
	case kNodeHeader :
		{
			fputs("<RCS>\n", out);
			
			const CRcsFile rcs = **(CLogNodeHeader*)node;
			fprintf(out, "<RCSFILE>%s</RCSFILE>\n",				FormatXmlString(rcs.RcsFile().c_str()).c_str());
			fprintf(out, "<WORKINGFILE>%s</WORKINGFILE>\n",		FormatXmlString(rcs.WorkingFile().c_str()).c_str());
			fprintf(out, "<HEADREV>%s</HEADREV>\n",				FormatXmlString(rcs.HeadRev().c_str()).c_str());
			fprintf(out, "<BRANCHREV>%s</BRANCHREV>\n",			FormatXmlString(rcs.BranchRev().c_str()).c_str());
			fprintf(out, "<KEYWORDSUBST>%s</KEYWORDSUBST>\n",	FormatXmlString(rcs.KeywordSubst().c_str()).c_str());
			fprintf(out, "<DESCRIPTION>%s</DESCRIPTION>\n",		FormatXmlString(rcs.DescLog().c_str()).c_str());
			fprintf(out, "<SELREVISIONS>%d</SELREVISIONS>\n",	rcs.SelRevisions());
			fprintf(out, "<TOTREVISIONS>%d</TOTREVISIONS>\n",	rcs.TotRevisions());
			fprintf(out, "<LOCKSTRICT>%d</LOCKSTRICT>\n",		rcs.LockStrict() ? 1 : 0);
			
			fputs("<ACCESSLIST>\n", out);
			const std::vector<std::string>& ac = rcs.AccessList();
			std::vector<std::string>::const_iterator i;
			for(i = ac.begin(); i != ac.end(); ++i)
			{
				fprintf(out, "<ACCESS>%s</ACCESS>\n", FormatXmlString((*i).c_str()).c_str());
			}
			fputs("</ACCESSLIST>\n", out);
			
			fputs("<SYMBOLICLIST>\n", out);
			const std::vector<CRevNumber>& sy = rcs.SymbolicList();
			std::vector<CRevNumber>::const_iterator j;
			for(j = sy.begin(); j != sy.end(); ++j)
			{
				fprintf(out, "<SYMBOLIC>%s</SYMBOLIC>\n", FormatXmlString((*j).c_str()).c_str());
			}
			fputs("</SYMBOLICLIST>\n", out);
			
			fputs("<LOCKLIST>\n", out);
			const std::vector<CRevNumber>& lo = rcs.LocksList();
			for(j = lo.begin(); j != lo.end(); ++j)
			{
				fprintf(out, "<LOCK>%s</LOCK>\n", FormatXmlString((*j).c_str()).c_str());
			}
			fputs("</LOCKLIST>\n", out);
			break;
		}
	case kNodeRev :
		{
			fputs("<REVISION>\n", out);
			
			const CRevFile& rev = **(CLogNodeRev*)node;
			fprintf(out, "<NUMBER>%s</NUMBER>\n", FormatXmlString(rev.RevNum().c_str()).c_str());
			const struct tm& tm = rev.RevTime();
			fprintf(out, "<DATE>%d/%d/%d %d:%d:%d</DATE>\n", 
				(tm.tm_year + 1900), tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fprintf(out, "<AUTHOR>%s</AUTHOR>\n", FormatXmlString(rev.Author().c_str()).c_str());
			fprintf(out, "<STATE>%s</STATE>\n", FormatXmlString(rev.State().c_str()).c_str());
			fprintf(out, "<CHANGES>+%d%d</CHANGES>\n", rev.ChgPos(), rev.ChgNeg());
			fprintf(out, "<DESCRIPTION>%s</DESCRIPTION>\n", FormatXmlString(rev.DescLog().c_str()).c_str());
			
			fputs("<BRANCHLIST>\n", out);
			const std::vector<CRevNumber>& sy = rev.BranchesList();
			std::vector<CRevNumber>::const_iterator j;
			for(j = sy.begin(); j != sy.end(); ++j)
			{
				fprintf(out, "<REVBRANCH>%s</REVBRANCH>\n", FormatXmlString((*j).c_str()).c_str());
			}
			fputs("</BRANCHLIST>\n", out);
			break;
		}
	case kNodeBranch :
		{
			fputs("<BRANCH>\n", out);
			
			const std::string& branch = **(CLogNodeBranch*)node;
			fprintf(out, "<BRANCHNAME>%s</BRANCHNAME>\n", FormatXmlString(branch.c_str()).c_str());
			break;
		}
	case kNodeTag :
		{
			fputs("<TAG>\n", out);
			
			const std::string& tag = **(CLogNodeTag*)node;
			fprintf(out, "<TAGNAME>%s</TAGNAME>\n", FormatXmlString(tag.c_str()).c_str());
			break;
		}
	}

	if( node->Childs().size() > 0 )
	{
		fputs("<REVISIONS>\n", out);
		std::vector<CLogNode*>::const_iterator i;
		for(i = node->Childs().begin(); i != node->Childs().end(); ++i)
		{
			outputXML(out, *i);
		}
		fputs("</REVISIONS>\n", out);
	}

	switch( node->GetType() )
	{
	case kNodeHeader :
		fputs("</RCS>\n", out);
		break;
	case kNodeRev :
		fputs("</REVISION>\n", out);
		break;
	case kNodeBranch :
		fputs("</BRANCH>\n", out);
		break;
	case kNodeTag :
		fputs("</TAG>\n", out);
		break;
	}

	if( node->Next() != 0L )
	{
		outputXML(out, node->Next());
	}
}

static PyObject *cvsgui_GetHistoryAsXML(PyObject *self, PyObject *args)
{
	PyObject *logfile, *xmlfile;

	if(!PyArg_ParseTuple(args, "O!O!", GetPyFile_Type(), &logfile,
		GetPyFile_Type(), &xmlfile))
			return NULL;

	TRY
	{
		std::vector<CRcsFile> & allRcs = CvsLogParse(PyFile_AsFile(logfile));

		std::vector<CRcsFile>::iterator i;
		FILE *xml = PyFile_AsFile(xmlfile);
		fputs("<ROOT>\n", xml);
		for(i = allRcs.begin(); i != allRcs.end(); ++i)
		{
			std::auto_ptr<CLogNode> node(CvsLogGraph(*i));
			outputXML(xml, node.get());
		}
		fputs("</ROOT>\n", xml);

		CvsLogReset();
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return PyContainer::Void();
}

static PyObject *cvsgui_RunCvs(PyObject *self, PyObject *args)
{
	CvsArgs arg;
	PyObject *cargs;
	int exitOnError = 1;
	int exitc;
	CPyCvsConsole console;

	if(!PyArg_ParseTuple(args, "O!|i", GetPyTuple_Type(), &cargs, &exitOnError))
		return NULL;

	TRY
	{
		int argc = PyTuple_Size(cargs);
		for(int i = 0; i < argc; i++)
		{
			PyObject *o = PyTuple_GET_ITEM(cargs, i);
			if(!PyString_Check(o))
			{
				PyErr_SetString(gCvsguiErr, "Wrong argument type, should be string");
				return NULL;
			}

			arg.add(PyString_AsString(o));
		}

		exitc = launchCVS(0L, arg.Argc(), arg.Argv(), &console);

		if( exitOnError && exitc != 0 )
		{
			string str(console.m_err, console.m_err.size());
			str += "cvs returned an non-zero error code (";
			str += exitc;
			str += ")\n";
			PyErr_SetString(gCvsguiErr, str.c_str());
			
			return NULL;
		}
	}
	CATCH_ALL(e)
	{
		PyHandleCvsguiError(e);
		return NULL;
	}
	END_CATCH_ALL

	return Py_BuildValue("(is#s#)", exitc, (char *)console.m_out, console.m_out.size(),
		(char *)console.m_err, console.m_err.size());
}

static PyMethodDef cvsgui_methods[] =
{
	{"write", (PyCFunction)cvsgui_write, METH_VARARGS, ""},
	{"GetMainMenuWidget", (PyCFunction)cvsgui_GetMainMenuWidget, METH_VARARGS, ""},
	{"GetTopMacroSelectionMenu", (PyCFunction)cvsgui_GetTopMacroSelectionMenu, METH_VARARGS, ""},
	{"GetTopMacroAdminMenu", (PyCFunction)cvsgui_GetTopMacroAdminMenu, METH_VARARGS, ""},
	{"GetSelection", (PyCFunction)cvsgui_GetSelection, METH_VARARGS, ""},
	{"GetCvsEntries", (PyCFunction)cvsgui_GetCvsEntries, METH_VARARGS, ""},
#if defined(WIN32)
	{"GetFileModTime", (PyCFunction)cvsgui_GetFileModTime, METH_VARARGS, ""},
#endif
	{"LaunchTclMacro", (PyCFunction)cvsgui_LaunchTclMacro, METH_VARARGS, ""},
	{"RunCvs", (PyCFunction)cvsgui_RunCvs, METH_VARARGS, ""},
	{"PromptMessage", (PyCFunction)cvsgui_PromptMessage, METH_VARARGS, ""},
	{"PromptEditMessage", (PyCFunction)cvsgui_PromptEditMessage, METH_VARARGS, ""},
	{"LaunchBrowser", (PyCFunction)cvsgui_LaunchBrowser, METH_VARARGS, ""},
	{"GetHistoryAsXML", (PyCFunction)cvsgui_GetHistoryAsXML, METH_VARARGS, ""},
	{"CvsAlert", (PyCFunction)cvsgui_CvsAlert, METH_VARARGS, ""},
	{"CvsPrompt", (PyCFunction)cvsgui_CvsPrompt, METH_VARARGS, ""},
	{"Exit", (PyCFunction)cvsgui_Exit, METH_VARARGS, ""},
	{NULL, NULL}
};

static std::vector<PyMethodDef> gAllMethods;

static void InitializeInterp(PyThreadState *interp)
{
	CPyThreadState lock(interp);

	// first time, create the method table for cvsgui module
	if(gAllMethods.empty())
	{
		PyMethodDef *tmp = cvsgui_methods;
		while(tmp->ml_name)
		{
			gAllMethods.push_back(*tmp);
			tmp++;
		}

		PyRegistry::RegisterAllObjects(gAllMethods);
	}

	// this module contains the glue code functions
	PyObject *m = Py_InitModule4("_cvsgui", &gAllMethods[0],
		"The cvsgui glue functions", (PyObject*)NULL,PYTHON_API_VERSION);
	
	if(m == NULL)
	{
		cvs_err("Could not create the Python cvsgui module !\n");
		return;
	}

	// this is the exception we raised when the error comes from us
	PyObject *d = PyModule_GetDict(m);
	if(gCvsguiErr == NULL)
		gCvsguiErr = PyErr_NewException("_cvsgui.error", NULL, NULL);
	if (gCvsguiErr != NULL)
		PyDict_SetItemString(d, "error", gCvsguiErr);

	PyRegistry::CallInitValues(d);

	// Ideally we need to use Jonathan's mechanism
	PyObject *ver = PyString_FromString("1.3");
	if (ver != NULL) {
		PyDict_SetItemString(d, "CVSGUI_VERSION", ver);
		Py_DECREF(ver);
	}

#ifdef WIN32
	char *script = NULL;

	HINSTANCE hInst = ::AfxGetInstanceHandle();
	HRSRC rsrc = FindResource(hInst, "PYSHELL", "PYSCRIPT");

	if(rsrc)
	{
		HGLOBAL h = LoadResource(hInst, rsrc);
		if(h)
		{
			DWORD size = SizeofResource(hInst, rsrc);
			script = (char *)malloc(size + 1);
			char *ptr = (char *)LockResource(h);

			if(script)
			{
				char *tmp = script;
				while(size--)
				{
					char c = *ptr++;
					if(c == '\r')
						continue;
					*tmp++ = c;
				}
				*tmp = '\0';
			}

			FreeResource(h);
		}
	}

#endif // WIN32
#ifdef qUnix		// Unix
	std::string 		filename;
	std::string 		path;
	MacrosGetLoc(path);
	
	filename =  path.c_str();
	filename += "/cvsgui/Shell.py";
#endif
	
#if qMacCvsPP
  char *script = NULL;

  CFURLRef    scriptURL(CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("mfcshell"), CFSTR("py"), NULL));
  if ( scriptURL )
  {
    CFDataRef   scriptData(NULL);
    if ( CFURLCreateDataAndPropertiesFromResource(NULL, scriptURL, &scriptData, NULL, NULL, NULL) )
    {
      CFIndex   size = CFDataGetLength(scriptData);
      script = (char *)malloc(size + 1);
      memcpy(script, CFDataGetBytePtr(scriptData), size);
      script[size] = '\0';
      CFRelease(scriptData);
    }
    CFRelease(scriptURL);
  }
#endif // qMacCvsPP

	// augment the library path of python with our library
	string ourLibPath;
	PythonLibGetLoc(ourLibPath);

#if 0
	// this is not the sys.path, this is the import path
	std::string  libPath = Py_GetPath();
#else
	// so instead we call sys.path
	string libPath;
	PyObject *sysm = PyImport_ImportModule("sys");
	if(sysm)
	{
		PyObject *d = PyModule_GetDict(sysm);
		PyObject *p = PyDict_GetItemString(d, "path");
		if(p && PyList_Check(p))
		{
			for(int i = 0; i < PyList_Size(p); i++)
			{
				PyObject *e = PyList_GetItem(p, i);
				if(e && PyString_Check(e))
				{
					if( libPath.empty() )
					{
						libPath = PyString_AsString(e);
					}
					else
					{
						libPath += DELIM;
						libPath += PyString_AsString(e);
					}
				}
			}
		}
		Py_DECREF(sysm);
	}
	if(libPath.empty())
		libPath = Py_GetPath();
#endif

	if(!ourLibPath.empty())
	{
		libPath += DELIM;
		libPath += ourLibPath.c_str();
		PySys_SetPath((char*)libPath.c_str());
	}

#ifdef qUnix
	FILE *f = fopen(filename.c_str(), "r");
	if(f)
	{
		PyRun_SimpleFile(f, (const char *)filename.c_str());
		fclose(f);
	}
	else
	{
		cvs_err("Could not initialize properly the Python interpreter !\n");
	}
#else
	// this is the script to setup stdout/stderr and reroute it
	// to the console window
	if(script)
	{
		PyRun_SimpleString(script);

		free(script);
	}
	else
	{
		cvs_err("Could not initialize properly the Python interpreter !\n");
	}
#endif
}
#endif // USE_PYTHON

CPython_Interp::CPython_Interp(bool mainInterp)
{
	fInterp = 0L;

#if USE_PYTHON
	if( !CPython_Interp::IsAvail() )
	{
		return;
	}
	
	if( mainInterp )
	{
		fInterp = m_mainInterpreter;
		gCvsPrefs.SetTclFileRunning(true);
		return;
	}

	{
		CPyThreadState lock(0L);

		fInterp = Py_NewInterpreter();
		if( fInterp == NULL )
			return;
	}

	InitializeInterp(fInterp);
	gCvsPrefs.SetTclFileRunning(true);
#endif
}

CPython_Interp::~CPython_Interp()
{
#if USE_PYTHON
	if( fInterp != 0L )
	{
		gCvsPrefs.SetTclFileRunning(false);

		if( fInterp != m_mainInterpreter )
		{
			CPyThreadState lock(fInterp);

			Py_EndInterpreter(fInterp);
		}
	}
#endif
}

/*!
	Check whether Python is available and load if not
	\param forceReload Force reload of Python library
	\return true if Python is available, false otherwise
*/
bool CPython_Interp::IsAvail(const bool forceReload /*= false*/)
{
#if USE_PYTHON
#	if (defined(WIN32) && !qCvsDebug) || TARGET_RT_MAC_MACHO
	static bool firstTime = true;
	static bool pyAvail = false;

	if( firstTime || forceReload )
	{
#	if defined(WIN32) || TARGET_RT_MAC_MACHO
		static CompatConnectID connID = 0L;

		if( connID )
		{
			if( CompatCloseLibrary(&connID) )
			{
				connID = 0L;
				pyAvail = false;
			}
		}
#	endif

#	ifdef WIN32
		if( !pyAvail )
		{
			pyAvail = (gCvsPrefs.PythonLibrary() && CompatLoadLibrary(&connID, gCvsPrefs.PythonLibrary())) || 
				CompatLoadLibrary(&connID, "python29.dll") || 
				CompatLoadLibrary(&connID, "python28.dll") || 
				CompatLoadLibrary(&connID, "python27.dll") || 
				CompatLoadLibrary(&connID, "python26.dll") || 
				CompatLoadLibrary(&connID, "python25.dll") || 
				CompatLoadLibrary(&connID, "python24.dll") || 
				CompatLoadLibrary(&connID, "python23.dll") || 
				CompatLoadLibrary(&connID, "python22.dll") || 
				CompatLoadLibrary(&connID, "python21.dll");
		}
#	endif
	
#	if TARGET_RT_MAC_MACHO
		pyAvail = CompatLoadLibrary(&connID, "/Library/Frameworks/Python.framework") ||
			CompatLoadLibrary(&connID, "/System/Library/Frameworks/Python.framework");
#	endif

#	if defined(WIN32) || TARGET_RT_MAC_MACHO

#	define LOAD_CODEFRAG2(name,sign) \
		if( pyAvail ) \
			pyAvail = (*(void **)&name##_glue = \
				_CompatCallLibrary(connID, sign)) != 0L

#	define LOAD_CODEFRAG(name) \
		if( pyAvail ) \
			pyAvail = (*(void **)&name##_glue = \
				_CompatCallLibrary(connID, #name)) != 0L

		LOAD_CODEFRAG(PyThreadState_Swap);
		LOAD_CODEFRAG(PyDict_SetItemString);
		LOAD_CODEFRAG(PyErr_Clear);
		LOAD_CODEFRAG(PyInt_FromLong);
		LOAD_CODEFRAG(PyErr_SetString);
		LOAD_CODEFRAG(PyArg_ParseTuple);
		LOAD_CODEFRAG(PyTuple_New);
		LOAD_CODEFRAG(Py_BuildValue);
		LOAD_CODEFRAG(PyFile_AsFile);
		LOAD_CODEFRAG(PyString_AsString);
		LOAD_CODEFRAG(PyTuple_Size);
		LOAD_CODEFRAG(PyList_Size);
		LOAD_CODEFRAG(PyRun_SimpleString);
		LOAD_CODEFRAG(PyRun_SimpleFile);
		LOAD_CODEFRAG(Py_SetProgramName);
		LOAD_CODEFRAG(Py_Initialize);
		LOAD_CODEFRAG(Py_Finalize);
		LOAD_CODEFRAG(Py_IsInitialized);
		LOAD_CODEFRAG(Py_NewInterpreter);
		LOAD_CODEFRAG(Py_EndInterpreter);
		LOAD_CODEFRAG(PyString_FromString);
		LOAD_CODEFRAG(PyErr_NewException);
		LOAD_CODEFRAG(PyModule_GetDict);
#if _WIN64
		LOAD_CODEFRAG2(Py_InitModule4,"Py_InitModule4_64");
#else
		LOAD_CODEFRAG(Py_InitModule4);
#endif
		LOAD_CODEFRAG(PyEval_InitThreads);
		LOAD_CODEFRAG(Py_FindMethod);
		LOAD_CODEFRAG(PyList_GetItem);
		LOAD_CODEFRAG(PyType_IsSubtype);
		LOAD_CODEFRAG(PyDict_GetItemString);
		LOAD_CODEFRAG(PyImport_ImportModule);
		LOAD_CODEFRAG(_PyObject_New);
#	if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 3
		LOAD_CODEFRAG(_PyObject_Del);
#	else
		LOAD_CODEFRAG(PyObject_Free);
#	endif
		LOAD_CODEFRAG(PySys_SetPath);
		LOAD_CODEFRAG(Py_GetPath);

		LOAD_CODEFRAG(PyType_Type);
		LOAD_CODEFRAG(PyFile_Type);
		LOAD_CODEFRAG(PyString_Type);
		LOAD_CODEFRAG(PyTuple_Type);
		LOAD_CODEFRAG(PyList_Type);
		LOAD_CODEFRAG(PyInt_Type);
		LOAD_CODEFRAG(_Py_NoneStruct);
		LOAD_CODEFRAG(_Py_ZeroStruct);
		LOAD_CODEFRAG(_Py_TrueStruct);

#		ifdef WIN32
		if( !pyAvail )
		{
			WarnNoPython();
		}
#		endif
#	endif /* WIN32 */

		firstTime = false;
	}

	return pyAvail;
#	else
	return true;
#	endif
#else // !USE_PYTHON
	return false;
#endif // !USE_PYTHON
}

/*!
	Execute a Python script
	\return true on success, false otherwise
*/
int CPython_Interp::DoScript(const char *script)
{
#if USE_PYTHON
	if( fInterp == 0L )
		return -1;

	CPyThreadState lock(fInterp);

	PyRun_SimpleString((char*)script);

	return 0;
#else
	return -1;
#endif
}

/*!
	Execute a Python script
	\return true on success, false otherwise
*/
int CPython_Interp::DoScriptVar(const char* format, ...)
{
#if USE_PYTHON
	if(fInterp == 0L)
		return -1;

	va_list args;
	char script[1024] = {'\0'};

	va_start(args, format);
	vsprintf(script, format, args);
	va_end (args);
	
	return DoScript(script);
#else
	return -1;
#endif
}

/*!
	Execute a Python script
	\return true on success, false otherwise
*/
int CPython_Interp::DoFile(const char* file)
{
#if USE_PYTHON
	if( fInterp == 0L )
		return -1;

	CPyThreadState lock(fInterp);

	FILE* f = fopen(file, "r");
	if( f )
	{
		PyRun_SimpleFile(f, (char*)file);
		fclose(f);
	}

	return 0;
#else
	return -1;
#endif
}

void CPython_Interp::InitPythonSupport(const bool forceReload /*= false*/)
{
#if USE_PYTHON
	if( !CPython_Interp::IsAvail(forceReload) )
		return;

	char* programName; 

#ifdef WIN32
	programName = "wincvs";
#elif TARGET_RT_MAC_MACHO
	programName = "maccvsX";
#else
	programName = "gcvs";
#endif

	Py_SetProgramName(programName);
	Py_Initialize();
	PyEval_InitThreads();

	m_mainInterpreter = PyThreadState_Swap(NULL);

	InitializeInterp(m_mainInterpreter);
#endif
}

void CPython_Interp::ClosePythonSupport(void)
{
#if USE_PYTHON
	if( !CPython_Interp::IsAvail() )
		return;

	PyThreadState_Swap(m_mainInterpreter);
//	PyEval_ReleaseLock();

	PyInvalUICache();
	
	Py_Finalize();

	m_mainInterpreter = 0L;
#endif
}

/*!
	Reload the available macros
*/
void PyMacrosReloadAll(void)
{
	// Get the macro folder
	string path;
	MacrosGetLoc(path);

#ifdef WIN32
	CString newpath(path.c_str());
	newpath.Replace("\\", "\\\\");
	path = newpath;
#endif

	CPython_Interp interp;
	interp.DoScriptVar("from cvsgui.MacroRegistry import *; "
		"gMacroRegistry.LoadMacros(\"%s\")", (char*)path.c_str());
}

/*!
	Update a menu state (call python)
	\param ucmdui Command UI update object
*/
void PyDoCmdUI(UCmdUI* ucmdui)
{
	CPython_Interp interp;
	interp.DoScriptVar("from cvsgui.MacroRegistry import *; "
		"from cvsgui.CmdUI import *; "
#if defined(SIZEOF_VOID_P) && (SIZEOF_VOID_P == 8)
		"gMacroRegistry.DoCmdUI(CmdUI(%llu), %d)", (long long unsigned)ucmdui, ucmdui->m_nID);
#else
		"gMacroRegistry.DoCmdUI(CmdUI(%d), %d)", (long)ucmdui, ucmdui->m_nID);
#endif
}

/*!
	Call python because a command has been triggered (by the menu or an icon)
	\param cmd Command UI update object
*/
void PyDoPython(int cmd)
{
	CPython_Interp interp;
	interp.DoScriptVar("from cvsgui.MacroRegistry import *; "
		"gMacroRegistry.DoCmd(%d)", cmd);
}

/*!
	Check whether the cache is valid
	\return true if cache is invalid, false otherwise
*/
bool PyIsUICacheValid()
{
#if USE_PYTHON
	return gUICacheValid;
#else
	return false;
#endif
}

/*!
	Mark the cache as valid
*/
void PyValidUICache()
{
#if USE_PYTHON
	gUICacheValid = true;
#endif
}

/*!
	Erase and mark the cache as invalid
*/
void PyInvalUICache()
{
#if USE_PYTHON
	gUICache.erase(gUICache.begin(), gUICache.end());
	gUICacheValid = false;
	Py_XDECREF(gUICacheObj);
	gUICacheObj = 0L;
#endif
}

/*!
	Add node data to the cache
	\param data Node data
*/
void PyAppendCache(EntnodeData* data)
{
#if USE_PYTHON
	Py_XDECREF(gUICacheObj);
	gUICacheObj = 0L;
	gUICache.push_back(ENTNODE(data));
	gUICacheValid = true;
#endif
}
