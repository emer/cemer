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

#include "PFCUnitSpec.h"
#include <LeabraNetwork>
#include <taProject>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PFCMiscSpec);
TA_BASEFUNS_CTORS_DEFN(PFCUnitSpec);

void PFCMiscSpec::Initialize() {
  out_gate = false;
  Defaults_init();
}

void PFCMiscSpec::Defaults_init() {
  s_mnt_gain = 0.25f;
  clear = 0.5f;
  out_clear = 0.5f;
  mnt_thal = 1.0f;
  use_dyn = true;
  max_mnt = 100;
  out_mnt = 1;
  gate_thr = 0.1f;
}

void PFCUnitSpec::Initialize() {
  Defaults_init();
}

void PFCUnitSpec::Defaults_init() {
  DefaultDynTable();
  SetUnique("deep", true);
  deep_raw_qtr = Q2_Q4;
  deep.on = true;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f; // todo??
  deep.mod_min = 1.0f;
}

void  PFCUnitSpec::FormatDynTable() {
  DataCol* dc;

  int idx;
  if((idx = dyn_table.FindColNameIdx("rise_dt")) >= 0)
    dyn_table.RemoveCol(idx);
  if((idx = dyn_table.FindColNameIdx("decay_dt")) >= 0)
    dyn_table.RemoveCol(idx);
  
  dc = dyn_table.FindMakeCol("name", VT_STRING);
  dc->desc = "name for this dynamic profile";

  dc = dyn_table.FindMakeCol("desc", VT_STRING);
  dc->desc = "description of this dynamic profile";

  dc = dyn_table.FindMakeCol("init", VT_FLOAT);
  dc->desc = "initial value at point when gating starts";

  dc = dyn_table.FindMakeCol("rise_tau", VT_FLOAT);
  dc->desc = "time constant for linear rise in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first";

  dc = dyn_table.FindMakeCol("decay_tau", VT_FLOAT);
  dc->desc = "time constant for linear decay in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first";

  dyn_table.EnforceRows(n_dyns);
}

