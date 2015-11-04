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
  gate_thr = 0.1f;
  out_mnt = 1;
  max_mnt = 100;
  s_mnt_gain = 0.2f;
  mnt_thal = 0.5f;
  use_dyn = true;
}

void PFCUnitSpec::Initialize() {
  Defaults_init();
}

void PFCUnitSpec::Defaults_init() {
  InitDynTable();
  SetUnique("deep", true);
  deep_raw_qtr = Q2_Q4;
  deep.on = true;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f; // todo??
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

void  PFCUnitSpec::InitDynTable() {
  if(pfc.out_gate)
    n_dyns = 1;
  else
    n_dyns = 5;
  FormatDynTable();

  int cur = 0;
  SetDynVal("phasic", DYN_NAME, cur);
  SetDynVal("immediate phasic response to gating event", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(1.0f, DYN_DECAY_TAU, cur);

  if(pfc.out_gate)
    return;
  
  cur++;
  SetDynVal("maint_flat", DYN_NAME, cur);
  SetDynVal("maintained, flat stable sustained activation", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_rise", DYN_NAME, cur);
  SetDynVal("maintained, rising value over time", DYN_DESC, cur);
  SetDynVal(0.1f, DYN_INIT, cur);
  SetDynVal(10.0f, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_decay", DYN_NAME, cur);
  SetDynVal("maintained, decaying value over time", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(10.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_updn", DYN_NAME, cur);
  SetDynVal("maintained, rising then falling alue over time", DYN_DESC, cur);
  SetDynVal(0.1f, DYN_INIT, cur);
  SetDynVal(5.0f, DYN_RISE_TAU, cur);
  SetDynVal(10.0f, DYN_DECAY_TAU, cur);

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
    net_ex += pfc.s_mnt_gain * u->deep_mod_net;
  }
  return net_ex;
}

void PFCUnitSpec::Quarter_Init_Deep(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  // this is first entry point for Quarter_Init, so we need to do out-go-clear,
  // and check if we are over max maint limits

  if(deep.IsSuper()) {
    if(pfc.out_gate) {
      if(u->thal_cnt >= pfc.out_mnt) {
        u->thal = 0.0f;
        u->thal_cnt = -1.0f;
        if(pfc.out_mnt == 1)    // this must happen here for 1
          ClearOtherMaint(u, net, thr_no); // do this now so it can take effect in Send_DeepCtxt
      }
      else if(u->thal > pfc.gate_thr) { // we just gated
        ClearOtherMaint(u, net, thr_no); // do this now so it can take effect in Send_DeepCtxt
      }
    }
    else {                        // maint gating
      if(u->thal_cnt >= pfc.max_mnt) {
        u->thal = 0.0f;
        u->thal_cnt = -1.0f;
      }
    }
  }
}

void PFCUnitSpec::Send_DeepCtxtNetin(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  if(!deep.IsSuper()) return;
  Compute_DeepRaw(u, net, thr_no); // recompute with current thal, thal_cnt
  inherited::Send_DeepCtxtNetin(u, net, thr_no);
}


void PFCUnitSpec::Compute_DeepStateUpdt(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawPrevQtr(net->quarter)) return;

  if(deep.IsSuper()) {
    if(u->thal > pfc.gate_thr) {
      if(u->thal_cnt > 0) {
        u->thal_cnt += 1.0f;    // deep can't recognize a re-gate of existing gating, so we don't
      }
      else {
        u->thal_cnt = 1.0f;     // new gating
      }
    }
    else if(u->thal_cnt > 0) {
      u->thal_cnt += 1.0f;      // keep maintaining -- out gets reset in Quarter_Init_Deep
    }
    else {
      if(u->thal_cnt >= 0)      // shouldn't happen
        u->thal_cnt = -1.0f;
      else
        u->thal_cnt -= 1.0f;
    }
  }
  else if(deep.IsDeep()) {
    LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
    int dyn_row = 0;
    LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
    if(pfc.use_dyn) {
      int unidx = un->UnitGpUnIdx();
      dyn_row = unidx % n_dyns;
    }
    LeabraUnGpData* ugd = lay->UnGpDataUn(un);
    if(ugd->am_deep_ctxt.max > 0.01f) {
      // we don't know here if this is a re-gate or just continued gating, so we just 
      // increment maintenance -- re-gating is just continued maintenance.
      // super could recognize, but deep is what matters for incrementing dynamics..
      if(u->thal_cnt > 0) { 
        u->thal_cnt += 1.0f;
        // now update dynamics if doing that!
        if(deep.IsDeep() && pfc.use_dyn) {
          float dynval = UpdtDynVal(dyn_row, (u->thal_cnt-1.0f));
          u->deep_ctxt = u->misc_1 * dynval;
        }
      }
      else {                      // not previously maintaining -- first gate
        u->misc_1 = u->deep_ctxt; // record gating ctxt
        u->thal_cnt = 1.0f;
        if(pfc.use_dyn) {
          u->deep_ctxt *= GetDynVal(DYN_INIT, dyn_row);
        }
      }
    }
    else {
      if(u->thal_cnt >= 0)      // could happen here -- we don't get cleared by output gating
        u->thal_cnt = -1.0f;
      else
        u->thal_cnt -= 1.0f;
    }
  }

  inherited::Compute_DeepStateUpdt(u, net, thr_no);
}

void PFCUnitSpec::Compute_DeepRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!deep.on || !Quarter_DeepRawNow(net->quarter)) return;
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
  if(u->thal > pfc.gate_thr || u->thal_cnt >= 1.0f) {
    thal_eff = MAX(u->thal, pfc.mnt_thal);
  }
  
  u->deep_raw = thal_eff * draw;
}

void PFCUnitSpec::ClearOtherMaint(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
    if(!cs->IsMarkerCon()) continue;
    for(int j=0;j<send_gp->size; j++) {
      LeabraUnitVars* uv = (LeabraUnitVars*)send_gp->UnVars(j,net);
      uv->thal = 0.0f;      // terminate!
      uv->thal_cnt = -1.0f; // terminate!
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

