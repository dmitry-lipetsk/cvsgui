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

	File: QImportFilter.h
	Last generated: Mon Nov 30 18:18:11 1998

 *********************************************************************/

#ifndef QImportFilter_included
#define QImportFilter_included

#include "QImportFilterData.h"

class ReportConflict;
class ReportWarning;

class QImportType : public QListViewItem
{
public:
	QImportType(QListView * parent);
	virtual ~QImportType();

	virtual bool IsWarning() const = 0;
};

class QImportError : public QImportType
{
public:
	QImportError(QListView * parent, ReportWarning *data);
	virtual ~QImportError();

	// from QListViewItem
	virtual void setup();

	virtual bool IsWarning() const { return true; }

	ReportWarning *warn;
};

class QImportConflict : public QImportType
{
public:
	QImportConflict(QListView * parent, ReportConflict *data);
	virtual ~QImportConflict();

	// from QListViewItem
	virtual void setup();
	virtual QString text(int column) const;

	virtual bool IsWarning() const { return false; }

	ReportConflict *conf;
};

class QImportFilter : public QImportFilterData
{
    Q_OBJECT

public:

    QImportFilter
    (
		ReportConflict *& entries,
		ReportWarning *& warnings,
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QImportFilter();

protected:
	ReportConflict *& m_entries;
	ReportWarning *& m_warnings;

protected slots:
    virtual void OnEdit();
    virtual void OnEdit(QListViewItem*);
    virtual void OnSelChanged(QListViewItem*);
};

#endif // QImportFilter_included
