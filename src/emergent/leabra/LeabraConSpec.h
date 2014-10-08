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

#ifndef LeabraConSpec_h
#define LeabraConSpec_h 1

// parent includes:
#include "network_def.h"
#include <ConSpec>
#include <BaseCons_inlines>
#include <SpecMemberBase>

// member includes:
#include <Schedule>
#include <FunLookup>
#include <RecvCons>
#include <LeabraCon>
#include "ta_vector_ops.h"

// declare all other types mentioned but not required to include:
class LeabraUnit; // 
class LeabraRecvCons; // 
class LeabraLayer; // 
class LeabraSendCons; // 
class LeabraNetwork; // 
class DataTable; // 


eTypeDef_Of(WtScaleSpec);

class E_API WtScaleSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra weight scaling specification
INHERITED(SpecMemberBase)
public:
  float		abs;		// #DEF_1 #MIN_0 absolute scaling (not subject to normalization: directly multiplies weight values)
  float		rel;		// [Default: 1] #MIN_0 relative scaling that shifts balance between different projections (subject to normalization across all other projections into unit)

  inline float	NetScale() 	{ return abs * rel; }

  float	SLayActScale(const float savg, const float lay_sz, const float n_cons);
  // compute scaling factor (new version) based on sending layer activity level (savg) and number of connections and overall layer size -- uses a fixed sem_extra standard-error-of-the-mean (SEM) extra value of 2 to add to the average expected number of active connections to receive, for purposes of computing scaling factors with partial connectivity -- for 25% layer activity, binomial SEM = sqrt(p(1-p)) = .43, so 3x = 1.3 so 2 is a reasonable default

  inline float	FullScale(const float savg, const float lay_sz, const float n_cons)
  { return NetScale() * SLayActScale(savg, lay_sz, n_cons); }
  // full scaling factor -- product of above two sub-factors

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtScaleSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(XCalLearnSpec);

