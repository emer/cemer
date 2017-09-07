// Copyright 2017, Regents of the University of Colorado,
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

#ifndef ArchivedParams_Group_h
#define ArchivedParams_Group_h 1

// parent includes:
#include <ParamSet_Group>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ArchivedParams_Group);

class TA_API ArchivedParams_Group : public ParamSet_Group {
  // #NO_EXPAND group of archived control panels, cluster runs, and param sets -- sorted by time with most recent at the top
INHERITED(ParamSet_Group)
public:
  virtual ParamSet*     NewArchive();
  // create a new param set for archiving parameters -- places it at top of list, sets date
 
  virtual ParamSet*     Archive(ControlPanel* params);
  // #BUTTON create a new archive of given set of parameters

  TA_BASEFUNS(ArchivedParams_Group);
private:
  NOCOPY(ArchivedParams_Group)
  void Initialize()  { };
  void Destroy()     { };
};

#endif // ArchivedParams_Group_h
