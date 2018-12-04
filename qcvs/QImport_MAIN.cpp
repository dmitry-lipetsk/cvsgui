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

	File: QImport_MAIN.cpp
	Last generated: Tue Dec 1 14:39:25 1998

 *********************************************************************/

#include "QImport_MAIN.h"
#include "CPStr.h"

#define Inherited QImport_MAINData

QImport_MAIN::QImport_MAIN
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QImport_MAIN::~QImport_MAIN()
{
}

void QImport_MAIN::DoDataExchange(bool putvalue, CPStr & sLastModName, CStr & sLastMsg,
								  CPStr & sLastVTag, CPStr & sLastRTag)
{
	if(putvalue)
	{
		fModule->setText(QString(sLastModName));
		fLog->setText(QString(sLastMsg));
		fVendor->setText(QString(sLastVTag));
		fRelease->setText(QString(sLastRTag));
	}
	else
	{
		sLastModName = fModule->text();
		sLastMsg = fLog->text();
		sLastVTag = fVendor->text();
		sLastRTag = fRelease->text();
	}
}

#include "QImport_MAIN.moc"
#include "QImport_MAINData.moc"
