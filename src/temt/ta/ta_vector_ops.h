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

#ifndef ta_vector_ops_h
#define ta_vector_ops_h 1

// this defines some useful macros and defaults for vector-based operations
// e.g., SSE / AVX, etc on x86 type processors

#ifdef __MAKETA__
// just don't define TA_VEC_USE and code should just skip it..
#define TA_VEC_SIZE 1

#else

// include Abner Fog's vector class code that makes it easy to program in 
// vector operation intrinsics
#include "vectorclass.h"

// then we figure out what code to use based on that..
#if MAX_VECTOR_SIZE >= 256
#define TA_VEC_USE 1            // use vectorization where possible
#if INSTRSET >= 8  // this is haswell-class chip or better -- use 8-way vectors
// use these to define the optimal vector size to use
#define TA_VEC_SIZE 8
#define VECF Vec8f
#define VECI Vec8i
#else
#define TA_VEC_SIZE 4
#define VECF Vec4f
#define VECI Vec4i
#endif
#endif

#endif

#endif // ta_vector_ops_h
