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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * PromptFiles.cpp --- class to pick set of files and folder
 */

#include "stdafx.h"
#include <string>

#ifdef TARGET_OS_MAC
#	include "GUSIInternal.h"
#	include "GUSIFileSpec.h"
#	include <string.h>
#	include <TextUtils.h>
#	include <LowMem.h>
#	include "stdfolder.h"
#	include "stdmfiles.h"
#	include "VolsPaths.h"
#endif /* TARGET_OS_MAC */

#if qMacPP
#	include <UConditionalDialogs.h>
#	include <UNavServicesDialogs.h>
#endif

#ifdef qGTK
#	include <gtk/gtk.h>
#	include <sys/stat.h>
#endif /* qGTK */

#include "PromptFiles.h"
#include "MultiFiles.h"
#include "AppConsole.h"
#include "CvsCommands.h"

#ifdef qQT
#	include "qcvsapp.h"
#	include "CPStr.h"
#	include <qfiledialog.h>
#	include <vector>
#	include <stdlib.h>
#endif /* qQT */

#ifdef WIN32
#include "MfFileDialog.h"

#	ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#	endif
#endif /* WIN32 */

using namespace std;

#ifdef WIN32
// Open file dialog filter defines
#define FILTER_SEPARATOR	"|"
#define FILTER_EXECUTABLE	"Executable Files (*.exe;*.bat;*.cmd)" FILTER_SEPARATOR "*.exe; *.bat; *.cmd"
#define FILTER_DLL			"DLL Files (*.dll)" FILTER_SEPARATOR "*.dll"
#define FILTER_ALL			"All Files (*.*)" FILTER_SEPARATOR "*.*"
#define FILTER_END			FILTER_SEPARATOR FILTER_SEPARATOR

#endif


#ifdef TARGET_OS_MAC   //{ Mac - Part	
extern "C" 
{
	/// Enumerate the multiple files
	static void MacEnumFiles_hook(UFSSpec* newFile, MultiFiles* mf)
	{
		UInt8 utf8path[PATH_MAX];
		if( FSRefMakePath(newFile, utf8path, sizeof(utf8path)) == noErr )
		{
			std::string  uppath, cname;
			SplitPath(utf8_to_posix(utf8path), uppath, cname);
			mf->newfile(cname, newFile);
		}
	}
}
#endif  //{ TARGET_OS_MAC 

#ifdef qQT  //{ QT - Part	

extern "C"
{
	static int compareCStr(const std::string  *s1, const std::string  *s2)
	{
		return strcmp(*s1, *s2);
	}
}

#endif //} qQT 


#ifdef qGTK  //{ GTK - Part	
static std::string  gtksel;
static bool gtkModalLoop;

static gint gtkcancel(GtkWidget *widget, gpointer data)
{
	gtkModalLoop = false;
	return TRUE;
}

static void file_ok_sel (GtkWidget *w, gpointer data)
{
	gtkModalLoop = false;
	gtksel = gtk_file_selection_get_filename(GTK_FILE_SELECTION (data));
}

static void file_cancel_sel (GtkWidget *w, gpointer data)
{
	gtkModalLoop = false;
}

static void fileop_c_dir_sel (GtkWidget *w, gpointer data)
{
	GtkWidget *dialog     = GTK_FILE_SELECTION (GTK_WIDGET (data))->fileop_dialog;
	
	gtk_grab_add (dialog);
	
	// remove grab from fileselection dialog
	gtk_grab_remove (GTK_WIDGET (data));
	fprintf(stderr, "fileop_c_dir open\n");
}
#endif //}  qGTK

