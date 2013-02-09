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

#ifndef ActAvgHebbConSpec_h
#define ActAvgHebbConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActAvgHebbMixSpec);

class E_API ActAvgHebbMixSpec : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra mixture of average activation hebbian learning and regular hebbian learning (on current act value)
INHERITED(SpecMemberBase)
public:
  float		act_avg;	// what proportion of average activation to include in hebbian receiving unit activation learning term
  float		cur_act;	// #READ_ONLY #SHOW 1.0 - act_avg -- proportion of current activation for hebbian learning

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(ActAvgHebbMixSpec);
  TA_BASEFUNS(ActAvgHebbMixSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(ActAvgHebbConSpec);

class E_API ActAvgHebbConSpec : public LeabraConSpec {
  // hebbian learning that includes a proportion of average activation over time, in addition to standard current unit activation;  produces a trace-based learning effect for learning over trajectories -- only for Leabra_CHL
INHERITED(LeabraConSpec)
public:
  ActAvgHebbMixSpec	act_avg_hebb; // mixture of current and average activations to use in hebbian learning

 inline float C_Compute_Hebb(LeabraCon* cn, LeabraSendCons* cg, float lin_wt,
			     float ru_act, float su_act, float ru_avg_act)
  {
    // wt is negative in linear form, so using opposite sign of usual here
    float eff_ru_act = act_avg_hebb.act_avg * ru_avg_act + act_avg_hebb.cur_act * ru_act;
    return eff_ru_act * (su_act * (cg->savg_cor - lin_wt) - (1.0f - su_act) * lin_wt);
  }

  // this computes weight changes based on sender at time t-1
  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    Compute_SAvgCor(cg, su);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < savg_cor.thresh) return;

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      float lin_wt = LinFmSigWt(cn->wt);
      C_Compute_dWt(cn, ru, 
		    C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p, ru->act_avg),
		    C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m, su->act_p, su->act_m));  
    }
  }

  TA_SIMPLE_BASEFUNS(ActAvgHebbConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // ActAvgHebbConSpec_h
