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

#ifndef QFILEVIEW_H
#define QFILEVIEW_H

#include <qlistview.h>
#include "CvsEntries.h"
#include "CPStr.h"

#ifdef TIME_WITH_SYS_TIME
#include <time.h>
#include <sys/time.h>
#endif

extern CPersistentT<bool> gFileViewIgnore;
extern CPersistentT<int> gFileDetails;

class QFileView : public QListView
{
	Q_OBJECT
public:
	QFileView(QWidget *parent = 0L, const char *name = 0L);
	virtual ~QFileView();

	void ResetView(const char *path, bool notifyBrowser = false);
		// reset to this path and notify optionnaly the directories tree

	void ResetView(bool forceReload = false, bool notifyBrowser = false);
		// - reload and try to guess if something changed
		// according to the mod. time of CVS
		// - notify optionnaly the directories tree

	static QPixmap *GetImageForEntry(EntnodeData *data);
	static QPixmap *GetImageForRW(bool isRW);

	inline const char *GetPath() const { return m_path; }

	void OnUpPath();
		// go up for one directory level

	void OnFileDetails(int details);
	inline int GetFileDetails() const { return gFileDetails; }
		// 0, 1, 2

protected:
	CSortList<ENTNODE> m_entries;
		// CVS/Entries infos
	CStr m_path;
		// current path
	time_t m_entriesMod;
	time_t m_entriesLogMod;
		// time stamp of CVS/*

	void GetEntriesModTime(time_t & newEntriesMod, time_t & newEntriesLogMod);

protected slots:
	void OnSelectionChanged ();
	void OnOpenSelection(QListViewItem *item);

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
