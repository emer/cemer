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

#ifndef PFCUnitSpec_h
#define PFCUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(PFCMaintSpec);

class E_API PFCMaintSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra specifications for maintenance in PFC, based on deep5b activations, which are in turn gated by thalamic circuit
INHERITED(SpecMemberBase)
public:
  int          maint_last_row;  // what is the last row of unit groups within the layer that supports active maintenance, where deep5b maintains itself over multiple consecutive trials or phases of thalamic gating signal being above threshold -- otherwise it always gets the current superficial activations, just like posterior cortex.  Typically the last row(s) of a layer contain the output gating stripes, which should generally not have maintenance.  Use negative numbers to count backward from the end row (-1 = last row (i.e., all stripes support maintenance), -2 = 2nd to last, etc)
  float        maint_d5b_gain;  // #DEF_0;0.8;1 for units within maint_last_row engaged in active maintenance: how much the deep5b activation drives extra net input to support maintenance in PFC neurons
  float        out_d5b_gain;  // #DEF_0;0.1;0.5;0.8;1 for units after maint_last_row, which are typically output-gating neurons, how much the deep5b activation drives extra net input to support extra activation in PFC neurons
  float        d5b_updt_tau;    // time constant for updating deep5b activations (at every phase or trial, depending on cifer.phase) where continuing maintenance is enabled (see d5b_maint_last_row) -- set to a large number to preserve initial gating information, and to a low number to allow rapid updating / drift of representations based on current superficial layer activation

  float         d5b_updt_dt;    // #READ_ONLY #EXPERT rate = 1 / tau

  String        GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(PFCMaintSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(PFCUnitSpec);

class E_API PFCUnitSpec : public LeabraUnitSpec {
  // PFC unit spec -- adds extra maintenance net input in proportion to deep5b activations, which in turn are thalamically gated -- automatically a localist, one-to-one form of maintenance -- also has special logic for updating deep5b activations, where repeated thalamic activation causes deep5b to update with the d5b_updt_tau time constant, capturing intrinsic maintenance properties of these neurons
INHERITED(LeabraUnitSpec)
public:
  PFCMaintSpec          pfc_maint; // specifications for maintenance in PFC, based on deep5b activations, which are in turn gated by thalamic circuit

  bool  ActiveMaint(LeabraUnit* u);
  // should active maintenance engaged for this unit -- looks at pfc_maint.maint_last_row compared to unit group Y position coordinate for unit

  float Compute_NetinExtras(float& net_syn, LeabraUnit* u, LeabraNetwork* net,
                            int thread_no=-1) override;

  void TI_Compute_Deep5bAct(LeabraUnit* u, LeabraNetwork* net) override;

  TA_SIMPLE_BASEFUNS(PFCUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

#endif // PFCUnitSpec_h
