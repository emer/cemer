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

#ifndef LeabraDaNoiseConSpec_h
#define LeabraDaNoiseConSpec_h 1

// parent includes:
#include <LeabraConSpec>

// member includes:
#include <LeabraNetwork>

// declare all other types mentioned but not required to include:

eTypeDef_Of(LeabraDaNoise);

class E_API LeabraDaNoise : public SpecMemberBase {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra da-noise modulated learning as in MazzoniAndersenJordan91
INHERITED(SpecMemberBase)
public:
  float		da_noise;	// #DEF_0:1 amount to add of additional reinforcement-learning term based on unit dopamine value (dav) and TRIAL_VM_NOISE noise value, as in MazzoniAndersenJordan91: dwt = dav * (ru_act_p+noise - ru_act_p-noise) * su_act -- activation with noise (std acts as computed) minus activation without noise (specially computed given noise value) times sending activation times dopamine value -- if it does better and noise made unit more active, then make it more active next time (and so on for all other permutations)
  float		std_leabra;	// #DEF_0:1 how much of standard leabra learning to include in addition to the da_noise term

  override String       GetTypeDecoKey() const { return "ConSpec"; }

  SIMPLE_COPY(LeabraDaNoise);
  TA_BASEFUNS(LeabraDaNoise);
protected:
  SPEC_DEFAULTS;
  //  void UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(LeabraDaNoiseConSpec);

class E_API LeabraDaNoiseConSpec : public LeabraConSpec {
  // ##CAT_Leabra da-noise modulated learning as in MazzoniAndersenJordan91
INHERITED(LeabraConSpec)
public:
  LeabraDaNoise	da_noise;	// how much da_noise based learning to include relative to std leabra

  inline void C_Compute_dWt_DaNoise(LeabraCon* cn, float lin_wt, float dav,
				    float ru_act, float ru_act_nonoise,
				    float su_act) {
    float err = dav * (ru_act - ru_act_nonoise) * su_act;
    // std leabra requires separate softbounding on all terms.. see XCAL for its version
    if(lmix.err_sb) {
      if(err > 0.0f)	err *= (1.0f - lin_wt);
      else		err *= lin_wt;
    }
    cn->dwt += cur_lrate * err;
  }

  inline void Compute_dWt_DaNoise(LeabraSendCons* cg, LeabraUnit* su) {
    // compute what activation value would be if we subtract out noise -- note that
    // we don't save v_m by phase so this is necessarily on the current v_m val, assumed
    // to be plus-phase value

    for(int i=0; i<cg->size; i++) {
      LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
      LeabraUnitSpec* rus = (LeabraUnitSpec*)ru->GetUnitSpec();

      // note: with switch to sender-based, this is very expensive -- fortunately it doesn't
      // really work so we don't care.. :) -- also, not supporting act.gelin at all here
      float ru_act_nonoise = ru->Compute_ActValFmVmVal_rate((ru->v_m - ru->noise) - rus->act.thr);
      float dav = ru->dav * da_noise.da_noise;

      LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
      float lin_wt = LinFmSigWt(cn->wt);
      C_Compute_dWt_DaNoise(cn, lin_wt, dav, ru->act_p, ru_act_nonoise, su->act_p);
    }
  }

  inline void C_Compute_dWt(LeabraCon* cn, LeabraUnit*, float heb, float err) {
    float dwt = lmix.err * err + lmix.hebb * heb;
    cn->dwt += da_noise.std_leabra * cur_lrate * dwt;
  }

  override void Compute_dWt_LeabraCHL(LeabraSendCons* cg, LeabraUnit* su) {
    if(da_noise.std_leabra > 0.0f) {
      // this is a copy of the main fun, but uses above C_Compute_dWt which mults dwt
      Compute_SAvgCor(cg, su);
      if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg >= savg_cor.thresh) {
	for(int i=0; i<cg->size; i++) {
	  LeabraUnit* ru = (LeabraUnit*)cg->Un(i);
	  LeabraCon* cn = (LeabraCon*)cg->OwnCn(i);
	  float lin_wt = LinFmSigWt(cn->wt);
	  C_Compute_dWt(cn, ru, 
			C_Compute_Hebb(cn, cg, lin_wt, ru->act_p, su->act_p),
			C_Compute_Err_LeabraCHL(cn, lin_wt, ru->act_p, ru->act_m,
						su->act_p, su->act_m));  
	}
      }
    }
    if(da_noise.da_noise > 0.0f) {
      Compute_dWt_DaNoise(cg, su);
    }
  }

  // todo: add xcal version perhaps if promising..  not.. so maybe not..

  TA_SIMPLE_BASEFUNS(LeabraDaNoiseConSpec);
protected:
  SPEC_DEFAULTS;
//   void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // LeabraDaNoiseConSpec_h
