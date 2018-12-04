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

#include "stdafx.h"

#if qGTK
#	include <gtk/gtk.h>
#	include <gdk/gdkkeysyms.h>
#endif

#include "UCvsApp.h"
#include "UCvsConsole.h"
#include "UCvsCommands.h"
#include "uconsole.h"
#include "CvsPrefs.h"
#include "TclGlue.h"
#include "PythonGlue.h"
#include "CvsPrefs.h"

UIMPLEMENT_DYNAMIC(UCvsConsole, UWidget)

UBEGIN_MESSAGE_MAP(UCvsConsole, UWidget)
	ON_UUPDATECMD(cmdCLEAR_ALL, UCvsConsole::OnCmdUIEraseAll)
	ON_UCOMMAND(cmdCLEAR_ALL, UCvsConsole::OnEraseAll)
	ON_KEYDOWN(UCvsConsole)
	ON_UDESTROY(UCvsConsole)
	ON_UCREATE(UCvsConsole)
UEND_MESSAGE_MAP()

UCvsConsole::UCvsConsole() : UWidget(kUMainConsoleID), m_buf(0L), m_numbuf(0)
{
	out.col = UCOLOR(0, 0, 0);
	out.bold = false;
	out.italic = false;
	out.underline = false;
}

UCvsConsole::~UCvsConsole()
{
	m_numbuf = 0;
	if(m_buf != 0L)
	{
		free(m_buf);
		m_buf = 0L;
	}
}

void UCvsConsole::FeedThisLine(const char *buf, size_t numbuf, bool isStderr)
{
	void *wid = GetWidget(kUMainWidget);
	if(wid == 0L)
	{
		UAppConsole("The console is not here !\n");
		return;
	}

#if qGTK
	if(!GTK_IS_TEXT(wid))
	{
		g_warning("The pseudo-console is not here !\n");
		return;
	}

	GtkText *w = GTK_TEXT(wid);
	gtk_text_set_point(w, gtk_text_get_length(w));
#endif

	UCOLOR color(0, 0, 0);
	bool colorChanged = false;
	std::string  s(buf, numbuf);

	if(s.length() >= 5 && s[1] == ' ' &&
		(s[0] == 'M' || s[0] == 'C' || s[0] == 'U' ||
		s[0] == 'P' || s[0] == 'N' || s[0] == 'A' ||
		s[0] == 'R' || s[0] == 'I' || s[0] == '?' ||
		s[0] == '<' || s[0] == '>'))
	{
		colorChanged = true;
		switch(s[0])
		{
		default:
		case '?' :
		case 'I' :
			color = UCOLOR(0x80, 0x80, 0x00);
			break;
		case 'U' :
		case 'N' :
			color = UCOLOR(0x00, 0xA0, 0x00);
			break;
		case 'M' :
		case 'A' :
		case 'R' :
			color = UCOLOR(0xFF, 0x00, 0xFF);
			break;
		case 'C' :
		case '<' :
			color = UCOLOR(0xFF, 0x00, 0x00);
			break;
		case '>' :
		case 'P' :
			color = UCOLOR(0x00, 0x00, 0xFF);
			break;
		}
	}
	else if(isStderr)
	{
		colorChanged = true;
		color = UCOLOR(0xFF, 0x80, 0x00);
	}

#if qGTK
	GdkColormap *cmap;
	GdkColor col;
	cmap = gdk_colormap_get_system();
	col.red = ((unsigned int)color.r) << 8;
	col.green = ((unsigned int)color.g) << 8;
	col.blue = ((unsigned int)color.b) << 8;
	if(color.r != 0 || color.g != 0 || color.b != 0)
	{
		if (!gdk_color_alloc(cmap, &col)) {
			g_error("Couldn't allocate color\n");
		}
		gtk_text_insert(w, m_font, &col, NULL, s.c_str(), -1);
	}
	else
		gtk_text_insert(w, m_font, &GTK_WIDGET(w)->style->text[GTK_STATE_NORMAL], NULL, s.c_str(), -1);
#endif
}

void UCvsConsole::OutConsole(const char *txt, size_t len, bool isStderr)
{
	if(len == 0)
		return;

	if(m_buf == 0L)
	{
		m_buf = (char *)malloc((MAX_CHAR_BY_LINE + 3) * sizeof(char));
		if(m_buf == 0L)
			return;
	}

	size_t i = 0;
	char c = 0;
once_again:
	for(; i < len && m_numbuf < MAX_CHAR_BY_LINE; m_numbuf++)
	{
		c = txt[i++];
		if(c == 0x0a)
			break;
		m_buf[m_numbuf] = c;
	}

	if(c == 0x0a || m_numbuf == MAX_CHAR_BY_LINE)
	{
		m_buf[m_numbuf++] = 0x0a;
		m_buf[m_numbuf++] = 0x00;
	
		FeedThisLine(m_buf, m_numbuf, isStderr);
		m_numbuf = 0;
	}

	if(i != len)
		goto once_again;
}

void UCvsConsole::OnDestroy(void)
{
	delete this;
}