class E_API XCalLearnSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Leabra CtLeabra temporally eXtended Contrastive Attractor Learning (XCAL) specs
INHERITED(SpecMemberBase)
public:
  bool          raw_l_mix;      // #DEF_false does thr_l_mix specify the actual amount that the long-term average activation  (self organizing, BCM-style) contributes to the floating threshold, or instead should the default COS_DIFF model be used, where we  multiply thr_l_mix by 1 - layer.cos_diff_avg (only for HIDDEN layers -- TARGET layers automatically get l_mix = 0 -- all error-driven learning under this mechanism -- actual val is in layer.cos_diff_avg_lmix) -- cos_diff_avg computes the running average of the cos diff value between act_m and act_p (no diff is 1, max diff is 0), so the effective lmix value is high when there are large error signals (differences) in a layer, and low when error signals are low, producing a more consistent mix overall -- typically this mix tends to be stable for a given layer, so this is really just a quick shortcut for setting layer-specific mixes by hand (which the brain can do) -- cos_diff_avg_tau rate constant is in LayerSpec.decay settings
  float		thr_l_mix;	// #DEF_0.001:1.0 [0.05 max std, .01 for raw_l_mix] #MIN_0 #MAX_1 amount that long time-scale average contributes to the adaptive learning threshold -- this is the self-organizing BCM-like homeostatic component of learning -- remainder is thr_m_mix -- medium (trial-wise) time scale contribution, which reflects pure error-driven learning -- if units should have highly non-uniform distributions of activity, then this value should be set lower to reduce the homeostatic forces
  float		thr_m_mix;	// #READ_ONLY = 1 - thr_l_mix -- contribution of error-driven learning
  float         thr_max;        // #DEF_1 #MIN_1 maximum for the final computed floating threshold value that use used in the XCAL equation -- given that activations can only go to 1, the threshold should not go much higher than that -- but some amount higher can be useful for driving the weights down for units that are persistently over-active
  float		s_mix;		// #DEF_0.9 #MIN_0 #MAX_1 how much the short (plus phase) versus medium (trial) time-scale factor contributes to the synaptic activation term for learning -- s_mix just makes sure that plus-phase states are sufficiently long/important (e.g., dopamine) to drive strong positive learning to these states -- if 0 then svm term is also negated -- but vals < 1 are needed to ensure that when unit is off in plus phase (short time scale) that enough medium-phase trace remains to drive appropriate learning
  float		m_mix;		// #READ_ONLY 1-s_mix -- amount that medium time scale value contributes to synaptic activation level: see s_mix for details
  float		d_rev;		// #DEF_0.1 #MIN_0 proportional point within LTD range where magnitude reverses to go back down to zero at zero -- err-driven svm component does better with smaller values, and BCM-like mvl component does better with larger values -- 0.1 is a compromise
  float		d_thr;		// #DEF_0.0001;0.01 #MIN_0 minimum LTD threshold value below which no weight change occurs -- small default value is mainly to optimize computation for the many values close to zero associated with inactive synapses
  float		d_rev_ratio;	// #HIDDEN #READ_ONLY -(1-d_rev)/d_rev -- multiplication factor in learning rule -- builds in the minus sign!

  inline float  dWtFun(const float srval, const float thr_p) {
    float rval;
    if(srval < d_thr)
      rval = 0.0f;
    else if(srval > thr_p * d_rev)
      rval = (srval - thr_p);
    else
      rval = srval * d_rev_ratio;
    return rval;
  }
  // XCAL function for weight change -- the "check mark" function 

  inline float  dWtFun_dgain(const float srval, const float thr_p, const float d_gain) {
    float rval;
    if(srval < d_thr)
      rval = 0.0f;
    else if(srval >= thr_p)
      rval = srval - thr_p;
    else if(srval > thr_p * d_rev)
      rval = d_gain * (srval - thr_p);
    else
      rval = d_gain * srval * d_rev_ratio;
    return rval;
  }
  // version that supports a differential d_gain term -- no longer included in base case

  inline float  SymSbFun(const float wt_val) {
    return 2.0f * wt_val * (1.0f - wt_val);
  }
  // symmetric soft bounding function -- factor of 2 to equate with asymmetric sb for overall lrate at a weight value of .5 (= .5)

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  SIMPLE_COPY(XCalLearnSpec);
  TA_BASEFUNS(XCalLearnSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(WtSigSpec);

class E_API WtSigSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra sigmoidal weight function specification
INHERITED(SpecMemberBase)
public:
  float		gain;		// #DEF_1;6 #MIN_0 gain (contrast, sharpness) of the weight contrast function (1 = linear)
  float		off;		// #DEF_1 #MIN_0 offset of the function (1=centered at .5, >1=higher, <1=lower) -- 1 is standard for XCAL
  bool		dwt_norm;	// #DEF_false normalize weight changes -- this adds a significant amount of computational cost, and generally makes learning more robust, but a well-tuned network should not require it, and it causes some interference with prior learning that may not be very biologically plausible or desirable -- dwt -= (act_p / sum act_p) (sum dwt) over receiving projection

  static inline float	SigFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return (1.0f / (1.0f + powf((of * (1.0f - w)) / w, gn)));
  }
  // static version of function for implementing weight sigmodid

  static inline float	SigFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    const float pwguy = (1.0f - w) / w;
    return (1.0f / (1.0f + pwguy*pwguy*pwguy*pwguy*pwguy*pwguy));
  }
  // static version of function for implementing weight sigmodid -- for default gain = 6, offset = 1 params

  static inline float	InvFun(const float w, const float gn, const float of) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / gn) / of);
  }
  // static version of function for implementing inverse of weight sigmoid

  static inline float	InvFun61(const float w) {
    if(w <= 0.0f) return 0.0f;
    if(w >= 1.0f) return 1.0f;
    return 1.0f / (1.0f + powf((1.0f - w) / w, 1.0f / 6.0f));
  }
  // static version of function for implementing inverse of weight sigmoid -- for default gain = 6, offset = 1 params

  inline float	SigFmLinWt(const float lw) {
    return SigFun(lw, gain, off);
  }
  // get sigmoidal contrast-enhanced weight from linear weight
  
  inline float	LinFmSigWt(const float sw) {
    return InvFun(sw, gain, off);
  }
  // get linear weight from sigmoidal contrast-enhanced weight

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(WtSigSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init() { Initialize(); }
};

