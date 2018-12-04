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

	File: QGetPassword.h
	Last generated: Sun Nov 22 22:31:13 1998

 *********************************************************************/

#ifndef QGetPassword_included
#define QGetPassword_included

#include "QGetPasswordData.h"

class QGetPassword : public QGetPasswordData
{
    Q_OBJECT

public:

    QGetPassword
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QGetPassword();

	const char *GetPassword() const { return fPassword->text() ; }

};
#endif // QGetPassword_included
