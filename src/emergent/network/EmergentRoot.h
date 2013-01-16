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

#ifndef EmergentRoot_h
#define EmergentRoot_h 1

// parent includes:
#include "network_def.h"
#include <taRootBase>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(EmergentRoot);

class EMERGENT_API EmergentRoot : public taRootBase {
  // structural root of object hierarchy
INHERITED(taRootBase)
public:

  override void	About();
#ifdef TA_GUI
  taBase*	Browse(const char* init_path=NULL);
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
#endif

  TA_BASEFUNS_NOCOPY(EmergentRoot);
private:
  void 	Initialize();
  void 	Destroy() { };
};


#endif // EmergentRoot_h
