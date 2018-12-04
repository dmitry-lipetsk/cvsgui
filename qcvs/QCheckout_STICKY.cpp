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

	File: QCheckout_STICKY.cpp
	Last generated: Tue Nov 24 16:49:08 1998

 *********************************************************************/

#include "QCheckout_STICKY.h"
#include "qcvsapp.h"

#define Inherited QCheckout_STICKYData

QCheckout_STICKY::QCheckout_STICKY
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QCheckout_STICKY::~QCheckout_STICKY()
{
}

void QCheckout_STICKY::DoDataExchange(bool putvalue, bool & hasDate, bool & hasRev,
									CPStr & sLastDate, CPStr & sLastRev, bool & useMostRecent,
									CMString & revNames, CMString & dateNames)
{
	::DoDataExchange(putvalue, revNames, fRev);
	::DoDataExchange(putvalue, dateNames, fDate);

	if(putvalue)
	{
		fDateCheck->setChecked(hasDate);
		fRevCheck->setChecked(hasRev);
		fRecent->setChecked(useMostRecent);
		fDate->setEditText(QString(sLastDate));
		fRev->setEditText(QString(sLastRev));

		OnDateCheck(fDateCheck->isChecked());
		OnRevCheck(fRevCheck->isChecked());
	}
	else
	{
		hasDate = fDateCheck->isChecked();
		hasRev = fRevCheck->isChecked();
		useMostRecent = fRecent->isChecked();

		sLastDate = fDate->currentText();
		sLastRev = fRev->currentText();
	}
}

void QCheckout_STICKY::OnDateCheck(bool state)
{
	fDate->setEnabled(fDateCheck->isChecked());
}

void QCheckout_STICKY::OnRevCheck(bool state)
{
	fRev->setEnabled(fRevCheck->isChecked());
}

#include "QCheckout_STICKY.moc"
#include "QCheckout_STICKYData.moc"
