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

#ifndef LeabraUnitSpec_h
#define LeabraUnitSpec_h 1

// parent includes:
#include "network_def.h"
#include <UnitSpec>
#include <SpecMemberBase>

// member includes:
#include <MinMaxRange>
#include <RandomSpec>
#include <Schedule>
#include <FunLookup>
#include <taMath_float>
#include <LeabraUnitVars>

// declare all other types mentioned but not required to include:
class Unit; // 
class Network; // 
class LeabraNetwork; // 
class LeabraUnit; // 
class DataTable; //
class LeabraLayer; //
class LeabraLayerSpec; //
class LeabraInhib; //

eTypeDef_Of(LeabraActFunSpec);
eTypeDef_Of(LeabraActMiscSpec);
eTypeDef_Of(SpikeFunSpec);
eTypeDef_Of(SpikeMiscSpec);
eTypeDef_Of(OptThreshSpec);
eTypeDef_Of(LeabraInitSpec);
eTypeDef_Of(LeabraDtSpec);
eTypeDef_Of(LeabraActAvgSpec);
eTypeDef_Of(LeabraAvgLSpec);
eTypeDef_Of(LeabraAvgL2Spec);
eTypeDef_Of(LeabraChannels);
eTypeDef_Of(ActAdaptSpec);
eTypeDef_Of(ShortPlastSpec);
eTypeDef_Of(SynDelaySpec);
eTypeDef_Of(DeepSpec);
eTypeDef_Of(TRCSpec);
eTypeDef_Of(DaModSpec);
eTypeDef_Of(NoiseAdaptSpec);
eTypeDef_Of(LeabraUnitSpec);
eTypeDef_Of(LeabraUnitSpec_core);

// key defines for LeabraUnitSpec_core

#define MEMBER_CLASS_SUFFIX 
#define SPEC_MEMBER_BASE SpecMemberBase

#define SPEC_CLASS_SUFFIX _core
#define SPEC_BASE UnitSpec

#include <LeabraUnitSpec_core>

class E_API LeabraUnitSpec : public LeabraUnitSpec_core {
  // Leabra unit specifications, point-neuron approximation
INHERITED(LeabraUnitSpec_core)
public:

  ///////////////////////////////////////////////////////////////////////
  //        General Init functions

