// Copyright 2017-2018, Regents of the University of Colorado,
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

// contains cpp non-inline (INIMPL) functions from _cpp.h

#include "LeabraNetworkState_cpp.h"

#include <LeabraConSpec_cpp>
#include <LeabraUnitSpec_cpp>
#include <LeabraLayerSpec_cpp>
#include <LeabraBiasSpec_cpp>

#include <LeabraExtraConSpecs_cpp>
#include <LeabraExtraUnitSpecs_cpp>
#include <LeabraExtraLayerSpecs_cpp>

#include <State_cpp>

#include "LeabraNetworkState_core.cpp"

using namespace std;

void LeabraNetworkState_cpp::Init_Acts() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Init_Acts_Thr);

  Init_Acts_Layers();
}

void LeabraNetworkState_cpp::Init_Netins() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Init_Netins_Thr);
}

void LeabraNetworkState_cpp::DecayState(float decay) {
  tmp_arg1 = decay;
  NET_THREAD_CALL(LeabraNetworkState_cpp::DecayState_Thr);
}

void LeabraNetworkState_cpp::ResetSynTR() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::ResetSynTR_Thr);
}

void LeabraNetworkState_cpp::Trial_Init_Unit() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Trial_Init_Unit_Thr);
}

void LeabraNetworkState_cpp::Trial_Init() {
  unlearnable_trial = false;
  Trial_Init_Counters();
  Trial_Init_Specs();

  Trial_Init_Unit(); // performs following at unit-level
  //  Trial_Init_SRAvg();
  //  Trial_DecayState();
  //  Trial_NoiseInit(); 

  Trial_Init_Layers();
}

void LeabraNetworkState_cpp::Quarter_Init() {

  Quarter_Init_Counters();
  Quarter_Init_Layers();
  // Compute_HardClamp_Layers();    // need layer hard clamp flag before Init_Unit
  Quarter_Init_Unit();           // do chunk of following unit-level functions:
//   Quarter_Init_TargFlags();
//   Compute_HardClamp();        // clamp all hard-clamped input acts

  Quarter_Init_Deep();          // in previous version it was done before rescaling..
  Compute_NetinScale();       // compute net scaling

  // also, super important to do this AFTER the Quarter_Init_Unit call so net is still
  // around for functions that use the previous value of it
  // NOTE: *everyone* has to init netins when scales change across quarters, because any existing netin has already been weighted at the previous scaled -- no way to rescale that aggregate -- just have to start over..
  if((phase == LeabraNetworkState_cpp::PLUS_PHASE && net_misc.diff_scale_p) ||
     (quarter == 1 && net_misc.diff_scale_q1)) {
    Init_Netins();
  }

  Quarter_Init_Layer_Post();
}

void LeabraNetworkState_cpp::Quarter_Init_Unit() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Quarter_Init_Unit_Thr);
}

void LeabraNetworkState_cpp::Quarter_Init_TargFlags() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  NET_THREAD_CALL(LeabraNetworkState_cpp::Quarter_Init_TargFlags_Thr);

  Quarter_Init_TargFlags_Layers();
}

void LeabraNetworkState_cpp::Quarter_Init_Deep() {
  if(!deep.on) return;
  if(!deep.Quarter_DeepRawPrevQtr(quarter)) return; // nobody doing it now..

  NET_THREAD_CALL(LeabraNetworkState_cpp::Quarter_Init_Deep_Thr);
}

void LeabraNetworkState_cpp::Compute_HardClamp() {
  // NOT called by default -- done in Quarter_Init_Unit
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_HardClamp_Thr);

  Compute_HardClamp_Layers();
}

void LeabraNetworkState_cpp::ExtToComp() {
  ExtToComp_Layers();

  NET_THREAD_CALL(LeabraNetworkState_cpp::ExtToComp_Thr);
}

void LeabraNetworkState_cpp::TargExtToComp() {
  TargExtToComp_Layers();

  NET_THREAD_CALL(LeabraNetworkState_cpp::TargExtToComp_Thr);
}

