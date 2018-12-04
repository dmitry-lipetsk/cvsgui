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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "qfileitem.h"
#include "qfileview.h"

QFileItem::QFileItem(QFileView * parent, EntnodeData *data) :
	QListViewItem(parent), m_data(data)
{
}

QFileItem::~QFileItem()
{
}

void QFileItem::setup()
{
	setExpandable(FALSE);
	QPixmap *p = QFileView::GetImageForEntry(m_data);
	if(p != 0L)
		setPixmap(0, *p);

	QPixmap *rw = QFileView::GetImageForRW(!m_data->IsLocked());
	if(rw != 0L)
		setPixmap(1, *rw);

	QListViewItem::setup();
}

QString QFileItem::text( int column ) const
{
	QString res("");
	QFileView *view = GetFileView();
   	int details = view->GetFileDetails();
	if(details == 1 && column > 5)
		return res;
	if(details == 2 && column > 2)
		return res;

	if(column == 0)
		res = (*m_data)[column];
	else if(column != 1)
		res = (*m_data)[column - 1];
	return res;
}

#include "qfileitem.moc"
