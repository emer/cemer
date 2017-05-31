// Copyright 2016, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ta_cuda_h
#define ta_cuda_h 1

#ifdef CUDA_COMPILE

#include <cuda_runtime.h>
#include <curand.h>
#include <stdio.h>

#define cudaSafeCall(err)  __cudaSafeCall(err,__FILE__,__LINE__)

inline void __cudaSafeCall(cudaError err,
                           const char *file, const int line) {
  if(cudaSuccess != err) {
    printf("%s(%i) : cudaSafeCall() Runtime API error : %s.\n",
           file, line, cudaGetErrorString(err) );
  }
}

// important: nvcc cannot compile Qt, etc, so we have to define everything
// completely separately in cuda space
// regular files can include cuda files, and serve as bridges into cuda
// but otherwise cuda must remain fully self-contained

// IMPORTANT: for indexing into the connection-level mem, for large nets, we really 
// should use int64_t -- BUT this does NOT seem to work, at least on latest
// mac pro retina late 2014 NVIDIA GeForce GT 750M chip!  should be 3.0 compute 
// capability but still doesn't work reliably!

// todo: revisit!

typedef int cudabigint;
// typedef int64_t bigint;
// typedef long long bigint;


// this CUDAFUN macro allows marked functions to work either in C++ or
// in device code, depending on the __CUDACC__ define which is only true
// inside of nvcc -- marking as both host and device allows either

#ifdef __CUDACC__
#define CUDAFUN __device__ __host__
#else
#define CUDAFUN
#endif


#endif // CUDA_COMPILE

#endif // ta_cuda_h
