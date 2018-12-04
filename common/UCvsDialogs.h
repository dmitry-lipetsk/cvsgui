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

#ifndef UCVSDIALOGS_H
#define UCVSDIALOGS_H

#include "uwidget.h"

// the pixmaps creations
void *UCreate_pixmap(UWidget *parent, const char *fileIcon);

// the dialogs creation
extern void *UCreate_MainWindow (void *Parent = NULL);
extern void *UCreate_GraphWindow (void *Parent = NULL);
extern void *UCreate_Alert (void *Parent = NULL);
extern void *UCreate_gcvs_Settings (void *Parent = NULL);
extern void *UCreate_UpdateDlg (void *Parent = NULL);
extern void *UCreate_PasswdDlg (void *Parent = NULL);
extern void *UCreate_CommitDlg (void *Parent = NULL);
extern void *UCreate_DiffDlg (void *Parent = NULL);
extern void *UCreate_CheckoutDlg (void *Parent = NULL);
extern void *UCreate_LogDlg (void *Parent = NULL);
extern void *UCreate_ImportFilterDlg (void *Parent = NULL);
extern void *UCreate_ImpWarningDlg (void *Parent = NULL);
extern void *UCreate_ImpEntryDlg (void *Parent = NULL);
extern void *UCreate_ImportDlg (void *Parent = NULL);
extern void *UCreate_CmdlineDlg (void *Parent = NULL);
extern void *UCreate_TagModDlg (void *Parent = NULL);
extern void *UCreate_UntagModDlg (void *Parent = NULL);
extern void *UCreate_BranchModDlg (void *Parent = NULL);
extern void *UCreate_TagDlg (void *Parent = NULL);
extern void *UCreate_UntagDlg (void *Parent = NULL);
extern void *UCreate_BranchDlg (void *Parent = NULL);
extern void *UCreate_SettingsDlg (void *Parent = NULL);
extern void *UCreate_AboutDlg (void *Parent = NULL);
extern void *UCreate_SelTagBranchDlg (void *Parent = NULL);
extern void *UCreate_SelDetailsDlg (void *Parent = NULL);
extern void *UCreate_AskYesNoDlg (void *Parent = NULL);

#endif
