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

#ifndef VTAUnitSpec_h
#define VTAUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:
class LeabraLayer; //

eTypeDef_Of(gdPVLVDaSpec);

class E_API gdPVLVDaSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for gdPVLV da parameters
INHERITED(SpecMemberBase)
public:
  float         da_gain;        // #DEF_0:2 #MIN_0 multiplier for dopamine values
  float         tonic_da;       // #DEF_0 set a tonic 'dopamine' (DA) level (offset to add to da values)
  float         burst_gain;     // #DEF_1 gain on bursts from PPTg
  float         dip_gain;       // #DEF_1 gain on dips from LHbRMTg
  float         pv_gain;        // #DEF_1 gain on positive PV component of total phasic DA signal (net after subtracting VSPatchIndir (PVi) shunt signal)
  float         pv_thr;         // #DEF_0.1 threshold on pv max act for setting pv_detected
  float         pvi_gain;       // #DEF_1 gain on VSPatchIndir (PVi) shunt signal - higher pvi_gain == more shunting
  float         vsp_thr;        // #DEF_0.1 threshold on VS Patch Indir max act for setting pv_detected

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(gdPVLVDaSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};


eTypeDef_Of(LVBlockSpec);

class E_API LVBlockSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for how PV and dip signals block LV signals in dopamine
INHERITED(SpecMemberBase)
public:
  float         pos_pv;         // #DEF_2 down-regulate LV by factor of: (1 - pos_pv * pv) for positive pv signals (e.g., from LHA etc) -- the larger this value, the more LV is blocked -- if it is 0, then there is no LV block at all -- net actual block is 1 - sum over both sources of block
  float         dip;            // #DEF_2 down-regulate LV by factor of: (1 - dip * lhb_rmtg) for da dip signals coming from the LHbRMTg sytem -- the larger this value, the more LV is blocked -- if it is 0, then there is no LV block at all -- net actual block is 1 - sum over both sources of block
  bool          rec_data;       // record all the internal computations in user data on the VTA layer

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(LVBlockSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init() { Initialize(); }
};


eTypeDef_Of(VTAUnitSpec);

class E_API VTAUnitSpec : public LeabraUnitSpec {
  // Models the Ventral Tegmental Area: computes gdPVLV dopamine (Da) signal from PPTg and LHbRMTg input projections, and also a direct input from a positive valence PV layer, and shunting inhibition from VS Patch Indirect -- uses deep_qtr to determine when to send dopamine values
INHERITED(LeabraUnitSpec)
public:
  gdPVLVDaSpec    da;             // parameters for the pvlv da computation
  LVBlockSpec     lv_block;       // how LV signals are blocked by PV and LHbRMTg dip signals -- there are good reasons for these signals to block LV, because they reflect a stronger overall signal about outcomes, compared to the more "speculative" LV signal

  virtual void  Send_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the da value to sending projections: every cycle
  virtual void  Compute_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // compute the da value based on recv projections from PPTg and LHbRMTg

  virtual bool  GetRecvLayers(LeabraUnit* u,
                              LeabraLayer*& pptg_lay, LeabraLayer*& lhb_lay,
                              LeabraLayer*& pospv_lay, LeabraLayer*& vspatch_lay);
  // get the recv layers..

  void	Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override { };
  void	Compute_ApplyInhib
    (LeabraUnitVars* uv, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
     LeabraInhib* thr, float ival) override { };
  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  void 	Compute_dWt(UnitVars* u, Network* net, int thr_no) override { };
  void	Compute_dWt_Norm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) { };
  void	Compute_Weights(UnitVars* u, Network* net, int thr_no) override { };

  void  HelpConfig();   // #BUTTON get help message for configuring this spec
  bool  CheckConfig_Unit(Unit* u, bool quiet=false);

  TA_SIMPLE_BASEFUNS(VTAUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init() { Initialize(); }
};

#endif // VTAUnitSpec_h
