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

#ifndef LearnDriverLayerSpec_h
#define LearnDriverLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(LearnDriverLayerSpec);

class E_API LearnDriverLayerSpec : public LeabraLayerSpec {
  // activity on this layer drives learning in the connections that it projects to -- used for Primary Value layers in gdPVLV architecture, to drive learning at time of PV in other layers
INHERITED(LeabraLayerSpec)
public:
  float         learn_thr;      // #DEF_0.1 Threshold value of unit activation to set the learning flag on

  virtual void  Send_LearnFlags(LeabraLayer* lay, LeabraNetwork* net);
  // send the unit LEARN flags to all units that we project to, based on unit activation thresholds -- called in PostSettle

  override void	PostSettle(LeabraLayer* lay, LeabraNetwork* net);

  TA_SIMPLE_BASEFUNS(LearnDriverLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // LearnDriverLayerSpec_h
