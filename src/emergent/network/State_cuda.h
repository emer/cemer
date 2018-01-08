// Copyright 2017-2017, Regents of the University of Colorado,
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

// establishes defines for TA-independent C++-based core code compilation
// can be included multiple times to establish context

#define STATE_CUDA

#ifdef STATE_CPP
#undef STATE_CPP
#endif

#ifdef STATE_MAIN
#undef STATE_MAIN
#endif

#ifdef STATE_SUFFIX
#undef STATE_SUFFIX
#endif

#define STATE_SUFFIX _cuda

#ifdef INLINE
#undef INLINE
#undef INIMPL
#endif

#define INLINE __device__ inline
#define INIMPL __device__ 

#include <ta_cuda>

#include <State_defs>

// make sure we have the base types define -- this only needs to happen once

#ifndef State_base_cuda_h
#define State_base_cuda_h

#include <State_base>

#endif
