# Microsoft Developer Studio Project File - Name="wincvs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wincvs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wincvs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wincvs.mak" CFG="wincvs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wincvs - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wincvs - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wincvs - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /Ob2 /I "../wincvs" /I "../common" /I "../rf" /I "../rf/support/mfc" /I "../../tcl/include" /I "../cvstree" /I "CJLib/include" /I "../cvsgui_protocol" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib shlwapi.lib htmlhelp.lib shfolder.lib /nologo /subsystem:windows /map /debug /machine:I386 /mapinfo:lines /opt:ref
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../cvsgui_p" /I "../wincvs" /I "../common" /I "../rf" /I "../rf/support/mfc" /I "../../tcl/include" /I "../cvstree" /I "CJLib/include" /I "../cvsgui_protocol" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /o "NUL" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib shlwapi.lib htmlhelp.lib shfolder.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "wincvs - Win32 Release"
# Name "wincvs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BrowseFileView.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseFileView.h
# End Source File
# Begin Source File

SOURCE=.\BrowserBar.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowserBar.h
# End Source File
# Begin Source File

SOURCE=.\ConsoleBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleBar.h
# End Source File
# Begin Source File

SOURCE=.\CustomizeMenus.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomizeMenus.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsSandboxInfo.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=.\CvsStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CvsStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\ExploreBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ExploreBar.h
# End Source File
# Begin Source File

SOURCE=.\FileViewWatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\FileViewWatcher.h
# End Source File
# Begin Source File

SOURCE=.\GraphDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphDoc.h
# End Source File
# Begin Source File

SOURCE=.\GraphFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphFrm.h
# End Source File
# Begin Source File

SOURCE=.\GraphView.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphView.h
# End Source File
# Begin Source File

SOURCE=.\LogFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ndir.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\NoPython.cpp
# End Source File
# Begin Source File

SOURCE=.\NoPython.h
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Stat.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\wincvs.cpp
# End Source File
# Begin Source File

SOURCE=.\wincvs.h
# End Source File
# Begin Source File

SOURCE=.\wincvs.odl
# End Source File
# Begin Source File

SOURCE=.\wincvs.rc
# End Source File
# Begin Source File

SOURCE=.\wincvs_winutil.cpp
# End Source File
# Begin Source File

SOURCE=.\wincvs_winutil.h
# End Source File
# Begin Source File

SOURCE=.\WinCvsBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\WinCvsBrowser.h
# End Source File
# Begin Source File

SOURCE=.\WinCvsDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\WinCvsDebug.h
# End Source File
# Begin Source File

SOURCE=.\wincvsDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\wincvsDoc.h
# End Source File
# Begin Source File

SOURCE=.\wincvsView.cpp
# End Source File
# Begin Source File

SOURCE=.\wincvsView.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\CvsSandboxInfo.h
# End Source File
# Begin Source File

SOURCE=.\ndir.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "wincvs - Win32 Release"

# Begin Custom Build - Making help include file...
TargetName=wincvs
InputPath=.\Resource.h

BuildCmds= \
	MakeHHelp.bat $(TargetName)