eTypeDef_Of(FastWtsSpec);

class E_API FastWtsSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra fast and slow weight specifications
INHERITED(SpecMemberBase)
public:
  bool          on;             // are fast weights enabled at all?  if not, then there is just one effective weight value at each synapse
  float         nofast_lrate;   // #CONDSHOW_OFF_on #DEF_2 if fast weights are NOT being used, this is an additional multiplier on the learning rate, applied in computing the cur_lrate, to make the overall effective learning rate comparable between using fast weights and not (i.e., the same basic lrate parameter can be used) -- note that the switch to cascaded average activations for avg_s and avg_m caused a reduction in effective learning rate, which this or fast weights then compensates for, allowing use of historical learning rate parameters (.02 default)
  float         decay_tau;      // #CONDSHOW_ON_on #DEF_300:6000 time constant of fast weight decay in trials (computed at Compute_Weights -- fast weights decay back to slow weight values) -- the biological tau is ~10 min, so for 100 msec trials = 10 trials per second, then ~6,000 is proper value -- more coarse-grained simulations should set this value lower (e.g., 600 for 1 trial / sec)
  float         wt_tau;         // #CONDSHOW_ON_on #DEF_10:300 time constant for how quickly the effective weight (wt) adapts to changes in the fast weight values -- the biological tau is about 20 seconds, so for 100 msec trials = 10 trials per second, the proper value is ~200 -- for 1 trial / sec it is 20
  float         fast_lrate;     // #CONDSHOW_ON_on #MIN_1 #DEF_5:10 how much more quickly do the fast weights change compared to the slow weights -- this is a multiplier on top of the standard learning rate parameter lrate -- it will show up in cur_lrate, and then gets undone for updating the slow weights

  float		decay_dt;	// #CONDSHOW_ON_on #READ_ONLY #EXPERT rate constant of decay = 1 / decay_tau
  float         wt_dt;          // #CONDSHOW_ON_on #READ_ONLY #EXPERT rate constant of wt adaptation = 1 / wt_tau
  float		slow_lrate;	// #CONDSHOW_ON_on #READ_ONLY #EXPERT 1 / fast_lrate (* lrs_mult if fast_no_lrs is in effect)

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(FastWtsSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy()	{ };
  void	Defaults_init();
};

eTypeDef_Of(CIFERLrateSpec);

class E_API CIFERLrateSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra learning rate specs for Cortical Information Flow via Extra Range theory -- effective learning rate can be enhanced for units receiving thalamic modulation vs. those without
INHERITED(SpecMemberBase)
public:
  bool          on;             // enable the CIFER differentia learning rates based on thalamic modulation signal
  float         bg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for background cortico-cortical activations -- this is the baseline learning rate prior to adding in contribution of thalamic term: lrate_eff = lrate * (bg_lrate + fg_lrate * thal)
  float         fg_lrate;       // #CONDSHOW_ON_on #MIN_0 learning rate multiplier for foreground activations, as a function of thalamic activation level: lrate_eff = lrate * (bg_lrate + fg_lrate * thal)

  String       GetTypeDecoKey() const override { return "ConSpec"; }

  TA_SIMPLE_BASEFUNS(CIFERLrateSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init();
};



eTypeDef_Of(LeabraConSpec);

