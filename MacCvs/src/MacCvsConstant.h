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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- September 2001
 */

/*
 * MacCvsConstant.h --- application constants
 */

#ifndef MACCVSCONSTANT_H
#define MACCVSCONSTANT_H

// menu commands
enum {
//	cmd_UpdateFolder		= 1000,
//	cmd_CommitFolder		= 1001,
	cmd_MacCvsPrefs			= 1002,
	cmd_CmdLine				= 1003,
	cmd_Checkout			= 1004,
	cmd_Import				= 1005,
	cmd_Login				= 1006,
//	cmd_UpdateFiles			= 1007,
//	cmd_CommitFiles			= 1008,
//	cmd_DiffFiles			= 1009,
//	cmd_LogFiles			= 1010,
//	cmd_AddFiles			= 1011,
//	cmd_AddBinaryFiles		= 1012,
//	cmd_RemoveFiles			= 1013,
//	cmd_StatusFiles			= 1014,
	cmd_CancelChanges		= 1015,
//	cmd_AddFolder			= 1016,
//	cmd_DiffFolder			= 1017,
//	cmd_LogFolder			= 1018,
//	cmd_StatusFolder		= 1021,
//	cmd_LockFiles			= 1019,
//	cmd_UnlockFiles			= 1020,
//	cmd_EditFiles			= 1022,
//	cmd_UneditFiles			= 1023,
//	cmd_WatchOnFiles		= 1024,
//	cmd_WatchOffFiles		= 1025,
//	cmd_WatchersFiles		= 1026,
//	cmd_EditorsFiles		= 1027,
//	cmd_LockFolder			= 1028,
//	cmd_UnlockFolder		= 1029,
//	cmd_EditFolder			= 1030,
//	cmd_UneditFolder		= 1031,
//	cmd_WatchOnFolder		= 1032,
//	cmd_WatchOffFolder		= 1033,
//	cmd_WatchersFolder		= 1034,
//	cmd_EditorsFolder		= 1035,
//	cmd_ReleaseFolder		= 1036,
	cmd_RtagCreate			= 1037,
	cmd_RtagDelete			= 1038,
	cmd_RtagBranch			= 1039,
//	cmd_TagCreateFolder		= 1040,
//	cmd_TagDeleteFolder		= 1041,
//	cmd_TagBranchFolder		= 1042,
//	cmd_TagCreateFiles		= 1043,
//	cmd_TagDeleteFiles		= 1044,
//	cmd_TagBranchFiles		= 1045,
	cmd_Logout				= 1046,
	cmd_Init				= 1047,
	cmd_Stop				= 1048,
	
	cmd_NewBrowser			= 1200,
	cmd_LocateBrowser		= 1201,
	cmd_SaveSettings		= 1202,
	cmd_RememberBrowserLocation = 1203,
	cmd_DockToolbar = 1204,
	cmd_ShowToolbar = 1205,
	
	cmd_Credits				= 1250,
	cmd_Copyright			= 1251,

	cmd_FirstWindow			= 3000,
	cmd_LastWindow			= 3100,
	
	cmd_FirstBrowser		= 3100,
	cmd_LastBrowser			= 3200,
	
	cmd_FirstMacro			= 3200,
	cmd_LastMacro			= 3600,
	
	cmd_FirstPathComponent = 3600, // for File Path submenu in contextual menu
	cmd_LastPathComponent = 3700,
	
	cmd_BrowserStart		= 2000,
	cmd_SelUpdate			= cmd_BrowserStart,
	cmd_SelCommit			= 2001,
	cmd_SelAdd				= 2002,
	cmd_SelAddBin			= 2003,
	cmd_SelRemove			= 2004,
	cmd_SelDiff				= 2005,
	cmd_SelLog				= 2006,
	cmd_SelStatus			= 2007,
	cmd_SelDelete			= 2008,
	cmd_SelLock				= 2009,
	cmd_SelUnLock			= 2010,
	cmd_SelWatchOn			= 2011,
	cmd_SelWatchOff			= 2012,
	cmd_SelEdit				= 2013,
	cmd_SelUnEdit			= 2014,
	cmd_SelRelease			= 2015,
	cmd_SelReload			= 2016,
	cmd_SelWatchers			= 2017,
	cmd_SelEditors			= 2018,
	cmd_SelLogTree			= 2019,
	cmd_SelTag				= 2020,
	cmd_SelTagCreate		= 2021,
	cmd_SelTagDelete		= 2022,
	cmd_SelTagBranch		= 2023,
	//cmd_SelIgnore			= 2024,
	cmd_SelQuery			= 2025,
	cmd_RevealInFinder = 2026,
	cmd_FilePath = 2027, // pseudo command - actually has a submenu attached
	cmd_SelAnnotate = 2028,
	cmd_GraphRemoveSticky = 2029, // pseudonym for regular update that'll allow to have a separate update menu item
	cmd_BrowserEnd,

