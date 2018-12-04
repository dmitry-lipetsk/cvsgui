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

	File: QGetPrefs_PORTS.h
	Last generated: Mon Nov 23 23:11:16 1998

 *********************************************************************/

#ifndef QGetPrefs_PORTS_included
#define QGetPrefs_PORTS_included

#include "QGetPrefs_PORTSData.h"

class QGetPrefs_PORTS : public QGetPrefs_PORTSData
{
    Q_OBJECT

public:

    QGetPrefs_PORTS
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QGetPrefs_PORTS();

	void DoDataExchange(bool putvalue);

protected slots:
    virtual void ToggleRshName(bool);
    virtual void ToggleShellPort(bool);
    virtual void ToggleKerberosPort(bool);
    virtual void TogglePserverPort(bool);
    virtual void ToggleServerName(bool);
};
#endif // QGetPrefs_PORTS_included
