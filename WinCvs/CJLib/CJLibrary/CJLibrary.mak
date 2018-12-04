# Microsoft Developer Studio Generated NMAKE File, Based on CJLibrary.dsp
!IF "$(CFG)" == ""
CFG=CJLibrary - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CJLibrary - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CJLibrary - Win32 Release" && "$(CFG)" != "CJLibrary - Win32 Debug" && "$(CFG)" != "CJLibrary - Win32 Unicode Release" && "$(CFG)" != "CJLibrary - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CJLibrary.mak" CFG="CJLibrary - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CJLibrary - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CJLibrary - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CJLibrary - Win32 Unicode Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CJLibrary - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "CJLibrary - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Lib\CJ609Lib.dll"


CLEAN :
	-@erase "$(INTDIR)\CJBrowseEdit.obj"
	-@erase "$(INTDIR)\CJCaption.obj"
	-@erase "$(INTDIR)\CJCaptionPopupWnd.obj"
	-@erase "$(INTDIR)\CJColorPicker.obj"
	-@erase "$(INTDIR)\CJColorPopup.obj"
	-@erase "$(INTDIR)\CJComboBoxEx.obj"
	-@erase "$(INTDIR)\CJControlBar.obj"
	-@erase "$(INTDIR)\CJDateTimeCtrl.obj"
	-@erase "$(INTDIR)\CJDockBar.obj"
	-@erase "$(INTDIR)\CJDockContext.obj"
	-@erase "$(INTDIR)\CJExplorerBar.obj"
	-@erase "$(INTDIR)\CJFlatButton.obj"
	-@erase "$(INTDIR)\CJFlatComboBox.obj"
	-@erase "$(INTDIR)\CJFlatHeaderCtrl.obj"
	-@erase "$(INTDIR)\CJFlatSplitterWnd.obj"
	-@erase "$(INTDIR)\CJFlatTabCtrl.obj"
	-@erase "$(INTDIR)\CJFontCombo.obj"
	-@erase "$(INTDIR)\CJFrameWnd.obj"
	-@erase "$(INTDIR)\CJHexEdit.obj"
	-@erase "$(INTDIR)\CJHtmlView.obj"
	-@erase "$(INTDIR)\CJHyperLink.obj"
	-@erase "$(INTDIR)\CJLibrary.obj"
	-@erase "$(INTDIR)\CJLibrary.pch"
	-@erase "$(INTDIR)\CJLibrary.res"
	-@erase "$(INTDIR)\CJListBox.obj"
	-@erase "$(INTDIR)\CJListCtrl.obj"
	-@erase "$(INTDIR)\CJListView.obj"
	-@erase "$(INTDIR)\CJLogoPane.obj"
	-@erase "$(INTDIR)\CJMaskEdit.obj"
	-@erase "$(INTDIR)\CJMDIChildWnd.obj"
	-@erase "$(INTDIR)\CJMDIFrameWnd.obj"
	-@erase "$(INTDIR)\CJMemFile.obj"
	-@erase "$(INTDIR)\CJMenu.obj"
	-@erase "$(INTDIR)\CJMenuBar.obj"
	-@erase "$(INTDIR)\CJMetaFileButton.obj"
	-@erase "$(INTDIR)\CJMiniDockFrameWnd.obj"
	-@erase "$(INTDIR)\CJOutlookBar.obj"
	-@erase "$(INTDIR)\CJPagerCtrl.obj"
	-@erase "$(INTDIR)\CJReBar.obj"
	-@erase "$(INTDIR)\CJReBarCtrl.obj"
	-@erase "$(INTDIR)\CJShell.obj"
	-@erase "$(INTDIR)\CJShellList.obj"
	-@erase "$(INTDIR)\CJShellTree.obj"
	-@erase "$(INTDIR)\CJSizeDockBar.obj"
	-@erase "$(INTDIR)\CJSortClass.obj"
	-@erase "$(INTDIR)\CJStatusBar.obj"
	-@erase "$(INTDIR)\CJTabCtrl.obj"
	-@erase "$(INTDIR)\CJTabCtrlBar.obj"
	-@erase "$(INTDIR)\CJTabView.obj"
	-@erase "$(INTDIR)\CJToolBar.obj"
	-@erase "$(INTDIR)\CJToolBarBase.obj"
	-@erase "$(INTDIR)\CJToolBarCtrl.obj"
	-@erase "$(INTDIR)\CJTreeCtrl.obj"
	-@erase "$(INTDIR)\CJWindowPlacement.obj"
	-@erase "$(INTDIR)\GfxGroupEdit.obj"
	-@erase "$(INTDIR)\GfxOutBarCtrl.obj"
	-@erase "$(INTDIR)\GfxSplitterWnd.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Subclass.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\Lib\CJ609Lib.dll"
	-@erase "..\Lib\CJ609Lib.exp"
	-@erase "..\Lib\CJ609Lib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W4 /GX /O2 /I "../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /Fp"$(INTDIR)\CJLibrary.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CJLibrary.res" /i "../Include" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CJLibrary.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=shlwapi.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\CJ609Lib.pdb" /machine:I386 /def:".\CJLibrary.def" /out:"../Lib/CJ609Lib.dll" /implib:"../Lib/CJ609Lib.lib" 
DEF_FILE= \
	".\CJLibrary.def"
