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

	File: QImportReport.cpp
	Last generated: Mon Nov 30 23:35:52 1998

 *********************************************************************/

#include "QImportReport.h"
#include "ImportFilter.h"
#include <qradiobutton.h>

#define Inherited QImportReportData

QImportReport::QImportReport
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( "Import report" );
}


QImportReport::~QImportReport()
{
}

void QImportReport::DoDataExchange(bool putvalue, ReportConflict *conf)
{
	if(putvalue)
	{
		if(conf->HasForceBinary())
			fbut1->setChecked(true);
		else if(conf->HasForceText())
			fbut2->setChecked(true);
		else if(conf->HasIgnore())
			fbut3->setChecked(true);
		else
			fbut0->setChecked(true);
		CStr print;
		conf->PrintOut(print);
		fConflict->setText(QString(print));	
	}
	else
	{
		if(fbut1->isChecked())
			conf->ForceBinary();
		else if(fbut2->isChecked())
			conf->ForceText();
		else if(fbut3->isChecked())
			conf->Ignore();
		else
			conf->DisableUserSettings();
	}
}

#include "QImportReport.moc"
#include "QImportReportData.moc"
