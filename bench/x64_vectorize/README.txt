Instructions for running the benchmark:

./configure [release] clean   # generates the makefiles using cmake -- cat the file
	    	      	      # for options 

make                          # compiles into build subdir

./build/x64vec		      # run the benchmark

Currently, no args are processed -- reasonable defaults are used

typical output for non-release (default -O2 optimization, no loop vectorization):

103 shadow:.../bench/x64_vectorize>./build/x64vec
total time used: 7.82485 total flops: 1.46801e+09 mflops/sec: 187.608

and for release, -O3 optimization, loop vectorization on a relatively modern i7 mac,
with clang 3.3 -- nearly a 10x speedup!!

108 shadow:.../bench/x64_vectorize>./build/x64vec
total time used: 1.10328 total flops: 1.46801e+09 mflops/sec: 1330.59


#########################
# Setting the compiler

set the environment variable CXX then do ./configure clean again

setenv CXX "/usr/bin/g++"
./configure release clean
