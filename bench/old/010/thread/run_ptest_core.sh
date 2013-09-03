#!/bin/csh -f

#echo "**SINGLE THREAD OPTIMIZED**"
./ptest_core 52 6000 0 -header
./ptest_core 52 6000 0

./ptest_core 500 100 0
./ptest_core 500 100 0

./ptest_core 5000 6 0
./ptest_core 5000 6 0


#echo "**1 THREAD**"
./ptest_core 52 6000 1
./ptest_core 52 6000 1

./ptest_core 500 100 1
./ptest_core 500 100 1

./ptest_core 5000 6 1
./ptest_core 5000 6 1


#echo "**2 THREAD**"
./ptest_core 52 6000 2
./ptest_core 52 6000 2

./ptest_core 500 100 2
./ptest_core 500 100 2

./ptest_core 5000 6 2
./ptest_core 5000 6 2


#echo "**4 THREAD**"
./ptest_core 52 6000 4
./ptest_core 52 6000 4

./ptest_core 500 100 4
./ptest_core 500 100 4

./ptest_core 5000 6 4
./ptest_core 5000 6 4

