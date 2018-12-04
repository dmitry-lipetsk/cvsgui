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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

/*
 * 
 */

#include "stdafx.h"

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#	include <fcntl.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#	include <sys/wait.h>
#endif

#if qGTK
#	include <gtk/gtk.h>
#	include <signal.h>
// glade generated that
extern "C"
{
#include "support.h"
#include "interface.h"
}
#endif

#include "UCvsApp.h"
#include "UCvsCommands.h"
#include "UCvsConsole.h"
#include "UCvsFrame.h"
#include "UCvsFolders.h"
#include "UCvsFiles.h"
#include "UCvsStatusbar.h"
#include "TclGlue.h"
#include "GetPrefs.h"
#include "CvsArgs.h"
#include "CvsPrefs.h"
#include "AppConsole.h"
#include "MacrosSetup.h"
#include "Authen.h"
#include "PythonGlue.h"
#include "getopt.h"

UIMPLEMENT_DYNAMIC (UCvsApp, UWidget)
UBEGIN_MESSAGE_MAP (UCvsApp, UWidget)
ON_TIMER (UCvsApp::kProcessTimer, UCvsApp::OnMainTimer)
UEND_MESSAGE_MAP ()
	UCvsApp *UCvsApp::gApp = 0L;
	CvsProcess *UCvsApp::gCvsProcess = 0L;

     static void usage (void)
{
	fprintf (stderr, "gcvs [--root=path]\n");
}

UCvsApp::UCvsApp (int argc, char **argv):
UWidget (::UEventGetWidID ()),
gCvsRunning (false),
gCvsStopping (false)
{
	gApp = this;

#if qGTK
# ifdef DEBUG
	add_pixmap_directory (PACKAGE_SOURCE_DIR "/pixmaps");
# endif
	add_pixmap_directory (PACKAGE_DATA_DIR "/pixmaps");
	add_pixmap_directory (PACKAGE_PIXMAPS_DIR);
#endif

	// load the persistent values (i.e. preferences) and the locate the macros
	gCvsPrefs.load ();
	
	AddTimer (50, kProcessTimer);

	m_mainFrame = new UCvsFrame();
	
	if (argc > 0)
	{
		static struct option long_options[] = {
			{"root", required_argument, 0L, 1},
			{0, 0, 0, 0}
		};
		optind = 0;
		int option_index = 0, c;
		while ((c = getopt_long
			(argc, argv, "+", long_options,
			 &option_index)) != EOF)
		{
			switch (c)
			{
			case 1:
				UCvsFolders::SetDefaultRoot (optarg);
				break;
			default:
				cvs_err (_("Warning : unknown option\n"));
				usage ();
				break;
			}
		}

	}
}

UCvsApp::~UCvsApp ()
{
}

void
UCvsApp::SayWelcome (void)
{
	// say hello from the application, optionnaly prompt for the CVSROOT
	cvs_out (_("Welcome to %s %s\n"), PACKAGE, VERSION);
	if (gCvsPrefs.empty ())
		CompatGetPrefs ();
	else
		cvs_out ("CVSROOT: %s (%s)\n", (const char *) gCvsPrefs,
			 gAuthen.kindstr ());

	StartScriptEngine();
}

void
UCvsApp::SetCvsRunning (bool state)
{
	if (state ^ gCvsRunning)
	{
		gCvsRunning = state;
	}
}

void
UCvsApp::SetCvsStopping (bool state)
{
	if (state ^ gCvsStopping)
	{
		gCvsStopping = state;
	}
}

UCvsStatusbar *
UCvsApp::GetStatusBar (void)
{
	UCmdTarget *cmd = UEventQueryWidget (kUCvsStatusbarID);
	if (cmd != 0L)
		return (UCvsStatusbar *) cmd;

	return 0L;
}

