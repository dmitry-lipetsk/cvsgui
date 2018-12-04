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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

/*
 * 
 */

#ifndef UCVSCONSOLE_H
#define UCVSCONSOLE_H

#include "uwidget.h"
#include "AppConsole.h"
#include <gdk/gdktypes.h>

struct UCOLOR
{
	UCOLOR() {}
	UCOLOR(char _r, char _g, char _b) : r(_r), g(_g), b(_b) {}

	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class UCvsConsole : public UWidget
{
	UDECLARE_DYNAMIC(UCvsConsole)
public:
	UCvsConsole();
	virtual ~UCvsConsole();

	void setFont(const char *fontName);

	void OutConsole(const char *txt, size_t len, bool isStderr = false);

	void OutColor(const char *txt, long len);
	void OutColor(kConsoleColor color);
		// colorized output
protected:
	void FeedThisLine(const char *buf, size_t numbuf, bool isStderr);

	enum
	{
		MAX_CHAR_BY_LINE = 1024
	};
	
	char *m_buf;
	size_t m_numbuf;
	GdkFont *m_font;

	struct
	{
		UCOLOR col;
		bool bold, italic, underline;
	} out;

	ev_msg void OnDestroy();
	ev_msg void OnCreate();
	ev_msg void OnEraseAll();
	ev_msg void OnKeyDown(int keyval);
	
	ev_msg void OnCmdUIEraseAll(UCmdUI* pCmdUI);
	
	UDECLARE_MESSAGE_MAP()
};

#endif
