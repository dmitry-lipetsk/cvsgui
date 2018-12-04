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

	File: QGetPrefs_GLOBALS.h
	Last generated: Mon Nov 23 22:23:01 1998

 *********************************************************************/

#ifndef QGetPrefs_GLOBALS_included
#define QGetPrefs_GLOBALS_included

#include "QGetPrefs_GLOBALSData.h"

class QGetPrefs_GLOBALS : public QGetPrefs_GLOBALSData
{
    Q_OBJECT

public:

    QGetPrefs_GLOBALS
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QGetPrefs_GLOBALS();

	void DoDataExchange(bool putvalue);
};
#endif // QGetPrefs_GLOBALS_included
