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

#ifndef QCVSCONSOLE_H
#define QCVSCONSOLE_H

#include <qmultilinedit.h>
#include "CPStr.h"

class QCvsConsole : public QMultiLineEdit
{
	Q_OBJECT
public:
	QCvsConsole(QWidget *parent=0, const char *name=0);
	virtual ~QCvsConsole();

	void OutConsole(const char *txt, unsigned int len, bool isStderr = false);

protected:
	void SetColor(int line, unsigned char color);
	void FeedThisLine(const char *buffer, unsigned int n, bool isStdErr);

	CStaticAllocT<unsigned char> colors;

	virtual void mousePressEvent(QMouseEvent * e);
	virtual void keyPressEvent(QKeyEvent * e);
	virtual void paintCell(QPainter *painter, int row, int col);
};

#endif
