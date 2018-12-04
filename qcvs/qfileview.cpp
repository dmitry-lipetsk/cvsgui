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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "qfileview.h"
#include "qfileitem.h"
#include "qdirview.h"
#include "qcvsapp.h"
#include "FileTraversal.h"
#include "AppConsole.h"
#include "CvsArgs.h"

#include <qpixmap.h>
#include <qkeycode.h>

#include "state1.xpm"
#include "state2.xpm"
#include "state3.xpm"
#include "state4.xpm"
#include "state5.xpm"
#include "state6.xpm"
#include "state7.xpm"
#include "state8.xpm"
#include "state9.xpm"
#include "state10.xpm"
#include "state11.xpm"
#include "state12.xpm"
#include "state13.xpm"
#include "ro.xpm"
#include "rw.xpm"

CPersistentT<bool> gFileViewIgnore("P_FileViewIgnore", true);
CPersistentT<int> gFileDetails("P_FileDetails", 0);

#define NUM_COLUMNS	8

static char *_gszColumnLabel[NUM_COLUMNS] =
{
	"Name", "RW", "Rev.", "Option", "Status",
	"Tag", "Date", "Conflict"
};

class CViewFill : public TraversalReport
{
public:
	QFileView *m_listCtrl;
	CSortList<ENTNODE> & m_entries;
	vector<CStr> m_ignlist;

	CViewFill(QFileView *listCtrl, CSortList<ENTNODE> & entries) :
		m_listCtrl(listCtrl), m_entries(entries) {}

	virtual ~CViewFill() {}

	virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname)
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

	virtual kTraversal ExitDirectory(const char *fullpath)
	{
		m_ignlist.erase(m_ignlist.begin(), m_ignlist.end());
		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char *err, int errcode)
	{
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char *fullpath)
	{
		return kContinueTraversal;
	}

	virtual kTraversal OnDirectory(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, FSSpec * macspec)
	{
		if(stricmp(name, "CVS") == 0)
			return kSkipFile;

		EntnodeData *data = Entries_SetVisited(fullpath, m_entries, name, dir, true, &m_ignlist);
		if(!gFileViewIgnore && data->IsIgnored())
			return kSkipFile;

		// get the tag
		CStr subCVS;
		CStr tagName;
		subCVS = fullname;
		if(!subCVS.endsWith(kPathDelimiter))
			subCVS << kPathDelimiter;
		subCVS << "CVS";
		if(chdir(subCVS) == 0)
			Tag_Open(tagName, subCVS);
		if(chdir(fullpath) != 0)
			return kTraversalError;

		new QFileItem(m_listCtrl, data);

		return kSkipFile;
	}

	virtual kTraversal OnAlias(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, FSSpec * macspec)
	{
		return OnFile(fullpath, fullname, name, dir, macspec);
	}

	virtual kTraversal OnFile(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, FSSpec * macspec)
	{
		EntnodeData *data = Entries_SetVisited(fullpath, m_entries, name, dir, false, &m_ignlist);
		if(!gFileViewIgnore && data->IsIgnored())
			return kContinueTraversal;

		new QFileItem(m_listCtrl, data);

		return kContinueTraversal;
	}
};

QFileView::QFileView(QWidget *parent, const char *name) : QListView(parent, name),
	m_entries(200, ENTNODE::Compare), m_entriesMod(0), m_entriesLogMod(0)

{
	QCvsApp::qApp->SetFileView(this);

	for(int i = 0; i < NUM_COLUMNS; i++)
	{
		addColumn(_gszColumnLabel[i]);
	}
	setMultiSelection(true);
	setAllColumnsShowFocus(true);

	connect(this, SIGNAL(selectionChanged()),
			this, SLOT(OnSelectionChanged()));
	connect(this, SIGNAL(doubleClicked(QListViewItem *)),
			this, SLOT(OnOpenSelection(QListViewItem *)));
	connect(this, SIGNAL(returnPressed(QListViewItem *)),
			this, SLOT(OnOpenSelection(QListViewItem *)));

	OnFileDetails(gFileDetails);

	QDirView *dirView = QCvsApp::qApp->GetDirView();
	if(dirView != 0L)
	{
		m_path = dirView->GetRootPath();
		ResetView(true);
	}
}