"HHelp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"HHelp\$(TargetName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# Begin Custom Build - Making help include file...
TargetName=wincvs
InputPath=.\Resource.h

BuildCmds= \
	MakeHHelp.bat $(TargetName)

"HHelp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"HHelp\$(TargetName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\version_fu.h
# End Source File
# Begin Source File

SOURCE=.\version_no.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\about.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Browsbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_popup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\custmenu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\filelist.bmp
# End Source File
# Begin Source File

SOURCE=.\res\histloc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\idle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\idr_cntr.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_grap.ico
# End Source File
# Begin Source File

SOURCE=.\res\import.bmp
# End Source File
# Begin Source File

SOURCE=.\res\itemlist.bmp
# End Source File
# Begin Source File

SOURCE=.\res\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\locate.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MB_Exclamation.ICO
# End Source File
# Begin Source File

SOURCE=.\res\MB_Information.ICO
# End Source File
# Begin Source File

SOURCE=.\res\MB_Question.ICO
# End Source File
# Begin Source File

SOURCE=.\res\MB_Stop.ICO
# End Source File
# Begin Source File

SOURCE=.\res\modules.bmp
# End Source File
# Begin Source File

SOURCE=.\res\multius.bmp
# End Source File
# Begin Source File

SOURCE=.\res\multiuse.bmp
# End Source File
# Begin Source File

SOURCE=.\res\python.bmp
# End Source File
# Begin Source File

SOURCE=.\res\smallico.bmp
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stateico.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbFilter.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\wincvs.ico
# End Source File
# Begin Source File

SOURCE=.\res\wincvs.rc2
# End Source File
# Begin Source File

SOURCE=.\res\wincvsDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\workspac.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HHelp\cshelp.txt
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MakeHHelp.bat
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\HHelp\tips.txt

!IF  "$(CFG)" == "wincvs - Win32 Release"

# Begin Custom Build
OutDir=.\Release
InputPath=.\HHelp\tips.txt

"$(OutDir)\tips.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "HHelp\tips.txt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\HHelp\tips.txt

"$(OutDir)\tips.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "HHelp\tips.txt" $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HHelp\toc.hhc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\HHelp\wincvs.hhk
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\HHelp\wincvs.hhp

!IF  "$(CFG)" == "wincvs - Win32 Release"

USERDEP__WINCV="HHelp\$(TargetName).hm"	"HHelp\cshelp.txt"	
# Begin Custom Build - Making HTML help file...
OutDir=.\Release
TargetName=wincvs
InputPath=.\HHelp\wincvs.hhp
InputName=wincvs

"HHelp\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hhc.exe "HHelp\$(InputName).hhp" 
	if not exist "HHelp\$(InputName).chm" goto :Error 
	copy "HHelp\$(InputName).chm" $(OutDir) 
	goto :done 
	:Error 
	echo "HHelp\$(InputName).hhp(1)" : error: 
	type "HHelp\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

USERDEP__WINCV="HHelp\$(TargetName).hm"	"HHelp\cshelp.txt"	
# Begin Custom Build - Making HTML help file...
OutDir=.\Debug
TargetName=wincvs
InputPath=.\HHelp\wincvs.hhp
InputName=wincvs

"HHelp\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hhc.exe "HHelp\$(InputName).hhp" 
	if not exist "HHelp\$(InputName).chm" goto :Error 
	copy "HHelp\$(InputName).chm" $(OutDir) 
	goto :done 
	:Error 
	echo "HHelp\$(InputName).hhp(1)" : error: 
	type "HHelp\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Common Gui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\AdminOptionDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\AdminOptionDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\AnnotateDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\AnnotateDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\AppConsole.cpp
# End Source File
# Begin Source File

SOURCE=..\common\AppConsole.h
# End Source File
# Begin Source File

SOURCE=..\common\AppGlue.cpp
# End Source File
# Begin Source File

SOURCE=..\common\AppGlue.h
# End Source File
# Begin Source File

SOURCE=..\common\AskYesNo.cpp
# End Source File
# Begin Source File

SOURCE=..\common\AskYesNo.h
# End Source File
# Begin Source File

SOURCE=..\common\Authen.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Authen.h
# End Source File
# Begin Source File

SOURCE=..\common\BrowseViewColumn.cpp
# End Source File
# Begin Source File

SOURCE=..\common\BrowseViewColumn.h
# End Source File
# Begin Source File

SOURCE=..\common\BrowseViewHandlers.cpp
# End Source File
# Begin Source File

SOURCE=..\common\BrowseViewHandlers.h
# End Source File
# Begin Source File

SOURCE=..\common\BrowseViewModel.cpp
# End Source File
# Begin Source File

SOURCE=..\common\BrowseViewModel.h
# End Source File
# Begin Source File

SOURCE=..\common\CheckoutDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CheckoutDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\CommitDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CommitDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsAlert.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CvsAlert.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsArgs.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CvsArgs.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsCommands.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CvsCommands.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsEntries.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CvsEntries.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsIgnore.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CvsIgnore.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsPrefs.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CvsPrefs.h
# End Source File
# Begin Source File

SOURCE=..\common\CvsRootDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\CvsRootDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\DiffDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\DiffDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\DiffParse.cpp
# End Source File
# Begin Source File

SOURCE=..\common\DiffParse.h
# End Source File
# Begin Source File

SOURCE=..\common\dll_loader.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\dll_loader.h
# End Source File
# Begin Source File

SOURCE=..\common\EditorMonDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\EditorMonDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\FileActionDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\FileActionDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\FileTraversal.cpp
# End Source File
# Begin Source File

SOURCE=..\common\FileTraversal.h
# End Source File
# Begin Source File

SOURCE=..\common\FilterMaskOptDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\FilterMaskOptDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\FilterSelectionDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\FilterSelectionDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\fnmatch.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\fnmatch.h
# End Source File
# Begin Source File

SOURCE=..\common\getline.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\getline.h
# End Source File
# Begin Source File

SOURCE=..\common\getopt.c
# ADD CPP /D "HAVE_STRING_H"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\getopt.h
# End Source File
# Begin Source File

SOURCE=..\common\getopt1.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\GetPassword.cpp
# End Source File
# Begin Source File

SOURCE=..\common\GetPassword.h
# End Source File
# Begin Source File

SOURCE=..\common\GetPrefs.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\GetPrefs.h
# End Source File
# Begin Source File

SOURCE=..\common\GraphOptDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\GraphOptDlg.h
# End Source File
# Begin Source File

SOURCE=.\HHelpSupport.cpp
# End Source File
# Begin Source File

SOURCE=.\HHelpSupport.h
# End Source File
# Begin Source File

SOURCE=..\common\ImportDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ImportDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\ImportFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ImportFilter.h
# End Source File
# Begin Source File

SOURCE=..\common\ImportFilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ImportFilterDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\initdlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\initdlg.h
# End Source File
# Begin Source File

SOURCE=..\common\ItemListDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\ItemListDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\LaunchHandlers.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LaunchHandlers.h
# End Source File
# Begin Source File

SOURCE=..\common\LineCmd.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LineCmd.h
# End Source File
# Begin Source File

SOURCE=..\common\LogDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LogDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\LoginDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LoginDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\LogParse.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LogParse.h
# End Source File
# Begin Source File

SOURCE=..\common\MakeSurePathExists.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MakeSurePathExists.h
# End Source File
# Begin Source File

SOURCE=.\MfFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MfFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\MMenuString.cpp
# End Source File
# Begin Source File

SOURCE=.\MMenuString.h
# End Source File
# Begin Source File

SOURCE=..\common\MoveToTrash.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MoveToTrash.h
# End Source File
# Begin Source File

SOURCE=..\common\MultiFiles.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MultiFiles.h
# End Source File
# Begin Source File

SOURCE=..\common\MultiString.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MultiString.h
# End Source File
# Begin Source File

SOURCE=..\common\OptionalPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=..\common\OptionalPropertySheet.h
# End Source File
# Begin Source File

SOURCE=..\common\Persistent.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\Persistent.h
# End Source File
# Begin Source File

SOURCE=..\common\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\PromptFiles.cpp
# End Source File
# Begin Source File

SOURCE=..\common\PromptFiles.h
# End Source File
# Begin Source File

SOURCE=..\common\ReleaseDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\ReleaseDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\RemoveDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\RemoveDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\RtagDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\RtagDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\SaveSettingsDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\SaveSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\SelectionDetailsDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\SelectionDetailsDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\SortList.h
# End Source File
# Begin Source File

SOURCE=..\common\StatusDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\StatusDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\TagDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\TagDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\TemplateDlg.cpp
# ADD CPP /I "..\WinCvs"
# End Source File
# Begin Source File

SOURCE=..\common\TemplateDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\TextBinary.cpp
# End Source File
# Begin Source File

SOURCE=..\common\TextBinary.h
# End Source File
# Begin Source File

SOURCE=..\common\UpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\common\UpdateDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\UpdateToTempParse.cpp
# End Source File
# Begin Source File

SOURCE=..\common\UpdateToTempParse.h
# End Source File
# End Group
# Begin Group "Relay Framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\rf\support\mfc\dib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\dib.h
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\PseudoButton.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\PseudoButton.h
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\PseudoDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\PseudoDlg.h
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\PseudoDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\PseudoDraw.h
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\PseudoList.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\support\mfc\pseudolist.h
# End Source File
# Begin Source File

SOURCE=..\rf\uconsole.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\uconsole.h
# End Source File
# Begin Source File

SOURCE=..\rf\uevent.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\umain.h
# End Source File
# Begin Source File

SOURCE=..\rf\umenu.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\umenu.h
# End Source File
# Begin Source File

SOURCE=..\rf\uobject.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\uobject.h
# End Source File
# Begin Source File

SOURCE=..\rf\ustr.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\ustr.h
# End Source File
# Begin Source File

SOURCE=..\rf\utoolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\utoolbar.h
# End Source File
# Begin Source File

SOURCE=..\rf\uwidget.cpp
# End Source File
# Begin Source File

SOURCE=..\rf\uwidget.h
# End Source File
# End Group
# Begin Group "cvsgui protocol"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\cvsgui_protocol\cvsgui_process.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\cvsgui_protocol\cvsgui_process.h
# End Source File
# Begin Source File

SOURCE=..\cvsgui_protocol\cvsgui_protocol.h
# End Source File
# Begin Source File

SOURCE=..\cvsgui_protocol\cvsgui_wire.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\cvsgui_protocol\cvsgui_wire.h
# End Source File
# End Group
# Begin Group "PythonLib"

# PROP Default_Filter ""
# Begin Group "cvsgui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\PythonLib\cvsgui\__init__.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\App.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\CmdUI.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\ColorConsole.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\Cvs.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\CvsEntry.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\CvsLog.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\Macro.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\MacroRegistry.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\MacroUtils.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\MenuMgr.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\Persistent.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\SafeTk.py

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\Shell.py
# End Source File
# Begin Source File

SOURCE=..\PythonLib\cvsgui\SubstModeTk.py
# End Source File
# End Group
# Begin Source File

SOURCE=..\common\MacrosSetup.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MacrosSetup.h
# End Source File
# Begin Source File

SOURCE=..\common\PythonContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\common\PythonContainer.h
# End Source File
# Begin Source File

SOURCE=..\common\PythonGlue.cpp
# End Source File
# Begin Source File

SOURCE=..\common\PythonGlue.h
# End Source File
# Begin Source File

SOURCE=..\common\PythonGlue.impl.h
# End Source File
# Begin Source File

SOURCE=..\common\PythonImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\common\PythonImpl.h
# End Source File
# Begin Source File

SOURCE=..\common\PythonMacros.h
# End Source File
# Begin Source File

SOURCE=..\common\TclGlue.cpp
# End Source File
# Begin Source File

SOURCE=..\common\TclGlue.h
# End Source File
# End Group
# Begin Group "Macros"

# PROP Default_Filter ""
# Begin Group "Python"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Macros\AddDelIgnore.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\AddRootModule.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\Bonsai.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\BrowseRepoTk.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ChangeRepo.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ChangeRoot.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ChangeSubstModeTk.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\Cleanup.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CleanupMissing.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CmdHere.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ConflictScanner.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CopyToBranch.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CopyToHeadVersion.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\cvs2cl.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\cvs2clTk.py
# End Source File
# Begin Source File

SOURCE=..\Macros\CvsAlertTest.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CvsPromptTest.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CvsVersion.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CvsView.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\EditFileTypeAssoc.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ExtDiffFiles.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ExtendedAddTk.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\FastSearch.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\FixDST.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\HideAdminFiles.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ListModules.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ListTags.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\MoveBranchPointToHead.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\Override2ndPartyAdd.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\RecursiveAddTk.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ResurrectFile.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\RListModule.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\SearchLogComments.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ShowLogEntry.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\TagAndUpdate.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\TemplateMacro.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\TemplateMacroTk.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\test.py
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ZipSelected.py
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "TCL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Macros\ChangeRoot.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ChangeRootTK.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\Cleanup.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ColorTest.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\cvs2cl.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\CvsAddAll.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\cvsignore_add.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\cvsignore_remove.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\EditSafely.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\FastModSearch.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\FolderTest.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ForceUpdate.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ListDeleted.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ListLockedFiles.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ListModules.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ListNonCVS.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\ListStickyTags.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\PrepPatch.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\QueryState.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\RevertChanges.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\SelectionTest.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\SetCurrentVersion.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\startup.tcl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Macros\TclVersion.tcl
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=..\Macros\DocMacros.txt

!IF  "$(CFG)" == "wincvs - Win32 Release"

!ELSEIF  "$(CFG)" == "wincvs - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ColorStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorStatic.h
# End Source File
# Begin Source File

SOURCE=.\CvsrootEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CvsrootEntryDlg.h
# End Source File
# Begin Source File

SOURCE=.\HistComboBoxEx.cpp
# End Source File
# Begin Source File

SOURCE=.\HistComboBoxEx.h
# End Source File
# Begin Source File

SOURCE=.\HtmlStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlStatic.h
# End Source File
# Begin Source File

SOURCE=.\MultilineEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MultilineEntryDlg.h
# End Source File
# Begin Source File

SOURCE=..\common\SmartComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\common\SmartComboBox.h
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SubstModeEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SubstModeEntryDlg.h
# End Source File
# End Group
# Begin Group "Inno Setup"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\InnoSetup\Info.txt
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\InnoSetup\WinCvs.iss
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Doxygen"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SourceDoc\DoDoxygen.bat
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\SourceDoc\DoDoxygen.sh
# End Source File
# Begin Source File

SOURCE=..\SourceDoc\Doxyfile
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\SourceDoc\Readme.txt
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "LibIntl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\cvsgui_i18n.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\ChangeLog
# End Source File
# Begin Source File

SOURCE=..\TODO
# End Source File
# End Target
# End Project
# Section wincvs : {A82AB342-BB35-11CF-8771-00A0C9039735}
# 	1:17:CG_IDS_DIDYOUKNOW:108
# 	1:22:CG_IDS_TIPOFTHEDAYMENU:107
# 	1:18:CG_IDS_TIPOFTHEDAY:106
# 	1:22:CG_IDS_TIPOFTHEDAYHELP:111
# 	1:19:CG_IDP_FILE_CORRUPT:110
# 	1:7:IDD_TIP:105
# 	1:13:IDB_LIGHTBULB:103
# 	1:18:CG_IDS_FILE_ABSENT:109
# 	2:17:CG_IDS_DIDYOUKNOW:CG_IDS_DIDYOUKNOW
# 	2:7:CTipDlg:CTipDlg
# 	2:22:CG_IDS_TIPOFTHEDAYMENU:CG_IDS_TIPOFTHEDAYMENU
# 	2:18:CG_IDS_TIPOFTHEDAY:CG_IDS_TIPOFTHEDAY
# 	2:12:CTIP_Written:OK
# 	2:22:CG_IDS_TIPOFTHEDAYHELP:CG_IDS_TIPOFTHEDAYHELP
# 	2:2:BH:
# 	2:19:CG_IDP_FILE_CORRUPT:CG_IDP_FILE_CORRUPT
# 	2:7:IDD_TIP:IDD_TIP
# 	2:8:TipDlg.h:TipDlg.h
# 	2:13:IDB_LIGHTBULB:IDB_LIGHTBULB
# 	2:18:CG_IDS_FILE_ABSENT:CG_IDS_FILE_ABSENT
# 	2:10:TipDlg.cpp:TipDlg.cpp
# End Section
# Section wincvs : {51C21334-EBD1-D545-92BD-CF11877200A0}
# 	1:10:IDB_SPLASH:112
# 	2:21:SplashScreenInsertKey:4.0
# End Section
