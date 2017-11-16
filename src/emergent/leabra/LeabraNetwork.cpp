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

#include "LeabraNetwork.h"
#include <LeabraCon>
#include <LeabraUnitSpec>
#include <LeabraConSpec>

#include <LeabraExtraConSpecs>
#include <LeabraExtraUnitSpecs>
#include <LeabraExtraLayerSpecs>

#include <LeabraConSpec_cpp>

#include <State_main>

#include <taProject>
#include <DataTable>
#include <Program>
#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(LeabraAvgMax);
TA_BASEFUNS_CTORS_DEFN(LeabraMarginVals);
TA_BASEFUNS_CTORS_DEFN(LeabraInhibVals);

TA_BASEFUNS_CTORS_DEFN(LeabraNetTiming);
TA_BASEFUNS_CTORS_DEFN(LeabraTimes);
TA_BASEFUNS_CTORS_DEFN(LeabraNetStats);
TA_BASEFUNS_CTORS_DEFN(LeabraNetMisc);
TA_BASEFUNS_CTORS_DEFN(LeabraNetDeep);
TA_BASEFUNS_CTORS_DEFN(RelNetinSched);
TA_BASEFUNS_CTORS_DEFN(LeabraNetwork);

using namespace std;

void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);
  net_timing.SetBaseType(&TA_LeabraNetTiming);

  Initialize_net_core();
}

NetworkState_cpp* LeabraNetwork::NewNetworkState() const {
  return new LeabraNetworkState_cpp;
}

TypeDef* LeabraNetwork::NetworkStateType() const { return &TA_LeabraNetworkState_cpp; }
TypeDef* LeabraNetwork::LayerStateType() const { return &TA_LeabraLayerState_cpp; }
TypeDef* LeabraNetwork::PrjnStateType() const { return &TA_LeabraPrjnState_cpp; }
TypeDef* LeabraNetwork::UnGpStateType() const { return &TA_LeabraUnGpState_cpp; }
TypeDef* LeabraNetwork::UnitStateType() const { return &TA_LeabraUnitState_cpp; }
TypeDef* LeabraNetwork::ConStateType() const { return &TA_LeabraConState_cpp; }


void LeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_LeabraCon;
  prjn->con_spec.SetBaseType(&TA_LeabraConSpec);
}

void LeabraNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  times.UpdateAfterEdit_NoGui();

  if(TestWarning(!lstats.off_errs && !lstats.on_errs, "UAE", "can't have both lstats.off_errs and lstats.on_errs be off (no err would be computed at all) -- turned both back on")) {
    lstats.on_errs = true;
    lstats.off_errs = true;
  }
}

void LeabraNetwork::Build() {
  inherited::Build();
  BuildLeabraThreadMem();
}

void LeabraNetwork::CheckSpecs() {
  inherited::CheckSpecs();
  CheckInhibCons();
}

void LeabraNetwork::CheckInhibCons() {
  bool inhib_cons = false;
  for(int i=0; i < n_con_specs_built; i++) {
    LEABRA_CON_SPEC_CPP* ls = (LEABRA_CON_SPEC_CPP*)net_state->GetConSpec(i);
    if(ls->inhib) {
      inhib_cons = true;
      break;
    }
  }
  // manually set in both places just to be sure!
  ((LeabraNetworkState_cpp*)net_state)->net_misc.inhib_cons = inhib_cons;
  net_misc.inhib_cons = inhib_cons;
  if(inhib_cons) {
    SetNetFlag(NETIN_PER_PRJN);	// inhib cons use per-prjn inhibition
  }
}

void LeabraNetwork::BuildLeabraThreadMem() {
  ((LeabraNetworkState_cpp*)net_state)->BuildLeabraThreadMem();
}


///////////////////////////////////////////////////////////////////////
//      General Init functions

void LeabraNetwork::SyncLayerState_Layer(Layer* ly) {
  inherited::SyncLayerState_Layer(ly);
  LeabraLayer* lay = (LeabraLayer*)ly;
  LEABRA_LAYER_STATE* lst = lay->GetLayerState(net_state);
  LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net_state);
  // these are important for saving in weights files -- keep them updated
  lst->acts_m_avg = lgpd->acts_m_avg;
  lst->acts_p_avg = lgpd->acts_p_avg;
  lst->acts_p_avg_eff = lgpd->acts_p_avg_eff;
  lay->acts_m_avg = lgpd->acts_m_avg;
  lay->acts_p_avg = lgpd->acts_p_avg;
  lay->acts_p_avg_eff = lgpd->acts_p_avg_eff;
}

