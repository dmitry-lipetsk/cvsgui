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

	File: QUpdate_MERGE.h
	Last generated: Tue Nov 24 16:48:27 1998

 *********************************************************************/

#ifndef QUpdate_MERGE_included
#define QUpdate_MERGE_included

#include "QUpdate_MERGEData.h"
#include "CPStr.h"
#include "MultiString.h"

class QUpdate_MERGE : public QUpdate_MERGEData
{
    Q_OBJECT

public:

    QUpdate_MERGE
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QUpdate_MERGE();

	void DoDataExchange(bool putvalue, bool & merge1, bool & merge2,
						CPStr & sLastRev1, CPStr & sLastRev2,
						CMString & revNames);
protected slots:
    virtual void OnTwo(bool);
    virtual void OnOne(bool);
    virtual void OnNone(bool);
};
#endif // QUpdate_MERGE_included
