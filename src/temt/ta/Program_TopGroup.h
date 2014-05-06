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

#ifndef Program_TopGroup_h
#define Program_TopGroup_h 1

// parent includes:
#include <Program_Group>

// member includes:
#include <ProgBrkPt_List>

// declare all other types mentioned but not required to include:

taTypeDef_Of(Program_TopGroup);

class TA_API Program_TopGroup : public Program_Group {
  // top-level program group -- this one lives in the project -- has all the breakpoints
INHERITED(Program_Group)
public:
  ProgBrkPt_List        break_points;
  // #NO_SAVE a list of breakpoints each associated with a Program and ProgLine where the program will be stopped if the breakpoint is enabled

 void  InitLinks();
  TA_BASEFUNS_NOCOPY(Program_TopGroup);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // Program_TopGroup_h
