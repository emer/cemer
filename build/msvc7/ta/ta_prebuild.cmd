@echo off
rem note: EXIT commands cause immediate exit, with the indicated error code
if not "%PDP4DIR%" == "" goto cont1
echo *** PDP4DIR environment variable must be set
EXIT /B 2

:cont1
if not "%1" == "" goto cont2
echo *** Must specify Target (Debug or Release)
EXIT /B 2

:cont2
@cd %PDP4DIR%\src\taiqtso
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" taiqtso_files
@if errorlevel 1 goto end

@cd %PDP4DIR%\src\ta
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" ta_files
@if errorlevel 1 goto end

@cd %PDP4DIR%\src\css
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" css_files
@if errorlevel 1 goto end

:end
