#!/bin/csh -f

echo "**SINGLE THREADED**"
./ptest_core2 52 6000 0
./ptest_core2 52 6000 0

./ptest_core2 500 100 0
./ptest_core2 500 100 0

./ptest_core2 5000 6 0
./ptest_core2 5000 6 0


echo "**1 THREADED**"
./ptest_core2 52 6000 1
./ptest_core2 48 6000 1

./ptest_core2 500 100 1
./ptest_core2 500 100 1

./ptest_core2 5000 6 1
./ptest_core2 5000 6 1

echo "**2 THREADED**"
./ptest_core2 52 6000 2
./ptest_core2 52 6000 2

./ptest_core2 500 100 2
./ptest_core2 500 100 2

./ptest_core2 5000 6 2
./ptest_core2 5000 6 2

