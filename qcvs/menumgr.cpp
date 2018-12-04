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

#include "menumgr.h"
#include "qcvsapp.h"
#include "AppConsole.h"
#include "CPStr.h"
#include "CvsCommands.h"
#include "GetPrefs.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "PromptFiles.h"
#include "MacrosSetup.h"
#include "TclGlue.h"

#include "qfileview.h"
#include "qdirview.h"
#include "qdiritem.h"
#include "qfileitem.h"

#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qwhatsthis.h>
#include <qmainwindow.h>
#include <qfiledialog.h>

#include <string.h>

#include "savefile.xpm"
#include "cut.xpm"
#include "copy.xpm"
#include "paste.xpm"
#include "print.xpm"

#include "add.xpm"
#include "addb.xpm"
#include "commit.xpm"
#include "details1.xpm"
#include "details2.xpm"
#include "details3.xpm"
#include "diff.xpm"	
#include "explore.xpm"
#include "fileopen.xpm"
#include "graph.xpm"	
#include "ignore.xpm"	
#include "locate.xpm"	
#include "log.xpm"
#include "reload.xpm"
#include "remove.xpm"
#include "status.xpm"
#include "stop.xpm"
#include "trash.xpm"
#include "update.xpm"
#include "lock.xpm"
#include "unlock.xpm"
#include "edit.xpm"
#include "unedit.xpm"
#include "watchon.xpm"
#include "watchoff.xpm"
#include "release.xpm"
#include "tag.xpm"
#include "untag.xpm"
#include "branch.xpm"
#include "upfolder.xpm"

static bool sUpdateMenusCheck = false;

class CTempToggleBool
{
public:
	CTempToggleBool(bool & value) : m_value(value)
	{
		m_value = !m_value;
	}

	~CTempToggleBool()
	{
		m_value = !m_value;
	}
protected:
	bool & m_value;
};

// this class remembers the selection in the file
// list or the browser list (depending on the focus)
// and allows the menu entries to enable/disable properly
class CBrowserEnt
{
public:
	CBrowserEnt();
	CBrowserEnt(const char *path, EntnodeData *data);
	CBrowserEnt(const CBrowserEnt & ent);
	~CBrowserEnt();

	CBrowserEnt & operator=(const CBrowserEnt & ent);

	CStr fPath;
	EntnodeData *fData;
	
	static int numFiles;
	static int numFolders;
	static int numMissing;
	static int numUnknowns;
	static int numTotal;

	typedef enum {
		fromDirView,
		fromFileView
	} fromKind;

	static fromKind fromWhere;

	static MultiFiles *GetMultiFiles(const vector<CBrowserEnt> & entries);
	static MultiFiles mf;

	const char *FullPath() const
	{
		static CStr result;
		result = fPath;
		if(!result.endsWith(kPathDelimiter))
			result << kPathDelimiter;
		result << (*fData)[EntnodeData::kName];
		return result;
	}
};

int CBrowserEnt::numFiles = 0;
int CBrowserEnt::numFolders = 0;
int CBrowserEnt::numMissing = 0;
int CBrowserEnt::numUnknowns = 0;
int CBrowserEnt::numTotal = 0;
CBrowserEnt::fromKind CBrowserEnt::fromWhere = CBrowserEnt::fromDirView;
MultiFiles CBrowserEnt::mf;

CBrowserEnt::CBrowserEnt() : fData(0L)
{
}

CBrowserEnt::CBrowserEnt(const char *path, EntnodeData *data) : fPath(path)
{
	fData = data->Ref();
}

CBrowserEnt::CBrowserEnt(const CBrowserEnt & ent) : fData(0L)
{
	*this = ent;
}

CBrowserEnt::~CBrowserEnt()
{
	if(fData != 0L)
		fData->UnRef();
}

CBrowserEnt & CBrowserEnt::operator=(const CBrowserEnt & ent)
{
	if(fData != 0L)
	{
		fData->UnRef();
		fData = 0L;
	}
	fData = ent.fData != 0L ? ent.fData->Ref() : 0L;
	fPath = ent.fPath;
	return *this;
}

MultiFiles *CBrowserEnt::GetMultiFiles(const vector<CBrowserEnt> & entries)
{
	mf.reset();
	mf.newdir(QCvsApp::qApp->GetFileView()->GetPath());
	vector<CBrowserEnt>::const_iterator i;
	for(i = entries.begin(); i != entries.end(); ++i)
	{
		mf.newfile((*(*i).fData)[EntnodeData::kName]);
	}
	return &mf;
}

static vector<CBrowserEnt> sCurEntries;

vector<QMenuSlot *> QMenuSlot::allslots;

QMenuSlot::QMenuSlot(CommandLaunchHook launch, CommandUpdateHook update) :
		id(-1), genericid(-1), button(0L), menu(0L), launchCmd(launch), updateCmd(update),
		isactivated(false), isenabled(true)
{
	allslots.push_back(this);
}

QMenuSlot::~QMenuSlot()
{	
}

QMenuSlot *QMenuSlot::current = 0L;

