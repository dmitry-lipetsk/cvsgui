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

	File: QCommit_MAIN.h
	Last generated: Sat Nov 28 20:44:24 1998

 *********************************************************************/

#ifndef QCommit_MAIN_included
#define QCommit_MAIN_included

#include "QCommit_MAINData.h"
#include "MultiString.h"

class QCommit_MAIN : public QCommit_MAINData
{
    Q_OBJECT

public:

    QCommit_MAIN
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCommit_MAIN();

	void DoDataExchange(bool putvalue, CStr & commit, bool & norecurs,
						const char *pathTmpl, CMString & commitLogs);

protected:
	CStr m_pathTmpl;
	CMString *m_CommitLogs;

protected slots:
    virtual void OnCombo(int);
};
#endif // QCommit_MAIN_included
