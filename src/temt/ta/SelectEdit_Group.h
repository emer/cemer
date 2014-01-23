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

#ifndef SelectEdit_Group_h
#define SelectEdit_Group_h 1

// parent includes:
#include <SelectEdit>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(SelectEdit_Group);

class TA_API SelectEdit_Group : public taGroup<SelectEdit> {
  // ##CAT_Display ##EXPAND_DEF_1 group of select edit dialog objects
INHERITED(taGroup<SelectEdit>)
public:
  virtual void  AutoEdit();

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "SelectEdit"; }

  TA_BASEFUNS(SelectEdit_Group);
private:
  NOCOPY(SelectEdit_Group)
  void  Initialize() { SetBaseType(&TA_SelectEdit);}
  void  Destroy()               { };
};

#endif // SelectEdit_Group_h