QFileView::~QFileView()
{
}

QPixmap *QFileView::GetImageForEntry(EntnodeData *data)
{
	QPixmap *result;

	static QPixmap *iText = 0L;
	static QPixmap *iBin = 0L;
	static QPixmap *iUnknown = 0L;
	static QPixmap *iAppend = 0L;
	static QPixmap *iConflict = 0L;
	static QPixmap *iMissing = 0L;
	static QPixmap *iDir = 0L;
	static QPixmap *iDirUnknown = 0L;
	static QPixmap *iDirMissing = 0L;
	static QPixmap *iTextMod = 0L;
	static QPixmap *iBinMod = 0L;
	static QPixmap *iFileIgnore = 0L;
	static QPixmap *iDirIgnore = 0L;

	if(iText == 0L)
	{
		iText = new QPixmap(i_state1);
		iBin = new QPixmap(i_state2);
		iUnknown = new QPixmap(i_state3);
		iAppend = new QPixmap(i_state4);
		iConflict = new QPixmap(i_state5);
		iMissing = new QPixmap(i_state6);
		iDir = new QPixmap(i_state7);
		iDirUnknown = new QPixmap(i_state8);
		iDirMissing = new QPixmap(i_state9);
		iTextMod = new QPixmap(i_state10);
		iBinMod = new QPixmap(i_state11);
		iFileIgnore = new QPixmap(i_state12);
		iDirIgnore = new QPixmap(i_state13);
	}

	if(data->GetType() == ENT_FILE)
	{
		const char *info = 0L;
		if(data->IsIgnored())
			result = iFileIgnore;
		else if(data->IsUnknown())
			result = iUnknown;
		else if((*data)[EntnodeFile::kConflict] != 0L)
			result = iConflict;
		else if((info = (*data)[EntnodeFile::kOption]) != 0L && strcmp(info, "-kb") == 0)
			result = data->IsUnmodified() ? iBin : iBinMod;
		else
			result = data->IsUnmodified() ? iText : iTextMod;
	}
	else
	{
		if(data->IsIgnored())
			result = iDirIgnore;
		else if(data->IsUnknown())
			result = iDirUnknown;
		else
			result = iDir;
	}

	return result;
}

QPixmap *QFileView::GetImageForRW(bool isRW)
{
	static QPixmap *iRW = 0L;
	static QPixmap *iRO = 0L;

	if(iRW == 0L)
	{
		iRW = new QPixmap(i_rw);
		iRO = new QPixmap(i_ro);
	}

	return isRW ? iRW : iRO;
}

void QFileView::GetEntriesModTime(time_t & newEntriesMod, time_t & newEntriesLogMod)
{
	newEntriesMod = 0;
	newEntriesLogMod = 0;
	if(chdir(m_path) != 0)
		return;
	if(chdir("CVS") != 0)
		return;

	struct stat sb;
	if (stat("Entries", &sb) != -1)
		newEntriesMod = sb.st_mtime;
	if (stat("Entries.log", &sb) != -1)
		newEntriesLogMod = sb.st_mtime;
	chdir(m_path);
}