/*!
	Prompts the user for directory selection
	\param prompt Dialog caption
	\param dir Pass initial directory, return selected directory if selection done
	\param pParentWnd Parent for the dialog, important if you call it from within the dialog
	\return true if selection was done, false otherwise
*/
bool BrowserGetDirectory(const char* prompt, std::string& dir, CWnd* pParentWnd /*= NULL*/)
{
	bool result = false;

#ifdef WIN32  //{ Win32 - Part
	CCJShell m_shell;
	m_shell.m_strTitle = prompt;
	m_shell.m_strSelDir = dir.c_str();
	
	if( m_shell.BrowseForFolder(pParentWnd != 0L ? pParentWnd : AfxGetMainWnd()) )
	{
		dir = m_shell.m_strPath;
		result = true;
	}
#endif //{ WIN32 

#ifdef TARGET_OS_MAC   //{ Mac - Part	
		UNavServicesDialogs::LFileChooser chooser;
		
		FSSpec folderReply;
		SInt32 outFolderDirID;
		if( chooser.AskChooseFolder(folderReply, outFolderDirID) )
		{
			GUSIFileSpec dirSpec(folderReply);
			dir = dirSpec.FullPath();
			result = true;
		}
#endif  //{ TARGET_OS_MAC 
	
#ifdef qQT  //{ QT - Part	
	QFileDialog::Mode mode = QFileDialog::Directory;
	QString start(QDir::currentDirPath());
	QString filter("*");
	QString caption(prompt);

	QFileDialog fd(start, filter, 0, 0L, TRUE);
	fd.setMode(mode);
	fd.setCaption(caption);
	fd.setSelection(start);
	if( fd.exec() == QDialog::Accepted )
	{
		dir = fd.dirPath();
		result = true;
	}
#endif //} qQT 

#ifdef qGTK  //{ GTK - Part	
extern GtkWidget *MainWindow;
	/* Create a new file selection widget */
	GtkWidget* filew = gtk_file_selection_new (prompt);
	GtkWidget *pp = pParentWnd ? (GtkWidget *)pParentWnd : MainWindow; \
	if( filew && pp) gtk_window_set_transient_for (GTK_WINDOW (filew), GTK_WINDOW (pp)); \
	
	gtk_signal_connect(GTK_OBJECT(filew), "delete_event",
					   GTK_SIGNAL_FUNC(gtkcancel), 0L);

	/* Connect the ok_button to file_ok_sel function */
	gtk_signal_connect(GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
					  "clicked", (GtkSignalFunc) file_ok_sel, filew);
	gtk_signal_connect(GTK_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
					  "clicked", (GtkSignalFunc) file_cancel_sel, filew);
	//{ WORKAROUND  Bug #440874
	// call 'fileop_c_dir_sel' when file options selected
	gtk_signal_connect(GTK_OBJECT (GTK_FILE_SELECTION (filew)->fileop_c_dir),
					  "clicked", (GtkSignalFunc) fileop_c_dir_sel, filew);
	//}  WORKAROUND  Bug #440874

	gtkModalLoop = true;
	gtksel = "";
	gtk_widget_show(filew);
	//{ WORKAROUND Bug 440874: blocks keyboard in create dialog if uncommented
	gtk_grab_add(filew);
	//} WORKAROUND Bug 440874
	while( gtkModalLoop )
	{
		gtk_main_iteration_do(TRUE);
	}

	//{ WORKAROUND Bug 440874: blocks keyboard in create dialog if uncommented
	gtk_grab_remove (filew);
	//} BWORKAROUND Bug 440874
	
	gtk_widget_destroy(filew);

	if( !gtksel.empty() )
	{
		static std::string  fname;

		fname = gtksel;
		if( !fname.empty() )
		{
			struct stat sb;
			if( stat(fname.c_str(), &sb) != -1 && S_ISREG(sb.st_mode) )
			{
				std::string  uppath, file;
				SplitPath(fname.c_str(), uppath, file);
				fname = uppath;
			}
			
			TrimRight(fname, kPathDelimiter);
			
			dir = fname;
			result = true;
		}
	}
#endif //}  qGTK

	return result;
}

