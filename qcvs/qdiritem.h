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

#ifndef QDIRITEM_H
#define QDIRITEM_H

#include <qlistview.h>
#include "CPStr.h"
#include "FileTraversal.h"
#include "CvsEntries.h"

class EntnodeData;
class QDirItem;
class QDirView;

// regarding a path and a root item, create an
// item with a dummy item inside if and only if
// the folder is not empty.
class CFillerDummy : public TraversalReport
{
public:
	QDirItem *m_root;
	QDirItem *m_item;
	QDirView *m_treeCtrl;
	bool m_empty;
	bool m_hascvs;
	EntnodeData *m_data;

	CFillerDummy(QDirView *treeCtrl, QDirItem *root, QDirItem *item, EntnodeData *data) :
		m_root(root), m_item(item), m_treeCtrl(treeCtrl), m_empty(true),
		m_hascvs(false), m_data(data) {}
	virtual ~CFillerDummy() {}

	virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname);
	virtual kTraversal ExitDirectory(const char *fullpath);

	virtual kTraversal OnError(const char *err, int errcode);
	virtual kTraversal OnIdle(const char *fullpath);

	virtual kTraversal OnDirectory(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, FSSpec * macspec);
};

// regarding a path and a root item, create an
// item with all the subdirectories inside.
class CExpandFiller : public TraversalReport
{
public:
	QDirItem *m_root;
	QDirItem *m_item;
	QDirView *m_treeCtrl;
	CSortList<ENTNODE> & m_entries;
	vector<CStr> m_ignlist;

	CExpandFiller(QDirView *treeCtrl, QDirItem *root, QDirItem *item, CSortList<ENTNODE> & entries) :
			m_root(root), m_item(item), m_treeCtrl(treeCtrl), m_entries(entries) {}
	virtual ~CExpandFiller() {}

	virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname);
	virtual kTraversal ExitDirectory(const char *fullpath);

	virtual kTraversal OnError(const char *err, int errcode);
	virtual kTraversal OnIdle(const char *fullpath);

	virtual kTraversal OnDirectory(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, FSSpec * macspec);
};

class QDirItem : public QListViewItem
{
public:
	QDirItem(QDirItem * parent, const char * filename, const char *fullpath);
	QDirItem(QDirView * parent, const char * filename, const char *fullpath);
	virtual ~QDirItem();

	// from QListViewItem
	virtual QString text(int column) const;
	virtual void setOpen(bool);
	virtual void setup();

	// CVS/Entries data
	void SetData(EntnodeData *data);
	inline EntnodeData *GetData() const { return m_data; }

	inline QDirView *GetDirView() const { return (QDirView *)listView(); }

	inline bool isExpanded() const { return m_expand; }
protected:
	void RemoveAllChilds();

	EntnodeData *m_data;
	CStr m_name;
	CStr m_path;
	bool m_expand;
};


#endif
/*test*/
