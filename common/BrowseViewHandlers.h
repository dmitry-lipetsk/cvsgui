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

#ifndef _BROWSEVIEWHANDLERS_H_
#define _BROWSEVIEWHANDLERS_H_

#include "CvsCommands.h"
#include "cvsgui_i18n.h"

/// Command selection type enum
typedef enum
{
	kSelNone,	/*!< Not specified */
	kSelFiles,	/*!< Selection contains files */
	kSelDir		/*!< Selection contains directory */
} kSelType;

// Forward declaration
class MultiFiles;
class CCvsConsole;

/*!
	\class KiSelectionHandler
	\brief Interface describing a handler to process a selection-related command

   KiSelectionHandler is an abstract class designed to aid processing of a selection-related commands.
   Create command handlers by deriving from KiSelectionHandler and overriding virtual methods 
   KiSelectionHandler::OnFolder and KiSelectionHandler::OnFiles.
   
   The default value of KiSelectionHandler::m_needSelection can be changed if the command does not require selection
   to work properly (e.g checkout, import or rtag commands). However it's important to supply the selection whenever
   available. 

   The goal of passing the selection is to enable support for additional data retrieval and present the selection 
   information for the user as needed. For example to show the files selected for commit command and enable
   to perform review of changes using diff command. There are also commands that use the selection information to 
   retrieve the repository data such as modules or tags.
*/
class KiSelectionHandler
{
private:
	std::string m_strCommand;	/*!< Command name, used when browsing for selection */

	MultiFiles* m_pmf;	/*!< Command selection (files or directory) */
	kSelType m_selType;	/*!< Command selection type */

	bool m_autoDelete;	/*!< Indicates whether to delete the command selection allocated memory */

protected:
	bool m_needSelection;	/*!< Indicates whether the selection is needed for command */
	bool m_needViewReset;	/*!< Indicates whether reseting the view is needed for command */

public:
	KiSelectionHandler(const char* strCommand, bool needSelection = true, bool needResetView = false);
	virtual ~KiSelectionHandler();

	kSelType GetSelType() const;
	bool GetNeedSelection() const;
	bool GetNeedViewReset() const;
	
	virtual void OnFolder(const char* fullpath) = 0;
	virtual void OnFiles(MultiFiles* pmf) = 0;
	virtual MultiFiles* GetSelection();
	virtual bool NeedsNormalize();
	virtual CCvsConsole* GetConsole();
};

/// Handles moving files to trash
class KoMoveToTrashHandler : public KiSelectionHandler
{
public:
	KoMoveToTrashHandler()
		: KiSelectionHandler(_i18n("Move To Trash"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{ /* ignore */ }

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdMoveToTrash(*this);
	}
};

/// Handles figuring out if moving files to trash is enabled
class KoUpdateMoveToTrashHandler : public KiSelectionHandler
{
// Attributes
	bool m_isFolders;
	bool m_isFiles;

public:
	KoUpdateMoveToTrashHandler()
		: KiSelectionHandler(_i18n("Update Move To Trash")), m_isFolders(false), m_isFiles(false)
	{}

	virtual void OnFolder(const char* fullpath)
	{ 
		m_isFolders = true;
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		m_isFiles = true;
	}

	bool IsEnabled() const
	{
		return !m_isFolders && m_isFiles;
	}
};

/// Handles tag command
template<kTagCmdType tagType>
class TTagHandler : public KiSelectionHandler
{
	CCvsConsole* m_console; /*!< CVS console, may be NULL */

public:
	TTagHandler(CCvsConsole* console = NULL)
		: m_console(console), 
		KiSelectionHandler(GetTagTypeName(tagType))
	{}
	
	virtual CCvsConsole* GetConsole()
	{
		return m_console;
	}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdTag(*this, tagType);
	}
	
	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdTag(*this, tagType);
	}
};

typedef TTagHandler<kTagCreate> KoCreateTagHandler;
typedef TTagHandler<kTagDelete> KoDeleteTagHandler;
typedef TTagHandler<kTagBranch> KoBranchTagHandler;

typedef TTagHandler<kTagCreateGraph> KoGraphCreateTagHandler;
typedef TTagHandler<kTagDeleteGraph> KoGraphDeleteTagHandler;
typedef TTagHandler<kTagBranchGraph> KoGraphBranchTagHandler;

/// Handles edit command
template<kEditCmdType editType>
class TEditHandler : public KiSelectionHandler
{
public:
	TEditHandler()
		: KiSelectionHandler(GetEditTypeName(editType), true, true)
	{}
	
	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdEdit(*this, editType);
	}
	
	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdEdit(*this, editType);
	}
};

typedef TEditHandler<kEditNormal> KoEditHandler;
typedef TEditHandler<kEditReserved> KoReservedEditHandler;
typedef TEditHandler<kEditForce> KoForceEditHandler;

