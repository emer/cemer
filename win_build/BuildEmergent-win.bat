# This script is a bootstrap for the Python script BuildEmergent-win.py
# NOTE: if you commit a new version of this file to svn, make sure to
# update the file on the ftp site as well

@echo off
setLocal EnableDelayedExpansion
color 07
cls
set python=

REM Create Emergent temp directory if it doesn't already exist
if exist %TEMP%\emergent goto tmpexists
mkdir %TEMP%\emergent
:tmpexists

REM Check if this script has already run and found a Python installation
echo.
echo Checking if Python has been found before...
if not exist %TEMP%\emergent\foundpy.txt goto findpython
for /f "tokens=* delims= " %%i in (%TEMP%\emergent\foundpy.txt) do (
  set python=%%i
  if exist !python! goto foundpy
)
echo Previous installation of Python no longer exists: %python%

REM Need to find a Python installation.  Start by looking for the latest.
:findpython
echo Looking for Python 2.7...
set python=C:\Python27\python.exe
if exist %python% goto foundpy

REM Couldn't find the latest, see if anything is installed.
:findany
echo Looking for any version of Python...
for /d %%i in (C:\*python*) do (
  set python=%%i\python.exe
  if exist !python! goto foundpy
)

REM Couldn't find any Python installations, see if the user knows of one.
:userpath
color 2e
echo.
echo Could not find a Python installation.  If you think you have Python installed,
echo enter the full path to the interpreter (python.exe).  Otherwise, hit enter.
set python=
set /p python=path: 
if "%python%" == "" goto askinstall
if exist %python% goto foundpy
goto userpath

REM User doesn't have Python installed yet, ask if they'd like to download it now.
:askinstall
echo.
set /p instpy=Would you like to download and install Python now? [Y/n] 
if "%instpy%" == "" goto installpython
if "%instpy%" == "Y" goto installpython
if "%instpy%" == "y" goto installpython

REM User doesn't want Python, so give up.
echo.
echo Quitting
goto theend

REM User wants to download and install Python.  Get it started.
:installpython
echo.
echo A Python installer will be downloaded using your web browser.
echo When the file finishes downloading, install it, accepting all default options.
echo Once the installation is complete, return to this screen.
echo.
set /p junk=Hit enter to begin downloading...
start http://www.python.org/ftp/python/2.7.1/python-2.7.1.msi
echo.
set /p junk=After Python is installed, hit enter to continue...
goto findpython

REM Python installation found.  Cache the location for next time.
:foundpy
color 07
echo.
echo Found Python installation: %python%
echo %python% > %TEMP%\emergent\foundpy.txt

REM Write a short script to download the real Python script we want to run
echo Creating helper script...
echo import urllib > %TEMP%\emergent\GetBuildEmergent-win.py
echo urllib.urlretrieve('ftp://grey.colorado.edu/pub/emergent/BuildEmergent-win.py', '%TEMP%\emergent\BuildEmergent-win.py') >> %TEMP%\emergent\GetBuildEmergent-win.py
	
REM Run that short script to get the real script
echo Downloading build script...
%python% %TEMP%\emergent\GetBuildEmergent-win.py
if exist %TEMP%\emergent\BuildEmergent-win.py goto runscript
echo.
echo Error downloading build script, quitting.
goto theend

REM Run the real script
:runscript
echo Running the build script...
%python% %TEMP%\emergent\BuildEmergent-win.py

:theend
color 07
pause