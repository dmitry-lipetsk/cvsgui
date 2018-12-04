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

	File: QImportWarning.h
	Last generated: Tue Dec 1 14:10:17 1998

 *********************************************************************/

#ifndef QImportWarning_included
#define QImportWarning_included

#include "QImportWarningData.h"

class ReportWarning;

class QImportWarning : public QImportWarningData
{
    Q_OBJECT

public:

    QImportWarning
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QImportWarning();

	void DoDataExchange(bool putvalue, ReportWarning *warn);
};
#endif // QImportWarning_included