void LeabraNetwork::Init_Acts() {
  NET_STATE_RUN(LeabraNetworkState, Init_Acts());
}

void LeabraNetwork::Init_Netins() {
  NET_STATE_RUN(LeabraNetworkState, Init_Netins());
}

void LeabraNetwork::DecayState(float decay) {
  NET_STATE_RUN(LeabraNetworkState, DecayState(decay));
}

void LeabraNetwork::ResetSynTR() {
  NET_STATE_RUN(LeabraNetworkState, ResetSynTR());
}


///////////////////////////////////////////////////////////////////////
//      TrialInit -- at start of trial

void LeabraNetwork::Trial_Init() {
  if(TestError(!IsBuiltIntact(), "Trial_Init",
               "Network is not built or is not intact -- must Build first")) {
    return;
  }
  unlearnable_trial = false;    //  this is main-side!
  NET_STATE_RUN(LeabraNetworkState, Trial_Init());
  SyncAllState();
  SetCurLrate();
}


void LeabraNetwork::SetCurLrate() {
  // this must be here because lrate schedule is complex obj not in State specs
  bool lrate_updtd = false;
  for(int i=0; i < n_con_specs_built; i++) {
    LeabraConSpec* ls = (LeabraConSpec*)StateConSpec(i);
    bool updt = ls->SetCurLrate((LeabraNetworkState_cpp*)net_state);
    if(updt) lrate_updtd = true;
  }

  if(lrate_updtd) {
    taMisc::Info("cur_lrate updated at epoch:", String(epoch));
// #ifdef CUDA_COMPILE
//     Cuda_UpdateConParams();     // push new params to cuda
// #endif
  }
}

///////////////////////////////////////////////////////////////////////
//      QuarterInit -- at start of settling

void LeabraNetwork::Quarter_Init() {
  if(times.cycle_qtr) {
    if(Program::step_mode && Program::cur_step_prog.ptr() != NULL) {
      if(Program::cur_step_prog->name.contains("Cycle")) {
        times.cycle_qtr = false;
        taMisc::Info("Note: turned off times.cycle_qtr in network:",
                     name, "to step by single cycles -- this will slow down precessing significantly, so turn it back on if you want maximum speed later.");
      }
    }
  }

  SyncAllState();

  NET_STATE_RUN(LeabraNetworkState, Quarter_Init());
  
  SyncAllState();
}


void LeabraNetwork::Quarter_Init_TargFlags() {
  // NOTE: this is not called by default!  Unit and Layer take care of it
  NET_STATE_RUN(LeabraNetworkState, Quarter_Init_TargFlags());
}


void LeabraNetwork::Quarter_Init_Deep() {
  if(!deep.on) return;
  if(!deep.Quarter_DeepRawPrevQtr(quarter)) return; // nobody doing it now..

  NET_STATE_RUN(LeabraNetworkState, Quarter_Init_Deep());
}


void LeabraNetwork::Compute_HardClamp() {
  // NOT called by default -- done in Quarter_Init_Unit
  NET_STATE_RUN(LeabraNetworkState, Compute_HardClamp());
}

void LeabraNetwork::ExtToComp() {
  NET_STATE_RUN(LeabraNetworkState, ExtToComp());
}

void LeabraNetwork::TargExtToComp() {
  NET_STATE_RUN(LeabraNetworkState, TargExtToComp());
}

void LeabraNetwork::NewInputData_Init() {
  NET_STATE_RUN(LeabraNetworkState, NewInputData_Init());
}

////////////////////////////////////////////////////////////////
//      Cycle_Run

void LeabraNetwork::Cycle_Run() {
  SyncAllState();
  NET_STATE_RUN(LeabraNetworkState, Cycle_Run());
  SyncAllState();
  Compute_OutputName();         // this used to be computed every cycle.. now just at end of cycle run
}


///////////////////////////////////////////////////////
//      Cycle Stage 1: netinput



///////////////////////////////////////////////////////////////////////
//      Cycle Step 3: Activation



///////////////////////////////////////////////////////////////////////
//      Cycle Stats


