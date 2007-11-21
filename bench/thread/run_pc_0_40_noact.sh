#!/bin/csh -f

# tests changes in activation percent

#echo "**SINGLE THREAD OPTIMIZED**"
./ptest_core 100 10000 0 3 40 5 -header -act=0
./ptest_core 100 10000 0 3 40 5 -act=0
./ptest_core 100 10000 0 3 40 20 -act=0
./ptest_core 100 10000 0 3 40 20 -act=0
./ptest_core 100 5000 0 3 40 50 -act=0
./ptest_core 100 5000 0 3 40 50 -act=0
./ptest_core 100 5000 0 3 40 100 -act=0
./ptest_core 100 5000 0 3 40 100 -act=0


./ptest_core 400 4000 0 3 40 5 -act=0
./ptest_core 400 4000 0 3 40 5 -act=0
./ptest_core 400 4000 0 3 40 20 -act=0
./ptest_core 400 4000 0 3 40 20 -act=0
./ptest_core 400 2000 0 3 40 50 -act=0
./ptest_core 400 2000 0 3 40 50 -act=0
./ptest_core 400 2000 0 3 40 100 -act=0
./ptest_core 400 2000 0 3 40 100 -act=0

./ptest_core 1600 1000 0 3 40 5 -act=0
./ptest_core 1600 1000 0 3 40 5 -act=0
./ptest_core 1600 1000 0 3 40 20 -act=0
./ptest_core 1600 1000 0 3 40 20 -act=0
./ptest_core 1600 500 0 3 40 50 -act=0
./ptest_core 1600 500 0 3 40 50 -act=0
./ptest_core 1600 500 0 3 40 100 -act=0
./ptest_core 1600 500 0 3 40 100 -act=0

./ptest_core 6400 100 0 3 40 5 -act=0
./ptest_core 6400 100 0 3 40 5 -act=0
./ptest_core 6400 100 0 3 40 20 -act=0
./ptest_core 6400 100 0 3 40 20 -act=0
./ptest_core 6400 50 0 3 40 50 -act=0
./ptest_core 6400 50 0 3 40 50 -act=0
./ptest_core 6400 50 0 3 40 100 -act=0
./ptest_core 6400 50 0 3 40 100 -act=0


