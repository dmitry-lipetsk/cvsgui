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

#ifndef QFILEITEM_H
#define QFILEITEM_H

#include <qlistview.h>
#include "CPStr.h"
#include "CvsEntries.h"

class EntnodeData;
class QFileView;

class QFileItem : public QListViewItem
{
public:
	QFileItem(QFileView * parent, EntnodeData *data);
	virtual ~QFileItem();

	// from QListViewItem
	virtual QString text(int column) const;
	virtual void setup();

	inline QFileView *GetFileView() const { return (QFileView *)listView(); }

	// CVS/Entries data
	inline EntnodeData *GetData() const { return m_data; }
protected:
	EntnodeData *m_data;
};

#endif
