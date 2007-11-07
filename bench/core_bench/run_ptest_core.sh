#!/bin/csh -f

./ptest_core 50 40000 1
./ptest_core 50 40000 2
./ptest_core 50 40000 4

./ptest_core 100 10000 1
./ptest_core 100 10000 2
./ptest_core 100 10000 4

#./ptest_core 500 4000 1

./ptest_core 1000 1000 1
./ptest_core 1000 1000 2
./ptest_core 1000 1000 4

./ptest_core 5000 40 1
./ptest_core 5000 40 2
./ptest_core 5000 40 4

