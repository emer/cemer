@echo off
rem note: EXIT commands cause immediate exit, with the indicated error code
if not "%EMERGENTDIR%" == "" goto cont1
echo *** EMERGENTDIR environment variable must be set
EXIT /B 2

:cont1
if not "%1" == "Debug" goto cont1a
rem Debug

goto cont2
:cont1a
if not "%1" == "Release" goto cont1b
rem Release

goto cont2
:cont1b
echo *** Must specify Target (Debug or Release)
EXIT /B 2

:cont2
@cd %EMERGENTDIR%\src\temt\taiqtso
nmake /f %EMERGENTDIR%\build\Makefile "TARGET=%1" taiqtso_files
@if errorlevel 1 goto end

@cd %EMERGENTDIR%\src\temt\ta
nmake /f %EMERGENTDIR%\build\Makefile "TARGET=%1" ta_files
@if errorlevel 1 goto end

@cd %EMERGENTDIR%\src\temt\css
nmake /f %EMERGENTDIR%\build\Makefile "TARGET=%1" css_files
@if errorlevel 1 goto end

:end
