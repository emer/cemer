#!/bin/csh -f

./ptest_core 50 60000 1
./ptest_core 50 60000 1

./ptest_core 500 1000 1
./ptest_core 500 1000 1

./ptest_core 5000 10 1
./ptest_core 5000 10 1

./ptest_core 8000 6 1
./ptest_core 8000 6 1


./ptest_core 50 60000 2
./ptest_core 50 60000 2

./ptest_core 500 2000 2
./ptest_core 500 2000 2

./ptest_core 5000 20 2
./ptest_core 5000 20 2

./ptest_core 8000 6 2
./ptest_core 8000 6 2


./ptest_core 50 60000 4
./ptest_core 50 60000 4

./ptest_core 500 4000 4
./ptest_core 500 4000 4

./ptest_core 5000 40 4
./ptest_core 5000 40 4

./ptest_core 8000 6 4
./ptest_core 8000 6 4
