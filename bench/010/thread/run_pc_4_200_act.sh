#!/bin/csh -f

# tests changes in activation percent

#echo "**SINGLE THREAD OPTIMIZED**"
./ptest_core 100 10000 4 3 200 5 -header
./ptest_core 100 10000 4 3 200 5
./ptest_core 100 10000 4 3 200 20
./ptest_core 100 10000 4 3 200 20
./ptest_core 100 5000 4 3 200 50
./ptest_core 100 5000 4 3 200 50
./ptest_core 100 5000 4 3 200 100
./ptest_core 100 5000 4 3 200 100


./ptest_core 400 4000 4 3 200 5
./ptest_core 400 4000 4 3 200 5
./ptest_core 400 4000 4 3 200 20
./ptest_core 400 4000 4 3 200 20
./ptest_core 400 2000 4 3 200 50
./ptest_core 400 2000 4 3 200 50
./ptest_core 400 2000 4 3 200 100
./ptest_core 400 2000 4 3 200 100

./ptest_core 1600 1000 4 3 200 5
./ptest_core 1600 1000 4 3 200 5
./ptest_core 1600 1000 4 3 200 20
./ptest_core 1600 1000 4 3 200 20
./ptest_core 1600 500 4 3 200 50
./ptest_core 1600 500 4 3 200 50
./ptest_core 1600 500 4 3 200 100
./ptest_core 1600 500 4 3 200 100

./ptest_core 6400 100 4 3 200 5
./ptest_core 6400 100 4 3 200 5
./ptest_core 6400 100 4 3 200 20
./ptest_core 6400 100 4 3 200 20
./ptest_core 6400 50 4 3 200 50
./ptest_core 6400 50 4 3 200 50
./ptest_core 6400 50 4 3 200 100
./ptest_core 6400 50 4 3 200 100