void QFileView::ResetView(bool forceReload, bool notifyBrowser)
{
	//CWaitCursor doWait;

	// - check if we really need to reload (forceReload == false)
	// - wait a bit for the watcher in order to let him tell us
	// if something was modified.
	time_t newEntriesMod;
	time_t newEntriesLogMod;
	GetEntriesModTime(newEntriesMod, newEntriesLogMod);
	if(!forceReload)
	{
		if(m_entriesMod == newEntriesMod && m_entriesLogMod == newEntriesLogMod
		   /* && !pathmodified(m_path) ? */)
			return;
	}

	// reset all the watchers
	m_entriesMod = newEntriesMod;
	m_entriesLogMod = newEntriesLogMod;

	// get the selection to later try to restore it
	CvsArgs selection(false);
	QFileItem * item = (QFileItem *)firstChild();
	EntnodeData *data;
	if(item != 0L) do
	{
		if(item->isSelected())
		{
			data = item->GetData();
			selection.add((*data)[EntnodeData::kName]);
		}
		item = (QFileItem *)item->nextSibling();
	} while(item != 0L);

	// delete all items
	clear();
	m_entries.Reset();

	// refetch all items
	CViewFill traverse(this, m_entries);
	/*kTraversal res = */FileTraverse(m_path, traverse);

	// add the missing files
	Entries_SetMissing(m_entries);

	int numEntries = m_entries.NumOfElements(), i;
	for(i = 0; i < numEntries; i++)
	{
		const ENTNODE & theNode = m_entries.Get(i);
		data = ((ENTNODE *)&theNode)->Data();
		if(!data->IsMissing())
			continue;

		/*item = */new QFileItem(this, data);
	}

	// now restore the selection
	int argc = selection.Argc();
	char * const *argv = selection.Argv();
	for(i = 0; i < argc; i++)
	{
		item = (QFileItem *)firstChild();

		if(item != 0L) do
		{
			data = item->GetData();
			if(strcmp((*data)[EntnodeData::kName], argv[i]) == 0)
			{
				setSelected(item, true);
			}
			item = (QFileItem *)item->nextSibling();
		} while(item != 0L);
	}

	if(notifyBrowser)
	{
		// notify the tree
		QCvsApp::qApp->GetDirView()->ResetBrowser(forceReload);
	}
}

void QFileView::ResetView(const char *path, bool notifyBrowser)
{
	m_path = path;
	ResetView(true);

	if(notifyBrowser)
	{
		// notify the tree
		QCvsApp::qApp->GetDirView()->StepToLocation(path);
	}
}

void QFileView::OnSelectionChanged ()
{
	QCvsApp::qApp->InvalidateMenus();
}

void QFileView::OnOpenSelection(QListViewItem *i)
{
	QFileItem *item = (QFileItem *)i;
	EntnodeData *data = item->GetData();
	if(data == 0L)
		return;

	CStr fullpath;
	fullpath = m_path;
	if(!fullpath.endsWith(kPathDelimiter))
		fullpath << kPathDelimiter;
	fullpath << (*data)[EntnodeData::kName];
	if(data->GetType() == ENT_SUBDIR)
	{
		ResetView(fullpath, true);
	}
	else
	{
		cvs_err("Not yet implemented !\n");
	}
}

void QFileView::OnFileDetails(int details)
{
	if(details <= 0)
		details = 0;
	else if(details >= 2)
		details = 2;

	for(int i = 0; i < NUM_COLUMNS; i++)
	{
		if(details == 0 || (details == 1 && i <= 5) ||
		   (details == 2 && i <= 2))
			setColumnText(i, _gszColumnLabel[i]);
		else
			setColumnText(i, "");
	}
	gFileDetails = details;
	QCvsApp::qApp->InvalidateMenus();
	triggerUpdate();
}

void QFileView::OnUpPath()
{
	if(chdir(m_path) == 0 && chdir("..") == 0)
	{
		char newpath[1024];
		getcwd(newpath, 1023);
		ResetView(newpath, true);
	}
}

void QFileView::mousePressEvent ( QMouseEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	if(!hasFocus())
		setFocus();

	QListView::mousePressEvent(e);
	app->SetSoftFocus(QCvsApp::kFileView);
}

void QFileView::mouseReleaseEvent ( QMouseEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::mouseReleaseEvent(e);
}

void QFileView::mouseDoubleClickEvent ( QMouseEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::mouseDoubleClickEvent(e);
}

void QFileView::focusInEvent ( QFocusEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::focusInEvent(e);
}

void QFileView::focusOutEvent ( QFocusEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::focusOutEvent(e);
}

void QFileView::keyPressEvent ( QKeyEvent * e )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;
    
	if (e != 0L && e->key() == Key_Backspace)
	{
		OnUpPath();
	}

	QListView::keyPressEvent(e);
}

void QFileView::enabledChange ( bool s )
{
	QCvsApp *app = QCvsApp::qApp;
	if(app->IsCvsLoaded())
		return;

	QListView::enabledChange(s);
}

#include "qfileview.moc"