void QMenuSlot::cmd()
{
	UpdateCmd();
	if(!isenabled)
	{
#if qCvsDebug
		cvs_err("A command was launched but should be disabled\n");
#endif
		return;
	}

	current = this;
	if(launchCmd != 0L)
	{
		emit cmdIsAboutToLaunch(this);

		launchCmd();
	}
#if qCvsDebug
	else
		cvs_err("A command was launched but has not entry\n");
#endif
	current = 0L;

	UpdateCmd();
}

void QMenuSlot::SetActivated(bool state)
{
	if(!(isactivated ^ state))
		return;

	isactivated = state;
	if(button != 0L)
		button->setOn(state);
	if(menu != 0L)
		menu->setItemChecked(id, state);
#if qCvsDebug
	if(button == 0L && menu == 0L)
		cvs_err("A slot get activated but has nothing to activate\n");
#endif
}

void QMenuSlot::SetEnabled(bool state)
{
	if(!(isenabled ^ state))
		return;

	isenabled = state;
	if(button != 0L)
		button->setEnabled(state);
	if(menu != 0L)
		menu->setItemEnabled(id, state);
#if qCvsDebug
	if(button == 0L && menu == 0L)
		cvs_err("A slot get enabled but has nothing to enable\n");
#endif
}

void QMenuSlot::UpdateCmd()
{
	current = this;
	MenuCommand cmd = {isenabled, isactivated};
	if(updateCmd != 0L)
		updateCmd(cmd);
	current = 0L;

	if(cmd.isEnabled ^ isenabled)
		SetEnabled(cmd.isEnabled);
	if(cmd.isActivated ^ isactivated)
		SetActivated(cmd.isActivated);
}

QMenuSlot *QMenuSlot::Get(int genericid)
{
	vector<QMenuSlot *>::iterator i;
	for(i = QMenuSlot::allslots.begin(); i != QMenuSlot::allslots.end(); ++i)
	{
		if((*i)->genericid == genericid)
			return *i;
	}
#if qCvsDebug
	cvs_err("Unknwon generic ID %d!\n", genericid);
#endif
	return 0L;
}

CCvsMenuBar::CCvsMenuBar(QMenuBar *menuBar) : fQTMenuBar(menuBar)
{
}

CCvsMenuBar::~CCvsMenuBar()
{
}

CCvsMenu::CCvsMenu(CCvsMenuBar & bar, const char *title) : fCheckable(false)
{
    fQTMenu = new QPopupMenu();
	bar.fQTMenuBar->insertItem(title, fQTMenu);
}

CCvsMenu::CCvsMenu(CCvsMenu & parent, const char *title) : fCheckable(false)
{
    fQTMenu = new QPopupMenu();
	parent.fQTMenu->insertItem(title, fQTMenu);
}

CCvsMenu::~CCvsMenu()
{
}

CCvsMenuItem::CCvsMenuItem(CCvsMenu & menu, const char *txt, int keystroke,
				 CommandLaunchHook launchCmd,
				 CommandUpdateHook updateCmd,
				 QToolBar *toolBar,
				 const char *helpString,
				 const char **icon,
				 kItemKind kind = kItemNormal)
{
    QPixmap theIcon;

	if(icon != 0L)
		theIcon = QPixmap(icon);

	slot = new QMenuSlot(launchCmd, updateCmd);
	slot->menu = menu.fQTMenu;
	if(icon != 0L)
		slot->id = menu.fQTMenu->insertItem(theIcon, txt, slot, SLOT(cmd()), keystroke);
	else
		slot->id = menu.fQTMenu->insertItem(txt, slot, SLOT(cmd()), keystroke);

	if(toolBar != 0L && icon != 0L)
	{
		CStr newstr(txt);
		char *amper = strchr(newstr, '&');
		if(amper != 0L)
		{
			memmove(amper, amper + 1, strlen(amper) * sizeof(char));
		}
		slot->button = new QToolButton(theIcon, QString(newstr), 0L, slot, SLOT(cmd()), toolBar, txt);
		if(helpString != 0L)
		{
			QWhatsThis::add(slot->button, QString(helpString));
		}
	}
	if(kind & kItemActivable)
	{
		if(!menu.fCheckable)
		{
			menu.fCheckable = true;
			menu.fQTMenu->setCheckable(TRUE);
		}
		menu.fQTMenu->setItemChecked(slot->id, false);
		if(slot->button != 0L)
		{
			slot->button->setToggleButton(true);
			slot->button->setOn(false);
		}
	}
}

CCvsMenuItem::CCvsMenuItem(CCvsMenu & menu, const char *txt, CCvsMenu & subMenu, const char *icon)
{
	if(icon != 0L)
		menu.fQTMenu->insertItem(QPixmap(icon), QString(txt), subMenu.fQTMenu);
	else
		menu.fQTMenu->insertItem(QString(txt), subMenu.fQTMenu);
}

CCvsMenuItem::CCvsMenuItem(CCvsMenu & menu)
{
    menu.fQTMenu->insertSeparator();
}

