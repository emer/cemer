// Copyright, 1995-2013, Regents of the University of Colorado,
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

// parent includes:
#include <ta_def.h>

#ifdef CUDA_COMPILE

#include <cuda_runtime.h>
#include <curand.h>
#include <taMisc>

#define CudaSafeCall(err)  __CudaSafeCall(err,__FILE__,__LINE__)

inline void __CudaSafeCall(cudaError err,
                           const char *file, const int line){
  if(cudaSuccess != err) {
    taMisc::Error("CudaSafeCall() Runtime API error:",
                  cudaGetErrorString(err), "\nin file:", file, ":", String(line));
  }
}

#endif // CUDA_COMPILE

#endif // ta_cuda_h