void LeabraNetwork::Compute_OutputName() {
  output_name = "";             // this will be updated by layer
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, layers) {
    if(lay->lesioned()) continue;
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetMainLayerSpec();
    ls->Compute_OutputName(lay, this);
  }
}


///////////////////////////////////////////////////////////////////////
//      DeepLeabra updates

void LeabraNetwork::ClearDeepActs() {
  NET_STATE_RUN(LeabraNetworkState, ClearDeepActs());
}

void LeabraNetwork::ClearMSNTrace() {
  NET_STATE_RUN(LeabraNetworkState, ClearMSNTrace());
}


///////////////////////////////////////////////////////////////////////
//      Quarter Final

void LeabraNetwork::Quarter_Final() {
  NET_STATE_RUN(LeabraNetworkState, Quarter_Final());
  SyncAllState();
}


void LeabraNetwork::Quarter_Compute_dWt() {
  NET_STATE_RUN(LeabraNetworkState, Quarter_Compute_dWt());
}


///////////////////////////////////////////////////////////////////////
//      Trial Update and Final

void LeabraNetwork::Trial_Final() {
  Compute_AbsRelNetin();
  SyncAllState();
  SyncPrjnState();
}

void LeabraNetwork::Compute_AbsRelNetin() {
  NET_STATE_RUN(LeabraNetworkState, Compute_AbsRelNetin());
}

void LeabraNetwork::Compute_AvgAbsRelNetin() {
  NET_STATE_RUN(LeabraNetworkState, Compute_AvgAbsRelNetin());  
}

///////////////////////////////////////////////////////////////////////
//      Learning


///////////////////////////////////////////////////////////////////////
//      Stats

void LeabraNetwork::LayerAvgAct(DataTable* report_table, LeabraLayerSpec* lay_spec) {
  taProject* proj = GetMyProj();
  if(!report_table) {
    report_table = proj->GetNewAnalysisDataTable(name + "_LayerAvgAct", true);
  }
  int idx;
  report_table->StructUpdate(true);
  report_table->ResetData();
  DataCol* ln = report_table->FindMakeColName("layer", idx, VT_STRING);
  DataCol* lsn = report_table->FindMakeColName("layer_spec", idx, VT_STRING);
  DataCol* actm = report_table->FindMakeColName("acts_m_avg", idx, VT_FLOAT);
  DataCol* actp = report_table->FindMakeColName("acts_p_avg", idx, VT_FLOAT);
  DataCol* init = report_table->FindMakeColName("avg_act_init", idx, VT_FLOAT);

  for(int li=0; li < n_layers_built; li++) {
    LeabraLayerState_cpp* lay = (LeabraLayerState_cpp*)net_state->GetLayerState(li);
    if(lay->lesioned()) continue;
    LeabraLayer* mlay = (LeabraLayer*)LayerFromState(lay);
    LeabraLayerSpec* ls = (LeabraLayerSpec*)mlay->GetMainLayerSpec();
    if(lay_spec != NULL) {
      if(ls != lay_spec) continue;
    }
    LEABRA_UNGP_STATE* lgpd = lay->GetLayUnGpState(net_state);
    report_table->AddBlankRow();
    ln->SetValAsString(mlay->name, -1);
    lsn->SetValAsString(ls->name, -1);
    actm->SetValAsFloat(lgpd->acts_m_avg, -1);
    actp->SetValAsFloat(lgpd->acts_p_avg, -1);
    init->SetValAsFloat(ls->avg_act.targ_init, -1);
  }
  report_table->StructUpdate(false);
  tabMisc::DelayedFunCall_gui(report_table, "BrowserSelectMe");
}

void LeabraNetwork::Set_ExtRew(bool avail, float ext_rew_val) {
  NET_STATE_RUN(LeabraNetworkState, Set_ExtRew(avail, ext_rew_val));
}

void LeabraNetwork::Compute_ExtRew() {
  NET_STATE_RUN(LeabraNetworkState, Compute_ExtRew());  
}

void LeabraNetwork::Compute_NormErr() {
  NET_STATE_RUN(LeabraNetworkState, Compute_NormErr());
}

void LeabraNetwork::Compute_CosErr() {
  NET_STATE_RUN(LeabraNetworkState, Compute_CosErr());
}

void LeabraNetwork::Compute_CosDiff() {
  NET_STATE_RUN(LeabraNetworkState, Compute_CosDiff());
}
    