CCvsMenuItem::~CCvsMenuItem()
{
}

static void UpdateLocate(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
}

static void LaunchLocate()
{
	const char * dir = BrowserGetDirectory("Choose a CVS directory to browse :");
	if (dir != 0L)
	{
		QCvsApp::qApp->GetDirView()->ResetBrowser(dir);
	}
}

static void UpdateQuit(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
}

static void LaunchQuit()
{
	gCvsPrefs.save();	
	qApp->quit();
}

static void UpdateAdd(MenuCommand & cmd)
{
	cmd.isEnabled = CBrowserEnt::numTotal == CBrowserEnt::numUnknowns && CBrowserEnt::numUnknowns > 0;
}

static void LaunchAdd()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdAddFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdAddFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchAddb()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdAddFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdAddBinaryFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchCommit()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdCommitFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdCommitFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void UpdateDetails1(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
	cmd.isActivated = QCvsApp::qApp->GetFileView()->GetFileDetails() == 2;
}

static void LaunchDetails1()
{
	QCvsApp::qApp->GetFileView()->OnFileDetails(2);
}

static void UpdateDetails2(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
	cmd.isActivated = QCvsApp::qApp->GetFileView()->GetFileDetails() == 1;
}

static void LaunchDetails2()
{
	QCvsApp::qApp->GetFileView()->OnFileDetails(1);
}

static void UpdateDetails3(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
	cmd.isActivated = QCvsApp::qApp->GetFileView()->GetFileDetails() == 0;
}

static void LaunchDetails3()
{
	QCvsApp::qApp->GetFileView()->OnFileDetails(0);
}

static void LaunchDiff()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdDiffFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdDiffFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void UpdateExplore(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
}

static void LaunchExplore()
{
	cvs_err("Not yet implemented !\n");
}

static void UpdateRelease(MenuCommand & cmd)
{
	cmd.isEnabled = CBrowserEnt::numFiles == 0 &&
		CBrowserEnt::numFolders > 0 &&
		CBrowserEnt::numUnknowns == 0 &&
		CBrowserEnt::numMissing == 0;
}

static void LaunchGraph()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdLogFiles(CBrowserEnt::GetMultiFiles(sCurEntries), true);
	}
}

static void UpdateIgnore(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded() && !gCvsPrefs.empty();
	cmd.isActivated = gFileViewIgnore;
}

static void LaunchIgnore()
{
	gFileViewIgnore = !(bool)gFileViewIgnore;
	QCvsApp::qApp->GetDirView()->ResetBrowser(true, true);	
}

static void LaunchLog()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdLogFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdLogFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void UpdateReload(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded() && !gCvsPrefs.empty();
}

static void LaunchReload()
{
	QCvsApp::qApp->GetDirView()->ResetBrowser(true, true);
}

static void UpdateRemove(MenuCommand & cmd)
{
	cmd.isEnabled = CBrowserEnt::numTotal > 0 && CBrowserEnt::numFolders == 0 &&
		CBrowserEnt::numUnknowns == 0;
}

static void LaunchRemove()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdRemoveFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchStatus()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdStatusFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdStatusFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void UpdateStop(MenuCommand & cmd)
{
	cmd.isEnabled = QCvsApp::qApp->IsCvsLoaded() && !QCvsApp::qApp->IsCvsStopping();
}

static void LaunchStop()
{
	if(QCvsApp::qApp->IsCvsStopping())
	{
		cvs_err("cvs is already scheduled to stop, please wait...\n");
	}
	else
		QCvsApp::qApp->SetCvsStopping(true);
}

static void UpdateTrash(MenuCommand & cmd)
{
	cmd.isEnabled = CBrowserEnt::numTotal > 0 && CBrowserEnt::numFolders == 0 &&
		CBrowserEnt::numMissing == 0;
}

static void LaunchTrash()
{
	cvs_err("Not yet implemented !\n");
}

static void UpdateUpdate(MenuCommand & cmd)
{
	cmd.isEnabled = CBrowserEnt::numTotal > 0 && CBrowserEnt::numUnknowns == 0;
}

static void LaunchUpdate()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdUpdateFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdUpdateFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchLock()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdLockFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdLockFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchUnlock()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdUnlockFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdUnlockFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchWatchOn()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdWatchOnFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdWatchOnFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchWatchOff()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdWatchOffFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdWatchOffFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchEdit()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdEditFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdEditFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchUnedit()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdUneditFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdUneditFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchWatchers()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdWatchersFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdWatchersFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchTagNew()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdTagCreateFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdTagCreateFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchTagDelete()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdTagDeleteFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdTagDeleteFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchTagBranch()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdTagBranchFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdTagBranchFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void LaunchRelease()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdReleaseFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdReleaseFolder(sCurEntries[i].FullPath());
	}
}

static void LaunchEditors()
{
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			CvsCmdEditorsFolder(sCurEntries[i].fPath);
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		CvsCmdEditorsFiles(CBrowserEnt::GetMultiFiles(sCurEntries));
	}
}

