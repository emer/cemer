Instructions for running the benchmark:

./configure [release] clean   # generates the makefiles using cmake -- cat the file
	    	      	      # for options 

make                          # compiles into build subdir

./build/x64 N_UNITS N_CONNECTIONS GPUorCPU		      # run the benchmark

For example

./build/x64 20000 3000 1  # runs the computation on the GPU
./build/x64 20000 3000 0  # runs the computation on the CPU
