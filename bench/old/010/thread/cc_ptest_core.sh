#!/bin/csh -f

# cocor command
set CC = "g++ -mtune=prescott -msse -mfpmath=sse"
#set CC = "g++"
echo $CC
$CC -O2 -S ptest_core.cc -I$QTDIR/include
$CC -g -O2 -o ptest_core ptest_core.cc -lm -lQtCore -I$QTDIR/include -L$QTDIR/lib

$CC -g -o ptest_cored ptest_core.cc -lm -lQtCore -I$QTDIR/include -L$QTDIR/lib
