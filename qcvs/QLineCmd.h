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

	File: QLineCmd.h
	Last generated: Thu Nov 26 22:42:24 1998

 *********************************************************************/

#ifndef QLineCmd_included
#define QLineCmd_included

#include "QLineCmdData.h"
#include "CPStr.h"

class QLineCmd : public QLineCmdData
{
    Q_OBJECT

public:

    QLineCmd
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QLineCmd();

	void DoDataExchange(bool putvalue, CStr & cmdline, CPStr & path, bool & hasPath);

protected slots:
    virtual void OnCheckPath(bool);
};
#endif // QLineCmd_included
