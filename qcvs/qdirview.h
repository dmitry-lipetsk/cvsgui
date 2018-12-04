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

#ifndef QDIRVIEW_H
#define QDIRVIEW_H

#include <qlistview.h>
#include "CPStr.h"
#include <vector>

class QDirItem;

class QDirView : public QListView
{
	Q_OBJECT
public:
	QDirView(QWidget *parent = 0L, const char *name = 0L);
	virtual ~QDirView();

	void ResetBrowser(const char *path, bool notifyView = false);
		// set the browser root path.

	void ResetBrowser(bool forceReload = false, bool notifyView = false);
		// force reload or check modifications before redrawing
		// and optionnaly synchronize the file view.

	void RetrievePath(QDirItem *item, CStr & path);
		// given an item of the tree, guess the full path.

	void StepToLocation(const char *path, bool notifyView = false);
		// expand and select the path and optionnaly synchronize the
		// file view.

	QDirItem *GetItemByLocation(const char *path);

	inline const char *GetRootPath() const { return m_path; }

protected:
	CStr m_path;

	void StoreExpanded(vector<CStr> & allExpanded);

	QDirItem *GetSelectedItem();

protected slots:
	void OnSelectionChanged (QListViewItem *item);

protected:
	virtual void mousePressEvent ( QMouseEvent * e );
	virtual void mouseReleaseEvent ( QMouseEvent * e );
	virtual void mouseDoubleClickEvent ( QMouseEvent * e );
	virtual void focusInEvent ( QFocusEvent * e );
	virtual void focusOutEvent ( QFocusEvent * e );
	virtual void keyPressEvent ( QKeyEvent * e );
	virtual void enabledChange ( bool );
};

#endif
