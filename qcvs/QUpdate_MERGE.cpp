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

	File: QUpdate_MERGE.cpp
	Last generated: Tue Nov 24 16:48:27 1998

 *********************************************************************/

#include "QUpdate_MERGE.h"
#include "qcvsapp.h"

#define Inherited QUpdate_MERGEData

QUpdate_MERGE::QUpdate_MERGE
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QUpdate_MERGE::~QUpdate_MERGE()
{
}

    QRadioButton* fNoneRadio;
    QRadioButton* fOneRadio;
    QRadioButton* fTwoRadio;
    QComboBox* fRev1;
    QComboBox* fRev2;

void QUpdate_MERGE::DoDataExchange(bool putvalue, bool & merge1, bool & merge2,
								   CPStr & sLastRev1, CPStr & sLastRev2,
								   CMString & revNames)
{
	::DoDataExchange(putvalue, revNames, fRev1);
	::DoDataExchange(putvalue, revNames, fRev2);

	if(putvalue)
	{
		if(merge1 && merge2)
			fTwoRadio->setChecked(true);
		else if(merge1)
			fOneRadio->setChecked(true);
		else
			fNoneRadio->setChecked(true);
		fRev1->setEditText(QString(sLastRev1));
		fRev2->setEditText(QString(sLastRev2));

		OnNone(fNoneRadio->isChecked());
		OnOne(fOneRadio->isChecked());
		OnTwo(fTwoRadio->isChecked());
	}
	else
	{
		sLastRev1 = fRev1->currentText();
		sLastRev2 = fRev2->currentText();
		if(fTwoRadio->isChecked())
		{
			merge1 = true;
			merge2 = true;
		}
		else if(fOneRadio->isChecked())
		{
			merge1 = true;
			merge2 = false;
		}
		else
		{
			merge1 = false;
			merge2 = false;
		}
	}
}

void QUpdate_MERGE::OnTwo(bool state)
{
	if(!state)
		return;

	fRev1->setEnabled(true);
	fRev2->setEnabled(true);
}

void QUpdate_MERGE::OnOne(bool state)
{
	if(!state)
		return;

	fRev1->setEnabled(true);
	fRev2->setEnabled(false);
}

void QUpdate_MERGE::OnNone(bool state)
{
	if(!state)
		return;

	fRev1->setEnabled(false);
	fRev2->setEnabled(false);
}

#include "QUpdate_MERGE.moc"
#include "QUpdate_MERGEData.moc"
