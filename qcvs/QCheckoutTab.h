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

	File: QCheckoutTab.h
	Last generated: Mon Nov 23 23:04:20 1998

 *********************************************************************/

#ifndef QCheckoutTab_included
#define QCheckoutTab_included

#include "QCheckoutTabData.h"

#include "QGetPrefs_GLOBALS.h"
#include "QCheckout_MAIN.h"
#include "QCheckout_STICKY.h"

class QCheckoutTab : public QCheckoutTabData
{
    Q_OBJECT

public:

    QCheckoutTab
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCheckoutTab();

	QCheckout_MAIN fDlg1;
	QCheckout_STICKY fDlg2;
	QGetPrefs_GLOBALS fDlg3;
};
#endif // QCheckoutTab_included
