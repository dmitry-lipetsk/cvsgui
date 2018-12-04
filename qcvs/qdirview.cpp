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

#include "qdirview.h"
#include "qfileview.h"
#include "qdiritem.h"
#include "qcvsapp.h"

// because some actions generate some select/unselect
// event, and because we broadcast this event to the
// file view, in some occasions, we want to turn off
// the notification.
static int sTemporaryTurnOffNotify = 0;

// waouh... (egcs bug ???) We cannot have two
// static delcarations of a class with the same name
class CTempToggleBool2
{
public:
	CTempToggleBool2(int & value) : m_value(value)
	{
		m_value++;
	}

	~CTempToggleBool2()
	{
		if(m_value > 0)
			m_value--;
	}
protected:
	int & m_value;
};

static PCPStr gOldLoc("P_BrowserLoc", "");

QDirView::QDirView(QWidget *parent, const char *name) : QListView(parent, name)
{
	QCvsApp::qApp->SetDirView(this);
	m_path = (const char *)*(CPStr *)&gOldLoc;
	connect(this, SIGNAL(selectionChanged(QListViewItem *)),
			this, SLOT(OnSelectionChanged(QListViewItem *)));

	addColumn("Name");
	addColumn("Type");
	setTreeStepSize(20);
	setMultiSelection(FALSE);
	setRootIsDecorated(true);

	if(!m_path.empty())
	{
		ResetBrowser(true, true);
	}
}

QDirView::~QDirView()
{
}

void QDirView::RetrievePath(QDirItem *item, CStr & path)
{
	CStr tmp, newPath;
	path = "";
	QDirItem *parent = 0L;
	do
	{
		parent = (QDirItem *)item->parent();

		tmp = path;
		newPath = parent == 0L ? (const char *)m_path : (const char *)item->text(0);
		if(!newPath.endsWith(kPathDelimiter))
			newPath << kPathDelimiter;
		newPath << path;
		path = newPath;

		item = parent;
	} while(item != 0L);
}

void QDirView::ResetBrowser(const char *path, bool notifyView)
{
	//CWaitCursor doWait;
	CTempToggleBool2 toggler(sTemporaryTurnOffNotify);

	clear();
	m_path = path;
	*(CPStr *)&gOldLoc = m_path;

	if(path == 0L)
		return;

	CFillerDummy traverse(this, 0L, 0L, 0L);
	/*kTraversal res = */FileTraverse(m_path, traverse);

	QDirItem *root = (QDirItem *)firstChild();
	if(root != 0L)
	{
		if(root->GetData() == 0L)
		{
			CStr uppath, filename;
			SplitPath(m_path, uppath, filename);
			EntnodeData *ent = new EntnodeFolder(filename);
			root->SetData(ent);
			ent->UnRef();
		}

		root->setOpen(TRUE);
		setSelected(root, true);

		QFileView *fileView = QCvsApp::qApp->GetFileView();
		if(fileView != 0L && notifyView)
		{
			fileView->ResetView(m_path);
		}
	}
}

void QDirView::StoreExpanded(vector<CStr> & allExpanded)
{
	QDirItem *item = (QDirItem *)firstChild();
	if(item == 0L)
		return;

	CStr path;
	do
	{
		if(item->isExpanded())
		{
			RetrievePath(item, path);
			allExpanded.push_back(path);
		}
	} while((item = (QDirItem *)item->itemBelow()) != 0L);
}

QDirItem *QDirView::GetSelectedItem()
{
	QDirItem *item = (QDirItem *)firstChild();
	if(item == 0L)
		return 0L;

	do
	{
		if(item->isSelected())
			return item;
	} while((item = (QDirItem *)item->itemBelow()) != 0L);
	
	return 0L;
}

void QDirView::StepToLocation(const char *path, bool notifyView)
{
	CStr root(m_path);
	if(!root.endsWith(kPathDelimiter))
		root << kPathDelimiter;
	CStr subpath(path);
	if(!subpath.endsWith(kPathDelimiter))
		subpath << kPathDelimiter;

	// check if it is a sub-path
	if(strncmp(root, subpath, root.length()) != 0)
		return;

	QDirItem *item = (QDirItem *)firstChild();
	QDirItem *lastitem = 0L;
	if(item == 0L)
		return;

	// step inside synchronized
	const char *tmp = (const char *)subpath + root.length() - 1;
	while((tmp = strchr(tmp, kPathDelimiter)) != 0L)
	{
		const char *name = ++tmp;
		if(name[0] == '\0')
			break;

		CStr subname;
		const char *tmp2 = strchr(name, kPathDelimiter);
		if(tmp2 == 0L)
			subname = name;
		else
			subname.set(name, tmp2 - name);

		QDirItem *childItem = (QDirItem *)item->firstChild();

		// find the subitem which matches this name
		while(childItem != 0L)
		{
			EntnodeData *data = childItem->GetData();
			if(data != 0L)
			{
				if(strcmp((*data)[EntnodeData::kName], subname) == 0)
				{
					// found it !
					lastitem = item = childItem;
					item->setOpen(true);
					break;
				}
			}

			childItem = (QDirItem *)childItem->nextSibling();
		}
	}

#if 0
	// in case this is called by the view, turn off notifying
	// when the item gets selected
	if(lastitem != 0L)
	{
		if(!notifyView)
		{
			CTempToggleBool2 toggler(sTemporaryTurnOffNotify);
			setSelected(lastitem, true);
		}
		else
		{
			setSelected(lastitem, true);
		}
	}
#endif
}