void  PFCUnitSpec::DefaultDynTable(float std_tau) {
  if(pfc.out_gate)
    n_dyns = 1;
  else
    n_dyns = 5; 
  FormatDynTable();

  int cur = 0;
  SetDynVal("maint_flat", DYN_NAME, cur);
  SetDynVal("maintained, flat stable sustained activation", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  if(pfc.out_gate)
    return;
  
  cur++;
  SetDynVal("phasic", DYN_NAME, cur);
  SetDynVal("immediate phasic response to gating event", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(1.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_rise", DYN_NAME, cur);
  SetDynVal("maintained, rising value over time", DYN_DESC, cur);
  SetDynVal(0.1f, DYN_INIT, cur);
  SetDynVal(std_tau, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_decay", DYN_NAME, cur);
  SetDynVal("maintained, decaying value over time", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(std_tau, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_updn", DYN_NAME, cur);
  SetDynVal("maintained, rising then falling alue over time", DYN_DESC, cur);
  SetDynVal(0.1f, DYN_INIT, cur);
  SetDynVal(.5f * std_tau, DYN_RISE_TAU, cur);
  SetDynVal(std_tau, DYN_DECAY_TAU, cur);

  UpdtDynTable();
}

void  PFCUnitSpec::UpdtDynTable() {
  dyn_table.StructUpdate(true);
  FormatDynTable();
  for(int i=0; i<dyn_table.rows; i++) {
    float init = GetDynVal(DYN_INIT, i);
    if(init == 0.0f) {          // init must be a minimum val -- deep_raw = 0 is non-gated
      SetDynVal(.1f, DYN_INIT, i);
    }
  }
  dyn_table.StructUpdate(false);
}

float PFCUnitSpec::UpdtDynVal(int row, float time_step) {
  float init_val = GetDynVal(DYN_INIT, row);
  float rise_tau = GetDynVal(DYN_RISE_TAU, row);
  float decay_tau = GetDynVal(DYN_DECAY_TAU, row);
  float val = init_val;
  if(rise_tau > 0 && decay_tau > 0) {
    if(time_step >= rise_tau) {
      val = 1.0f - ((time_step - rise_tau) / decay_tau);
    }
    else {
      val = init_val + (1.0f - init_val) * (time_step / rise_tau);
    }
  }
  else if(rise_tau > 0) {
    val = init_val + (1.0f - init_val) * (time_step / rise_tau);
  }
  else if(decay_tau > 0) {
    val = init_val - init_val * (time_step / decay_tau);
  }
  if(val > 1.0f) val = 1.0f;
  if(val < 0.001f) val = 0.001f; // non-zero indicates gated..
  return val;
}

void PFCUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdtDynTable();
}

float PFCUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  float net_ex = inherited::Compute_NetinExtras(u, net, thr_no, net_syn);
  if(deep.IsSuper()) {
    if(u->thal_cnt == 0.0f) { // just gated!
      // special logic here to maintain if nothing coming in
      LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
      LeabraLayer* lay = (LeabraLayer*)un->own_lay();
      LeabraUnGpData* ugd = lay->UnGpDataUn(un);
      if(ugd->netin.max < 0.05f) { // weak..
        net_ex += pfc.s_mnt_gain * u->deep_mod_net;
      }
    }
    else if(u->thal_cnt > 0.0f) { // do it!
      net_ex += pfc.s_mnt_gain * u->deep_mod_net;
    }
  }
  return net_ex;
}


void PFCUnitSpec::Compute_PFCGating(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.IsSuper() || !Quarter_DeepRawNextQtr(net->quarter))
    return;

  const int cyc_per_qtr = net->times.quarter;
  const int qtr_cyc = net->cycle - net->quarter * cyc_per_qtr; // quarters into this cyc
  const int half_cyc = cyc_per_qtr / 2;

  if(pfc.out_gate) {
    if(qtr_cyc != half_cyc - 1) // out gate goes first so maint can override clear!
      return;
  }
  else {
    if(qtr_cyc != half_cyc)
      return;
  }

  // first, test for over-duration maintenance -- allow for active gating to override
  if(pfc.out_gate) {
    if(u->thal_cnt >= pfc.out_mnt) {
      u->thal_cnt = -1.0f;
    }
  }
  else {                        // maint gating
    if(u->thal_cnt >= pfc.max_mnt) {
      u->thal_cnt = -1.0f;
    }
  }
  
  if(u->thal > pfc.gate_thr) { // new gating signal -- reset counter
    if(u->thal_cnt >= 1.0f) { // already maintaining
      if(pfc.clear > 0.0f) {
        DecayState(u, net, thr_no, pfc.clear);
      }
    }
    u->thal_cnt = 0.0f;        // this is the "just gated" signal
    if(pfc.out_gate) {         // time to clear out maint
      ClearOtherMaint(u, net, thr_no); 
    }
  }
}

void PFCUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Rate(u, net, thr_no);
  Compute_PFCGating(u, net, thr_no);
}

void PFCUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Spike(u, net, thr_no);
  Compute_PFCGating(u, net, thr_no);
}


void PFCUnitSpec::Compute_DeepRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;

  // NOTE: only super does anything here -- this is where the gating is detected and updated
  if(!deep.IsSuper()) return;
  
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();

  float thr_cmp = lay->acts_raw.avg +
    deep.raw_thr_rel * (lay->acts_raw.max - lay->acts_raw.avg);
  thr_cmp = MAX(thr_cmp, deep.raw_thr_abs);
  float draw = 0.0f;
  if(u->act_raw >= thr_cmp) {
    draw = u->act_raw;
  }

  float thal_eff = 0.0f;
  if(u->thal_cnt >= 0.0f) {     // gated or maintaining
    thal_eff = MAX(u->thal, pfc.mnt_thal);
  }
  
  u->deep_raw = thal_eff * draw;
}

