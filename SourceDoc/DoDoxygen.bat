@REM --- COMMAND LINE PARAMETERS ---

@REM --- SELECT THE TARGET ---

@set DOC_TARGET=Help

@if "%1%"=="" set		DOC_TARGET=WinCvs
@if "%1%"=="WinCvs" set	DOC_TARGET=WinCvs
@if "%1%"=="W" set		DOC_TARGET=WinCvs
@if "%1%"=="w" set		DOC_TARGET=WinCvs

@if "%1%"=="MacCvs" set	DOC_TARGET=MacCvs
@if "%1%"=="M" set		DOC_TARGET=MacCvs
@if "%1%"=="m" set		DOC_TARGET=MacCvs

@if "%1%"=="gCvs" set	DOC_TARGET=gCvs
@if "%1%"=="G" set		DOC_TARGET=gCvs
@if "%1%"=="g" set		DOC_TARGET=gCvs

@REM --- DISPLAY HELP ---
@if %DOC_TARGET%==Help goto HELP


@REM --- SET THE ENVIRONMENT ---

@REM *Platform-specific variables*

@echo Building target: %DOC_TARGET%

@if %DOC_TARGET%==WinCvs (
	@REM *Project*
	@set DOX_PROJECT_NAME=WinCvs Source
	@set DOX_PROJECT_VERSION=2.0.6 Build 1
	@set DOX_OUTPUT_DIRECTORY=WinCvsSrcDoc
	@set DOX_PREDEFINED=WIN32 __AFX_H__ wnt_stat=stat
	@set DOX_INCLUDE_PATH=../common ../rf ../WinCvs/CJLib/Include

	@REM *Input*
	@set DOX_INPUT=../rf ../common ../WinCvs
	@set DOX_FILE_PATTERNS=
	@set DOX_EXCLUDE_PATTERNS=*/CVS/*
)

@if %DOC_TARGET%==MacCvs (
	@REM *Project*
	@set DOX_PROJECT_NAME=MacCvs Source
	@set DOX_PROJECT_VERSION=3.3a2
	@set DOX_OUTPUT_DIRECTORY=MacCvsSrcDoc
	@set DOX_PREDEFINED=qMacCvsPP __cplusplus=1 TARGET_RT_MAC_MACHO=1 TARGET_CARBON=1 __MWERKS__=0x3000 powerc=1 __powerc=1 __POWERPC__=1 ppc=1 macintosh=1 __MACOS__=1 TARGET_API_MAC_OS8=0 TARGET_API_MAC_CARBON=1 TARGET_API_MAC_OSX=0 __BIG_ENDIAN__=1
	@set DOX_INCLUDE_PATH=../common ../rf ../rf/support/gtk ../gcvs ../gcvs/src

	@REM *Input*
	@set DOX_INPUT=../common ../rf ../rf/support/gtk ../MacCvs ../MacCvs/src
	@set DOX_FILE_PATTERNS=*.c *.cc *.cpp *.cxx *.inl *.y *.h *.hpp *.odl
	@set DOX_EXCLUDE_PATTERNS=*/CVS/*
)

@if %DOC_TARGET%==gCvs (
	@REM *Project*
	@set DOX_PROJECT_NAME=gCvs Source
	@set DOX_PROJECT_VERSION=1.0b1
	@set DOX_OUTPUT_DIRECTORY=GCvsSrcDoc
	@set DOX_PREDEFINED=qUnix
	@set DOX_INCLUDE_PATH=../common ../rf ../rf/support/gtk ../gcvs ../gcvs/src

	@REM *Input*
	@set DOX_INPUT=../common ../rf ../rf/support/gtk ../gcvs ../gcvs/src
	@set DOX_FILE_PATTERNS=*.c *.cc *.cpp *.cxx *.inl *.y *.h *.hpp *.odl
	@set DOX_EXCLUDE_PATTERNS=*/CVS/*
)

@REM *Variable to strip from files path (STRIP_FROM_PATH)*
@REM You may need to hardcode the value of DOX_STRIP_FROM_PATH on NT on Win9x that doesn't support PWD for CD command
@set PWD_ORG=%CD%
@cd ..
@set DOX_STRIP_FROM_PATH=%CD:\=/%
@cd %PWD_ORG%

@REM *Output settings*
@set DOX_CASE_SENSE_NAMES=NO
@set DOX_SHORT_NAMES=YES
@set DOX_OUTPUT_LANG=English
@set DOX_USE_WINDOWS_ENCODING=YES

@REM *Output types*
@set DOX_GENERATE_HTML=YES
@set DOX_DOT_IMAGE_FORMAT=png
@set DOX_GENERATE_HTMLHELP=YES
@set DOX_GENERATE_TREEVIEW=NO
@set DOX_GENERATE_LATEX=NO
@set DOX_GENERATE_RTF=NO
@set DOX_GENERATE_MAN=NO
@set DOX_GENERATE_XML=NO
@set DOX_GENERATE_AUTOGEN_DEF=NO
@set DOX_SEARCHENGINE=NO


@REM --- SET THE FILES ---

@rem Execution log file
@set DOX_LOG=DoDoxygen.log

@rem HHC project file
@set DOX_HHP=%DOX_OUTPUT_DIRECTORY%\html\index.hhp


@REM --- EXECUTE ---

@echo Generating the documentation
@echo Start: %DATE% %TIME% > %DOX_LOG%
@doxygen.exe -d Time >> %DOX_LOG% 2>&1 

@echo Creating the compressed html file
@if EXIST %DOX_HHP% hhc.exe %DOX_HHP% >> %DOX_LOG%
@echo Finished: %DATE% %TIME% >> %DOX_LOG%

goto EXIT

@REM --- DISPLAY HELP ---

:HELP
@REM DISPLAY THE HELP INFO
@type Readme.txt | more
@PAUSE


@REM --- EXIT ---
:EXIT