void LeabraNetwork::Compute_AvgActDiff() {
  NET_STATE_RUN(LeabraNetworkState, Compute_AvgActDiff());
}

void LeabraNetwork::Compute_TrialCosDiff() {
  NET_STATE_RUN(LeabraNetworkState, Compute_TrialCosDiff());
}
    
void LeabraNetwork::Compute_ActMargin() {
  NET_STATE_RUN(LeabraNetworkState, Compute_ActMargin());
}
    
void LeabraNetwork::Compute_NetSd() {
  NET_STATE_RUN(LeabraNetworkState, Compute_NetSd());
}
    
void LeabraNetwork::Compute_HogDeadPcts() {
  NET_STATE_RUN(LeabraNetworkState, Compute_HogDeadPcts());
}
    
void LeabraNetwork::Compute_TrialStats() {
  taMisc::Warning("do not call Compute_TrialStats() anymore -- it is obsolete -- use Compute_PhaseStats or Compute_MinusStats / Compute_PlusStats for more appropriate stats computation at the right time");
}

void LeabraNetwork::Compute_PhaseStats() {
  if(quarter == 3)
    Compute_MinusStats();
  else if(quarter == 4)
    Compute_PlusStats();
}

void LeabraNetwork::Compute_MinusStats() {
  SyncAllState();
  minus_output_name = output_name; // grab and hold..

  NET_STATE_RUN(LeabraNetworkState, Compute_MinusStats());

  for(int li=0; li < n_layers_built; li++) {
    LeabraLayerState_cpp* lay = (LeabraLayerState_cpp*)net_state->GetLayerState(li);
    if(lay->lesioned()) continue;
    LeabraLayer* mlay = (LeabraLayer*)LayerFromState(lay);
    mlay->minus_output_name = mlay->output_name;
  }
  SyncAllState();
}

void LeabraNetwork::Compute_PlusStats() {
  SyncAllState();
  NET_STATE_RUN(LeabraNetworkState, Compute_PlusStats()); // do all threading at on()ce
  SyncAllState();
}

void LeabraNetwork::Compute_EpochWeights() {
  NET_STATE_RUN(LeabraNetworkState, Compute_EpochWeights());
}


/////////////////////////////////////////////////////////////////////////////////////


static String pct_val_out(float val, float sum) {
  String rval;
  rval = taMisc::FormatValue(val, 7, 3) + " " 
    + taMisc::FormatValue(100.0f * (val / sum), 7, 3);
  return rval;
}