class E_API LeabraConSpec : public ConSpec {
  // #STEM_BASE ##CAT_Leabra Leabra connection specs
INHERITED(ConSpec)
public:
  enum LeabraConVars {
    FWT = DWT+1,                // fast learning linear (underlying) weight value -- learns according to the lrate specified in the connection spec -- this is converted into the effective weight value, "wt", via sigmoidal contrast enhancement (wt_sig)
    SWT,                        // slow learning linear (underlying) weight value -- learns more slowly from weight changes than fast weights, and fwt decays down to swt over time
  };

  bool		inhib;		// #DEF_false #CAT_Activation makes the connection inhibitory (to g_i instead of net)
  WtScaleSpec	wt_scale;	// #CAT_Activation scale effective weight values to control the overall strength of a projection -- relative shifts balance among different projections, while absolute is a direct multipler
  bool          diff_scale_p;   // #CAT_Activation use a different wt_scale setting for the plus phase compared to the std wt_scale which is used only for the minus phase if this is checked
  WtScaleSpec	wt_scale_p;	// #CAT_Activation #CONDSHOW_ON_diff_scale_p plus phase only: scale effective weight values to control the overall strength of a projection -- relative shifts balance among different projections, while absolute is a direct multipler

  bool		learn;		// #CAT_Learning #DEF_true individual control over whether learning takes place in this connection spec -- if false, no learning will take place regardless of any other settings -- if true, learning will take place if it is enabled at the network and other relevant levels
  float		lrate;		// #CAT_Learning #DEF_0.01;0.02 #MIN_0 [0.01 for std Leabra, .02 for CtLeabra] #CONDSHOW_ON_learn learning rate -- how fast do the weights change per experience
  float		cur_lrate;	// #READ_ONLY #NO_INHERIT #SHOW #CONDSHOW_ON_learn #CAT_Learning current actual learning rate = lrate * lrate_sched current value (* 1 if no lrate_sched)
  float		lrs_mult;	// #READ_ONLY #NO_INHERIT #CAT_Learning learning rate multiplier obtained from the learning rate schedule
  Schedule	lrate_sched;	// #CAT_Learning schedule of learning rate over training epochs (NOTE: these factors (lrs_mult) multiply lrate to give the cur_lrate value)
  bool          ignore_unlearnable; // #CAT_Learning #CONDSHOW_ON_learn ignore unlearnable trials

  XCalLearnSpec	xcal;		// #CAT_Learning #CONDSHOW_ON_learn XCAL (eXtended Contrastive Attractor Learning) learning parameters
  WtSigSpec	wt_sig;		// #CAT_Learning #CONDSHOW_ON_learn sigmoidal weight function for contrast enhancement: high gain makes weights more binary & discriminative
  FastWtsSpec   fast_wts;       // #CAT_Learning #CONDSHOW_ON_learn fast weights specifications -- parameters for how fast and slowly adapting weights learning
  CIFERLrateSpec cifer;		// #CAT_Learning #CONDSHOW_ON_learn learning rate specs for Cortical Information Flow via Extra Range theory -- effective learning rate can be enhanced for units receiving thalamic modulation vs. those without

  FunLookup	wt_sig_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes wt sigmoidal fun 
  FunLookup	wt_sig_fun_inv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning computes inverse of wt sigmoidal fun
  WtSigSpec	wt_sig_fun_lst;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of wt sig parameters for which the wt_sig_fun's were computed; prevents excessive updating
  float		wt_sig_fun_res;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Learning last values of resolution parameters for which the wt_sig_fun's were computed

  inline float	SigFmLinWt(float lin_wt) { return wt_sig_fun.Eval(lin_wt);  }
  // #CAT_Learning get contrast-enhanced weight from linear weight value
  inline float	LinFmSigWt(float sig_wt) { return wt_sig_fun_inv.Eval(sig_wt); }
  // #CAT_Learning get linear weight value from contrast-enhanced sigmoidal weight value

  inline void   Init_dWt(BaseCons* cg, Unit* un, Network* net) override {
    float* dwts = cg->OwnCnVar(DWT);
    for(int i=0; i<cg->size; i++) {
      C_Init_dWt(dwts[i]);
    }
  }

