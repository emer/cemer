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

#ifndef ScaleRange_List_h
#define ScaleRange_List_h 1

// parent includes:
#include "network_def.h"
#include <taList>
#include <ScaleRange>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ScaleRange_List);

class E_API ScaleRange_List : public taList<ScaleRange> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of ScaleRange objects
INHERITED(taList<ScaleRange>)
public:
  void                  Initialize() {SetBaseType(&TA_ScaleRange);}
  void                  Destroy() {};
  TA_BASEFUNS_NOCOPY(ScaleRange_List);
};


#endif // ScaleRange_List_h
