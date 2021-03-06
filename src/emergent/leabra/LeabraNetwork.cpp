// Copyright 2013-2018, Regents of the University of Colorado,
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

TA_BASEFUNS_CTORS_DEFN(LeabraMarginVals);
TA_BASEFUNS_CTORS_DEFN(LeabraTimes);
TA_BASEFUNS_CTORS_DEFN(LeabraNetStats);
TA_BASEFUNS_CTORS_DEFN(LeabraNetMisc);
TA_BASEFUNS_CTORS_DEFN(LeabraNetDeep);
TA_BASEFUNS_CTORS_DEFN(RelNetinSched);
TA_BASEFUNS_CTORS_DEFN(LeabraNetwork);

using namespace std;

void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);
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
  lst->CopyFromUnGpState(lgpd);
  lay->CopyFromUnGpState(lgpd);
}

void LeabraNetwork::Init_Acts() {
  if(TestError(!IsBuiltIntact(), "Init_Acts",
               "Network is not built or is not intact -- must Build first")) {
    return;
  }
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
  SyncPrjnState();              // netin scale updated
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
  MonitorData();
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


void LeabraNetwork::TimingReportInitNames() {
  inherited::TimingReportInitNames(); // allocs
  net_timer_names[LeabraNetworkState_cpp::NT_NETIN_STATS] = "Net_Input_Stats";
  net_timer_names[LeabraNetworkState_cpp::NT_INHIB] = "Inhibition";
  net_timer_names[LeabraNetworkState_cpp::NT_ACT_POST] = "Act_post  ";
  net_timer_names[LeabraNetworkState_cpp::NT_CYCLE_STATS] = "Cycle_Stats";
}  


String LeabraNetwork::AllWtScales() {
  String str;
  for(int li=0; li < n_layers_built; li++) {
    LeabraLayerState_cpp* lay = (LeabraLayerState_cpp*)net_state->GetLayerState(li);
    if(lay->lesioned()) continue;
    LeabraLayer* mlay = (LeabraLayer*)LayerFromState(lay);
    str += "\nLayer: " + mlay->GetName() + "\n";
    for(int i = 0; i < mlay->projections.size; i++) {
      Projection* prjn = mlay->projections[i];
      if(!prjn->MainIsActive())
        continue;
      LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
      str += "\t" + prjn->name + "\t\tAbs:\t" + String(cs->wt_scale.abs) + "\tRel:\t" + String(cs->wt_scale.rel) + "\n";
    } 
  }
  return str;
}