void UCvsConsole::OnKeyDown(int keyval)
{
	GtkWidget *widget = (GtkWidget*)GetWidget(kUMainWidget);

	if (keyval != GDK_Return && keyval != GDK_KP_Enter)
		return;

	if(!CTcl_Interp::IsAvail())
		return;
	
	if(!GTK_IS_TEXT(widget))
	{
		g_warning("The pseudo-console is not here !\n");
		return;
	}
	
	GtkText *text = GTK_TEXT(widget);
	const GtkEditable & w = text->editable;
	guint len = gtk_text_get_length(text);
	if(len == 0)
		return;
	
	guint nStartChar, nEndChar;
	
	if (!w.has_selection)
	{
		// get the end and the beginning of the line
		nStartChar = nEndChar = w.current_pos;

		if(nStartChar > 0 && GTK_TEXT_INDEX(text, nStartChar) == '\n')
			nStartChar--;
		while(nStartChar > 0 && GTK_TEXT_INDEX(text, nStartChar) != '\n')
			nStartChar--;
		if(GTK_TEXT_INDEX(text, nStartChar) == '\n')
			nStartChar++;
		while(nEndChar < len && GTK_TEXT_INDEX(text, nEndChar) != '\n')
			nEndChar++;
	}
	else
	{
		nStartChar = w.selection_start_pos;
		nEndChar = w.selection_end_pos;
	}
	
	guint long length = nEndChar - nStartChar;
	if(length > 0 && nStartChar < len)
	{			
		char *buf = (char *)malloc((length + 1) * sizeof(char));
		if(buf != 0L)
		{
			char *tmp = buf;
			while(nStartChar < nEndChar)
			{
				*tmp++ = GTK_TEXT_INDEX(text, nStartChar);
				nStartChar++;
			}
			*tmp++ = '\0';

			OutConsole("\n", 1);
			
			if( gCvsPrefs.GetUseShell() == SHELL_PYTHON && CPython_Interp::IsAvail() )
			{
				CPython_Interp interp;
				interp.DoScript(buf);
			}
			else if( gCvsPrefs.GetUseShell() == SHELL_TCL && CTcl_Interp::IsAvail() )
			{
				CTcl_Interp interp;
				interp.DoScript(buf);
			}

			free(buf);
		}
	}
	
	return;
}

void UCvsConsole::OnCreate(void)
{
	void *wid = GetWidget(kUMainWidget);
	if(wid == 0L)
	{
		UAppConsole("The console is not here !\n");
		return;
	}

#if qGTK
	setFont( gCvsPrefs.ConsoleFont() );
#endif
}

void UCvsConsole::setFont(const char* fontName) 
{
	m_font = gdk_font_load ( fontName );
}

void UCvsConsole::OutColor(const char *txt, long len)
{
	void *wid = GetWidget(kUMainWidget);
	if(wid == 0L)
	{
		UAppConsole("The console is not here !\n");
		return;
	}

#if qGTK
	if(!GTK_IS_TEXT(wid))
	{
		g_warning("The pseudo-console is not here !\n");
		return;
	}

	GtkText *w = GTK_TEXT(wid);
	gtk_text_set_point(w, gtk_text_get_length(w));

	GdkColormap *cmap;
	GdkColor col;
	GdkFont *font;
	std::string  fontName;
	
	cmap = gdk_colormap_get_system();
	col.red = ((unsigned int)out.col.r) << 8;
	col.green = ((unsigned int)out.col.g) << 8;
	col.blue = ((unsigned int)out.col.b) << 8;
	
	fontName = gCvsPrefs.ConsoleFont();
	
	if(out.bold)
		Replace(fontName, "-medium-", "-bold-");
	
	if(out.italic)
		Replace(fontName, "-r-", "-o-");
	
	/* underlined not supported yet
	* if(out.underline)
	*/
	
	font = gdk_font_load ( fontName.c_str() );
	if(font == NULL)
		font = m_font;
	
	if(out.col.r != 0 || out.col.g != 0 || out.col.b != 0)
	{
		if (!gdk_color_alloc(cmap, &col)) {
			g_error("Couldn't allocate color\n");
		}
		gtk_text_insert(w, font, &col, NULL, txt, len);
	}
	else
		gtk_text_insert(w, font, &GTK_WIDGET(w)->style->text[GTK_STATE_NORMAL], NULL, txt, len);
#endif
	
	// give some time to process idle work
	UCvsApp::gApp->PeekPumpAndIdle(true);
}

void UCvsConsole::OutColor(kConsoleColor color)
{
	switch(color)
	{
	case kBrown :
		out.col = UCOLOR(0x80, 0x80, 0x00);
		break;
	case kGreen :
		out.col = UCOLOR(0x00, 0xA0, 0x00);
		break;
	case kMagenta :
		out.col = UCOLOR(0xFF, 0x00, 0xFF);
		break;
	case kRed :
		out.col = UCOLOR(0xFF, 0x00, 0x00);
		break;
	case kBlue :
		out.col = UCOLOR(0x00, 0x00, 0xFF);
		break;
	case kBold :
		out.bold = true;
		break;
	case kItalic :
		out.italic = true;
		break;
	case kUnderline :
		out.underline = true;
		break;
	case kNormal:
		out.col = UCOLOR(0, 0, 0);
		out.bold = false;
		out.italic = false;
		out.underline = false;
		break;
	case kNL :
		break;
	}
}
void UCvsConsole::OnEraseAll(void)
{
	UEventSendMessage(GetWidID(), EV_SETTEXT, kUMainWidget, 0L);
}

void UCvsConsole::OnCmdUIEraseAll(UCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsAppEnabled());
}
