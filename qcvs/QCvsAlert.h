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

	File: QCvsAlert.h
	Last generated: Sun Nov 22 21:57:39 1998

 *********************************************************************/

#ifndef QCvsAlert_included
#define QCvsAlert_included

#include "QCvsAlertData.h"

class QCvsAlert : public QCvsAlertData
{
    Q_OBJECT

public:

    QCvsAlert
    (
		const char *prompt, const char *defhit, const char *cnclhit,
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCvsAlert();
protected slots:

protected:
};
#endif // QCvsAlert_included