void PFCUnitSpec::GetThalCntFromSuper(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // look for layer we recv a deep context con from, that is also a PFCUnitSpec SUPER
  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(!cs->IsDeepCtxtCon()) continue;
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraUnitSpec* fmus = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(!fmus->InheritsFrom(&TA_PFCUnitSpec)) continue;
    PFCUnitSpec* pfcus = (PFCUnitSpec*)fmus;
    if(!pfcus->deep.IsSuper() || recv_gp->size == 0) continue;
    LeabraUnitVars* suv = (LeabraUnitVars*)recv_gp->UnVars(0,net); // get first connection
    u->thal_cnt = suv->thal_cnt; // all super guys in same stripe should have same thal_cnt
  }
}  


void PFCUnitSpec::Quarter_Init_Deep(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  // this is first entry point for Quarter_Init -- because of parallel, need to
  // sync with super here and then do rest of updating in next step
  if(deep.IsDeep()) {
    GetThalCntFromSuper(u, net, thr_no); // sync us up with super
  }
}

void PFCUnitSpec::Send_DeepCtxtNetin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  if(u->thal_cnt < 0.0f) {      // not maintaining or just gated -- zero!
    u->deep_raw = 0.0f;
    u->deep_ctxt = 0.0f;
    u->thal_cnt -= 1.0f;        // decrement count -- optional
  }
  else {
    u->thal_cnt += 1.0f;          // we are maintaining, update count for all
  }

  if(deep.IsSuper()) {
    if(u->thal_cnt < 0.0f) return; // optimization: don't send if not maintaining!
  }

  inherited::Send_DeepCtxtNetin(u, net, thr_no);
}


void PFCUnitSpec::Compute_DeepStateUpdt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  if(pfc.use_dyn && deep.IsDeep() && u->thal_cnt >= 0) { // update dynamics!
    LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
    LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
    int unidx = un->UnitGpUnIdx();
    int dyn_row = unidx % n_dyns;
    if(u->thal_cnt <= 1.0f) { // first gating -- should only ever be 1.0 here..
      u->misc_1 = u->deep_ctxt; // record gating ctxt
      u->deep_ctxt *= GetDynVal(DYN_INIT, dyn_row);
    }
    else {
      u->deep_ctxt = u->misc_1 * UpdtDynVal(dyn_row, (u->thal_cnt-1.0f));
    }
  }

  inherited::Compute_DeepStateUpdt(u, net, thr_no);
}

void PFCUnitSpec::ClearOtherMaint(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraLayer* lay = (LeabraLayer*)un->own_lay();
  LeabraUnGpData* ugd = lay->UnGpDataUn(un);
  if(ugd->acts_eq.max < 0.1f)   // we can't clear anyone if nobody in our group is active!
    return;
  
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
    if(!cs->IsMarkerCon()) continue;
    for(int j=0;j<send_gp->size; j++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)send_gp->UnVars(j,net);
      if(uv->thal_cnt >= 1.0f) { // important!  only for established maint, not just gated!
        uv->thal_cnt = -1.0f; // terminate!
        if(pfc.out_clear > 0.0f) {
          DecayState(uv, net, thr_no, pfc.out_clear); // note: thr_no is WRONG here! but shouldn't matter..
        }
      }
    }
  }
}

void PFCUnitSpec::GraphPFCDyns(DataTable* graph_data, int n_trials) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_PFCDyns", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* rw = graph_data->FindMakeColName("trial", idx, VT_FLOAT);

  for(int nd=0; nd < n_dyns; nd++) {
    graph_data->FindMakeColName("deep_raw_" + String(nd), idx, VT_FLOAT);
  }

  for(int x = 0; x <= n_trials; x++) {
    graph_data->AddBlankRow();
    rw->SetValAsFloat(x, -1);
    for(int nd=0; nd < n_dyns; nd++) {
      float nw;
      if(x == 0) {
        nw = GetDynVal(DYN_INIT, nd);
      }
      else {
        nw = UpdtDynVal(nd, (float)x);
      }
      graph_data->SetValAsFloat(nw, nd+1, -1);
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

