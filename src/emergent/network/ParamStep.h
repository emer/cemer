// Copyright, 1995-2013, Regents of the University of Colorado,
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

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ParamStep);

class E_API ParamStep : public ParamSet {
  // ##DEF_CHILD_mbrs one step of parameters to set at given epoch of network training -- always include an epoch 0 case to initialize any changing parameters at the start of training
INHERITED(ParamSet)
public:
  int                   epoch;  // #CONTROL_PANEL_SHOW epoch at which these parameters will be set -- always include an epoch 0 case to initialize any changing parameters at the start of training -- set to -1 to disable

#ifdef __MAKETA__
  EditMbrItem_Group     mbrs;   // the members of the control panel
  EditMthItem_Group     mths; // #HIDDEN
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


eTypeDef_Of(ParamStep_List);

class E_API ParamStep_List : public taList<ParamStep> {
  // #NO_TOKENS #NO_UPDATE_AFTER ##EXPAND_DEF_2 list of parameter steps
INHERITED(taList<ParamStep>)
public:
  TA_BASEFUNS(ParamStep_List);
private:
  NOCOPY(ParamStep_List)
  void  Initialize()            { SetBaseType(&TA_ParamStep); }
  void  Destroy()               { };
};

#endif // ParamStep_h
