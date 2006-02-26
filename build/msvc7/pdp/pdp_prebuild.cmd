if not "%PDP4DIR%" == "" goto cont1
echo PDP4DIR environment variable must be set
rem set error level
goto end

:cont1
rem parameter means just run the item
if "%1" == "" goto cont2

@cd %PDP4DIR%\src\%1
nmake /f %PDP4DIR%\build\msvc7\Makefile %2
goto end


:cont2
@cd %PDP4DIR%\src\pdp
nmake /f %PDP4DIR%\build\msvc7\Makefile pdp_files
@if errorlevel 1 goto end

@cd %PDP4DIR%\src\bp
nmake /f %PDP4DIR%\build\msvc7\Makefile bp_files
@if errorlevel 1 goto end


:end
