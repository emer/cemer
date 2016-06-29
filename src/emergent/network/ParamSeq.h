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

#ifndef ParamSeq_h
#define ParamSeq_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// member includes:
#include <ParamStep>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ParamSeq);

class E_API ParamSeq : public taNBase {
  // #STEM_BASE ##TOKENS ##CAT_Network ##DEF_CHILD_steps ##EXPAND_DEF_3 ##EXPAND_AS_GROUP A sequence of parameters to apply over training of a network -- MUST add network.Init_Epoch() call at start of epoch in appropriate program code (e.g., LeabraEpoch or EpochStart programs)
INHERITED(taNBase)
public:
  String          desc;           // description of what this parameter sequence does
  bool            on;             // #DEF_true is this parameter sequence active?
  bool            verbose;        // print out a message whenever parameters are changed -- shows up in job output, project log, and console
  ParamStep_List  steps;          // parameter steps to take


  virtual void    SetParamsAtEpoch(int epoch);
  // #BUTTON set parameters at given epoch
  
  String          GetDesc() const override { return desc; }
  int             GetEnabled() const override { return on; }
  void            SetEnabled(bool value) override { on = value; }
  String          GetTypeDecoKey() const override { return "ControlPanel"; }
  taList_impl*	  children_() override {return &steps;}
  
  TA_SIMPLE_BASEFUNS(ParamSeq);
protected:
  String          prev_name;    // for detecting name changes

  void            UpdateAfterEdit_impl();
  
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ParamSeq_h
