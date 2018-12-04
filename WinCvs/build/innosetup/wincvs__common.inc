#define WINCVS_SRC_DIR1             "..\..\output\"+WINCVS_SRC_DIR1_BIT
#define WINCVS_SRC_DIR2             "..\..\..\"

#define WINCVS_SRC_FILE1(name) WINCVS_SRC_DIR1+name
#define WINCVS_SRC_FILE2(name) WINCVS_SRC_DIR2+name

#define WINCVS_VERSION              GetFileVersion(WINCVS_SRC_FILE1("wincvs.exe"))

#define WINCVS_INST_DEF_DIR         "{pf}\"+WINCVS_APP_DEF_DIR_NAME
#define WINCVS_INST_APP_NAME        WINCVS_APP_NAME
#define WINCVS_INST_APP_VER_NAME    WINCVS_APP_NAME+" "+WINCVS_VERSION
#define WINCVS_INST_APP_PUBLISHER   WINCVS_APP_PUBLISHER

#define WINCVS_OUT_DIR              "..\..\output"
#define WINCVS_OUT_BASEFILENAME     "wincvs_setup__"+WINCVS_OUT_BASEFILENAME_SUFFIX_BIT+"_"+WINCVS_VERSION

#define WINCVS_INST_VI_COMPANY      WINCVS_APP_PUBLISHER
#define WINCVS_INST_VI_PRIMARY_VER  WINCVS_VERSION

