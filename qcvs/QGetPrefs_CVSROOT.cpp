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

	File: QGetPrefs_CVSROOT.cpp
	Last generated: Mon Nov 23 23:12:54 1998

 *********************************************************************/

#include "QGetPrefs_CVSROOT.h"
#include "CvsPrefs.h"
#include "Authen.h"

#define Inherited QGetPrefs_CVSROOTData

QGetPrefs_CVSROOT::QGetPrefs_CVSROOT
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( "Preferences" );
}


QGetPrefs_CVSROOT::~QGetPrefs_CVSROOT()
{
}

void QGetPrefs_CVSROOT::DoDataExchange(bool putvalue)
{
	CStr str;
	if(putvalue)
	{
		fCvsroot->setEditText((const char *)gCvsPrefs);

		fAuthen->setExclusive(TRUE);
		if(gAuthen.kind() == local)
			fLocal->setChecked(true);
		else if(gAuthen.kind() == rhosts)
			fRsh->setChecked(true);
		else if(gAuthen.kind() == pserver)
			fPserver->setChecked(true);
		else if(gAuthen.kind() == kserver)
			fKserver->setChecked(true);
		else if(gAuthen.kind() == ssh)
			fSsh->setChecked(true);
		fCvs->setButton(0);

		for(int i = 0; i < NUM_CVSROOT; i++)
		{
			const CPStr & str = ((const CvsPrefs &)gCvsPrefs).get_cvsroot_list(i);
			if(!str.empty())
			{
				fCvsroot->insertItem(QString(str));
			}
		}
	}
	else
	{
		if(fLocal->isChecked())
			gAuthen.setkind(local);
		else if(fRsh->isChecked())
			gAuthen.setkind(rhosts);
		else if(fPserver->isChecked())
			gAuthen.setkind(pserver);
		else if(fKserver->isChecked())
			gAuthen.setkind(kserver);
		else if(fSsh->isChecked())
			gAuthen.setkind(ssh);

		str = fCvsroot->currentText();
		if(!str.empty())
			gCvsPrefs = str;
	}
}

#include "QGetPrefs_CVSROOT.moc"
#include "QGetPrefs_CVSROOTData.moc"