	cmd_FilterStart			= 2100,
	cmd_FilterCommitable	= cmd_FilterStart,
	cmd_FilterChanged		= 2101,
	cmd_FilterAdded			= 2102,
	cmd_FilterRemoved		= 2103,
	cmd_FilterConflict		= 2104,
	cmd_FilterShowUnknown	= 2105,
	cmd_FilterHideUnknown	= 2106,
	cmd_FilterMissing		= 2107,
	cmd_FilterIgnored		= 2108,
	cmd_FilterPopup			= 2109,
	cmd_FilterFlat			= 2110,
	cmd_FilterEdit			= 2111,
	cmd_FilterMaskType  = 2112,
	cmd_FilterEnd
};

// messages
enum {
	msg_UpdateBrowserMenu	= 1201
};


// dialogs items
enum {
	item_OK					= 'ok  ',
	item_Cancel				= 'cncl',
	
	item_MultiViewPrefs		= 'MPVW',
	item_GroupViewPrefs		= 'PGBX',
	
	item_CheckoutRO			= 'CKRD',
	item_PruneDir			= 'PRUN',
	item_TCPCompress		= 'Z9CO',
	item_ZLevel				= 'Z9LV',
	item_QuietMsg			= 'QUIE',
	item_AddControl			= 'BCTL',
	item_DirtySupport		= 'DIRF',
	item_AutoLogout			= 'GPLO',
	item_AutoLogoutTime		= 'GPLT',
	item_UnixLF				= 'UNLF',

	item_ICOnText			= 'ADCT',
	item_ISOConvert			= 'ISOS',
	item_UseExtDiff			= 'EDIF',
	item_EditExtDiff		= 'EEDI',
	item_EditExtDiffView		= 'EEDV',
	item_BrowseExtDiff		= 'BEDI',
  item_UnifiedDiff      = 'UDIF',
  item_IgnoreWhiteSpaceInDiff      = ' DIF',
  item_IgnoreEmptyLinesInDiff      = '/DIF',
  item_IgnoreCaseInDiff      = '^DIF',
  
	item_LogMsg				= 'LOGM',
	item_LogMsgEncoding = 'MEnc',
	item_NoRecurs			= 'NREC',
	item_PreviewDiffPopup	= 'PFDI',
	item_PreviewDiffBtn		= 'BFDI',
	
	item_ForceCommit		= 'CMFC',
	item_ForceRecurse		= 'CMFR',
	item_NoModuleProgram	= 'CMNM',
	
	item_ModuleName			= 'MODN', /* item_ModuleName toggles the OK button */
	item_PopModName			= 'POPM',
	item_Vendortag			= 'VTAG',
	item_Reltag				= 'RTAG',
	item_ImportCvsIgnore	= 'IMCI',
	item_ImportRunFilter = 'IMRF',
	item_ImportNoVendorBranch = 'IMNV',
	item_ImportOverwriteTags = 'IMOT',
	item_ImportUseFileModDate = 'IMFM',
	item_ImportCreateCVSDirectories = 'IMCD',
	item_ImportUseVendorBranchID = 'IMVB',
	item_ImportVendorBranchID = 'IMvb',
	
	item_GetPasswd			= 'EPAS',
	
	item_CheckGroupPath		= 'CGPA',
	item_EditPath			= 'PATH',
	item_ShowPath     = 'Path',
	
	item_FontNames			= 'FNTP', /* Changes item_FontQuote */
	item_FontSizes			= 'FNTS', /* Changes item_FontQuote */
	item_FontQuote			= 'FNTT',
	
	item_ImpEditWarn		= 'EWRN',
	
	item_ImpEntryUnch		= 'IKUN',
	item_ImpEntryFBin		= 'IKFB',
	item_ImpEntryFText		= 'IKFT',
	item_ImpEntryFIgn		= 'IKFI',
	item_ImpPushEdit		= 'EENT',
	item_ImpFilterList		= 'TABL',

	item_NonEmptyTextEdit	= 'NETE',