  inline void Init_Weights(BaseCons* cg, Unit* ru, Network* net) override {
    Init_Weights_symflag(net);
    if(cg->prjn->spec->init_wts) return; // we don't do it, prjn does

    float* wts = cg->OwnCnVar(WT);
    float* dwts = cg->OwnCnVar(DWT);

    if(rnd.type != Random::NONE) {
      for(int i=0; i<cg->size; i++) {
        C_Init_Weight_Rnd(wts[i]);
        C_Init_dWt(dwts[i]);
      }
    }
  }

  inline void Init_Weights_post(BaseCons* cg, Unit* un, Network* net) override {
    float* wts = cg->OwnCnVar(WT);
    float* swts = cg->OwnCnVar(SWT);
    float* fwts = cg->OwnCnVar(FWT);
    for(int i=0; i<cg->size; i++) {
      swts[i] = LinFmSigWt(wts[i]); // swt, fwt are linear underlying weight values
      fwts[i] = swts[i];
    }
  }

  ///////////////////////////////////////////////////////////////
  //	Activation: Netinput -- only NetinDelta is supported

  virtual void	Compute_NetinScale(LeabraRecvCons* recv_gp, LeabraLayer* from,
                                   bool plus_phase = false);
  // #IGNORE compute recv_gp->scale_eff based on params in from layer
  inline virtual bool  IsTICtxtCon() { return false; }
  // #IGNORE is this a TI context connection (LeabraTICtctConSpec) -- optimized check for higher speed
  inline virtual bool  IsDeep5bCon() { return false; }
  // #IGNORE is this a deep5b connection (Deep5bConSpec) -- optimized check for higher speed

  inline void 	C_Send_NetinDelta_Thread(const float wt, float* send_netin_vec,
                                         const int ru_idx, const float su_act_delta_eff)
  { send_netin_vec[ru_idx] += wt * su_act_delta_eff; }
  // #IGNORE
#ifdef TA_VEC_USE
  inline void 	Send_NetinDelta_vec(LeabraSendCons* cg, const float su_act_delta_eff,
                                    float* send_netin_vec, const float* wts);
  // #IGNORE vectorized version
#endif
  inline void 	Send_NetinDelta_impl(LeabraSendCons* cg, LeabraNetwork* net,
                                     const int thread_no, const float su_act_delta,
                                     const float* wts);
  // #IGNORE implementation that uses specified weights -- typically only diff in different subclasses is the weight variables used
  inline virtual void 	Send_NetinDelta(LeabraSendCons* cg, LeabraNetwork* net,
                                   const int thread_no, const float su_act_delta);
  // #IGNORE #CAT_Activation sender-based delta-activation net input for con group (send net input to receivers) -- always goes into tmp matrix (thread_no >= 0!) and is then integrated into net through Compute_NetinInteg function on units

  // recv-based also needed for some statistics, but is NOT used for main compute code -- uses act_eq for sender act as well
  inline float 	C_Compute_Netin(const float wt, const float su_act)
  { return wt * su_act;	}
  // #IGNORE NOTE: doesn't work with spiking -- need a separate function to use act_eq for that case -- using act_eq does NOT work with scalarval etc
  inline float 	Compute_Netin(RecvCons* cg, Unit* ru, Network* net);
  // #IGNORE

  ///////////////////////////////////////////////////////////////
  //	Learning

  /////////////////////////////////////
  // CtLeabraXCAL code

  inline void 	C_Compute_dWt_CtLeabraXCAL
    (float& dwt, const float clrate, const float ru_avg_s, const float ru_avg_m,
     const float ru_avg_l, const float su_avg_s,
     const float su_avg_m, const float su_act_mult) 
  { float srs = ru_avg_s * su_avg_s;
    float srm = ru_avg_m * su_avg_m;
    float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
    float lthr = su_act_mult * ru_avg_l;
    float effthr = xcal.thr_m_mix * srm + lthr;
    effthr = MIN(effthr, xcal.thr_max);
    dwt += clrate * xcal.dWtFun(sm_mix, effthr);
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL) -- separate computation of sr averages -- trial-wise version 

