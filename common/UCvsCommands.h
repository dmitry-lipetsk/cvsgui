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

#ifndef UCVSCOMMANDS_H
#define UCVSCOMMANDS_H

#include "umain.h"

// the global pseudo-widgets have fixed assigned IDs in order
// to be able to access them from anywhere.
enum
{
	kUCvsFrameID = 1,			/* the main frane */
	kUMainConsoleID,			/* the pseudo-console */
	kUCvsFoldersID,				/* the folder tree */
	kUCvsFilesID,				/* the files list */
	kUCvsToolbarID,				/* the toolbar */
	kUCvsBrowseToolbarID,		/* the browse toolbar */
	kUCvsFilterToolbarID,		/* the filter toolbar */
	kUCvsMenubarID,				/* the menubar */
	kUCvsStatusbarID
};

// commands shared over the pseudo-widgets : these commands are dispatched
// mainly by the toolbar and the menubar.
enum
{
	cmdSAVE = EV_COMMAND_START + 100,
	cmdSAVE_AS,
	cmdPRINT,
	cmdPRINT_PREVIEW,
	cmdPRINT_SETUP,
	cmdSEND_MAIL,
	cmdEXIT,
	cmdUNDO,
	cmdCUT,
	cmdCOPY,
	cmdPASTE,
	cmdSELECT_ALL,
	cmdCLEAR_ALL,
	cmdFIND,
	cmdREPEAT,
	cmdFONT,
	cmdSHELL,
	cmdUSE_TCLSHELL,
	cmdUSE_PYSHELL,
	cmdTOOLBAR,
	cmdSTATUS_BAR,
	cmdBROWSER_BAR,
	cmdFILES_BAR,
	cmdMULTI_BAR,
	cmdTAGS_BAR,
	cmdCONSOLE_BAR,
	cmdWORKSPACE,
	cmdOPEN,
	cmdPREFERENCES,
	cmdCMDLINE,
	cmdLOGIN,
	cmdLOGOUT,
	cmdSTOPCVS,
	cmdINIT,
	cmdIMPORT,
	cmdCHECKOUT,
	cmdRTAGCREATE,
	cmdRTAGDELETE,
	cmdRTAGBRANCH,
	cmdUPDATE,
	cmdCOMMIT,
	cmdADD,
	cmdADDB,
	cmdRMV,
	cmdTRASH,
	cmdTAGNEW,
	cmdTAGDELETE,
	cmdTAGBRANCH,
	cmdEXPLORE,
	cmdQUERYUPDATE,
	cmdRELOAD,
	cmdEDITSEL,
	cmdEDITSELDEF,
	cmdDIFF,
	cmdLOG,
	cmdSTATUS,
	cmdGRAPH,
	cmdLOCKF,
	cmdUNLOCKF,
	cmdEDIT,
	cmdRESEDIT,
	cmdUNEDIT,
	cmdWATCHON,
	cmdWATCHOFF,
	cmdRELEASE,
	cmdWATCHERS,
	cmdEDITORS,
	cmdCASCADE,
	cmdTILE_HORZ,
	cmdARRANGE,
	cmdFINDER,
	cmdCVS,
	cmdCVSCLIENT,
	cmdABOUT,
	cmdIGNORE,
	cmdUPFOLDER,
	cmdSAVESETTINGS,
	cmdFLAT,
	cmdVIEW_ADDED,
	cmdVIEW_REMOVED,
	cmdVIEW_CHANGED,
	cmdHIDE_UNKNOWN,
	cmdVIEW_UNKNOWN,
	cmdVIEW_MODIFIED,
	cmdVIEW_MISSING,
	cmdVIEW_CONFLICT,
	cmdVIEW_FILEMASK,
	
	cmdMACROS,

#ifndef USE_PYTHON
	cmdSELMACRO, // contiguous BEGIN
	cmdSELMACROEND = cmdSELMACRO + 30,
#endif

	cmdADMINMACROS,
	cmdMACROADMIN, // contiguous BEGIN
	cmdMACROADMINEND = cmdMACROADMIN + 20,
	
	cmdCREDITS,
	cmdCOPYRIGHTS, 
	
#ifdef USE_PYTHON
	cmdPyMACROS = 10000,  // contiguous BEGIN
	cmdPyMACROSEND = cmdPyMACROS + 100 
#endif
};

#endif
