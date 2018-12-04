#ifndef MENUMGR_H
#define MENUMGR_H

// Handle the menus/buttons activation/launch

#include <qobject.h>
#include <vector.h>

// the generic ids
enum
{
	iQuit,
	iMacrosAdmin = 1000,
	iMacrosSel = 2000
};

class CCvsMenu;
class CCvsMenuBar;
class QToolButton;
class QPopupMenu;
class QToolBar;
class QMenuBar;
class QMainWindow;

typedef enum
{
	kItemNormal = 0x00,
		// no special behavior
	kItemActivable = 0x01
		// - menu item : checkable
		// - toolbar button : can toggle
} kItemKind;

// this structure is passed to each update hook
// to set the state of an item.
struct MenuCommand
{
	bool isEnabled;
	bool isActivated;
};

typedef void (*CommandUpdateHook)(MenuCommand & cmd);
	// update hook for each item
typedef void (*CommandLaunchHook)(void);
	// launch hook for each item

// The slot remember the last states of the item
// and update it if necessary
class QMenuSlot : public QObject
{
	Q_OBJECT
public:
	QMenuSlot(CommandLaunchHook launch = 0L, CommandUpdateHook update = 0L);
	virtual ~QMenuSlot();

	void SetEnabled(bool state);
	void SetActivated(bool state);

	void UpdateCmd();
		// call the update hook then call the two functions above
		// if necessary

	static QMenuSlot *Get(int genericid);
		// used to make the connections easy
	
	static QMenuSlot *current;
		// the slot which is currently receiving the update/launch signal

public slots:
	void cmd();
		// slot connected to the menu hit, button push...
signals:
	void cmdIsAboutToLaunch(QMenuSlot *);

public:
	int id;
	int genericid;
	QToolButton *button;
	QPopupMenu *menu;
	CommandLaunchHook launchCmd;
	CommandUpdateHook updateCmd;
	bool isactivated;
	bool isenabled;

	static vector<QMenuSlot *> allslots;
};

// add a new menu item
class CCvsMenuItem
{
public:
	CCvsMenuItem(CCvsMenu & menu, const char *txt, int keystroke,
				 CommandLaunchHook launchCmd,
				 CommandUpdateHook updateCmd = 0L,
				 QToolBar *toolBar = 0L,
				 const char *helpString = 0L,
				 const char **icon = 0L,
				 kItemKind kind = kItemNormal);
	CCvsMenuItem(CCvsMenu & menu, const char *txt, CCvsMenu & subMenu, const char *icon = 0L);
	CCvsMenuItem(CCvsMenu & menu);

	virtual ~CCvsMenuItem();

	void SetGenericId(int genericid) { slot->genericid = genericid; }
protected:
	QMenuSlot *slot;
};

// add a new menu
class CCvsMenu
{
public:
	CCvsMenu(CCvsMenuBar & bar, const char *name);
	CCvsMenu(CCvsMenu & parent, const char *name);
	virtual ~CCvsMenu();

	QPopupMenu *fQTMenu;
	bool fCheckable;
};

// add a new menubar
class CCvsMenuBar
{
public:
	CCvsMenuBar(QMenuBar *menuBar);
	virtual ~CCvsMenuBar();

	QMenuBar *fQTMenuBar;
};

void InstallMenus(QMainWindow *mainWindow);
	// initial set-up for the menus

void UpdateMenus();
	// update the state (activated, enabled...) of all the known items
	// (menu item, toolbar button...)

bool CurrentUpdatingMenus();
	// Avoid reentring in the update menus process

#endif