[Setup]
AppName                       ={#WINCVS_INST_APP_NAME}
AppVerName                    ={#WINCVS_INST_APP_VER_NAME}
AppPublisher                  ={#WINCVS_INST_APP_PUBLISHER}
AppPublisherURL               =http://www.wincvs.org
AppSupportURL                 =http://groups.yahoo.com/subscribe/cvsgui
AppUpdatesURL                 =http://sourceforge.net/projects/cvsgui/
DefaultDirName                ={#WINCVS_INST_DEF_DIR}
DefaultGroupName              ={#WINCVS_APP_NAME}
AllowNoIcons                  =true
LicenseFile                   ={#WINCVS_SRC_FILE2("COPYING")}
InfoBeforeFile                =info.txt
EnableDirDoesntExistWarning   =false
UninstallDisplayIcon          ={app}\wincvs.exe

AppID                         =D2D77DC2-8299-11D1-8949-444553540000
DisableStartupPrompt          =true
AlwaysShowComponentsList      =true
SolidCompression              =true
Compression                   =lzma
InternalCompressLevel         =normal
ShowLanguageDialog            =yes
OutputDir                     ={#WINCVS_OUT_DIR}
OutputBaseFilename            ={#WINCVS_OUT_BASEFILENAME}

VersionInfoCompany            ={#WINCVS_INST_VI_COMPANY}
VersionInfoVersion            ={#WINCVS_INST_VI_PRIMARY_VER}
VersionInfoTextVersion        ={#WINCVS_INST_VI_PRIMARY_VER}
VersionInfoProductVersion     ={#WINCVS_INST_VI_PRIMARY_VER}
VersionInfoProductTextVersion ={#WINCVS_INST_VI_PRIMARY_VER}


[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:
Name: quicklaunchicon; Description: Create a &Quick Launch icon; GroupDescription: Additional icons:
Name: shellregistry; Description: Create a shell context menu; GroupDescription: Shell:

[Files]
; WinCvs files
Source: {#WINCVS_SRC_FILE1("wincvs.exe")};   DestDir: {app}; Components: Main; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE1("CJ609Lib.dll")}; DestDir: {app}; Components: Main; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE1("wincvs.chm")};   DestDir: {app}; Components: Main; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE1("tips.txt")};     DestDir: {app}; Components: Main; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE2("COPYING")}; DestDir: {app}; Components: Main; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE2("INSTALL")}; DestDir: {app}; Components: Help; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE2("ChangeLog")}; DestDir: {app}; Components: Help; Flags: ignoreversion
;Source: ..\WinCvs\INSTALL.txt; DestDir: {app}; Components: Help; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE2("PythonLib\cvsgui\*.*")}; DestDir: {app}\PythonLib\cvsgui; Components: Macros; Flags: ignoreversion comparetimestamp
Source: {#WINCVS_SRC_FILE1("wincvs.pdb")};   DestDir: {app}; Components: Debug_PDB; Flags: ignoreversion
Source: {#WINCVS_SRC_FILE1("CJ609Lib.pdb")}; DestDir: {app}; Components: Debug_PDB; Flags: ignoreversion

; CSVNT binaries
;;Source: output\cvsnt_setup.exe; DestDir: {app}\CVSNT; Components: Dos_commands; Flags: ignoreversion external skipifsourcedoesntexist dontcopy

; VC++ redistributable files, copied to the application directory to avoid the problems with versions
;;Source: cvsnt_setup_options.ini; DestDir: {app}; Components: Dos_commands
Source: {#WINCVS_SRC_FILE1("redist\*.*")};   DestDir: "{app}"; Components: Main_CRT; Flags: ignoreversion 

; macros
Source: {#WINCVS_SRC_FILE2("Macros\AddDelIgnore.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\AddRootModule.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\Bonsai.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\BrowseRepoTk.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ChangeRepo.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ChangeRoot.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ChangeSubstModeTk.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\Cleanup.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\CleanupMissing.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\CmdHere.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ConflictScanner.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\CopyToBranch.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\CopyToHeadVersion.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\cvs2cl.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\CvsVersion.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\CvsView.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\EditFileTypeAssoc.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ExtDiffFiles.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ExtendedAddTk.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\FastSearch.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\FixDST.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ListModules.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ListTags.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\MoveBranchPointToHead.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\Override2ndPartyAdd.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\RecursiveAddTk.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ResurrectFile.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\RListModule.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\SearchLogComments.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ShowLogEntry.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\TagAndUpdate.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\test.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ZipSelected.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros

Source: {#WINCVS_SRC_FILE2("Macros\TemplateMacro.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\TemplateMacroTk.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros

Source: {#WINCVS_SRC_FILE2("Macros\EditSafely.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ForceUpdate.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ListDeleted.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ListLockedFiles.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ListNonCVS.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\ListStickyTags.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\PrepPatch.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\QueryState.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\RevertChanges.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\SelectionTest.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\startup.tcl")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: {#WINCVS_SRC_FILE2("Macros\DocMacros.txt")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros

; new, experimental macros
Source: {#WINCVS_SRC_FILE2("Macros\cvs2clTk.py")}; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros\NewMacros

; old, deprecated macros
Source: {#WINCVS_SRC_FILE2("Macros\ChangeRoot.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\ChangeRootTK.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\Cleanup.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\ColorTest.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\cvs2cl.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\CvsAddAll.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\cvsignore_add.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\cvsignore_remove.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\HideAdminFiles.py")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\FastModSearch.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\FolderTest.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\ListModules.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\SetCurrentVersion.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: {#WINCVS_SRC_FILE2("Macros\TclVersion.tcl")}; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros

[INI]
Filename: {app}\cvsgui.url; Section: InternetShortcut; Key: URL; String: http://cvsgui.sourceforge.net/; Components: Main
Filename: {app}\cvsgui-faq.url; Section: InternetShortcut; Key: URL; String: http://cvsgui.sf.net/newfaq.htm; Components: Main
Filename: {app}\cvsgui-list.url; Section: InternetShortcut; Key: URL; String: http://groups.yahoo.com/subscribe/cvsgui; Components: Main
Filename: {app}\cvsgui-list-archive.url; Section: InternetShortcut; Key: URL; String: http://groups.yahoo.com/messages/cvsgui; Components: Main
Filename: {app}\cvsgui-project.url; Section: InternetShortcut; Key: URL; String: http://sourceforge.net/projects/cvsgui/; Components: Main

[Icons]
Name: {group}\WinCvs; Filename: {app}\wincvs.exe; IconIndex: 0; Flags: createonlyiffileexists
Name: {userdesktop}\WinCvs; Filename: {app}\wincvs.exe; Tasks: desktopicon; IconIndex: 0; Flags: createonlyiffileexists
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinCvs; Filename: {app}\wincvs.exe; Tasks: quicklaunchicon; IconIndex: 0; Flags: createonlyiffileexists
Name: {group}\WinCvs Help; Filename: {app}\wincvs.chm; WorkingDir: {app}; Comment: WinCvs Help; Flags: createonlyiffileexists
Name: {group}\Web\CvsGui Homepage; Filename: {app}\cvsgui.url; WorkingDir: {app}; Comment: CvsGui Homepage; Components: Main
Name: {group}\Web\WinCvs FAQ (work in progress); Filename: {app}\cvsgui-faq.url; WorkingDir: {app}; Comment: WinCvs FAQ; Components: Main
Name: {group}\Web\CvsGui mailing list; Filename: {app}\cvsgui-list.url; WorkingDir: {app}; Comment: CvsGui mailing list; Components: Main
Name: {group}\Web\CvsGui mailing list archive; Filename: {app}\cvsgui-list-archive.url; WorkingDir: {app}; Components: Main
Name: {group}\Web\CvsGui project page; Filename: {app}\cvsgui-project.url; WorkingDir: {app}; Comment: CvsGui project page; Components: Main

[Run]
;;Filename: {src}\cvsnt_setup.exe; Flags: skipifdoesntexist postinstall skipifsilent nowait; Parameters: "/LOADINF=""{app}\cvsnt_setup_options.ini"""; Description: Install CVSNT (required to execute CVS commands); Components: Dos_commands
Filename: {app}\cvsgui-list.url; WorkingDir: {app}; Description: Go to CvsGui mailing list subscription page; Flags: nowait shellexec postinstall skipifsilent unchecked
Filename: {app}\wincvs.exe; Description: Launch WinCvs; Flags: nowait postinstall skipifsilent unchecked skipifdoesntexist

[Components]
Name: Main; Description: Core executable files; Types: TYPE_Custom;
;Name: Dos_commands; Description: Command line client files; Types: custom full
Name: Help; Description: Additional documentation files; Types: TYPE_Custom;
Name: Main_CRT; Description: Runtime Library (CRT, MFC); Types: TYPE_Custom;
Name: Macros; Description: Macros; Types: TYPE_Custom; Flags: checkablealone;
Name: Macros\NewMacros; Description: Experimental, new Macros; Types: TYPE_Custom;
Name: Macros\OldMacros; Description: Deprecated, old Macros; Flags: dontinheritcheck;
Name: Debug_PDB; Description: Debug: PDB files;

[Types]
Name: TYPE_Custom; Description: Custom installation; Flags: iscustom

[_ISTool]
EnableISX=false

[UninstallDelete]
Name: {app}\cvsgui.url; Type: files
Name: {app}\cvsgui-list.url; Type: files
Name: {app}\cvsgui-project.url; Type: files
Type: files; Name: {app}\cvsgui-list-archive.url
Type: files; Name: {app}\cvsgui-faq.url

[Registry]
Root: HKCR; SubKey: Folder\shell\wincvs; ValueType: string; ValueData: Browse with &{#WINCVS_APP_NAME}; Tasks: shellregistry; Flags: uninsdeletekey
Root: HKCR; SubKey: Folder\shell\wincvs\command; ValueType: string; ValueData: """{app}\wincvs.exe"" ""%1""\"; Tasks: shellregistry; Flags: uninsdeletekey
