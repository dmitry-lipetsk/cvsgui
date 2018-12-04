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

	File: QImportWarning.cpp
	Last generated: Tue Dec 1 14:10:17 1998

 *********************************************************************/

#include "QImportWarning.h"
#include "ImportFilter.h"

#define Inherited QImportWarningData

QImportWarning::QImportWarning
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( "Import Warning" );
}


QImportWarning::~QImportWarning()
{
}

void QImportWarning::DoDataExchange(bool putvalue, ReportWarning *warn)
{
	if(putvalue)
	{
		CStr str;
		warn->PrintOut(str);
		fText->setText(QString(str));
	}
}

#include "QImportWarning.moc"
#include "QImportWarningData.moc"