LINK32_OBJS= \
	"$(INTDIR)\CJBrowseEdit.obj" \
	"$(INTDIR)\CJCaption.obj" \
	"$(INTDIR)\CJCaptionPopupWnd.obj" \
	"$(INTDIR)\CJColorPicker.obj" \
	"$(INTDIR)\CJColorPopup.obj" \
	"$(INTDIR)\CJComboBoxEx.obj" \
	"$(INTDIR)\CJControlBar.obj" \
	"$(INTDIR)\CJDateTimeCtrl.obj" \
	"$(INTDIR)\CJDockBar.obj" \
	"$(INTDIR)\CJDockContext.obj" \
	"$(INTDIR)\CJExplorerBar.obj" \
	"$(INTDIR)\CJFlatButton.obj" \
	"$(INTDIR)\CJFlatComboBox.obj" \
	"$(INTDIR)\CJFlatHeaderCtrl.obj" \
	"$(INTDIR)\CJFlatSplitterWnd.obj" \
	"$(INTDIR)\CJFlatTabCtrl.obj" \
	"$(INTDIR)\CJFontCombo.obj" \
	"$(INTDIR)\CJFrameWnd.obj" \
	"$(INTDIR)\CJHexEdit.obj" \
	"$(INTDIR)\CJHtmlView.obj" \
	"$(INTDIR)\CJHyperLink.obj" \
	"$(INTDIR)\CJLibrary.obj" \
	"$(INTDIR)\CJListBox.obj" \
	"$(INTDIR)\CJListCtrl.obj" \
	"$(INTDIR)\CJListView.obj" \
	"$(INTDIR)\CJLogoPane.obj" \
	"$(INTDIR)\CJMaskEdit.obj" \
	"$(INTDIR)\CJMDIChildWnd.obj" \
	"$(INTDIR)\CJMDIFrameWnd.obj" \
	"$(INTDIR)\CJMemFile.obj" \
	"$(INTDIR)\CJMenu.obj" \
	"$(INTDIR)\CJMenuBar.obj" \
	"$(INTDIR)\CJMetaFileButton.obj" \
	"$(INTDIR)\CJMiniDockFrameWnd.obj" \
	"$(INTDIR)\CJOutlookBar.obj" \
	"$(INTDIR)\CJPagerCtrl.obj" \
	"$(INTDIR)\CJReBar.obj" \
	"$(INTDIR)\CJReBarCtrl.obj" \
	"$(INTDIR)\CJShell.obj" \
	"$(INTDIR)\CJShellList.obj" \
	"$(INTDIR)\CJShellTree.obj" \
	"$(INTDIR)\CJSizeDockBar.obj" \
	"$(INTDIR)\CJSortClass.obj" \
	"$(INTDIR)\CJStatusBar.obj" \
	"$(INTDIR)\CJTabCtrl.obj" \
	"$(INTDIR)\CJTabCtrlBar.obj" \
	"$(INTDIR)\CJTabView.obj" \
	"$(INTDIR)\CJToolBar.obj" \
	"$(INTDIR)\CJToolBarBase.obj" \
	"$(INTDIR)\CJToolBarCtrl.obj" \
	"$(INTDIR)\CJTreeCtrl.obj" \
	"$(INTDIR)\CJWindowPlacement.obj" \
	"$(INTDIR)\GfxGroupEdit.obj" \
	"$(INTDIR)\GfxOutBarCtrl.obj" \
	"$(INTDIR)\GfxSplitterWnd.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Subclass.obj" \
	"$(INTDIR)\CJLibrary.res"

"..\Lib\CJ609Lib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\Lib\CJ609Libd.dll" "$(OUTDIR)\CJLibrary.bsc"