static void UpdateUpfolder(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
}

static void LaunchUpfolder()
{
	QCvsApp::qApp->GetFileView()->OnUpPath();
}

static void MainUpdate(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded() && !gCvsPrefs.empty();
}

static void PreferencesUpdate(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded();
}

static void OnAppLogGraph()
{
	CvsCmdLogFiles(0L, true);
}

static void OnAppUpdatef() 
{
	CvsCmdUpdateFiles();
}

static void OnAppCommitf() 
{
	CvsCmdCommitFiles();
}

static void OnAppDiff() 
{
	CvsCmdDiffFiles();
}

static void OnAppLog() 
{
	CvsCmdLogFiles();
}

static void OnAppStatus() 
{
	CvsCmdStatusFiles();
}

static void OnAppCancelc() 
{
	CvsCmdCancelChangesFiles();
}

static void OnAppLockf() 
{
	CvsCmdLockFiles();
}

static void OnAppUnlockf() 
{
	CvsCmdUnlockFiles();
}

static void OnAppEditf() 
{
	CvsCmdEditFiles();
}

static void OnAppUneditf() 
{
	CvsCmdUneditFiles();
}

static void OnAppWatchf() 
{
	CvsCmdWatchOnFiles();
}

static void OnAppUnwatchf() 
{
	CvsCmdWatchOffFiles();
}

static void OnAppWatchersf() 
{
	CvsCmdWatchersFiles();
}

static void OnAppEditorsf() 
{
	CvsCmdEditorsFiles();
}

static void OnAppAddf() 
{
	CvsCmdAddFiles();
}

static void OnAppAddb() 
{
	CvsCmdAddBinaryFiles();
}

static void OnAppRemovef() 
{
	CvsCmdRemoveFiles();	
}

static void OnAppTagNewf()
{
	CvsCmdTagCreateFiles();
}

static void OnAppTagDeletef()
{
	CvsCmdTagDeleteFiles();
}

static void OnAppTagBranchf()
{
	CvsCmdTagBranchFiles();
}

static void OnCvsUpdate() 
{
	CvsCmdUpdateFolder();
}

static void OnAppCommit() 
{
	CvsCmdCommitFolder();
}

static void OnAppAdd() 
{
	CvsCmdAddFolder();
}

static void OnAppDifff() 
{
	CvsCmdDiffFolder();
}

static void OnAppLogf() 
{
	CvsCmdLogFolder();
}

static void OnAppStatusfo() 
{
	CvsCmdStatusFolder();
}

static void OnAppLock() 
{
	CvsCmdLockFolder();
}

static void OnAppUnlock() 
{
	CvsCmdUnlockFolder();
}

static void OnAppEdit() 
{
	CvsCmdEditFolder();
}

static void OnAppUnedit() 
{
	CvsCmdUneditFolder();
}

static void OnAppWatch() 
{
	CvsCmdWatchOnFolder();
}

static void OnAppUnwatch() 
{
	CvsCmdWatchOffFolder();
}

static void OnAppRelease() 
{
	CvsCmdReleaseFolder();
}

static void OnAppWatchers()
{
	CvsCmdWatchersFolder();
}

static void OnAppEditors() 
{
	CvsCmdEditorsFolder();
}

static void OnAppTagNew()
{
	CvsCmdTagCreateFolder();
}

static void OnAppTagDelete()
{
	CvsCmdTagDeleteFolder();
}

static void OnAppTagBranch()
{
	CvsCmdTagBranchFolder();
}

static void MacroAdminLaunch()
{
	CMacroEntry & entry = gMacrosAdmin.entries[QMenuSlot::current->genericid - (int)iMacrosAdmin];
	CvsCmdMacrosAdmin(entry.path);
}

static void MacroAdminUpdate(MenuCommand & cmd)
{
	cmd.isEnabled = !QCvsApp::qApp->IsCvsLoaded() && !gCvsPrefs.empty();
}

static void MacroSelLaunch()
{
	TclBrowserReset();
	if(CBrowserEnt::fromWhere == CBrowserEnt::fromDirView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
		{
			CStr uppath, filename;
			SplitPath(sCurEntries[i].fPath, uppath, filename);
			TclBrowserAppend(uppath, sCurEntries[i].fData);
		}
	}
	else if(CBrowserEnt::fromWhere == CBrowserEnt::fromFileView)
	{
		for(size_t i = 0; i < sCurEntries.size(); i++)
			TclBrowserAppend(sCurEntries[i].fPath, sCurEntries[i].fData);
	}

	CTcl_Interp interp;
	CMacroEntry & entry = gMacrosSel.entries[QMenuSlot::current->genericid - (int)iMacrosSel];
	CStr path = entry.path;
	CTcl_Interp::Unixfy(path);
	
	interp.DoScriptVar("source \"%s\"", (const char *)path);
}

static void MacroSelUpdate(MenuCommand & cmd)
{
	cmd.isEnabled = CBrowserEnt::numTotal > 0;
}