	item_ToStout			= 'COSO',
	item_CreateMissDir		= 'CRMI',
	item_GetCleanCopy		= 'UPCC',
	item_ResetSticky		= 'RESS',
	item_MostRecentRev		= 'MREV',
	item_CheckForceRoot		= 'COFR',
	item_CheckExport		= 'COEX',
	item_CheckDontShortenModulePaths = 'NSHR',
	item_CheckDate			= 'CDAT',
	item_EditDate			= 'DATE',
	item_CheckRevTag		= 'CREV',
	item_EditRevTag			= 'REVT',
	item_CheckRevTag1		= 'CRE1',
	item_EditRevTag1		= 'REV1',
	item_CheckRevTag2		= 'CRE2',
	item_EditRevTag2		= 'REV2',
	item_MergeNone			= 'NONE', ///\todo obsolete
	item_FillerDate			= 'POPD',
	item_FillerTag			= 'POPR',
	item_FillerRev1			= 'POP1',
	item_FillerRev2			= 'POP2',
	item_DiffLabelOptions    = 'DifO',
	item_Keyword			  = 'KSUB',
	item_CheckKeyword		= 'ksub',
	item_CaseSensitive  = 'SCCN',
	item_UseLastCheckinTime = 'UULC',
	
	item_Produce3WayConflicts = '3WAY',
	item_MergeFromBranchPoint = 'MBPT',
	item_MergeType           = 'MGTP',
	item_MergeOptions        = 'MGOP',
	item_EditDate1           = 'EDT1',
	item_EditDate2           = 'EDT2',
	item_MergeRev1Options    = 'MGO1',
	item_MergeRev2Options    = 'MGO2',
	
	item_DiffExternal		= 'DIEX',

	item_LogNoTags			= 'LGNT',
	item_LogDefBranch		= 'LGDB',
	item_LogNoUserLogs		= 'LGNL',
	item_LogOnlyRcs			= 'LGOR',
	item_LogCheckByDate		= 'LCBD',
	item_LogCheckByState	= 'LCBS',
	item_LogCheckByRev		= 'LCBR',
	item_LogCheckByUser		= 'LCBU',
	item_LogEditByDate		= 'LGBD',
	item_LogEditByState		= 'LGBS',
	item_LogEditByRev		= 'LGBR',
	item_LogEditByUser		= 'LGBU',
	item_LogFillByDate		= 'LPBD',
	item_LogFillByState		= 'LPBS',
	item_LogFillByRev		= 'LPBR',
	item_LogFillByUser		= 'LPBU',

	item_CheckDate1			= 'DAT1',
	item_CheckDate2			= 'DAT2',
	
	item_MacBinEdit			= 'MBED', /* listened by the app */
	item_MacBinDelete		= 'MBDL', /* listened by the app */
	item_MacBinAdd			= 'MBAD', /* listened by the app */
	item_MacBinPopUp		= 'MBPO',
	item_MacBinOnlyBinaries		= 'MBBO',

	item_MacBinType			= 'MBTY',
	item_MacBinExtension = 'MBEX',
	item_MacBinDesc			= 'MBDE',
	item_MacBinDefault		= 'MBDF',
	item_MacBinHQX			= 'MBHQ',
	item_MacBinAS			= 'MBAS',
	item_MacBinPlain		= 'MBPB',
	item_MacBinList			= 'TABL',
	
	item_BrowserTable		= 'BROW',
	item_TreeView			= 'TREW',
	item_BrowserStaticText	= 'BRST',
	item_BrowserColumns		= 'COLU',

	item_SelectTagList		= 'TABL',
	item_Refresh			= 'REFR',

	item_OverwriteRev		= 'OREV',
	item_InAttic			= 'LATT',
	item_BranchRev			= 'REVB',
	item_CheckUnmod			= 'CUMD',
	
	item_HideCmdDlg     = 'HIDE',
	
	item_ProgressCancel = 'CANC',
	item_ProgressTitle = 'TITL',
	item_ProgressBar = 'PROG',
	item_ProgressText = 'TPRG',
	
	item_AddSelection = '+SEL',
	item_AddDefault = '+DEF',
	
	item_ForceCVSRoot = 'FCVR',
	item_CVSRoot      = 'CVSR',
	item_CVSRootWizard  = 'CVRW'
};

