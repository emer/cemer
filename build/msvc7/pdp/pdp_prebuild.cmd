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
@cd %PDP4DIR%\src\pdp
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" pdp_files
@if errorlevel 1 goto end

@cd %PDP4DIR%\src\cs
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" cs_files
@if errorlevel 1 goto end

@cd %PDP4DIR%\src\bp
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" bp_files
@if errorlevel 1 goto end

@cd %PDP4DIR%\src\leabra
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" leabra_files
@if errorlevel 1 goto end

@cd %PDP4DIR%\src\so
nmake /f %PDP4DIR%\build\msvc7\Makefile "TARGET=%1" so_files
@if errorlevel 1 goto end


:end