void
UCvsApp::SetStatusBarText(const char* text)
{
	UCvsStatusbar* statusBar = GetStatusBar();
	if(statusBar != 0L) {
		UEventSendMessage(GetStatusBar()->GetWidID(), EV_STATUSBAR_TEXT, kUMainWidget, (void*)text);
	}
}

UCvsFolders *
UCvsApp::GetBrowserView (void)
{
	UCmdTarget *cmd = UEventQueryWidget (kUCvsFoldersID);
	if (cmd != 0L && cmd->IsKindOf (URUNTIME_CLASS (UCvsFolders)))
		return (UCvsFolders *) cmd;

	return 0L;
}

UCvsFiles *
UCvsApp::GetFilesView (void)
{
	UCmdTarget *cmd = UEventQueryWidget (kUCvsFilesID);
	if (cmd != 0L && cmd->IsKindOf (URUNTIME_CLASS (UCvsFiles)))
		return (UCvsFiles *) cmd;

	return 0L;
}

UCvsConsole *
UCvsApp::GetConsole (void)
{
	UCmdTarget *cmd = UEventQueryWidget (kUMainConsoleID);
	if (cmd != 0L && cmd->IsKindOf (URUNTIME_CLASS (UCvsConsole)))
		return (UCvsConsole *) cmd;

	return 0L;
}

const char *
UCvsApp::GetAppName () const
{
#ifdef macintosh
	return "MacCvs";
#elif defined(WIN32)
	return "WinCvs";
#elif defined(qUnix)
	return PACKAGE;
#endif
}

const char *
UCvsApp::GetAppVersion () const
{
#ifdef macintosh

#elif defined(WIN32)

#elif defined(qUnix)
	return VERSION;
#endif
}


void UCvsApp::PeekPumpAndIdle(bool doIdle)
{
	while (gtk_events_pending())	
		gtk_main_iteration();
					
	UEventGiveTime();
	if(doIdle) {
		// nothing to do on idle time yet
		/*long lIdle = 0;
		
		while( OnIdle(lIdle++) )
			; */
	}
}

void UCvsApp::RestartScriptEngine()
{
	StopScriptEngine();
	StartScriptEngine(true);
}

/*!
	Start the script engine
*/
void UCvsApp::StartScriptEngine(const bool forceReload /*= false*/)
{
	CPython_Interp::InitPythonSupport(forceReload);
	
	//MacrosReloadAll();
	PyMacrosReloadAll();

	m_hasTCL = CTcl_Interp::IsAvail(forceReload);
	m_hasPY = CPython_Interp::IsAvail();
	
	if( gCvsPrefs.GetUseShell() == SHELL_PYTHON && !m_hasPY && m_hasTCL )
	{
		cvs_out("Python is not available, switching the shell to TCL\n");
		gCvsPrefs.SetUseShell(SHELL_TCL);
	}
	else if( gCvsPrefs.GetUseShell() == SHELL_TCL && m_hasPY && !m_hasTCL )
	{
		cvs_out("TCL is not available, switching the shell to Python\n");
		gCvsPrefs.SetUseShell(SHELL_PYTHON);
	}
	
	if( gCvsPrefs.GetUseShell() == SHELL_TCL && m_hasTCL )
	{
		cvs_out("TCL is available, shell is enabled : help (select and press enter)\n");
	}
	
	if( !m_hasTCL && !m_hasPY )
	{
		cvs_err("TCL or Python are not available, shell is disabled\n");
	}
}

/*!
	Stop the script engine
*/
void UCvsApp::StopScriptEngine()
{
	PyInvalUICache();
	CPython_Interp::ClosePythonSupport();
}
// retrieve filtering model to use
KiFilterModel* UCvsApp::GetFilterModel() const
{
        return m_mainFrame->GetFilterModel();
}

// retrieve recursion model to use
KiRecursionModel* UCvsApp::GetRecursionModel() const
{
        return m_mainFrame->GetRecursionModel();
}

KiIgnoreModel* UCvsApp::GetIgnoreModel() const
{
        return m_mainFrame->GetIgnoreModel();
}

