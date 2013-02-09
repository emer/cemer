// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef EmergentMisc_h
#define EmergentMisc_h 1

// parent includes:
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(EmergentMisc);

class E_API EmergentMisc { 
  // #NO_TOKENS miscellaneous things for Emergent
public:
  static int	Main(int& argc, const char* argv[]);
  // #IGNORE the main startup function -- this is called from bin/main.cpp
};

#endif // EmergentMisc_h