static void launchImport()
{
	CvsCmdImportModule();
}

static void launchCheckout()
{
	CvsCmdCheckoutModule();
}

void InstallMenus(QMainWindow *mainWindow)
{
   	CCvsMenuBar bar(mainWindow->menuBar());

	QToolBar *mainBar = new QToolBar("Main toolbar", mainWindow);
	QToolBar *selBar = new QToolBar("Selection toolbar", mainWindow);
	QToolBar *monitorBar = new QToolBar("Monitor toolbar", mainWindow, QMainWindow::Top, true);
	QToolBar *tagBar = new QToolBar("Tag toolbar", mainWindow);
	vector<CMacroEntry>::const_iterator i;
	int cnt;

	CCvsMenu fileMenu(bar, "&File");
	CCvsMenuItem f1(fileMenu, "&Locate browser...", Qt::CTRL+Qt::Key_O, LaunchLocate, UpdateLocate, mainBar, "Select the browser location", i_locate);
	CCvsMenuItem f2(fileMenu, "&Up folder", Qt::CTRL+Qt::Key_Up, LaunchUpfolder, UpdateUpfolder, mainBar, "Go one step up in the browser hierarchy", i_upfolder);
	mainBar->addSeparator();
	CCvsMenuItem f3(fileMenu);
	CCvsMenu detailsMenu(fileMenu, "&View");
	CCvsMenuItem v1(detailsMenu, "Show ignored files", 0, LaunchIgnore, UpdateIgnore, mainBar, "Show/Hide the ignored files", i_ignore, kItemActivable);
	CCvsMenuItem v2(detailsMenu);
	CCvsMenuItem v3(detailsMenu, "View details 1", 0, LaunchDetails1, UpdateDetails1, mainBar, "Details1", i_details1, kItemActivable);
	CCvsMenuItem v4(detailsMenu, "View details 2", 0, LaunchDetails2, UpdateDetails2, mainBar, "Details2", i_details2, kItemActivable);
	CCvsMenuItem v5(detailsMenu, "View details 3", 0, LaunchDetails3, UpdateDetails3, mainBar, "Details3", i_details3, kItemActivable);
	mainBar->addSeparator();
	CCvsMenuItem f4(fileMenu);
	CCvsMenuItem f5(fileMenu, "&Quit", Qt::CTRL+Qt::Key_Q, LaunchQuit, UpdateQuit);
	f5.SetGenericId(iQuit);

	CCvsMenu adminMenu(bar, "Cvs &Admin");
	CCvsMenuItem a1(adminMenu, "&Import module...", 0, launchImport, MainUpdate);
	CCvsMenuItem a2(adminMenu, "Check&out module...", 0, launchCheckout, MainUpdate);
	CCvsMenuItem a3(adminMenu);
	CCvsMenu tagadminMenu(adminMenu, "&Tag module");
	CCvsMenuItem t1(tagadminMenu, "&Create a tag...", 0, CvsCmdRtagCreate, MainUpdate);
	CCvsMenuItem t2(tagadminMenu, "&Delete a tag...", 0, CvsCmdRtagDelete, MainUpdate);
	CCvsMenuItem t3(adminMenu);
	CCvsMenuItem t4(tagadminMenu, "Create a &branch...", 0, CvsCmdRtagBranch, MainUpdate);
	CCvsMenu macroAdminMenu(adminMenu, "Macros admin");
	for(cnt = 0, i = gMacrosAdmin.entries.begin(); i != gMacrosAdmin.entries.end(); ++i, ++cnt)
	{
		CCvsMenuItem mac(macroAdminMenu, (*i).name, 0, MacroAdminLaunch, MacroAdminUpdate);
		mac.SetGenericId((int)iMacrosAdmin + cnt);
	}	
	CCvsMenuItem a4(adminMenu);
	CCvsMenuItem a5(adminMenu, "Pr&eferences...", Qt::CTRL+Qt::Key_F1, CompatGetPrefs, PreferencesUpdate);
	CCvsMenuItem a6(adminMenu, "&Command Line...", 0, CvsCmdLine, MainUpdate);
	CCvsMenuItem a7(adminMenu);
	CCvsMenuItem a8(adminMenu, "&Login...", 0, CvsCmdLogin, MainUpdate);
	CCvsMenuItem a9(adminMenu, "Logou&t", 0, CvsCmdLogout, MainUpdate);
	CCvsMenuItem a10(adminMenu);
	CCvsMenuItem a11(adminMenu, "&Stop cvs", 0, LaunchStop, UpdateStop, mainBar, "Stop cvs when running", i_stop);

	CCvsMenu filesMenu(bar, "Cvs F&iles");
	CCvsMenuItem fi1(filesMenu, "&Update files...", 0, OnAppUpdatef, MainUpdate);
	CCvsMenuItem fi2(filesMenu, "Co&mmit files...", 0, OnAppCommitf, MainUpdate);
	CCvsMenuItem fi3(filesMenu);
	CCvsMenuItem fi31(filesMenu, "&Add files...", 0, OnAppAddf, MainUpdate);
	CCvsMenuItem fi32(filesMenu, "Add &binary files...", 0, OnAppAddb, MainUpdate);
	CCvsMenuItem fi33(filesMenu, "&Remove files...", 0, OnAppRemovef, MainUpdate);
	CCvsMenuItem fi34(filesMenu);
	CCvsMenuItem fi4(filesMenu, "&Diff files...", 0, OnAppDiff, MainUpdate);
	CCvsMenuItem fi5(filesMenu, "&Log files...", 0, OnAppLog, MainUpdate);
	CCvsMenuItem fi6(filesMenu, "&Status files...", 0, OnAppStatus, MainUpdate);
	CCvsMenuItem fi7(filesMenu, "Graph files...", 0, OnAppLogGraph, MainUpdate);
	CCvsMenuItem fi8(filesMenu);
	CCvsMenuItem fi9(filesMenu, "&Cancel changes...", 0, OnAppCancelc, MainUpdate);
	CCvsMenuItem fia(filesMenu);
	CCvsMenu monitorfilesMenu(filesMenu, "M&onitors files");
	CCvsMenuItem m1(monitorfilesMenu, "L&ock files...", 0, OnAppLockf, MainUpdate);
	CCvsMenuItem m2(monitorfilesMenu, "Unloc&k files...", 0, OnAppUnlockf, MainUpdate);
	CCvsMenuItem m3(monitorfilesMenu, "&Edit files...", 0, OnAppEditf, MainUpdate);
	CCvsMenuItem m4(monitorfilesMenu, "Unedi&t files...", 0, OnAppUneditf, MainUpdate);
	CCvsMenuItem m5(monitorfilesMenu, "Add &Watch on files...", 0, OnAppWatchf, MainUpdate);
	CCvsMenuItem m6(monitorfilesMenu, "Remove Watch from &files...", 0, OnAppUnwatchf, MainUpdate);
	CCvsMenuItem m7(monitorfilesMenu);
	CCvsMenuItem m8(monitorfilesMenu, "Watchers of files...", 0, OnAppWatchersf, MainUpdate);
	CCvsMenuItem m9(monitorfilesMenu, "Editors of files...", 0, OnAppEditorsf, MainUpdate);
	CCvsMenu tagfilesMenu(filesMenu, "&Tag files");
	CCvsMenuItem tf1(tagfilesMenu, "&Create a tag...", 0, OnAppTagNewf, MainUpdate);
	CCvsMenuItem tf2(tagfilesMenu, "&Delete a tag...", 0, OnAppTagDeletef, MainUpdate);
	CCvsMenuItem tf3(tagfilesMenu);
	CCvsMenuItem tf4(tagfilesMenu, "Create a &branch...", 0, OnAppTagBranchf, MainUpdate);
	CCvsMenu macroFilesMenu(filesMenu, "Macros files");

	CCvsMenu folderMenu(bar, "Cvs F&olders");
	CCvsMenuItem fo1(folderMenu, "&Update folder...", Qt::CTRL+Qt::Key_U, OnCvsUpdate, MainUpdate);
	CCvsMenuItem fo2(folderMenu, "Co&mmit folder...", Qt::CTRL+Qt::Key_M, OnAppCommit, MainUpdate);
	CCvsMenuItem fo3(folderMenu);
	CCvsMenuItem fo4(folderMenu, "&Add a folder...", 0, OnAppAdd, MainUpdate);
	CCvsMenuItem fo5(folderMenu);
	CCvsMenuItem fo6(folderMenu, "&Diff a folder...", 0, OnAppDifff, MainUpdate);
	CCvsMenuItem fo7(folderMenu, "&Log a folder...", 0, OnAppLogf, MainUpdate);
	CCvsMenuItem fo8(folderMenu, "&Status a folder...", 0, OnAppStatusfo, MainUpdate);
	CCvsMenuItem fo9(folderMenu);
	CCvsMenu foldermonitorMenu(folderMenu, "Monitors folder");
	CCvsMenuItem fom1(foldermonitorMenu, "L&ock a folder...", 0, OnAppLock, MainUpdate);
	CCvsMenuItem fom2(foldermonitorMenu, "Unloc&k a folder...", 0, OnAppUnlock, MainUpdate);
	CCvsMenuItem fom3(foldermonitorMenu, "&Edit a folder...", 0, OnAppEdit, MainUpdate);
	CCvsMenuItem fom4(foldermonitorMenu, "U&nedit a folder...", 0, OnAppUnedit, MainUpdate);
	CCvsMenuItem fom5(foldermonitorMenu, "Add &Watch on a folder...", 0, OnAppWatch, MainUpdate);
	CCvsMenuItem fom6(foldermonitorMenu, "Remove a Wat&ch from a folder...", 0, OnAppUnwatch, MainUpdate);
	CCvsMenuItem fom7(foldermonitorMenu);
	CCvsMenuItem fom8(foldermonitorMenu, "&Release a folder...", 0, OnAppRelease, MainUpdate);
	CCvsMenuItem fom9(foldermonitorMenu);
	CCvsMenuItem foma(foldermonitorMenu, "Watchers of &folder...", 0, OnAppWatchers, MainUpdate);
	CCvsMenuItem fomb(foldermonitorMenu, "Ed&itors of folder...", 0, OnAppEditors, MainUpdate);
	CCvsMenu foldertagMenu(folderMenu, "Tag folder");
	CCvsMenuItem ft1(foldertagMenu, "&Create a tag...", 0, OnAppTagNew, MainUpdate);
	CCvsMenuItem ft2(foldertagMenu, "&Delete a tag...", 0, OnAppTagDelete, MainUpdate);
	CCvsMenuItem ft3(foldertagMenu);
	CCvsMenuItem ft4(foldertagMenu, "Create a &branch...", 0, OnAppTagBranch, MainUpdate);

	CCvsMenu selMenu(bar, "Selection&s");
	CCvsMenuItem s1(selMenu, "&Update selection", Qt::CTRL+Qt::SHIFT+Qt::Key_U, LaunchUpdate, UpdateUpdate, selBar,
					"Update the selection with the remote repository version", i_update);
	CCvsMenuItem s2(selMenu, "Co&mmit selection...", Qt::CTRL+Qt::SHIFT+Qt::Key_M, LaunchCommit, UpdateUpdate, selBar,
					"Commit the selection in the remote repository", i_commit);
	CCvsMenuItem s3(selMenu);
	selBar->addSeparator();
	CCvsMenuItem s4(selMenu, "&Add selection", 0, LaunchAdd, UpdateAdd, selBar,
					"Add the selection to the module, after use commit to permanetly add it", i_add);
	CCvsMenuItem s5(selMenu, "Add selection &binary", 0, LaunchAddb, UpdateAdd, selBar,
					"Add the binary selection to the module, after use commit to permanetly add it", i_addb);
	CCvsMenuItem s6(selMenu, "&Remove selection", 0, LaunchRemove, UpdateRemove, selBar,
					"Remove the selection from the repository, after use commit to permanently remove it", i_remove);
	selBar->addSeparator();
	CCvsMenuItem s7(selMenu);
	CCvsMenuItem s8(selMenu);
	CCvsMenuItem s9(selMenu, "R&eload view", Qt::Key_F5, LaunchReload, UpdateReload, selBar,
					"Reload the view", i_reload);
	CCvsMenuItem sa(selMenu, "E&xplore selection", Qt::Key_F2, LaunchExplore, UpdateExplore, selBar,
					"Explore the selection", i_explore);
	CCvsMenuItem sb(selMenu);
	selBar->addSeparator();
	CCvsMenuItem sc(selMenu, "Trash &selection", Qt::Key_Delete, LaunchTrash, UpdateTrash, selBar,
					"Trash the selection, do not touch the repository", i_trash);
	selBar->addSeparator();
	CCvsMenuItem sd(selMenu);
	CCvsMenuItem se(selMenu, "&Diff selection", 0, LaunchDiff, UpdateUpdate, selBar,
					"Print a diff for the selection with an arbitrary revision of the remote repository", i_diff);
	CCvsMenuItem sf(selMenu, "&Log selection", 0, LaunchLog, UpdateUpdate, selBar,
					"Print the history for the selection", i_log);
	CCvsMenuItem sg(selMenu, "Status sele&ction", 0, LaunchStatus, UpdateUpdate, selBar,
					"Print the status for he selection", i_status);
	CCvsMenuItem sh(selMenu, "Graph selection", 0, LaunchGraph, UpdateRemove, selBar,
					"Output the history of the selection as a graphic tree", i_graph);
	CCvsMenuItem si(selMenu);
	CCvsMenu menuMonitorsel(selMenu, "Monitors selection");
	CCvsMenuItem ms1(menuMonitorsel, "Lock selection", 0, LaunchLock, UpdateUpdate, monitorBar,
					"Lock the selection on the repository so nobody will be able to commit on it", i_lock);
	CCvsMenuItem ms2(menuMonitorsel, "Unlock selection", 0, LaunchUnlock, UpdateUpdate, monitorBar,
					"Give-up the lock on the selection", i_unlock);
	CCvsMenuItem ms3(menuMonitorsel, "Edit selection", 0, LaunchEdit, UpdateUpdate, monitorBar,
					"Let know to others users that you are currently editing the selection", i_edit);
	CCvsMenuItem ms4(menuMonitorsel, "Unedit selection", 0, LaunchUnedit, UpdateUpdate, monitorBar,
					"Use if you are not editing anymore the selection", i_unedit);
	CCvsMenuItem ms5(menuMonitorsel, "Add a Watch on selection", 0, LaunchWatchOn, UpdateUpdate, monitorBar,
					"Let you know automatically when the state change on the repository for this selection", i_watchon);
	CCvsMenuItem ms6(menuMonitorsel, "Remove a watch from selection", 0, LaunchWatchOff, UpdateUpdate, monitorBar,
					"Give-up the watcher on this selection", i_watchoff);
	CCvsMenuItem ms7(menuMonitorsel);
	CCvsMenuItem ms8(menuMonitorsel, "Release selection", 0, LaunchRelease, UpdateRelease, monitorBar,
					"Give-up all the edited files", i_release);
	CCvsMenuItem ms9(menuMonitorsel);
	CCvsMenuItem msa(menuMonitorsel, "Watchers of selection", 0, LaunchWatchers, UpdateUpdate);
	CCvsMenuItem msb(menuMonitorsel, "Editors of selection", 0, LaunchEditors, UpdateUpdate);
	CCvsMenu menuTagsel(selMenu, "Tag selection");
	CCvsMenuItem ts1(menuTagsel, "&Create a tag...", 0, LaunchTagNew, UpdateUpdate, tagBar,
					"Create a tag in the repository using the revision(s) and file(s) from the selection", i_tag);
	CCvsMenuItem ts2(menuTagsel, "&Delete a tag...", 0, LaunchTagDelete, UpdateUpdate, tagBar,
					"Delete a tag/branch name from the repository by using the files included in the selection", i_untag);
	CCvsMenuItem ts3(menuTagsel);
	CCvsMenuItem ts4(menuTagsel, "Create a &branch...", 0, LaunchTagBranch, UpdateUpdate, tagBar,
					"Create a branch in the repository using the revision(s) and file(s) from the selection", i_branch);	
	CCvsMenu macroSelMenu(selMenu, "Macros selection");
	for(cnt = 0, i = gMacrosSel.entries.begin(); i != gMacrosSel.entries.end(); ++i, ++cnt)
	{
		CCvsMenuItem mac(macroSelMenu, (*i).name, 0, MacroSelLaunch, MacroSelUpdate);
		mac.SetGenericId((int)iMacrosSel + cnt);
	}	

	(void)QWhatsThis::whatsThisButton(mainBar);
}

