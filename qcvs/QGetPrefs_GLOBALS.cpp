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

	File: QGetPrefs_GLOBALS.cpp
	Last generated: Mon Nov 23 22:23:01 1998

 *********************************************************************/

#include "QGetPrefs_GLOBALS.h"
#include "CvsPrefs.h"

#define Inherited QGetPrefs_GLOBALSData

QGetPrefs_GLOBALS::QGetPrefs_GLOBALS
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QGetPrefs_GLOBALS::~QGetPrefs_GLOBALS()
{
}

void QGetPrefs_GLOBALS::DoDataExchange(bool putvalue)
{
	CStr str;
	if(putvalue)
	{
		fReadonly->setChecked(gCvsPrefs.CheckoutRO());
		fPrune->setChecked(gCvsPrefs.PruneOption());
		fZ9->setChecked(gCvsPrefs.Z9Option());
		fQuiet->setChecked(gCvsPrefs.QuietOption());
		fAddcontrol->setChecked(gCvsPrefs.AddControl());

		/*m_alwaysroot = gCvsPrefs.AlwaysUseCvsroot();
		m_logout = gCvsPrefs.LogoutTimeOut() > 0;
		m_editlogout = gCvsPrefs.LogoutTimeOut();
		fCvsroot->setEditText((const char *)gCvsPrefs);*/
	}
	else
	{
		gCvsPrefs.SetCheckoutRO(fReadonly->isChecked());
		gCvsPrefs.SetPruneOption(fPrune->isChecked());
		gCvsPrefs.SetZ9Option(fZ9->isChecked());
		gCvsPrefs.SetQuietOption(fQuiet->isChecked());
		gCvsPrefs.SetAddControl(fAddcontrol->isChecked());
	}
}

#include "QGetPrefs_GLOBALS.moc"
#include "QGetPrefs_GLOBALSData.moc"
