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

#ifndef taThreadDefaults_h
#define taThreadDefaults_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taThreadDefaults {
  // #EDIT_INLINE threading default parameters
public:
  int           cpus;           // #READ_ONLY #SHOW #NO_SAVE number of physical processors (cores) on this system -- typically you want to set n_threads equal to this, but not necessarily always -- may need to use fewer threads in some cases (almost never more)
  int           n_threads;      // #MIN_1 number of threads to actually use -- typically the number of physical processors (cores) available (see cpus), and is initialized to that.
  float         alloc_pct;      // #MIN_0 #MAX_1 proportion (0-1) of total to process by pre-allocating a set of computations to a given thread -- the remainder of the load is allocated dynamically through a nibbling mechanism, where each thread takes a nibble_chunk at a time until the job is done.  current experience is that this should be no greater than .2, unless the load is quite large, as there is a high degree of variability in thread start times, so the automatic load balancing of nibbling is important, and it has very little additional overhead.
  int           nibble_chunk;   // #MIN_1 how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.  roughly 8 seems good in many cases
  float         compute_thr;    // #MIN_0 #MAX_1 threshold value for amount of computation in a given function to actually deploy on threads, as opposed to just running it on main thread -- value is normalized (0-1) with 1 being the most computationally intensive task, and 0 being the least -- as with min_units, it may not be worth it to parallelize very lightweight computations.  See Thread_Params page on emergent wiki for relevant comparison values.
  int           min_units;      // #MIN_1 minimum number of computational units (e.g., network units) to apply parallel threading to -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.

  taThreadDefaults();
// implicit copy and assign
};

#endif // taThreadDefaults_h
