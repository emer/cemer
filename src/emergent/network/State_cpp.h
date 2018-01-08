// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#include <MTRnd>

#define STATE_CPP

#ifdef STATE_CUDA
#undef STATE_CUDA
#endif

#ifdef STATE_MAIN
#undef STATE_MAIN
#endif

#ifdef STATE_SUFFIX
#undef STATE_SUFFIX
#endif
#define STATE_SUFFIX _cpp

#ifdef INLINE
#undef INLINE
#undef INIMPL
#endif
#define INLINE inline
#define INIMPL

#include <State_defs>

// make sure we have the base types define -- this only needs to happen once

#ifndef State_base_cpp_h
#define State_base_cpp_h

#include <State_base>

#endif
