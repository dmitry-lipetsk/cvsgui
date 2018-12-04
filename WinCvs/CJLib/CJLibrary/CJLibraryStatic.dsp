# Microsoft Developer Studio Project File - Name="CJLibraryStatic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CJLibraryStatic - Win32 Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CJLibraryStatic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CJLibraryStatic.mak"\
 CFG="CJLibraryStatic - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CJLibraryStatic - Win32 Debug" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "CJLibraryStatic - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "CJLibraryStatic - Win32 Debug Unicode" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "CJLibraryStatic - Win32 Release Unicode" (based on\
 "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
CPP=cl.exe

!IF  "$(CFG)" == "CJLibraryStatic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Static_Debug"
# PROP Intermediate_Dir "Static_Debug"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
# ADD BASE CPP /nologo /W3 /GX /Zi /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /Zi /Z7 /Od /I "../Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\CJ608LibStaticd.lib"

!ELSEIF  "$(CFG)" == "CJLibraryStatic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Static_Release"
# PROP Intermediate_Dir "Static_Release"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
# ADD BASE CPP /nologo /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /Zi /O2 /I "../Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\CJ608LibStatic.lib"

!ELSEIF  "$(CFG)" == "CJLibraryStatic - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CJLibraryStatic___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "CJLibraryStatic___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Static_Debug_Unicode"
# PROP Intermediate_Dir "Static_Debug_Unicode"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
# ADD BASE CPP /nologo /W3 /GX /Zi /Z7 /Od /I "../Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /Zi /Z7 /Od /I "../Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib\CJ608LibStaticd.lib"
# ADD LIB32 /nologo /out:"..\Lib\CJ608LibStaticud.lib"

!ELSEIF  "$(CFG)" == "CJLibraryStatic - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CJLibraryStatic___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "CJLibraryStatic___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Static_Release_Unicode"
# PROP Intermediate_Dir "Static_Release_Unicode"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
# ADD BASE CPP /nologo /W3 /GX /Zi /O2 /I "../Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /Zi /O2 /I "../Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib\CJ608LibStatic.lib"
# ADD LIB32 /nologo /out:"..\Lib\CJ608LibStaticu.lib"

!ENDIF 

# Begin Target

# Name "CJLibraryStatic - Win32 Debug"
# Name "CJLibraryStatic - Win32 Release"
# Name "CJLibraryStatic - Win32 Debug Unicode"
# Name "CJLibraryStatic - Win32 Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CJBrowseEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\CJCaption.cpp
# End Source File
# Begin Source File

SOURCE=.\CJCaptionPopupWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJColorPicker.cpp
# End Source File
# Begin Source File

SOURCE=.\CJColorPopup.cpp
# End Source File
# Begin Source File

SOURCE=.\CJComboBoxEx.cpp
# End Source File
# Begin Source File

SOURCE=.\CJControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJDateTimeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJDockBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJDockContext.cpp
# End Source File
# Begin Source File

SOURCE=.\CJExplorerBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatSplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFlatTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFontCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\CJFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJHtmlView.cpp
# End Source File
# Begin Source File

SOURCE=.\CJHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\CJListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJListView.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMaskEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMDIChildWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMDIFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMetaFileButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CJMiniDockFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CJOutlookBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJPagerCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJReBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJReBarCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJShell.cpp
# End Source File
# Begin Source File

SOURCE=.\CJShellList.cpp
# End Source File
# Begin Source File

SOURCE=.\CJShellTree.cpp
# End Source File
# Begin Source File

SOURCE=.\CJSizeDockBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJSortClass.cpp
# End Source File
# Begin Source File

SOURCE=.\CJTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJTabCtrlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJTabView.cpp
# End Source File
# Begin Source File

SOURCE=.\CJToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CJToolBarBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CJToolBarCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\CJWindowPlacement.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxGroupEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxOutBarCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GfxSplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Subclass.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Include\CJBrowseEdit.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJCaption.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJCaptionPopupWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJColorPicker.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJColorPopup.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJComboBoxEx.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJControlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJDateTimeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJDockBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJDockContext.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJExplorerBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatButton.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatComboBox.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatSplitterWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFlatTabCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFontCombo.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJHtmlView.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJHyperLink.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJLibrary.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJListView.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMaskEdit.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMDIChildWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMDIFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMenu.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMenuBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMetaFileButton.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJMiniDockFrameWnd.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJOutlookBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJPagerCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJReBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJReBarCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJShell.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJShellList.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJShellTree.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJSizeDockBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJSortClass.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJTabCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJTabCtrlBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJTabView.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJToolBar.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJToolBarBase.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJToolBarCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\CJWindowPlacement.h
# End Source File
# Begin Source File

SOURCE=..\Include\GfxGroupEdit.h
# End Source File
# Begin Source File

SOURCE=..\Include\GfxOutBarCtrl.h
# End Source File
# Begin Source File

SOURCE=..\Include\GfxSplitterWnd.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Include\Subclass.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
