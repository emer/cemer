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

#ifndef CHLConSpec_h
#define CHLConSpec_h 1

// parent includes:
#include <LeabraNetwork>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ChlSpecs);

class E_API ChlSpecs : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra Leabra CHL mixture of learning factors (hebbian vs. error-driven) specification
INHERITED(SpecMemberBase)
public:
  bool		use;	        // use CHL learning instead of standard XCAL learning -- allows easy exploration of CHL vs. XCAL
  float		hebb;		// [Default: .001] #MIN_0 amount of hebbian learning (should be relatively small, can be effective at .0001)
  float		err;		// #READ_ONLY #SHOW [Default: .999] amount of error driven learning, automatically computed to be 1-hebb
  float		savg_cor;       // #DEF_0.4:0.8 #MIN_0 #MAX_1 proportion of correction to apply to sending average activation for hebbian learning component (0=none, 1=all, .5=half, etc)
  float		savg_thresh;    // #DEF_0.001 #MIN_0 threshold of sending average activation below which learning does not occur (prevents learning when there is no input)

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(ChlSpecs);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl() override;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { };	// note: does NOT do any init -- these vals are not really subject to defaults in the usual way, so don't mess with them
};

eTypeDef_Of(CHLConSpec);

class E_API CHLConSpec : public LeabraConSpec {
  // #AKA_XCalCHLConSpec does CHL-based Leabra learning under XCAL and CAL learning framework -- sometimes CHL performs better, e.g., in the hippocampus..
INHERITED(LeabraConSpec)
public:
  ChlSpecs	chl;		// #CAT_Learning CHL learning parameters

  inline float Compute_SAvgCor(LeabraConGroup* cg, LeabraNetwork* net, int thr_no) {
    LeabraLayer* fm = (LeabraLayer*)cg->prjn->from.ptr();
    float savg = .5f + chl.savg_cor * (fm->acts_p_avg_eff - .5f);
    savg = MAX(chl.savg_thresh, savg); // keep this computed value within bounds
    return .5f / savg;
  }
  // #IGNORE compute sending average activation, corrected

  inline float	C_Compute_Hebb(const float cg_savg_cor, const float lin_wt,
			       const float ru_act, const float su_act) 
  { return ru_act * (su_act * (cg_savg_cor - lin_wt) - (1.0f - su_act) * lin_wt); }
  // #IGNORE compute Hebbian associative learning

  inline float 	C_Compute_Err_LeabraCHL(const float lin_wt,
					const float ru_act_p, const float ru_act_m,
					const float su_act_p, const float su_act_m)
  { float err = (ru_act_p * su_act_p) - (ru_act_m * su_act_m);
    if(err > 0.0f)	err *= (1.0f - lin_wt);
    else		err *= lin_wt;
    return err;
  }
  // #IGNORE compute generec error term, sigmoid case

  inline void 	C_Compute_dWt_LeabraCHL(float& dwt, const float heb, const float err)
  {  dwt += cur_lrate * (chl.err * err + chl.hebb * heb); }
  // #IGNORE combine associative and error-driven weight change, actually update dwt

  inline void Compute_dWt(ConGroup* rcg, Network* rnet, int thr_no) override {
    if(!chl.use) {
      Compute_dWt(rcg, rnet, thr_no);
      return;
    }
    LeabraNetwork* net = (LeabraNetwork*)rnet;
    if(!learn || (use_unlearnable && net->unlearnable_trial)) return;
    LeabraConGroup* cg = (LeabraConGroup*)rcg;
    LeabraUnitVars* su = (LeabraUnitVars*)cg->ThrOwnUnVars(net, thr_no);

    const float savg_cor = Compute_SAvgCor(cg, net, thr_no);
    if(((LeabraLayer*)cg->prjn->from.ptr())->acts_p.avg < chl.savg_thresh) return;

    const float su_act_m = su->act_m;
    const float su_act_p = su->act_p;
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);

    const int sz = cg->size;
    for(int i=0; i<sz; i++) {
      LeabraUnitVars* ru = (LeabraUnitVars*)cg->UnVars(i,net);
      const float lin_wt = fwts[i];
      C_Compute_dWt_LeabraCHL
        (dwts[i],
         C_Compute_Hebb(savg_cor, lin_wt, ru->act_p, su_act_p),
         C_Compute_Err_LeabraCHL(lin_wt, ru->act_p, ru->act_m, su_act_p, su_act_m));
    }
  }

  inline void	C_Compute_Weights_LeabraCHL
    (float& wt, float& dwt, float& fwt, float& swt, float& scale)
  { if(dwt != 0.0f) {
      fwt += dwt;
      swt = fwt;                // keep sync'd -- not tech necc..
      wt = scale * SigFmLinWt(fwt);
      dwt = 0.0f;
    }
  }
  // #IGNORE 

  inline void	C_Compute_Weights_LeabraCHL_slow
    (float& wt, float& dwt, float& fwt, float& swt, float& scale, int tot_trials)
  { 
    fwt += dwt;
    float eff_wt = slow_wts.swt_pct * swt + slow_wts.fwt_pct * fwt;
    float nwt = scale * SigFmLinWt(eff_wt);
    wt += slow_wts.wt_dt * (nwt - wt);
    if(slow_wts.cont_swt) {
      swt += slow_wts.slow_dt * (fwt - swt);
    }
    else {
      if(tot_trials % slow_wts.slow_tau == 0)
        swt = fwt;
    }
    dwt = 0.0f;
  }
  // #IGNORE 

  inline void Compute_Weights(ConGroup* rcg, Network* net, int thr_no) override {
    if(!chl.use) {
      Compute_Weights(rcg, net, thr_no);
      return;
    }
    if(!learn) return;

    LeabraConGroup* cg = (LeabraConGroup*)rcg;

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);
    float* fwts = cg->OwnCnVar(FWT);
    float* swts = cg->OwnCnVar(SWT);
    float* scales = cg->OwnCnVar(SCALE);

    if(slow_wts.on) {
      CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL_slow
                     (wts[i], dwts[i], fwts[i], swts[i], scales[i], net->total_trials));
    }
    else {
      CON_GROUP_LOOP(cg, C_Compute_Weights_LeabraCHL
                     (wts[i], dwts[i], fwts[i], swts[i], scales[i]));
    }
  }

  TA_SIMPLE_BASEFUNS(CHLConSpec);
protected:
  SPEC_DEFAULTS;
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init()		{ };
};

#endif // CHLConSpec_h
