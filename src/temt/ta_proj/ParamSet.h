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

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ParamSet);

class TA_API ParamSet : public ControlPanel {
  // ##EXT_prm ##CAT_Display A set of member/value pairs from one or more objects in a project, like a ControlPanel, but values can be saved (SaveCurrent) and re-activated (Activate) so you can easily swap in/out different sets of parameter values.  New groups of ParamSet's have a Master/Clone config: first ParamSet is Master, rest are Clones and members are auto-updated in Clones from changesin Master
  INHERITED(ControlPanel)
public:
  bool                  last_activated;
  // #READ_ONLY #SHOW this param set was the last one activated, within its owning group
    
  inline void           SaveCurrent(bool info_msg = false)
  { CopyActiveToSaved(info_msg); }
  // #CAT_ParamSet #BUTTON #ARGC_0 copy the current active (live) values on the objects to the saved values
  inline void           Activate(bool info_msg = false)
  { CopySavedToActive(info_msg); }
  // #CAT_ParamSet #BUTTON #ARGC_0 copy the saved_value values to be active (live) values on the objects
  
  void                  AllStable() override  { inherited::AllStable(); }
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #MENU_SEP_BEFORE #CONFIRM #BUTTON set all members to be STABLE -- you can then selectively mark a subset as ACTIVE -- in general don't want too many active fields
  void                  AllLocked() override { inherited::AllLocked(); }
  // #CAT_CtrlPanel #MENU #MENU_ON_ControlPanel #CONFIRM #BUTTON set all members to be LOCKED (not subject to editing of any sort) -- you can then selectively mark a subset as STABLE (editable but not recorded) or ACTIVE -- use this to lock down a very stable set of parameters and prevent further editing
  
  virtual void          CopyActiveToSaved(bool info_msg = false);
  // #CAT_ParamSet #ARGC_0 copy the current active (live) values on the objects to the saved values
  virtual void          CopySavedToActive(bool info_msg = false);
  // #CAT_ParamSet #ARGC_0 copy the saved_value values to be active (live) values on the objects

  virtual bool          ActiveEqualsSaved(String member_name); 
  // #CAT_ParamSet test whether active and saved versions of a member are the same or different

  virtual void          CopyActiveToSaved_item(int itm_idx);
  // #CAT_ParamSet #IGNORE copy the current active (live) values on the objects to the saved values
  virtual void          CopySavedToActive_item(int itm_idx);
  // #CAT_ParamSet #IGNORE copy the previously-saved values to be active (live) values on the objects

  virtual void          ComparePeers(ParamSet* param_set);
  // #CAT_ParamSet #BUTTON #DYN12N calls taProject::ParamSetComparePeers to populate a table that compares this param_sets member values with its peers values - this data does not update - call again if you edit any of the param_sets!

  virtual void          ResetLastActivated();
  // #CAT_ParamSet reset the last_activated flags for all elements within this ParamSet's group
  
  int                   GetSpecialState() const override;

  TA_SIMPLE_BASEFUNS(ParamSet);
protected:
  void                  UpdateAfterEdit_impl() override;

private:
  void Initialize();
  void Destroy()     { };
};

#endif // ParamSet_h
