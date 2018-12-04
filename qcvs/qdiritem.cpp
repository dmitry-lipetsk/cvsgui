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
#endif

#include "qdiritem.h"
#include "qfileview.h"

#include <qpixmap.h>

#include <vector>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "state7.xpm"
#include "state8.xpm"
#include "state9.xpm"
#include "state13.xpm"

static void SetIcon(QDirView *treeCtrl, QDirItem *item,
					bool hasCvsInfos, bool expand, EntnodeData *data = 0L)
{
	QPixmap *newImage;

	static QPixmap *iDir = 0L;
	static QPixmap *iDirUnknown = 0L;
	static QPixmap *iDirMissing = 0L;
	static QPixmap *iDirIgnore = 0L;

	if(iDir == 0L)
	{
		iDir = new QPixmap(i_state7);
		iDirUnknown = new QPixmap(i_state8);
		iDirMissing = new QPixmap(i_state9);
		iDirIgnore = new QPixmap(i_state13);
	}

	if(data == 0L)
	{
		if(expand)
			newImage = hasCvsInfos ? iDir : iDirUnknown;
		else
			newImage = hasCvsInfos ? iDir : iDirUnknown;
	}
	else
	{
		if(data->IsIgnored())
			newImage = iDirIgnore;
		else if(data->IsUnknown())
			newImage = iDirUnknown;
		else if(data->IsMissing())
			newImage = iDirMissing;
		else
			newImage = iDir;
	}

	item->setPixmap(0, *newImage);
}

static const char *gDummyFile = "@@@dummy file@@@";

kTraversal CFillerDummy::EnterDirectory(const char *fullpath, const char *dirname)
{
	if(m_item == 0L)
	{
		if(m_root == 0L)
			m_item = new QDirItem(m_treeCtrl, dirname, fullpath);
		else
			m_item = new QDirItem(m_root, dirname, fullpath);
	}

	// assign the entries to this item
	if(m_data != 0L)
		m_item->SetData(m_data);
	else
		m_data = m_item->GetData();

	return kContinueTraversal;
}

kTraversal CFillerDummy::ExitDirectory(const char *fullpath)
{
	SetIcon(m_treeCtrl, m_item, m_hascvs, false, m_data);

	if(m_empty)
		return kContinueTraversal;

		// add a dummy item to simulate the folder
		// is not empty. This dummy item will be
		// replaced by a complete listing when the
		// node is expanding...
	new QDirItem(m_item, gDummyFile, "");

	return kContinueTraversal;
}

kTraversal CFillerDummy::OnError(const char *err, int errcode)
{
	return kTraversalError;
}

kTraversal CFillerDummy::OnIdle(const char *fullpath)
{
	return kContinueTraversal;
}

kTraversal CFillerDummy::OnDirectory(const char *fullpath,
									 const char *fullname,
									 const char *name,
									 const struct stat & dir, FSSpec * macspec)
{
	if(strcmp(name, "CVS") == 0)
		m_hascvs = true;
	else
		m_empty = false;
	return kSkipFile;
}

kTraversal CExpandFiller::EnterDirectory(const char *fullpath, const char *dirname)
{
	if(chdir("CVS") == 0)
	{
		Entries_Open (m_entries, fullpath);
		if(chdir(fullpath) != 0)
			return kTraversalError;
		BuildIgnoredList(m_ignlist);
	}

	return kContinueTraversal;
}

kTraversal CExpandFiller::ExitDirectory(const char *fullpath)
{
	m_ignlist.erase(m_ignlist.begin(), m_ignlist.end());
	SetIcon(m_treeCtrl, m_item, true /*ignored*/, true, m_item->GetData());
	return kContinueTraversal;
}

kTraversal CExpandFiller::OnError(const char *err, int errcode)
{
	return kTraversalError;
}

kTraversal CExpandFiller::OnIdle(const char *fullpath)
{
	return kContinueTraversal;
}

kTraversal CExpandFiller::OnDirectory(const char *fullpath,
									  const char *fullname,
									  const char *name,
									  const struct stat & dir, FSSpec * macspec)
{
	if(strcmp(name, "CVS") == 0)
		return kSkipFile;

	// is the sub-directory ignored ?
	EntnodeData *data = Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);
	if(!(bool)gFileViewIgnore && data->IsIgnored())
		return kSkipFile;

	// create the item for the sub-directory
	CFillerDummy traverse(m_treeCtrl, m_item, 0L, data);
	/*kTraversal res = */FileTraverse(fullname, traverse);

	// assign the icon regarding our Entries info
	if(traverse.m_item != 0L)
	{
		SetIcon(m_treeCtrl, traverse.m_item, true /*ignored*/, false, data);
	}

	return kSkipFile;
}

QDirItem::QDirItem(QDirItem * parent, const char * filename, const char *fullpath) :
	QListViewItem(parent), m_data(0L), m_name(filename), m_path(fullpath), m_expand(false)
{
}

QDirItem::QDirItem(QDirView * parent, const char * filename, const char *fullpath) :
	QListViewItem((QListView *)parent), m_data(0L), m_name(filename), m_path(fullpath), m_expand(false)
{
}

void QDirItem::SetData(EntnodeData *data)
{
	EntnodeData *oldData = m_data;
	m_data = data;

	if(m_data != 0L)
		m_data->Ref();
	if(oldData != 0L)
		oldData->UnRef();
}

void QDirItem::RemoveAllChilds()
{
	QListViewItem *item;

	while((item = firstChild()) != 0L)
	{
		removeItem(item);
	}
}

void QDirItem::setOpen(bool o)
{
	if(m_data != 0L && m_data->IsMissing())
		return;

	m_expand = o;
	RemoveAllChilds();

	if(!o)
	{
		CFillerDummy traverse(GetDirView(), 0L, this, 0L);
		/*kTraversal res = */FileTraverse(m_path, traverse);
	}
	else
	{
		CSortList<ENTNODE> entries(200, ENTNODE::Compare);
		CExpandFiller traverse(GetDirView(), 0L, this, entries);
		/*kTraversal res = */FileTraverse(m_path, traverse);

		// add the missing folders
		Entries_SetMissing(entries);
		int numEntries = entries.NumOfElements();
		for(int i = 0; i < numEntries; i++)
		{
			const ENTNODE & theNode = entries.Get(i);
			EntnodeData *data = ((ENTNODE *)&theNode)->Data();
			if(!data->IsMissing() || data->GetType() != ENT_SUBDIR)
				continue;
			
			QDirItem *item = new QDirItem(this, (*data)[EntnodeData::kName], (*data)[EntnodeData::kName]);
			item->SetData(data);
		}
	}

	QListViewItem::setOpen( o );
}

void QDirItem::setup()
{
	setExpandable(TRUE);
	QListViewItem::setup();
}

QString QDirItem::text( int column ) const
{
	QString res("");

	if ( column == 0 )
		res = m_name;
	else if(m_data != 0L)
		res = (*m_data)[EntnodeData::kStatus];
	return res;
}

QDirItem::~QDirItem()
{
	if(m_data != 0L)
		m_data->UnRef();
}

#include "qdiritem.moc"
