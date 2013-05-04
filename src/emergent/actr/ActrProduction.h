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

#ifndef ActrProduction_h
#define ActrProduction_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ActrCondition_List>
#include <ActrAction_List>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrProduction);

class E_API ActrProduction : public taNBase {
  // ##INSTANCE ##CAT_ActR ##SCOPE_ActrModel a production that matches conditions and produces actions
INHERITED(taNBase)
public:
  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this production -- what does it do?
  float                 util;  // #READ_ONLY #SHOW current utility of this production, updated from rewards
  float                 rew;   // reward value associated with the firing of this production
  ActrCondition_List    conds; // conditions that must be matched to fire this production
  ActrAction_List       acts;  // actions that this production causes when it fires

  float         Compute_Util(float rewval, float lrate) {
    util += lrate * (rewval - util);
    return util;
  }
  // #CAT_ActR update the utility of this production from given reward value and learning rate
 
  override String       GetDesc() const {return desc;}
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  TA_SIMPLE_BASEFUNS(ActrProduction);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrProduction_h
