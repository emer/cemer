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

  virtual void          CopyActiveToSaved();  // #BUTTON
  virtual void          CopySavedToActive();  // #BUTTON

protected:
  void                  UpdateAfterEdit_impl() override;

private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // ParamSet_h
