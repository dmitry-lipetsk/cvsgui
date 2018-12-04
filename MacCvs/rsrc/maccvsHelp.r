/* maccvsHelp.r - automatically generated stub file for balloon help.
	from Balloony, by David Phillip Oster, oster@netcom.com
Connect hdlg's to dialogs by adding a line like this at the end of your DITLs:
	{0, 0, 0, 0},			helpItem { disabled, HMScanhdlg { <hdlgID> } }
 */

/* I edited many, commented others, and left too many uselessly, also!
   Nevertheless, they are working...
   
   Jocelyn
*/

#include "MacTypes.r"
#include "BalloonTypes.r"
#define hMenuBase	2000
#define hDlogBase	3000

#if 0 /* removed */
#define AppleMenu	128
#define AppleHmnu hMenuBase+AppleMenu
resource 'STR#' (AppleHmnu, purgeable) {{
	/* 1 */	"",
	/* 2 */	"About MacCVS…",
	/* 3 */	"Credits",
	/* 4 */	"Copyrights"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (AppleMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {AppleHmnu, 1, AppleHmnu, 1, AppleHmnu, 1, AppleHmnu, 1 },
	HMStringResItem {AppleHmnu, 2, AppleHmnu, 2, AppleHmnu, 2, AppleHmnu, 2 },
	HMStringResItem {AppleHmnu, 3, AppleHmnu, 3, AppleHmnu, 3, AppleHmnu, 3 },
	HMStringResItem {AppleHmnu, 4, AppleHmnu, 4, AppleHmnu, 4, AppleHmnu, 4 }
}};
#endif

#define CvsAdminMenu	129
#define CvsAdminHmnu hMenuBase+CvsAdminMenu
resource 'STR#' (CvsAdminHmnu, purgeable) {{
	/* 1 */	"Gives access to the preferences, administration commands, "
			"as well as command line action.",
	/* 2 */	"Use this option to checkout from scratch a new module on your disk.",
	/* 3 */	"Use this option to create a new module into the cvs repository located "
			"on the cvs server machine. After importing, you should use the checkout "
			"command to start using this new module.",
	/* 4 */	"Places a tag, or symbolic name, on all the files in the module.",
	/* 5 */	"To access admin macros…",
	/* 6 */	"To set the CVSROOT, server type, display font and other various options…",
	/* 7 */	"To enter a command line, just like a console…",
	/* 8 */	"To log on a server… (only necessary the first time you log on a pserver)"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (CvsAdminMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {CvsAdminHmnu, 1, CvsAdminHmnu, 1, CvsAdminHmnu, 1, CvsAdminHmnu, 1 },
	HMStringResItem {CvsAdminHmnu, 2, CvsAdminHmnu, 2, CvsAdminHmnu, 2, CvsAdminHmnu, 2 },
	HMStringResItem {CvsAdminHmnu, 3, CvsAdminHmnu, 3, CvsAdminHmnu, 3, CvsAdminHmnu, 3 },
	HMSkipItem {},
	HMStringResItem {CvsAdminHmnu, 4, CvsAdminHmnu, 4, CvsAdminHmnu, 4, CvsAdminHmnu, 4 },
	HMSkipItem {},
	HMStringResItem {CvsAdminHmnu, 5, CvsAdminHmnu, 5, CvsAdminHmnu, 5, CvsAdminHmnu, 5 },
	HMSkipItem {},
	HMStringResItem {CvsAdminHmnu, 6, CvsAdminHmnu, 6, CvsAdminHmnu, 6, CvsAdminHmnu, 6 },
	HMStringResItem {CvsAdminHmnu, 7, CvsAdminHmnu, 7, CvsAdminHmnu, 7, CvsAdminHmnu, 7 },
	HMSkipItem {},
	HMStringResItem {CvsAdminHmnu, 8, CvsAdminHmnu, 8, CvsAdminHmnu, 8, CvsAdminHmnu, 8 }
}};

#define CvsFilesMenu	130
#define CvsFilesHmnu hMenuBase+CvsFilesMenu
resource 'STR#' (CvsFilesHmnu, purgeable) {{
	/* 1 */	"Commands concerning the files…",
	/* 2 */	"Gets the latest version of the specified file(s) from the cvs server "
			"repository to your local checkout directory.",
	/* 3 */	"Sends out the specified file(s) from your local directory to the cvs "
			"server repository.\n"
			"\n"
			"You may want to Update beforehand in order to merge the changes that might be needed.",
	/* 4 */	"Adds the specified file(s) to the cvs server repository. You must Commit them "
			"for changes to actually take effect.",
	/* 5 */	"Adds the specified file(s) to the cvs server repository, transferring in a "
			"binary mode.\n"
			"\n"
			"You must Commit them for changes to actually take effect.",
	/* 6 */	"Moves the specified file(s) in the Attic directory on the cvs server repository.\n"
			"\n"
			"Such files can be retrieved later.",
	/* 7 */	"Executes a Diff command that compares the specified file(s) on your local "
			"repository with an arbitrary version present on the cvs server repository.",
	/* 8 */	"Requests the cvs server the item's history (comments, revision numbers, tags, etc.)\n"
			"\n"
			"Uses the Log Window as output.",
	/* 9 */	"Asks the cvs server the current status of the specified file(s) (modified, locked, etc.)",
	/* 10 */	"It is not a real cvs command but it’s provided for convenience.\n"
				"\n"
				"This option will send to the trash the specified files and will ask the "
				"cvs server to update them.\n"
				"(you can use the “Put Away” option of the Finder to recover them…)",
	/* 11 */	"Monitors files",
	/* 12 */	"Tag files",
	/* 13 */	"Macros files"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (CvsFilesMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {CvsFilesHmnu, 1, CvsFilesHmnu, 1, CvsFilesHmnu, 1, CvsFilesHmnu, 1 },
	HMStringResItem {CvsFilesHmnu, 2, CvsFilesHmnu, 2, CvsFilesHmnu, 2, CvsFilesHmnu, 2 },
	HMStringResItem {CvsFilesHmnu, 3, CvsFilesHmnu, 3, CvsFilesHmnu, 3, CvsFilesHmnu, 3 },
	HMSkipItem {},
	HMStringResItem {CvsFilesHmnu, 4, CvsFilesHmnu, 4, CvsFilesHmnu, 4, CvsFilesHmnu, 4 },
	HMStringResItem {CvsFilesHmnu, 5, CvsFilesHmnu, 5, CvsFilesHmnu, 5, CvsFilesHmnu, 5 },
	HMStringResItem {CvsFilesHmnu, 6, CvsFilesHmnu, 6, CvsFilesHmnu, 6, CvsFilesHmnu, 6 },
	HMSkipItem {},
	HMStringResItem {CvsFilesHmnu, 7, CvsFilesHmnu, 7, CvsFilesHmnu, 7, CvsFilesHmnu, 7 },
	HMStringResItem {CvsFilesHmnu, 8, CvsFilesHmnu, 8, CvsFilesHmnu, 8, CvsFilesHmnu, 8 },
	HMStringResItem {CvsFilesHmnu, 9, CvsFilesHmnu, 9, CvsFilesHmnu, 9, CvsFilesHmnu, 9 },
	HMSkipItem {},
	HMStringResItem {CvsFilesHmnu, 10, CvsFilesHmnu, 10, CvsFilesHmnu, 10, CvsFilesHmnu, 10 },
	HMSkipItem {},
	HMSkipItem {},
	HMSkipItem {},
	HMSkipItem {}
}};

#define CvsFolderMenu	131
#define CvsFolderHmnu hMenuBase+CvsFolderMenu
resource 'STR#' (CvsFolderHmnu, purgeable) {{
	/* 1 */	"Commands applied on a whole folder…",
	/* 2 */	"Gets the latest version of the specified folder (and all including files) from "
			"the cvs server repository to your local checkout directory.",
	/* 3 */	"Sends out all of the specified folder from your local directory to the cvs "
			"server repository.\n"
			"\n"
			"You may want to Update beforehand in order to merge the changes that might be needed.",
	/* 4 */	"Adds a folder (but not its content) to the cvs server repository. You always need "
			"to add the folder before adding its content.",
	/* 5 */	"Executes a Diff command that compares the specified folder on your local repository "
			"to any arbitrary version currently present on the cvs server repository.",
	/* 6 */	"Requests the cvs server the specified folder history (comments, revision numbers, "
			"tags, etc.)\n"
			"\n"
			"Uses the Log Window as output.",
	/* 7 */	"Asks the cvs server the current status of the specified folder (modified, locked, etc.)",
	/* 8 */	"Monitors folder",
	/* 9 */	"Tag folder",
	/* 10 */	"Macros folder"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (CvsFolderMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {CvsFolderHmnu, 1, CvsFolderHmnu, 1, CvsFolderHmnu, 1, CvsFolderHmnu, 1 },
	HMStringResItem {CvsFolderHmnu, 2, CvsFolderHmnu, 2, CvsFolderHmnu, 2, CvsFolderHmnu, 2 },
	HMStringResItem {CvsFolderHmnu, 3, CvsFolderHmnu, 3, CvsFolderHmnu, 3, CvsFolderHmnu, 3 },
	HMSkipItem {},
	HMStringResItem {CvsFolderHmnu, 4, CvsFolderHmnu, 4, CvsFolderHmnu, 4, CvsFolderHmnu, 4 },
	HMSkipItem {},
	HMStringResItem {CvsFolderHmnu, 5, CvsFolderHmnu, 5, CvsFolderHmnu, 5, CvsFolderHmnu, 5 },
	HMStringResItem {CvsFolderHmnu, 6, CvsFolderHmnu, 6, CvsFolderHmnu, 6, CvsFolderHmnu, 6 },
	HMStringResItem {CvsFolderHmnu, 7, CvsFolderHmnu, 7, CvsFolderHmnu, 7, CvsFolderHmnu, 7 },
	HMSkipItem {},
	HMSkipItem {},
	HMSkipItem {},
	HMSkipItem {}
}};

#if 0	/* removed  -> line 466 for #endif */
#define FileMenu	133
#define FileHmnu hMenuBase+FileMenu
resource 'STR#' (FileHmnu, purgeable) {{
	/* 1 */	"File",
	/* 2 */	"Save As…",
	/* 3 */	"Close",
	/* 4 */	"Page  SetUp…",
	/* 5 */	"Print…",
	/* 6 */	"Quit"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (FileMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {FileHmnu, 1, FileHmnu, 1, FileHmnu, 1, FileHmnu, 1 },
	HMStringResItem {FileHmnu, 2, FileHmnu, 2, FileHmnu, 2, FileHmnu, 2 },
	HMStringResItem {FileHmnu, 3, FileHmnu, 3, FileHmnu, 3, FileHmnu, 3 },
	HMSkipItem {},
	HMStringResItem {FileHmnu, 4, FileHmnu, 4, FileHmnu, 4, FileHmnu, 4 },
	HMStringResItem {FileHmnu, 5, FileHmnu, 5, FileHmnu, 5, FileHmnu, 5 },
	HMSkipItem {},
	HMStringResItem {FileHmnu, 6, FileHmnu, 6, FileHmnu, 6, FileHmnu, 6 }
}};

#define EditMenu	134
#define EditHmnu hMenuBase+EditMenu
resource 'STR#' (EditHmnu, purgeable) {{
	/* 1 */	"Edit",
	/* 2 */	"Undo",
	/* 3 */	"Cut",
	/* 4 */	"Copy",
	/* 5 */	"Paste",
	/* 6 */	"Select All"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (EditMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {EditHmnu, 1, EditHmnu, 1, EditHmnu, 1, EditHmnu, 1 },
	HMStringResItem {EditHmnu, 2, EditHmnu, 2, EditHmnu, 2, EditHmnu, 2 },
	HMSkipItem {},
	HMStringResItem {EditHmnu, 3, EditHmnu, 3, EditHmnu, 3, EditHmnu, 3 },
	HMStringResItem {EditHmnu, 4, EditHmnu, 4, EditHmnu, 4, EditHmnu, 4 },
	HMStringResItem {EditHmnu, 5, EditHmnu, 5, EditHmnu, 5, EditHmnu, 5 },
	HMSkipItem {},
	HMStringResItem {EditHmnu, 6, EditHmnu, 6, EditHmnu, 6, EditHmnu, 6 }
}};

#define PrefsMenu	135
#define PrefsHmnu hMenuBase+PrefsMenu
resource 'STR#' (PrefsHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Globals",
	/* 3 */	"Text Files",
	/* 4 */	"Binary Files",
	/* 5 */	"Ports",
	/* 6 */	"Fonts"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (PrefsMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {PrefsHmnu, 1, PrefsHmnu, 1, PrefsHmnu, 1, PrefsHmnu, 1 },
	HMStringResItem {PrefsHmnu, 2, PrefsHmnu, 2, PrefsHmnu, 2, PrefsHmnu, 2 },
	HMStringResItem {PrefsHmnu, 3, PrefsHmnu, 3, PrefsHmnu, 3, PrefsHmnu, 3 },
	HMStringResItem {PrefsHmnu, 4, PrefsHmnu, 4, PrefsHmnu, 4, PrefsHmnu, 4 },
	HMStringResItem {PrefsHmnu, 5, PrefsHmnu, 5, PrefsHmnu, 5, PrefsHmnu, 5 },
	HMStringResItem {PrefsHmnu, 6, PrefsHmnu, 6, PrefsHmnu, 6, PrefsHmnu, 6 }
}};

#define ISOMenu	136
#define ISOHmnu hMenuBase+ISOMenu
resource 'STR#' (ISOHmnu, purgeable) {{
	/* 1 */	"no ISO-8859 translation",
	/* 2 */	"ISO-8859-1 Latin 1",
	/* 3 */	"ISO-8859-2 Latin 2",
	/* 4 */	"ISO-8859-3 Latin 3",
	/* 5 */	"ISO-8859-4 Latin 4",
	/* 6 */	"ISO-8859-5 Cyrillic",
	/* 7 */	"ISO-8859-6 Arabic",
	/* 8 */	"ISO-8859-7 Greek",
	/* 9 */	"ISO-8859-8 Hebrew",
	/* 10 */	"ISO-8859-9 Latin 5 Turkish",
	/* 11 */	"ISO-8859-10 Latin 6 Nordic"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ISOMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ISOHmnu, 1, ISOHmnu, 1, ISOHmnu, 1, ISOHmnu, 1 },
	HMStringResItem {ISOHmnu, 2, ISOHmnu, 2, ISOHmnu, 2, ISOHmnu, 2 },
	HMStringResItem {ISOHmnu, 3, ISOHmnu, 3, ISOHmnu, 3, ISOHmnu, 3 },
	HMStringResItem {ISOHmnu, 4, ISOHmnu, 4, ISOHmnu, 4, ISOHmnu, 4 },
	HMStringResItem {ISOHmnu, 5, ISOHmnu, 5, ISOHmnu, 5, ISOHmnu, 5 },
	HMStringResItem {ISOHmnu, 6, ISOHmnu, 6, ISOHmnu, 6, ISOHmnu, 6 },
	HMStringResItem {ISOHmnu, 7, ISOHmnu, 7, ISOHmnu, 7, ISOHmnu, 7 },
	HMStringResItem {ISOHmnu, 8, ISOHmnu, 8, ISOHmnu, 8, ISOHmnu, 8 },
	HMStringResItem {ISOHmnu, 9, ISOHmnu, 9, ISOHmnu, 9, ISOHmnu, 9 },
	HMStringResItem {ISOHmnu, 10, ISOHmnu, 10, ISOHmnu, 10, ISOHmnu, 10 },
	HMStringResItem {ISOHmnu, 11, ISOHmnu, 11, ISOHmnu, 11, ISOHmnu, 11 }
}};

#define CommitMenu	137
#define CommitHmnu hMenuBase+CommitMenu
resource 'STR#' (CommitHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (CommitMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {CommitHmnu, 1, CommitHmnu, 1, CommitHmnu, 1, CommitHmnu, 1 },
	HMStringResItem {CommitHmnu, 2, CommitHmnu, 2, CommitHmnu, 2, CommitHmnu, 2 }
}};

#define ImportMenu	138
#define ImportHmnu hMenuBase+ImportMenu
resource 'STR#' (ImportHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Initial tags",
	/* 3 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ImportMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ImportHmnu, 1, ImportHmnu, 1, ImportHmnu, 1, ImportHmnu, 1 },
	HMStringResItem {ImportHmnu, 2, ImportHmnu, 2, ImportHmnu, 2, ImportHmnu, 2 },
	HMStringResItem {ImportHmnu, 3, ImportHmnu, 3, ImportHmnu, 3, ImportHmnu, 3 }
}};

#define CheckoutMenu	139
#define CheckoutHmnu hMenuBase+CheckoutMenu
resource 'STR#' (CheckoutHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Options",
	/* 3 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (CheckoutMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {CheckoutHmnu, 1, CheckoutHmnu, 1, CheckoutHmnu, 1, CheckoutHmnu, 1 },
	HMStringResItem {CheckoutHmnu, 2, CheckoutHmnu, 2, CheckoutHmnu, 2, CheckoutHmnu, 2 },
	HMStringResItem {CheckoutHmnu, 3, CheckoutHmnu, 3, CheckoutHmnu, 3, CheckoutHmnu, 3 }
}};

#define CmdlineMenu	140
#define CmdlineHmnu hMenuBase+CmdlineMenu
resource 'STR#' (CmdlineHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (CmdlineMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {CmdlineHmnu, 1, CmdlineHmnu, 1, CmdlineHmnu, 1, CmdlineHmnu, 1 },
	HMStringResItem {CmdlineHmnu, 2, CmdlineHmnu, 2, CmdlineHmnu, 2, CmdlineHmnu, 2 }
}};

#define UpdateMenu	141
#define UpdateHmnu hMenuBase+UpdateMenu
resource 'STR#' (UpdateHmnu, purgeable) {{
	/* 1 */	"Update settings",
	/* 2 */	"Sticky update",
	/* 3 */	"Merge options",
	/* 4 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (UpdateMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {UpdateHmnu, 1, UpdateHmnu, 1, UpdateHmnu, 1, UpdateHmnu, 1 },
	HMStringResItem {UpdateHmnu, 2, UpdateHmnu, 2, UpdateHmnu, 2, UpdateHmnu, 2 },
	HMStringResItem {UpdateHmnu, 3, UpdateHmnu, 3, UpdateHmnu, 3, UpdateHmnu, 3 },
	HMStringResItem {UpdateHmnu, 4, UpdateHmnu, 4, UpdateHmnu, 4, UpdateHmnu, 4 }
}};

#define ImportMenu	142
#define ImportHmnu hMenuBase+ImportMenu
resource 'STR#' (ImportHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Filters",
	/* 3 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ImportMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ImportHmnu, 1, ImportHmnu, 1, ImportHmnu, 1, ImportHmnu, 1 },
	HMStringResItem {ImportHmnu, 2, ImportHmnu, 2, ImportHmnu, 2, ImportHmnu, 2 },
	HMStringResItem {ImportHmnu, 3, ImportHmnu, 3, ImportHmnu, 3, ImportHmnu, 3 }
}};

#define DiffMenu	143
#define DiffHmnu hMenuBase+DiffMenu
resource 'STR#' (DiffHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (DiffMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {DiffHmnu, 1, DiffHmnu, 1, DiffHmnu, 1, DiffHmnu, 1 },
	HMStringResItem {DiffHmnu, 2, DiffHmnu, 2, DiffHmnu, 2, DiffHmnu, 2 }
}};
#endif

#define ViewsMenu	144
#define ViewsHmnu hMenuBase+ViewsMenu
resource 'STR#' (ViewsHmnu, purgeable) {{
	/* 1 */	"To manage the browser window(s) and setup the Log Window visibility",
	/* 2 */	"To create a new browser that displays the repository hierarchy "
			"in a Finder-like way…",
	/* 3 */	"Access a recently used browser…",	/*not shown*/
	/* 4 */	"To refresh the browser's content"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ViewsMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {ViewsHmnu, 1, ViewsHmnu, 1, ViewsHmnu, 1, ViewsHmnu, 1 },
	HMStringResItem {ViewsHmnu, 2, ViewsHmnu, 2, ViewsHmnu, 2, ViewsHmnu, 2 },
	HMStringResItem {ViewsHmnu, 3, ViewsHmnu, 3, ViewsHmnu, 3, ViewsHmnu, 3 },
	HMStringResItem {ViewsHmnu, 4, ViewsHmnu, 4, ViewsHmnu, 4, ViewsHmnu, 4 },
	HMSkipItem {}
}};

#define SelectionMenu	145
#define SelectionHmnu hMenuBase+SelectionMenu
#define SelectionHmnu2 5140	/* we use the browser's strings */
resource 'STR#' (SelectionHmnu, purgeable) {{
	/* 1 */	"Commands applied to selections made in a browser window",
	/* 2 */	"Update selection…",
	/* 3 */	"Commit selection…",
	/* 4 */	"Add selection",
	/* 5 */	"Add binary selection",
	/* 6 */	"Remove selection",
	/* 7 */	"Diff selection…",
	/* 8 */	"Log selection…",
	/* 9 */	"Status selection",
	/* 10 */	"Erase selection",
	/* 11 */	"Monitors selection",
	/* 12 */	"Tag selection",
	/* 13 */	"Macros selection"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (SelectionMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {SelectionHmnu, 1, SelectionHmnu, 1, SelectionHmnu, 1, SelectionHmnu, 1 },
	HMStringResItem {SelectionHmnu2, 1, SelectionHmnu2, 1, SelectionHmnu2, 1, SelectionHmnu2, 1 },
	HMStringResItem {SelectionHmnu2, 3, SelectionHmnu2, 3, SelectionHmnu2, 3, SelectionHmnu2, 3 },
	HMSkipItem {},
	HMStringResItem {SelectionHmnu2, 5, SelectionHmnu2, 5, SelectionHmnu2, 5, SelectionHmnu2, 5 },
	HMStringResItem {SelectionHmnu2, 7, SelectionHmnu2, 7, SelectionHmnu2, 7, SelectionHmnu2, 7 },
	HMStringResItem {SelectionHmnu2, 9, SelectionHmnu2, 9, SelectionHmnu2, 9, SelectionHmnu2, 9 },
	HMSkipItem {},
	HMStringResItem {SelectionHmnu2, 11, SelectionHmnu2, 11, SelectionHmnu2, 11, SelectionHmnu2, 11 },
	HMStringResItem {SelectionHmnu2, 13, SelectionHmnu2, 13, SelectionHmnu2, 13, SelectionHmnu2, 138 },
	HMStringResItem {SelectionHmnu2, 15, SelectionHmnu2, 15, SelectionHmnu2, 15, SelectionHmnu2, 15 },
	HMSkipItem {},
	HMStringResItem {SelectionHmnu2, 19, SelectionHmnu2, 19, SelectionHmnu2, 19, SelectionHmnu2, 19 },
	HMSkipItem {},
	HMSkipItem {},
	HMSkipItem {},
	HMSkipItem {}
}};

#define RtagCreateMenu	146
#define RtagCreateHmnu hMenuBase+RtagCreateMenu
resource 'STR#' (RtagCreateHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Options",
	/* 3 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (RtagCreateMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {RtagCreateHmnu, 1, RtagCreateHmnu, 1, RtagCreateHmnu, 1, RtagCreateHmnu, 1 },
	HMStringResItem {RtagCreateHmnu, 2, RtagCreateHmnu, 2, RtagCreateHmnu, 2, RtagCreateHmnu, 2 },
	HMStringResItem {RtagCreateHmnu, 3, RtagCreateHmnu, 3, RtagCreateHmnu, 3, RtagCreateHmnu, 3 }
}};

#define RtagDeleteMenu	147
#define RtagDeleteHmnu hMenuBase+RtagDeleteMenu
resource 'STR#' (RtagDeleteHmnu, purgeable) {{
	/* 1 */	"General",
	/* 2 */	"Globals"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (RtagDeleteMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {RtagDeleteHmnu, 1, RtagDeleteHmnu, 1, RtagDeleteHmnu, 1, RtagDeleteHmnu, 1 },
	HMStringResItem {RtagDeleteHmnu, 2, RtagDeleteHmnu, 2, RtagDeleteHmnu, 2, RtagDeleteHmnu, 2 }
}};

#define FontMenuMenu	160
#define FontMenuHmnu hMenuBase+FontMenuMenu
resource 'STR#' (FontMenuHmnu, purgeable) {{
	/* 1 */	"Font"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (FontMenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {FontMenuHmnu, 1, FontMenuHmnu, 1, FontMenuHmnu, 1, FontMenuHmnu, 1 }
}};

#define SizeMenuMenu	161
#define SizeMenuHmnu hMenuBase+SizeMenuMenu
resource 'STR#' (SizeMenuHmnu, purgeable) {{
	/* 1 */	"Size",
	/* 2 */	"Point 9",
	/* 3 */	"Point 10",
	/* 4 */	"Point 12",
	/* 5 */	"Point 14",
	/* 6 */	"Point 16",
	/* 7 */	"Point 18"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (SizeMenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {SizeMenuHmnu, 1, SizeMenuHmnu, 1, SizeMenuHmnu, 1, SizeMenuHmnu, 1 },
	HMStringResItem {SizeMenuHmnu, 2, SizeMenuHmnu, 2, SizeMenuHmnu, 2, SizeMenuHmnu, 2 },
	HMStringResItem {SizeMenuHmnu, 3, SizeMenuHmnu, 3, SizeMenuHmnu, 3, SizeMenuHmnu, 3 },
	HMStringResItem {SizeMenuHmnu, 4, SizeMenuHmnu, 4, SizeMenuHmnu, 4, SizeMenuHmnu, 4 },
	HMStringResItem {SizeMenuHmnu, 5, SizeMenuHmnu, 5, SizeMenuHmnu, 5, SizeMenuHmnu, 5 },
	HMStringResItem {SizeMenuHmnu, 6, SizeMenuHmnu, 6, SizeMenuHmnu, 6, SizeMenuHmnu, 6 },
	HMStringResItem {SizeMenuHmnu, 7, SizeMenuHmnu, 7, SizeMenuHmnu, 7, SizeMenuHmnu, 7 }
}};

#define EmptyMenuMenu	162
#define EmptyMenuHmnu hMenuBase+EmptyMenuMenu
resource 'STR#' (EmptyMenuHmnu, purgeable) {{

}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (EmptyMenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {}
}};

#define DatesmenuMenu	163
#define DatesmenuHmnu hMenuBase+DatesmenuMenu
resource 'STR#' (DatesmenuHmnu, purgeable) {{
	/* 1 */	"1998-3",
	/* 2 */	"1998-3-24",
	/* 3 */	"1998-3-24 20:55",
	/* 4 */	"24 Mar",
	/* 5 */	">=1998-3-24"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (DatesmenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {DatesmenuHmnu, 1, DatesmenuHmnu, 1, DatesmenuHmnu, 1, DatesmenuHmnu, 1 },
	HMStringResItem {DatesmenuHmnu, 2, DatesmenuHmnu, 2, DatesmenuHmnu, 2, DatesmenuHmnu, 2 },
	HMStringResItem {DatesmenuHmnu, 3, DatesmenuHmnu, 3, DatesmenuHmnu, 3, DatesmenuHmnu, 3 },
	HMStringResItem {DatesmenuHmnu, 4, DatesmenuHmnu, 4, DatesmenuHmnu, 4, DatesmenuHmnu, 4 },
	HMStringResItem {DatesmenuHmnu, 5, DatesmenuHmnu, 5, DatesmenuHmnu, 5, DatesmenuHmnu, 5 }
}};

#define RevTagmenuMenu	164
#define RevTagmenuHmnu hMenuBase+RevTagmenuMenu
resource 'STR#' (RevTagmenuHmnu, purgeable) {{
	/* 1 */	"1.1",
	/* 2 */	"1.1.1",
	/* 3 */	"branch-release-1-0",
	/* 4 */	"HEAD"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (RevTagmenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {RevTagmenuHmnu, 1, RevTagmenuHmnu, 1, RevTagmenuHmnu, 1, RevTagmenuHmnu, 1 },
	HMStringResItem {RevTagmenuHmnu, 2, RevTagmenuHmnu, 2, RevTagmenuHmnu, 2, RevTagmenuHmnu, 2 },
	HMStringResItem {RevTagmenuHmnu, 3, RevTagmenuHmnu, 3, RevTagmenuHmnu, 3, RevTagmenuHmnu, 3 },
	HMStringResItem {RevTagmenuHmnu, 4, RevTagmenuHmnu, 4, RevTagmenuHmnu, 4, RevTagmenuHmnu, 4 }
}};

#define ExDatesmenuMenu	165
#define ExDatesmenuHmnu hMenuBase+ExDatesmenuMenu
resource 'STR#' (ExDatesmenuHmnu, purgeable) {{
	/* 1 */	">1998-3",
	/* 2 */	">1998-3-24",
	/* 3 */	">1998-3-24 20:55",
	/* 4 */	">24 Mar",
	/* 5 */	"<1998-3-24",
	/* 6 */	"1998-1-24<1998-3-24"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ExDatesmenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ExDatesmenuHmnu, 1, ExDatesmenuHmnu, 1, ExDatesmenuHmnu, 1, ExDatesmenuHmnu, 1 },
	HMStringResItem {ExDatesmenuHmnu, 2, ExDatesmenuHmnu, 2, ExDatesmenuHmnu, 2, ExDatesmenuHmnu, 2 },
	HMStringResItem {ExDatesmenuHmnu, 3, ExDatesmenuHmnu, 3, ExDatesmenuHmnu, 3, ExDatesmenuHmnu, 3 },
	HMStringResItem {ExDatesmenuHmnu, 4, ExDatesmenuHmnu, 4, ExDatesmenuHmnu, 4, ExDatesmenuHmnu, 4 },
	HMStringResItem {ExDatesmenuHmnu, 5, ExDatesmenuHmnu, 5, ExDatesmenuHmnu, 5, ExDatesmenuHmnu, 5 },
	HMStringResItem {ExDatesmenuHmnu, 6, ExDatesmenuHmnu, 6, ExDatesmenuHmnu, 6, ExDatesmenuHmnu, 6 }
}};

#define ExRevTagMenu	166
#define ExRevTagHmnu hMenuBase+ExRevTagMenu
resource 'STR#' (ExRevTagHmnu, purgeable) {{
	/* 1 */	"1.32:1.35",
	/* 2 */	":1.32",
	/* 3 */	"1.35:",
	/* 4 */	"branch-release-1-0",
	/* 5 */	"1.4.3:1.4.5",
	/* 6 */	"1.4."
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ExRevTagMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ExRevTagHmnu, 1, ExRevTagHmnu, 1, ExRevTagHmnu, 1, ExRevTagHmnu, 1 },
	HMStringResItem {ExRevTagHmnu, 2, ExRevTagHmnu, 2, ExRevTagHmnu, 2, ExRevTagHmnu, 2 },
	HMStringResItem {ExRevTagHmnu, 3, ExRevTagHmnu, 3, ExRevTagHmnu, 3, ExRevTagHmnu, 3 },
	HMStringResItem {ExRevTagHmnu, 4, ExRevTagHmnu, 4, ExRevTagHmnu, 4, ExRevTagHmnu, 4 },
	HMStringResItem {ExRevTagHmnu, 5, ExRevTagHmnu, 5, ExRevTagHmnu, 5, ExRevTagHmnu, 5 },
	HMStringResItem {ExRevTagHmnu, 6, ExRevTagHmnu, 6, ExRevTagHmnu, 6, ExRevTagHmnu, 6 }
}};

#define ExStateMenu	167
#define ExStateHmnu hMenuBase+ExStateMenu
resource 'STR#' (ExStateHmnu, purgeable) {{
	/* 1 */	"Exp",
	/* 2 */	"dead",
	/* 3 */	"Rel",
	/* 4 */	"MyState",
	/* 5 */	"Exp,dead,Rel"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ExStateMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ExStateHmnu, 1, ExStateHmnu, 1, ExStateHmnu, 1, ExStateHmnu, 1 },
	HMStringResItem {ExStateHmnu, 2, ExStateHmnu, 2, ExStateHmnu, 2, ExStateHmnu, 2 },
	HMStringResItem {ExStateHmnu, 3, ExStateHmnu, 3, ExStateHmnu, 3, ExStateHmnu, 3 },
	HMStringResItem {ExStateHmnu, 4, ExStateHmnu, 4, ExStateHmnu, 4, ExStateHmnu, 4 },
	HMStringResItem {ExStateHmnu, 5, ExStateHmnu, 5, ExStateHmnu, 5, ExStateHmnu, 5 }
}};

#define ExUsersMenu	168
#define ExUsersHmnu hMenuBase+ExUsersMenu
resource 'STR#' (ExUsersHmnu, purgeable) {{
	/* 1 */	"johna",
	/* 2 */	"johna,garyb"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ExUsersMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ExUsersHmnu, 1, ExUsersHmnu, 1, ExUsersHmnu, 1, ExUsersHmnu, 1 },
	HMStringResItem {ExUsersHmnu, 2, ExUsersHmnu, 2, ExUsersHmnu, 2, ExUsersHmnu, 2 }
}};

#define RevTagDatemenuMenu	169
#define RevTagDatemenuHmnu hMenuBase+RevTagDatemenuMenu
resource 'STR#' (RevTagDatemenuHmnu, purgeable) {{
	/* 1 */	"1.1",
	/* 2 */	"1.1.1",
	/* 3 */	"branch-release-1-0",
	/* 4 */	"HEAD",
	/* 5 */	"98-3-24",
	/* 6 */	"98-3-24 20:55",
	/* 7 */	">=98-3-24"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (RevTagDatemenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {RevTagDatemenuHmnu, 1, RevTagDatemenuHmnu, 1, RevTagDatemenuHmnu, 1, RevTagDatemenuHmnu, 1 },
	HMStringResItem {RevTagDatemenuHmnu, 2, RevTagDatemenuHmnu, 2, RevTagDatemenuHmnu, 2, RevTagDatemenuHmnu, 2 },
	HMStringResItem {RevTagDatemenuHmnu, 3, RevTagDatemenuHmnu, 3, RevTagDatemenuHmnu, 3, RevTagDatemenuHmnu, 3 },
	HMStringResItem {RevTagDatemenuHmnu, 4, RevTagDatemenuHmnu, 4, RevTagDatemenuHmnu, 4, RevTagDatemenuHmnu, 4 },
	HMStringResItem {RevTagDatemenuHmnu, 5, RevTagDatemenuHmnu, 5, RevTagDatemenuHmnu, 5, RevTagDatemenuHmnu, 5 },
	HMStringResItem {RevTagDatemenuHmnu, 6, RevTagDatemenuHmnu, 6, RevTagDatemenuHmnu, 6, RevTagDatemenuHmnu, 6 },
	HMStringResItem {RevTagDatemenuHmnu, 7, RevTagDatemenuHmnu, 7, RevTagDatemenuHmnu, 7, RevTagDatemenuHmnu, 7 }
}};

#define MacBinmenuMenu	170
#define MacBinmenuHmnu hMenuBase+MacBinmenuMenu
resource 'STR#' (MacBinmenuHmnu, purgeable) {{
	/* 1 */	"HQX",
	/* 2 */	"AppleSingle"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MacBinmenuMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {MacBinmenuHmnu, 1, MacBinmenuHmnu, 1, MacBinmenuHmnu, 1, MacBinmenuHmnu, 1 },
	HMStringResItem {MacBinmenuHmnu, 2, MacBinmenuHmnu, 2, MacBinmenuHmnu, 2, MacBinmenuHmnu, 2 }
}};

#define ModuleTagsMenu	171
#define ModuleTagsHmnu hMenuBase+ModuleTagsMenu
resource 'STR#' (ModuleTagsHmnu, purgeable) {{
	/* 1 */	"Create a new tag on your remote repository.\n"
			"\n"
			"This command doesn't need to have a checked-out copy in order to take effect.",
	/* 2 */	"Deletes a tag on your remote repository.\n"
			"\n"
			"This command doesn't need to have a checked-out copy in order to take effect.",
	/* 3 */	"Creates a new branch on your remote repository.\n"
			"\n"
			"This command doesn't need to have a checked-out copy in order to take effect.",
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (ModuleTagsMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {ModuleTagsHmnu, 1, ModuleTagsHmnu, 1, ModuleTagsHmnu, 1, ModuleTagsHmnu, 1 },
	HMStringResItem {ModuleTagsHmnu, 2, ModuleTagsHmnu, 2, ModuleTagsHmnu, 2, ModuleTagsHmnu, 2 },
	HMStringResItem {ModuleTagsHmnu, 3, ModuleTagsHmnu, 3, ModuleTagsHmnu, 3, ModuleTagsHmnu, 3 }
}};

#define FilesTagsMenu	172
#define FilesTagsHmnu hMenuBase+FilesTagsMenu
resource 'STR#' (FilesTagsHmnu, purgeable) {{
	/* 1 */	"Creates a new tag on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of the local selected file(s) in order to create the tag.",
	/* 2 */	"Deletes a tag on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of the local selected file(s) in order to delete the tag.",
	/* 3 */	"Creates a new branch on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of the local selected file(s) in order to create the branch.",
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (FilesTagsMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {FilesTagsHmnu, 1, FilesTagsHmnu, 1, FilesTagsHmnu, 1, FilesTagsHmnu, 1 },
	HMStringResItem {FilesTagsHmnu, 2, FilesTagsHmnu, 2, FilesTagsHmnu, 2, FilesTagsHmnu, 2 },
	HMStringResItem {FilesTagsHmnu, 3, FilesTagsHmnu, 3, FilesTagsHmnu, 3, FilesTagsHmnu, 3 }
}};

#define FolderTagsMenu	173
#define FolderTagsHmnu hMenuBase+FolderTagsMenu
resource 'STR#' (FolderTagsHmnu, purgeable) {{
	/* 1 */	"Create a new tag on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of all the files included in the local folder in order to create the tag.",
	/* 2 */	"Delete a tag on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of all the files included in the local folder in order to delete the tag.",
	/* 3 */	"Create a new branch on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of all the files included in the local folder in order to create the branch."
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (FolderTagsMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {FolderTagsHmnu, 1, FolderTagsHmnu, 1, FolderTagsHmnu, 1, FolderTagsHmnu, 1 },
	HMStringResItem {FolderTagsHmnu, 2, FolderTagsHmnu, 2, FolderTagsHmnu, 2, FolderTagsHmnu, 2 },
	HMStringResItem {FolderTagsHmnu, 3, FolderTagsHmnu, 3, FolderTagsHmnu, 3, FolderTagsHmnu, 3 }
}};

#define SelectionTagsMenu	174
#define SelectionTagsHmnu hMenuBase+SelectionTagsMenu
resource 'STR#' (SelectionTagsHmnu, purgeable) {{
	/* 1 */	"Create a new tag on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of the local selected file(s) in order to create the tag.",
	/* 2 */	"Delete a tag on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of the local selected file(s) in order to delete the tag.",
	/* 3 */	"Create a new branch on your remote repository.\n"
			"\n"
			"By default it uses the revision(s) of the local selected file(s) in order to create the branch."
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (SelectionTagsMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {},
	HMStringResItem {SelectionTagsHmnu, 1, SelectionTagsHmnu, 1, SelectionTagsHmnu, 1, SelectionTagsHmnu, 1 },
	HMStringResItem {SelectionTagsHmnu, 2, SelectionTagsHmnu, 2, SelectionTagsHmnu, 2, SelectionTagsHmnu, 2 },
	HMStringResItem {SelectionTagsHmnu, 3, SelectionTagsHmnu, 3, SelectionTagsHmnu, 3, SelectionTagsHmnu, 3 }
}};

#define MonitorsfilesMenu	175
#define MonitorsfilesHmnu hMenuBase+MonitorsfilesMenu
resource 'STR#' (MonitorsfilesHmnu, purgeable) {{
	/* 1 */	"Menu",
	/* 2 */	"Locks the selected file(s) on the remote repository to prevent someone else from changing it.",
	/* 3 */	"Unlocks the selected file(s) so everybody can commit changes on the selected file(s).",
	/* 4 */	"Editing the selected file(s) will allow other users to know that you currently work "
			"on these file(s) (by using the Editors command).",
	/* 5 */	"Unediting the selected file(s) will erase your name from the list of people currently "
			"editing the file(s).",
	/* 6 */	"Adds a Watch on the selected file(s) so you get notified when the file(s) change on the remote "
			"repository.",
	/* 7 */	"Removes the Watch from the selected file(s) so you stop being notified when the file(s) change "
			"on the remote repository.",
	/* 8 */	"Watchers of the selected file(s) list all the users who have a watch on the file(s).",
	/* 9 */	"Editors of the selected file(s) list all the users who are currently editing the file(s)."
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MonitorsfilesMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {MonitorsfilesHmnu, 1, MonitorsfilesHmnu, 1, MonitorsfilesHmnu, 1, MonitorsfilesHmnu, 1 },
	HMStringResItem {MonitorsfilesHmnu, 2, MonitorsfilesHmnu, 2, MonitorsfilesHmnu, 2, MonitorsfilesHmnu, 2 },
	HMStringResItem {MonitorsfilesHmnu, 3, MonitorsfilesHmnu, 3, MonitorsfilesHmnu, 3, MonitorsfilesHmnu, 3 },
	HMStringResItem {MonitorsfilesHmnu, 4, MonitorsfilesHmnu, 4, MonitorsfilesHmnu, 4, MonitorsfilesHmnu, 4 },
	HMStringResItem {MonitorsfilesHmnu, 5, MonitorsfilesHmnu, 5, MonitorsfilesHmnu, 5, MonitorsfilesHmnu, 5 },
	HMStringResItem {MonitorsfilesHmnu, 6, MonitorsfilesHmnu, 6, MonitorsfilesHmnu, 6, MonitorsfilesHmnu, 6 },
	HMStringResItem {MonitorsfilesHmnu, 7, MonitorsfilesHmnu, 7, MonitorsfilesHmnu, 7, MonitorsfilesHmnu, 7 },
	HMSkipItem {},
	HMStringResItem {MonitorsfilesHmnu, 8, MonitorsfilesHmnu, 8, MonitorsfilesHmnu, 8, MonitorsfilesHmnu, 8 },
	HMStringResItem {MonitorsfilesHmnu, 9, MonitorsfilesHmnu, 9, MonitorsfilesHmnu, 9, MonitorsfilesHmnu, 9 }
}};

#define MonitorsfoldersMenu	176
#define MonitorsfoldersHmnu hMenuBase+MonitorsfoldersMenu
resource 'STR#' (MonitorsfoldersHmnu, purgeable) {{
	/* 1 */	"Menu",
	/* 2 */	"Locks the enclosed file(s) on the remote repository to prevent someone else from changing it.",
	/* 3 */	"Unlocks the enclosed file(s) so everybody can commit changes on the enclosed file(s).",
	/* 4 */	"Editing the enclosed file(s) will allow other users to know that you currently work "
			"on these file(s) (by using the Editors command).",
	/* 5 */	"Unediting the enclosed file(s) will erase your name from the list of people currently "
			"editing the file(s).",
	/* 6 */	"Adds a Watch on the enclosed file(s) so you get notified when the file(s) changed on the remote "
			"repository.",
	/* 7 */	"Removes a Watch from the enclosed file(s) so you stop being notified when the file(s) change on "
			"the remote repository.",
	/* 8 */	"Watchers of the enclosed file(s) list all the users who have a watch on the file(s).",
	/* 9 */	"Editors of the enclosed file(s) list all the users who are currently editing the file(s).",
	/* 10 */	"Release folder will release all your editing files enclosed in this folder.\n"
				"\n"
				"It also allows you to know what you changed locally in this folder."
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MonitorsfoldersMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {MonitorsfoldersHmnu, 1, MonitorsfoldersHmnu, 1, MonitorsfoldersHmnu, 1, MonitorsfoldersHmnu, 1 },
	HMStringResItem {MonitorsfoldersHmnu, 2, MonitorsfoldersHmnu, 2, MonitorsfoldersHmnu, 2, MonitorsfoldersHmnu, 2 },
	HMStringResItem {MonitorsfoldersHmnu, 3, MonitorsfoldersHmnu, 3, MonitorsfoldersHmnu, 3, MonitorsfoldersHmnu, 3 },
	HMStringResItem {MonitorsfoldersHmnu, 4, MonitorsfoldersHmnu, 4, MonitorsfoldersHmnu, 4, MonitorsfoldersHmnu, 4 },
	HMStringResItem {MonitorsfoldersHmnu, 5, MonitorsfoldersHmnu, 5, MonitorsfoldersHmnu, 5, MonitorsfoldersHmnu, 5 },
	HMStringResItem {MonitorsfoldersHmnu, 6, MonitorsfoldersHmnu, 6, MonitorsfoldersHmnu, 6, MonitorsfoldersHmnu, 6 },
	HMStringResItem {MonitorsfoldersHmnu, 7, MonitorsfoldersHmnu, 7, MonitorsfoldersHmnu, 7, MonitorsfoldersHmnu, 7 },
	HMSkipItem {},
	HMStringResItem {MonitorsfoldersHmnu, 8, MonitorsfoldersHmnu, 8, MonitorsfoldersHmnu, 8, MonitorsfoldersHmnu, 8 },
	HMStringResItem {MonitorsfoldersHmnu, 9, MonitorsfoldersHmnu, 9, MonitorsfoldersHmnu, 9, MonitorsfoldersHmnu, 9 },
	HMSkipItem {},
	HMStringResItem {MonitorsfoldersHmnu, 10, MonitorsfoldersHmnu, 10, MonitorsfoldersHmnu, 10, MonitorsfoldersHmnu, 10 }
}};

#define MonitorsselectionMenu	177
#define MonitorsselectionHmnu hMenuBase+MonitorsselectionMenu
resource 'STR#' (MonitorsselectionHmnu, purgeable) {{
	/* 1 */	"Menu",
	/* 2 */	"Locks the selected file(s) on the remote repository to prevent someone else from changing it.",
	/* 3 */	"Unlocks the selected file(s) so everybody can commit changes on the selected file(s).",
	/* 4 */	"Adds a Watch on the selected file(s) so you get notified when the file(s) changed on the remote "
			"repository.",
	/* 5 */	"Removes a Watch from the selected file(s) so you stop being notified when the file(s) changed on "
			"the remote repository.",
	/* 6 */	"Editing the selected file(s) will allow other users to know that you currently work "
			"on these file(s) (by using the Editors command).",
	/* 7 */	"Unediting the selected file(s) will erase your name from the list of people currently "
			"editing the file(s).",
	/* 8 */	"Will release all your selected files.\n"
			"\n"
			"It also allows you to know what you changed locally in this folder.",
	/* 9 */	"Watchers of the selected file(s) list all the users who have a watch on the file(s).",
	/* 10 */	"Editors of the selected file(s) list all the users who are currently editing the file(s)."
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MonitorsselectionMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {MonitorsselectionHmnu, 1, MonitorsselectionHmnu, 1, MonitorsselectionHmnu, 1, MonitorsselectionHmnu, 1 },
	HMStringResItem {MonitorsselectionHmnu, 2, MonitorsselectionHmnu, 2, MonitorsselectionHmnu, 2, MonitorsselectionHmnu, 2 },
	HMStringResItem {MonitorsselectionHmnu, 3, MonitorsselectionHmnu, 3, MonitorsselectionHmnu, 3, MonitorsselectionHmnu, 3 },
	HMStringResItem {MonitorsselectionHmnu, 4, MonitorsselectionHmnu, 4, MonitorsselectionHmnu, 4, MonitorsselectionHmnu, 4 },
	HMStringResItem {MonitorsselectionHmnu, 5, MonitorsselectionHmnu, 5, MonitorsselectionHmnu, 5, MonitorsselectionHmnu, 5 },
	HMStringResItem {MonitorsselectionHmnu, 6, MonitorsselectionHmnu, 6, MonitorsselectionHmnu, 6, MonitorsselectionHmnu, 6 },
	HMStringResItem {MonitorsselectionHmnu, 7, MonitorsselectionHmnu, 7, MonitorsselectionHmnu, 7, MonitorsselectionHmnu, 7 },
	HMSkipItem {},
	HMStringResItem {MonitorsselectionHmnu, 8, MonitorsselectionHmnu, 8, MonitorsselectionHmnu, 8, MonitorsselectionHmnu, 8 },
	HMSkipItem {},
	HMStringResItem {MonitorsselectionHmnu, 9, MonitorsselectionHmnu, 9, MonitorsselectionHmnu, 9, MonitorsselectionHmnu, 9 },
	HMStringResItem {MonitorsselectionHmnu, 10, MonitorsselectionHmnu, 10, MonitorsselectionHmnu, 10, MonitorsselectionHmnu, 10 }
}};

#define RecentBrowserMenu	178
#define RecentBrowserHmnu hMenuBase+RecentBrowserMenu
resource 'STR#' (RecentBrowserHmnu, purgeable) {{

}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (RecentBrowserMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {}
}};

#define MacrosAdminMenu	179
#define MacrosAdminHmnu hMenuBase+MacrosAdminMenu
resource 'STR#' (MacrosAdminHmnu, purgeable) {{
	/* 1 */	"Menu"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MacrosAdminMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {MacrosAdminHmnu, 1, MacrosAdminHmnu, 1, MacrosAdminHmnu, 1, MacrosAdminHmnu, 1 }
}};

#define MacrosFilesMenu	180
#define MacrosFilesHmnu hMenuBase+MacrosFilesMenu
resource 'STR#' (MacrosFilesHmnu, purgeable) {{
	/* 1 */	"Menu"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MacrosFilesMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {MacrosFilesHmnu, 1, MacrosFilesHmnu, 1, MacrosFilesHmnu, 1, MacrosFilesHmnu, 1 }
}};

#define MacrosFolderMenu	181
#define MacrosFolderHmnu hMenuBase+MacrosFolderMenu
resource 'STR#' (MacrosFolderHmnu, purgeable) {{
	/* 1 */	"Menu"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MacrosFolderMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {MacrosFolderHmnu, 1, MacrosFolderHmnu, 1, MacrosFolderHmnu, 1, MacrosFolderHmnu, 1 }
}};

#define MacrosSelMenu	182
#define MacrosSelHmnu hMenuBase+MacrosSelMenu
resource 'STR#' (MacrosSelHmnu, purgeable) {{
	/* 1 */	"Menu"
}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (MacrosSelMenu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {MacrosSelHmnu, 1, MacrosSelHmnu, 1, MacrosSelHmnu, 1, MacrosSelHmnu, 1 }
}};

#define h12345Menu	12345
#define h12345Hmnu hMenuBase+h12345Menu
resource 'STR#' (h12345Hmnu, purgeable) {{

}};

/* The 'hmnu' resource connects menu items to STR# items.
 */
resource 'hmnu' (h12345Menu, purgeable) {
	HelpMgrVersion,
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMSkipItem {}
}};

#define h129Ditl	129
#define h129Hdlg hDlogBase+h129Ditl
resource 'STR#' (h129Hdlg, purgeable) {{
	/* 1 */	"^0",
	/* 2 */	"^1",
	/* 3 */	"^2"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (h129Ditl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, h129Hdlg, 1, h129Hdlg, 1, h129Hdlg, 1, h129Hdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, h129Hdlg, 2, h129Hdlg, 2, h129Hdlg, 2, h129Hdlg, 2 },
	HMStringResItem {{0,0}, {0,0,0,0}, h129Hdlg, 3, h129Hdlg, 3, h129Hdlg, 3, h129Hdlg, 3 }
}};

#define h201Ditl	201
#define h201Hdlg hDlogBase+h201Ditl
resource 'STR#' (h201Hdlg, purgeable) {{
	/* 1 */	"Save",
	/* 2 */	"Cancel",
	/* 3 */	"Don’t Save",
	/* 4 */	"Save changes to the ^0 document “^1” before closing?"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (h201Ditl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, h201Hdlg, 1, h201Hdlg, 1, h201Hdlg, 1, h201Hdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, h201Hdlg, 2, h201Hdlg, 2, h201Hdlg, 2, h201Hdlg, 2 },
	HMStringResItem {{0,0}, {0,0,0,0}, h201Hdlg, 3, h201Hdlg, 3, h201Hdlg, 3, h201Hdlg, 3 },
	HMStringResItem {{0,0}, {0,0,0,0}, h201Hdlg, 4, h201Hdlg, 4, h201Hdlg, 4, h201Hdlg, 4 }
}};

#define h202Ditl	202
#define h202Hdlg hDlogBase+h202Ditl
resource 'STR#' (h202Hdlg, purgeable) {{
	/* 1 */	"Save",
	/* 2 */	"Cancel",
	/* 3 */	"Don’t Save",
	/* 4 */	"Save changes to the ^0 document “^1” before quitting?"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (h202Ditl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, h202Hdlg, 1, h202Hdlg, 1, h202Hdlg, 1, h202Hdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, h202Hdlg, 2, h202Hdlg, 2, h202Hdlg, 2, h202Hdlg, 2 },
	HMStringResItem {{0,0}, {0,0,0,0}, h202Hdlg, 3, h202Hdlg, 3, h202Hdlg, 3, h202Hdlg, 3 },
	HMStringResItem {{0,0}, {0,0,0,0}, h202Hdlg, 4, h202Hdlg, 4, h202Hdlg, 4, h202Hdlg, 4 }
}};

#define h203Ditl	203
#define h203Hdlg hDlogBase+h203Ditl
resource 'STR#' (h203Hdlg, purgeable) {{
	/* 1 */	"Revert",
	/* 2 */	"Cancel",
	/* 3 */	"Revert to the last saved version of “^0”?"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (h203Ditl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, h203Hdlg, 1, h203Hdlg, 1, h203Hdlg, 1, h203Hdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, h203Hdlg, 2, h203Hdlg, 2, h203Hdlg, 2, h203Hdlg, 2 },
	HMStringResItem {{0,0}, {0,0,0,0}, h203Hdlg, 3, h203Hdlg, 3, h203Hdlg, 3, h203Hdlg, 3 }
}};

#define h251Ditl	251
#define h251Hdlg hDlogBase+h251Ditl
resource 'STR#' (h251Hdlg, purgeable) {{
	/* 1 */	"OK",
	/* 2 */	"Exception Thrown\n\nError # ^0\nFile: ^1\nLine # ^2"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (h251Ditl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, h251Hdlg, 1, h251Hdlg, 1, h251Hdlg, 1, h251Hdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, h251Hdlg, 2, h251Hdlg, 2, h251Hdlg, 2, h251Hdlg, 2 }
}};

#define h252Ditl	252
#define h252Hdlg hDlogBase+h252Ditl
resource 'STR#' (h252Hdlg, purgeable) {{
	/* 1 */	"OK",
	/* 2 */	"Signal Raised\n\nCondition: ^0\nFile: ^1\nLine # ^2"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (h252Ditl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, h252Hdlg, 1, h252Hdlg, 1, h252Hdlg, 1, h252Hdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, h252Hdlg, 2, h252Hdlg, 2, h252Hdlg, 2, h252Hdlg, 2 }
}};

#define stdfolderDitl	6042
#define stdfolderHdlg hDlogBase+stdfolderDitl
resource 'STR#' (stdfolderHdlg, purgeable) {{
	/* 1 */	"Open",
	/* 2 */	"Cancel",
	/* 3 */	"user",
	/* 4 */	"user",
	/* 5 */	"Eject",
	/* 6 */	"Desktop",
	/* 7 */	"user",
	/* 8 */	"user",
	/* 9 */	"pict",
	/* 10 */	"Select current folder:",
	/* 11 */	"text",
	/* 12 */	"text",
	/* 13 */	"pict",
	/* 14 */	"pict",
	/* 15 */	"pict",
	/* 16 */	"pict"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (stdfolderDitl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 1, stdfolderHdlg, 1, stdfolderHdlg, 1, stdfolderHdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 2, stdfolderHdlg, 2, stdfolderHdlg, 2, stdfolderHdlg, 2 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 3, stdfolderHdlg, 3, stdfolderHdlg, 3, stdfolderHdlg, 3 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 4, stdfolderHdlg, 4, stdfolderHdlg, 4, stdfolderHdlg, 4 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 5, stdfolderHdlg, 5, stdfolderHdlg, 5, stdfolderHdlg, 5 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 6, stdfolderHdlg, 6, stdfolderHdlg, 6, stdfolderHdlg, 6 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 7, stdfolderHdlg, 7, stdfolderHdlg, 7, stdfolderHdlg, 7 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 8, stdfolderHdlg, 8, stdfolderHdlg, 8, stdfolderHdlg, 8 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 9, stdfolderHdlg, 9, stdfolderHdlg, 9, stdfolderHdlg, 9 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 10, stdfolderHdlg, 10, stdfolderHdlg, 10, stdfolderHdlg, 10 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 11, stdfolderHdlg, 11, stdfolderHdlg, 11, stdfolderHdlg, 11 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 12, stdfolderHdlg, 12, stdfolderHdlg, 12, stdfolderHdlg, 12 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 13, stdfolderHdlg, 13, stdfolderHdlg, 13, stdfolderHdlg, 13 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 14, stdfolderHdlg, 14, stdfolderHdlg, 14, stdfolderHdlg, 14 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 15, stdfolderHdlg, 15, stdfolderHdlg, 15, stdfolderHdlg, 15 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 16, stdfolderHdlg, 16, stdfolderHdlg, 16, stdfolderHdlg, 16 }
}};

#define stdfolderDitl	6043
#define stdfolderHdlg hDlogBase+stdfolderDitl
resource 'STR#' (stdfolderHdlg, purgeable) {{
	/* 1 */	"Add",
	/* 2 */	"Cancel",
	/* 3 */	"user",
	/* 4 */	"user",
	/* 5 */	"Eject",
	/* 6 */	"Desktop",
	/* 7 */	"user",
	/* 8 */	"user",
	/* 9 */	"pict",
	/* 10 */	"Add all",
	/* 11 */	"Remove",
	/* 12 */	"user",
	/* 13 */	"text",
	/* 14 */	"Remove all",
	/* 15 */	"Done"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (stdfolderDitl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 1, stdfolderHdlg, 1, stdfolderHdlg, 1, stdfolderHdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 2, stdfolderHdlg, 2, stdfolderHdlg, 2, stdfolderHdlg, 2 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 3, stdfolderHdlg, 3, stdfolderHdlg, 3, stdfolderHdlg, 3 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 4, stdfolderHdlg, 4, stdfolderHdlg, 4, stdfolderHdlg, 4 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 5, stdfolderHdlg, 5, stdfolderHdlg, 5, stdfolderHdlg, 5 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 6, stdfolderHdlg, 6, stdfolderHdlg, 6, stdfolderHdlg, 6 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 7, stdfolderHdlg, 7, stdfolderHdlg, 7, stdfolderHdlg, 7 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 8, stdfolderHdlg, 8, stdfolderHdlg, 8, stdfolderHdlg, 8 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 9, stdfolderHdlg, 9, stdfolderHdlg, 9, stdfolderHdlg, 9 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 10, stdfolderHdlg, 10, stdfolderHdlg, 10, stdfolderHdlg, 10 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 11, stdfolderHdlg, 11, stdfolderHdlg, 11, stdfolderHdlg, 11 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 12, stdfolderHdlg, 12, stdfolderHdlg, 12, stdfolderHdlg, 12 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 13, stdfolderHdlg, 13, stdfolderHdlg, 13, stdfolderHdlg, 13 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 14, stdfolderHdlg, 14, stdfolderHdlg, 14, stdfolderHdlg, 14 },
	HMStringResItem {{0,0}, {0,0,0,0}, stdfolderHdlg, 15, stdfolderHdlg, 15, stdfolderHdlg, 15, stdfolderHdlg, 15 }
}};

#define GetDirectoryDitl	10240
#define GetDirectoryHdlg hDlogBase+GetDirectoryDitl
resource 'STR#' (GetDirectoryHdlg, purgeable) {{
	/* 1 */	"Open",
	/* 2 */	"Hidden",
	/* 3 */	"Cancel",
	/* 4 */	"user",
	/* 5 */	"Eject",
	/* 6 */	"Drive",
	/* 7 */	"user",
	/* 8 */	"user",
	/* 9 */	"user",
	/* 10 */	"text",
	/* 11 */	"Directory",
	/* 12 */	"Select Current Directory:",
	/* 13 */	"Select a Folder"
}};

/* The 'dlg' resource connects ditl items to STR# items.
 */
resource 'hdlg' (GetDirectoryDitl, purgeable) {
	HelpMgrVersion,
	0,	/* start with first item */
	hmDefaultOptions,
	0,	/* balloon def id */
	0,	/* var code, hang right */
	HMSkipItem {},	/* default item */
	{
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 1, GetDirectoryHdlg, 1, GetDirectoryHdlg, 1, GetDirectoryHdlg, 1 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 2, GetDirectoryHdlg, 2, GetDirectoryHdlg, 2, GetDirectoryHdlg, 2 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 3, GetDirectoryHdlg, 3, GetDirectoryHdlg, 3, GetDirectoryHdlg, 3 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 4, GetDirectoryHdlg, 4, GetDirectoryHdlg, 4, GetDirectoryHdlg, 4 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 5, GetDirectoryHdlg, 5, GetDirectoryHdlg, 5, GetDirectoryHdlg, 5 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 6, GetDirectoryHdlg, 6, GetDirectoryHdlg, 6, GetDirectoryHdlg, 6 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 7, GetDirectoryHdlg, 7, GetDirectoryHdlg, 7, GetDirectoryHdlg, 7 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 8, GetDirectoryHdlg, 8, GetDirectoryHdlg, 8, GetDirectoryHdlg, 8 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 9, GetDirectoryHdlg, 9, GetDirectoryHdlg, 9, GetDirectoryHdlg, 9 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 10, GetDirectoryHdlg, 10, GetDirectoryHdlg, 10, GetDirectoryHdlg, 10 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 11, GetDirectoryHdlg, 11, GetDirectoryHdlg, 11, GetDirectoryHdlg, 11 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 12, GetDirectoryHdlg, 12, GetDirectoryHdlg, 12, GetDirectoryHdlg, 12 },
	HMStringResItem {{0,0}, {0,0,0,0}, GetDirectoryHdlg, 13, GetDirectoryHdlg, 13, GetDirectoryHdlg, 13, GetDirectoryHdlg, 13 }
}};
