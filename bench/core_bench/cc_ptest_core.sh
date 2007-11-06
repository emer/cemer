#!/bin/csh -f

# cocor command
set CC = "g++ -O2"
#set CC = "g++ -g"
$CC -o ptest_core ptest_core.cc -lm -lQtCore -I$QTDIR/include -L$QTDIR/lib

echo $CC
