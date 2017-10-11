// Copyright 2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

// include maketa information for these two targets
#pragma maketa_file_is_target Network
#pragma maketa_file_is_target NetworkState_cpp

class STATE_CLASS(NetworkCudaSpec) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Network parameters for NVIDA CUDA GPU implementation -- only applicable for CUDA_COMPILE binaries
  INHERITED(taOBase)
public:
  bool          on;             // #NO_SAVE is CUDA running?  this is true by default when running in an executable compiled with CUDA_COMPILE defined, and false otherwise -- can be turned off to fall back on regular C++ code, but cannot be turned on if CUDA has not been compiled
  bool          sync_units;     // #CONDSHOW_ON_on for debugging or visualization purposes -- keep the C++ host state Unit variables synchronized with the GPU device state (which is slow!) -- otherwise the GPU just runs, and updates a few key statistics (fastest)
  bool          sync_cons;      // #CONDSHOW_ON_on for debugging or visualization purposes -- keep the C++ host state Connection variables synchronized with the GPU device state (which is VERY slow!) -- otherwise the GPU just runs, and updates a few key statistics (fastest)
  int           min_threads;    // #CONDSHOW_ON_on #DEF_32 #MIN_32 minuimum number of CUDA threads to allocate per block -- each block works on all the cons in a single ConState, and the threads divide up the connections in turn -- must be a multiple of 32 (i.e., min of 32) -- actual size will be determined at run-time as function of max number of connections per connection group / cons_per_thread -- to specify an exact number of threads, just set min and max_threads to the same number
  int           max_threads;    // #CONDSHOW_ON_on #DEF_1024 #MAX_1024 maximum number of CUDA threads to allocate per block (sending group of connections) -- actual size will be determined at run-time as function of max number of connections per connection group / cons_per_thread -- for modern cards (compute capability 2.0 or higher) the max is 1024, but in general you might need to experiment to find the best performing number for your card and network, and interaction with cons_per_thread -- to specify an exact number of threads, just set min and max_threads to the same number
  int           cons_per_thread; // #CONDSHOW_ON_on #DEF_1:8 when computing number of threads to use, divide max number of connections per unit by this number, and then round to nearest multiple of 32, subject to the min and max_threads constraints
  bool          timers_on;      // #CONDSHOW_ON_on Accumulate timing information for each step of processing -- for debugging / optimizing threading
  int           n_threads;      // #READ_ONLY #SHOW computed number of threads it is actually using

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(NetworkCudaSpec);
  STATE_UAE(
#ifndef CUDA_COMPILE
  on = false;                   // can never be on!
#endif
              );
  
private:
  INLINE void	Initialize() {
#ifdef CUDA_COMPILE
    on = true;
#else
    on = false;
#endif
    sync_units = false;
    sync_cons = false;
    min_threads = 32;
    max_threads = 1024;
    cons_per_thread = 2;
    timers_on = false;
    n_threads = min_threads;
  }
};


class STATE_CLASS(NetStatsSpecs) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Network parameters for how stats are computed
INHERITED(taOBase)
public:
  bool          sse_unit_avg;   // #CAT_Statistic compute sse as average sse over units (i.e., divide by total number of target units in layer)
  bool          sse_sqrt;       // #CAT_Statistic take the square root of the SSE, producing a Euclidian distance instead of raw sse (note this cannot technically be added across trials in a linear fashion, as raw sse can)
  float         cnt_err_tol;    // #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  bool          prerr;          // #CAT_Statistic compute precision and recall error values over units, at same time as computing sum squared error (SSE)

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(NetStatsSpecs);
private:
  INLINE void	Initialize() {
    sse_unit_avg = false;
    sse_sqrt = false;
    cnt_err_tol = 0.0f;
    prerr = false;
  }

};
