# Microsoft Developer Studio Project File - Name="pseudo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pseudo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pseudo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pseudo.mak" CFG="pseudo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pseudo - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pseudo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pseudo - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\Applications\VideoShop\Application\WinBuild\CJLib\Include" /I "." /I "../../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "pseudo - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\Applications\VideoShop\Application\WinBuild\CJLib\Include" /I "." /I "../../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "pseudo - Win32 Release"
# Name "pseudo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PseudoButton.cpp
# End Source File
# Begin Source File

SOURCE=.\PseudoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PseudoDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\PseudoList.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\uconsole.cpp
# End Source File
# Begin Source File

SOURCE=..\..\udraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\uevent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\umenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\uobject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ustr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\utoolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\uwidget.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dib.h
# End Source File
# Begin Source File

SOURCE=.\PseudoButton.h
# End Source File
# Begin Source File

SOURCE=.\PseudoDlg.h
# End Source File
# Begin Source File

SOURCE=.\PseudoDraw.h
# End Source File
# Begin Source File

SOURCE=.\PseudoList.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\uconsole.h
# End Source File
# Begin Source File

SOURCE=..\..\udraw.h
# End Source File
# Begin Source File

SOURCE=..\..\umain.h
# End Source File
# Begin Source File

SOURCE=..\..\umenu.h
# End Source File
# Begin Source File

SOURCE=..\..\uobject.h
# End Source File
# Begin Source File

SOURCE=..\..\ustr.h
# End Source File
# Begin Source File

SOURCE=..\..\utoolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\uwidget.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
