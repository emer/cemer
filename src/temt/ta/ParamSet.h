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

#ifndef ParamSet_h
#define ParamSet_h 1

// parent includes:
#include <ControlPanel>
#include <ParamSetItem_Group>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ParamSet);

class TA_API ParamSet : public ControlPanel {
  // ##EXT_prm ##CAT_Display A set of member/value pairs from one or more objects in a project. Sets are saved and can be recalled to set all members in the set to the saved values
  INHERITED(ControlPanel)
  
public:
  ParamSetItem_Group      params;  // the parameters in this set
  EditMbrItem_Group       mbrs;    // #HIDDEN don't show this member of the parent class
  EditMthItem_Group       mths;    // #HIDDEN don't show this member of the parent class

  TA_SIMPLE_BASEFUNS(ParamSet);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // ParamSet_h
