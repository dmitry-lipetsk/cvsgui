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

	File: QUpdate_MAIN.cpp
	Last generated: Tue Nov 24 16:25:12 1998

 *********************************************************************/

#include "QUpdate_MAIN.h"

#define Inherited QUpdate_MAINData

QUpdate_MAIN::QUpdate_MAIN
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QUpdate_MAIN::~QUpdate_MAIN()
{
}

void QUpdate_MAIN::DoDataExchange(bool putvalue, bool & toStdout, bool & noRecurs,
						bool & resetSticky, bool & createMissDir)
{
	if(putvalue)
	{
		fStdout->setChecked(toStdout);
		fNorecurs->setChecked(noRecurs);
		fResetSticky->setChecked(resetSticky);
		fCreateMissing->setChecked(createMissDir);
	}
	else
	{
		noRecurs = fNorecurs->isChecked();
		toStdout = fStdout->isChecked();
		resetSticky = fResetSticky->isChecked();
		createMissDir = fCreateMissing->isChecked();
	}
}

#include "QUpdate_MAIN.moc"
#include "QUpdate_MAINData.moc"