String LeabraNetwork::TimingReport(DataTable& dt, bool print) {
  if(!HasNetFlag(BUILT)) {
    String rval = "Network not built yet!";
    if(print)
      taMisc::Info(rval);
    return rval;
  }

  int idx;
  DataCol* thc = dt.FindMakeColName("thread", idx, VT_INT);
  DataCol* stat = dt.FindMakeColName("stat", idx, VT_STRING);
  DataCol* rca = dt.FindMakeColName("run_avg", idx, VT_FLOAT);
  DataCol* rcs = dt.FindMakeColName("run_sum", idx, VT_FLOAT);
  float rescale_val = 1.0e6;        // how many microseconds
  float rescale = rescale_val;      // effective

  LeabraNetTiming* net_tm = (LeabraNetTiming*)net_timing.Peek();
  net_tm->netin.avg_used.ResetSum();
  net_tm->act.avg_used.ResetSum();
  net_tm->dwt.avg_used.ResetSum();
  net_tm->wt.avg_used.ResetSum();
  net_tm->netin_integ.avg_used.ResetSum();
  net_tm->netin_stats.avg_used.ResetSum();
  net_tm->inhib.avg_used.ResetSum();
  net_tm->act_post.avg_used.ResetSum();
  net_tm->cycstats.avg_used.ResetSum();

  float tot_time = 0.0f;
  float wait_time_avg = 0.0f;
  float wait_time_sum = 0.0f;
  for(int i=0; i<net_timing.size; i++) {
    LeabraNetTiming* tm = (LeabraNetTiming*)net_timing[i];
    if(i < n_thrs_built) {
      net_tm->netin.avg_used.IncrementAvg(tm->netin.avg_used.sum);
      net_tm->act.avg_used.IncrementAvg(tm->act.avg_used.sum);
      net_tm->dwt.avg_used.IncrementAvg(tm->dwt.avg_used.sum);
      net_tm->wt.avg_used.IncrementAvg(tm->wt.avg_used.sum);
      net_tm->netin_integ.avg_used.IncrementAvg(tm->netin_integ.avg_used.sum);
      net_tm->netin_stats.avg_used.IncrementAvg(tm->netin_stats.avg_used.sum);
      net_tm->inhib.avg_used.IncrementAvg(tm->inhib.avg_used.sum);
      net_tm->act_post.avg_used.IncrementAvg(tm->act_post.avg_used.sum);
      net_tm->cycstats.avg_used.IncrementAvg(tm->cycstats.avg_used.sum);
    }
    else {
      rescale = 1.0f;
      tot_time += tm->netin.avg_used.avg;
      tot_time += tm->act.avg_used.avg;
      tot_time += tm->dwt.avg_used.avg;
      tot_time += tm->wt.avg_used.avg;
      tot_time += tm->netin_integ.avg_used.avg;
      tot_time += tm->netin_stats.avg_used.avg;
      tot_time += tm->inhib.avg_used.avg;
      tot_time += tm->act_post.avg_used.avg;
      tot_time += tm->cycstats.avg_used.avg;
    }

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("netin_time", -1);
    rca->SetValAsFloat(tm->netin.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->netin.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("netin_integ_time", -1);
    rca->SetValAsFloat(tm->netin_integ.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->netin_integ.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("netin_stats_time", -1);
    rca->SetValAsFloat(tm->netin_stats.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->netin_stats.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("inhib_time", -1);
    rca->SetValAsFloat(tm->inhib.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->inhib.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("act_time", -1);
    rca->SetValAsFloat(tm->act.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->act.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("act_post_time", -1);
    rca->SetValAsFloat(tm->act_post.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->act_post.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("cycstats_time", -1);
    rca->SetValAsFloat(tm->cycstats.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->cycstats.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("dwt_time", -1);
    rca->SetValAsFloat(tm->dwt.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->dwt.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("wt_time", -1);
    rca->SetValAsFloat(tm->wt.avg_used.avg * rescale, -1);
    rcs->SetValAsFloat(tm->wt.avg_used.sum, -1);

    dt.AddBlankRow();
    thc->SetValAsInt(i, -1);
    stat->SetValAsString("sync_time", -1);

    // if(i < n_thrs_built) {
    //   TimeUsedHR& wt = ((NetworkThreadTask*)threads.tasks[i])->wait_time;
    //   rca->SetValAsFloat(wt.avg_used.avg * rescale, -1);
    //   rcs->SetValAsFloat(wt.avg_used.sum, -1);
    //   wait_time_avg += wt.avg_used.avg;
    //   wait_time_sum += wt.avg_used.sum;
    // }
    // else {
      wait_time_avg /= (float)n_thrs_built;
      wait_time_sum /= (float)n_thrs_built;

      rca->SetValAsFloat(wait_time_avg * rescale_val, -1);
      rcs->SetValAsFloat(wait_time_sum, -1);
    // }
  }

  String report = name + " timing report:\n";
  report << "function       time     percent \n"
         << "netin:         " << pct_val_out(net_tm->netin.avg_used.avg, tot_time) << "\n"
         << "netin_integ:   " << pct_val_out(net_tm->netin_integ.avg_used.avg, tot_time) << "\n"
         << "netin_stats:   " << pct_val_out(net_tm->netin_stats.avg_used.avg, tot_time) << "\n"
         << "inhib:         " << pct_val_out(net_tm->inhib.avg_used.avg, tot_time) << "\n"
         << "act:           " << pct_val_out(net_tm->act.avg_used.avg, tot_time) << "\n"
         << "act_post:      " << pct_val_out(net_tm->act_post.avg_used.avg, tot_time) << "\n"
         << "cycstats:      " << pct_val_out(net_tm->cycstats.avg_used.avg, tot_time) << "\n"
         << "dwt:           " << pct_val_out(net_tm->dwt.avg_used.avg, tot_time) << "\n"
         << "wt:            " << pct_val_out(net_tm->wt.avg_used.avg, tot_time) << "\n"
         << "    total:     " << taMisc::FormatValue(tot_time, 7, 3) << "\n"
         << "sync wait:     " << taMisc::FormatValue(wait_time_sum, 7, 3) << "\n"
    ;

  if(print)
    taMisc::Info(report);
  return report;
}


