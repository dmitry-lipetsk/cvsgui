[Setup]
AppName=WinCvs
AppVerName=WinCvs 2.0
AppPublisher=CvsGui
AppPublisherURL=http://www.wincvs.org
AppSupportURL=http://groups.yahoo.com/subscribe/cvsgui
AppUpdatesURL=http://sourceforge.net/projects/cvsgui/
DefaultDirName={pf}\GNU\WinCvs 2.0
DefaultGroupName=WinCvs
AllowNoIcons=true
LicenseFile=..\COPYING
InfoBeforeFile=Info.txt
EnableDirDoesntExistWarning=false
UninstallDisplayIcon={app}\wincvs.exe

AppID=D2D77DC2-8299-11D1-8949-444553540000
DisableStartupPrompt=true
AlwaysShowComponentsList=true
SolidCompression=true
Compression=lzma
InternalCompressLevel=normal
ShowLanguageDialog=yes
OutputBaseFilename=wincvs_setup
OutputDir=output

[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:
Name: quicklaunchicon; Description: Create a &Quick Launch icon; GroupDescription: Additional icons:
Name: shellregistry; Description: Create a shell context menu; GroupDescription: Shell:

[Files]
; WinCvs files
Source: ..\WinCvs\Release\wincvs.exe; DestDir: {app}; Components: Main; Flags: ignoreversion
Source: ..\WinCvs\Release\CJ609Lib.dll; DestDir: {app}; Components: Main; Flags: ignoreversion
Source: ..\WinCvs\Release\wincvs.chm; DestDir: {app}; Components: Main; Flags: ignoreversion
Source: ..\WinCvs\Release\tips.txt; DestDir: {app}; Components: Main; Flags: ignoreversion
Source: ..\COPYING; DestDir: {app}; Components: Main; Flags: ignoreversion
Source: ..\INSTALL; DestDir: {app}; Components: Help; Flags: ignoreversion
Source: ..\ChangeLog; DestDir: {app}; Components: Help; Flags: ignoreversion
Source: ..\WinCvs\INSTALL.txt; DestDir: {app}; Components: Help; Flags: ignoreversion
Source: ..\PythonLib\cvsgui\*.*; DestDir: {app}\PythonLib\cvsgui; Components: Macros; Flags: ignoreversion comparetimestamp

; CSVNT binaries
Source: output\cvsnt_setup.exe; DestDir: {app}\CVSNT; Components: Dos_commands; Flags: ignoreversion external skipifsourcedoesntexist dontcopy

; VC++ redistributable files, copied to the application directory to avoid the problems with versions
Source: cvsnt_setup_options.ini; DestDir: {app}; Components: Dos_commands
Source: vcredist\mfc42.dll; DestDir: {sys}; Components: Shared_DLLs; Flags: sharedfile uninsneveruninstall onlyifdoesntexist
Source: vcredist\msvcp60.dll; DestDir: {sys}; Components: Shared_DLLs; Flags: sharedfile uninsneveruninstall onlyifdoesntexist
Source: vcredist\msvcrt.dll; DestDir: {sys}; Components: Shared_DLLs; Flags: sharedfile uninsneveruninstall onlyifdoesntexist
; VC++ redistributable files, copied to the system directory - not used but keep that code here in case it is needed
;Source: "..\vcredist\mfc42.dll";    DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: restartreplace uninsneveruninstall regserver
;Source: "..\vcredist\msvcp60.dll";  DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: restartreplace uninsneveruninstall
;Source: "..\vcredist\msvcrt.dll";   DestDir: "{sys}"; CopyMode: alwaysskipifsameorolder; Flags: restartreplace uninsneveruninstall

; macros
Source: ..\Macros\AddDelIgnore.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\AddRootModule.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\Bonsai.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\BrowseRepoTk.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ChangeRepo.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ChangeRoot.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ChangeSubstModeTk.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\Cleanup.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\CleanupMissing.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\CmdHere.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ConflictScanner.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\CopyToBranch.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\CopyToHeadVersion.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\cvs2cl.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\CvsVersion.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\CvsView.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\EditFileTypeAssoc.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ExtDiffFiles.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ExtendedAddTk.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\FastSearch.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\FixDST.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ListModules.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ListTags.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\MoveBranchPointToHead.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\Override2ndPartyAdd.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\RecursiveAddTk.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ResurrectFile.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\RListModule.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\SearchLogComments.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ShowLogEntry.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\TagAndUpdate.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\test.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ZipSelected.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros

Source: ..\Macros\TemplateMacro.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\TemplateMacroTk.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros

Source: ..\Macros\EditSafely.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ForceUpdate.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ListDeleted.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ListLockedFiles.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ListNonCVS.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\ListStickyTags.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\PrepPatch.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\QueryState.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\RevertChanges.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\SelectionTest.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\startup.tcl; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros
Source: ..\Macros\DocMacros.txt; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros

; new, experimental macros
Source: ..\Macros\cvs2clTk.py; DestDir: {app}\Macros; Flags: comparetimestamp ignoreversion; Components: Macros\NewMacros

; old, deprecated macros
Source: ..\Macros\ChangeRoot.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\ChangeRootTK.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\Cleanup.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\ColorTest.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\cvs2cl.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\CvsAddAll.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\cvsignore_add.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\cvsignore_remove.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\HideAdminFiles.py; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\FastModSearch.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\FolderTest.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\ListModules.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\SetCurrentVersion.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros
Source: ..\Macros\TclVersion.tcl; DestDir: {app}\Macros; Flags: ignoreversion; Components: Macros\OldMacros

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
Filename: {src}\cvsnt_setup.exe; Flags: skipifdoesntexist postinstall skipifsilent nowait; Parameters: "/LOADINF=""{app}\cvsnt_setup_options.ini"""; Description: Install CVSNT (required to execute CVS commands); Components: Dos_commands
Filename: {app}\cvsgui-list.url; WorkingDir: {app}; Description: Go to CvsGui mailing list subscription page; Flags: nowait shellexec postinstall skipifsilent unchecked
Filename: {app}\wincvs.exe; Description: Launch WinCvs; Flags: nowait postinstall skipifsilent unchecked skipifdoesntexist

[Components]
Name: Main; Description: Core executable files; Flags: fixed; Types: custom full
Name: Dos_commands; Description: Command line client files; Types: custom full
Name: Help; Description: Additional documentation files; Types: custom full
Name: Shared_DLLs; Description: Shared DLL libraries; Types: custom full
Name: Macros; Description: Macros; Types: custom full; Flags: checkablealone
Name: Macros\NewMacros; Description: Experimental, new Macros; Types: custom full; Flags: dontinheritcheck
Name: Macros\OldMacros; Description: Deprecated, old Macros; Flags: dontinheritcheck

[Types]
Name: full; Description: Full installation (recommended)
Name: custom; Description: Custom installation; Flags: iscustom

[_ISTool]
EnableISX=false

[UninstallDelete]
Name: {app}\cvsgui.url; Type: files
Name: {app}\cvsgui-list.url; Type: files
Name: {app}\cvsgui-project.url; Type: files
Type: files; Name: {app}\cvsgui-list-archive.url
Type: files; Name: {app}\cvsgui-faq.url

[Registry]
Root: HKCR; SubKey: Folder\shell\wincvs; ValueType: string; ValueData: Browse with &WinCvs; Tasks: shellregistry; Flags: uninsdeletekey
Root: HKCR; SubKey: Folder\shell\wincvs\command; ValueType: string; ValueData: """{app}\wincvs.exe"" ""%1""\"; Tasks: shellregistry; Flags: uninsdeletekey
