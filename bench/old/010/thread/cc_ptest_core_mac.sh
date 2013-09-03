#!/bin/csh -f

# cocor command
set CC = "g++"
set CCd = "g++"
echo $CC
$CC -O2 -S ptest_core2.cc  -L/Library/Frameworks
$CC -O2 -o ptest_core2 ptest_core2.cc -lm -Wl,-framework,QtCore -L/Library/Frameworks
echo $CCd
$CCd -g -o ptest_core2d ptest_core2.cc -lm -Wl,-framework,QtCore -L/Library/Frameworks

