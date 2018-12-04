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

#include "UCvsMenubar.h"
#include "UCvsCommands.h"
#include "MacrosSetup.h"

UIMPLEMENT_DYNAMIC(UCvsMenubar, UMenu)

UBEGIN_MESSAGE_MAP(UCvsMenubar, UMenu)
	ON_UDESTROY(UCvsMenubar)
	ON_UCREATE(UCvsMenubar)
UEND_MESSAGE_MAP()

UCvsMenubar::UCvsMenubar() : UMenu(kUCvsMenubarID)
{
}

UCvsMenubar::~UCvsMenubar()
{
}

void UCvsMenubar::OnDestroy(void)
{
	delete this;
}

void UCvsMenubar::OnCreate(void)
{
}

void CreateMainMenubar(int widid)
{
	int cnt;

    UMENU menu = UMenuAddPulldown(widid, _i18n("&File"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("&Save\tCtrl+S"), menu, 0L, cmdSAVE);
	UMenuAddButton(widid, _i18n("Save &As..."), menu, 0L, cmdSAVE_AS);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Print...\tCtrl+P"), menu, 0L, cmdPRINT);
	UMenuAddButton(widid, _i18n("Print Pre&view"), menu, 0L, cmdPRINT_PREVIEW);
	UMenuAddButton(widid, _i18n("P&rint Setup..."), menu, 0L, cmdPRINT_SETUP);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Sen&d..."), menu, 0L, cmdSEND_MAIL);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("E&xit\tCtrl+Q"), menu, 0L, cmdEXIT);

	menu = UMenuAddPulldown(widid, _i18n("&Edit"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("&Undo\tCtrl+Z"), menu, 0L, cmdUNDO);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Cu&t\tCtrl+X"), menu, 0L, cmdCUT);
	UMenuAddButton(widid, _i18n("&Copy\tCtrl+C"), menu, 0L, cmdCOPY);
	UMenuAddButton(widid, _i18n("&Paste\tCtrl+V"), menu, 0L, cmdPASTE);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Select &All\tCtrl+A"), menu, 0L, cmdSELECT_ALL);
	UMenuAddButton(widid, _i18n("&Erase all"), menu, 0L, cmdCLEAR_ALL);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Find...\tCtrl+F"), menu, 0L, cmdFIND);
	UMenuAddButton(widid, _i18n("Find next\tF3"), menu, 0L, cmdREPEAT);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("F&ont..."), menu, 0L, cmdFONT);
	menu = UMenuAddPulldown(widid, _i18n("&Shell"), menu, cmdSHELL);
	UMenuAddRadio(widid, _i18n("&Tcl"), menu, 0L, cmdUSE_TCLSHELL);
	UMenuAddRadio(widid, _i18n("&Python"), menu, 0L, cmdUSE_PYSHELL);
		
	menu = UMenuAddPulldown(widid, _i18n("&View"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("&Toolbar"), menu, 0L, cmdTOOLBAR);
	UMenuAddButton(widid, _i18n("&Status Bar"), menu, 0L, cmdSTATUS_BAR);
	UMenuAddButton(widid, _i18n("&Browser Bar"), menu, 0L, cmdBROWSER_BAR);
	UMenuAddButton(widid, _i18n("&Files Bar"), menu, 0L, cmdFILES_BAR);
	UMenuAddButton(widid, _i18n("&Multi-Users Bar"), menu, 0L, cmdMULTI_BAR);
	UMenuAddButton(widid, _i18n("T&ags Bar"), menu, 0L, cmdTAGS_BAR);
	UMenuAddButton(widid, _i18n("&Console Bar"), menu, 0L, cmdCONSOLE_BAR);
	UMenuAddButton(widid, _i18n("&Workspace Bar"), menu, 0L, cmdWORKSPACE);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Change browser &location..."), menu, 0L, cmdOPEN);
	UMenuAddButton(widid, _i18n("S&ave settings for location..."), menu, 0L, cmdSAVESETTINGS);

	menu = UMenuAddPulldown(widid, _i18n("&Admin"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("Pr&eferences...\tCtrl+F1"), menu, 0L, cmdPREFERENCES);
	UMenuAddButton(widid, _i18n("&Command Line..."), menu, 0L, cmdCMDLINE);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Login..."), menu, 0L, cmdLOGIN);
	UMenuAddButton(widid, _i18n("Lo&gout"), menu, 0L, cmdLOGOUT);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Stop cvs"), menu, 0L, cmdSTOPCVS);

	menu = UMenuAddPulldown(widid, _i18n("Macros admin"), menu, cmdADMINMACROS);

	menu = UMenuAddPulldown(widid, _i18n("&Remote"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("&Create a new repository..."), menu, 0L, cmdINIT);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Import module..."), menu, 0L, cmdIMPORT);
	UMenuAddButton(widid, _i18n("Checkout module..."), menu, 0L, cmdCHECKOUT);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Create a tag by module..."), menu, 0L, cmdRTAGCREATE);
	UMenuAddButton(widid, _i18n("&Delete a tag by module..."), menu, 0L, cmdRTAGDELETE);
	UMenuAddButton(widid, _i18n("Create a &branch by module..."), menu, 0L, cmdRTAGBRANCH);

	menu = UMenuAddPulldown(widid, _i18n("&Modify"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("&Update selection...\tCtrl+U"), menu, 0L, cmdUPDATE);
	UMenuAddButton(widid, _i18n("Co&mmit selection...\tCtrl+M"), menu, 0L, cmdCOMMIT);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Add selection"), menu, 0L, cmdADD);
	UMenuAddButton(widid, _i18n("Add selection binar&y"), menu, 0L, cmdADDB);
	UMenuAddButton(widid, _i18n("&Remove selection"), menu, 0L, cmdRMV);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Erase &selection\tCtrl+Backspace"), menu, 0L, cmdTRASH);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Create a tag on selection..."), menu, 0L, cmdTAGNEW);
	UMenuAddButton(widid, _i18n("&Delete a tag on selection..."), menu, 0L, cmdTAGDELETE);
	UMenuAddButton(widid, _i18n("Create a &branch on selection..."), menu, 0L, cmdTAGBRANCH);

	menu = UMenuAddPulldown(widid, _i18n("&Query"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("E&xplore selection\tF2"), menu, 0L, cmdEXPLORE);
	UMenuAddButton(widid, _i18n("&Query Update\tF4"), menu, 0L, cmdQUERYUPDATE);
	UMenuAddButton(widid, _i18n("R&eload view\tF5"), menu, 0L, cmdRELOAD);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("View selection\tF7"), menu, 0L, cmdEDITSEL);
	UMenuAddButton(widid, _i18n("View default"), menu, 0L, cmdEDITSELDEF);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Diff selection\tCtrl+D"), menu, 0L, cmdDIFF);
	UMenuAddButton(widid, _i18n("&Log selection"), menu, 0L, cmdLOG);
	UMenuAddButton(widid, _i18n("Status sele&ction"), menu, 0L, cmdSTATUS);
	UMenuAddButton(widid, _i18n("Graph selection\tCtrl+G"), menu, 0L, cmdGRAPH);

	menu = UMenuAddPulldown(widid, _i18n("&Trace"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("Lock selection"), menu, 0L, cmdLOCKF);
	UMenuAddButton(widid, _i18n("Unlock selection"), menu, 0L, cmdUNLOCKF);
	UMenuAddButton(widid, _i18n("Edit selection"), menu, 0L, cmdEDIT);
	UMenuAddButton(widid, _i18n("Reserved edit selection"), menu, 0L, cmdRESEDIT);
	UMenuAddButton(widid, _i18n("Unedit selection"), menu, 0L, cmdUNEDIT);
	UMenuAddButton(widid, _i18n("Add a Watch on selection"), menu, 0L, cmdWATCHON);
	UMenuAddButton(widid, _i18n("Remove a watch from selection"), menu, 0L, cmdWATCHOFF);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Release selection"), menu, 0L, cmdRELEASE);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("Watchers of selection"), menu, 0L, cmdWATCHERS);
	UMenuAddButton(widid, _i18n("Editors of selection"), menu, 0L, cmdEDITORS);

	menu = UMenuAddPulldown(widid, _i18n("Macro&s"), UMENU_NULL, cmdMACROS);
	
	menu = UMenuAddPulldown(widid, _i18n("&Window"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("&Cascade"), menu, 0L, cmdCASCADE);
	UMenuAddButton(widid, _i18n("&Tile"), menu, 0L, cmdTILE_HORZ);
	UMenuAddButton(widid, _i18n("&Arrange Icons"), menu, 0L, cmdARRANGE);

	menu = UMenuAddPulldown(widid, _i18n("&Help"), UMENU_NULL);
	UMenuAddButton(widid, _i18n("&Help Topics"), menu, 0L, cmdFINDER);
	UMenuAddButton(widid, _i18n("Help on cvs-1.10"), menu, 0L, cmdCVS);
	UMenuAddButton(widid, _i18n("Help on cvs client"), menu, 0L, cmdCVSCLIENT);
	UMenuAddSeparator(widid, menu);
	UMenuAddButton(widid, _i18n("&Credits"), menu, 0L, cmdCREDITS);
	UMenuAddButton(widid, _i18n("C&opyrights"), menu, 0L, cmdCOPYRIGHTS);
	UMenuAddSeparator(widid, menu);
#ifdef WIN32
	UMenuAddButton(widid, _i18n("&About wincvs..."), menu, 0L, cmdABOUT);
#endif
#ifdef qUnix
	UMenuAddButton(widid, _i18n("&About gcvs..."), menu, 0L, cmdABOUT);
#endif
}