/*const char *
UCvsApp::GetAppLibFolder () const
{
#if qUnix
	return EXECLIB;
#else
	return 0L;
#endif
}*/

#if qUnix
static void
set_cloexec (gint fd)
{
	fcntl (fd, F_SETFD, FD_CLOEXEC);
}

static int
gnome_execute_async_with_env_fds (const char *dir, int argc,
				  char *const argv[], int envc,
				  char *const envv[], gboolean close_fds)
{
	int parent_comm_pipes[2], child_comm_pipes[2];
	int child_errno, itmp, i, open_max;
	int res;
	char **cpargv;
	pid_t child_pid, immediate_child_pid;	/* XXX this routine assumes
						 * pid_t is signed */

	if (pipe (parent_comm_pipes))
		return -1;

	child_pid = immediate_child_pid = fork ();

	switch (child_pid)
	{
	case -1:
		close (parent_comm_pipes[0]);
		close (parent_comm_pipes[1]);
		return -1;

	case 0:		/* START PROCESS 1: child */
		child_pid = -1;
		res = pipe (child_comm_pipes);
		close (parent_comm_pipes[0]);
		if (!res)
			child_pid = fork ();

		switch (child_pid)
		{
		case -1:
			itmp = errno;
			child_pid = -1;	/* simplify parent code */
			write (parent_comm_pipes[1], &child_pid,
			       sizeof (child_pid));
			write (parent_comm_pipes[1], &itmp, sizeof (itmp));
			close (child_comm_pipes[0]);
			close (child_comm_pipes[1]);
			_exit (0);
			break;	/* END PROCESS 1: monkey in the middle dies */

		default:
		{
			char buf[16];

			close (child_comm_pipes[1]);
			while ((res =
				read (child_comm_pipes[0], buf,
				      sizeof (buf))) > 0)
				write (parent_comm_pipes[1], buf, res);
			close (child_comm_pipes[0]);
			_exit (0);	/* END PROCESS 1: monkey in the middle dies */
		}
			break;

		case 0:	/* START PROCESS 2: child of child */
			close (parent_comm_pipes[1]);
			/* pre-exec setup */
			close (child_comm_pipes[0]);
			set_cloexec (child_comm_pipes[1]);
			child_pid = getpid ();
			res = write (child_comm_pipes[1], &child_pid,
				     sizeof (child_pid));

			if (envv)
			{
				for (itmp = 0; itmp < envc; itmp++)
					putenv (envv[itmp]);
			}

			if (dir)
				chdir (dir);

			cpargv = (char **) malloc ((argc + 1) *
						   sizeof (char *));
			memcpy (cpargv, argv, argc * sizeof (char *));
			cpargv[argc] = NULL;

			if (close_fds)
			{
				int stdinfd;
				/* Close all file descriptors but stdin stdout and stderr */
				open_max = sysconf (_SC_OPEN_MAX);
				for (i = 3; i < open_max; i++)
					set_cloexec (i);

				if (child_comm_pipes[1] != 0)
				{
					close (0);
					/* Open stdin as being nothingness, so that if someone tries to
					 * read from this they don't hang up the whole GNOME session. BUGFIX #1548 */
					stdinfd =
						open ("/dev/null", O_RDONLY);
					g_assert (stdinfd >= 0);
					if (stdinfd != 0)
					{
						dup2 (stdinfd, 0);
						close (stdinfd);
					}
				}
			}
			signal (SIGPIPE, SIG_DFL);
			/* doit */
			execvp (cpargv[0], cpargv);
			free (cpargv);

			/* failed */
			itmp = errno;
			write (child_comm_pipes[1], &itmp, sizeof (itmp));
			_exit (1);
			break;	/* END PROCESS 2 */
		}
		break;

	default:		/* parent process */
		/* do nothing */
		break;
	}

	close (parent_comm_pipes[1]);

	res = read (parent_comm_pipes[0], &child_pid, sizeof (child_pid));
	if (res != sizeof (child_pid))
	{
		g_message ("res is %d instead of %d", res,
			   (int)sizeof (child_pid));
		child_pid = -1;	/* really weird things happened */
	}
	else if (read
		 (parent_comm_pipes[0], &child_errno,
		  sizeof (child_errno)) == sizeof (child_errno))
	{
		errno = child_errno;
		child_pid = -1;
	}

	/* do this after the read's in case some OS's handle blocking on pipe writes
	 * differently */
	waitpid (immediate_child_pid, &itmp, 0);	/* eat zombies */

	close (parent_comm_pipes[0]);

	if (child_pid < 0)
		g_message ("gnome_execute_async_with_env_fds: returning %d",
			   child_pid);

	return child_pid;
}
#endif

