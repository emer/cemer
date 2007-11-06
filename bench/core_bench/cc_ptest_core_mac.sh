#!/bin/csh -f

# cocor command
set CC = "g++ -O2"
set CCd = "g++ -g -O2"
echo $CC
$CC -o ptest_core ptest_core.cc -lm -Wl,-framework,QtCore -L/Library/Frameworks
echo $CCd
$CCd -o ptest_cored ptest_core.cc -lm -Wl,-framework,QtCore -L/Library/Frameworks

