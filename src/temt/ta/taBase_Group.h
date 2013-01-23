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

#ifndef taBase_Group_h
#define taBase_Group_h 1

// parent includes:
#include <taBase>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


// define default base group to not keep tokens
TypeDef_Of(taBase_Group);

class TA_API taBase_Group : public taGroup<taBase> {
  // #NO_TOKENS #NO_UPDATE_AFTER ##EXPAND_DEF_0 group of objects
INHERITED(taGroup<taBase>)
public:
  void  Initialize()            { SetBaseType(&TA_taBase); }
  void  Destroy()               { };
  TA_BASEFUNS_NOCOPY(taBase_Group);
};

#endif // taBase_Group_h
