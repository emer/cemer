// Copyright 2017, Regents of the University of Colorado,
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

#ifndef MSNUnitSpec_h
#define MSNUnitSpec_h 1

// parent includes:
#include <D1D2UnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixActSpec);

class E_API MatrixActSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra specs for matrix MSN unit activation
INHERITED(SpecMemberBase)
public:
  float         patch_shunt;    // #DEF_0.2;0.5 #MIN_0 #MAX_1 how much the patch shunt activation multiplies the dopamine values -- 0 = complete shunting, 1 = no shunting -- should be a factor < 1.0
  bool          shunt_ach;      // #DEF_true also shunt the ACh value driven from TAN units -- this prevents clearing of MSNConSpec traces -- more plausibly the patch units directly interfere with the effects of TAN's rather than through ach, but it is easier to implement with ach shunting here.
  float         out_ach_inhib;  // #DEF_0:0.3 how much does the LACK of ACh from the TAN units drive extra inhibition to output-gating Matrix units -- gi += out_ach_inhib * (1-ach) -- provides a bias for output gating on reward trials -- do NOT apply to NoGo, only Go -- this is a key param -- between 0.1-0.3 usu good -- see how much output gating happening and change accordingly
  int           n_mnt_x; // #DEF_-1 number of maintenance gating stripes along the x axis -- -1 = divide evenly between mnt and out -- if uneven numbers or only one or the other is present then you need to set this

  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixActSpec);
protected:
  SPEC_DEFAULTS;

private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};


eTypeDef_Of(MSNUnitSpec);

class E_API MSNUnitSpec : public D1D2UnitSpec {
  // Medium Spiny Neuron, which is principal cell of the striatum -- determines the dopamine receptor type and patch / matrix specialization of the neuron, and overall anatomical location (dorsal / ventral), which are then used by the MSNConSpec and other areas in determining learning and other dynamics
INHERITED(D1D2UnitSpec)
public:
  enum MatrixPatch {            // matrix (matrisome) vs. patch (striosome) structure
    MATRIX,                     // Matrix-type units, which in dorsal project to GPe/Gpi and are primarily responsible for gating events, and in ventral are responsive to CS onsets 
    PATCH,                      // Patch-type units, which in dorsal may shunt dopamine signals, and in ventral are responsible for blocking transient dopamine bursts via shunting and dipping
  };

  enum DorsalVentral {          // dorsal vs. ventral striatum
    DORSAL,                     // dorsal striatum -- projects to GPi/e and drives gating of PFC and modulation of MSN dopamine
    VENTRAL,                    // ventral striatum -- projects to VTA, LHB, ventral pallidum -- drives updating of OFC, ACC and modulation of VTA dopamine
  };

  enum GateType {               // type of gating that Matrix unit engages in
    MAINT,                      // maintenance gating
    OUT,                        // output gating
  };
  
  DAReceptor            dar;            // type of dopamine receptor: D1 vs. D2 -- also determines direct vs. indirect pathway in dorsal striatum
  MatrixPatch           matrix_patch;   // matrix vs. patch specialization
  DorsalVentral         dorsal_ventral; // dorsal vs. ventral specialization
  Valence               valence;        // #CONDSHOW_ON_dorsal_ventral:VENTRAL US valence coding of the ventral neurons
  MatrixActSpec         matrix;         // #CONDSHOW_ON_matrix_patch:MATRIX parameters for Matrix activation dynamics
  bool                  deep_mod_zero;  // #CONDSHOW_ON_matrix_patch:PATCH for modulation coming from the BLA via deep_mod_net -- when this modulation signal is below zero, does it have the ability to zero out the patch activations?  i.e., is the modulation required to enable patch firing?
  
  virtual GateType      MatrixGateType(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // get type of gating that given unit participates in
  
  void	Compute_ApplyInhib
    (LeabraUnitState_cpp* uv, LeabraNetwork* net, int thr_no, LeabraLayer* lay,
     LeabraLayerSpec* lspec, LeabraInhib* thr, float ival) override;
  // note: called in compute_act -- applies ach inhibition of output

  virtual void  Compute_PatchShunt(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // compute patch shunting of da and ach from shunt variable received in prior cycle Act_Post stage -- updated in compute_act prior to new acts
  virtual void  SaveGatingThal(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no);
  // save gating value into thal_cnt and gated activation into act_g when thal_gate indicates gating -- note 1 trial delayed from actual gating -- updated in compute_act *prior* to computing new act, so it reflects actual gating cycle activation

  void  SaveGatingAct(LeabraUnitState_cpp* uv, LeabraNetwork* net, int thr_no) override { };
  void	Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) override;
  void  Compute_DeepMod(LeabraUnitState_cpp* uv, LeabraNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(MSNUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // MSNUnitSpec_h