void LeabraNetworkState_cpp::NewInputData_Init() {
  Quarter_Init_Layers();
  Quarter_Init_TargFlags();
  Compute_HardClamp();
  Quarter_Init_Layer_Post();
}

void LeabraNetworkState_cpp::Cycle_Run() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Cycle_Run_Thr);
}

void LeabraNetworkState_cpp::ClearDeepActs() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::ClearDeepActs_Thr);
}

void LeabraNetworkState_cpp::ClearMSNTrace() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::ClearMSNTrace_Thr);
}

void LeabraNetworkState_cpp::Quarter_Final() {
  Quarter_Final_Pre();
  Quarter_Final_Layers(); // note: this MUST come before unit to get acts_p for cos_diff
  NET_THREAD_CALL(LeabraNetworkState_cpp::Quarter_Final_Unit_Thr);
  if(quarter == 3) {
    Compute_CosDiff_Agg();      // aggregate from Unit_Thr
  }
  Quarter_Compute_dWt();
  Quarter_Final_Counters();
}

void LeabraNetworkState_cpp::Quarter_Compute_dWt() {
  if(train_mode == TEST)
    return;

  Compute_dWt();                // always call -- let units sort it out..
}

void LeabraNetworkState_cpp::Trial_Final() {
  Compute_AbsRelNetin();
}

void LeabraNetworkState_cpp::Compute_dWt() {
  Compute_dWt_Layer_pre();
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_dWt_Thr);
}

void LeabraNetworkState_cpp::Compute_Weights() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_Weights_Thr);
  
  if(net_misc.wt_bal && (total_trials % times.wt_bal_int == 0)) {
    NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_WtBal_Thr);
    if(net_misc.wt_bal && lstats.wt_bal) {
      Compute_WtBalStats();
    }
  }
}

void LeabraNetworkState_cpp::Compute_NormErr() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_NormErr_Thr);
  Compute_NormErr_Agg();
}

float LeabraNetworkState_cpp::Compute_CosErr() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_CosErr_Thr);
  return Compute_CosErr_Agg();
}

float LeabraNetworkState_cpp::Compute_CosDiff() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_CosDiff_Thr);
  return Compute_CosDiff_Agg();
}
    
float LeabraNetworkState_cpp::Compute_AvgActDiff() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_AvgActDiff_Thr);
  return Compute_AvgActDiff_Agg();
}

float LeabraNetworkState_cpp::Compute_TrialCosDiff() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_TrialCosDiff_Thr);
  return Compute_TrialCosDiff_Agg();
}

void LeabraNetworkState_cpp::Compute_ActMargin() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_ActMargin_Thr);
  Compute_ActMargin_Agg();
}
    
float LeabraNetworkState_cpp::Compute_NetSd() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_NetSd_Thr);
  return Compute_NetSd_Agg();
}
    
void LeabraNetworkState_cpp::Compute_HogDeadPcts() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_HogDeadPcts_Thr);
  Compute_HogDeadPcts_Agg();
}
    
void LeabraNetworkState_cpp::Compute_PhaseStats() {
  if(quarter == 3)
    Compute_MinusStats();
  else if(quarter == 4)
    Compute_PlusStats();
}

void LeabraNetworkState_cpp::Compute_MinusStats() {
  Compute_RTCycles_Agg();
  Compute_NetSd(); // todo: combine as in plus if more than one
}

void LeabraNetworkState_cpp::Compute_PlusStats() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_PlusStats_Thr); // do all threading at once
  Compute_PlusStats_Agg();
  Compute_ExtRew();
}

void LeabraNetworkState_cpp::Compute_EpochStats() {
  Compute_EpochWeights();
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
  Compute_AvgNormErr();
  Compute_AvgCosErr();
  Compute_AvgCosDiff();
  Compute_AvgAvgActDiff();
  Compute_AvgTrialCosDiff();
  Compute_AvgNetSd();
  Compute_AvgExtRew();
  Compute_AvgSendPct();
  Compute_AvgAbsRelNetin();
  Compute_HogDeadPcts();
}

