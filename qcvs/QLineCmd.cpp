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

	File: QLineCmd.cpp
	Last generated: Thu Nov 26 22:42:24 1998

 *********************************************************************/

#include "QLineCmd.h"
#include "PromptFiles.h"

#define Inherited QLineCmdData

QLineCmd::QLineCmd
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( "Line command" );
}


QLineCmd::~QLineCmd()
{
}

void QLineCmd::DoDataExchange(bool putvalue, CStr & cmdline, CPStr & path, bool & hasPath)
{
	if(putvalue)
	{
		fCmd->setText(QString(cmdline));
		fPath->setText(QString(path));
		fPathCheck->setChecked(hasPath);

		OnCheckPath(fPathCheck->isChecked());
	}
	else
	{
		cmdline = fCmd->text();
		path = fPath->text();
		hasPath = fPathCheck->isChecked();
	}
}

void QLineCmd::OnCheckPath(bool state)
{
	fPath->setEnabled(state);
	if(state)
	{
		const char *path = BrowserGetDirectory("Select a directory to execute in :");
		if(path != 0L)
			fPath->setText(path);
	}
}

#include "QLineCmd.moc"
#include "QLineCmdData.moc"