/// Handles edit stop request on the selected items
class KoUneditHandler : public KiSelectionHandler
{
public:
	KoUneditHandler()
		: KiSelectionHandler(_i18n("Unedit"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdUnedit(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdUnedit(*this);
	}
};

/// Handles releasing on the selected items
class KoReleaseHandler : public KiSelectionHandler
{
public:
	KoReleaseHandler()
		: KiSelectionHandler(_i18n("Release"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdRelease(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{	}
};

/// Handles watchers on the selected items
class KoWatchersHandler : public KiSelectionHandler
{
public:
	KoWatchersHandler()
		: KiSelectionHandler(_i18n("Watchers"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdWatchers(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdWatchers(*this);
	}
};

/// Handles editors on the selected items
class KoEditorsHandler : public KiSelectionHandler
{
public:
	KoEditorsHandler()
		: KiSelectionHandler(_i18n("Editors"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdEditors(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdEditors(*this);
	}
};

/// Handles watch on on the selected items
class KoWatchOnHandler : public KiSelectionHandler
{
public:
	KoWatchOnHandler()
		: KiSelectionHandler(_i18n("Watch On"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdWatchOn(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdWatchOn(*this);
	}
};

/// Handles watch off on the selected items
class KoWatchOffHandler : public KiSelectionHandler
{
public:
	KoWatchOffHandler()
		: KiSelectionHandler(_i18n("Watch Off"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdWatchOff(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdWatchOff(*this);
	}
};

/// Handles diffing on the selected items
class KoDiffHandler : public KiSelectionHandler
{
public:
	KoDiffHandler()
		: KiSelectionHandler(_i18n("Diff"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdDiff(*this, kDiffFolder);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdDiff(*this, kDiffFile);
	}
};

/// Handles diffing on the selected items
class KoGraphDiffHandler : public KiSelectionHandler
{
public:
	KoGraphDiffHandler()
		: KiSelectionHandler(_i18n("Diff"))
	{}

	virtual void OnFolder(const char* fullpath)
	{	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdDiff(*this, kDiffGraph);
	}
};

/// Handles log queries on the selected items
class KoLogHandler : public KiSelectionHandler
{
public:
	KoLogHandler()
		: KiSelectionHandler(_i18n("Log"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdLog(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdLog(*this);
	}
};

/// Handles graph log queries on the selected items
class KoGraphHandler : public KiSelectionHandler
{
public:
	KoGraphHandler()
		: KiSelectionHandler(_i18n("Graph"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		/*we avoid processing directory*/
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdLog(*this, true);
	}
	
	virtual bool NeedsNormalize()
	{
		return true;
	}
};

/// Handles status queries on the selected items
class KoStatusHandler : public KiSelectionHandler
{
public:
	KoStatusHandler()
		: KiSelectionHandler(_i18n("Status"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdStatus(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdStatus(*this);
	}
};

/// Handles file remove requests
class KoFileRemoveHandler : public KiSelectionHandler
{
public:
	KoFileRemoveHandler()
		: KiSelectionHandler(_i18n("Remove"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{ 
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdRemove(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdRemove(*this);
	}
};

/// Handles locking on the selected items
class KoLockHandler : public KiSelectionHandler
{
public:
	KoLockHandler()
		: KiSelectionHandler(_i18n("Lock"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdLock(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdLock(*this);
	}
};

/// Handles unlocking on the selected items
class KoUnlockHandler : public KiSelectionHandler
{
public:
	KoUnlockHandler()
		: KiSelectionHandler(_i18n("Unlock"))
	{}
	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdUnlock(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdUnlock(*this);
	}
};

/// Handles addition of the selected items
class KoAddHandler : public KiSelectionHandler
{
public:
	KoAddHandler()
		: KiSelectionHandler(_i18n("Add"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdAdd(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdAdd(*this);
	}
	
	virtual bool NeedsNormalize()
	{
		return true;
	}
};

/// Handles binary addition of the selected items
class KoAddBinaryHandler : public KiSelectionHandler
{
public:
	KoAddBinaryHandler() 
		: KiSelectionHandler(_i18n("Add Binary"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdAdd(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdAdd(*this, kAddBinary);
	}

	virtual bool NeedsNormalize()
	{
		return true;
	}
};

/// Handles unicode addition of the selected items
class KoAddUnicodeHandler : public KiSelectionHandler
{
public:
	KoAddUnicodeHandler()
		: KiSelectionHandler(_i18n("Add Unicode"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdAdd(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdAdd(*this, kAddUnicode);
	}

	virtual bool NeedsNormalize()
	{
		return true;
	}
};

/// Handles commits of the selected items
class KoCommitHandler : public KiSelectionHandler
{
public:
	KoCommitHandler()
		: KiSelectionHandler(_i18n("Commit"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdCommit(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdCommit(*this);
	}
};

/// Handles updates on the selected items
class KoUpdateHandler : public KiSelectionHandler
{
public:
	KoUpdateHandler()
		: KiSelectionHandler(_i18n("Update"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdUpdate(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdUpdate(*this);
	}
};

/// Handles query updates on the selected items
class KoQueryUpdateHandler : public KiSelectionHandler
{
public:
	KoQueryUpdateHandler()
		: KiSelectionHandler(_i18n("Query Update"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdUpdate(*this, true);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdUpdate(*this, true);
	}
};

/// Handles annotate on the selected items
class KoAnnotateHandler : public KiSelectionHandler
{
public:
	KoAnnotateHandler()
		: KiSelectionHandler(_i18n("Annotate"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		/*we avoid processing directory*/
	}
	
	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdAnnotate(*this);
	}
};

/// Handles annotate on the selected graph items
class KoGraphAnnotateHandler : public KiSelectionHandler
{
public:
	KoGraphAnnotateHandler()
		: KiSelectionHandler(_i18n("Annotate"))
	{}

	virtual void OnFolder(const char* fullpath)
	{ }

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdAnnotate(*this, kAnnotateGraph);
	}
};


/// Handles cancel changes on the selected items
class KoCancelChangesHandler : public KiSelectionHandler
{
public:
	KoCancelChangesHandler()
		: KiSelectionHandler(_i18n("Cancel Changes"))
	{}

	virtual void OnFolder(const char* fullpath)
	{
		/*we avoid processing directory*/
	}
	
	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdCancelChanges(*this);
	}
};

/// Handles Command Line
class KoCommandLineHandler : public KiSelectionHandler
{
public:
	KoCommandLineHandler()
		: KiSelectionHandler(_i18n("Command Line"), false)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdCommandLine(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdCommandLine(*this);
	}
};

/// Handles Import
class KoImportHandler : public KiSelectionHandler
{
public:
	KoImportHandler()
		: KiSelectionHandler(_i18n("Import"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdImportModule(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdImportModule(*this);
	}
};

/// Handles Checkout
class KoCheckoutHandler : public KiSelectionHandler
{
public:
	KoCheckoutHandler()
		: KiSelectionHandler(_i18n("Checkout"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdCheckoutModule(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdCheckoutModule(*this);
	}
};

/// Handles Export
class KoExportHandler : public KiSelectionHandler
{
public:
	KoExportHandler()
		: KiSelectionHandler(_i18n("Export"), true, true)
	{}

	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdExportModule(*this);
	}

	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdExportModule(*this);
	}
};

/// Handles rtag command
template<kTagCmdType tagType>
class TRtagHandler : public KiSelectionHandler
{
public:
	TRtagHandler()
		: KiSelectionHandler(GetRtagTypeName(tagType))
	{}
	
	virtual void OnFolder(const char* fullpath)
	{
		KiSelectionHandler::OnFolder(fullpath);
		CvsCmdRtag(*this, tagType);
	}
	
	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdRtag(*this, tagType);
	}
};

typedef TRtagHandler<kTagCreate> KoRtagCreateHandler;
typedef TRtagHandler<kTagDelete> KoRtagDeleteHandler;
typedef TRtagHandler<kTagBranch> KoRtagBranchHandler;

/// Handles file action
class KoFileActionHandler : public KiSelectionHandler
{
public:
	KoFileActionHandler()
		: KiSelectionHandler(_i18n("File Action"), true)
	{}
	
	virtual void OnFolder(const char* fullpath)
	{ /* ignore */ }
	
	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdFileAction(*this);
	}
};

/// Handles shell action launch
template<kLaunchType lauchType>
class TLaunchHandler : public KiSelectionHandler
{
public:
	TLaunchHandler()
		: KiSelectionHandler(GetLaunchTypeName(lauchType), true)
	{}
	
	virtual void OnFolder(const char* fullpath)
	{ /* ignore */ }
	
	virtual void OnFiles(MultiFiles* pmf)
	{
		KiSelectionHandler::OnFiles(pmf);
		CvsCmdLaunch(*this, lauchType);
	}
};

typedef TLaunchHandler<kLaunchEdit> KoLaunchEditHandler;
typedef TLaunchHandler<kLaunchDefaultEdit> KoLaunchDefaultEditHandler;
typedef TLaunchHandler<kLaunchOpen> KoLaunchOpenHandler;
typedef TLaunchHandler<kLaunchOpenAs> KoLaunchOpenAsHandler;
typedef TLaunchHandler<kLaunchDefault> KoLaunchDefaultHandler;
typedef TLaunchHandler<kLaunchConflictEdit> KoLaunchConflictEditHandler;
typedef TLaunchHandler<kLaunchAnnotateViewer> KoLaunchAnnotateViewerHandler;

#endif
// OWSEVIEWHANDLERS_H_
