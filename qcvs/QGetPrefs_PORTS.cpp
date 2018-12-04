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

/**********************************************************************

	--- Qt Architect generated file ---

	File: QGetPrefs_PORTS.cpp
	Last generated: Mon Nov 23 23:11:16 1998

 *********************************************************************/

#include "QGetPrefs_PORTS.h"
#include "CvsPrefs.h"

#define Inherited QGetPrefs_PORTSData

QGetPrefs_PORTS::QGetPrefs_PORTS
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QGetPrefs_PORTS::~QGetPrefs_PORTS()
{
}

void QGetPrefs_PORTS::DoDataExchange(bool putvalue)
{
	QString str;
	int val;

	if(putvalue)
	{
		fShellPortCheck->setChecked(gCvsPrefs.RhostPort() != -1);
		fPserverPortCheck->setChecked(gCvsPrefs.PserverPort() != -1);
		fKerberosPortCheck->setChecked(gCvsPrefs.KserverPort() != -1);
		str.setNum(gCvsPrefs.RhostPort() != -1 ? gCvsPrefs.RhostPort() : 0);
		fShellPort->setText(str);
		str.setNum(gCvsPrefs.PserverPort() != -1 ? gCvsPrefs.PserverPort() : 0);
		fServerPort->setText(str);
		str.setNum(gCvsPrefs.KserverPort() != -1 ? gCvsPrefs.KserverPort() : 0);
		fKerberosPort->setText(str);
		
		fAltServerCheck->setChecked(gCvsPrefs.ServerName() != 0L);
		fServerName->setText(gCvsPrefs.ServerName() == 0L ? "cvs" : gCvsPrefs.ServerName());
		fAltRshCheck->setChecked(gCvsPrefs.RshName() != 0L);
		fRshName->setText(gCvsPrefs.RshName() == 0L ? "ssh" : gCvsPrefs.RshName());

		ToggleRshName(fAltRshCheck->isChecked());
		ToggleShellPort(fShellPortCheck->isChecked());
		ToggleKerberosPort(fKerberosPortCheck->isChecked());
		TogglePserverPort(fPserverPortCheck->isChecked());
		ToggleServerName(fAltServerCheck->isChecked());
	}
	else
	{
		bool OK;
		str = fKerberosPort->text();
		val = str.toInt(&OK);
		gCvsPrefs.SetKserverPort(fKerberosPortCheck->isChecked() && OK ? val : -1);
		str = fServerPort->text();
		val = str.toInt(&OK);
		gCvsPrefs.SetPserverPort(fPserverPortCheck->isChecked() && OK ? val : -1);
		str = fShellPort->text();
		val = str.toInt(&OK);
		gCvsPrefs.SetRhostPort(fShellPortCheck->isChecked() && OK ? val : -1);

		gCvsPrefs.SetServerName(fAltServerCheck->isChecked() ? (const char *)fServerName->text() : 0L);
		gCvsPrefs.SetRshName(fAltRshCheck->isChecked() ? (const char *)fRshName->text() : 0L);
	}
}

void QGetPrefs_PORTS::ToggleRshName(bool state)
{
	fRshName->setEnabled(state);
}

void QGetPrefs_PORTS::ToggleShellPort(bool state)
{
	fShellPort->setEnabled(state);
}

void QGetPrefs_PORTS::ToggleKerberosPort(bool state)
{
	fKerberosPort->setEnabled(state);
}

void QGetPrefs_PORTS::TogglePserverPort(bool state)
{
	fServerPort->setEnabled(state);
}

void QGetPrefs_PORTS::ToggleServerName(bool state)
{
	fServerName->setEnabled(state);
}

#include "QGetPrefs_PORTS.moc"
#include "QGetPrefs_PORTSData.moc"
