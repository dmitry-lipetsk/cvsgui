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

	File: QCheckout_STICKY.h
	Last generated: Tue Nov 24 16:49:08 1998

 *********************************************************************/

#ifndef QCheckout_STICKY_included
#define QCheckout_STICKY_included

#include "QCheckout_STICKYData.h"
#include "CPStr.h"
#include "MultiString.h"

class QCheckout_STICKY : public QCheckout_STICKYData
{
    Q_OBJECT

public:

    QCheckout_STICKY
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCheckout_STICKY();

	void DoDataExchange(bool putvalue, bool & hasDate, bool & hasRev,
						CPStr & sLastDate, CPStr & sLastRev, bool & useMostRecent,
						CMString & revNames, CMString & dateNames);

protected slots:
    virtual void OnDateCheck(bool);
    virtual void OnRevCheck(bool);
};
#endif // QCheckout_STICKY_included
