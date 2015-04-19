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

TA_BASEFUNS_CTORS_DEFN(PFCMaintSpec);
TA_BASEFUNS_CTORS_DEFN(PFCUnitSpec);

void PFCMaintSpec::Initialize() {
  Defaults_init();
}

void PFCMaintSpec::Defaults_init() {
  d5b_updt_tau = 10.0f;
  
  d5b_updt_dt = 1.0f / d5b_updt_tau;
}

void PFCMaintSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  d5b_updt_dt = 1.0f / d5b_updt_tau;
}

void PFCUnitSpec::Initialize() {
  InitDynTable();
  Defaults_init();
}

void PFCUnitSpec::Defaults_init() {
  // act_avg.l_up_inc = 0.1f;       // needs a slower upside due to longer maintenance window..
  deep.on = true;
}

void  PFCUnitSpec::FormatDynTable() {
  DataCol* dc;
  
  dc = dyn_table.FindMakeCol("name", VT_STRING);
  dc->desc = "name for this dynamic profile";

  dc = dyn_table.FindMakeCol("desc", VT_STRING);
  dc->desc = "description of this dynamic profile";

  dc = dyn_table.FindMakeCol("gain", VT_FLOAT);
  dc->desc = "overall gain multiplier on strength of maintenance";

  dc = dyn_table.FindMakeCol("init", VT_FLOAT);
  dc->desc = "initial value at point when gating starts";

  dc = dyn_table.FindMakeCol("rise_tau", VT_FLOAT);
  dc->desc = "time constant for rise in deep_raw maintenance activation (per quarter when deep is updated)";
  dc = dyn_table.FindMakeCol("rise_dt", VT_FLOAT);
  dc->desc = "rate = 1 / tau -- automatically computed from tau";

  dc = dyn_table.FindMakeCol("decay_tau", VT_FLOAT);
  dc->desc = "time constant for decay in deep_raw maintenance activation (per quarter when deep is updated)";
  dc = dyn_table.FindMakeCol("decay_dt", VT_FLOAT);
  dc->desc = "rate = 1 / tau -- automatically computed from tau";

  dyn_table.EnforceRows(n_dyns);
}

void  PFCUnitSpec::InitDynTable() {
  n_dyns = 5;
  FormatDynTable();

  int cur = 0;
  SetDynVal("phasic", DYN_NAME, cur);
  SetDynVal("immediate phasic response to gating event", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_GAIN, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(1.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_flat", DYN_NAME, cur);
  SetDynVal("maintained, flat stable sustained activation", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_GAIN, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_rise", DYN_NAME, cur);
  SetDynVal("maintained, rising value over time", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_GAIN, cur);
  SetDynVal(0.0f, DYN_INIT, cur);
  SetDynVal(10.0f, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_decay", DYN_NAME, cur);
  SetDynVal("maintained, decaying value over time", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_GAIN, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(10.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_updn", DYN_NAME, cur);
  SetDynVal("maintained, rising then falling alue over time", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_GAIN, cur);
  SetDynVal(0.0f, DYN_INIT, cur);
  SetDynVal(5.0f, DYN_RISE_TAU, cur);
  SetDynVal(10.0f, DYN_DECAY_TAU, cur);

  UpdtDynTable();
}

void  PFCUnitSpec::UpdtDynTable() {
  // taMisc::Info("updt dyn table");
  dyn_table.StructUpdate(true);
  FormatDynTable();
  for(int i=0; i<dyn_table.rows; i++) {
    float tau = GetDynVal(DYN_RISE_TAU, i);
    float dt = 0.0f;
    if(tau > 0.0f)
      dt = 1.0f / tau;
    SetDynVal(dt, DYN_RISE_DT, i);

    tau = GetDynVal(DYN_DECAY_TAU, i);
    dt = 0.0f;
    if(tau > 0.0f)
      dt = 1.0f / tau;
    SetDynVal(dt, DYN_DECAY_DT, i);
  }
  dyn_table.StructUpdate(false);
}

void PFCUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdtDynTable();
}

void PFCUnitSpec::GraphPFCDyns(DataTable* graph_data, int n_trials) {
  taProject* proj = GET_MY_OWNER(taProject);
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

  float_Array vals;
  vals.SetSize(n_dyns);
  
  graph_data->AddBlankRow();
  rw->SetValAsFloat(0, -1);
  for(int nd=0; nd < n_dyns; nd++) {
    float init = GetDynVal(DYN_INIT, nd);
    vals[nd] = init;
    graph_data->SetValAsFloat(vals[nd], nd+1, -1);
  }
  
  for(int x = 1; x <= n_trials; x++) {
    graph_data->AddBlankRow();
    rw->SetValAsFloat(x, -1);
    for(int nd=0; nd < n_dyns; nd++) {
      float gain = GetDynVal(DYN_GAIN, nd);
      float rise_tau = GetDynVal(DYN_RISE_TAU, nd);
      float rise_dt = GetDynVal(DYN_RISE_DT, nd);
      float decay_dt = GetDynVal(DYN_DECAY_DT, nd);
      float cur = vals[nd];
      float nw = cur;
      if(x-1 < rise_tau) {
        nw += rise_dt;
      }
      else {
        nw -= decay_dt;
      }
      if(nw > 1.0f) nw = 1.0f;
      if(nw < 0.0f) nw = 0.0f;
      vals[nd] = nw;
      graph_data->SetValAsFloat(vals[nd], nd+1, -1);
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

float PFCUnitSpec::Compute_NetinExtras(LeabraUnitVars* uv, LeabraNetwork* net,
                            int thr_no, float& net_syn) {
  float net_ex = inherited::Compute_NetinExtras(uv, net, thr_no, net_syn);
  // bool act_mnt = ActiveMaint((LeabraUnit*)uv->Un(net, thr_no));
  // if(act_mnt) {
  //   net_ex += pfc_maint.maint_d5b_to_super * uv->deep_raw;
  // }
  return net_ex;
}

void PFCUnitSpec::Compute_Act_ThalDeep5b(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  // if(cifer_thal.on) {
  //   if(cifer_thal.auto_thal) {
  //     u->thal = u->act_eq;
  //   }
  //   if(u->thal < cifer_thal.thal_thr)
  //     TestWrite(u->thal, 0.0f);
  //   if(cifer_thal.thal_bin && u->thal > 0.0f)
  //     TestWrite(u->thal, 1.0f);
  // }

  // if(!cifer_d5b.on) return;

  // bool act_mnt = ActiveMaint((LeabraUnit*)u->Un(net, thr_no));
  
  // if(Quarter_Deep5bNow(net->quarter)) {
  //   float act5b = u->act_eq;
  //   if(act5b < cifer_d5b.act5b_thr) {
  //     act5b = 0.0f;
  //   }
  //   act5b *= u->thal;

  //   if(act_mnt && u->thal_prv > 0.0f && u->thal > 0.0f) { // ongoing maintenance
  //     u->deep5b += pfc_maint.d5b_updt_dt * (act5b - u->deep5b);
  //   }
  //   else {                        // first update or off..
  //     u->deep5b = act5b;
  //   }
  // }
  // else {
  //   if(cifer_d5b.burst && !act_mnt) {
  //     u->deep5b = 0.0f;         // turn it off! only if not maint!
  //   }
  // }
}

