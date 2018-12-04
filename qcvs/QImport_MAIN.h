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

	File: QImport_MAIN.h
	Last generated: Tue Dec 1 14:39:25 1998

 *********************************************************************/

#ifndef QImport_MAIN_included
#define QImport_MAIN_included

#include "QImport_MAINData.h"

class CStr;
class CPStr;

class QImport_MAIN : public QImport_MAINData
{
    Q_OBJECT

public:

    QImport_MAIN
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QImport_MAIN();

	void DoDataExchange(bool putvalue, CPStr & sLastModName, CStr & sLastMsg, CPStr & sLastVTag, CPStr & sLastRTag);
};
#endif // QImport_MAIN_included