QDirItem *QDirView::GetItemByLocation(const char *path)
{
	CStr root(m_path);
	if(!root.endsWith(kPathDelimiter))
		root << kPathDelimiter;
	CStr subpath(path);
	if(!subpath.endsWith(kPathDelimiter))
		subpath << kPathDelimiter;

	// check if it is a sub-path
	if(strncmp(root, subpath, root.length()) != 0)
		return 0L;

	QDirItem *item = (QDirItem *)firstChild();
	if(item == 0L)
		return 0L;

	// step inside synchronized
	const char *tmp = (const char *)subpath + root.length() - 1;
	while((tmp = strchr(tmp, kPathDelimiter)) != 0L)
	{
		const char *name = ++tmp;
		if(name[0] == '\0')
			break;

		CStr subname;
		const char *tmp2 = strchr(name, kPathDelimiter);
		if(tmp2 == 0L)
			subname = name;
		else
			subname.set(name, tmp2 - name);

		
		QDirItem *childItem = (QDirItem *)item->firstChild();

		// find the subitem which matches this name
		while(childItem != 0L)
		{
			EntnodeData *data = childItem->GetData();
			if(data != 0L)
			{
				if(strcmp((*data)[EntnodeData::kName], subname) == 0)
				{
					item = childItem;
					break;
				}
			}

			childItem = (QDirItem *)childItem->nextSibling();
		}
		if(childItem == 0L)
			return 0L;
	}

	return item;
}

void QDirView::ResetBrowser(bool forceReload, bool notifyView)
{
	//CWaitCursor doWait;
	CTempToggleBool2 toggler(sTemporaryTurnOffNotify);

	bool contentChanged = false;
	vector<CStr> allExpanded;
	StoreExpanded(allExpanded);

	CStr selPath;
	QDirItem * selItem = GetSelectedItem();
	if(selItem != 0L)
		RetrievePath(selItem, selPath);

	if(forceReload)
	{
		ResetBrowser(m_path);
		contentChanged = true;
	}

	if(contentChanged)
	{
		// restore the expanded paths and the selected item
		vector<CStr>::const_iterator i;
		for(i = allExpanded.begin(); i != allExpanded.end(); ++i)
		{
			StepToLocation(*i);
		}
		if(!selPath.empty())
		{
			selItem = GetItemByLocation(selPath);
			if(selItem != 0L)
				setSelected(selItem, true);
		}
	}

	if(notifyView)
	{
		QFileView *fileView = QCvsApp::qApp->GetFileView();
		if(fileView != 0L)
			fileView->ResetView(forceReload);
	}
}

void QDirView::OnSelectionChanged (QListViewItem *i)
{
	if(i == 0L)
		return;

	QDirItem *item = (QDirItem *)i;
	EntnodeData *data = item->GetData();
	if(data != 0L && data->IsMissing())
		return;

	if(!sTemporaryTurnOffNotify)
	{
		CStr path;
		RetrievePath(item, path);
		QFileView *fileView = QCvsApp::qApp->GetFileView();
		if(fileView != 0L)
			fileView->ResetView(path);
	}

	QCvsApp::qApp->InvalidateMenus();
}

void QDirView::mousePressEvent ( QMouseEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	if(!hasFocus())
		setFocus();

	QListView::mousePressEvent(e);
	app->SetSoftFocus(QCvsApp::kDirView);
}

void QDirView::mouseReleaseEvent ( QMouseEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::mouseReleaseEvent(e);
}

void QDirView::mouseDoubleClickEvent ( QMouseEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::mouseDoubleClickEvent(e);
}

void QDirView::focusInEvent ( QFocusEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::focusInEvent(e);
}

void QDirView::focusOutEvent ( QFocusEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::focusOutEvent(e);
}

void QDirView::keyPressEvent ( QKeyEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::keyPressEvent(e);
}

void QDirView::enabledChange ( bool s )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::enabledChange(s);
}

#include "qdirview.moc"
