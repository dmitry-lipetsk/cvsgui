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
 * AppConsole.cpp --- output to the console
 */

#include "stdafx.h"

#ifdef qMacCvsPP
#	include "MacCvsApp.h"
#	include "LogWindow.h"
#endif /* qMacCvsPP */

#ifdef qUnix
#	include "UCvsConsole.h"
#	include "UCvsCommands.h"
#	include "uconsole.h"
#endif /* qUnix */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "AppConsole.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "wincvsView.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

void cvs_out(const char *format, ...)
{
	va_list args;
	char mess[1024] = {'\0'};

	va_start (args, format);
	vsprintf (mess, format, args);
	va_end (args);

#ifdef WIN32
	CWinApp* app = AfxGetApp();
	//ASSERT(app->IsKindOf(RUNTIME_CLASS(CWincvsApp)));
	CWincvsView *view = ((CWincvsApp *)app)->GetConsoleView();
	if(view == NULL)
		return;

	view->OutConsole(mess, strlen(mess));
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return;
	log->WriteCharsToConsole(mess, strlen(mess));
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole* console = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(console == 0L)
	{
		UAppConsole(mess);
		return;
	}

	console->OutConsole(mess, strlen(mess));
#endif /* qUnix */
}

void cvs_err(const char *format, ...)
{
	va_list args;
	char mess[1024] = {'\0'};

	va_start (args, format);
	vsprintf (mess, format, args);
	va_end (args);

#ifdef WIN32
	CWinApp* app = AfxGetApp();
	//ASSERT(app->IsKindOf(RUNTIME_CLASS(CWincvsApp)));
	CWincvsView *view = ((CWincvsApp *)app)->GetConsoleView();
	if(view == NULL)
		return;

	view->OutConsole(mess, strlen(mess), true);
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return;
	log->WriteCharsToConsole(mess, strlen(mess), true);
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole* console = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(console == 0L)
	{
		UAppConsole(mess);
		return;
	}

	console->OutConsole(mess, strlen(mess), true);
#endif /* qUnix */
}

void cvs_outstr(const char *str, size_t len)
{
#ifdef WIN32
	CWinApp* app = AfxGetApp();
	//ASSERT(app->IsKindOf(RUNTIME_CLASS(CWincvsApp)));
	CWincvsView *view = ((CWincvsApp *)app)->GetConsoleView();
	if(view == NULL)
		return;

	view->OutConsole(str, len);
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return;
	log->WriteCharsToConsole(str, len);
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole* console = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(console == 0L)
	{
		UAppConsole(str);
		return;
	}

	console->OutConsole(str, len);
#endif /* qUnix */
}

void cvs_errstr(const char *str, size_t len)
{
#ifdef WIN32
	CWinApp* app = AfxGetApp();
	//ASSERT(app->IsKindOf(RUNTIME_CLASS(CWincvsApp)));
	CWincvsView *view = ((CWincvsApp *)app)->GetConsoleView();
	if(view == NULL)
		return;

	view->OutConsole(str, len, true);
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return;
	log->WriteCharsToConsole(str, len, true);
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole* console = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(console == 0L)
	{
		UAppConsole(str);
		return;
	}

	console->OutConsole(str, len, true);
#endif /* qUnix */
}

#ifdef WIN32
CWincvsView * CColorConsole::GetView()
{
	CWinApp* app = AfxGetApp();
	CWincvsView *view = fView != 0L ? fView : ((CWincvsApp *)app)->GetConsoleView();
	if(view && view->m_hWnd == 0L)
		view = 0L;
	return view;
}
#endif /* WIN32 */

CColorConsole::CColorConsole()
{
#ifdef WIN32
	fView = 0L;
	CWincvsView *view = GetView();
	if(view == 0L)
		return;

	oldFormat = view->GetCurFormat();
#else /* !WIN32 */
	*this << kNormal;
#endif /* !WIN32 */
}

#ifdef WIN32
CColorConsole::CColorConsole(CWincvsView * aview)
{
	fView = aview;
	CWincvsView *view = GetView();
	if(view == 0L)
		return;

	oldFormat = view->GetCurFormat();
}
#endif /* WIN32 */

CColorConsole::~CColorConsole()
{
	*this << kNormal;
}

CColorConsole & CColorConsole::operator<<(char c)
{
#ifdef WIN32
	CWincvsView *view = GetView();
	if(view == 0L)
		return *this;

	view->OutColor(&c, 1);
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return *this;
	
	log->OutColor(&c, 1);
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole *log = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(log == 0L)
		return *this;
	
	log->OutColor(&c, 1);
#endif /* qUnix */

	return *this;
}

CColorConsole & CColorConsole::operator<<(kConsoleColor color)
{
#ifdef WIN32
	CWincvsView *view = GetView();
	if(view == 0L)
		return *this;

	if(color == kNL)
	{
		char tmp[2] = {0x0a};
		view->OutColor(tmp, 1);
	}
	else if(color == kNormal)
	{
		oldFormat.dwMask = CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFE_UNDERLINE;
		view->OutColor(oldFormat);
	}
	else
		view->OutColor(color);
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return *this;
	
	if(color == kNL)
	{
		char tmp[1] = {0x0d};
		log->OutColor(tmp, 1);
	}
	else
		log->OutColor(color);
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole *log = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(log == 0L)
		return *this;
	
	if(color == kNL)
	{
		char tmp[1] = {0x0a};
		log->OutColor(tmp, 1);
	}
	else
		log->OutColor(color);
#endif /* qUnix */

	return *this;
}

CColorConsole & CColorConsole::operator<<(const char *str)
{
#ifdef WIN32
	CWincvsView *view = GetView();
	if(view == 0L)
		return *this;

	view->OutColor(str, strlen(str));
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return *this;
	
	log->OutColor(str, strlen(str));
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole *log = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(log == 0L)
		return *this;
	
	log->OutColor(str, strlen(str));
#endif /* qUnix */

	return *this;
}

CColorConsole & CColorConsole::operator<<(int value)
{
#ifdef WIN32
	CWincvsView *view = GetView();
	if(view == 0L)
		return *this;

	char tmp[20] = {'\0'};
	sprintf(tmp, "%d", value);
	view->OutColor(tmp, strlen(tmp));
#endif /* WIN32 */
#ifdef qMacCvsPP
	CLogWindow *log = CMacCvsApp::gApp != 0L ? CMacCvsApp::gApp->GetLogWindow() : 0L;
	if(log == 0L)
		return *this;
	
	char tmp[20] = {'\0'};
	sprintf(tmp, "%d", value);
	log->OutColor(tmp, strlen(tmp));
#endif /* qMacCvsPP */
#ifdef qUnix
	UCvsConsole *log = (UCvsConsole *)::UEventQueryWidget(kUMainConsoleID);
	if(log == 0L)
		return *this;
	
	char tmp[20] = {'\0'};
	sprintf(tmp, "%d", value);
	log->OutColor(tmp, strlen(tmp));
#endif /* qUnix */

	return *this;
}

