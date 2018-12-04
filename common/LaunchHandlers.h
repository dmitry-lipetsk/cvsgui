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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- January 2004
 */

/*
 * LaunchHandlers.cpp : Launch helper functions
 */

#if !defined(AFX_LAUNCHHANDLERS_H__26387DB1_9D2A_4909_A7A7_5ACAFA46A57B__INCLUDED_)
#define AFX_LAUNCHHANDLERS_H__26387DB1_9D2A_4909_A7A7_5ACAFA46A57B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/// Launch type enum
typedef enum{
	kLaunchEdit,			/*!< Launch in associated editor */
	kLaunchDefaultEdit,		/*!< Launch in default editor */
	kLaunchOpen,			/*!< Launch in associated handler application */
	kLaunchOpenAs,			/*!< Launch handler application selection dialog  */
	kLaunchDefault,			/*!< Launch default associated shell action */
	kLaunchConflictEdit,	/*!< Launch in conflict editor */
	kLaunchAnnotateViewer,	/*!< Launch annotate viewer */
	kLaunchEnd				/*!< End marker */
}kLaunchType;

const char* GetLaunchTypeName(const kLaunchType launchType, const bool shortName = true);

bool LaunchDiff(const char* file1, const char* file2, 
				const char* diffTool = NULL, bool unifiedDiff = false, bool noWhiteSpace = false, bool noBlankLines = false, bool ignoreCase = false);
bool LaunchTool(const char* filePath, const char* tool, void* par = NULL);

bool LaunchHandler(const kLaunchType launchType, const char* filePath);

bool LaunchViewer(const char* filePath, bool useOpenAs = false);
bool LaunchEditor(const char* filePath, bool useDefault = false);
bool LaunchDefaultEditor(const char* filePath, void* par = NULL);
bool LaunchConflictEditor(const char* filePath, void* par = NULL);
bool LaunchAnnotateViewer(const char* filePath, void* par = NULL);

void LaunchExplore(const char* path);
void LaunchCommandPrompt(const char* path);
void LaunchNewInst(const char* path);
void LaunchProperties(const char* path);

#endif // !defined(AFX_LAUNCHHANDLERS_H__26387DB1_9D2A_4909_A7A7_5ACAFA46A57B__INCLUDED_)