CLEAN :
	-@erase "$(INTDIR)\CJBrowseEdit.obj"
	-@erase "$(INTDIR)\CJBrowseEdit.sbr"
	-@erase "$(INTDIR)\CJCaption.obj"
	-@erase "$(INTDIR)\CJCaption.sbr"
	-@erase "$(INTDIR)\CJCaptionPopupWnd.obj"
	-@erase "$(INTDIR)\CJCaptionPopupWnd.sbr"
	-@erase "$(INTDIR)\CJColorPicker.obj"
	-@erase "$(INTDIR)\CJColorPicker.sbr"
	-@erase "$(INTDIR)\CJColorPopup.obj"
	-@erase "$(INTDIR)\CJColorPopup.sbr"
	-@erase "$(INTDIR)\CJComboBoxEx.obj"
	-@erase "$(INTDIR)\CJComboBoxEx.sbr"
	-@erase "$(INTDIR)\CJControlBar.obj"
	-@erase "$(INTDIR)\CJControlBar.sbr"
	-@erase "$(INTDIR)\CJDateTimeCtrl.obj"
	-@erase "$(INTDIR)\CJDateTimeCtrl.sbr"
	-@erase "$(INTDIR)\CJDockBar.obj"
	-@erase "$(INTDIR)\CJDockBar.sbr"
	-@erase "$(INTDIR)\CJDockContext.obj"
	-@erase "$(INTDIR)\CJDockContext.sbr"
	-@erase "$(INTDIR)\CJExplorerBar.obj"
	-@erase "$(INTDIR)\CJExplorerBar.sbr"
	-@erase "$(INTDIR)\CJFlatButton.obj"
	-@erase "$(INTDIR)\CJFlatButton.sbr"
	-@erase "$(INTDIR)\CJFlatComboBox.obj"
	-@erase "$(INTDIR)\CJFlatComboBox.sbr"
	-@erase "$(INTDIR)\CJFlatHeaderCtrl.obj"
	-@erase "$(INTDIR)\CJFlatHeaderCtrl.sbr"
	-@erase "$(INTDIR)\CJFlatSplitterWnd.obj"
	-@erase "$(INTDIR)\CJFlatSplitterWnd.sbr"
	-@erase "$(INTDIR)\CJFlatTabCtrl.obj"
	-@erase "$(INTDIR)\CJFlatTabCtrl.sbr"
	-@erase "$(INTDIR)\CJFontCombo.obj"
	-@erase "$(INTDIR)\CJFontCombo.sbr"
	-@erase "$(INTDIR)\CJFrameWnd.obj"
	-@erase "$(INTDIR)\CJFrameWnd.sbr"
	-@erase "$(INTDIR)\CJHexEdit.obj"
	-@erase "$(INTDIR)\CJHexEdit.sbr"
	-@erase "$(INTDIR)\CJHtmlView.obj"
	-@erase "$(INTDIR)\CJHtmlView.sbr"
	-@erase "$(INTDIR)\CJHyperLink.obj"
	-@erase "$(INTDIR)\CJHyperLink.sbr"
	-@erase "$(INTDIR)\CJLibrary.obj"
	-@erase "$(INTDIR)\CJLibrary.pch"
	-@erase "$(INTDIR)\CJLibrary.res"
	-@erase "$(INTDIR)\CJLibrary.sbr"
	-@erase "$(INTDIR)\CJListBox.obj"
	-@erase "$(INTDIR)\CJListBox.sbr"
	-@erase "$(INTDIR)\CJListCtrl.obj"
	-@erase "$(INTDIR)\CJListCtrl.sbr"
	-@erase "$(INTDIR)\CJListView.obj"
	-@erase "$(INTDIR)\CJListView.sbr"
	-@erase "$(INTDIR)\CJLogoPane.obj"
	-@erase "$(INTDIR)\CJLogoPane.sbr"
	-@erase "$(INTDIR)\CJMaskEdit.obj"
	-@erase "$(INTDIR)\CJMaskEdit.sbr"
	-@erase "$(INTDIR)\CJMDIChildWnd.obj"
	-@erase "$(INTDIR)\CJMDIChildWnd.sbr"
	-@erase "$(INTDIR)\CJMDIFrameWnd.obj"
	-@erase "$(INTDIR)\CJMDIFrameWnd.sbr"
	-@erase "$(INTDIR)\CJMemFile.obj"
	-@erase "$(INTDIR)\CJMemFile.sbr"
	-@erase "$(INTDIR)\CJMenu.obj"
	-@erase "$(INTDIR)\CJMenu.sbr"
	-@erase "$(INTDIR)\CJMenuBar.obj"
	-@erase "$(INTDIR)\CJMenuBar.sbr"
	-@erase "$(INTDIR)\CJMetaFileButton.obj"
	-@erase "$(INTDIR)\CJMetaFileButton.sbr"
	-@erase "$(INTDIR)\CJMiniDockFrameWnd.obj"
	-@erase "$(INTDIR)\CJMiniDockFrameWnd.sbr"
	-@erase "$(INTDIR)\CJOutlookBar.obj"
	-@erase "$(INTDIR)\CJOutlookBar.sbr"
	-@erase "$(INTDIR)\CJPagerCtrl.obj"
	-@erase "$(INTDIR)\CJPagerCtrl.sbr"
	-@erase "$(INTDIR)\CJReBar.obj"
	-@erase "$(INTDIR)\CJReBar.sbr"
	-@erase "$(INTDIR)\CJReBarCtrl.obj"
	-@erase "$(INTDIR)\CJReBarCtrl.sbr"
	-@erase "$(INTDIR)\CJShell.obj"
	-@erase "$(INTDIR)\CJShell.sbr"
	-@erase "$(INTDIR)\CJShellList.obj"
	-@erase "$(INTDIR)\CJShellList.sbr"
	-@erase "$(INTDIR)\CJShellTree.obj"
	-@erase "$(INTDIR)\CJShellTree.sbr"
	-@erase "$(INTDIR)\CJSizeDockBar.obj"
	-@erase "$(INTDIR)\CJSizeDockBar.sbr"
	-@erase "$(INTDIR)\CJSortClass.obj"
	-@erase "$(INTDIR)\CJSortClass.sbr"
	-@erase "$(INTDIR)\CJStatusBar.obj"
	-@erase "$(INTDIR)\CJStatusBar.sbr"
	-@erase "$(INTDIR)\CJTabCtrl.obj"
	-@erase "$(INTDIR)\CJTabCtrl.sbr"
	-@erase "$(INTDIR)\CJTabCtrlBar.obj"
	-@erase "$(INTDIR)\CJTabCtrlBar.sbr"
	-@erase "$(INTDIR)\CJTabView.obj"
	-@erase "$(INTDIR)\CJTabView.sbr"
	-@erase "$(INTDIR)\CJToolBar.obj"
	-@erase "$(INTDIR)\CJToolBar.sbr"
	-@erase "$(INTDIR)\CJToolBarBase.obj"
	-@erase "$(INTDIR)\CJToolBarBase.sbr"
	-@erase "$(INTDIR)\CJToolBarCtrl.obj"
	-@erase "$(INTDIR)\CJToolBarCtrl.sbr"
	-@erase "$(INTDIR)\CJTreeCtrl.obj"
	-@erase "$(INTDIR)\CJTreeCtrl.sbr"
	-@erase "$(INTDIR)\CJWindowPlacement.obj"
	-@erase "$(INTDIR)\CJWindowPlacement.sbr"
	-@erase "$(INTDIR)\GfxGroupEdit.obj"
	-@erase "$(INTDIR)\GfxGroupEdit.sbr"
	-@erase "$(INTDIR)\GfxOutBarCtrl.obj"
	-@erase "$(INTDIR)\GfxOutBarCtrl.sbr"
	-@erase "$(INTDIR)\GfxSplitterWnd.obj"
	-@erase "$(INTDIR)\GfxSplitterWnd.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Subclass.obj"
	-@erase "$(INTDIR)\Subclass.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CJ609Libd.map"
	-@erase "$(OUTDIR)\CJ609Libd.pdb"
	-@erase "$(OUTDIR)\CJLibrary.bsc"
	-@erase "..\Lib\CJ609Libd.dll"
	-@erase "..\Lib\CJ609Libd.exp"
	-@erase "..\Lib\CJ609Libd.ilk"
	-@erase "..\Lib\CJ609Libd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CJLibrary.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CJLibrary.res" /i "../Include" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CJLibrary.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CJBrowseEdit.sbr" \
	"$(INTDIR)\CJCaption.sbr" \
	"$(INTDIR)\CJCaptionPopupWnd.sbr" \
	"$(INTDIR)\CJColorPicker.sbr" \
	"$(INTDIR)\CJColorPopup.sbr" \
	"$(INTDIR)\CJComboBoxEx.sbr" \
	"$(INTDIR)\CJControlBar.sbr" \
	"$(INTDIR)\CJDateTimeCtrl.sbr" \
	"$(INTDIR)\CJDockBar.sbr" \
	"$(INTDIR)\CJDockContext.sbr" \
	"$(INTDIR)\CJExplorerBar.sbr" \
	"$(INTDIR)\CJFlatButton.sbr" \
	"$(INTDIR)\CJFlatComboBox.sbr" \
	"$(INTDIR)\CJFlatHeaderCtrl.sbr" \
	"$(INTDIR)\CJFlatSplitterWnd.sbr" \
	"$(INTDIR)\CJFlatTabCtrl.sbr" \
	"$(INTDIR)\CJFontCombo.sbr" \
	"$(INTDIR)\CJFrameWnd.sbr" \
	"$(INTDIR)\CJHexEdit.sbr" \
	"$(INTDIR)\CJHtmlView.sbr" \
	"$(INTDIR)\CJHyperLink.sbr" \
	"$(INTDIR)\CJLibrary.sbr" \
	"$(INTDIR)\CJListBox.sbr" \
	"$(INTDIR)\CJListCtrl.sbr" \
	"$(INTDIR)\CJListView.sbr" \
	"$(INTDIR)\CJLogoPane.sbr" \
	"$(INTDIR)\CJMaskEdit.sbr" \
	"$(INTDIR)\CJMDIChildWnd.sbr" \
	"$(INTDIR)\CJMDIFrameWnd.sbr" \
	"$(INTDIR)\CJMemFile.sbr" \
	"$(INTDIR)\CJMenu.sbr" \
	"$(INTDIR)\CJMenuBar.sbr" \
	"$(INTDIR)\CJMetaFileButton.sbr" \
	"$(INTDIR)\CJMiniDockFrameWnd.sbr" \
	"$(INTDIR)\CJOutlookBar.sbr" \
	"$(INTDIR)\CJPagerCtrl.sbr" \
	"$(INTDIR)\CJReBar.sbr" \
	"$(INTDIR)\CJReBarCtrl.sbr" \
	"$(INTDIR)\CJShell.sbr" \
	"$(INTDIR)\CJShellList.sbr" \
	"$(INTDIR)\CJShellTree.sbr" \
	"$(INTDIR)\CJSizeDockBar.sbr" \
	"$(INTDIR)\CJSortClass.sbr" \
	"$(INTDIR)\CJStatusBar.sbr" \
	"$(INTDIR)\CJTabCtrl.sbr" \
	"$(INTDIR)\CJTabCtrlBar.sbr" \
	"$(INTDIR)\CJTabView.sbr" \
	"$(INTDIR)\CJToolBar.sbr" \
	"$(INTDIR)\CJToolBarBase.sbr" \
	"$(INTDIR)\CJToolBarCtrl.sbr" \
	"$(INTDIR)\CJTreeCtrl.sbr" \
	"$(INTDIR)\CJWindowPlacement.sbr" \
	"$(INTDIR)\GfxGroupEdit.sbr" \
	"$(INTDIR)\GfxOutBarCtrl.sbr" \
	"$(INTDIR)\GfxSplitterWnd.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\Subclass.sbr"

