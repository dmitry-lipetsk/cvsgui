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

	File: QGetPrefsTab.cpp
	Last generated: Mon Nov 23 23:04:20 1998

 *********************************************************************/

#include "QGetPrefsTab.h"

#define Inherited QGetPrefsTabData

QGetPrefsTab::QGetPrefsTab
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name ),
	fDlg1(parent, name),
	fDlg2(parent, name),
	fDlg3(parent, name)
{
	setCaption( "Preferences" );

    addTab( &fDlg1, "General" );
    addTab( &fDlg2, "Globals" );
    addTab( &fDlg3, "Ports" );
    
    setCancelButton();
}


QGetPrefsTab::~QGetPrefsTab()
{
}

#include "QGetPrefsTab.moc"
#include "QGetPrefsTabData.moc"
