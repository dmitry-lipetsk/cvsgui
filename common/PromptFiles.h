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
 * PromptFiles.h --- functions to pick set of files and folder
 */

#ifndef PROMPTFILES_H
#define PROMPTFILES_H

class MultiFiles;
class CWnd;

/// Get MultiFiles dialog type enum
typedef enum
{
	kSelectAny,			/*!< Any file can be selected */
	kSelectExecutable,	/*!< Executable file can be selected */
	kSelectDll			/*!< DLL file can be selected */
} kFilesSelectionType;

bool BrowserGetDirectory(const char* prompt, std::string& dir, CWnd* pParentWnd = NULL);
bool BrowserGetMultiFiles(const char* prompt, MultiFiles& mf, bool single, const kFilesSelectionType selectionType = kSelectAny, bool blnMustExist = true);
bool BrowserGetSaveasFile(const char* prompt, std::string& fullpath, const kFilesSelectionType selectionType);
bool BrowserGetFile(const char* prompt, const kFilesSelectionType selectionType, std::string& fullpath, bool mustExist = true);

#endif /* PROMPTFILES_H */
