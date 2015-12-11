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

#ifndef GPiSoftMaxLayerSpec_h
#define GPiSoftMaxLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GPiSoftMaxSpec);

class E_API GPiSoftMaxSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra soft max specs
INHERITED(SpecMemberBase)
public:
  int           n_gate;         // how many units to select for gating on each trial
  float         temp;           // temperature for softmax
  float         rnd_p;          // probability of selecting completely at random (uniform random choice across all stripes) -- combines epsilon greedy with softmax..
  
  String        GetTypeDecoKey() const override { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(GPiSoftMaxSpec);
protected:
  SPEC_DEFAULTS;
  //  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};

eTypeDef_Of(GPiSoftMaxLayerSpec);

class E_API GPiSoftMaxLayerSpec : public LeabraLayerSpec {
  // performs soft-max action selection for a GPi layer -- add this to GPi Layer using GPiInvUnitSpec
INHERITED(LeabraLayerSpec)
public:
  GPiSoftMaxSpec        soft_max; // soft max specs

  virtual void Compute_SoftMax(LeabraLayer* lay, LeabraNetwork* net);
  // compute the softmax
  
  void	Compute_CycleStats_Pre(LeabraLayer* lay, LeabraNetwork* net) override;

 TA_SIMPLE_BASEFUNS(GPiSoftMaxLayerSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // GPiSoftMaxLayerSpec_h
