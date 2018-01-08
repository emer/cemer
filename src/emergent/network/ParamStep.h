// Copyright 2016-2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef ParamStep_h
#define ParamStep_h 1

// parent includes:
#include "network_def.h"
#include <ParamSet>
#include <ParamSet_Group>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ParamStep);

class E_API ParamStep : public ParamSet {
  // ##DEF_CHILD_mbrs one step of parameters to set at given epoch of network training -- always include an epoch 0 case to initialize any changing parameters at the start of training
INHERITED(ParamSet)
public:
  int                   epoch;  // #CONTROL_PANEL_SHOW epoch at which these parameters will be set -- always include an epoch 0 case to initialize any changing parameters at the start of training -- set to -1 to disable

#ifdef __MAKETA__
  String                name;  // #READ_ONLY #SHOW name is automatically computed based on sequence name and the epoch
  ControlPanelMember_Group     mbrs;   // the members of the control panel
  ControlPanelMethod_Group     mths; // #HIDDEN
#endif

  int             GetEnabled() const override;
  String          GetTypeDecoKey() const override { return "ControlPanel"; }
  taList_impl*	  children_() override {return &mbrs;}

  virtual void  AutoName();
  // #IGNORE
  virtual void  SetEpoch(int epc);
  // for programmatic setting of epoch -- updates name but does not redo-sorting

  void InitLinks() override;
  SIMPLE_CUTLINKS(ParamStep);
  TA_BASEFUNS_SC(ParamStep);
protected:
  int           prev_epoch;     // detect if epoch changed..
  void UpdateAfterEdit_impl() override;

private:
  void Initialize();
  void Destroy() { };
};


eTypeDef_Of(ParamStep_Group);

class E_API ParamStep_Group : public ParamSet_Group {
  // list of parameter steps
INHERITED(ParamSet_Group)
public:
  TA_BASEFUNS(ParamStep_Group);
private:
  NOCOPY(ParamStep_Group)
  void  Initialize()            { SetBaseType(&TA_ParamStep); }
  void  Destroy()               { };
};

#endif // ParamStep_h
