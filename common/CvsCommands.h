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
 * CvsCommands.h --- set of CVS commands
 */

#ifndef CVSCOMMANDS_H
#define CVSCOMMANDS_H

#include "LaunchHandlers.h"

/// Edit command type enum
typedef enum
{
	kEditNormal,	/*!< cvs edit */
	kEditReserved,	/*!< cvs edit -c */
	kEditForce		/*!< cvs edit -f */
} kEditCmdType;

/// Tag command type enum
typedef enum
{
	kTagCreate,			/*!< Create tag */
	kTagDelete,			/*!< Delete tag */
	kTagBranch,			/*!< Create branch tag */
	kTagCreateGraph,	/*!< Create tag on graph selection */
	kTagDeleteGraph,	/*!< Delete tag on graph selection */
	kTagBranchGraph		/*!< Create branch tag on graph selection */
} kTagCmdType;

/// Add command type enum
typedef enum
{
	kAddNormal,	/*!< cvs add */
	kAddBinary,	/*!< cvs add -kb */
	kAddUnicode	/*!< cvs add -ku */
} kAddCmdType;

/// Diff command type enum
typedef enum
{
	kDiffFile,		/*!< Diff file selection */
	kDiffFolder,	/*!< Diff folder selection */
	kDiffGraph		/*!< Diff graph selection */
} kDiffCmdType;

/// Annotate command type enum
typedef enum
{
	kAnnotateFile,		/*!< Annotate file selection */
	kAnnotateGraph		/*!< Annotate graph selection */
} kAnnotateCmdType;

class KiSelectionHandler;

// Command functions that don't carry the selection and don't need the selection handler
void CvsCmdInit();
void CvsCmdLogin(void);
void CvsCmdLogout(void);

// Command functions that may need the selection and need a selection handler
void CvsCmdRtag(KiSelectionHandler& handler, const kTagCmdType tagType);
void CvsCmdCommandLine(KiSelectionHandler& handler);

// Command functions that need both the selection and the selection handler
void CvsCmdCheckoutModule(KiSelectionHandler& handler);
void CvsCmdExportModule(KiSelectionHandler& handler);
void CvsCmdImportModule(KiSelectionHandler& handler);
void CvsCmdCancelChanges(KiSelectionHandler& handler);
void CvsCmdUpdate(KiSelectionHandler& handler, bool queryOnly = false);
void CvsCmdCommit(KiSelectionHandler& handler);
void CvsCmdDiff(KiSelectionHandler& handler, kDiffCmdType diffType);
void CvsCmdAdd(KiSelectionHandler& handler, kAddCmdType addType = kAddNormal);
void CvsCmdRemove(KiSelectionHandler& handler);
void CvsCmdLog(KiSelectionHandler& handler, bool outGraph = false, void* defWnd = 0L);
void CvsCmdStatus(KiSelectionHandler& handler);
void CvsCmdLock(KiSelectionHandler& handler);
void CvsCmdUnlock(KiSelectionHandler& handler);
void CvsCmdTag(KiSelectionHandler& handler, const kTagCmdType tagType);
void CvsCmdEdit(KiSelectionHandler& handler, kEditCmdType editType = kEditNormal);
void CvsCmdUnedit(KiSelectionHandler& handler);
void CvsCmdWatchOn(KiSelectionHandler& handler);
void CvsCmdWatchOff(KiSelectionHandler& handler);
void CvsCmdRelease(KiSelectionHandler& handler);
void CvsCmdWatchers(KiSelectionHandler& handler);
void CvsCmdEditors(KiSelectionHandler& handler);
void CvsCmdAnnotate(KiSelectionHandler& handler, kAnnotateCmdType annotateType = kAnnotateFile);
void CvsCmdMoveToTrash(KiSelectionHandler& handler);
void CvsCmdFileAction(KiSelectionHandler& handler);
void CvsCmdLaunch(KiSelectionHandler& handler, const kLaunchType launchType);

// Helper functions
const char* GetEditTypeName(const kEditCmdType editType);
const char* GetTagTypeName(const kTagCmdType tagType);
const char* GetRtagTypeName(const kTagCmdType tagType);

#endif /* CVSCOMMANDS_H */
