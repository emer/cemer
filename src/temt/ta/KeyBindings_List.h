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

#ifndef KeyBindings_List_h
#define KeyBindings_List_h 1

// parent includes:
#include <taList>
#include <KeyBindings>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(KeyBindings_List);

class TA_API KeyBindings_List : public taList<KeyBindings> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_UserData a list of KeyBindings objects which are lists of key bindings
INHERITED(taList<KeyBindings> )
public:

  TA_BASEFUNS_NOCOPY(KeyBindings_List);
private:
  void Initialize()  { SetBaseType(&TA_KeyBindings);};
  void Destroy()     { };
};

#endif // KeyBindings_List_h