/*!
	Get the set of files
	\param prompt Caption for the dialog
	\param mf Pass the initial file(single file only!) and directory, return the set of files, sorted by directory (class MultiFiles)
	\param single true to allow the selection of a single file only, false for multi-selection
	\param selectionType Selection type
	\param mustExist true to ensure that file exists, false to allow any entry
	\return true on success, false otherwise
*/	
bool BrowserGetMultiFiles(const char* prompt, MultiFiles& mf, bool single, 
						  const kFilesSelectionType selectionType /*= kSelectAny*/,
						  bool mustExist /*= true*/)
{
	bool result = false;

	// Extract the directory and file to select
	string selDir;	// Directory to be initially selected (browse starting from that directory)
	string selFile;	// File to be initially selected

	// Check the passed file/directory
	if( mf.NumDirs() > 0 )
	{
		MultiFiles::const_iterator mfi = mf.begin();
		if( mfi != mf.end() )
		{
			selDir = mfi->GetDir();
			
			// Single file passed - we can select it
			if( mf.TotalNumFiles() == 1 )
			{
				string path;
				string fileName;
				string currRev;

				if( mfi->get(0, path, fileName, currRev) )
				{
					selFile = fileName;
				}
			}
		}
	}

	// Clear the selection object
	mf.reset();

#ifdef WIN32  //{ Win32 - Part
	const char* lpszFilter = FILTER_ALL FILTER_END;
	switch( selectionType )
	{
	default:
	case kSelectAny:
		lpszFilter = FILTER_ALL FILTER_END;
		break;
	case kSelectExecutable:
		lpszFilter = FILTER_EXECUTABLE FILTER_SEPARATOR FILTER_ALL FILTER_END;
		break;
	case kSelectDll:
		lpszFilter = FILTER_DLL FILTER_SEPARATOR FILTER_ALL FILTER_END;
		break;
	}

	if( selDir.empty() )
	{
		// No selection specified, use current directory
		TCHAR curDir[_MAX_PATH] = { 0 };
		GetCurrentDirectory(_MAX_PATH-1, curDir);
		selDir = curDir;
	}

	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	
	if( !single )
	{
		dwFlags |= OFN_ALLOWMULTISELECT;
	}

	if( mustExist )
	{
		dwFlags |= OFN_FILEMUSTEXIST;
	}

	CMfFileDialog mfFileDialog(mf, NULL, selFile.c_str(), dwFlags, lpszFilter);

	mfFileDialog.m_ofn.lpstrTitle = prompt;
	mfFileDialog.m_ofn.lpstrInitialDir = selDir.c_str();

	if( mfFileDialog.DoModal() == IDOK )
	{
		result = true;
	}
#endif //{ WIN32 

#ifdef TARGET_OS_MAC   //{ Mac - Part	
	Point apt = {-1, -1};
	Str255 pprompt;
	
	c2pstrcpy(pprompt, prompt);
	
	if(!SF_PromptForMFiles(apt, pprompt))
		return false;
	
	SF_PFMF_INFO info;
	UFSSpec dirSpec;
	
	SF_PFMF_BeginTraversal(&info);
	while( 1 )
	{
		if( !SF_PFMF_GetCurrentDir(&info, &dirSpec) )
			break;
		
		char aPath[255];
		GUSIFileSpec spec = dirSpec;
		char* specPath = spec.FullPath();
		if( spec.Error() == noErr )
		{
		  strcpy(aPath, specPath);
			mf.newdir(aPath);
			
			if( SF_PFMF_DoTraversal(&info, true, (SF_PFMF_HOOK)MacEnumFiles_hook, &mf) )
			{
				result = true;
			}
		}
		else
		{
			cvs_err("MacCvs: Unable to convert path name\n");
		}

	}
	SF_PFMF_EndTraversal(&info);
	
#endif  //{ TARGET_OS_MAC 

#ifdef qQT  //{ QT - Part	
	QStringList s = QFileDialog::getOpenFileNames();
	vector<std::string > allpaths;
	QStringList::Iterator f;
	for(f = s.begin(); f != s.end(); f++)
	{
		allpaths.push_back(std::string (*f));
	}
	
	if( allpaths.size() > 0 )
	{
		qsort(&allpaths[0], allpaths.size(), sizeof(std::string ), (int(*)(const void*, const void*))compareCStr);
		
		vector<std::string >::const_iterator i;
		std::string  uppath, file, oldpath;
		for(i = allpaths.begin(); i != allpaths.end(); ++i)
		{
			SplitPath(*i, uppath, file);
			if( strcmp(uppath, oldpath) != 0 )
				mf.newdir(uppath);

			oldpath = uppath;
			mf.newfile(file);
		}
		
		result = true;
	}
#endif //} qQT 

#ifdef qGTK  //{ GTK - Part	
	/* Create a new file selection widget */
	GtkWidget* filew = gtk_file_selection_new(prompt);
	
	gtk_signal_connect(GTK_OBJECT(filew), "delete_event",
		GTK_SIGNAL_FUNC(gtkcancel), 0L);
	
	/* Connect the ok_button to file_ok_sel function */
	gtk_signal_connect(GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		"clicked", (GtkSignalFunc) file_ok_sel, filew);
	gtk_signal_connect(GTK_OBJECT (GTK_FILE_SELECTION (filew)->cancel_button),
		"clicked", (GtkSignalFunc) file_cancel_sel, filew);
	
	gtkModalLoop = true;
	gtksel = "";
	gtk_widget_show(filew);
	
	//{ WORKAROUND Bug 440874: Directory creation is not necessary in this file selection box
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(filew));
	//{ WORKAROUND Bug 440874
	
	gtk_grab_add(filew);
	while( gtkModalLoop )
	{
		gtk_main_iteration_do(TRUE);
	}

	gtk_grab_remove (filew);
	gtk_widget_destroy(filew);
	
	if( !gtksel.empty() )
	{
		std::string  fname = gtksel;
		if( !fname.empty() )
		{
			std::string  uppath, file;
			SplitPath(fname.c_str(), uppath, file);
			mf.newdir(uppath.c_str());
			mf.newfile(file.c_str());
			
			result = true;
		}
	}