void LeabraNetworkState_cpp::Compute_EpochWeights() {
  NET_THREAD_CALL(LeabraNetworkState_cpp::Compute_EpochWeights_Thr);
}


void LeabraNetworkState_cpp::BuildLeabraThreadMem() {
  AllocLeabraStatsMem();
  
  NET_THREAD_CALL(LeabraNetworkState_cpp::InitLeabraThreadMem_Thr);
}


int LeabraNetworkState_cpp::LayerLoadWeights_LayerVars
(istream& strm, LayerState_cpp* play, WtSaveFormat fmt, bool quiet) {

  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)play;
  
  while(true) {
    int c = strm.peek();          // check for <U for UnGp
    if(c == '<') {
      strm.get();
      c = strm.peek();
      if(c == 'U') {
        strm.unget();           // < goes back
        break;                  // done
      }
      else { // got a SAVE_WTS member
        strm.unget();           // < goes back
        String tag;
        String val;
        read_tag(strm, tag, val);
        float fval = (float)val;
        if(tag == "acts_m_avg")
          lay->acts_m_avg = fval;
        else if(tag == "acts_p_avg")
          lay->acts_p_avg = fval;
        else if(tag == "acts_p_avg")
          lay->acts_p_avg = fval;
        else if(tag == "acts_p_avg_eff")
          lay->acts_p_avg_eff = fval;
        else if(tag == "adapt_gi")
          lay->adapt_gi = fval;
        else if(tag == "margin_low_thr")
          lay->margin.low_thr = fval;
        else if(tag == "margin_med_thr")
          lay->margin.med_thr = fval;
        else if(tag == "margin_hi_thr")
          lay->margin.hi_thr = fval;
        else if(tag == "margin_low_avg")
          lay->margin.low_avg = fval;
        else if(tag == "margin_med_avg")
          lay->margin.med_avg = fval;
        else if(tag == "margin_hi_avg")
          lay->margin.hi_avg = fval;
        else {
#ifdef DEBUG
          StateError("LayerLoadWeights_LayerVals: member not found:", tag, "value:", val);
#endif          
        }
      }
    }
    else {
      break;                  // some other badness
    }
  }

  // these vars actually live on the unit group
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);
  lgpd->acts_m_avg = lay->acts_m_avg;
  lgpd->acts_p_avg = lay->acts_p_avg;
  lgpd->acts_p_avg_eff = lay->acts_p_avg_eff;
  
  return TAG_END;
}

void LeabraNetworkState_cpp::LayerSaveWeights_LayerVars
(ostream& strm, LayerState_cpp* play, WtSaveFormat fmt) {

  LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)play;
  // these vars actually live on the unit group
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(this);
  lay->acts_m_avg     = lgpd->acts_m_avg;
  lay->acts_p_avg     = lgpd->acts_p_avg;
  lay->acts_p_avg_eff = lgpd->acts_p_avg_eff;
  
  strm
    << "<acts_m_avg " << lay->acts_m_avg << ">\n"
    << "<acts_p_avg " << lay->acts_p_avg << ">\n"
    << "<acts_p_avg_eff " << lay->acts_p_avg_eff << ">\n"
    << "<adapt_gi " << lay->adapt_gi << ">\n"
    << "<margin_low_thr " << lay->margin.low_thr << ">\n"
    << "<margin_med_thr " << lay->margin.med_thr << ">\n"
    << "<margin_hi_thr "  << lay->margin.hi_thr << ">\n"
    << "<margin_low_avg " << lay->margin.low_avg << ">\n"
    << "<margin_med_avg " << lay->margin.med_avg << ">\n"
    << "<margin_hi_avg " << lay->margin.hi_avg << ">\n";
}