void
UCvsApp::Execute (int argc, char *const *argv)
{
#if qUnix
	// The Authentication code (AnthenModel) nulls home
	// after a CVS call so rather that mess with it (somebody
	// apparently meant to do it) we will make sure HOME is set
	// for our child process here.
	CvsArgs args(false);
	std::string home;
	home = "HOME=";
	home += gCvsPrefs.Home();
	args.add(home.c_str());

	gnome_execute_async_with_env_fds (0L, argc, argv, args.Argc(), args.Argv(), true);
#endif
}

void
UCvsApp::OnMainTimer (long time)
{
	if (gCvsProcess != 0L)
	{
		cvs_process_give_time ();
		if (!cvs_process_is_active (gCvsProcess))
		{
			SetCvsRunning (false);
			SetCvsProcess((CvsProcess*)0L);
			DoValidUI (false);
			GetFilesView()->ResetView (false, false);
//			fprintf(stderr, "Update\n");
		}
//	fprintf(stderr, "Update2\n");
	}
}

#if qGTK
extern GtkWidget *MainWindow;
#	define NEWDLG(name) \
	void *UCreate_##name(void *Parent) \
	{	GtkWidget *wid = create_##name(); \
		GtkWidget *pp = Parent ? (GtkWidget *)Parent : MainWindow; \
		if( wid && pp) gtk_window_set_transient_for (GTK_WINDOW (wid), GTK_WINDOW (pp)); \
		return wid; \
	}
NEWDLG (MainWindow)
NEWDLG (GraphWindow)
NEWDLG (Alert)
NEWDLG (gcvs_Settings)
NEWDLG (UpdateDlg)
NEWDLG (PasswdDlg)
NEWDLG (CommitDlg)
NEWDLG (DiffDlg)
NEWDLG (CheckoutDlg)
NEWDLG (LogDlg)
NEWDLG (ImportFilterDlg)
NEWDLG (ImpWarningDlg)
NEWDLG (ImpEntryDlg)
NEWDLG (ImportDlg)
NEWDLG (CmdlineDlg)
NEWDLG (TagModDlg)
NEWDLG (BranchModDlg)
NEWDLG (UntagModDlg)
NEWDLG (TagDlg)
NEWDLG (BranchDlg)
NEWDLG (UntagDlg)
NEWDLG (SettingsDlg)
NEWDLG (AboutDlg)
NEWDLG (SelTagBranchDlg)
NEWDLG (SelDetailsDlg)
NEWDLG (AskYesNoDlg)
#endif

void *
UCreate_pixmap (UWidget * parent, const char *fileIcon)
{
#if qGTK
	GtkWidget *wid = (GtkWidget *) parent->GetWidget (kUMainWidget);
	return GTK_PIXMAP (create_pixmap (wid, fileIcon));
#else
	return 0L;
#endif
}

bool IsAppEnabled()
{
    UCvsApp *app = UCvsApp::gApp;
	return !app->IsCvsRunning() && !gCvsPrefs.empty();
}

bool IsEscKeyPressed()
{
	return UCvsApp::gApp->IsEscKeyPressed();
}
