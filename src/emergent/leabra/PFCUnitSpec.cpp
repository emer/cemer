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
  Defaults_init();
}

void PFCUnitSpec::Defaults_init() {
  // act_avg.l_up_inc = 0.1f;       // needs a slower upside due to longer maintenance window..
  InitDynTable();
  deep.on = true;
}

void  PFCUnitSpec::FormatDynTable() {
  DataCol* dc;
  
  dc = dyn_table.FindMakeCol("name", VT_STRING);
  dc->desc = "name for this dynamic profile";

  dc = dyn_table.FindMakeCol("desc", VT_STRING);
  dc->desc = "description of this dynamic profile";

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
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(1.0f, DYN_DECAY_TAU, cur);

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

    float init = GetDynVal(DYN_INIT, i);
    if(init == 0.0f) {          // init must be a minimum val -- uses 0 to detect start
      SetDynVal(.1f, DYN_INIT, i);
    }
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

  float_Matrix vals;
  vals.SetGeom(2, 2, n_dyns);     // prev and cur
  vals.InitVals(0.0f);
  
  for(int x = 0; x <= n_trials; x++) {
    graph_data->AddBlankRow();
    rw->SetValAsFloat(x, -1);
    for(int nd=0; nd < n_dyns; nd++) {
      float& cur = vals.FastEl2d(0, nd);
      float& prv = vals.FastEl2d(1, nd);
      float nw = UpdtDynVal(cur, prv, nd);
      graph_data->SetValAsFloat(nw, nd+1, -1);
      prv = cur;
      cur = nw;
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void PFCUnitSpec::Compute_DeepRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(u->thal == 0.0f) {
    TestWrite(u->deep_raw, 0.0f); // not gated, off..
    TestWrite(u->thal_prv, 0.0f); // clear any processed signal
    return;
  }
  // thal > 0 at this point..
  if(u->thal_prv == -0.001f) {     // we've already processed the current thal signal
    return;
  }

  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  LeabraLayer* lay = (LeabraLayer*)un->own_lay();
  int unidx = un->idx % lay->un_geom.n;
  int dyn_row = unidx % n_dyns;

  if(u->thal_prv == 0.0f && u->thal > 0.0f) { // just gated
    u->deep_raw = u->act_eq * GetDynVal(DYN_INIT, dyn_row);
    u->thal_prv = -0.001f;      // mark as processed
    return;
  }
  // must be continued maintenance at this point
  float cur_val = u->deep_raw;
  float prv_val = u->deep_raw_prv;
  float nw_val = UpdtDynVal(cur_val, prv_val, dyn_row);
  u->deep_raw = nw_val;
  u->thal_prv = -0.001f;      // mark as processed
}

float PFCUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {
  LeabraLayerSpec* ls = (LeabraLayerSpec*)u->Un(net, thr_no)->own_lay()->GetLayerSpec();

  float net_ex = 0.0f;
  if(bias_spec) {
    net_ex += u->bias_scale * u->bias_wt;
  }
  if(u->HasExtFlag(UnitVars::EXT)) {
    net_ex += u->ext * ls->clamp.gain;
  }
  if(deep.on) {
    if(deep.d_to_s > 0.0f) {
      net_ex += deep.d_to_s * u->deep_raw; // this is only diff from LeabraUnitSpec!
    }
    if(deep.ctxt_to_s > 0.0f) {
      net_ex += deep.ctxt_to_s * u->deep_ctxt;
    }
    if(deep.thal_to_s > 0.0f) {
      net_ex += deep.thal_to_s * u->thal;
    }
  }
  if(da_mod.on) {
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      net_ex += da_mod.plus * u->dav * net_syn;
    }
    else {                      // MINUS_PHASE
      net_ex += da_mod.minus * u->dav * net_syn;
    }
  }
  return net_ex;
}