// dialogs
enum {
	dlg_Prefs				= 129,
	dlg_Commit				= 130,
	dlg_Import				= 131,
	dlg_Checkout			= 132,
	dlg_Passwd				= 133,
	dlg_Cmdline				= 134,
	dlg_ImpFilter			= 135,
	dlg_Update				= 136,
	dlg_Log					= 137,
	dlg_Diff				= 138,
	dlg_YesNo				= 139,
	dlg_RtagCreate			= 142,
	dlg_RtagDelete			= 143,
	dlg_RtagBranch			= 144,
	dlg_TagCreate			= 145,
	dlg_TagDelete			= 146,
	dlg_TagBranch			= 147,
	dlg_SaveSettings		= 148,
	dlg_PromptCommand		= 149,
	dlg_PromptEditCommand	= 150,
	dlg_NoPython			= 151,
	dlg_NoPythonMachO		= 152,
	dlg_Select				= 153,
	dlg_Progress      = 170,
	dlg_ImpFilterWarn		= 2010,
	dlg_ImpFilterEntry		= 2011,
	dlg_MacBinEntry			= 2019,
	dlg_About				= 3000
};

// menus #
enum {
	menu_Windows			= 144,
	menu_RecentBrowser		= 178,
	menu_MacroAdmin			= 179,
	menu_MacroFiles			= 180,
	menu_MacroFolder		= 181,
	menu_MacroSel			= 182
};

// windows #
enum {
	wind_Browser			= 140,
	wind_LogTree			= 141
};

// cursors #
enum {
	curs_SizeColumns		= 256
};

// "built-in" external diff tools
enum {
  diff_CodeWarrior = 1,
  diff_BBEdit,
  diff_FileMerge,
  diff_Custom
};

#define kDiffTool_CodeWarrior "{maccvs_res}diff_in_codewarrior"
#define kDiffTool_BBEdit      "{maccvs_res}diff_in_bbedit"
#define kDiffTool_FileMerge   "/usr/bin/opendiff"


// exchanges functions for each view
class MacBinMaps;
class UPStr;
class UStr;
class MultiFiles;

void DoDataExchange (LView *theDialog, PaneIDT paneID, bool & ioValue, bool putValue);
void DoDataExchange (LView *theDialog, PaneIDT paneID, SInt32 & ioValue, bool putValue);
void DoDataExchange (LView *theDialog, PaneIDT paneID, UStr & ioValue, bool putValue);
void DoDataExchange (LView *theDialog, PaneIDT paneID, UPStr & ioValue, bool putValue);
void DoDataExchange_Globals(LWindow *theDialog, bool putValue);
void DoDataExchange_TextFiles(LWindow *theDialog, bool putValue);
void DoDataExchange_BinFiles(LWindow *theDialog, MacBinMaps & maps, bool putValue);
void DoDataExchange_Fonts(LWindow *theDialog, bool putValue);
void DoHitButton_BinFiles(LWindow *theDialog, MessageT hitMessage);
void DoDataExchange_DiffTool(LWindow *theDialog, UStr& diffTool, bool& extDiff, bool allowExtDiff, bool putValue);
bool HandleHitMessage_DiffTool(LWindow *theDialog, MessageT hitMessage);
void SetupHandler_CVSRoot(LWindow* theDialog, StDialogHandler& inHandler);
void DoDataExchange_CVSRoot(LWindow *theDialog, bool& forceCvsroot, UStr& cvsroot, bool putValue);
bool HandleHitMessage_CVSRoot(LWindow *theDialog, MessageT hitMessage);
void DoDataExchange_UpdateOptions(LWindow *theDialog, 
                                  const MultiFiles* mf,
                                  bool & hasDate, 
                                  bool & hasRev,
                                  UStr & date, 
                                  UStr & rev, 
                                  bool & useMostRecent, 
                                  bool & resetSticky, 
                                  bool & toStdout,
		                              UStr & keyword,
		                              bool & caseSensitiveNames,
		                              bool & lastCheckinTime,
                                  bool putValue);

void DoDataExchange_UpdateMerge(LWindow *theDialog, 
                                const MultiFiles* mf,
	                              bool & merge1, 
	                              bool & merge2,
                                UStr & rev1, 
                                UStr & rev2,
                                bool & hasDate1,
                                UStr & date1,
                                bool & hasDate2,
                                UStr & date2,
                                bool & produce3WayConflicts,
                                bool & mergeFromBranchPoint,
                                bool putValue);
void SetupHandler_UpdateMerge(LWindow* theDialog, StDialogHandler& inHandler);
bool HandleHitMessage_UpdateMerge(LWindow *theDialog, MessageT hitMessage);

LListener *GetDlgHandler(void);
	// dialog handler which listens to the dialog event (the application in fact)

#endif /* MACCVSCONSTANT_H */