"$(OUTDIR)\CJLibrary.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=shlwapi.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\CJ609Libd.pdb" /map:"$(INTDIR)\CJ609Libd.map" /debug /machine:I386 /def:".\CJLibrary.def" /out:"../Lib/CJ609Libd.dll" /implib:"../Lib/CJ609Libd.lib" 
DEF_FILE= \
	".\CJLibrary.def"
LINK32_OBJS= \
	"$(INTDIR)\CJBrowseEdit.obj" \
	"$(INTDIR)\CJCaption.obj" \
	"$(INTDIR)\CJCaptionPopupWnd.obj" \
	"$(INTDIR)\CJColorPicker.obj" \
	"$(INTDIR)\CJColorPopup.obj" \
	"$(INTDIR)\CJComboBoxEx.obj" \
	"$(INTDIR)\CJControlBar.obj" \
	"$(INTDIR)\CJDateTimeCtrl.obj" \
	"$(INTDIR)\CJDockBar.obj" \
	"$(INTDIR)\CJDockContext.obj" \
	"$(INTDIR)\CJExplorerBar.obj" \
	"$(INTDIR)\CJFlatButton.obj" \
	"$(INTDIR)\CJFlatComboBox.obj" \
	"$(INTDIR)\CJFlatHeaderCtrl.obj" \
	"$(INTDIR)\CJFlatSplitterWnd.obj" \
	"$(INTDIR)\CJFlatTabCtrl.obj" \
	"$(INTDIR)\CJFontCombo.obj" \
	"$(INTDIR)\CJFrameWnd.obj" \
	"$(INTDIR)\CJHexEdit.obj" \
	"$(INTDIR)\CJHtmlView.obj" \
	"$(INTDIR)\CJHyperLink.obj" \
	"$(INTDIR)\CJLibrary.obj" \
	"$(INTDIR)\CJListBox.obj" \
	"$(INTDIR)\CJListCtrl.obj" \
	"$(INTDIR)\CJListView.obj" \
	"$(INTDIR)\CJLogoPane.obj" \
	"$(INTDIR)\CJMaskEdit.obj" \
	"$(INTDIR)\CJMDIChildWnd.obj" \
	"$(INTDIR)\CJMDIFrameWnd.obj" \
	"$(INTDIR)\CJMemFile.obj" \
	"$(INTDIR)\CJMenu.obj" \
	"$(INTDIR)\CJMenuBar.obj" \
	"$(INTDIR)\CJMetaFileButton.obj" \
	"$(INTDIR)\CJMiniDockFrameWnd.obj" \
	"$(INTDIR)\CJOutlookBar.obj" \
	"$(INTDIR)\CJPagerCtrl.obj" \
	"$(INTDIR)\CJReBar.obj" \
	"$(INTDIR)\CJReBarCtrl.obj" \
	"$(INTDIR)\CJShell.obj" \
	"$(INTDIR)\CJShellList.obj" \
	"$(INTDIR)\CJShellTree.obj" \
	"$(INTDIR)\CJSizeDockBar.obj" \
	"$(INTDIR)\CJSortClass.obj" \
	"$(INTDIR)\CJStatusBar.obj" \
	"$(INTDIR)\CJTabCtrl.obj" \
	"$(INTDIR)\CJTabCtrlBar.obj" \
	"$(INTDIR)\CJTabView.obj" \
	"$(INTDIR)\CJToolBar.obj" \
	"$(INTDIR)\CJToolBarBase.obj" \
	"$(INTDIR)\CJToolBarCtrl.obj" \
	"$(INTDIR)\CJTreeCtrl.obj" \
	"$(INTDIR)\CJWindowPlacement.obj" \
	"$(INTDIR)\GfxGroupEdit.obj" \
	"$(INTDIR)\GfxOutBarCtrl.obj" \
	"$(INTDIR)\GfxSplitterWnd.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Subclass.obj" \
	"$(INTDIR)\CJLibrary.res"

"..\Lib\CJ609Libd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"

OUTDIR=.\Release_Unicode
INTDIR=.\Release_Unicode

ALL : "..\Lib\CJ609Libu.dll"


