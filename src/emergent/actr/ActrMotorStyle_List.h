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

#ifndef ActrMotorStyle_List_h
#define ActrMotorStyle_List_h 1

// parent includes:
#include <ActrMotorStyle>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrMotorStyle_List);

class E_API ActrMotorStyle_List : public taList<ActrMotorStyle> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_ActR list of ActR events
INHERITED(taList<ActrMotorStyle>)
public:

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

  TA_BASEFUNS_NOCOPY(ActrMotorStyle_List);
private:
  void Initialize();
  void Destroy()     { Reset(); }
};

#endif // ActrMotorStyle_List_h
