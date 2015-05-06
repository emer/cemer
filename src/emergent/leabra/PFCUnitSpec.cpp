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
#include <MarkerConSpec>
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
}

void PFCUnitSpec::Initialize() {
  Defaults_init();
}

void PFCUnitSpec::Defaults_init() {
  InitDynTable();
  SetUnique("deep", true);
  deep_qtr = Q2_Q4;
  deep.on = true;
  deep.thr = 0.1f;
  deep.d_to_d = 0.0f;
  deep.d_to_s = 1.0f;
  deep.thal_to_d = 0.0f;
  deep.thal_to_s = 0.0f;
  SetUnique("deep_norm", true);
  deep_norm.on = true;
  deep_norm.mod = false;
  deep_norm.raw_val = DeepNormSpec::UNIT;
  deep_norm.contrast = 1.0f;
  deep_norm.copy_def = 0.0f;
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
    if(init == 0.0f) {          // init must be a minimum val -- deep_raw = 0 is non-gated
      SetDynVal(.1f, DYN_INIT, i);
    }
  }
  dyn_table.StructUpdate(false);
}

void PFCUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdtDynTable();
}

void PFCUnitSpec::Send_DeepRawNetin(LeabraUnitVars* u, LeabraNetwork* net,
                                    int thr_no) {
  // always send!
  Send_DeepRawNetin_impl(u, net, thr_no);
}

void PFCUnitSpec::Send_DeepRawNetin_Post(LeabraUnitVars* u, LeabraNetwork* net,
                                         int thr_no) {
  // always send!
  Send_DeepRawNetin_Post_impl(u, net, thr_no);
}


void PFCUnitSpec::Compute_DeepRaw(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  int unidx = un->UnitGpUnIdx();
  int dyn_row = unidx % n_dyns;

  if(u->thal < pfc.gate_thr && u->thal_cnt <= 0.0f) {
    // we are not gating, and nor are we maintaining
    TestWrite(u->deep_raw, 0.0f); // not gated, off..
    TestWrite(u->thal_cnt, -1.0f); // clear any processed signal
    TestWrite(u->misc_1, 0.0f);
    return;
  }
  // now we are either gating or maintaining
  
  if(u->thal >= pfc.gate_thr) {
    // new gating signal -- doesn't hurt to continuously update here..
    TestWrite(u->thal_cnt, 0.0f);     // reset count
    if(u->act_eq < opt_thresh.send) {            // not active enough for gating
      TestWrite(u->deep_raw, 0.0f);
      TestWrite(u->misc_1, 0.0f);
    }
    else {
      u->deep_raw = u->act_eq * GetDynVal(DYN_INIT, dyn_row);
      u->misc_1 = u->act_eq;
    }
    return;
  }
  
  // now we are continuing to maintain, do nothing at this point..
  // todo: could update deep_raw in some in some way, but not really..
}

void PFCUnitSpec::Compute_DeepNorm(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(!Compute_DeepTest(u, net, thr_no))
    return;
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  int unidx = un->UnitGpUnIdx();
  int dyn_row = unidx % n_dyns;

  if(u->thal_cnt < 0.0f) {
    TestWrite(u->deep_norm, 0.0f);        // not maintaining, bail
    TestWrite(u->deep_raw, 0.0f);        // not maintaining, bail
    return;
  }

  if(pfc.out_gate) {
    if(u->thal_cnt >= pfc.out_mnt) {
      // out gating is transient, now turn it off, and clear other guys
      TestWrite(u->deep_norm, 0.0f);        // not maintaining, bail
      TestWrite(u->deep_raw, 0.0f);        // not maintaining, bail
      TestWrite(u->thal_cnt, -1.0f);
    }
    else {
      u->deep_norm = u->deep_raw;
      u->thal_cnt += 1.0f;
    }
  }
  else {
    // compute deep_norm based on current values..
    // float dctxt = u->deep_ctxt;
    // float lctxt = lay->am_deep_ctxt.avg;
    // float nw_nrm = 0.0f;
    // if(u->deep_raw > opt_thresh.send) {
    //   // deep_norm only registered for units that have deep_raw firing -- others use lay->deep_norm_def
    //   nw_nrm = deep_norm.ComputeNormLayCtxt(u->deep_raw, dctxt, lctxt);
    // }
    // u->deep_norm = nw_nrm;

    if(u->thal_cnt >= pfc.max_mnt) {
      TestWrite(u->deep_norm, 0.0f);        // not maintaining, bail
      TestWrite(u->deep_raw, 0.0f);        // not maintaining, bail
      TestWrite(u->thal_cnt, -1.0f);
    }
    else {
      u->deep_norm = u->deep_raw;

      // now update maintenance for next time!
      u->thal_cnt += 1.0f;
      float cur_val = u->deep_raw;
      float prv_val = u->deep_raw_pprv;
      float nw_val = UpdtDynVal(cur_val, prv_val, u->misc_1, dyn_row);
      u->deep_raw = nw_val;
    }
  }
}

void PFCUnitSpec::Send_DeepNormNetin(LeabraUnitVars* u, LeabraNetwork* net,
                                      int thr_no) {
  if(!Compute_DeepTest(u, net, thr_no))
    return;

  if(pfc.out_gate) {
    if(u->thal_cnt == 1.0f) {
      ClearOtherMaint(u, net, thr_no); // send clear to others
    }
  }

  // actually, don't send right now -- does deep_norm renorm..
  //  inherited::Send_DeepNormNetin(u, net, thr_no);
}

float PFCUnitSpec::Compute_NetinExtras(LeabraUnitVars* u, LeabraNetwork* net,
                                          int thr_no, float& net_syn) {

  float net_ex = inherited::Compute_NetinExtras(u, net, thr_no, net_syn);
  if(u->thal >= pfc.gate_thr) { // our gate is open
    net_ex += u->deep_raw_net;  // add in the gated deep inputs!
  }
  return net_ex;
}

void PFCUnitSpec::ClearOtherMaint(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)send_gp->GetConSpec();
    if(!cs->InheritsFrom(TA_MarkerConSpec)) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitVars*)send_gp->UnVars(j,net))->thal_cnt = -1.0f; // terminate!
    }
  }
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
      float nw;
      if(x == 0) {
        nw = GetDynVal(DYN_INIT, nd);
      }
      else {
        nw = UpdtDynVal(cur, prv, 1.0f, nd);
      }
      graph_data->SetValAsFloat(nw, nd+1, -1);
      prv = cur;
      cur = nw;
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