CLEAN :
	-@erase "$(INTDIR)\CJBrowseEdit.obj"
	-@erase "$(INTDIR)\CJCaption.obj"
	-@erase "$(INTDIR)\CJCaptionPopupWnd.obj"
	-@erase "$(INTDIR)\CJColorPicker.obj"
	-@erase "$(INTDIR)\CJColorPopup.obj"
	-@erase "$(INTDIR)\CJComboBoxEx.obj"
	-@erase "$(INTDIR)\CJControlBar.obj"
	-@erase "$(INTDIR)\CJDateTimeCtrl.obj"
	-@erase "$(INTDIR)\CJDockBar.obj"
	-@erase "$(INTDIR)\CJDockContext.obj"
	-@erase "$(INTDIR)\CJExplorerBar.obj"
	-@erase "$(INTDIR)\CJFlatButton.obj"
	-@erase "$(INTDIR)\CJFlatComboBox.obj"
	-@erase "$(INTDIR)\CJFlatHeaderCtrl.obj"
	-@erase "$(INTDIR)\CJFlatSplitterWnd.obj"
	-@erase "$(INTDIR)\CJFlatTabCtrl.obj"
	-@erase "$(INTDIR)\CJFontCombo.obj"
	-@erase "$(INTDIR)\CJFrameWnd.obj"
	-@erase "$(INTDIR)\CJHexEdit.obj"
	-@erase "$(INTDIR)\CJHtmlView.obj"
	-@erase "$(INTDIR)\CJHyperLink.obj"
	-@erase "$(INTDIR)\CJLibrary.obj"
	-@erase "$(INTDIR)\CJLibrary.pch"
	-@erase "$(INTDIR)\CJLibrary.res"
	-@erase "$(INTDIR)\CJListBox.obj"
	-@erase "$(INTDIR)\CJListCtrl.obj"
	-@erase "$(INTDIR)\CJListView.obj"
	-@erase "$(INTDIR)\CJLogoPane.obj"
	-@erase "$(INTDIR)\CJMaskEdit.obj"
	-@erase "$(INTDIR)\CJMDIChildWnd.obj"
	-@erase "$(INTDIR)\CJMDIFrameWnd.obj"
	-@erase "$(INTDIR)\CJMemFile.obj"
	-@erase "$(INTDIR)\CJMenu.obj"
	-@erase "$(INTDIR)\CJMenuBar.obj"
	-@erase "$(INTDIR)\CJMetaFileButton.obj"
	-@erase "$(INTDIR)\CJMiniDockFrameWnd.obj"
	-@erase "$(INTDIR)\CJOutlookBar.obj"
	-@erase "$(INTDIR)\CJPagerCtrl.obj"
	-@erase "$(INTDIR)\CJReBar.obj"
	-@erase "$(INTDIR)\CJReBarCtrl.obj"
	-@erase "$(INTDIR)\CJShell.obj"
	-@erase "$(INTDIR)\CJShellList.obj"
	-@erase "$(INTDIR)\CJShellTree.obj"
	-@erase "$(INTDIR)\CJSizeDockBar.obj"
	-@erase "$(INTDIR)\CJSortClass.obj"
	-@erase "$(INTDIR)\CJStatusBar.obj"
	-@erase "$(INTDIR)\CJTabCtrl.obj"
	-@erase "$(INTDIR)\CJTabCtrlBar.obj"
	-@erase "$(INTDIR)\CJTabView.obj"
	-@erase "$(INTDIR)\CJToolBar.obj"
	-@erase "$(INTDIR)\CJToolBarBase.obj"
	-@erase "$(INTDIR)\CJToolBarCtrl.obj"
	-@erase "$(INTDIR)\CJTreeCtrl.obj"
	-@erase "$(INTDIR)\CJWindowPlacement.obj"
	-@erase "$(INTDIR)\GfxGroupEdit.obj"
	-@erase "$(INTDIR)\GfxOutBarCtrl.obj"
	-@erase "$(INTDIR)\GfxSplitterWnd.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Subclass.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\Lib\CJ609Libu.dll"
	-@erase "..\Lib\CJ609Libu.exp"
	-@erase "..\Lib\CJ609Libu.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_UNICODE" /Fp"$(INTDIR)\CJLibrary.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CJLibrary.res" /i "../Include" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CJLibrary.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=shlwapi.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\CJ609Libu.pdb" /machine:I386 /def:".\CJLibrary.def" /out:"../Lib/CJ609Libu.dll" /implib:"../Lib/CJ609Libu.lib" 
DEF_FILE= \
	".\CJLibrary.def"
LINK32_OBJS= \
	"$(INTDIR)\CJBrowseEdit.obj" \
	"$(INTDIR)\CJCaption.obj" \
	"$(INTDIR)\CJCaptionPopupWnd.obj" \
	"$(INTDIR)\CJColorPicker.obj" \
	"$(INTDIR)\CJColorPopup.obj" \
	"$(INTDIR)\CJComboBoxEx.obj" \
	"$(INTDIR)\CJControlBar.obj" \
	"$(INTDIR)\CJDateTimeCtrl.obj" \
	"$(INTDIR)\CJDockBar.obj" \
	"$(INTDIR)\CJDockContext.obj" \
	"$(INTDIR)\CJExplorerBar.obj" \
	"$(INTDIR)\CJFlatButton.obj" \
	"$(INTDIR)\CJFlatComboBox.obj" \
	"$(INTDIR)\CJFlatHeaderCtrl.obj" \
	"$(INTDIR)\CJFlatSplitterWnd.obj" \
	"$(INTDIR)\CJFlatTabCtrl.obj" \
	"$(INTDIR)\CJFontCombo.obj" \
	"$(INTDIR)\CJFrameWnd.obj" \
	"$(INTDIR)\CJHexEdit.obj" \
	"$(INTDIR)\CJHtmlView.obj" \
	"$(INTDIR)\CJHyperLink.obj" \
	"$(INTDIR)\CJLibrary.obj" \
	"$(INTDIR)\CJListBox.obj" \
	"$(INTDIR)\CJListCtrl.obj" \
	"$(INTDIR)\CJListView.obj" \
	"$(INTDIR)\CJLogoPane.obj" \
	"$(INTDIR)\CJMaskEdit.obj" \
	"$(INTDIR)\CJMDIChildWnd.obj" \
	"$(INTDIR)\CJMDIFrameWnd.obj" \
	"$(INTDIR)\CJMemFile.obj" \
	"$(INTDIR)\CJMenu.obj" \
	"$(INTDIR)\CJMenuBar.obj" \
	"$(INTDIR)\CJMetaFileButton.obj" \
	"$(INTDIR)\CJMiniDockFrameWnd.obj" \
	"$(INTDIR)\CJOutlookBar.obj" \
	"$(INTDIR)\CJPagerCtrl.obj" \
	"$(INTDIR)\CJReBar.obj" \
	"$(INTDIR)\CJReBarCtrl.obj" \
	"$(INTDIR)\CJShell.obj" \
	"$(INTDIR)\CJShellList.obj" \
	"$(INTDIR)\CJShellTree.obj" \
	"$(INTDIR)\CJSizeDockBar.obj" \
	"$(INTDIR)\CJSortClass.obj" \
	"$(INTDIR)\CJStatusBar.obj" \
	"$(INTDIR)\CJTabCtrl.obj" \
	"$(INTDIR)\CJTabCtrlBar.obj" \
	"$(INTDIR)\CJTabView.obj" \
	"$(INTDIR)\CJToolBar.obj" \
	"$(INTDIR)\CJToolBarBase.obj" \
	"$(INTDIR)\CJToolBarCtrl.obj" \
	"$(INTDIR)\CJTreeCtrl.obj" \
	"$(INTDIR)\CJWindowPlacement.obj" \
	"$(INTDIR)\GfxGroupEdit.obj" \
	"$(INTDIR)\GfxOutBarCtrl.obj" \
	"$(INTDIR)\GfxSplitterWnd.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Subclass.obj" \
	"$(INTDIR)\CJLibrary.res"

