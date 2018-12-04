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
#include "qcvsmainw.h"
#include "CvsPrefs.h"
#include "Authen.h"
#include "AppConsole.h"
#include "GetPrefs.h"
#include "menumgr.h"
#include "TclGlue.h"
#include "MacrosSetup.h"

#include <qcombobox.h>
#include <qtimer.h>

QCvsApp *QCvsApp::qApp = 0L;

QCvsApp::QCvsApp(int & argc, char ** argv) : QApplication(argc, argv),
	m_dirview(0L), m_fileview(0L), m_console(0L), cvsLoaded(false), gCvsStopping(false),
	menuInvalid(true), m_focus(kNone)
{
	qApp = this;
	gCvsPrefs.load();
	MacrosReloadAll();

	QCvsMainWindow * mw = new QCvsMainWindow();
	setMainWidget(mw);
	mw->setCaption("qcvs");

	cvs_out("Welcome to %s %s\n", PACKAGE, VERSION);
	if(gCvsPrefs.empty())
		CompatGetPrefs();
	else
		cvs_out("CVSROOT: %s (%s)\n", (const char *)gCvsPrefs, gAuthen.kindstr());

	if(CTcl_Interp::IsAvail())
	{
		cvs_out("TCL is available, shell and macros are enabled : help (select and press enter)\n");
	}
	else
	{
		cvs_err("TCL is *not* available, shell and macros are disabled\n");
	}

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(OnNullEvent()) );
	timer->start(100, false);

	connect(this, SIGNAL(lastWindowClosed()), QMenuSlot::Get(iQuit), SLOT(cmd()));	
}

QCvsApp::~QCvsApp()
{
}

void QCvsApp::SetSoftFocus(kSoftFocus focus)
{
	m_focus = focus;
	InvalidateMenus();
}

void QCvsApp::InvalidateMenus()
{
	menuInvalid = true;
}

void QCvsApp::SetCvsLoaded(bool state)
{
	if(state ^ cvsLoaded)
	{
		cvsLoaded = state;
		InvalidateMenus();
	}
}

void QCvsApp::SetCvsStopping(bool state)
{
	if(state ^ gCvsStopping)
	{
		gCvsStopping = state;
		InvalidateMenus();
	}
}

void QCvsApp::OnNullEvent()
{
	if(isMenuInvalid() && !CurrentUpdatingMenus())
	{
		UpdateMenus();
		ValidateMenus();
	}
}

void DoDataExchange(bool putValue, CMString & mstr, QComboBox * combo)
{
	if(putValue)
	{
		combo->clear();
		const vector<CStr> & list = mstr.GetList();
		vector<CStr>::const_iterator i;
		for(i = list.begin(); i != list.end(); ++i)
		{
			combo->insertItem(QString(*i));
		}
	}
	else
	{
		CStr value(combo->currentText());
		if(!value.empty())
		{
			mstr.Insert(value);
		}
	}
}

const char *QCvsApp::GetPkgDataDir() const
{
	return PKGDATA;
}

#include "qcvsapp.moc"
