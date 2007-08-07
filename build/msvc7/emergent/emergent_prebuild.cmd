@echo off
rem note: EXIT commands cause immediate exit, with the indicated error code
if not "%EMERGENTDIR%" == "" goto cont1
echo *** EMERGENTDIR environment variable must be set
EXIT /B 2

:cont1
if not "%1" == "" goto cont2
echo *** Must specify Target (Debug or Release)
EXIT /B 2

:cont2
@cd %EMERGENTDIR%\src\emergent\network
nmake /f %EMERGENTDIR%\build\msvc7\Makefile "TARGET=%1" network_files
@if errorlevel 1 goto end

@cd %EMERGENTDIR%\src\emergent\cs
nmake /f %EMERGENTDIR%\build\msvc7\Makefile "TARGET=%1" cs_files
@if errorlevel 1 goto end

@cd %EMERGENTDIR%\src\emergent\bp
nmake /f %EMERGENTDIR%\build\msvc7\Makefile "TARGET=%1" bp_files
@if errorlevel 1 goto end

@cd %EMERGENTDIR%\src\emergent\leabra
nmake /f %EMERGENTDIR%\build\msvc7\Makefile "TARGET=%1" leabra_files
@if errorlevel 1 goto end

@cd %EMERGENTDIR%\src\emergent\so
nmake /f %EMERGENTDIR%\build\msvc7\Makefile "TARGET=%1" so_files
@if errorlevel 1 goto end


:end