"..\Lib\CJ609Libu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"

OUTDIR=.\Debug_Unicode
INTDIR=.\Debug_Unicode

ALL : "..\Lib\CJ609Libud.dll"


CLEAN :
	-@erase "$(INTDIR)\CJBrowseEdit.obj"
	-@erase "$(INTDIR)\CJCaption.obj"
	-@erase "$(INTDIR)\CJCaptionPopupWnd.obj"
	-@erase "$(INTDIR)\CJColorPicker.obj"
	-@erase "$(INTDIR)\CJColorPopup.obj"
	-@erase "$(INTDIR)\CJComboBoxEx.obj"
	-@erase "$(INTDIR)\CJControlBar.obj"
	-@erase "$(INTDIR)\CJDateTimeCtrl.obj"
	-@erase "$(INTDIR)\CJDockBar.obj"
	-@erase "$(INTDIR)\CJDockContext.obj"
	-@erase "$(INTDIR)\CJExplorerBar.obj"
	-@erase "$(INTDIR)\CJFlatButton.obj"
	-@erase "$(INTDIR)\CJFlatComboBox.obj"
	-@erase "$(INTDIR)\CJFlatHeaderCtrl.obj"
	-@erase "$(INTDIR)\CJFlatSplitterWnd.obj"
	-@erase "$(INTDIR)\CJFlatTabCtrl.obj"
	-@erase "$(INTDIR)\CJFontCombo.obj"
	-@erase "$(INTDIR)\CJFrameWnd.obj"
	-@erase "$(INTDIR)\CJHexEdit.obj"
	-@erase "$(INTDIR)\CJHtmlView.obj"
	-@erase "$(INTDIR)\CJHyperLink.obj"
	-@erase "$(INTDIR)\CJLibrary.obj"
	-@erase "$(INTDIR)\CJLibrary.pch"
	-@erase "$(INTDIR)\CJLibrary.res"
	-@erase "$(INTDIR)\CJListBox.obj"
	-@erase "$(INTDIR)\CJListCtrl.obj"
	-@erase "$(INTDIR)\CJListView.obj"
	-@erase "$(INTDIR)\CJLogoPane.obj"
	-@erase "$(INTDIR)\CJMaskEdit.obj"
	-@erase "$(INTDIR)\CJMDIChildWnd.obj"
	-@erase "$(INTDIR)\CJMDIFrameWnd.obj"
	-@erase "$(INTDIR)\CJMemFile.obj"
	-@erase "$(INTDIR)\CJMenu.obj"
	-@erase "$(INTDIR)\CJMenuBar.obj"
	-@erase "$(INTDIR)\CJMetaFileButton.obj"
	-@erase "$(INTDIR)\CJMiniDockFrameWnd.obj"
	-@erase "$(INTDIR)\CJOutlookBar.obj"
	-@erase "$(INTDIR)\CJPagerCtrl.obj"
	-@erase "$(INTDIR)\CJReBar.obj"
	-@erase "$(INTDIR)\CJReBarCtrl.obj"
	-@erase "$(INTDIR)\CJShell.obj"
	-@erase "$(INTDIR)\CJShellList.obj"
	-@erase "$(INTDIR)\CJShellTree.obj"
	-@erase "$(INTDIR)\CJSizeDockBar.obj"
	-@erase "$(INTDIR)\CJSortClass.obj"
	-@erase "$(INTDIR)\CJStatusBar.obj"
	-@erase "$(INTDIR)\CJTabCtrl.obj"
	-@erase "$(INTDIR)\CJTabCtrlBar.obj"
	-@erase "$(INTDIR)\CJTabView.obj"
	-@erase "$(INTDIR)\CJToolBar.obj"
	-@erase "$(INTDIR)\CJToolBarBase.obj"
	-@erase "$(INTDIR)\CJToolBarCtrl.obj"
	-@erase "$(INTDIR)\CJTreeCtrl.obj"
	-@erase "$(INTDIR)\CJWindowPlacement.obj"
	-@erase "$(INTDIR)\GfxGroupEdit.obj"
	-@erase "$(INTDIR)\GfxOutBarCtrl.obj"
	-@erase "$(INTDIR)\GfxSplitterWnd.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Subclass.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CJ609Libud.map"
	-@erase "$(OUTDIR)\CJ609Libud.pdb"
	-@erase "..\Lib\CJ609Libud.dll"
	-@erase "..\Lib\CJ609Libud.exp"
	-@erase "..\Lib\CJ609Libud.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_UNICODE" /Fp"$(INTDIR)\CJLibrary.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CJLibrary.res" /i "../Include" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CJLibrary.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=shlwapi.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\CJ609Libud.pdb" /map:"$(INTDIR)\CJ609Libud.map" /debug /debugtype:both /machine:I386 /def:".\CJLibrary.def" /out:"../Lib/CJ609Libud.dll" /implib:"../Lib/CJ609Libud.lib" /pdbtype:sept 
DEF_FILE= \
	".\CJLibrary.def"
