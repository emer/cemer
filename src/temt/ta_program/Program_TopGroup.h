// Co2018ght 2014-2017, Regents of the University of Colorado,
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
  bool                  DiffCompare(taBase* cmp_obj) override;
  // #BUTTON #MENU_ON_Svn #CAT_File compare this group of programs against another, pulling up a diff browser where the differences can be viewed, and then any changes selected to generate a Pach for each project that will convert from one into the other -- this is very robust and operates on separate major groups of objects -- strongly recommended for recording and communicating changes between projects

  ProgBrkPt_List        break_points;
  // #NO_SAVE a list of breakpoints each associated with a Program and ProgLine where the program will be stopped if the breakpoint is enabled

  Program_Group         tmp_progs;
  // #HIDDEN #TREE_HIDDEN #NO_SAVE #CAT_IGNORE temporary programs, e.g., for diff compare in program libs

  void  InitLinks() override;
  TA_BASEFUNS_NOCOPY(Program_TopGroup);
protected:
  void UpdateAfterEdit_impl() override;

private:
  void Initialize();
  void Destroy()     { };
};

#endif // Program_TopGroup_h