#endif //}  qGTK

	return result;
}

/*!
	Get a file name to save to
	\param prompt Caption for the dialog
	\param fullpath The initial selection path, return the selected path
	\param selectionType Selection type
	\return true on success, false otherwise
*/	
bool BrowserGetSaveasFile(const char* prompt, std::string& fullpath, const kFilesSelectionType selectionType)
{
	bool result = false;

	string selDir;	// Directory to be initially selected (browse starting from that directory)
	string selFile;	// File to be initially selected

	SplitPath(fullpath.c_str(), selDir, selFile);

#ifdef WIN32  //{ Win32 - Part
	const char* lpszFilter = FILTER_ALL FILTER_END;
	switch( selectionType )
	{
	default:
	case kSelectAny:
		lpszFilter = FILTER_ALL FILTER_END;
		break;
	case kSelectExecutable:
		lpszFilter = FILTER_EXECUTABLE FILTER_SEPARATOR FILTER_ALL FILTER_END;
		break;
	case kSelectDll:
		lpszFilter = FILTER_DLL FILTER_SEPARATOR FILTER_ALL FILTER_END;
		break;
	}

	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	
	CFileDialog fdFileDialog(false, NULL, selFile.c_str(), dwFlags, lpszFilter);
	fdFileDialog.m_ofn.lpstrInitialDir = selDir.c_str();
	fdFileDialog.m_ofn.lpstrTitle = prompt;

	if( fdFileDialog.DoModal() == IDOK )
	{
		fullpath = fdFileDialog.m_ofn.lpstrFile;
		result = true;
	}
#endif //{ WIN32

#ifdef TARGET_OS_MAC   //{ Mac - Part	
#endif  //{ TARGET_OS_MAC 

#ifdef qQT  //{ QT - Part	
#endif //} qQT 

#ifdef qGTK  //{ GTK - Part	
#endif //}  qGTK

	return result;

}

/*!
	Browse for file
	\param prompt Caption for the dialog
	\param selectionType Selection type
	\param fullpath The initial selection path, return the selected path
	\param mustExist true to ensure that file exists, false to allow any entry
	\return true if selection was made, false otherwise
*/
bool BrowserGetFile(const char* prompt, const kFilesSelectionType selectionType, std::string& fullpath, bool mustExist /*= true*/)
{
	bool res = false;

	MultiFiles mf;
	
	if( !fullpath.empty() )
	{
		if( fullpath.at(fullpath.length() - 1) == kPathDelimiter )
		{
			mf.newdir(fullpath.c_str());
		}
		else
		{
			string uppath, file;			
			if( SplitPath(fullpath.c_str(), uppath, file) )
			{
				mf.newdir(uppath.c_str());
				mf.newfile(file.c_str());
			}
		}
	}
	
	if( BrowserGetMultiFiles(prompt, mf, true, selectionType, mustExist) )
	{
		MultiFiles::const_iterator mfi = mf.begin();
		if( mfi != mf.end() )
		{
			string path;
			string fileName;
			string currRev;

			if( (*mfi).get(0, path, fileName, currRev) )
			{
				fullpath = path;
				NormalizeFolderPath(fullpath);
				fullpath += fileName;
				
				res = true;
			}
		}
	}

	return res;
}
