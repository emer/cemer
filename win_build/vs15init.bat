:: This is a .BAT script for configuring the emergent build process under VS tools

:: to make this runnable as a desktop shortcut, first create a shortcut to cmd.exe on the desktop
:: (open the windows file explorer, go to C:\Windows\System32, search for cmd.exe, right click, create shortcut)
:: then edit the shortcut's properties on the desktop, and in the box where it says Target, enter:
:: C:\Windows\System32\cmd.exe /K C:\src\emergent\win_build\vs15init.bat
:: and under the Advanced tab you can click the "Run as Administrator" button
:: you can also change the name in General tab to something like emergent vs15init

:: all the init for command-line operation of VS
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

:: doskey = aliases!

:: for unix shell users
doskey ls=dir $*
doskey cp=copy $*
doskey rm=del $*
doskey rmr=rmdir /s $*
doskey mv=move $*

set BITS=64
set BUILD_TYPE=Release
set SRC_DIR=C:\src\emergent
set ARCH=64

set QTDIR=C:\Qt\Qt5.6.1\5.6\msvc2015_64
set EMER_MISC_LIBS_DIR=C:\EmerMiscLibs
set COINDIR=C:\Coin\4.0.0a
set EMER_SVN_LIBS_DIR=C:\Subversion
set ZLIB_ROOT=C:\EmerMiscLibs

:: this is the key configure command -- you can now type it just by typing "configure"
doskey configure=cmake %SRC_DIR% -G "NMake Makefiles JOM" -Wno-dev -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DQT_USE_5=ON -DQTDIR=%QTDIR% -DCMAKE_SHARED_LINKER_FLAGS="/machine:X%ARCH%" -DCMAKE_MODULE_LINKER_FLAGS="/machine:X%ARCH%" -DCMAKE_EXE_LINKER_FLAGS="/machine:X%ARCH%" -DZLIB_ROOT=%ZLIB_ROOT%

:: Following is for ide usage -- default to Debug mode for ide b/c that is what it is useful for:

:: The following environment variable must be set from the command line to force the IDE to use the x64 native compiler
set PreferredToolArchitecture=x64

:: run this in a build_ide_dbg subdirectory to configure for ide
doskey configure_ide_dbg=cmake %SRC_DIR% -G "Visual Studio 14 Win64" -Wno-dev -DCMAKE_BUILD_TYPE=Debug -DQT_USE_5=ON -DQTDIR=%QTDIR% -DCMAKE_SHARED_LINKER_FLAGS="/machine:X%ARCH%" -DCMAKE_MODULE_LINKER_FLAGS="/machine:X%ARCH%" -DCMAKE_EXE_LINKER_FLAGS="/machine:X%ARCH%" -DZLIB_ROOT=%ZLIB_ROOT%

:: run this in any build directory that cmake generated, e.g., the one from configure_ide_dbg above
:: ALWAYS open the solution from the command line, to ensure the PreferredToolArchitecture variable is set correctly.
doskey vside="C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe" Emergent.sln

:: set the global path to include all our stuff -- this path is also used to find .dll's so it is like LD_LIBRARY_PATH
path C:\Program Files (x86)\CMake\bin;C:\Subversion;C:\jom_1_1_1;C:\cygwin64\bin;%QTDIR%\bin;%COINDIR%\bin;%EMER_MISC_LIBS_DIR%\bin;%EMER_SVN_LIBS_DIR%;%EMER_SVN_LIBS_DIR%\bin;;C:\Windows\system32;%PATH%

cd %SRC_DIR%
