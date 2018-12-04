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

	File: QUpdate_MAIN.h
	Last generated: Tue Nov 24 16:25:12 1998

 *********************************************************************/

#ifndef QUpdate_MAIN_included
#define QUpdate_MAIN_included

#include "QUpdate_MAINData.h"

class QUpdate_MAIN : public QUpdate_MAINData
{
    Q_OBJECT

public:

    QUpdate_MAIN
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QUpdate_MAIN();

	void DoDataExchange(bool putvalue, bool & toStdout, bool & noRecurs,
						bool & resetSticky, bool & createMissDir);
};
#endif // QUpdate_MAIN_included
