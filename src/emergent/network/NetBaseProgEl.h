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

#ifndef NetBaseProgEl_h
#define NetBaseProgEl_h 1

// parent includes:
#include "network_def.h"
#include <ProgEl>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(NetBaseProgEl);

class EMERGENT_API NetBaseProgEl: public ProgEl { 
  // #VIRT_BASE #NO_INSTANCE base type for network-oriented prog els (filter function, etc)
INHERITED(ProgEl)
public:
  // Signature must match that of the item_filter_fun typedef.
  static bool		NetProgVarFilter(void* base, void* var); // Network* progvar filter -- only shows Network* items -- use in ITEM_FILTER comment directive
  TA_BASEFUNS_NOCOPY(NetBaseProgEl);
private:
 void	Initialize() { };
 void	Destroy() { };
};


#endif // NetBaseProgEl_h
