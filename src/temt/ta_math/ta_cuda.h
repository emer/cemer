// Copyright 2014-2018, Regents of the University of Colorado,
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
#include <curand_kernel.h>
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

// could use different random state -- use define
#define CU_RAND_STATE curandState

class Random_cuda {
  // Random Number Generation in cuda
public:
  enum Type {
    UNIFORM,                    // uniform with var = range on either side of the mean
    BINOMIAL,                   // binomial with var = p, par = n
    POISSON,                    // poisson with lambda = var
    GAMMA,                      // gamma with var scaling parameter and par = k stages
    GAUSSIAN,                   // normal with var
    BETA,                       // beta with var = a and par = b shape parameters
    NONE,                       // just the mean
  };

  Type          type;           // type of random variable to generate
  double        mean;           // mean of random distribution
  double        var;            // #CONDSHOW_OFF_type:NONE 'varibility' parameter for the random numbers (gauss = standard deviation, not variance; uniform = half-range)
  double        par;            // #CONDSHOW_ON_type:GAMMA,BINOMIAL,BETA extra parameter for distribution (depends on each one)

  CUDAFUN float Gen(CU_RAND_STATE* state) const {
    if(var == 0.0f) return mean;
    switch(type) {
    case NONE:
      return mean;
    case UNIFORM:
      return UniformMeanRange(mean, var, state);
    // case BINOMIAL:
    //   return mean + Binom((int)par, var, state);
    case POISSON:
      return mean + Poisson(var, state);
    // case GAMMA:
    //   return mean + Gamma(var, (int)par, state);
    case GAUSSIAN:
      return mean + Gauss(var, state);
    // case BETA:
    //   return mean + Beta(var, par, state);
    }
    return 0.0f;
  }

  CUDAFUN static int    IntZeroN(int n, CU_RAND_STATE* state)
  { if(n > 0) return (int)(curand(state) % (uint)n); return 0; }
  // #CAT_Int uniform random integer in the range between 0 and n, exclusive of n: [0,n) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  CUDAFUN static int    IntMinMax(int min, int max, CU_RAND_STATE* state)
  { return min + IntZeroN(max - min, state); }
  // #CAT_Int uniform random integer in range between min and max, exclusive of max: [min,max) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  CUDAFUN static int    IntMeanRange(int mean, int range, CU_RAND_STATE* state)
  { return mean + (IntZeroN(2 * range + 1, state) - range); }
  // #CAT_Int uniform random integer with given range on either side of the mean: [mean - range, mean + range] -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  // CUDAFUN static int    Discrete(taMatrix* distribution, CU_RAND_STATE* state);
  // // #CAT_Int Discrete distribtion. Samples from a discrete distribution with probabilities given (automatically renormalizes the values) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  CUDAFUN static float ZeroOne(CU_RAND_STATE* state)
  { return curand_uniform(state); }
  // #CAT_Float uniform random number between zero and one (inclusive of 1 due to rounding!) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  CUDAFUN static float UniformMinMax(float min, float max, CU_RAND_STATE* state)
  { return min + (max - min) * ZeroOne(state); }
  // #CAT_Float uniform random number between min and max values inclusive (Do not use for generating integers - will not include max!) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  CUDAFUN static float UniformMeanRange(float mean, float range, CU_RAND_STATE* state)
  { return mean + range * 2.0 * (ZeroOne(state) - 0.5); }
  // #CAT_Float uniform random number with given range on either size of the mean: [mean - range, mean + range] -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  // CUDAFUN static float Binom(int n, float p, CU_RAND_STATE* state);
  // // #CAT_Float binomial with n trials (par) each of probability p (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  CUDAFUN static float Poisson(float l, CU_RAND_STATE* state)
  { return curand_poisson(state, l); }
  // #CAT_Float poisson with parameter l (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
 // TODO: Also note that this distribuition requires pre-processing on the host. See the description of curandCreatePoissonDistribution() below.
  
  // CUDAFUN static float Gamma(float var, float j, CU_RAND_STATE* state);
  // // #CAT_Float gamma with given variance, number of exponential stages (par) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  CUDAFUN static float Gauss(float stdev, CU_RAND_STATE* state)
  { return stdev * curand_normal(state); }
  // #CAT_Float gaussian (normal) random number with given standard deviation -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  // CUDAFUN static float Beta(float a, float b, CU_RAND_STATE* state);
  // #CAT_Float beta random number with two shape parameters a > 0 and b > 0 -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)


  CUDAFUN static bool   BoolProb(float p, CU_RAND_STATE* state)
  { return (ZeroOne(state) < p); }
  // #CAT_Bool boolean true/false with given probability -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  Random_cuda() { Initialize(); }
  
private:
  void Initialize() {
    type = UNIFORM;
    mean = 0.0f;
    var = 1.0f;
    par = 1.0f;
  }

  void Copy_(const Random_cuda& cp) {
    type = cp.type;
    mean = cp.mean;
    var = cp.var;
    par = cp.par;
  }
};


#endif // CUDA_COMPILE

#endif // ta_cuda_h
