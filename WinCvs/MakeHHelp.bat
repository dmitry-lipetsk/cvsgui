@echo off

REM --- CHECK PARAMETERS ---
if "%1%"=="" goto HELP

REM --- PREPARE ENVIRONMENT ---
set tName=%1
set TargetName=%tName:"=%

REM --- CREATE RESOURCE ID MAPPING ---
echo. >"HHelp\%TargetName%.hm"  

echo // Commands (ID_* and IDM_*) >>"HHelp\%TargetName%.hm"  
makehm ID_,IDH_ID_,0x10000 IDM_,IDH_IDM_,0x10000 resource.h >>"HHelp\%TargetName%.hm"  

echo. >>"HHelp\%TargetName%.hm"  

echo // Prompts (IDP_*) >>"HHelp\%TargetName%.hm"  
makehm IDP_,IDH_IDP_,0x30000 resource.h >>"HHelp\%TargetName%.hm"  

echo. >>"HHelp\%TargetName%.hm"  

echo // Resources (IDR_*) >>"HHelp\%TargetName%.hm"  
makehm IDR_,IDH_IDR_,0x20000 resource.h >>"HHelp\%TargetName%.hm"  

echo. >>"HHelp\%TargetName%.hm"  

echo // Dialogs (IDD_*) >>"HHelp\%TargetName%.hm"  
makehm IDD_,IDH_IDD_,0x20000 resource.h >>"HHelp\%TargetName%.hm"  

echo. >>"HHelp\%TargetName%.hm"  

echo // Frame Controls (IDW_*) >>"HHelp\%TargetName%.hm"  
makehm IDW_,IDH_IDW_,0x50000 resource.h >>"HHelp\%TargetName%.hm"  

echo. >>"HHelp\%TargetName%.hm"  

echo // Dialog Controls (IDC_*) >>"HHelp\%TargetName%.hm"  
makehm IDC_,IDH_IDC_,0x00000 resource.h >>"HHelp\%TargetName%.hm"  

echo. >>"HHelp\%TargetName%.hm"  

REM --- CREATE C++ HEADER FILE ---
echo.Formatting HTML Help map include file...  
if exist "HHelp\%TargetName%.h" del HHelp\%TargetName%.h  
for /F "tokens=1 delims=" %%i in (HHelp\%TargetName%.hm) do (  
set replace=%%i  
call :PROCESS  
)  
goto END  
:PROCESS  
echo.%replace:IDH_=#define IDH_%>>HHelp\%TargetName%.h  
:END

REM --- EXIT ---
goto EXIT

:HELP
REM --- DISPLAY HELP ---
echo Missing the target parameter

REM --- EXIT ---
:EXIT
