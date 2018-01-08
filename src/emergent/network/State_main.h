// Co2018ght 2017-22018 Regents of the University of Colorado,
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

// establishes defines for TA-based C++-based main code compilation
// can be included multiple times to establish context

#include <MTRnd>

#define STATE_MAIN

#ifdef STATE_CUDA
#undef STATE_CUDA
#endif

#ifdef STATE_CPP
#undef STATE_CPP
#endif

// empty suffix
#ifdef STATE_SUFFIX
#undef STATE_SUFFIX
#endif
#define STATE_SUFFIX


#ifdef INLINE
#undef INLINE
#endif
#define INLINE inline

#include <State_defs>