  inline void 	C_Compute_dWt_CtLeabraXCAL_cosdiff
    (float& dwt, const float clrate, const float ru_avg_s, const float ru_avg_m,
     const float ru_avg_l, const float su_avg_s, const float su_avg_m,
     const float su_act_mult, const float effmmix) 
  { float srs = ru_avg_s * su_avg_s;
    float srm = ru_avg_m * su_avg_m;
    float sm_mix = xcal.s_mix * srs + xcal.m_mix * srm;
    float lthr = su_act_mult * ru_avg_l;
    float effthr = effmmix * srm + lthr;
    effthr = MIN(effthr, xcal.thr_max);
    dwt += clrate * xcal.dWtFun(sm_mix, effthr);
  }
  // #IGNORE compute temporally eXtended Contrastive Attractor Learning (XCAL) -- separate computation of sr averages -- trial-wise version, X_COS_DIFF version

#ifdef TA_VEC_USE
  inline void Compute_dWt_CtLeabraXCAL_cosdiff_vec
    (LeabraSendCons* cg, LeabraUnit* su, LeabraNetwork* net, float* dwts,
     float* avg_s, float* avg_m, float* avg_l, float* thal,
     const bool cifer_on, const float clrate, const float bg_lrate, const float fg_lrate,
     const float su_avg_s, const float su_avg_m, const float effmmix, const float su_act_mult);
  // #IGNORE vectorized version
#endif

