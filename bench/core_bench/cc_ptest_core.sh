#!/bin/csh -f

# cocor command
set CC = "g++ -O2"
set CCd = "g++ -g"
echo $CC
$CC -o ptest_core ptest_core.cc -lm -lQtCore -I$QTDIR/include -L$QTDIR/lib

echo $CCd
$CCd -o ptest_cored ptest_core.cc -lm -lQtCore -I$QTDIR/include -L$QTDIR/lib