LINK32_OBJS= \
	"$(INTDIR)\CJBrowseEdit.obj" \
	"$(INTDIR)\CJCaption.obj" \
	"$(INTDIR)\CJCaptionPopupWnd.obj" \
	"$(INTDIR)\CJColorPicker.obj" \
	"$(INTDIR)\CJColorPopup.obj" \
	"$(INTDIR)\CJComboBoxEx.obj" \
	"$(INTDIR)\CJControlBar.obj" \
	"$(INTDIR)\CJDateTimeCtrl.obj" \
	"$(INTDIR)\CJDockBar.obj" \
	"$(INTDIR)\CJDockContext.obj" \
	"$(INTDIR)\CJExplorerBar.obj" \
	"$(INTDIR)\CJFlatButton.obj" \
	"$(INTDIR)\CJFlatComboBox.obj" \
	"$(INTDIR)\CJFlatHeaderCtrl.obj" \
	"$(INTDIR)\CJFlatSplitterWnd.obj" \
	"$(INTDIR)\CJFlatTabCtrl.obj" \
	"$(INTDIR)\CJFontCombo.obj" \
	"$(INTDIR)\CJFrameWnd.obj" \
	"$(INTDIR)\CJHexEdit.obj" \
	"$(INTDIR)\CJHtmlView.obj" \
	"$(INTDIR)\CJHyperLink.obj" \
	"$(INTDIR)\CJLibrary.obj" \
	"$(INTDIR)\CJListBox.obj" \
	"$(INTDIR)\CJListCtrl.obj" \
	"$(INTDIR)\CJListView.obj" \
	"$(INTDIR)\CJLogoPane.obj" \
	"$(INTDIR)\CJMaskEdit.obj" \
	"$(INTDIR)\CJMDIChildWnd.obj" \
	"$(INTDIR)\CJMDIFrameWnd.obj" \
	"$(INTDIR)\CJMemFile.obj" \
	"$(INTDIR)\CJMenu.obj" \
	"$(INTDIR)\CJMenuBar.obj" \
	"$(INTDIR)\CJMetaFileButton.obj" \
	"$(INTDIR)\CJMiniDockFrameWnd.obj" \
	"$(INTDIR)\CJOutlookBar.obj" \
	"$(INTDIR)\CJPagerCtrl.obj" \
	"$(INTDIR)\CJReBar.obj" \
	"$(INTDIR)\CJReBarCtrl.obj" \
	"$(INTDIR)\CJShell.obj" \
	"$(INTDIR)\CJShellList.obj" \
	"$(INTDIR)\CJShellTree.obj" \
	"$(INTDIR)\CJSizeDockBar.obj" \
	"$(INTDIR)\CJSortClass.obj" \
	"$(INTDIR)\CJStatusBar.obj" \
	"$(INTDIR)\CJTabCtrl.obj" \
	"$(INTDIR)\CJTabCtrlBar.obj" \
	"$(INTDIR)\CJTabView.obj" \
	"$(INTDIR)\CJToolBar.obj" \
	"$(INTDIR)\CJToolBarBase.obj" \
	"$(INTDIR)\CJToolBarCtrl.obj" \
	"$(INTDIR)\CJTreeCtrl.obj" \
	"$(INTDIR)\CJWindowPlacement.obj" \
	"$(INTDIR)\GfxGroupEdit.obj" \
	"$(INTDIR)\GfxOutBarCtrl.obj" \
	"$(INTDIR)\GfxSplitterWnd.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Subclass.obj" \
	"$(INTDIR)\CJLibrary.res"

"..\Lib\CJ609Libud.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("CJLibrary.dep")
!INCLUDE "CJLibrary.dep"
!ELSE 
!MESSAGE Warning: cannot find "CJLibrary.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CJLibrary - Win32 Release" || "$(CFG)" == "CJLibrary - Win32 Debug" || "$(CFG)" == "CJLibrary - Win32 Unicode Release" || "$(CFG)" == "CJLibrary - Win32 Unicode Debug"
SOURCE=.\CJBrowseEdit.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJBrowseEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJBrowseEdit.obj"	"$(INTDIR)\CJBrowseEdit.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJBrowseEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJBrowseEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJCaption.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJCaption.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJCaption.obj"	"$(INTDIR)\CJCaption.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJCaption.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJCaption.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJCaptionPopupWnd.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJCaptionPopupWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJCaptionPopupWnd.obj"	"$(INTDIR)\CJCaptionPopupWnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJCaptionPopupWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJCaptionPopupWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJColorPicker.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJColorPicker.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJColorPicker.obj"	"$(INTDIR)\CJColorPicker.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJColorPicker.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJColorPicker.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJColorPopup.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJColorPopup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJColorPopup.obj"	"$(INTDIR)\CJColorPopup.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJColorPopup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJColorPopup.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJComboBoxEx.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJComboBoxEx.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJComboBoxEx.obj"	"$(INTDIR)\CJComboBoxEx.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJComboBoxEx.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJComboBoxEx.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJControlBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJControlBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJControlBar.obj"	"$(INTDIR)\CJControlBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJControlBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJControlBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJDateTimeCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJDateTimeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJDateTimeCtrl.obj"	"$(INTDIR)\CJDateTimeCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJDateTimeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJDateTimeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJDockBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJDockBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJDockBar.obj"	"$(INTDIR)\CJDockBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJDockBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJDockBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJDockContext.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJDockContext.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJDockContext.obj"	"$(INTDIR)\CJDockContext.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJDockContext.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJDockContext.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJExplorerBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJExplorerBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJExplorerBar.obj"	"$(INTDIR)\CJExplorerBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJExplorerBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJExplorerBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJFlatButton.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJFlatButton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJFlatButton.obj"	"$(INTDIR)\CJFlatButton.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJFlatButton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJFlatButton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJFlatComboBox.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJFlatComboBox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJFlatComboBox.obj"	"$(INTDIR)\CJFlatComboBox.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJFlatComboBox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJFlatComboBox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJFlatHeaderCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJFlatHeaderCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJFlatHeaderCtrl.obj"	"$(INTDIR)\CJFlatHeaderCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJFlatHeaderCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJFlatHeaderCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJFlatSplitterWnd.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJFlatSplitterWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJFlatSplitterWnd.obj"	"$(INTDIR)\CJFlatSplitterWnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJFlatSplitterWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJFlatSplitterWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJFlatTabCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJFlatTabCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJFlatTabCtrl.obj"	"$(INTDIR)\CJFlatTabCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJFlatTabCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJFlatTabCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJFontCombo.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJFontCombo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJFontCombo.obj"	"$(INTDIR)\CJFontCombo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJFontCombo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJFontCombo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJFrameWnd.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJFrameWnd.obj"	"$(INTDIR)\CJFrameWnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJHexEdit.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJHexEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJHexEdit.obj"	"$(INTDIR)\CJHexEdit.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJHexEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJHexEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJHtmlView.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJHtmlView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJHtmlView.obj"	"$(INTDIR)\CJHtmlView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJHtmlView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJHtmlView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJHyperLink.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJHyperLink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJHyperLink.obj"	"$(INTDIR)\CJHyperLink.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJHyperLink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJHyperLink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJLibrary.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJLibrary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJLibrary.obj"	"$(INTDIR)\CJLibrary.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJLibrary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJLibrary.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJLibrary.rc

