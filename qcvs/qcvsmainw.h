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

#ifndef QCVSMAINW_H
#define QCVSMAINW_H

#include <qmainwindow.h>
#include <qsplitter.h>

class QMenuSlot;
class QDirView;
class QFileView;
class QCvsConsole;

// this class just for accessing moveSplitter...
class QCvsSplitter : public QSplitter
{
	Q_OBJECT
public:
	QCvsSplitter(Orientation o, QWidget * parent = 0, const char * name = 0) :
		QSplitter(o, parent, name) {}
	virtual ~QCvsSplitter() {}

	inline void MoveSplitter (QCOORD pos) { moveSplitter(pos, 0); }
};

class QCvsMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	QCvsMainWindow();
	virtual ~QCvsMainWindow();

protected slots:
	void OnQuit(QMenuSlot *);

protected:
	QCvsSplitter *s1, *s2;
	QDirView *leftPane;
	QFileView *rightPane;
	QCvsConsole *console;
};

#endif
