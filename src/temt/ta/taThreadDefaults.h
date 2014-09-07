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


taTypeDef_Of(taThreadDefaults);

class TA_API taThreadDefaults {
  // #EDIT_INLINE threading default parameters
public:
  int           cpus;           // #READ_ONLY #SHOW #NO_SAVE number of physical processors (cores) on this system -- typically you want to set n_threads equal to this, but not necessarily always -- may need to use fewer threads in some cases (almost never more)
  int           n_threads;      // #MIN_1 number of threads to actually use -- typically the number of physical processors (cores) available (see cpus), and is initialized to that.

  taThreadDefaults();
// implicit copy and assign
};

#endif // taThreadDefaults_h
