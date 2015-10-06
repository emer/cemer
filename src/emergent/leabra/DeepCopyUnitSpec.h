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

#ifndef DeepCopyUnitSpec_h
#define DeepCopyUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(DeepCopyUnitSpec);

class E_API DeepCopyUnitSpec : public LeabraUnitSpec {
  // #AKA_Deep5bUnitSpec units that continuously copy deep layer activation values from a one-to-one receiving projection -- should typically just be used for visualization convenience -- use SendDeepRawConSpec or SendDeepNormConSpec to send these variables directly from a given layer (although these only operate at restricted times)
INHERITED(LeabraUnitSpec)
public:
  enum DeepCopyVar {            // variable to copy from
    DEEP_RAW,                   // deep_raw
    DEEP_MOD,                   // deep_mod
    DEEP_LRN,                   // deep_lrn
    DEEP_CTXT,                  // deep_ctxt
  };

  DeepCopyVar   deep_var;        // which variable to copy from
  
  virtual void Compute_ActFmSource(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // set current act of deep unit to sending super unit activation

  void	Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_ApplyInhib(LeabraUnitVars* u, LeabraNetwork* net, int thr_no,
                           LeabraLayerSpec* lspec, LeabraInhib* thr, float ival)
    override { };
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  void 	Compute_dWt(UnitVars* u, Network* net, int thr_no) override { };
  void	Compute_dWt_Norm(UnitVars* u, Network* net, int thr_no) { };
  void	Compute_Weights(UnitVars* u, Network* net, int thr_no) override { };

  bool  CheckConfig_Unit(Unit* u, bool quiet=false) override;

  TA_SIMPLE_BASEFUNS(DeepCopyUnitSpec);
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init();
};

#endif // DeepCopyUnitSpec_h
