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

#ifndef ActrAction_List_h
#define ActrAction_List_h 1

// parent includes:
#include <ActrAction>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrAction_List);

class E_API ActrAction_List : public taList<ActrAction> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_ActR list of ActR chunks
INHERITED(taList<ActrAction>)
public:

  TA_BASEFUNS_NOCOPY(ActrAction_List);
private:
  void Initialize();
  void Destroy()     { Reset(); }
};

#endif // ActrAction_List_h
