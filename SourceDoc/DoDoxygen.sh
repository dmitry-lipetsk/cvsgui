#!/bin/sh

# DISPLAY THE HELP INFO
help() {
	more Readme.txt | more
}

# --- COMMAND LINE PARAMETERS ---
case "$1" in
  "")
    # try to guess what's the right thing to build by default:
    #  If we're running on Mac OS X, we'll build the MacCvs documentation, else we'll build the gCvs documentation
    #  To find out whether we're running on Mac OS X, check whether the Cocoa framework folder exists
    #
    if [ -d /System/Library/Frameworks/Cocoa.framework ]; then
      whichDox="MacCvs"
    else
      whichDox="gCvs"
    fi
    ;;
  
  "M"|"m"|"MacCvs")
    whichDox="MacCvs"
    ;;
  
  "G"|"g"|"gCvs")
    whichDox="gCvs"
    ;;
  
  "W"|"w"|"WinCvs")
    whichDox="WinCvs"
    ;;
  
  *)
    help
    exit
    ;;
esac

# --- SET THE ENVIRONMENT ---

# *Platform-specific variables*
case "$whichDox" in
  "MacCvs")
    export DOX_PROJECT_NAME="MacCvs Source"
    export DOX_PROJECT_VERSION="3.3a2"
    export DOX_OUTPUT_DIRECTORY=MacCvsSrcDoc
    export DOX_PREDEFINED="qMacCvsPP" "__cplusplus=1" "TARGET_RT_MAC_MACHO=1" "TARGET_CARBON=1" "__MWERKS__=0x3000" "powerc=1" "__powerc=1" "__POWERPC__=1" "ppc=1" "macintosh=1" "__MACOS__=1" "TARGET_API_MAC_OS8=0" "TARGET_API_MAC_CARBON=1" "TARGET_API_MAC_OSX=0" "__BIG_ENDIAN__=1"
    export DOX_INCLUDE_PATH=" ../MacCvs ../common ../MacCvs/src ../MacCvs/tcl ../cvstree ../rf/support/powerplant ../rf ../MacCvs/WText/Source/Wrapper ../cvs-1.10/lib ../MacCvs/MachOGUSI ../MacCvs/WASTE"
    export DOX_INPUT="../common ../rf ../rf/support/powerplant ../cvstree ../MacCvs ../MacCvs/src ../MacCvs/tcl ../MacCvs/MachOGUSI"
  ;;
  
  "gCvs")
    export DOX_PROJECT_NAME="gCvs Source"
    export DOX_PROJECT_VERSION="1.0b1"
    export DOX_OUTPUT_DIRECTORY=GCvsSrcDoc
    export DOX_PREDEFINED="qUnix"
    export DOX_INCLUDE_PATH="../common ../rf ../rf/support/gtk ../gcvs ../gcvs/src"
    export DOX_INPUT="../common ../rf ../rf/support/gtk ../gcvs ../gcvs/src"
  ;;
  
  "WinCvs")
    export DOX_PROJECT_NAME="WinCvs Source"
    export DOX_PROJECT_VERSION="2.0.6 Build 1"
    export DOX_OUTPUT_DIRECTORY=WinCvsSrcDoc
    export DOX_PREDEFINED=WIN32 __AFX_H__
    export DOX_INCLUDE_PATH="../common ../rf ../WinCvs/CJLib/Include"
    export DOX_INPUT="../rf ../common ../WinCvs"
  ;;
esac

# *Input*
export DOX_FILE_PATTERNS="*.c *.cc *.cpp *.cxx *.inl *.y *.h *.hpp *.odl"
export DOX_EXCLUDE_PATTERNS="*/CVS/*"

# *Variable to strip from files path (STRIP_FROM_PATH)*
PWD_ORG=`pwd`
cd ..
export DOX_STRIP_FROM_PATH=`pwd`
cd "$PWD_ORG"

# *Output settings*
export DOX_CASE_SENSE_NAMES=NO
export DOX_SHORT_NAMES=YES
export DOX_MAN_EXT=4
export DOX_OUTPUT_LANG=English
export DOX_USE_WINDOWS_ENCODING=NO

# *Output types*
export DOX_GENERATE_HTML=YES
export DOX_DOT_IMAGE_FORMAT=gif
export DOX_GENERATE_HTMLHELP=YES
export DOX_GENERATE_TREEVIEW=NO
export DOX_GENERATE_LATEX=NO
export DOX_GENERATE_RTF=NO
export DOX_GENERATE_MAN=NO
export DOX_GENERATE_XML=NO
export DOX_GENERATE_AUTOGEN_DEF=NO
export DOX_SEARCHENGINE=NO


# --- SET THE FILES ---

#@rem Execution log file
DOX_LOG=DoDoxygen.log

#@rem HHC project file
#export DOX_HHP=$DOX_OUTPUT_DIRECTORY\html\index.hhp


# --- EXECUTE ---

echo Generating the documentation for $whichDox
echo Start: `date +"%d.%m.%y %H:%M:%S"`> $DOX_LOG
doxygen -d Time >> $DOX_LOG 2>&1 

#echo Creating the compressed html file
#if EXIST %DOX_HHP% hhc.exe %DOX_HHP% >> %DOX_LOG%

echo Finished: `date +"%d.%m.%y %H:%M:%S"` >> $DOX_LOG

