:: This is for compiling coin with cygwin - use cygwin not cygwin64 (unless you can figure out how to get Cygwin 64 to find the VS tools)
:: I don’t know exactly what you need to add to the basic setup but at least some of the gcc C++ extras need to be added in cygwin setup.
:: If something isn’t found you can go back to cygwin setup and search for the missing tool and add it

:: Get the source from https://grey.colorado.edu/svn/coin3d/coin/trunk
svn checkout --username anonymous --password emergent https://grey.colorado.edu/svn/coin3d/coin/trunk C:\src\coin

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

:: Get into a Cygwin shell.
:: Must be cygwin not cygwin64 - otherwise the VC++ compiler isn't found (saying it again!)

C:\cygwin\Cygwin.bat 

:: Adjust the following lines for each build type (/x64, /debug, etc)

export BITS=64
export BUILD_TYPE=release
export MSVS_EXT=msvs2015-$BITS
export BLD_EXT=qt56
export COINDIR=C:/Coin/4.0.0a

:: Set up the build environment.
:: The next few lines will move the cygwin compile tools to end of path so MSVS tools are found first.
unset CC
unset CXXmake
[ -n "${TMP}" ] || export TMP=/tmp
export PATH=`echo $PATH|sed 's,\(/usr/local/bin:/usr/bin\):\(.*\),\2:\1,'`
export COINDIR=`cygpath "$COINDIR"`

export COIN_BLD_DIR=/cygdrive/c/Coin/build/cyg-$BLD_EXT-$MSVS_EXT-$BUILD_TYPE
mkdir $COIN_BLD_DIR
cd $COIN_BLD_DIR

:: Disabling pkg-config seems to be fairly important:

:: Release libraries:
:: /cygdrive/c/src/coin/configure --disable-pkgconfig --prefix=$COINDIR --with-coin=$COINDIR --with-msvcrt=md --disable-debug

make

:: Debug libraries:
:: /cygdrive/c/src/coin/configure --disable-pkgconfig --prefix=$COINDIR --with-coin=$COINDIR --program-suffix=d --with-suffix=d --with-msvcrt=mdd

make

:: now you need to move the libraries into COINDIR