  void         LoadBiasWtVal(float bwt, UnitVars* u, Network* net) override;
  void         Init_Vars(UnitVars* uv, Network* net, int thr_no) override;
  void         Init_Weights(UnitVars* uv, Network* net, int thr_no) override;
  void         Init_Acts(UnitVars* uv, Network* net, int thr_no) override;
  virtual void Init_ActAvg(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation initialize average activation values, used to control learning

  virtual void Init_Netins(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation initialize netinput computation variables (delta-based requires several intermediate variables)
  virtual void DecayState(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no,
                           float decay);
  // #CAT_Activation decay activation states towards initial values by given amount (0 = no decay, 1 = full decay)
  
  virtual void ResetSynTR(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation reset synaptic transmitter

  ///////////////////////////////////////////////////////////////////////
  //        TrialInit -- at start of trial

  virtual void Trial_Init_Specs(LeabraNetwork* net);
  // #CAT_Learning initialize specs and specs update network flags 

  virtual void Trial_Init_Unit(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation trial unit-level initialization functions: Trial_Init_SRAvg, DecayState, NoiseInit

    virtual void Trial_Init_PrvVals(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Learning save previous trial values at start of new trial -- allow values at end of trial to be valid for visualization..
    virtual void Trial_Init_SRAvg(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Learning reset the sender-receiver coproduct average -- call at start of trial
    virtual void Trial_DecayState(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation decay activation states towards initial values: at trial-level boundary
    virtual void Trial_NoiseInit(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation init trial-level noise -- ONLY called if noise_adapt.trial_fixed is set
    virtual void Trial_STP_TrialBinary_Updt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
    // #CAT_Activation calculates short term depression of units by a trial by trial basis
  ///////////////////////////////////////////////////////////////////////
  //        QuarterInit -- at start of new gamma-quarter

  virtual void Quarter_Init_Unit(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation quarter unit-level initialization functions: Init_TargFlags, Init_PrvNet, NetinScale
    virtual void Quarter_Init_TargFlags(LeabraUnitVars* uv, LeabraNetwork* net,
                                        int thr_no);
    // #CAT_Activation initialize external input flags based on phase
    virtual void Quarter_Init_PrvVals(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation update the previous values: e.g., netinput variables (prv_net_q) based on current counters
    virtual void Compute_NetinScale(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation compute net input scaling values -- call at start of quarter just to be sure

    virtual void Quarter_Init_Deep(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) { };
    // #CAT_Deep first entry point into quarter init deep for deep -- needed in subclasses (PFCUnitSpec) for updates prior to sending DeepCtxtNetin
    virtual void Send_DeepCtxtNetin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Deep send deep_raw to deep_ctxt netinput, using deepraw netin temp buffer -- not delta based
    virtual void Compute_DeepCtxt(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Deep integrate deep_ctxt from deepraw netin temp sent previously by Send_DeepCtxtNetin
    virtual void Compute_DeepStateUpdt(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Deep state update for deep leabra -- typically at start of new alpha trial --

  void  Init_InputData(UnitVars* u, Network* net, int thr_no) override;
  
  virtual void ApplyInputData_post(LeabraUnitVars* uv);
  // #CAT_Activation post-apply input data -- cache the ext value b/c it might get overwritten in transforms of the input data, as in ScalarValLayerSpec
  virtual void Compute_HardClamp(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation force units to external values provided by environment
  virtual void Compute_HardClampNoClip(LeabraUnitVars* uv, LeabraNetwork* net,
                                        int thr_no);
  // #CAT_Activation hard-clamp units without clipping values to clamp_range (use for freezing activation states for example, e.g., in second plus phase)

  virtual void ExtToComp(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation change external inputs to comparisons (remove input)
  virtual void TargExtToComp(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation change target & external inputs to comparisons (remove targ & input)


  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 1: Netinput 

  virtual void Send_NetinDelta(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation send netinput; sender based and only when act changes above a threshold -- only this delta form is supported
  virtual void Compute_NetinInteg(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation integrate newly-computed netinput delta values into a resulting complete netinput value for the network (does both excitatory and inhibitory)
    virtual void  Compute_NetinRaw(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #IGNORE called by Compute_NetinInteg -- roll up the deltas into net_raw and gi_syn values (or compute net_raw by some other means for special algorithms)
    virtual void DeepModNetin_Integ(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #IGNORE integrate deep_mod_net values
    virtual float Compute_NetinExtras(LeabraUnitVars* uv, LeabraNetwork* net,
                                      int thr_no, float& net_syn);
    // #IGNORE called by Compute_NetinInteg -- get extra excitatory net input factors to add on top of regular synapticaly-generated net inputs, passed as net_syn -- standard items include: bias weights, external soft-clamp input, TI extras (ti_ctxt, d5b_net), CIFER extras: thal (which multiplies net_syn), and da_mod (which multiplies net_syn) -- specialized algorithms can even overwrite net_syn if they need too..
    virtual float Compute_DaModNetin(LeabraUnitVars* uv, LeabraNetwork* net,
                                     int thr_no, float& net_syn);
    // #IGNORE compute the da_mod netinput extra contribution -- only called if da_mod.on is true so this doesn't need to check that flag -- subtypes can do things to change the function (e.g., D1 vs D2 effects)
    virtual void Compute_NetinInteg_Spike_e(LeabraUnitVars* uv, LeabraNetwork* net,
                                            int thr_no);
    // #IGNORE called by Compute_NetinInteg for spiking units: compute actual excitatory netin conductance value for spiking units by integrating over spike
    virtual void Compute_NetinInteg_Spike_i(LeabraUnitVars* uv, LeabraNetwork* net,
                                            int thr_no);
    // #IGNORE called by Compute_NetinInteg for spiking units: compute actual inhibitory netin conductance value for spiking units by integrating over spike

  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 2: Inhibition: these are actually called by Compute_Act to integrate
  //            inhibition computed at the layer level

  inline LeabraInhib* GetInhib(LeabraUnit* u);
  // #CAT_Activation #IGNORE get the inhib that applies to this unit (either unit group or entire layer, depending on layer spec setting)


  ///////////////////////////////////////////////////////////////////////
  //        Cycle Step 3: Activation

  // main function is Compute_Act_Rate or _Spike which calls all the various sub-functions
  // below derived types that send activation directly to special unit variables (e.g.,
  // VTAUnitSpec -> da_p) should do this here, so they can be processed in Compute_Act_Post

  // IMPORTANT: the following function is NOT called -- Network calls _Rate or _Spike
  // directly!!
  inline void  Compute_Act(UnitVars* uv, Network* net, int thr_no) override {
    if(act_fun == SPIKE)
      Compute_Act_Spike((LeabraUnitVars*)uv, (LeabraNetwork*)net, thr_no);
    else
      Compute_Act_Rate((LeabraUnitVars*)uv, (LeabraNetwork*)net, thr_no);
  }

  virtual void        Compute_ApplyInhib
    (LeabraUnitVars* uv, LeabraNetwork* net, int thr_no, LeabraLayer* lay,
     LeabraLayerSpec* lspec,  LeabraInhib* thr, float ival);
  // #IGNORE apply computed inhibition value to unit inhibitory conductance -- called by Compute_Act functions -- this is not a separate step in computation

  virtual void SaveGatingAct(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation save act_eq to act_g based on network.times.thal_gate_cycle
  
  virtual void Compute_Act_Rate(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation Rate coded activation

    virtual void Compute_DeepMod(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation compute deep_lrn and deep_mod values
    virtual void Compute_ActFun_Rate(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation compute the activation from g_e vs. threshold -- rate code functions
    virtual float Compute_ActFun_Rate_fun(float val_sub_thr);
    // #CAT_Activation raw activation function: computes an activation value from given value subtracted from its relevant threshold value

    virtual void Compute_RateCodeSpike(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation compute spiking activation (u->spike) based off of rate-code activation value

  virtual void Compute_Act_Spike(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation Spiking activation

    virtual void Compute_ActFun_Spike(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation compute the activation from membrane potential -- discrete spiking
    virtual void Compute_ClampSpike(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no,
                                    float spike_p);
    // #CAT_Activation compute spiking activation according to spike.clamp_type with given probability (typically spike.clamp_max_p * u->ext) -- includes depression and other active factors as done in Compute_ActFun_spike -- used for hard clamped inputs in spiking nets

  virtual void Compute_Vm(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation Act Step 2: compute the membrane potential from input conductances

  virtual void Compute_SelfReg_Cycle(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation Act Step 3: compute self-regulatory dynamics at the cycle time scale -- adapt, etc
    virtual void Compute_ActAdapt_Cycle(LeabraUnitVars* uv, LeabraNetwork* net, 
                                        int thr_no);
    // #CAT_Activation compute the activation-based adaptation value based on spiking and membrane potential
    virtual void Compute_ShortPlast_Cycle(LeabraUnitVars* uv, LeabraNetwork* net,
                                            int thr_no);
    // #CAT_Activation compute whole-neuron (presynaptic) short-term plasticity at the cycle level, using the stp parameters -- updates the syn_* unit variables

    virtual float Compute_Noise(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation utility fun to generate and return the noise value based on current settings -- will set unit->noise value as appropriate (generally excludes effect of noise_sched schedule)

  ///////////////////////////////////////////////////////////////////////
  //        Post Activation Step

  virtual void Compute_Act_Post(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation post-processing step after activations are computed -- calls  Compute_SRAvg and Compute_Margin by default -- this is also when any modulatory signals should be sent -- NEVER send any such signals during Compute_Act as they might be consumed by other layers during that time
    virtual void Compute_SRAvg(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Learning compute sending-receiving running activation averages (avg_ss, avg_s, avg_m) -- only for this unit (SR name is a hold-over from connection-level averaging that is no longer used) -- unit level only, used for XCAL -- called by Compute_Act_Post
    virtual void Compute_Margin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Learning compute margin status of acivation relative to layer thresholds

  ///////////////////////////////////////////////////////////////////////
  //        Cycle Stats


  ///////////////////////////////////////////////////////////////////////
  //        Deep Leabra Computations -- after superifical acts updated

  virtual void Compute_DeepRaw(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation update the deep_raw activations -- assumes checks have already been done
  virtual void Send_DeepRawNetin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Deep send deep5b netinputs through SendDeepRawConSpec connections
  virtual void DeepRawNetin_Integ(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Deep send context netinputs through SendDeepRawConSpec connections -- post processing rollup -- checks deeptest

  virtual void ClearDeepActs(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Deep clear all the deep lamina variables -- can be useful to do at discontinuities of experience

  ///////////////////////////////////////////////////////////////////////
  //        Quarter Final

  inline float GetRecAct(LeabraUnitVars* u) {
    if(act_misc.rec_nd) return u->act_nd;
    return u->act_eq;
  }
  
  virtual void Quarter_Final(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
  // #CAT_Activation record state variables after each gamma-frequency quarter-trial of processing
    virtual void Quarter_Final_RecVals(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation record state variables after each gamma-frequency quarter-trial of processing
    virtual void Compute_ActTimeAvg(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no);
    // #CAT_Activation compute time-averaged activation of unit (using act.avg_dt time constant), typically done at end of settling in Quarter_Final function

  ///////////////////////////////////////////////////////////////////////
  //        Stats

  float Compute_SSE(UnitVars* uv, Network* net, int thr_no, bool& has_targ) override;
  bool  Compute_PRerr
    (UnitVars* uv, Network* net, int thr_no, float& true_pos, float& false_pos,
     float& false_neg, float& true_neg) override;
  virtual float  Compute_NormErr(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no,
                                 bool& targ_active);
  // #CAT_Statistic compute normalized binary error (0-1 as function of bits off of act_m vs target) according to settings on the network (returns a 1 or 0) -- if (net->lstats.on_errs && act_m > .5 && targ < .5) return 1; if (net->lstats.off_errs && act_m < .5 && targ > .5) return 1; else return 0

  ///////////////////////////////////////////////////////////////////////
  //        Misc Housekeeping, non Compute functions

  virtual void        CreateNXX1Fun(LeabraActFunSpec& act_spec, FunLookup& nxx1_fl,
                              FunLookup& noise_fl);
  // #CAT_Activation create convolved gaussian and x/x+1 function lookup tables

  virtual void         BioParams
    (float norm_sec=0.001f, float norm_volt=0.1f, float volt_off=-0.1f,
     float norm_amp=1.0e-8f, float C_pF=281.0f, float gbar_l_nS=10.0f,
     float gbar_e_nS=100.0f, float gbar_i_nS=100.0f,
     float erev_l_mV=-70.0f, float erev_e_mV=0.0f, float erev_i_mV=-75.0f,
     float act_thr_mV=-50.0f, float spk_thr_mV=20.0f, float exp_slope_mV=2.0f,
     float adapt_tau_ms=144.0f, float adapt_vm_gain_nS=4.0f,
     float adapt_spk_gain_nA=0.0805);
  // #BUTTON set parameters based on biologically-based values, using normalization scaling to convert into typical Leabra standard parameters.  norm_x are normalization values to convert from SI units to normalized values (defaults are 1ms = .001 s, 100mV with -100 mV offset to bring into 0-1 range between -100..0 mV, 1e-8 amps (makes g_bar, C, etc params nice).  other defaults are based on the AdEx model of Brette & Gurstner (2005), which the SPIKE mode implements exactly with these default parameters -- last bit of name indicates the units in which this value must be provided (mV = millivolts, ms = milliseconds, pF = picofarads, nS = nanosiemens, nA = nanoamps)

  virtual void        GraphVmFun(DataTable* graph_data, float g_i = .5, float min = 0.0,
                           float max = 1.0, float incr = .01);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph membrane potential (v_m) as a function of excitatory net input (net) for given inhib conductance (g_i) (NULL = new graph data)
  virtual void        GraphActFmNetFun(DataTable* graph_data, float g_i = .5, float min = 0.0,
                                 float max = 1.0, float incr = .001, float g_e_thr = 0.5,
                                 float lin_gain = 10);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the activation function as a function of net input -- this is the direct activation function, computed relative to the g_e_thr threshold value provided -- a linear comparison with lin_gain slope is also provided for reference -- always computed as lin_gain * (net - g_e_thr) (NULL = new graph data)
  virtual void        GraphSpikeAlphaFun(DataTable* graph_data, bool force_alpha=false);
  // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the spike alpha function for conductance integration over time window given in spike parameters -- last data point is the sum over the whole window (total conductance of a single spike) -- force_alpha means use explicit alpha function even when rise=0 (otherewise it simulates actual recursive exp decay used in optimized code)
//   virtual void        GraphSLNoiseAdaptFun(DataTable* graph_data, float incr = 0.05f);
//   // #MENU_BUTTON #MENU_ON_Graph #NULL_OK #NULL_TEXT_NewGraphData graph the short and long-term noise adaptation function, which integrates both short-term and long-term performance values
  virtual void        TimeExp(int mode, int nreps=100000000);
  // #EXPERT time how long it takes to compute various forms of exp() function: mode=0 = double sum ctrl (baseline), mode=1 = std double exp(), mode=2 = taMath_double::exp_fast, mode=3 = float sum ctrl (float baseline), mode=4 = expf, mode=5 = taMath_float::exp_fast -- this is the dominant cost in spike alpha function computation, so we're interested in optimizing it..

  bool          CheckConfig_Unit(Layer* lay, bool quiet=false) override;

  String        GetToolbarName() const override { return "unit spec"; }

  void          InitLinks() override;
  SIMPLE_COPY(LeabraUnitSpec);
  TA_BASEFUNS(LeabraUnitSpec);
protected:
  SPEC_DEFAULTS;
  void         UpdateAfterEdit_impl() override;        // to set _impl sig
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void        Initialize();
  void        Destroy()                { };
  void        Defaults_init();
};

TA_SMART_PTRS(E_API, LeabraUnitSpec);

#endif // LeabraUnitSpec_h
