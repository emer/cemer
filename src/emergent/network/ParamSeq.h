// Co2018ght 2016-2017, Regents of the University of Colorado,
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
  // #STEM_BASE ##TOKENS ##CAT_Network ##DEF_CHILD_steps A sequence of parameters to apply over training of a network -- MUST add network.Init_Epoch() call at start of epoch in appropriate program code (e.g., LeabraEpoch or EpochStart programs)
INHERITED(taNBase)
public:
  String          desc;           // description of what this parameter sequence does
  bool            on;             // #DEF_true is this parameter sequence active?
  bool            verbose;        // print out a message whenever parameters are changed -- shows up in job output, project log, and console
  int             last_epoch_set; // #READ_ONLY #SHOW #NO_SAVE last epoch when parameters were updated
  ParamStep_Group steps;          // parameter steps to take


  virtual bool    SetParamsAtEpoch(int epoch);
  // #BUTTON set parameters at given epoch -- returns true if some params were changed
  virtual void    MakeEpochSteps(int epcs_per_step = 50, int n_steps=4,
                                 bool copy_first = true);
  // #BUTTON make regular epoch steps, optionally copying the parameters present in the first step if it is already present
  virtual void    LinearInterp();
  // #BUTTON #CONFIRM linearly interpolate the values between the first and last steps as currently present -- only for single numeric parameters, and the params must be in the same position of the mbrs list across all steps -- to use, setup the saved values for the first and last steps and then run this function
  
  String          GetDesc() const override { return desc; }
  int             GetEnabled() const override { return on; }
  void            SetEnabled(bool value) override { on = value; }
  String          GetTypeDecoKey() const override { return "ControlPanel"; }
  taList_impl*	  children_() override {return &steps;}
  
  TA_SIMPLE_BASEFUNS(ParamSeq);
protected:
  String          prev_name;    // for detecting name changes

  void            UpdateAfterEdit_impl() override;
  
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ParamSeq_h
