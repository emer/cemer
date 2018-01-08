// Copyright 2014-2017, Regents of the University of Colorado,
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

#ifndef ParamSet_Group_h
#define ParamSet_Group_h 1

// parent includes:
#include <ControlPanel_Group>

// member includes:

// declare all other types mentioned but not required to include:
class ParamSet; //

taTypeDef_Of(ParamSet);
taTypeDef_Of(ParamSet_Group);

class TA_API ParamSet_Group : public ControlPanel_Group {
  // a group of param sets 
  INHERITED(ControlPanel_Group)
public:
  virtual bool  ActivateParamSet(const String& set_name, bool err_not_found = true);
  // #CAT_ParamSet #AKA_LoadParamSet find given parameter set by name within this group, and call CopySavedtoActive(true) on that ParamSet if found, otherwise emit an error (if err_not_found) -- one step function for applying named parameter sets in programs

  virtual void  ActivateSaveAll();
  // #CAT_ParamSet #MENU #MENU_ON_ControlPanel #CONFIRM for each param set in this group, first Activate the saved parameter settings, and then do SaveCurrent to save the current values -- this does a refresh of the saved values for each item, and can be useful if new members were added or their types were changed
  
  virtual void  SetSavedValue(const String& member_name, const String& saved_value,
                              bool no_locked = true);
  // #CAT_ParamSet for each in group: set the saved_value of given member -- if no_locked is set, does NOT affect a member with state of LOCKED
  virtual void  SetMemberState(const String& member_name, int state);
  // #CAT_ParamSet for each in group: set the state of given member

  String        GetTypeDecoKey() const override { return "ParamSet"; }

  TA_BASEFUNS(ParamSet_Group);
private:
  NOCOPY(ParamSet_Group)
  void  Initialize() { SetBaseType(&TA_ParamSet);}
  void  Destroy()               { };
};

#endif // ParamSet_Group_h
