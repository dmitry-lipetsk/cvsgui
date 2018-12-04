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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "qcvsapp.h"
#include "qcvsmainw.h"
#include "menumgr.h"
#include "qdirview.h"
#include "qfileview.h"
#include "qcvsconsole.h"
#include "Persistent.h"

#include <qlistview.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qkeycode.h>

static CPersistentT<QSize> sSize("P_qcvsmainwsize", QSize());
static CPersistentT<QSize> sMaxSize("P_qcvsmainwmaxsize", QSize());
static CPersistentT<QSize> sMinSize("P_qcvsmainwmminsize", QSize());
static CPersistentT<QCOORD> sSplitLoc1("P_qcvssplitloc1", 0);
static CPersistentT<QCOORD> sSplitLoc2("P_qcvssplitloc2", 0);

QCvsMainWindow::QCvsMainWindow() : QMainWindow( 0, "qcvs main window" ),
	s1(0L), s2(0L), leftPane(0L), rightPane(0L), console(0L)
{
	if(sMaxSize->isValid())
		setMaximumSize(sMaxSize);
	if(sMinSize->isValid())
		setMinimumSize(sMinSize);
	if(sSize->isValid())
		resize(sSize);

	//setIcon( PixmapFactory::GetPixmap( QtArch_bmp ) );
	setIconText("qcvs");
	InstallMenus(this);

	s1 = new QCvsSplitter(QSplitter::Horizontal, this, "main");
	leftPane = new QDirView(s1, "directories");

    s2 = new QCvsSplitter(QSplitter::Vertical, s1, "submain");
	rightPane = new QFileView(s2, "filesview");
	console = new QCvsConsole(s2, "console");
	console->setFocus();
	setCentralWidget(s1);

	statusBar()->message( "Ready", 2000 );

	connect(QMenuSlot::Get(iQuit), SIGNAL(cmdIsAboutToLaunch(QMenuSlot *)), this, SLOT(OnQuit(QMenuSlot *)));
	
	show();

	// the window needs to be realized to do this
	if(sSplitLoc1 > 0)
		s1->MoveSplitter(sSplitLoc1);
	if(sSplitLoc2 > 0)
		s2->MoveSplitter(sSplitLoc2);
}

QCvsMainWindow::~QCvsMainWindow()
{
}

void QCvsMainWindow::OnQuit(QMenuSlot *)
{
	sMaxSize = maximumSize();
	sMinSize = minimumSize();
	sSize = size();
	sSplitLoc1 = leftPane->geometry().width();
	sSplitLoc2 = rightPane->geometry().height();
}

#include "qcvsmainw.moc"
