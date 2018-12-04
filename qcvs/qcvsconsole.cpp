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
 * Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1998
 */

#include "qcvsapp.h"
#include "qcvsconsole.h"
#include "CPStr.h"
#include "TclGlue.h"

#include <qkeycode.h>

#define MAX_CHAR_BY_LINE 1024

static const QColor c_black(0x00, 0x00, 0x00);
static const QColor c_green(0x00, 0xA0, 0x00);
static const QColor c_violet(0xFF, 0x00, 0xFF);
static const QColor c_red(0xFF, 0x00, 0x00);
static const QColor c_blue(0x00, 0x00, 0xFF);
static const QColor c_yellow(0x80, 0x80, 0x00);
static const QColor c_orange(0xA0, 0x40, 0x00);

static const unsigned char i_black = 1;
static const unsigned char i_green = 2;
static const unsigned char i_violet = 3;
static const unsigned char i_red = 4;
static const unsigned char i_blue = 5;
static const unsigned char i_yellow = 6;
static const unsigned char i_orange = 7;

QCvsConsole::QCvsConsole(QWidget *parent=0, const char *name=0) : QMultiLineEdit(parent, name)
{
	QCvsApp::qApp->SetConsole(this);
}

QCvsConsole::~QCvsConsole()
{
}

void QCvsConsole::FeedThisLine(const char *buffer, unsigned int n, bool isStdErr)
{
	if(!atEnd())
		setCursorPosition(INT_MAX, INT_MAX);

	unsigned char c = 0;
	
	if(n >= 5 && buffer[1] == ' ' &&
		(buffer[0] == 'M' || buffer[0] == 'C' || buffer[0] == 'U' ||
		buffer[0] == 'P' || buffer[0] == 'N' || buffer[0] == 'A' || buffer[0] == 'R' ||
		buffer[0] == 'I' || buffer[0] == '?' || buffer[0] == '>' || buffer[0] == '<'))
	{
		switch(buffer[0])
		{
		case 'I' :
		case '?' :
			c = i_yellow;
			break;
		case 'U' :
		case 'N' :
			c = i_green;
			break;
		case 'A' :
		case 'R' :
		case 'M' :
			c = i_violet;
			break;
		case 'C' :
		case '<' :
			c = i_red;
			break;
		case 'P' :
		case '>' :
			c = i_blue;
			break;
		}
	}
	else if(isStdErr)
	{
		c = i_orange;
	}

	int line, col;
	getCursorPosition(&line, &col);

	if(c != 0)
		SetColor(line, c);

	insertAt(buffer, line, col);
}

void QCvsConsole::OutConsole(const char *buffer, unsigned int n, bool isStderr)
{
	if(n == 0)
		return;
	
	static CStaticAllocT<char> sbuf;
	static size_t snumbuf = 0;
	if(sbuf.size() == 0)
	{
		sbuf.AdjustSize(MAX_CHAR_BY_LINE);
	}

	size_t i = 0;
	char c = 0;
once_again:
	for(; i < n && snumbuf < MAX_CHAR_BY_LINE; snumbuf++)
	{
		c = buffer[i++];
		if(c == 0x0a)
			break;
		sbuf[snumbuf] = c;
	}

	if(c == 0x0a || snumbuf == MAX_CHAR_BY_LINE)
	{
		sbuf[snumbuf++] = 0x0a;
		sbuf[snumbuf++] = 0x00;
	
		FeedThisLine(sbuf, snumbuf - 1, isStderr);
		snumbuf = 0;
	}

	if(i != n)
		goto once_again;
}

void QCvsConsole::SetColor(int line, unsigned char color)
{
	size_t oldsize = colors.size();
	if(line > (int)oldsize)
	{
		size_t newsize = QMAX(oldsize + 1000, (size_t)line);
		colors.AdjustSize(newsize);
		memset((unsigned char *)colors + oldsize, 0, (newsize - oldsize) * sizeof(char));
	}
	unsigned char & l = colors[line];
	l = color;
}

void QCvsConsole::mousePressEvent(QMouseEvent * e)
{
	QCvsApp *app = QCvsApp::qApp;
	if(!app->IsCvsLoaded())
	{
		app->SetSoftFocus(QCvsApp::kConsole);
	}
	QMultiLineEdit::mousePressEvent(e);
}

void QCvsConsole::keyPressEvent(QKeyEvent * e)
{
	QCvsApp *app = QCvsApp::qApp;
	CStr script;
	if(!app->IsCvsLoaded())
	{
		if (e != 0L && e->key() == Key_Return)
		{
			int line, col;
			getCursorPosition(&line, &col);
			script = textLine(line);
		}
	}

	QMultiLineEdit::keyPressEvent(e);

	if(!script.empty())
	{
		CTcl_Interp interp;
		interp.DoScript(script);
	}
}

void QCvsConsole::paintCell(QPainter *painter, int row, int col)
{
	QPalette * pal = (QPalette *)&palette();
	QColor r;
	QColor s1 = pal->normal().text();
	QColor s2 = pal->disabled().text();
	QColor s3 = pal->active().text();

	bool changedColor = true;
	if((size_t)row < colors.size())
	{
		switch(colors[row])
		{
		case i_yellow :
			r = c_yellow;
			break;
		case i_green :
			r = c_green;
			break;
		case i_violet :
			r = c_violet;
			break;
		case i_red :
			r = c_red;
			break;
		case i_blue :
			r = c_blue;
			break;
		case i_orange :
			r = c_orange;
			break;
		default:
			changedColor = false;
			break;
		}
	}
	else
		changedColor = false;

	// that's tricky, but that's work
	if(changedColor)
	{
		*((QColor *)&pal->normal().text()) = r;
		*((QColor *)&pal->disabled().text()) = r;
		*((QColor *)&pal->active().text()) = r;
	}
	QMultiLineEdit::paintCell(painter, row, col);
	if(changedColor)
	{
		*((QColor *)&pal->normal().text()) = s1;
		*((QColor *)&pal->disabled().text()) = s2;
		*((QColor *)&pal->active().text()) = s3;
	}
}

#include "qcvsconsole.moc"
