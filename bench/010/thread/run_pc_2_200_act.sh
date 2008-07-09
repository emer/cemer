#!/bin/csh -f

# tests changes in activation percent

#echo "**SINGLE THREAD OPTIMIZED**"
./ptest_core 100 10000 2 3 200 5 -header
./ptest_core 100 10000 2 3 200 5
./ptest_core 100 10000 2 3 200 20
./ptest_core 100 10000 2 3 200 20
./ptest_core 100 5000 2 3 200 50
./ptest_core 100 5000 2 3 200 50
./ptest_core 100 5000 2 3 200 100
./ptest_core 100 5000 2 3 200 100


./ptest_core 400 4000 2 3 200 5
./ptest_core 400 4000 2 3 200 5
./ptest_core 400 4000 2 3 200 20
./ptest_core 400 4000 2 3 200 20
./ptest_core 400 2000 2 3 200 50
./ptest_core 400 2000 2 3 200 50
./ptest_core 400 2000 2 3 200 100
./ptest_core 400 2000 2 3 200 100

./ptest_core 1600 1000 2 3 200 5
./ptest_core 1600 1000 2 3 200 5
./ptest_core 1600 1000 2 3 200 20
./ptest_core 1600 1000 2 3 200 20
./ptest_core 1600 500 2 3 200 50
./ptest_core 1600 500 2 3 200 50
./ptest_core 1600 500 2 3 200 100
./ptest_core 1600 500 2 3 200 100

./ptest_core 6400 100 2 3 200 5
./ptest_core 6400 100 2 3 200 5
./ptest_core 6400 100 2 3 200 20
./ptest_core 6400 100 2 3 200 20
./ptest_core 6400 50 2 3 200 50
./ptest_core 6400 50 2 3 200 50
./ptest_core 6400 50 2 3 200 100
./ptest_core 6400 50 2 3 200 100


