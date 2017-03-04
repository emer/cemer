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
  // ##EXT_prm ##CAT_Display A set of member/value pairs from one or more objects in a project. Sets are saved and can be recalled to set all members in the set to the saved values. You can modify the current values, test your model and then click "Copy Active To Saved" to store the values for later recall.
  INHERITED(ControlPanel)
  
public:
  TA_SIMPLE_BASEFUNS(ParamSet);

  virtual void          CopyActiveToSaved(bool info_msg = false);
  // #BUTTON #ARGC_0 copy the current active (live) values on the objects to the saved values
  virtual void          CopySavedToActive(bool info_msg = false);
  // #BUTTON #ARGC_0 copy the saved_value values to be active (live) values on the objects
  virtual bool          ActiveEqualsSaved(String member_name); 
  // test whether active and saved versions of a member are the same or different

  virtual void          CopyActiveToSaved_item(int itm_idx);
  // #IGNORE copy the current active (live) values on the objects to the saved values
  virtual void          CopySavedToActive_item(int itm_idx);
  // #IGNORE copy the previously-saved values to be active (live) values on the objects

  virtual void          ComparePeers(ParamSet* param_set);
  // #BUTTON #DYN12N calls taProject::ParamSetComparePeers to populate a table that compares this param_sets member values with its peers values - this data does not update - call again if you edit any of the param_sets!

  
protected:
  void                  UpdateAfterEdit_impl() override;

private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // ParamSet_h
