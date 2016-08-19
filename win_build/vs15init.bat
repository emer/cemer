call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

:: for unix shell users
doskey ls=dir $*
doskey cp=copy $*
doskey rmr=rmdir /s $*

set BITS=64
set BUILD_TYPE=Release
set SRC_DIR=C:\src\emergent
set ARCH=64

set QTDIR=C:\Qt\5.6.1\5.6\msvc2015_64
set EMER_3RDPARTY_DIR=C:\3rdParty
set COINDIR=C:\Coin\4.0.0a
set EMER_SVN_LIBS_DIR=C:\Subversion
set ZLIB_ROOT=C:\3rdParty\zlib-win64-msvs2013

:: this is the key configure command -- you can now type it just by typing "configure"
doskey configure=cmake %SRC_DIR% -G "NMake Makefiles JOM" -Wno-dev -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DQT_USE_5=ON -DQTDIR=%QTDIR% -DCMAKE_SHARED_LINKER_FLAGS_DEBUG="/INCREMENTAL:NO"ù -DCMAKE_MODULE_LINKER_FLAGS_DEBUG="/INCREMENTAL:NO"ù -DCMAKE_SHARED_LINKER_FLAGS="/machine:X%ARCH%" -DCMAKE_MODULE_LINKER_FLAGS="/machine:X%ARCH%" -DCMAKE_EXE_LINKER_FLAGS="/machine:X%ARCH%" -DZLIB_ROOT=%ZLIB_ROOT%

path C:\Program Files (x86)\CMake_3.3.2\bin;C:\Subversion;C:\DevTools\jom_1_1_1;%QTDIR%\bin;%COINDIR%\bin;%3RDPARTY_DIR%\bin;%EMER_SVN_LIBS_DIR%\bin;C:\Windows\system32;%PATH%

cd %SRC_DIR%
