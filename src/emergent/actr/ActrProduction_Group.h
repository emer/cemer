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

#ifndef ActrProduction_Group_h
#define ActrProduction_Group_h 1

// parent includes:
#include <taGroup>
#include <ActrProduction>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrProduction_Group);

class E_API ActrProduction_Group : public taGroup<ActrProduction> {
  // ##CAT_ActR ##NO_TOKENS group of ACT-R productions
INHERITED(taGroup<ActrProduction>)
public:

  TA_BASEFUNS_NOCOPY(ActrProduction_Group);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrProduction_Group_h
