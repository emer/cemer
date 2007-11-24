################################################################################
#
# Build script for project
#
################################################################################

# Add source files here
EXECUTABLE	:= ./embench_cuda
# Cuda source files (compiled with cudacc)
CUFILES		:= embench_cuda.cu
# C/C++ source files (compiled with gcc / c++)
CCFILES		:= \
	main.cpp 
# C/C++ header files
HEADERS		+= embench_cuda.h

BINDIR	= ./

################################################################################
# Rules and targets

include ${CUDA_SDK}/common/common.mk

# additional dependencies

$(OBJS) : $(HEADERS)
