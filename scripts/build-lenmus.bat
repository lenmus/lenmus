::------------------------------------------------------------------------------
:: Building LenMus Phonascus in Windows
:: This script DOES NOT INSTALL LenMus Phonascus. It just creates
:: lenmus binaries in folder <root>\zz_build_area\bin
::
:: Usage: 
::   This script MUST BE RUN from folder <root>\scripts\
::      cd C:\Users\cecilio\source\repos\lenmus\scripts
::      build-lenmus.bat
::------------------------------------------------------------------------------

@echo off

set E_SUCCESS=0         ::success
set E_NOARGS=65         ::no arguments
set E_BADPATH=66        ::not running from lenmus/scripts
set E_BADARGS=67        ::bad arguments
set E_BUIL_ERROR=68

::get current directory and check we are running from <root>\scripts.
::For this I just check that "src" folder exists
set scripts_path=%CD%
for %%f in ("%scripts_path%") do set this_folder=%%~nxf
for /D %%D in ("%scripts_path%") do (
    set "root_path=%%~dpD"
)
::echo root_path = %root_path%
if not exist "%root_path%/src" (
    echo "Error: not running from <root>\scripts"
    exit /B %E_BADPATH%
)

::path for building
set build_path=%root_path%zz_build-win10
set sources=%root_path%

::create or clear build folder
if not exist "%build_path%" (
    ::create build folder
    echo Creating build folder
    mkdir %build_path%
    echo -- Build folder created
) else (
    :: clear build folders
    echo Removing last build
    cd %build_path% || exit /B %E_BADPATH%
    del %build_path%\*.* /s /f /q  > NUL
    echo -- Build folders now empty
)

:: create makefile
cd %build_path%
echo Creating makefile
cmake -G "Ninja" %sources% -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG" || exit /B %E_BUIL_ERROR%
echo -- Makefile created

::build program
echo Building LenMus
set start=%time%
ninja || exit /B %E_BUIL_ERROR%
set end=%time%

:: compute elapsed time
set options="tokens=1-4 delims=:.,"
for /f %options% %%a in ("%start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %hours% lss 0 set /a hours = 24%hours%
if 1%ms% lss 100 set ms=0%ms%
echo Build finished
echo -- Build time: %hours%h %mins%m %secs%.%ms%s

cd ..\scripts
exit /B %E_SUCCESS%