void UpdateMenus()
{
	if(sUpdateMenusCheck)
	{
#if qCvsDebug
		cvs_err("This should not happen\n");
#endif
		return;
	}
	CTempToggleBool toggler(sUpdateMenusCheck);

	// grab the current selection
	sCurEntries.erase(sCurEntries.begin(), sCurEntries.end());
	CBrowserEnt::numFiles = 0;
	CBrowserEnt::numFolders = 0;
	CBrowserEnt::numMissing = 0;
	CBrowserEnt::numUnknowns = 0;
	CBrowserEnt::numTotal = 0;
	if(!QCvsApp::qApp->IsCvsLoaded() && !gCvsPrefs.empty())
	{
		QFileView *fview = QCvsApp::qApp->GetFileView();
		QDirView *dview = QCvsApp::qApp->GetDirView();
		EntnodeData *data;

		if(fview != 0L && QCvsApp::qApp->GetSoftFocus() == QCvsApp::kFileView)
		{
			CBrowserEnt::fromWhere = CBrowserEnt::fromFileView;
			QFileItem *fitem = (QFileItem *)fview->firstChild();
			if(fitem != 0L) do
			{
				data = fitem->GetData();
				if(data != 0L && fitem->isSelected())
				{
					if(data->GetType() == ENT_FILE)
						CBrowserEnt::numFiles++;
					else
						CBrowserEnt::numFolders++;
					
					if(data->IsMissing())
						CBrowserEnt::numMissing++;
					else if(data->IsUnknown())
						CBrowserEnt::numUnknowns++;
					
					CBrowserEnt::numTotal++;
					sCurEntries.push_back(CBrowserEnt(fview->GetPath(), data));
				}
				fitem = (QFileItem *)fitem->nextSibling();
			} while(fitem != 0L);
		}
		else if(dview != 0L && QCvsApp::qApp->GetSoftFocus() == QCvsApp::kDirView)
		{
			CBrowserEnt::fromWhere = CBrowserEnt::fromDirView;
			QDirItem *ditem = (QDirItem *)dview->firstChild();
			if(ditem != 0L) do
			{
				data = ditem->GetData();
				if(data != 0L && ditem->isSelected())
				{
					if(data->GetType() == ENT_FILE)
						CBrowserEnt::numFiles++;
					else
						CBrowserEnt::numFolders++;
					
					if(data->IsMissing())
						CBrowserEnt::numMissing++;
					else if(data->IsUnknown())
						CBrowserEnt::numUnknowns++;
					
					CBrowserEnt::numTotal++;
					CStr path;
					dview->RetrievePath(ditem, path);
					sCurEntries.push_back(CBrowserEnt(path, data));
				}
				ditem = (QDirItem *)ditem->itemBelow();
			} while(ditem != 0L);			
		}
	}

	vector<QMenuSlot *>::iterator i;
	for(i = QMenuSlot::allslots.begin(); i != QMenuSlot::allslots.end(); ++i)
	{
		(*i)->UpdateCmd();
	}
}

bool CurrentUpdatingMenus()
{
	return sUpdateMenusCheck;
}

#include "menumgr.moc"
