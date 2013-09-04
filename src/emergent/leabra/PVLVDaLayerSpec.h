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

#ifndef PVLVDaLayerSpec_h
#define PVLVDaLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PVLVDaSpec);

class E_API PVLVDaSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for PVLV da parameters
INHERITED(SpecMemberBase)
public:
  float         da_gain;        // #DEF_0:2 #MIN_0 multiplier for dopamine values
  float         tonic_da;       // #DEF_0 set a tonic 'dopamine' (DA) level (offset to add to da values)
  float         pv_gain;        // #DEF_1;0.1;0.5 #MIN_0 extra gain modulation of PV generated DA -- it can be much larger in general than lv so sometimes it is useful to turn it down (e.g., in new version of PBWM)
  bool          add_pv_lv;      // #DEF_false for cases where reward is expected/delivered, add PV and LV dopamine signals (otherwise, only use PV signal)

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(PVLVDaSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};

eTypeDef_Of(PVLVDaLayerSpec);

class E_API PVLVDaLayerSpec : public LeabraLayerSpec {
  // computes PVLV dopamine (Da) signal: typically if(ER), da = PVe-PVi, else LVe - LVi
INHERITED(LeabraLayerSpec)
public:
  PVLVDaSpec    da;             // parameters for the pvlv da computation

  virtual void  Send_Da(LeabraLayer* lay, LeabraNetwork* net);
  // send the da value to sending projections: every cycle
  virtual void  Compute_Da(LeabraLayer* lay, LeabraNetwork* net);
  // compute the da value based on recv projections: every cycle in 1+ phases (delta version)

  override void Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net);
  override void Compute_NetinStats(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) { };
  override void Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net);

  // never learn
  override bool Compute_SRAvg_Test(LeabraLayer* lay, LeabraNetwork* net)  { return false; }
  override bool Compute_dWt_FirstPlus_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }
  override bool Compute_dWt_Nothing_Test(LeabraLayer* lay, LeabraNetwork* net) { return false; }

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(PVLVDaLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()               { };
  void  Defaults_init() { Initialize(); }
};

#endif // PVLVDaLayerSpec_h