"$(INTDIR)\CJLibrary.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\CJListBox.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJListBox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJListBox.obj"	"$(INTDIR)\CJListBox.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJListBox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJListBox.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJListCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJListCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJListCtrl.obj"	"$(INTDIR)\CJListCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJListCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJListCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJListView.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJListView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJListView.obj"	"$(INTDIR)\CJListView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJListView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJListView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJLogoPane.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJLogoPane.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJLogoPane.obj"	"$(INTDIR)\CJLogoPane.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJLogoPane.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJLogoPane.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMaskEdit.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMaskEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMaskEdit.obj"	"$(INTDIR)\CJMaskEdit.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMaskEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMaskEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMDIChildWnd.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMDIChildWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMDIChildWnd.obj"	"$(INTDIR)\CJMDIChildWnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMDIChildWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMDIChildWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMDIFrameWnd.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMDIFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMDIFrameWnd.obj"	"$(INTDIR)\CJMDIFrameWnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMDIFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMDIFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMemFile.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMemFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMemFile.obj"	"$(INTDIR)\CJMemFile.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMemFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMemFile.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMenu.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMenu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMenu.obj"	"$(INTDIR)\CJMenu.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMenu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMenu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMenuBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMenuBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMenuBar.obj"	"$(INTDIR)\CJMenuBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMenuBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMenuBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMetaFileButton.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMetaFileButton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMetaFileButton.obj"	"$(INTDIR)\CJMetaFileButton.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMetaFileButton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMetaFileButton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJMiniDockFrameWnd.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJMiniDockFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJMiniDockFrameWnd.obj"	"$(INTDIR)\CJMiniDockFrameWnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJMiniDockFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJMiniDockFrameWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJOutlookBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJOutlookBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJOutlookBar.obj"	"$(INTDIR)\CJOutlookBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJOutlookBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJOutlookBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJPagerCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJPagerCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJPagerCtrl.obj"	"$(INTDIR)\CJPagerCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJPagerCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJPagerCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJReBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJReBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJReBar.obj"	"$(INTDIR)\CJReBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJReBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJReBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJReBarCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJReBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJReBarCtrl.obj"	"$(INTDIR)\CJReBarCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJReBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJReBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJShell.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJShell.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJShell.obj"	"$(INTDIR)\CJShell.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJShell.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJShell.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJShellList.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJShellList.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJShellList.obj"	"$(INTDIR)\CJShellList.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJShellList.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJShellList.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJShellTree.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJShellTree.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJShellTree.obj"	"$(INTDIR)\CJShellTree.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJShellTree.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJShellTree.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJSizeDockBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJSizeDockBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJSizeDockBar.obj"	"$(INTDIR)\CJSizeDockBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJSizeDockBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJSizeDockBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJSortClass.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJSortClass.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJSortClass.obj"	"$(INTDIR)\CJSortClass.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJSortClass.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJSortClass.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJStatusBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJStatusBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJStatusBar.obj"	"$(INTDIR)\CJStatusBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJStatusBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJStatusBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJTabCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJTabCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJTabCtrl.obj"	"$(INTDIR)\CJTabCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJTabCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJTabCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJTabCtrlBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJTabCtrlBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJTabCtrlBar.obj"	"$(INTDIR)\CJTabCtrlBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJTabCtrlBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJTabCtrlBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJTabView.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJTabView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJTabView.obj"	"$(INTDIR)\CJTabView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJTabView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJTabView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJToolBar.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJToolBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJToolBar.obj"	"$(INTDIR)\CJToolBar.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJToolBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJToolBar.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJToolBarBase.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJToolBarBase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJToolBarBase.obj"	"$(INTDIR)\CJToolBarBase.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJToolBarBase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJToolBarBase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJToolBarCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJToolBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJToolBarCtrl.obj"	"$(INTDIR)\CJToolBarCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJToolBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJToolBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJTreeCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJTreeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJTreeCtrl.obj"	"$(INTDIR)\CJTreeCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJTreeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJTreeCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\CJWindowPlacement.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\CJWindowPlacement.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\CJWindowPlacement.obj"	"$(INTDIR)\CJWindowPlacement.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\CJWindowPlacement.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\CJWindowPlacement.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\GfxGroupEdit.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\GfxGroupEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\GfxGroupEdit.obj"	"$(INTDIR)\GfxGroupEdit.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\GfxGroupEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\GfxGroupEdit.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\GfxOutBarCtrl.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\GfxOutBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\GfxOutBarCtrl.obj"	"$(INTDIR)\GfxOutBarCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\GfxOutBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\GfxOutBarCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\GfxSplitterWnd.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\GfxSplitterWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\GfxSplitterWnd.obj"	"$(INTDIR)\GfxSplitterWnd.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\GfxSplitterWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\GfxSplitterWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"

CPP_SWITCHES=/nologo /MD /W4 /GX /O2 /I "../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /Fp"$(INTDIR)\CJLibrary.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\CJLibrary.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\CJLibrary.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\CJLibrary.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_UNICODE" /Fp"$(INTDIR)\CJLibrary.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\CJLibrary.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_UNICODE" /Fp"$(INTDIR)\CJLibrary.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\CJLibrary.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Subclass.cpp

!IF  "$(CFG)" == "CJLibrary - Win32 Release"


"$(INTDIR)\Subclass.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Debug"


"$(INTDIR)\Subclass.obj"	"$(INTDIR)\Subclass.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Release"


"$(INTDIR)\Subclass.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ELSEIF  "$(CFG)" == "CJLibrary - Win32 Unicode Debug"


"$(INTDIR)\Subclass.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\CJLibrary.pch"


!ENDIF 


!ENDIF 