  inline virtual void 	Compute_dWt_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                 LeabraNetwork* net);
  // #IGNORE CtLeabraXCAL weight changes

  inline void	C_Compute_Weights_CtLeabraXCAL
    (float& wt, float& dwt, float& fwt, float& swt)
  { if(dwt != 0.0f) {
      if(dwt > 0.0f)	dwt *= (1.0f - fwt);
      else		dwt *= fwt;
      fwt += dwt;
      swt = fwt;                // keep sync'd -- not tech necc..
      wt = SigFmLinWt(fwt);
    }
    dwt = 0.0f;
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- no fast wts

  inline void	C_Compute_Weights_CtLeabraXCAL_fast
    (const float decay_dt, const float wt_dt, const float slow_lrate,
     float& wt, float& dwt, float& fwt, float& swt)
  { 
    if(dwt > 0.0f)	dwt *= (1.0f - fwt);
    else		dwt *= fwt;
    fwt += dwt + decay_dt * (swt - fwt);
    swt += dwt * slow_lrate;
    float nwt = SigFmLinWt(fwt);
    wt += wt_dt * (nwt - wt);
    dwt = 0.0f;
  }
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- fast wts

#ifdef TA_VEC_USE
  inline void	Compute_Weights_CtLeabraXCAL_vec
    (LeabraSendCons* cg, float* wts, float* dwts, float* fwts, float* swts);
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- no fast wts -- vectorized
  inline void	Compute_Weights_CtLeabraXCAL_fast_vec
    (LeabraSendCons* cg, const float decay_dt, const float wt_dt, const float slow_lrate,
     float* wts, float* dwts, float* fwts, float* swts);
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule -- fast wts -- vectorized
#endif

  inline virtual void	Compute_Weights_CtLeabraXCAL(LeabraSendCons* cg, LeabraUnit* su,
                                                     LeabraNetwork* net);
  // #IGNORE overall compute weights for CtLeabraXCAL learning rule

  /////////////////////////////////////
  // Master dWt, Weights functions

  inline void	Compute_dWt(BaseCons* cg, Unit* su, Network* net) override;
  // #IGNORE overall compute delta-weights for Leabra 

  inline void	Compute_Weights(BaseCons* cg, Unit* su, Network* net) override;
  // #IGNORE overall compute weights for Leabra

  inline virtual void 	Compute_dWt_Norm(LeabraRecvCons* cg, LeabraUnit* ru,
                                         LeabraNetwork* net);
  // #IGNORE compute dwt normalization

  /////////////////////////////////////
  // 	Bias Weights

  inline void    B_Init_dWt(RecvCons* cg, Unit* ru, Network* net) override {
    C_Init_dWt(cg->OwnCn(0, BaseCons::DWT));
  }

  inline void   B_Init_Weights_post(RecvCons* cg, Unit* ru, Network* net) {
    float wt = cg->OwnCn(0, WT);
    cg->OwnCn(0, SWT) = wt; cg->OwnCn(0, FWT) = wt;
  }

  inline virtual void	B_Compute_dWt_CtLeabraXCAL(RecvCons* bias, LeabraUnit* ru,
                                                   LeabraLayer* rlay);
  // #IGNORE compute bias weight change for XCAL rule

  inline void	B_Compute_dWt(RecvCons* bias, LeabraUnit* ru, LeabraLayer* rlay);
  // #IGNORE overall compute bias delta-weights for Leabra

  inline virtual void	B_Compute_Weights(RecvCons* bias, LeabraUnit* ru);
  // #IGNORE update weights for bias connection (same for all algos)

  /////////////////////////////////////
  // General 

  inline void Compute_CopyWeights(LeabraSendCons* cg, LeabraSendCons* src_cg,
                                  LeabraNetwork* net);
  // #IGNORE copy weights from src_cg to cg -- typically used to compute synchronization of weights thought to take place during sleep -- typically in TI mode, where the Thal pathway synchronizes with the Super weights -- can be useful for any plus phase conveying weights to avoid positive feedback loop dynamics

  virtual void	Trial_Init_Specs(LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags -- e.g., set current learning rate based on schedule given epoch (or error value)

  virtual void	CreateWtSigFun();
  // #CAT_Learning create the wt_sig_fun and wt_sig_fun_inv

  virtual void	LogLrateSched(int epcs_per_step = 50, float n_steps=7);
  // #BUTTON #CAT_Learning establish a logarithmic learning rate schedule with given total number of steps (including first step at lrate) and epochs per step: numbers go down in sequence: 1, .5, .2, .1, .05, .02, .01, etc.. this is a particularly good lrate schedule for large nets on hard tasks

  virtual void	GraphWtSigFun(DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the sigmoidal weight contrast enhancement function (NULL = new data table)
  virtual void	GraphXCaldWtFun(DataTable* graph_data = NULL, float thr_p = 0.25);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the xcal dWt function for given threshold value (NULL = new data table)
  virtual void	GraphXCalSoftBoundFun(DataTable* graph_data = NULL);
  // #BUTTON #NULL_OK #NULL_TEXT_NewGraphData graph the xcal soft weight bounding function (NULL = new data table)

  virtual void 	WtScaleCvt(float slay_kwta_pct=.25, int slay_n_units=100,
                           int n_recv_cons=5, bool norm_con_n=true);
  // #BUTTON helper for converting from old wt_scale computation to new one -- enter parameters for the sending layer kwta pct (overall layer activit), number of receiving connections, and whether the norm_con_n flag was on or off (effectively always on in new version) -- it reports what the effective weight scaling was in the old version, what it is in the new version, and what you should set the wt_scale.abs to to get equivalent performance, assuming wt_scale.abs reflects what was set previously

  bool CheckConfig_RecvCons(RecvCons* cg, bool quiet=false) override;
  // check for for misc configuration settings required by different algorithms

  void	InitLinks();
  SIMPLE_COPY(LeabraConSpec);
  TA_BASEFUNS(LeabraConSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init();
};

TA_SMART_PTRS(E_API, LeabraConSpec);

#endif // LeabraConSpec_h
