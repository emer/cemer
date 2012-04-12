// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_extra.h"

#include "netstru_extra.h"
#include "ta_dataproc.h"
#include "ta_dataanal.h"

//////////////////////////////////
//      MarkerConSpec           //
//////////////////////////////////

void MarkerConSpec::Initialize() {
  Defaults_init();
}

void MarkerConSpec::Defaults_init() {
  SetUnique("rnd", true);
  rnd.mean = 0.0f; rnd.var = 0.0f;
  SetUnique("wt_limits", true);
  wt_limits.sym = false;
  SetUnique("wt_scale", true);
  wt_scale.rel = 0.0f;
  SetUnique("lrate", true);
  lrate = 0.0f;
  cur_lrate = 0.0f;
}

//////////////////////////////////
//      ContextLayerSpec        //
//////////////////////////////////

void CtxtUpdateSpec::Initialize() {
  fm_hid = 1.0f;
  fm_prv = 0.0f;
  to_out = 1.0f;
}

const String
LeabraContextLayerSpec::do_update_key("LeabraContextLayerSpec__do_update");

void LeabraContextLayerSpec::Initialize() {
  updt.fm_prv = 0.0f;
  updt.fm_hid = 1.0f;
  updt.to_out = 1.0f;
  SetUnique("decay", true);
  update_criteria = UC_TRIAL;
  Defaults_init();
}

void LeabraContextLayerSpec::Defaults_init() {
  decay.event = 0.0f;
  decay.phase = 0.0f;
}

// void LeabraContextLayerSpec::UpdateAfterEdit-impl() {
//   inherited::UpdateAfterEdit_impl();
//   hysteresis_c = 1.0f - hysteresis;
// }

bool LeabraContextLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

//   LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  return rval;
}

taBase::DumpQueryResult LeabraContextLayerSpec::Dump_QuerySaveMember(MemberDef* md) {
  // only save n_spec if needed (to ease backwards compat)
  if (md->name != "n_spec")
    return inherited::Dump_QuerySaveMember(md);
  return (update_criteria == UC_N_TRIAL) ? DQR_SAVE : DQR_NO_SAVE;
}

void LeabraContextLayerSpec::Compute_Context(LeabraLayer* lay, LeabraUnit* u, LeabraNetwork* net) {
  if(net->phase == LeabraNetwork::PLUS_PHASE) {
    u->ext = u->act_m;          // just use previous minus phase value!
  }
  else {
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(TestError(!cg, "Compute_Context", "requires one recv projection!")) {
      return;
    }
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(TestError(!su, "Compute_Context", "requires one unit in recv projection!")) {
      return;
    }
    u->ext = updt.fm_prv * u->act_p + updt.fm_hid * su->act_p; // compute new value
  }
  u->SetExtFlag(Unit::EXT);
  u->Compute_HardClamp(net);
}

void LeabraContextLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = true;     // cache this flag
  lay->SetExtFlag(Unit::EXT);
  bool do_update = lay->GetUserDataDef(do_update_key, false).toBool();
  if (do_update) {
    lay->SetUserData(do_update_key, false); // reset
  } else { // not explicit triger, so try other conditions
    switch (update_criteria) {
    case UC_TRIAL:
      do_update = true;
      break;
    case UC_MANUAL: break; // weren't triggered, so that's it
    case UC_N_TRIAL: {
      // do modulo the trial, adding offset -- add 1 so first trial is not trigger
      do_update = (((net->trial + n_spec.n_offs + 1) % n_spec.n_trials) == 0);
    } break;
    }
  }
  if (!do_update) return;

  lay->Inhib_SetVals(inhib.kwta_pt);            // assume 0 - 1 clamped inputs

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    Compute_Context(lay, u, net);
  }
  Compute_CycleStats(lay, net);
}

void LeabraContextLayerSpec::TriggerUpdate(LeabraLayer* lay) {
  if (!lay) return;
  if (TestError((lay->spec.spec.ptr() != this),
    "TriggerUpdate", "Spec does not belong to the layer passed as arg"))
    return;
  lay->SetUserData(do_update_key, true);
}

void LeabraLayer::TriggerContextUpdate() {
  LeabraContextLayerSpec* cls = dynamic_cast<LeabraContextLayerSpec*>
    (spec.spec.ptr());
  if (cls) {
    cls->TriggerUpdate(this);
  }
}

//////////////////////////////////
//      MultCopyLayerSpec

void LeabraMultCopyLayerSpec::Initialize() {
  one_minus = false;
  mult_gain = 1.0f;
}

bool LeabraMultCopyLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "leabra mult copy layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  if(lay->CheckError(u->recv.size != 2, quiet, rval,
                "leabra mult copy layer must have exactly 2 recv prjns, first = act to copy, second = act to multiply:", lay->name)) {
    return false;               // fatal
  }

  LeabraRecvCons* copy_gp = (LeabraRecvCons*)u->recv.FastEl(0);
  if(lay->CheckError(copy_gp->size != 1, quiet, rval,
                "leabra mult copy layer first prjn (copy act source) must have exactly 1 connection to copy from:", lay->name)) {
    return false;               // fatal
  }
  LeabraRecvCons* mult_gp = (LeabraRecvCons*)u->recv.FastEl(1);
  if(lay->CheckError(mult_gp->size != 1, quiet, rval,
                "leabra mult copy layer second prjn (mult act source) must have exactly 1 connection to get mult act from:", lay->name)) {
    return false;               // fatal
  }
  return rval;
}

void LeabraMultCopyLayerSpec::Compute_MultCopyAct(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    if(u->recv.size < 2) continue;

    LeabraRecvCons* copy_gp = (LeabraRecvCons*)u->recv.FastEl(0);
    LeabraRecvCons* mult_gp = (LeabraRecvCons*)u->recv.FastEl(1);

    if(copy_gp->size != 1) continue;
    if(mult_gp->size != 1) continue;

    LeabraUnitSpec* rus = (LeabraUnitSpec*)u->GetUnitSpec();

    LeabraUnit* copy_un = (LeabraUnit*)copy_gp->Un(0);
    LeabraUnit* mult_un = (LeabraUnit*)mult_gp->Un(0);

    float mult_eff = mult_gain * mult_un->act_eq;;
    if(mult_eff > 1.0f) mult_eff = 1.0f;
    float new_act;
    if(one_minus)
      new_act = copy_un->act_eq * (1.0f - mult_eff);
    else
      new_act = copy_un->act_eq * mult_eff;
    u->act = new_act;
    u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;               // I'm fully settled!
    u->AddToActBuf(rus->syn_delay);
  }
}

void LeabraMultCopyLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_MultCopyAct(lay, net);
  inherited::Compute_CycleStats(lay, net);
}


//////////////////////////////////////////
//      Misc Special Objects            //
//////////////////////////////////////////

//////////////////////////////////
//      Linear Unit             //
//////////////////////////////////

void LeabraLinUnitSpec::Initialize() {
  SetUnique("act_fun", true);
  SetUnique("act_range", true);
  SetUnique("clamp_range", true);
  SetUnique("act", true);
  act_fun = LINEAR;
  act_range.max = 20;
  act_range.min = 0;
  act_range.UpdateAfterEdit_NoGui();
  clamp_range.max = 1.0f;
  clamp_range.UpdateAfterEdit_NoGui();
  act.gain = 2;
}

void LeabraLinUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraNetwork* net) {
  float new_act = u->net * act.gain; // use linear netin as act

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += Compute_Noise(u, net);
  }
  u->act = u->act_nd = u->act_eq = act_range.Clip(new_act);
}

//////////////////////////
//      NegBias         //
//////////////////////////

void LeabraNegBiasSpec::Initialize() {
  decay = 0.0f;
  updt_immed = false;
}

//////////////////////////////////
//      XCalSRAvgConSpec
//////////////////////////////////

void XCalSRAvgConSpec::Initialize() {
}

bool XCalSRAvgConSpec::CheckConfig_RecvCons(RecvCons* cg, bool quiet) {
  bool rval = true;
  if((learn_rule == CTLEABRA_CAL) || (learn_rule == CTLEABRA_XCAL)) {
    if(cg->prjn) {
      if(cg->prjn->CheckError(!cg->prjn->con_type->InheritsFrom(&TA_LeabraSRAvgCon), quiet, rval,
                      "does not have con_type = LeabraSRAvgCon -- required for CTLEABRA_CAL or _XCAL learning to hold the sravg connection-level values -- I just fixed this for you in this projection, but must do Build to get it to take effect")) {
        cg->prjn->con_type = &TA_LeabraSRAvgCon;
      }
    }
  }
  return rval;
}


//////////////////////////////////
//      XCalHebbConSpec
//////////////////////////////////

void XCalHebbConSpec::Initialize() {
  hebb_mix = 0.0f;
  su_act_min = 0.0f;
  hebb_mix_c = 1.0f - hebb_mix;
}

void XCalHebbConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  hebb_mix_c = 1.0f - hebb_mix;
}

// //////////////////////////////////
// //   XCalMlTraceConSpec
// //////////////////////////////////

// void XCalMlTraceConSpec::Initialize() {
//   ml_mix = 0.0f;
//   sm_mix = 1.0f - ml_mix;
// }

// void XCalMlTraceConSpec::UpdateAfterEdit_impl() {
//   inherited::UpdateAfterEdit_impl();
//   sm_mix = 1.0f - ml_mix;
// }

//////////////////////////////////
//      TrialSynDepConSpec      //
//////////////////////////////////

void TrialSynDepSpec::Initialize() {
  rec = 1.0f;
  depl = 1.1f;
}

void TrialSynDepConSpec::Initialize() {
  min_obj_type = &TA_TrialSynDepCon;
}

void TrialSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(syn_dep.rec <= 0.0f)       // can't go to zero!
    syn_dep.rec = 1.0f;
}

//////////////////////////////////
//      CycleSynDepConSpec      //
//////////////////////////////////

void CycleSynDepSpec::Initialize() {
  rec = 0.002f;
  asymp_act = 0.4f;
  depl = rec * (1.0f - asymp_act); // here the drive is constant
}

void CycleSynDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(rec < .00001f) rec = .00001f;
  // chg = rec * (1 - cur) - dep * drive = 0; // equilibrium point
  // rec * (1 - cur) = dep * drive
  // dep = rec * (1 - cur) / drive
  depl = rec * (1.0f - asymp_act); // here the drive is constant
  depl = MAX(depl, 0.0f);
}

void CycleSynDepConSpec::Initialize() {
  min_obj_type = &TA_CycleSynDepCon;
}

void CycleSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  syn_dep.UpdateAfterEdit_NoGui();
}

//////////////////////////////////
//      CaiSynDepCon
//////////////////////////////////

void CaiSynDepSpec::Initialize() {
  ca_inc = .2f;                 // base per-cycle is .01
  ca_dec = .2f;                 // base per-cycle is .01
  sd_ca_thr = 0.2f;
  sd_ca_gain = 0.3f;
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CaiSynDepSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  sd_ca_thr_rescale = sd_ca_gain / (1.0f - sd_ca_thr);
}

void CaiSynDepConSpec::Initialize() {
  min_obj_type = &TA_CaiSynDepCon;
}

void CaiSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ca_dep.UpdateAfterEdit_NoGui();
}

//// SRAvg version

void SRAvgCaiSynDepConSpec::Initialize() {
  min_obj_type = &TA_SRAvgCaiSynDepCon;
}

void SRAvgCaiSynDepConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ca_dep.UpdateAfterEdit_NoGui();
}



bool SRAvgCaiSynDepConSpec::CheckConfig_RecvCons(RecvCons* cg, bool quiet) {
  bool rval = true;
  if((learn_rule == CTLEABRA_CAL) || (learn_rule == CTLEABRA_XCAL)) {
    if(cg->prjn) {
      if(cg->prjn->CheckError(!cg->prjn->con_type->InheritsFrom(&TA_SRAvgCaiSynDepCon), quiet, rval,
                              "does not have con_type = SRAvgCaiSynDepCon -- required for CTLEABRA_CAL or _XCAL learning to hold the sravg connection-level values -- I just fixed this for you in this projection, but must do Build to get it to take effect")) {
        cg->prjn->con_type = &TA_SRAvgCaiSynDepCon;
      }
    }
  }
  return rval;
}



//////////////////////////////////
//      FastWtConSpec           //
//////////////////////////////////

void FastWtSpec::Initialize() {
  lrate = .05f;
  use_lrs = false;
  cur_lrate = .05f;
  decay = 1.0f;
  slw_sat = true;
  dk_mode = SU_THR;
}

void FastWtConSpec::Initialize() {
  min_obj_type = &TA_FastWtCon;
}

void FastWtConSpec::SetCurLrate(LeabraNetwork* net, int epoch) {
  LeabraConSpec::SetCurLrate(net, epoch);
  if(fast_wt.use_lrs)
    fast_wt.cur_lrate = fast_wt.lrate * lrate_sched.GetVal(epoch);
  else
    fast_wt.cur_lrate = fast_wt.lrate;
}

///////////////////////////////////////////////////////////////
//   ActAvgHebbConSpec

void ActAvgHebbMixSpec::Initialize() {
  act_avg = .5f;
  cur_act = .5f;
}

void ActAvgHebbMixSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cur_act = 1.0f - act_avg;
}

void ActAvgHebbConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  act_avg_hebb.UpdateAfterEdit_NoGui();
}

void ActAvgHebbConSpec::Initialize() {
}

///////////////////////////////////////////////////////////////
//   LeabraDeltaConSpec

void LeabraDeltaConSpec::Initialize() {
  SetUnique("lmix", true);
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
//   lmix.err_sb = false;

  SetUnique("wt_limits", true);
  wt_limits.sym = false;

  SetUnique("wt_sig", true);
  wt_sig.gain = 1.0f;  wt_sig.off = 1.0f;

//   SetUnique("xcalm", true);
//   xcalm.use_sb = false;
}

void LeabraDeltaConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // these are enforced absolutely because the code does not use them:
  lmix.hebb = 0.0f;
  lmix.err = 1.0f;
}

///////////////////////////////////////////////////////////////
//   LeabraXCALSpikeConSpec

void XCALSpikeSpec::Initialize() {
  ss_sr = false;
  ca_norm = 5.0f;
  k_ca = 0.3f / ca_norm;
  ca_vgcc = 1.3f / ca_norm;
  ca_v_nmda = 0.0223f / ca_norm;
  ca_nmda = 0.5 / ca_norm;
  ca_dt = 20.0f;
  ca_rate = 1.0f / ca_dt;
  ca_off = 0.1f;
  nmda_dt = 40.0f;
  nmda_rate = 1.0f / nmda_dt;
}

void XCALSpikeSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  k_ca = 0.3f / ca_norm;
  ca_vgcc = 1.3f / ca_norm;
  ca_v_nmda = 0.0223f / ca_norm;
  ca_nmda = 0.5 / ca_norm;
  ca_rate = 1.0f / ca_dt;
  nmda_rate = 1.0f / nmda_dt;
}

void LeabraXCALSpikeConSpec::Initialize() {
  min_obj_type = &TA_LeabraSpikeCon;
}

void LeabraXCALSpikeConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  xcal_spike.UpdateAfterEdit_NoGui();
}

void LeabraXCALSpikeConSpec::GraphXCALSpikeSim(DataTable* graph_data,
                                               LeabraUnitSpec* unit_spec,
                                               float rate_min, float rate_max, float rate_inc,
                                               float max_time, int reps_per_point,
                                               float lin_norm) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_XCALSpikeSim", true);
  }

  bool local_us = false;
  if(!unit_spec) {
    unit_spec = new LeabraUnitSpec;
    local_us = true;
  }

  String sim_data_name = name + "_XCALSpikeSim_Tmp";
  DataTable* sim_data = proj->GetNewAnalysisDataTable(sim_data_name, true);

  sim_data->StructUpdate(true);
//   graph_data->ResetData();
  int idx;

//   DataTable sim_data;
//   taBase::Ref(sim_data);
  DataCol* sim_r_rate = sim_data->FindMakeColName("r_rate", idx, VT_FLOAT);
  DataCol* sim_s_rate = sim_data->FindMakeColName("s_rate", idx, VT_FLOAT);
  DataCol* sim_ca_avg = sim_data->FindMakeColName("ca_avg", idx, VT_FLOAT);
  DataCol* sim_sravg_ss = sim_data->FindMakeColName("sravg_ss", idx, VT_FLOAT);
  DataCol* sim_sravg_s = sim_data->FindMakeColName("sravg_s", idx, VT_FLOAT);
  DataCol* sim_sravg_m = sim_data->FindMakeColName("sravg_m", idx, VT_FLOAT);
  DataCol* sim_srprod_s = sim_data->FindMakeColName("srprod_s", idx, VT_FLOAT);
  DataCol* sim_srprod_m = sim_data->FindMakeColName("srprod_m", idx, VT_FLOAT);
  DataCol* sim_sravg_lin = sim_data->FindMakeColName("sravg_lin", idx, VT_FLOAT);

  float s_rate, r_rate;
  for(r_rate = rate_min; r_rate <= rate_max; r_rate += rate_inc) {
    for(s_rate = rate_min; s_rate <= rate_max; s_rate += rate_inc) {
      for(int rep=0; rep < reps_per_point; rep++) {
        float nmda = 0.0f;
        float ca = 0.0f;
        float ca_avg = 0.0f;
        float ca_sum = 0.0f;
        float vmd = 0.0f;
        float r_p = r_rate / 1000.0f;
        float s_p = s_rate / 1000.0f;
        float time = 0.0f;
        bool s_act = false;
        bool r_act = false;
        float s_avg = 0.0f;
        float r_avg = 0.0f;
        float s_lin = 0.0f;
        float r_lin = 0.0f;
        float s_avg_s = 0.15f;
        float s_avg_m = 0.15f;
        float r_avg_s = 0.15f;
        float r_avg_m = 0.15f;
        float sravg_ss = 0.15f;
        float sravg_s = 0.15f;
        float sravg_m = 0.15f;
        for(time = 0.0f; time < max_time; time += 1.0f) {
          s_act = (bool)Random::Poisson(s_p);
          r_act = (bool)Random::Poisson(r_p);
          if(r_act) {
            vmd += unit_spec->spike_misc.vm_dend;
            r_avg += 1.0f;
          }
          vmd -= vmd / unit_spec->spike_misc.vm_dend_dt;
          float dnmda = -nmda * xcal_spike.nmda_rate;
          float dca = (nmda * (xcal_spike.ca_v_nmda * vmd + xcal_spike.ca_nmda))
            - (ca * xcal_spike.ca_rate);
          if(s_act) { s_avg += 1.0f; dnmda += xcal_spike.k_ca / (xcal_spike.k_ca + ca); }
          if(r_act) { dca += xcal_spike.ca_vgcc; }
          nmda += dnmda;
          ca += dca;
          ca_sum += ca;

          float sr = (ca - xcal_spike.ca_off); if(sr < 0.0f) sr = 0.0f;
          sravg_ss += unit_spec->act_avg.ss_dt * (sr - sravg_ss);
          sravg_s += unit_spec->act_avg.s_dt * (sravg_ss - sravg_s);
          sravg_m += unit_spec->act_avg.m_dt * (sravg_s - sravg_m);

          r_avg_s += unit_spec->act_avg.s_dt * ((float)r_act - r_avg_s);
          r_avg_m += unit_spec->act_avg.m_dt * (r_avg_s - r_avg_m);
          s_avg_s += unit_spec->act_avg.s_dt * ((float)s_act - s_avg_s);
          s_avg_m += unit_spec->act_avg.m_dt * (s_avg_s - s_avg_m);
        }
        ca_avg = ca_sum / max_time;
        s_lin *= lin_norm;
        r_lin *= lin_norm;
        float sravg_lin = s_avg * r_avg;

        float srprod_s = r_avg_s * s_avg_s;
        float srprod_m = r_avg_m * s_avg_m;

        sim_data->AddBlankRow();
        sim_r_rate->SetValAsFloat(r_rate, -1);
        sim_s_rate->SetValAsFloat(s_rate, -1);
        sim_ca_avg->SetValAsFloat(ca_avg, -1);
        sim_sravg_ss->SetValAsFloat(sravg_ss, -1);
        sim_sravg_s->SetValAsFloat(sravg_s, -1);
        sim_sravg_m->SetValAsFloat(sravg_m, -1);
        sim_srprod_s->SetValAsFloat(srprod_s, -1);
        sim_srprod_m->SetValAsFloat(srprod_m, -1);
        sim_sravg_lin->SetValAsFloat(sravg_lin, -1);
      }
    }
  }

  if(local_us) {
    delete unit_spec;
  }

  sim_data->StructUpdate(false);

  DataGroupSpec dgs;
  taBase::Ref(dgs);
  dgs.append_agg_name = false;
  //  dgs.SetDataTable(sim_data);
  dgs.AddAllColumns(sim_data);
  dgs.ClearColumns();

  ((DataGroupEl*)dgs.ops[0])->agg.op = Aggregate::GROUP; // r_rate
  ((DataGroupEl*)dgs.ops[1])->agg.op = Aggregate::GROUP; // s_rate
  for(int i=2; i< dgs.ops.size; i++)
    ((DataGroupEl*)dgs.ops[i])->agg.op = Aggregate::MEAN;

  taDataProc::Group(graph_data, sim_data, &dgs);
  dgs.ClearColumns();
  taDataAnal::Matrix3DGraph(graph_data, "s_rate", "r_rate");

  DataCol* gp_r_rate = graph_data->FindMakeColName("r_rate", idx, VT_FLOAT);
  gp_r_rate->SetUserData("X_AXIS", true);
  DataCol* gp_s_rate = graph_data->FindMakeColName("s_rate", idx, VT_FLOAT);
  gp_s_rate->SetUserData("Z_AXIS", true);
  DataCol* gp_sravg_m = graph_data->FindMakeColName("sravg_m", idx, VT_FLOAT);
  gp_sravg_m->SetUserData("PLOT_1", true);
  DataCol* gp_sravg_s = graph_data->FindMakeColName("sravg_s", idx, VT_FLOAT);
  gp_sravg_s->SetUserData("PLOT_2", true);
  DataCol* gp_srprod_m = graph_data->FindMakeColName("srprod_m", idx, VT_FLOAT);
  gp_srprod_m->SetUserData("PLOT_3", true);
  DataCol* gp_srprod_s = graph_data->FindMakeColName("srprod_s", idx, VT_FLOAT);
  gp_srprod_s->SetUserData("PLOT_4", true);

  graph_data->FindMakeGraphView();

  proj->data.RemoveLeafName(sim_data_name); // nuke it
}


///////////////////////////////////////////////////////////////
//   LeabraLimPrecConSpec

void LeabraLimPrecConSpec::Initialize() {
  prec_levels = 1024;
}

void LeabraDaNoise::Initialize() {
  da_noise = 1.0f;
  std_leabra = 1.0f;
}

void LeabraDaNoiseConSpec::Initialize() {
}

//////////////////////////////////
//      Scalar Value Layer      //
//////////////////////////////////

void ScalarValSpec::Initialize() {
  rep = LOCALIST;
  un_width = .3f;
  norm_width = false;
  clamp_pat = false;
  clip_val = true;
  send_thr = false;
  init_nms = true;

  min = val = 0.0f;
  range = incr = 1.0f;
  un_width_eff = un_width;

  Defaults_init();
}

void ScalarValSpec::Defaults_init() {
  min_sum_act = 0.2f;
}

void ScalarValSpec::InitRange(float umin, float urng) {
  min = umin; range = urng;
  un_width_eff = un_width;
  if(norm_width)
    un_width_eff *= range;
}

void ScalarValSpec::InitVal(float sval, int ugp_size, float umin, float urng) {
  InitRange(umin, urng);
  val = sval;
  incr = range / (float)(ugp_size - 2); // skip 1st unit, and count end..
  //  incr -= .000001f;         // round-off tolerance..
}

// rep 1.5.  ugp_size = 4, incr = 1.5 / 3 = .5
// 0  .5   1
// oooo111122222 = val / incr

// 0 .5  1  val = .8, incr = .5
// 0 .4 .6
// (.4 * .5 + .6 * 1) / (.6 + .4) = .8

// act = 1.0 - (fabs(val - cur) / incr)


float ScalarValSpec::GetUnitAct(int unit_idx) {
  int eff_idx = unit_idx - 1;
  if(rep == GAUSSIAN) {
    float cur = min + incr * (float)eff_idx;
    float dist = (cur - val) / un_width_eff;
    return taMath_float::exp_fast(-(dist * dist));
  }
  else if(rep == LOCALIST) {
    float cur = min + incr * (float)eff_idx;
    if(fabs(val - cur) > incr) return 0.0f;
    return 1.0f - (fabs(val - cur) / incr);
  }
  return 0.0f;                  // compiler food
}

float ScalarValSpec::GetUnitVal(int unit_idx) {
  int eff_idx = unit_idx - 1;
  float cur = min + incr * (float)eff_idx;
  return cur;
}

void ScalarValBias::Initialize() {
  un = NO_UN;
  un_shp = VAL;
  un_gain = 1.0f;
  wt = NO_WT;
  val = 0.0f;
  wt_gain = 1.0f;
}

void ScalarValLayerSpec::Initialize() {
  Defaults_init();
}

void ScalarValLayerSpec::Defaults_init() {
  SetUnique("kwta", true);
  SetUnique("gp_kwta", true);
  SetUnique("inhib", true);
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    kwta.k_from = KWTASpec::USE_K;
    kwta.k = 3;
    gp_kwta.k_from = KWTASpec::USE_K;
    gp_kwta.k = 3;
    inhib.type = LeabraInhibSpec::KWTA_INHIB;
    inhib.kwta_pt = 0.25f;

    unit_range.min = -0.5f;   unit_range.max = 1.5f;
    unit_range.UpdateAfterEdit_NoGui();
    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else if(scalar.rep == ScalarValSpec::LOCALIST) {
    kwta.k_from = KWTASpec::USE_K;
    kwta.k = 1;
    gp_kwta.k_from = KWTASpec::USE_K;
    gp_kwta.k = 1;
    inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
    inhib.kwta_pt = .9f;

    unit_range.min = 0.0f;  unit_range.max = 1.0f;
    unit_range.UpdateAfterEdit_NoGui();
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;

    scalar.min_sum_act = .2f;
  }
  val_range.UpdateAfterEdit_NoGui();
}

void ScalarValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  unit_range.UpdateAfterEdit_NoGui();
  scalar.UpdateAfterEdit_NoGui();
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else {
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  val_range.UpdateAfterEdit_NoGui();
}

void ScalarValLayerSpec::HelpConfig() {
  String help = "ScalarValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent a single scalar value.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current scalar value is displayed in the first unit in the layer, which can be clamped\
 and compared, etc (i.e., set the environment patterns to have just one unit and provide\
 the actual scalar value and it will automatically establish the appropriate distributed\
 representation in the rest of the units).  This first unit is only viewable as act_eq,\
 not act, because it must not send activation to other units.\n\
 \nScalarValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the ScalarValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool ScalarValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
                "coarse-coded scalar representation requires at least 3 units, I just set un_geom.n")) {
    if(scalar.rep == ScalarValSpec::LOCALIST) {
      lay->un_geom.n = 4;
      lay->un_geom.x = 4;
    }
    else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
      lay->un_geom.n = 12;
      lay->un_geom.x = 12;
    }
  }

  if(scalar.rep == ScalarValSpec::LOCALIST) {
    kwta.k = 1;         // localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == ScalarValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->hyst.init, quiet, rval,
                  "bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
    }
    if(lay->CheckError(us->acc.init, quiet, rval,
                  "bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("acc", true);
      us->acc.init = false;
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "scalar val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "scalar val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "scalar val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
        cs->SetUnique("lrate", true);
        cs->lrate = 0.0f;
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return rval;
}

void ScalarValLayerSpec::ReConfig(Network* net, int n_units) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr() != this) continue;

    if(n_units > 0) {
      lay->un_geom.n = n_units;
      lay->un_geom.x = n_units;
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);    // taking 1st unit as representative

    if(scalar.rep == ScalarValSpec::LOCALIST) {
      scalar.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = 0.0f; unit_range.max = 1.0f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = ScalarValBias::GC;
      bias_val.wt = ScalarValBias::NO_WT;
      unit_range.min = -.5f; unit_range.max = 1.5f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = true;
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void ScalarValLayerSpec::Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md,
                                            int gpidx, float val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = .03f * bias_val.wt_gain * scalar.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
         cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        LeabraCon* cn = (LeabraCon*)recv_gp->PtrCn(ci);
        cn->wt += act;
        if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
        if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u);
    }
  }
}

void ScalarValLayerSpec::Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md,
                                            int gpidx, float val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = bias_val.un_gain * scalar.GetUnitAct(i);
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.OwnCn(0)->wt = act;
  }
}

void ScalarValLayerSpec::Compute_UnBias_NegSlp(LeabraLayer* lay, Layer::AccessMode acc_md,
                                               int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  float val = 0.0f;
  float incr = bias_val.un_gain / (float)(nunits - 2);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_a = val;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.OwnCn(0)->wt = -val;
  }
}

void ScalarValLayerSpec::Compute_UnBias_PosSlp(LeabraLayer* lay, Layer::AccessMode acc_md,
                                               int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  float val = bias_val.un_gain;
  float incr = bias_val.un_gain / (float)(nunits - 2);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(bias_val.un == ScalarValBias::GC)
      u->vcb.g_h = val;
    else if(bias_val.un == ScalarValBias::BWT)
      u->bias.OwnCn(0)->wt = val;
  }
}

void ScalarValLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  if(bias_val.un != ScalarValBias::NO_UN) {
    if(bias_val.un_shp == ScalarValBias::VAL) {
      UNIT_GP_ITR(lay, Compute_UnBias_Val(lay, acc_md, gpidx, bias_val.val););
    }
    else if(bias_val.un_shp == ScalarValBias::NEG_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_NegSlp(lay, acc_md, gpidx););
    }
    else if(bias_val.un_shp == ScalarValBias::POS_SLP) {
      UNIT_GP_ITR(lay, Compute_UnBias_PosSlp(lay, acc_md, gpidx););
    }
  }
  if(bias_val.wt == ScalarValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(lay, acc_md, gpidx, bias_val.val););
  }
}

void ScalarValLayerSpec::BuildUnits_Threads_ugp(LeabraLayer* lay,
                                                Layer::AccessMode acc_md, int gpidx,
                                                LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* un = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(un->lesioned()) continue;
    if(i == 0) { un->flat_idx = 0; continue; }
    un->flat_idx = net->units_flat.size;
    net->units_flat.Add(un);
  }
}

void ScalarValLayerSpec::BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net) {
  lay->units_flat_idx = net->units_flat.size;
  if(lay->units.leaves == 0) return; // not built yet
  UNIT_GP_ITR(lay, BuildUnits_Threads_ugp(lay, acc_md, gpidx, net););
}

void ScalarValLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  Compute_BiasVal(lay, net);
  if(scalar.init_nms)
    LabelUnits(lay, net);
}

void ScalarValLayerSpec::Compute_AvgMaxVals_ugp(LeabraLayer* lay,
                                                Layer::AccessMode acc_md, int gpidx,
                                                AvgMaxVals& vals, ta_memb_ptr mb_off) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  vals.InitVals();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(i == 0) { continue; } // skip first unit
    float val = *((float*)MemberDef::GetOff_static((void*)u, 0, mb_off));
    vals.UpdtVals(val, i);
  }
  vals.CalcAvg(nunits);
}

void ScalarValLayerSpec::ClampValue_ugp(LeabraLayer* lay,
                                        Layer::AccessMode acc_md, int gpidx,
                                        LeabraNetwork*, float rescale) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
  if(!clamp.hard)
    u->UnSetExtFlag(Unit::EXT);
  else
    u->SetExtFlag(Unit::EXT);
  float val = u->ext;
  if(scalar.clip_val)
    val = val_range.Clip(val);          // first unit has the value to clamp
  scalar.InitVal(val, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = rescale * scalar.GetUnitAct(i);
    if(act < us->opt_thresh.send)
      act = 0.0f;
    u->SetExtFlag(Unit::EXT);
    u->ext = act;
  }
}

float ScalarValLayerSpec::ClampAvgAct(int ugp_size) {
  if(ugp_size < 3) return 0.0f;
  float val = val_range.min + .5f * val_range.Range(); // half way
  scalar.InitVal(val, ugp_size, unit_range.min, unit_range.range);
  float sum = 0.0f;
  for(int i=1;i<ugp_size;i++) {
    float act = scalar.GetUnitAct(i);
    sum += act;
  }
  sum /= (float)(ugp_size - 1);
  return sum;
}

float ScalarValLayerSpec::ReadValue_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                        LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return 0.0f;   // must be at least a few units..

  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  float avg = 0.0f;
  float sum_act = 0.0f;
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
    float cur = scalar.GetUnitVal(i);
    float act_val = 0.0f;
    if(!scalar.send_thr || (u->act_eq >= us->opt_thresh.send)) // only if over sending thresh!
      act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
    avg += cur * act_val;
    sum_act += act_val;
  }
  sum_act = MAX(sum_act, scalar.min_sum_act);
  if(sum_act > 0.0f)
    avg /= sum_act;
  // set the first unit in the group to represent the value
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  u->act_eq = u->act_nd = avg;
  u->act = 0.0f;                // very important to clamp act to 0: don't send!
  u->da = 0.0f;                 // don't contribute to change in act
  return u->act_eq;
}

void ScalarValLayerSpec::ReadValue(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::Compute_ExtToPlus_ugp(LeabraLayer* lay,
                                               Layer::AccessMode acc_md, int gpidx,
                                               LeabraNetwork*) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(i > 0) u->act_p = us->clamp_range.Clip(u->ext);
    else u->act_p = u->ext;
    u->act_dif = u->act_p - u->act_m;
    // important to clear ext stuff, otherwise it will get added into netin next time around!!
    u->ext = 0.0f;
    u->ext_flag = Unit::NO_EXTERNAL;
  }
}

void ScalarValLayerSpec::Compute_ExtToAct_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraNetwork*) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(i > 0) u->act_eq = u->act = us->clamp_range.Clip(u->ext);
    else u->act_eq = u->ext;
    u->ext = 0.0f;
    u->ext_flag = Unit::NO_EXTERNAL;
  }
}

void ScalarValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_HardClamp(lay, net);
  ResetAfterClamp(lay, net);
}

void ScalarValLayerSpec::ResetAfterClamp_ugp(LeabraLayer* lay,
                                             Layer::AccessMode acc_md, int gpidx,
                                             LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits > 2) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
    u->act = 0.0f;              // must reset so it doesn't contribute!
    u->act_eq = u->act_nd = u->ext;     // avoid clamp_range!
  }
}

void ScalarValLayerSpec::ResetAfterClamp(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ResetAfterClamp_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                        LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  for(int i=1;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float cur = scalar.GetUnitVal(i);
    u->name = (String)cur;
  }
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  u->name = "val";              // overall value
}

void ScalarValLayerSpec::LabelUnits(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::LabelUnitsNet(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, net->layers) {
    if(l->spec.SPtr() == this)
      LabelUnits(l, net);
  }
}

void ScalarValLayerSpec::Settle_Init_Unit0_ugp(LeabraLayer* lay,
                                               Layer::AccessMode acc_md, int gpidx,
                                               LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits > 2) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
    u->Settle_Init_Unit(net);
  }
}

void ScalarValLayerSpec::Settle_Init_Unit0(LeabraLayer* lay, LeabraNetwork* net) {
  // very important: unit 0 in each layer is used for the netin scale parameter and
  // it is otherwise not computed on this unit b/c it is excluded from units_flat!
  // also the targflags need to be updated
  UNIT_GP_ITR(lay, Settle_Init_Unit0_ugp(lay, acc_md, gpidx, net); );
}

void ScalarValLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_Layer(lay, net);

  Settle_Init_Unit0(lay, net);

  if(bias_val.un == ScalarValBias::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;
    }
  }
}

void ScalarValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(scalar.clamp_pat) {
    inherited::Compute_HardClamp(lay, net);
    return;
  }
  if(!lay->HasExtFlag(Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, ClampValue_ugp(lay, acc_md, gpidx, net); );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void ScalarValLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  ReadValue(lay, net);          // always read out the value
}

float ScalarValLayerSpec::Compute_SSE_ugp(LeabraLayer* lay,
                                          Layer::AccessMode acc_md, int gpidx, int& n_vals) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(u->HasExtFlag(Unit::TARG | Unit::COMP) && val_range.RangeTestEq(u->targ)) {
    n_vals++;
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork*,
                                      int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->HasExtFlag(Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay, lay->sse += Compute_SSE_ugp(lay, acc_md, gpidx, n_vals); );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float ScalarValLayerSpec::Compute_NormErr_ugp(LeabraLayer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              LeabraInhib* thr, LeabraNetwork* net) {
  LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  // only count if target value is within range -- otherwise considered a non-target
  if(u->HasExtFlag(Unit::TARG | Unit::COMP) && val_range.RangeTestEq(u->targ)) {
    float uerr = u->targ - u->act_m;
    if(fabsf(uerr) < us->sse_tol)
      return 0.0f;
    return fabsf(uerr);
  }
  return 0.0f;
}

float ScalarValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!lay->HasExtFlag(Unit::TARG | Unit::COMP)) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      ntot += unit_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    ntot += unit_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     (lay->HasExtFlag(Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}


//////////////////////////////////
//      Scalar Value Self Prjn  //
//////////////////////////////////

void ScalarValSelfPrjnSpec::Initialize() {
  init_wts = true;
  width = 3;
  wt_width = 2.0f;
  wt_max = 1.0f;
}

void ScalarValSelfPrjnSpec::Connect_UnitGroup(Layer* lay,
                                              Layer::AccessMode acc_md, int gpidx,
                                              Projection* prjn) {
//   float neigh1 = 1.0f / wt_width;
//   float val1 = expf(-(neigh1 * neigh1));
//  float scale_val = wt_max / val1;

  int n_cons = 2*width + 1;
  int nunits = lay->UnitAccess_NUnits(acc_md);

  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(int i=0;i<nunits;i++) {
      LeabraUnit* ru = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);

      if(!alloc_loop)
        ru->RecvConsPreAlloc(n_cons, prjn);

      int j;
      for(j=-width;j<=width;j++) {
        int sidx = i+j;
        if((sidx < 0) || (sidx >= nunits)) continue;
        LeabraUnit* su = (LeabraUnit*)lay->UnitAccess(acc_md, sidx, gpidx);
        if(!self_con && (ru == su)) continue;
        ru->ConnectFromCk(su, prjn);
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void ScalarValSelfPrjnSpec::Connect_impl(Projection* prjn) {
  if(!prjn->from)       return;
  if(TestError(prjn->from.ptr() != prjn->layer, "Connect_impl", "must be used as a self-projection!")) {
    return;
  }

  Layer* lay = prjn->layer;
  UNIT_GP_ITR(lay, Connect_UnitGroup(lay, acc_md, gpidx, prjn); );
}

void ScalarValSelfPrjnSpec::C_Init_Weights(Projection*, RecvCons* cg, Unit* ru) {
  float neigh1 = 1.0f / wt_width;
  float val1 = expf(-(neigh1 * neigh1));
  float scale_val = wt_max / val1;

  int ru_idx = ru->idx;         // index within owning group

  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int su_idx = su->idx;
    float dist = (float)(ru_idx - su_idx) / wt_width;
    float wtval = scale_val * expf(-(dist * dist));
    cg->PtrCn(i)->wt = wtval;
  }
}

//////////////////////////////////
//      MotorForceLayerSpec     //
//////////////////////////////////

void MotorForceSpec::Initialize() {
  pos_width = .2f;
  vel_width = .2f;
  norm_width = true;
  clip_vals = true;

  cur_pos = cur_vel = 0.0f;
  pos_min = vel_min = 0.0f;
  pos_range = vel_range = 1.0f;

  pos_width_eff = pos_width;
  vel_width_eff = vel_width;
}

void MotorForceSpec::InitRanges(float pos_min_, float pos_range_, float vel_min_, float vel_range_) {
  pos_min = pos_min_;
  pos_range = pos_range_;
  vel_min = vel_min_;
  vel_range = vel_range_;
  pos_width_eff = pos_width;
  vel_width_eff = vel_width;
  if(norm_width) {
    pos_width_eff *= pos_range;
    vel_width_eff *= vel_range;
  }
}

void MotorForceSpec::InitVals(float pos, int pos_size, float pos_min_, float pos_range_,
                              float vel, int vel_size, float vel_min_, float vel_range_) {
  InitRanges(pos_min_, pos_range_, vel_min_, vel_range_);
  cur_pos = pos;
  pos_incr = pos_range / (float)(pos_size-1);
  cur_vel = vel;
  vel_incr = vel_range / (float)(vel_size-1);
}

float MotorForceSpec::GetWt(int pos_gp_idx, int vel_gp_idx) {
  float ug_pos = pos_min + pos_incr * (float)pos_gp_idx;
  float pos_dist = (ug_pos - cur_pos) / pos_width_eff;
  float ug_vel = vel_min + vel_incr * (float)vel_gp_idx;
  float vel_dist = (ug_vel - cur_vel) / vel_width_eff;
  return taMath_float::exp_fast(-(pos_dist * pos_dist + vel_dist * vel_dist));
}

void MotorForceLayerSpec::Initialize() {
  pos_range.min = 0.0f;
  pos_range.max = 2.0f;
  vel_range.min = -.1f;
  vel_range.max = .1f;
  add_noise = true;
  force_noise.type = Random::GAUSSIAN;
  force_noise.var = .01f;
}

void MotorForceLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  pos_range.UpdateAfterEdit_NoGui();
  vel_range.UpdateAfterEdit_NoGui();
  force_noise.UpdateAfterEdit_NoGui();
}

bool MotorForceLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);
  if(!rval) return rval;

  if(lay->CheckError(!lay->unit_groups, quiet, rval,
                "requires unit groups -- I just set it for you")) {
    lay->unit_groups = true;
  }
  if(lay->CheckError(lay->gp_geom.x < 3, quiet, rval,
                "requires at least 3 unit groups in x axis -- I just set it for you")) {
    lay->gp_geom.x = 5;
  }
  if(lay->CheckError(lay->gp_geom.y < 3, quiet, rval,
                "requires at least 3 unit groups in y axis -- I just set it for you")) {
    lay->gp_geom.y = 5;
  }
  return rval;
}

float MotorForceLayerSpec::ReadForce(LeabraLayer* lay, LeabraNetwork* net, float pos, float vel) {
  if(motor_force.clip_vals) {
    pos = pos_range.Clip(pos);
    vel = vel_range.Clip(vel);
  }
  motor_force.InitVals(pos, lay->gp_geom.x, pos_range.min, pos_range.range,
                       vel, lay->gp_geom.y, vel_range.min, vel_range.range);

  float force = 0.0f;
  float wt_sum = 0.0f;
  for(int y=0; y<lay->gp_geom.y; y++) {
    for(int x=0; x<lay->gp_geom.x; x++) {
      float wt = motor_force.GetWt(x,y);
      int gpidx = y * lay->gp_geom.x + x;
      LeabraUnit* un0 = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, 0, gpidx);
      force += wt * un0->act_eq;
      wt_sum += wt;
    }
  }
  if(wt_sum > 0.0f)
    force /= wt_sum;
  if(add_noise)
    force += force_noise.Gen();
  return force;
}

void MotorForceLayerSpec::ClampForce(LeabraLayer* lay, LeabraNetwork* net, float force, float pos, float vel) {
  if(motor_force.clip_vals) {
    pos = pos_range.Clip(pos);
    vel = vel_range.Clip(vel);
  }
  motor_force.InitVals(pos, lay->gp_geom.x, pos_range.min, pos_range.range,
                       vel, lay->gp_geom.y, vel_range.min, vel_range.range);

  for(int y=0; y<lay->gp_geom.y; y++) {
    for(int x=0; x<lay->gp_geom.x; x++) {
      float wt = motor_force.GetWt(x,y);
      int gpidx = y * lay->gp_geom.x + x;
      LeabraUnit* un0 = (LeabraUnit*)lay->UnitAccess(Layer::ACC_GP, 0, gpidx);
      un0->ext = force;
      ClampValue_ugp(lay, Layer::ACC_GP, gpidx, net, wt);
    }
  }
  lay->SetExtFlag(Unit::EXT);
  lay->hard_clamped = clamp.hard;
  HardClampExt(lay, net);
  scalar.clamp_pat = true;      // must have this to keep this clamped val
  UNIT_GP_ITR(lay,
              LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
              u->ext = 0.0f;            // must reset so it doesn't contribute!
              );
}

void MotorForceLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  float vel_mid = .5f * (float)(lay->gp_geom.y-1);
  float pos_mid = .5f * (float)(lay->gp_geom.x-1);
  for(int y=0; y<lay->gp_geom.y; y++) {
    float vel_dist = -((float)y - vel_mid) / vel_mid;
    for(int x=0; x<lay->gp_geom.x; x++) {
      float pos_dist = -((float)x - pos_mid) / pos_mid;
      float sum_val = .5f * vel_dist + .5f * pos_dist;
      int gpidx = y * lay->gp_geom.x + x;

      if(bias_val.un != ScalarValBias::NO_UN) {
        Compute_UnBias_Val(lay, Layer::ACC_GP, gpidx, sum_val);
      }
      if(bias_val.wt == ScalarValBias::WT) {
        Compute_WtBias_Val(lay, Layer::ACC_GP, gpidx, sum_val);
      }
    }
  }
}

//////////////////////////////////
//      TwoD Value Layer        //
//////////////////////////////////

void TwoDValLeabraLayer::Initialize() {
}

void TwoDValLeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateTwoDValsGeom();
}

void TwoDValLeabraLayer::UpdateTwoDValsGeom() {
  TwoDValLayerSpec* ls = (TwoDValLayerSpec*)GetLayerSpec();
  if(!ls) return;
  if(ls->InheritsFrom(&TA_TwoDValLayerSpec)) {
    if(unit_groups)
      twod_vals.SetGeom(5, 2, TWOD_N, ls->twod.n_vals, gp_geom.x, gp_geom.y);
    else
      twod_vals.SetGeom(5, 2, TWOD_N, ls->twod.n_vals, 1, 1);
  }
}

void TwoDValLeabraLayer::ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
                              Random* ran, const TwoDCoord& offs, bool na_by_range) {
  // only no unit_group supported!
  if(TestError(unit_groups, "ApplyInputData_2d",
               "input data must be 4d for layers with unit_groups: outer 2 are group dims, inner 2 are x,y vals and n_vals")) {
    return;
  }
  for(int d_y = 0; d_y < data->dim(1); d_y++) {
    int val_idx = offs.y + d_y;
    for(int d_x = 0; d_x < data->dim(0); d_x++) {
      int xy_idx = offs.x + d_x;
      Variant val = data->SafeElAsVar(d_x, d_y);
      if(ext_flags & Unit::EXT)
        twod_vals.SetFmVar(val, xy_idx, TWOD_EXT, val_idx, 0, 0);
      else
        twod_vals.SetFmVar(val, xy_idx, TWOD_TARG, val_idx, 0, 0);
    }
  }
}

void TwoDValLeabraLayer::ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
                                  Random* ran, const TwoDCoord& offs, bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  if(TestError(!unit_groups, "ApplyInputData_Flat4d",
               "input data must be 2d for layers without unit_groups: x,y vals and n_vals")) {
    return;
  }
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
          Unit* un = UnitAtCoord(u_x, u_y);
          if(un) {
            float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
            un->ApplyInputData(val, ext_flags, ran, na_by_range);
          }
        }
      }
    }
  }
}

void TwoDValLeabraLayer::ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags, Random* ran,
                                bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int val_idx = d_y;
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int xy_idx = d_x;
          Variant val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y);
          if(ext_flags & Unit::EXT)
            twod_vals.SetFmVar(val, xy_idx, TWOD_EXT, val_idx, dg_x, dg_y);
          else
            twod_vals.SetFmVar(val, xy_idx, TWOD_TARG, val_idx, dg_x, dg_y);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////
//              TwoDValLayerSpec

void TwoDValSpec::Initialize() {
  rep = GAUSSIAN;
  n_vals = 1;
  un_width = .3f;
  norm_width = false;
  clamp_pat = false;
  min_sum_act = 0.2f;
  mn_dst = 0.5f;
  clip_val = true;

  x_min = x_val = y_min = y_val = 0.0f;
  x_range = x_incr = y_range = y_incr = 1.0f;
  x_size = y_size = 1;
  un_width_x = un_width_y = un_width;
}

void TwoDValSpec::InitRange(float xmin, float xrng, float ymin, float yrng) {
  x_min = xmin; x_range = xrng; y_min = ymin; y_range = yrng;
  un_width_x = un_width;
  un_width_y = un_width;
  if(norm_width) {
    un_width_x *= x_range;
    un_width_y *= y_range;
  }
}

void TwoDValSpec::InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng) {
  InitRange(xmin, xrng, ymin, yrng);
  x_val = xval; y_val = yval;
  x_size = xsize; y_size = ysize;
  x_incr = x_range / (float)(x_size - 1); // DON'T skip 1st row, and count end..
  y_incr = y_range / (float)(y_size - 1); // DON'T skip 1st row, and count end..
  //  incr -= .000001f;         // round-off tolerance..
}

float TwoDValSpec::GetUnitAct(int unit_idx) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  if(rep == GAUSSIAN) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float x_dist = (x_cur - x_val) / un_width_x;
    float y_cur = y_min + y_incr * (float)y_idx;
    float y_dist = (y_cur - y_val) / un_width_y;
    float dist = x_dist * x_dist + y_dist * y_dist;
    return expf(-dist);
  }
  else if(rep == LOCALIST) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float y_cur = y_min + y_incr * (float)y_idx;
    float x_dist = fabs(x_val - x_cur);
    float y_dist = fabs(y_val - y_cur);
    if((x_dist > x_incr) && (y_dist > y_incr)) return 0.0f;

    return 1.0f - .5 * ((x_dist / x_incr) + (y_dist / y_incr)); // todo: no idea if this is right.
  }
  return 0.0f;
}

void TwoDValSpec::GetUnitVal(int unit_idx, float& x_cur, float& y_cur) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  x_cur = x_min + x_incr * (float)x_idx;
  y_cur = y_min + y_incr * (float)y_idx;
}

void TwoDValBias::Initialize() {
  un = NO_UN;
  un_gain = 1.0f;
  wt = NO_WT;
  wt_gain = 1.0f;
  x_val = 0.0f;
  y_val = 0.0f;
}

void TwoDValLayerSpec::Initialize() {
  min_obj_type = &TA_TwoDValLeabraLayer;

  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 9;
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 9;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;

  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateAfterEdit_NoGui();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateAfterEdit_NoGui();
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else if(twod.rep == TwoDValSpec::LOCALIST) {
    x_range.min = 0.0f;  x_range.max = 1.0f;  x_range.UpdateAfterEdit_NoGui();
    y_range.min = 0.0f;  y_range.max = 1.0f;  y_range.UpdateAfterEdit_NoGui();
    x_val_range.min = x_range.min;  x_val_range.max = x_range.max;
    y_val_range.min = y_range.min;  y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void TwoDValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  x_range.UpdateAfterEdit_NoGui(); y_range.UpdateAfterEdit_NoGui();
  twod.UpdateAfterEdit_NoGui();
  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else {
    x_val_range.min = x_range.min;    y_val_range.min = y_range.min;
    x_val_range.max = x_range.max;    y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void TwoDValLayerSpec::HelpConfig() {
  String help = "TwoDValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent two-dimensional values.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current twod value is encoded in the twod_vals member of the TwoDValLeabraLayer (x1,y1, x2,y2, etc),\
 which are set by input data, and updated to reflect current values encoded over layer.\
 For no unit groups case, input data should be 2d with inner dim of size 2 (x,y) and outer dim\
 of n_vals size.  For unit_groups, data should be 4d with two extra outer dims of gp_x, gp_y.\
 Provide the actual twod values in input data and it will automatically establish the \
 appropriate distributed representation in the rest of the units.\n\
 \nTwoDValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the TwoDValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool TwoDValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
                "coarse-coded twod representation requires at least 3 units, I just set un_geom.n")) {
    if(twod.rep == TwoDValSpec::LOCALIST) {
      lay->un_geom.n = 9;
      lay->un_geom.x = 3;
      lay->un_geom.y = 3;
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      lay->un_geom.n = 121;
      lay->un_geom.x = 11;
      lay->un_geom.y = 11;
    }
  }

  if(lay->InheritsFrom(&TA_TwoDValLeabraLayer)) { // inh will be flagged above
    ((TwoDValLeabraLayer*)lay)->UpdateTwoDValsGeom();
  }

  if(twod.rep == TwoDValSpec::LOCALIST) {
    kwta.k = 1;         // localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == TwoDValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->hyst.init, quiet, rval,
                  "bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
    }
    if(lay->CheckError(us->acc.init, quiet, rval,
                  "bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("acc", true);
      us->acc.init = false;
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "twod val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "twod val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "twod val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
        cs->SetUnique("lrate", true);
        cs->lrate = 0.0f;
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return rval;
}

void TwoDValLayerSpec::ReConfig(Network* net, int n_units) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr() != this) continue;

    if(n_units > 0) {
      lay->SetNUnits(n_units);
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);    // taking 1st unit as representative

    if(twod.rep == TwoDValSpec::LOCALIST) {
      twod.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = TwoDValBias::GC; bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = 0.0f; x_range.max = 1.0f;
      y_range.min = 0.0f; y_range.max = 1.0f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = TwoDValBias::GC;  bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = -.5f; x_range.max = 1.5f;
      y_range.min = -.5f; y_range.max = 1.5f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = true;
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void TwoDValLayerSpec::Compute_WtBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                          float x_val, float y_val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = .03f * bias_val.wt_gain * twod.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
         cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        LeabraCon* cn = (LeabraCon*)recv_gp->PtrCn(ci);
        cn->wt += act;
        if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
        if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u);
    }
  }
}

void TwoDValLayerSpec::Compute_UnBias_Val(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx,
                                          float x_val, float y_val) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float act = bias_val.un_gain * twod.GetUnitAct(i);
    if(bias_val.un == TwoDValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == TwoDValBias::BWT)
      u->bias.OwnCn(0)->wt = act;
  }
}

void TwoDValLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  if(bias_val.un != TwoDValBias::NO_UN) {
    UNIT_GP_ITR(lay, Compute_UnBias_Val(lay, acc_md, gpidx, bias_val.x_val, bias_val.y_val););
  }
  if(bias_val.wt == TwoDValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(lay, acc_md, gpidx, bias_val.x_val, bias_val.y_val););
  }
}

void TwoDValLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  Compute_BiasVal(lay, net);
}

void TwoDValLayerSpec::ClampValue_ugp(TwoDValLeabraLayer* lay,
                                      Layer::AccessMode acc_md, int gpidx,
                                      LeabraNetwork* net, float rescale) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  TwoDCoord gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  // first initialize to zero
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    u->SetExtFlag(Unit::EXT);
    u->ext = 0.0;
  }
  for(int k=0;k<twod.n_vals;k++) {
    float x_val = lay->GetTwoDVal(TwoDValLeabraLayer::TWOD_X, TwoDValLeabraLayer::TWOD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    float y_val = lay->GetTwoDVal(TwoDValLeabraLayer::TWOD_Y, TwoDValLeabraLayer::TWOD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    if(twod.clip_val) {
      x_val = x_val_range.Clip(x_val);
      y_val = y_val_range.Clip(y_val);
    }
    twod.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      float act = rescale * twod.GetUnitAct(i);
      if(act < us->opt_thresh.send)
        act = 0.0f;
      u->ext += act;
    }
  }
}

void TwoDValLayerSpec::ReadValue(TwoDValLeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, acc_md, gpidx, net); );
}

void TwoDValLayerSpec::ReadValue_ugp(TwoDValLeabraLayer* lay,
                                     Layer::AccessMode acc_md, int gpidx, LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  TwoDCoord gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  if(twod.n_vals == 1) {        // special case
    float x_avg = 0.0f; float y_avg = 0.0f;
    float sum_act = 0.0f;
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
      if(u->lesioned()) continue;
      float x_cur, y_cur;  twod.GetUnitVal(i, x_cur, y_cur);
      float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
      x_avg += x_cur * act_val;
      y_avg += y_cur * act_val;
      sum_act += act_val;
    }
    sum_act = MAX(sum_act, twod.min_sum_act);
    if(sum_act > 0.0f) {
      x_avg /= sum_act; y_avg /= sum_act;
    }
    // encode the value
    lay->SetTwoDVal(x_avg, TwoDValLeabraLayer::TWOD_X, TwoDValLeabraLayer::TWOD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
    lay->SetTwoDVal(y_avg, TwoDValLeabraLayer::TWOD_Y, TwoDValLeabraLayer::TWOD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
  }
  else {                        // multiple items
    // first find the max values, using sum of -1..+1 region
    static ValIdx_Array sort_ary;
    sort_ary.Reset();
    for(int i=0;i<nunits;i++) {
      float sum = 0.0f;
      float nsum = 0.0f;
      for(int x=-1;x<=1;x++) {
        for(int y=-1;y<=1;y++) {
          int idx = i + y * lay->un_geom.x + x;
          if(idx < 0 || idx >= nunits) continue;
          LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, idx, gpidx);
	  if(u->lesioned()) continue;
          float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
          nsum += 1.0f;
          sum += act_val;
        }
      }
      if(nsum > 0.0f) sum /= nsum;
      ValIdx vi(sum, i);
      sort_ary.Add(vi);
    }
    sort_ary.Sort();
    float mn_x = twod.mn_dst * twod.un_width_x * x_range.Range();
    float mn_y = twod.mn_dst * twod.un_width_y * y_range.Range();
    float mn_dist = mn_x * mn_x + mn_y * mn_y;
    int outi = 0;  int j = 0;
    while((outi < twod.n_vals) && (j < sort_ary.size)) {
      ValIdx& vi = sort_ary[sort_ary.size - j - 1]; // going backward through sort_ary
      float x_cur, y_cur;  twod.GetUnitVal(vi.idx, x_cur, y_cur);
      // check distance from all previous!
      float my_mn = x_range.Range() + y_range.Range();
      for(int k=0; k<j; k++) {
        ValIdx& vo = sort_ary[sort_ary.size - k - 1];
        if(vo.val == -1.0f) continue; // guy we skipped over before
        float x_prv, y_prv;  twod.GetUnitVal(vo.idx, x_prv, y_prv);
        float x_d = x_cur - x_prv; float y_d = y_cur - y_prv;
        float dist = x_d * x_d + y_d * y_d;
        my_mn = MIN(dist, my_mn);
      }
      if(my_mn < mn_dist) { vi.val = -1.0f; j++; continue; } // mark with -1 so we know we skipped it

      // encode the value
      lay->SetTwoDVal(x_cur, TwoDValLeabraLayer::TWOD_X, TwoDValLeabraLayer::TWOD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetTwoDVal(y_cur, TwoDValLeabraLayer::TWOD_Y, TwoDValLeabraLayer::TWOD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      j++; outi++;
    }
  }
}

void TwoDValLayerSpec::LabelUnits_ugp(LeabraLayer* lay, Layer::AccessMode acc_md, int gpidx) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    float x_cur, y_cur; twod.GetUnitVal(i, x_cur, y_cur);
    u->name = (String)x_cur + "," + String(y_cur);
  }
}

void TwoDValLayerSpec::LabelUnits(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(lay, acc_md, gpidx); );
}

void TwoDValLayerSpec::LabelUnitsNet(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, net->layers) {
    if(l->spec.SPtr() == this)
      LabelUnits(l, net);
  }
}

void TwoDValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_HardClamp(lay, net);
}

void TwoDValLayerSpec::Settle_Init_TargFlags_Layer_ugp(TwoDValLeabraLayer* lay,
                                                       Layer::AccessMode acc_md, int gpidx,
                                                       LeabraNetwork* net) {
  TwoDCoord gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  for(int k=0;k<twod.n_vals;k++) {
    float x_val, y_val;
    lay->GetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_TARG,
                     k, gp_geom_pos.x, gp_geom_pos.y);
    lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_EXT,
                     k, gp_geom_pos.x, gp_geom_pos.y);
  }
}


void TwoDValLayerSpec::Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_TargFlags_Layer(lay, net);
  // need to actually copy over targ to ext vals!
  TwoDValLeabraLayer* tdlay = (TwoDValLeabraLayer*)lay;
  if(lay->HasExtFlag(Unit::TARG)) {     // only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      UNIT_GP_ITR(lay, Settle_Init_TargFlags_Layer_ugp(tdlay, acc_md, gpidx, net); );
    }
  }
}


void TwoDValLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_Layer(lay, net);

  TwoDValLeabraLayer* tdlay = (TwoDValLeabraLayer*)lay;
  tdlay->UpdateTwoDValsGeom();  // quick, make sure no mismatch

  if(bias_val.un == TwoDValBias::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;              // keep a constant scaling so it doesn't depend on network size!
    }
  }
}

void TwoDValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(twod.clamp_pat) {
    inherited::Compute_HardClamp(lay, net);
    return;
  }
  if(!(lay->ext_flag & Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, ClampValue_ugp((TwoDValLeabraLayer*)lay, acc_md, gpidx, net); );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void TwoDValLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  ReadValue((TwoDValLeabraLayer*)lay, net);             // always read out the value
}

void TwoDValLayerSpec::PostSettle(LeabraLayer* ly, LeabraNetwork* net) {
  inherited::PostSettle(ly, net);
  TwoDValLeabraLayer* lay = (TwoDValLeabraLayer*)ly;
  UNIT_GP_ITR(lay, PostSettle_ugp(lay, acc_md, gpidx, net); );
}

void TwoDValLayerSpec::PostSettle_ugp(TwoDValLeabraLayer* lay,
                                      Layer::AccessMode acc_md, int gpidx,
                                      LeabraNetwork* net) {
  TwoDCoord gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);

  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  for(int k=0;k<twod.n_vals;k++) {
    float x_val, y_val, x_m, y_m, x_p, y_p;
    lay->GetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT,
                     k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetTwoDVals(x_m, y_m, TwoDValLeabraLayer::TWOD_ACT_M,
                     k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetTwoDVals(x_p, y_p, TwoDValLeabraLayer::TWOD_ACT_P,
                     k, gp_geom_pos.x, gp_geom_pos.y);

    switch(net->phase_order) {
    case LeabraNetwork::MINUS_PLUS:
      if(no_plus_testing) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(0.0f, 0.0f, TwoDValLeabraLayer::TWOD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetTwoDVals(x_val - x_m, y_val - y_m, TwoDValLeabraLayer::TWOD_ACT_DIF,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_MINUS:
      if(no_plus_testing) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(0.0f, 0.0f, TwoDValLeabraLayer::TWOD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetTwoDVals(x_p - x_val, y_p - y_val, TwoDValLeabraLayer::TWOD_ACT_DIF,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_ONLY:
      lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetTwoDVals(0.0f, 0.0f, TwoDValLeabraLayer::TWOD_ACT_DIF,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      break;
    case LeabraNetwork::MINUS_PLUS_NOTHING:
    case LeabraNetwork::MINUS_PLUS_MINUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val - x_m, y_val - y_m, TwoDValLeabraLayer::TWOD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        if(no_plus_testing) {
          // update act_m because it is actually another test case!
          lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      else {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_p - x_val, y_p - y_val, TwoDValLeabraLayer::TWOD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::PLUS_NOTHING:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_p - x_val, y_p - y_val, TwoDValLeabraLayer::TWOD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::MINUS_PLUS_PLUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val - x_m, y_val - y_m, TwoDValLeabraLayer::TWOD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val - x_p, y_val - y_p, TwoDValLeabraLayer::TWOD_ACT_DIF2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::MINUS_PLUS_PLUS_NOTHING:
    case LeabraNetwork::MINUS_PLUS_PLUS_MINUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val - x_m, y_val - y_m, TwoDValLeabraLayer::TWOD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 2) {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_P2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_val - x_p, y_val - y_p, TwoDValLeabraLayer::TWOD_ACT_DIF2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetTwoDVals(x_val, y_val, TwoDValLeabraLayer::TWOD_ACT_M2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetTwoDVals(x_p - x_val, y_p - y_val, TwoDValLeabraLayer::TWOD_ACT_DIF2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    }
  }
}

float TwoDValLayerSpec::Compute_SSE_ugp(LeabraLayer* ly,
                                        Layer::AccessMode acc_md, int gpidx,
                                        int& n_vals) {
  TwoDValLeabraLayer* lay = (TwoDValLeabraLayer*)ly;
  TwoDCoord gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
    float x_targ, y_targ;
    lay->GetTwoDVals(x_targ, y_targ, TwoDValLeabraLayer::TWOD_TARG, k,
                     gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      n_vals++;
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<twod.n_vals;j++) {
        float x_act_m, y_act_m;
        lay->GetTwoDVals(x_act_m, y_act_m, TwoDValLeabraLayer::TWOD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = dx * dx + dy * dy;
        if(dist < mn_dist) {
          mn_dist = dist;
          lay->SetTwoDVals(dx, dy, TwoDValLeabraLayer::TWOD_ERR,
                           k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetTwoDVals(dx*dx, dy*dy, TwoDValLeabraLayer::TWOD_SQERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float TwoDValLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork*,
                                    int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay,
              lay->sse += Compute_SSE_ugp(lay, acc_md, gpidx, n_vals);
              );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float TwoDValLayerSpec::Compute_NormErr_ugp(LeabraLayer* ly,
                                            Layer::AccessMode acc_md, int gpidx,
                                            LeabraInhib* thr, LeabraNetwork* net) {
  TwoDValLeabraLayer* lay = (TwoDValLeabraLayer*)ly;
  TwoDCoord gp_geom_pos = lay->UnitGpPosFmIdx(gpidx);
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<twod.n_vals;k++) { // first loop over and find potential target values
    float x_targ, y_targ;
    lay->GetTwoDVals(x_targ, y_targ, TwoDValLeabraLayer::TWOD_TARG, k,
                     gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<twod.n_vals;j++) {
        float x_act_m, y_act_m;
        lay->GetTwoDVals(x_act_m, y_act_m, TwoDValLeabraLayer::TWOD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = fabsf(dx) + fabsf(dy); // only diff from sse!
        if(dist < mn_dist)
          mn_dist = dist;
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float TwoDValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && lay->unit_groups) {
    for(int g=0; g < lay->gp_geom.n; g++) {
      LeabraUnGpData* gpd = lay->ungp_data.FastEl(g);
      nerr += Compute_NormErr_ugp(lay, Layer::ACC_GP, g, (LeabraInhib*)gpd, net);
      ntot += x_range.range + y_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, Layer::ACC_LAY, 0, (LeabraInhib*)lay, net);
    ntot += x_range.range + y_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

///////////////////////////////////////////////////////////////
//   DecodeTwoDValLayerSpec

void DecodeTwoDValLayerSpec::Initialize() {
}

void DecodeTwoDValLayerSpec::Compute_Inhib(LeabraLayer*, LeabraNetwork*) {
  return;                       // do nothing!
}

void DecodeTwoDValLayerSpec::ReadValue_ugp(TwoDValLeabraLayer* lay,
                                           Layer::AccessMode acc_md, int gpidx,
                                           LeabraNetwork* net) {
  int nunits = lay->UnitAccess_NUnits(acc_md);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, i, gpidx);
    if(u->lesioned()) continue;
    if(u->recv.size == 0) continue;
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv[0];
    if(cg->size == 0) continue;
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    u->net = su->net;
    u->act = su->act;
    u->act_eq = su->act_eq;
    u->act_nd = su->act_nd;
  }
  inherited::ReadValue_ugp(lay, acc_md, gpidx, net);
}

/*

//////////////////////////////////
//      FourD Value Layer       //
//////////////////////////////////

void FourDValLeabraLayer::Initialize() {
}

void FourDValLeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateFourDValsGeom();
}

void FourDValLeabraLayer::UpdateFourDValsGeom() {
  FourDValLayerSpec* ls = (FourDValLayerSpec*)GetLayerSpec();
  if(!ls) return;
  if(ls->InheritsFrom(&TA_FourDValLayerSpec)) {
    if(unit_groups)
      fourd_vals.SetGeom(5, 2, FOURD_N, ls->fourd.n_vals, gp_geom.x, gp_geom.y);
    else
      fourd_vals.SetGeom(5, 2, FOURD_N, ls->fourd.n_vals, 1, 1);
  }
}

void FourDValLeabraLayer::ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
                              Random* ran, const TwoDCoord& offs, bool na_by_range) {
  // only no unit_group supported!
  if(TestError(unit_groups, "ApplyInputData_2d",
               "input data must be 4d for layers with unit_groups: outer 2 are group dims, inner 2 are x,y vals and n_vals")) {
    return;
  }
  for(int d_y = 0; d_y < data->dim(1); d_y++) {
    int val_idx = offs.y + d_y;
    for(int d_x = 0; d_x < data->dim(0); d_x++) {
      int xy_idx = offs.x + d_x;
      Variant val = data->SafeElAsVar(d_x, d_y);
      if(ext_flags & Unit::EXT)
        fourd_vals.SetFmVar(val, xy_idx, FOURD_EXT, val_idx, 0, 0);
      else
        fourd_vals.SetFmVar(val, xy_idx, FOURD_TARG, val_idx, 0, 0);
    }
  }
}

void FourDValLeabraLayer::ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
                                  Random* ran, const TwoDCoord& offs, bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  if(TestError(!unit_groups, "ApplyInputData_Flat4d",
               "input data must be 2d for layers without unit_groups: x,y vals and n_vals")) {
    return;
  }
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
          Unit* un = UnitAtCoord(u_x, u_y);
          if(un) {
            float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
            un->ApplyInputData(val, ext_flags, ran, na_by_range);
          }
        }
      }
    }
  }
}

void FourDValLeabraLayer::ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags, Random* ran,
                                bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int val_idx = d_y;
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int xy_idx = d_x;
          Variant val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y);
          if(ext_flags & Unit::EXT)
            fourd_vals.SetFmVar(val, xy_idx, FOURD_EXT, val_idx, dg_x, dg_y);
          else
            fourd_vals.SetFmVar(val, xy_idx, FOURD_TARG, val_idx, dg_x, dg_y);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////
//              FourDValLayerSpec

void FourDValSpec::Initialize() {
  rep = GAUSSIAN;
  n_vals = 1;
  un_width = .3f;
  norm_width = false;
  clamp_pat = false;
  min_sum_act = 0.2f;
  mn_dst = 0.5f;
  clip_val = true;

  x_min = x_val = y_min = y_val = 0.0f;
  x_range = x_incr = y_range = y_incr = 1.0f;
  x_size = y_size = 1;
  un_width_x = un_width_y = un_width;
}

void FourDValSpec::InitRange(float xmin, float xrng, float ymin, float yrng) {
  x_min = xmin; x_range = xrng; y_min = ymin; y_range = yrng;
  un_width_x = un_width;
  un_width_y = un_width;
  if(norm_width) {
    un_width_x *= x_range;
    un_width_y *= y_range;
  }
}

void FourDValSpec::InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng) {
  InitRange(xmin, xrng, ymin, yrng);
  x_val = xval; y_val = yval;
  x_size = xsize; y_size = ysize;
  x_incr = x_range / (float)(x_size - 1); // DON'T skip 1st row, and count end..
  y_incr = y_range / (float)(y_size - 1); // DON'T skip 1st row, and count end..
  //  incr -= .000001f;         // round-off tolerance..
}

float FourDValSpec::GetUnitAct(int unit_idx) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  if(rep == GAUSSIAN) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float x_dist = (x_cur - x_val) / un_width_x;
    float y_cur = y_min + y_incr * (float)y_idx;
    float y_dist = (y_cur - y_val) / un_width_y;
    float dist = x_dist * x_dist + y_dist * y_dist;
    return expf(-dist);
  }
  else if(rep == LOCALIST) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float y_cur = y_min + y_incr * (float)y_idx;
    float x_dist = fabs(x_val - x_cur);
    float y_dist = fabs(y_val - y_cur);
    if((x_dist > x_incr) && (y_dist > y_incr)) return 0.0f;

    return 1.0f - .5 * ((x_dist / x_incr) + (y_dist / y_incr)); // todo: no idea if this is right.
  }
  return 0.0f;
}

void FourDValSpec::GetUnitVal(int unit_idx, float& x_cur, float& y_cur) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  x_cur = x_min + x_incr * (float)x_idx;
  y_cur = y_min + y_incr * (float)y_idx;
}

void FourDValBias::Initialize() {
  un = NO_UN;
  un_gain = 1.0f;
  wt = NO_WT;
  wt_gain = 1.0f;
  x_val = 0.0f;
  y_val = 0.0f;
}

void FourDValLayerSpec::Initialize() {
  min_obj_type = &TA_FourDValLeabraLayer;

  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 9;
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 9;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;

  if(fourd.rep == FourDValSpec::GAUSSIAN) {
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateAfterEdit_NoGui();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateAfterEdit_NoGui();
    fourd.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * fourd.un_width_x);
    x_val_range.max = x_range.max - (.5f * fourd.un_width_x);
    y_val_range.min = y_range.min + (.5f * fourd.un_width_y);
    y_val_range.max = y_range.max - (.5f * fourd.un_width_y);
  }
  else if(fourd.rep == FourDValSpec::LOCALIST) {
    x_range.min = 0.0f;  x_range.max = 1.0f;  x_range.UpdateAfterEdit_NoGui();
    y_range.min = 0.0f;  y_range.max = 1.0f;  y_range.UpdateAfterEdit_NoGui();
    x_val_range.min = x_range.min;  x_val_range.max = x_range.max;
    y_val_range.min = y_range.min;  y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void FourDValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  x_range.UpdateAfterEdit_NoGui(); y_range.UpdateAfterEdit_NoGui();
  fourd.UpdateAfterEdit_NoGui();
  if(fourd.rep == FourDValSpec::GAUSSIAN) {
    fourd.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * fourd.un_width_x);
    y_val_range.min = y_range.min + (.5f * fourd.un_width_y);
    x_val_range.max = x_range.max - (.5f * fourd.un_width_x);
    y_val_range.max = y_range.max - (.5f * fourd.un_width_y);
  }
  else {
    x_val_range.min = x_range.min;    y_val_range.min = y_range.min;
    x_val_range.max = x_range.max;    y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void FourDValLayerSpec::HelpConfig() {
  String help = "FourDValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent two-dimensional values.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current fourd value is encoded in the fourd_vals member of the FourDValLeabraLayer (x1,y1, x2,y2, etc),\
 which are set by input data, and updated to reflect current values encoded over layer.\
 For no unit groups case, input data should be 2d with inner dim of size 2 (x,y) and outer dim\
 of n_vals size.  For unit_groups, data should be 4d with two extra outer dims of gp_x, gp_y.\
 Provide the actual fourd values in input data and it will automatically establish the \
 appropriate distributed representation in the rest of the units.\n\
 \nFourDValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the FourDValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool FourDValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
                "coarse-coded fourd representation requires at least 3 units, I just set un_geom.n")) {
    if(fourd.rep == FourDValSpec::LOCALIST) {
      lay->un_geom.n = 9;
      lay->un_geom.x = 3;
      lay->un_geom.y = 3;
    }
    else if(fourd.rep == FourDValSpec::GAUSSIAN) {
      lay->un_geom.n = 121;
      lay->un_geom.x = 11;
      lay->un_geom.y = 11;
    }
  }

  if(lay->InheritsFrom(&TA_FourDValLeabraLayer)) { // inh will be flagged above
    ((FourDValLeabraLayer*)lay)->UpdateFourDValsGeom();
  }

  if(fourd.rep == FourDValSpec::LOCALIST) {
    kwta.k = 1;         // localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == FourDValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->hyst.init, quiet, rval,
                  "bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
    }
    if(lay->CheckError(us->acc.init, quiet, rval,
                  "bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("acc", true);
      us->acc.init = false;
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "fourd val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "fourd val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "fourd val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
        cs->SetUnique("lrate", true);
        cs->lrate = 0.0f;
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return rval;
}

void FourDValLayerSpec::ReConfig(Network* net, int n_units) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr() != this) continue;

    if(n_units > 0) {
      lay->SetNUnits(n_units);
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);    // taking 1st unit as representative

    if(fourd.rep == FourDValSpec::LOCALIST) {
      fourd.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = FourDValBias::GC; bias_val.wt = FourDValBias::NO_WT;
      x_range.min = 0.0f; x_range.max = 1.0f;
      y_range.min = 0.0f; y_range.max = 1.0f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    else if(fourd.rep == FourDValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = FourDValBias::GC;  bias_val.wt = FourDValBias::NO_WT;
      x_range.min = -.5f; x_range.max = 1.5f;
      y_range.min = -.5f; y_range.max = 1.5f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = true;
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void FourDValLayerSpec::Compute_WtBias_Val(Unit_Group* ugp, float x_val, float y_val) {
  if(nunits < 3) return;        // must be at least a few units..
  Layer* lay = ugp->own_lay;
  fourd.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = .03f * bias_val.wt_gain * fourd.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
         cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        LeabraCon* cn = (LeabraCon*)recv_gp->Cn(ci);
        cn->wt += act;
        if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
        if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u);
    }
  }
}

void FourDValLayerSpec::Compute_UnBias_Val(Unit_Group* ugp, float x_val, float y_val) {
  if(nunits < 3) return;        // must be at least a few units..
  Layer* lay = ugp->own_lay;
  fourd.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = bias_val.un_gain * fourd.GetUnitAct(i);
    if(bias_val.un == FourDValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == FourDValBias::BWT)
      u->bias.OwnCn(0)->wt = act;
  }
}

void FourDValLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  if(bias_val.un != FourDValBias::NO_UN) {
    UNIT_GP_ITR(lay, Compute_UnBias_Val(ugp, bias_val.x_val, bias_val.y_val););
  }
  if(bias_val.wt == FourDValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(ugp, bias_val.x_val, bias_val.y_val););
  }
}

void FourDValLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  Compute_BiasVal(lay, net);
}

void FourDValLayerSpec::ClampValue_ugp(Unit_Group* ugp, LeabraNetwork*, float rescale) {
  if(nunits < 3) return;        // must be at least a few units..
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ugp->own_lay;
  TwoDCoord gp_geom_pos = ugp->GpLogPos();
  // first initialize to zero
  LeabraUnitSpec* us = (LeabraUnitSpec*)ugp->FastEl(0)->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->SetExtFlag(Unit::EXT);
    u->ext = 0.0;
  }
  for(int k=0;k<fourd.n_vals;k++) {
    float x_val = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    float y_val = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    if(fourd.clip_val) {
      x_val = x_val_range.Clip(x_val);
      y_val = y_val_range.Clip(y_val);
    }
    fourd.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      float act = rescale * fourd.GetUnitAct(i);
      if(act < us->opt_thresh.send)
        act = 0.0f;
      u->ext += act;
    }
  }
}

void FourDValLayerSpec::ReadValue(FourDValLeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, ugp, net); );
}

void FourDValLayerSpec::ReadValue_ugp(FourDValLeabraLayer* lay, Unit_Group* ugp, LeabraNetwork* net) {
  if(nunits < 3) return;        // must be at least a few units..
  fourd.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  TwoDCoord gp_geom_pos = ugp->GpLogPos();
  if(fourd.n_vals == 1) {       // special case
    float x_avg = 0.0f; float y_avg = 0.0f;
    float sum_act = 0.0f;
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      float x_cur, y_cur;  fourd.GetUnitVal(i, x_cur, y_cur);
      float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
      x_avg += x_cur * act_val;
      y_avg += y_cur * act_val;
      sum_act += act_val;
    }
    sum_act = MAX(sum_act, fourd.min_sum_act);
    if(sum_act > 0.0f) {
      x_avg /= sum_act; y_avg /= sum_act;
    }
    // encode the value
    lay->SetFourDVal(x_avg, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
    lay->SetFourDVal(y_avg, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
  }
  else {                        // multiple items
    // first find the max values, using sum of -1..+1 region
    static ValIdx_Array sort_ary;
    sort_ary.Reset();
    for(int i=0;i<nunits;i++) {
      float sum = 0.0f;
      float nsum = 0.0f;
      for(int x=-1;x<=1;x++) {
        for(int y=-1;y<=1;y++) {
          int idx = i + y * lay->un_geom.x + x;
          if(idx < 0 || idx >= nunits) continue;
          LeabraUnit* u = (LeabraUnit*)ugp->FastEl(idx);
          LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
          float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
          nsum += 1.0f;
          sum += act_val;
        }
      }
      if(nsum > 0.0f) sum /= nsum;
      ValIdx vi(sum, i);
      sort_ary.Add(vi);
    }
    sort_ary.Sort();
    float mn_x = fourd.mn_dst * fourd.un_width_x * x_range.Range();
    float mn_y = fourd.mn_dst * fourd.un_width_y * y_range.Range();
    float mn_dist = mn_x * mn_x + mn_y * mn_y;
    int outi = 0;  int j = 0;
    while((outi < fourd.n_vals) && (j < sort_ary.size)) {
      ValIdx& vi = sort_ary[sort_ary.size - j - 1]; // going backward through sort_ary
      float x_cur, y_cur;  fourd.GetUnitVal(vi.idx, x_cur, y_cur);
      // check distance from all previous!
      float my_mn = x_range.Range() + y_range.Range();
      for(int k=0; k<j; k++) {
        ValIdx& vo = sort_ary[sort_ary.size - k - 1];
        if(vo.val == -1.0f) continue; // guy we skipped over before
        float x_prv, y_prv;  fourd.GetUnitVal(vo.idx, x_prv, y_prv);
        float x_d = x_cur - x_prv; float y_d = y_cur - y_prv;
        float dist = x_d * x_d + y_d * y_d;
        my_mn = MIN(dist, my_mn);
      }
      if(my_mn < mn_dist) { vi.val = -1.0f; j++; continue; } // mark with -1 so we know we skipped it

      // encode the value
      lay->SetFourDVal(x_cur, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetFourDVal(y_cur, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      j++; outi++;
    }
  }
}

void FourDValLayerSpec::LabelUnits_ugp(Unit_Group* ugp) {
  if(nunits < 3) return;        // must be at least a few units..
  Layer* lay = ugp->own_lay;
  fourd.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float x_cur, y_cur; fourd.GetUnitVal(i, x_cur, y_cur);
    u->name = (String)x_cur + "," + String(y_cur);
  }
}

void FourDValLayerSpec::LabelUnits(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(ugp); );
}

void FourDValLayerSpec::LabelUnitsNet(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, net->layers) {
    if(l->spec.SPtr() == this)
      LabelUnits(l, net);
  }
}

void FourDValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_HardClamp(lay, net);
}

void FourDValLayerSpec::Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_TargFlags_Layer(lay, net);
  // need to actually copy over targ to ext vals!
  FourDValLeabraLayer* tdlay = (FourDValLeabraLayer*)lay;
  if(lay->ext_flag & Unit::TARG) {      // only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      UNIT_GP_ITR(tdlay,
                  for(int k=0;k<fourd.n_vals;k++) {
                    TwoDCoord gp_geom_pos = ugp->GpLogPos();
                    float x_val = tdlay->GetFourDVal(FourDValLeabraLayer::FOURD_X,
                                                    FourDValLeabraLayer::FOURD_TARG,
                                                    k, gp_geom_pos.x, gp_geom_pos.y);
                    float y_val = tdlay->GetFourDVal(FourDValLeabraLayer::FOURD_Y,
                                                    FourDValLeabraLayer::FOURD_TARG,
                                                    k, gp_geom_pos.x, gp_geom_pos.y);
                    tdlay->SetFourDVal(x_val, FourDValLeabraLayer::FOURD_X,
                                      FourDValLeabraLayer::FOURD_EXT,
                                      k, gp_geom_pos.x, gp_geom_pos.y);
                    tdlay->SetFourDVal(y_val, FourDValLeabraLayer::FOURD_Y,
                                      FourDValLeabraLayer::FOURD_EXT,
                                      k, gp_geom_pos.x, gp_geom_pos.y);
                  }
                  );
    }
  }
}


void FourDValLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_Layer(lay, net);

  FourDValLeabraLayer* tdlay = (FourDValLeabraLayer*)lay;
  tdlay->UpdateFourDValsGeom(); // quick, make sure no mismatch

  if(bias_val.un == ScalarValBias::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;              // keep a constant scaling so it doesn't depend on network size!
    }
  }
}

void FourDValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(fourd.clamp_pat) {
    inherited::Compute_HardClamp(lay, net);
    return;
  }
  if(!(lay->ext_flag & Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, if(nunits > 2) { ClampValue_ugp(ugp, net); } );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void FourDValLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  ReadValue((FourDValLeabraLayer*)lay, net);            // always read out the value
}

void FourDValLayerSpec::PostSettle(LeabraLayer* ly, LeabraNetwork* net) {
  inherited::PostSettle(ly, net);
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ly;
  UNIT_GP_ITR(lay, PostSettle_ugp(lay, ugp, net); );
}

void FourDValLayerSpec::PostSettle_ugp(FourDValLeabraLayer* lay, Unit_Group* ugp, LeabraNetwork* net) {
  TwoDCoord gp_geom_pos = ugp->GpLogPos();

  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  for(int k=0;k<fourd.n_vals;k++) {
    float x_val, y_val, x_m, y_m, x_p, y_p;
    lay->GetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT,
                      k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetFourDVals(x_m, y_m, FourDValLeabraLayer::FOURD_ACT_M,
                      k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetFourDVals(x_p, y_p, FourDValLeabraLayer::FOURD_ACT_P,
                      k, gp_geom_pos.x, gp_geom_pos.y);

    switch(net->phase_order) {
    case LeabraNetwork::MINUS_PLUS:
      if(no_plus_testing) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(0.0f, 0.0f, FourDValLeabraLayer::FOURD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVals(x_val - x_m, y_val - y_m, FourDValLeabraLayer::FOURD_ACT_DIF,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_MINUS:
      if(no_plus_testing) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(0.0f, 0.0f, FourDValLeabraLayer::FOURD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVals(x_p - x_val, y_p - y_val, FourDValLeabraLayer::FOURD_ACT_DIF,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_ONLY:
      lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetFourDVals(0.0f, 0.0f, FourDValLeabraLayer::FOURD_ACT_DIF,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      break;
    case LeabraNetwork::MINUS_PLUS_NOTHING:
    case LeabraNetwork::MINUS_PLUS_MINUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val - x_m, y_val - y_m, FourDValLeabraLayer::FOURD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        if(no_plus_testing) {
          // update act_m because it is actually another test case!
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      else {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_p - x_val, y_p - y_val, FourDValLeabraLayer::FOURD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::PLUS_NOTHING:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_p - x_val, y_p - y_val, FourDValLeabraLayer::FOURD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::MINUS_PLUS_PLUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val - x_m, y_val - y_m, FourDValLeabraLayer::FOURD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val - x_p, y_val - y_p, FourDValLeabraLayer::FOURD_ACT_DIF2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::MINUS_PLUS_PLUS_NOTHING:
    case LeabraNetwork::MINUS_PLUS_PLUS_MINUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val - x_m, y_val - y_m, FourDValLeabraLayer::FOURD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 2) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val - x_p, y_val - y_p, FourDValLeabraLayer::FOURD_ACT_DIF2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_p - x_val, y_p - y_val, FourDValLeabraLayer::FOURD_ACT_DIF2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    }
  }
}

float FourDValLayerSpec::Compute_SSE_ugp(Unit_Group* ugp, LeabraLayer* ly, int& n_vals) {
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ly;
  TwoDCoord gp_geom_pos = ugp->GpLogPos();
  LeabraUnitSpec* us = (LeabraUnitSpec*)ugp->FastEl(0)->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<fourd.n_vals;k++) { // first loop over and find potential target values
    float x_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_TARG, k,
                                   gp_geom_pos.x, gp_geom_pos.y);
    float y_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_TARG, k,
                                   gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      n_vals++;
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<fourd.n_vals;j++) {
        float x_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float y_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = dx * dx + dy * dy;
        if(dist < mn_dist) {
          mn_dist = dist;
          lay->SetFourDVal(dx, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVal(dy, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVal(dx*dx, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_SQERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVal(dy*dy, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_SQERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float FourDValLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork*,
                                    int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay,
              lay->sse += Compute_SSE_ugp(ugp, lay, n_vals);
              );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float FourDValLayerSpec::Compute_NormErr_ugp(LeabraLayer* ly, Unit_Group* ugp,
                                           LeabraInhib* thr, LeabraNetwork* net) {
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ly;
  TwoDCoord gp_geom_pos = ugp->GpLogPos();
  LeabraUnitSpec* us = (LeabraUnitSpec*)ugp->FastEl(0)->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<fourd.n_vals;k++) { // first loop over and find potential target values
    float x_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_TARG,
                                   k, gp_geom_pos.x, gp_geom_pos.y);
    float y_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_TARG,
                                   k, gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<fourd.n_vals;j++) {
        float x_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float y_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = fabsf(dx) + fabsf(dy); // only diff from sse!
        if(dist < mn_dist)
          mn_dist = dist;
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float FourDValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && (lay->unit_groups)) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      nerr += Compute_NormErr_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      ntot += x_range.range + y_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
    ntot += x_range.range + y_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

*/

////////////////////////////////////////////////////////////
//      V1RFPrjnSpec

// void V1RFPrjnSpec::Initialize() {
//   init_wts = true;
//   wrap = false;
//   dog_surr_mult = 1.0f;
// }

// void V1RFPrjnSpec::UpdateAfterEdit_impl() {
//   inherited::UpdateAfterEdit_impl();
//   rf_spec.name = name + "_rf_spec";
// }

// void V1RFPrjnSpec::Connect_impl(Projection* prjn) {
//   if(!(bool)prjn->from)      return;
//   if(prjn->layer->units.leaves == 0) // an empty layer!
//     return;
//   if(TestWarning(prjn->layer->units.gp.size == 0, "Connect_impl",
//               "requires recv layer to have unit groups!")) {
//     return;
//   }

//   rf_spec.InitFilters();     // this one call initializes all filter info once and for all!
//   // renorm the dog net filter to 1 abs max!
//   if(rf_spec.filter_type == GaborV1Spec::BLOB) {
//     for(int i=0;i<rf_spec.blob_specs.size;i++) {
//       DoGFilter* df = (DoGFilter*)rf_spec.blob_specs.FastEl(i);
//       taMath_float::vec_norm_abs_max(&(df->net_filter));
//     }
//   }
//   TestWarning(rf_spec.n_filters != prjn->layer->un_geom.n,
//            "number of filters from rf_spec:", (String)rf_spec.n_filters,
//            "does not match layer un_geom.n:", (String)prjn->layer->un_geom.n);

//   TwoDCoord rf_width = rf_spec.rf_width;
//   int n_cons = rf_width.Product();
//   TwoDCoord rf_half_wd = rf_width / 2;
//   TwoDCoord ru_geo = prjn->layer->gp_geom;

//   TwoDCoord su_geo = prjn->from->un_geom;

//   TwoDCoord ruc;
//   for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
//     for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
//       for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++) {

//      Unit_Group* ru_gp = prjn->layer->UnitGpAtCoord(ruc);
//      if(ru_gp == NULL) continue;

//      TwoDCoord su_st;
//      if(wrap) {
//        su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
//        su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
//      }
//      else {
//        su_st.x = (int)floor((float)ruc.x * rf_move.x);
//        su_st.y = (int)floor((float)ruc.y * rf_move.y);
//      }

//      su_st.WrapClip(wrap, su_geo);
//      TwoDCoord su_ed = su_st + rf_width;
//      if(wrap) {
//        su_ed.WrapClip(wrap, su_geo); // just wrap ends too
//      }
//      else {
//        if(su_ed.x > su_geo.x) {
//          su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
//        }
//        if(su_ed.y > su_geo.y) {
//          su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
//        }
//      }

//      for(int rui=0;rui<ru_gp->size;rui++) {
//        Unit* ru_u = (Unit*)ru_gp->FastEl(rui);
//        if(!alloc_loop)
//          ru_u->RecvConsPreAlloc(n_cons, prjn);

//        TwoDCoord suc;
//        TwoDCoord suc_wrp;
//        for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
//          for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
//            suc_wrp = su_st + suc;
//            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
//              continue;
//            Unit* su_u = prjn->from->UnitAtCoord(suc_wrp);
//            if(su_u == NULL) continue;
//            if(!self_con && (su_u == ru_u)) continue;
//            ru_u->ConnectFrom(su_u, prjn, alloc_loop); // don't check: saves lots of time!
//          }
//        }
//      }
//       }
//     }
//     if(alloc_loop) { // on first pass through alloc loop, do sending allocations
//       prjn->from->SendConsPostAlloc(prjn);
//     }
//   }
// }

// void V1RFPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
//   Unit_Group* rugp = (Unit_Group*)ru->GetOwner();
//   int recv_idx = ru->pos.y * rugp->geom.x + ru->pos.x;

//   bool on_rf = true;
//   if(prjn->from->name.contains("_off"))
//     on_rf = false;
//   DoGFilter::ColorChannel col_chan = DoGFilter::BLACK_WHITE;
//   if(prjn->from->name.contains("_rg_"))
//     col_chan = DoGFilter::RED_GREEN;
//   else if(prjn->from->name.contains("_by_"))
//     col_chan = DoGFilter::BLUE_YELLOW;

//   int send_x = rf_spec.rf_width.x;
//   if(rf_spec.filter_type == GaborV1Spec::BLOB) {
//     // color is outer-most dimension, and if it doesn't match, then bail
//     int clr_dx = (recv_idx / (rf_spec.blob_rf.n_sizes * 2) % 2);
//     DoGFilter* df = (DoGFilter*)rf_spec.blob_specs.SafeEl(recv_idx);
//     if(!df) return;          // oops
//     if(col_chan != DoGFilter::BLACK_WHITE) {
//       // outer-most mod is color, after phases (2) and sizes (inner)
//       if((clr_dx == 0 && col_chan == DoGFilter::BLUE_YELLOW) ||
//       (clr_dx == 1 && col_chan == DoGFilter::RED_GREEN)) {
//      for(int i=0; i<cg->size; i++)
//        cg->Cn(i)->wt = 0.0f;
//      return;                 // bail if not our channel.
//       }
//     }
//     for(int i=0; i<cg->size; i++) {
//       int su_x = i % send_x;
//       int su_y = i / send_x;
//       float val = rf_spec.gabor_rf.amp * df->net_filter.SafeEl(su_x, su_y);
//       if(on_rf) {
//      if(df->on_sigma > df->off_sigma) val *= dog_surr_mult;
//      if(val > 0.0f) cg->Cn(i)->wt = val;
//      else           cg->Cn(i)->wt = 0.0f;
//       }
//       else {
//      if(df->off_sigma > df->on_sigma) val *= dog_surr_mult;
//      if(val < 0.0f)  cg->Cn(i)->wt = -val;
//      else            cg->Cn(i)->wt = 0.0f;
//       }
//     }
//   }
//   else {                     // GABOR
//     GaborFilterSpec* gf = (GaborFilterSpec*)rf_spec.gabor_specs.SafeEl(recv_idx);
//     if(!gf) return;          // oops
//     for(int i=0; i<cg->size; i++) {
//       int su_x = i % send_x;
//       int su_y = i / send_x;
//       float val = gf->filter.SafeEl(su_x, su_y);
//       if(on_rf) {
//      if(val > 0.0f) cg->Cn(i)->wt = val;
//      else           cg->Cn(i)->wt = 0.0f;
//       }
//       else {
//      if(val < 0.0f)  cg->Cn(i)->wt = -val;
//      else            cg->Cn(i)->wt = 0.0f;
//       }
//     }
//   }
// }

// bool V1RFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
//   trg_send_geom.x = send_x;
//   trg_send_geom.y = send_y;

//   if(wrap)
//     trg_recv_geom = (trg_send_geom / rf_move);
//   else
//     trg_recv_geom = (trg_send_geom / rf_move) - 1;

//   // now fix it the other way
//   if(wrap)
//     trg_send_geom = (trg_recv_geom * rf_move);
//   else
//     trg_send_geom = ((trg_recv_geom +1) * rf_move);

//   DataChanged(DCR_ITEM_UPDATED);
//   return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
// }

// bool V1RFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
//   trg_recv_geom.x = recv_x;
//   trg_recv_geom.y = recv_y;

//   if(wrap)
//     trg_send_geom = (trg_recv_geom * rf_move);
//   else
//     trg_send_geom = ((trg_recv_geom+1) * rf_move);

//   // now fix it the other way
//   if(wrap)
//     trg_recv_geom = (trg_send_geom / rf_move);
//   else
//     trg_recv_geom = (trg_send_geom / rf_move) - 1;

//   DataChanged(DCR_ITEM_UPDATED);
//   return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
// }

// void V1RFPrjnSpec::GraphFilter(DataTable* graph_data, int recv_unit_no) {
//   rf_spec.GraphFilter(graph_data, recv_unit_no);
// }

// void V1RFPrjnSpec::GridFilter(DataTable* graph_data) {
//   rf_spec.GridFilter(graph_data);
// }


////////////////////////////////////////////////////////////
//      LeabraExtOnlyUnitSpec

void LeabraExtOnlyUnitSpec::Initialize() {

}

void LeabraExtOnlyUnitSpec::Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  if(lay->hard_clamped) return;

  if(net->inhib_cons_used) {
    u->g_i_raw += u->g_i_delta;
    if(act_fun == SPIKE) {
      u->gc.i = MAX(u->g_i_raw, 0.0f);
      Compute_NetinInteg_Spike_i(u, net);
    }
    else {
      u->gc.i = u->prv_g_i + dt.net * (u->g_i_raw - u->prv_g_i);
      u->prv_g_i = u->gc.i;
    }
  }

  u->net_raw += u->net_delta;
  float tot_net = (u->bias_scale * u->bias.OwnCn(0)->wt) + u->net_raw;

  if(u->HasExtFlag(Unit::EXT)) {
    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    float extin = u->ext * ls->clamp.gain;
    if(extin < opt_thresh.send)
      tot_net = 0.0f;           // not allowed to activate
    else
      tot_net += extin;
  }
  else {
    // get first projection, first connection
    LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(TestError(!cg, "Compute_NetinInteg", "requires one recv projection!")) {
      return;
    }
    LeabraUnit* su = (LeabraUnit*)cg->Un(0);
    if(TestError(!su, "Compute_NetinInteg", "requires one unit in recv projection!")) {
      return;
    }
    float extin = su->act_eq;
    if(extin < opt_thresh.send)
      tot_net = 0.0f;           // veto
  }

  u->net_delta = 0.0f;  // clear for next use
  u->g_i_delta = 0.0f;  // clear for next use

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = MAX(tot_net, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u,net);
  }
  else {
    float dnet = dt.net * (tot_net - u->prv_net);
    u->net = u->prv_net + dnet;
    u->prv_net = u->net;
    u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
  }

  // add just before computing i_thr -- after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += Compute_Noise(u, net);
  }
  u->i_thr = Compute_IThresh(u, net);
}


////////////////////////////////////////////////////////////
//      SaliencyPrjnSpec

void SaliencyPrjnSpec::Initialize() {
  //  init_wts = true;
  convergence = 1;
  reciprocal = false;
  feat_only = true;
  feat_gps = 2;
  dog_wts.filter_width = 3;
  dog_wts.filter_size = 7;
  dog_wts.on_sigma = 1;
  dog_wts.off_sigma = 2;
  wt_mult = 1.0f;
  surr_mult = 1.0f;
  units_per_feat_gp = 4;
}

void SaliencyPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires sending layer to have unit groups!")) {
    return;
  }
  if(feat_only)
    Connect_feat_only(prjn);
  else
    Connect_full_dog(prjn);
}

void SaliencyPrjnSpec::Connect_feat_only(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(reciprocal) {
    recv_lay = prjn->from;
    send_lay = prjn->layer;

    FOREACH_ELEM_IN_GROUP(Unit, su, send_lay->units) {
      su->RecvConsPreAlloc(1, prjn); // only ever have 1!
    }
  }

  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord ruu_geo = recv_lay->un_geom;
  TwoDCoord su_geo = send_lay->gp_geom;

  int fltsz = convergence;
  int sg_sz_tot = fltsz * fltsz;

  int feat_no = 0;
  TwoDCoord rug;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(rug.y = 0; rug.y < rug_geo.y; rug.y++) {
      for(rug.x = 0; rug.x < rug_geo.x; rug.x++, feat_no++) {
        Unit_Group* ru_gp = recv_lay->UnitGpAtCoord(rug);
        if(!ru_gp) continue;

        int rui = 0;
        TwoDCoord ruc;
        for(ruc.y = 0; ruc.y < ruu_geo.y; ruc.y++) {
          for(ruc.x = 0; ruc.x < ruu_geo.x; ruc.x++, rui++) {

            TwoDCoord su_st = ruc*convergence;

            Unit* ru_u = (Unit*)ru_gp->SafeEl(rui);
            if(!ru_u) break;
            if(!reciprocal && !alloc_loop)
              ru_u->RecvConsPreAlloc(sg_sz_tot, prjn);

            TwoDCoord suc;
            for(suc.y = 0; suc.y < fltsz; suc.y++) {
              for(suc.x = 0; suc.x < fltsz; suc.x++) {
                TwoDCoord sugc = su_st + suc;
                Unit_Group* su_gp = send_lay->UnitGpAtCoord(sugc);
                if(!su_gp) continue;

                Unit* su_u = (Unit*)su_gp->SafeEl(feat_no);
                if(su_u) {
                  if(reciprocal)
                    su_u->ConnectFrom(ru_u, prjn, alloc_loop);
                  else
                    ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                }
              }
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void SaliencyPrjnSpec::Connect_full_dog(Projection* prjn) {
  dog_wts.UpdateFilter();
  taMath_float::vec_norm_abs_max(&(dog_wts.net_filter)); // renorm to abs max = 1

  if(TestError(reciprocal, "Connect_full_dog", "full DoG connection not supported in reciprocal mode!!!")) return;

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord ruu_geo = recv_lay->un_geom;
  TwoDCoord su_geo = send_lay->gp_geom;

  int fltwd = dog_wts.filter_width; // no convergence..
  int fltsz = dog_wts.filter_size * convergence;

  int sg_sz_tot = fltsz * fltsz;
  Unit_Group* su_gp0 = (Unit_Group*)send_lay->units.gp[0];
  int alloc_no = sg_sz_tot * su_gp0->size;

  units_per_feat_gp = su_gp0->size / feat_gps;

  int feat_no = 0;
  TwoDCoord rug;
  for(rug.y = 0; rug.y < rug_geo.y; rug.y++) {
    for(rug.x = 0; rug.x < rug_geo.x; rug.x++, feat_no++) {
      Unit_Group* ru_gp = recv_lay->UnitGpAtCoord(rug);
      if(!ru_gp) continue;

      int rui = 0;
      TwoDCoord ruc;
      for(ruc.y = 0; ruc.y < ruu_geo.y; ruc.y++) {
        for(ruc.x = 0; ruc.x < ruu_geo.x; ruc.x++, rui++) {

          TwoDCoord su_st = ruc*convergence - convergence*fltwd;

          Unit* ru_u = (Unit*)ru_gp->SafeEl(rui);
          if(!ru_u) break;
          ru_u->RecvConsPreAlloc(alloc_no, prjn);

          TwoDCoord suc;
          for(suc.y = 0; suc.y < fltsz; suc.y++) {
            for(suc.x = 0; suc.x < fltsz; suc.x++) {
              TwoDCoord sugc = su_st + suc;
              Unit_Group* su_gp = send_lay->UnitGpAtCoord(sugc);
              if(!su_gp) continue;

              for(int sui=0;sui<su_gp->size;sui++) {
                Unit* su_u = (Unit*)su_gp->FastEl(sui);
                if(!self_con && (su_u == ru_u)) continue;
                ru_u->ConnectFrom(su_u, prjn);
              }
            }
          }
        }
      }
    }
  }
}

void SaliencyPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  if(feat_only) {               // just use regular..
    inherited::C_Init_Weights(prjn, cg, ru);
    return;
  }
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int fltwd = dog_wts.filter_width; // no convergence.
  int fltsz = dog_wts.filter_size * convergence;

  Unit_Group* su_gp0 = (Unit_Group*)send_lay->units.gp[0];
  units_per_feat_gp = su_gp0->size / feat_gps;

  Unit_Group* rugp = (Unit_Group*)ru->GetOwner();

  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord rgp_pos = rugp->GpLogPos();

  int feat_no = rgp_pos.y * rug_geo.x + rgp_pos.x; // unit group index
  int my_feat_gp = feat_no / units_per_feat_gp;
  int fg_st = my_feat_gp * units_per_feat_gp;
  int fg_ed = fg_st + units_per_feat_gp;

  TwoDCoord ruu_geo = recv_lay->un_geom;
  TwoDCoord su_st;              // su starting (left)
  su_st.x = (ru->idx % ruu_geo.x)*convergence - convergence*fltwd;
  su_st.y = (ru->idx / ruu_geo.x)*convergence - convergence*fltwd;

  TwoDCoord su_geo = send_lay->gp_geom;

  int su_idx = 0;
  TwoDCoord suc;
  for(suc.y = 0; suc.y < fltsz; suc.y++) {
    for(suc.x = 0; suc.x < fltsz; suc.x++) {
      TwoDCoord sugc = su_st + suc;
      Unit_Group* su_gp = send_lay->UnitGpAtCoord(sugc);
      if(!su_gp) continue;

      float wt = wt_mult * dog_wts.net_filter.FastEl(suc.x/convergence,
                                                     suc.y/convergence);

      if(wt > 0) {
        for(int sui=0;sui<su_gp->size;sui++) {
          if(sui == feat_no)
            cg->Cn(su_idx++)->wt = wt; // target feature
          else
            cg->Cn(su_idx++)->wt = 0.0f; // everyone else
        }
      }
      else {
        for(int sui=0;sui<su_gp->size;sui++) {
          if(sui != feat_no && sui >= fg_st && sui < fg_ed)
            cg->Cn(su_idx++)->wt = -surr_mult * wt;
          else
            cg->Cn(su_idx++)->wt = 0.0f; // not in our group or is guy itself
        }
      }
    }
  }
}

void SaliencyPrjnSpec::GraphFilter(DataTable* graph_data) {
  dog_wts.GraphFilter(graph_data);
}

void SaliencyPrjnSpec::GridFilter(DataTable* graph_data) {
  dog_wts.GridFilter(graph_data);
}


//////////////////////////////////////////////////////////
//              V1LateralContourPrjnSpec

void V1LateralContourPrjnSpec::Initialize() {
  radius = 4;
  wrap = true;
  ang_pow = 4.0f;
  dist_sigma = 1.0f;
  con_thr = 0.2f;
  oth_feat_wt = 0.5f;
  init_wts = true;

}

void V1LateralContourPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void V1LateralContourPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }
  if(TestWarning(prjn->from != prjn->layer, "Connect_impl",
                 "requires send and recv to be the same layer -- lateral projection!")) {
    return;
  }
  if(TestWarning(prjn->con_spec->wt_limits.sym, "Connect_impl",
                 "cannot have wt_limits.sym on in conspec -- turning off in spec:",
                 prjn->con_spec->name,
                 "This might affect children of this spec.")) {
    prjn->con_spec->SetUnique("wt_limits", true);
    prjn->con_spec->wt_limits.sym = false;
    prjn->con_spec->UpdateAfterEdit();
  }

  Layer* lay = prjn->from;
  TwoDCoord gp_geo = lay->gp_geom;
  TwoDCoord un_geo = lay->un_geom;
  float n_angles = (float)un_geo.x;

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {

        TwoDCoord suc;
        TwoDCoord suc_wrp;
        for(suc.y = ruc.y-radius; suc.y <= ruc.y+radius; suc.y++) {
          for(suc.x = ruc.x-radius; suc.x <= ruc.x+radius; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, gp_geo) && !wrap)
              continue;
            int sgpidx = lay->UnitGpIdxFmPos(suc_wrp);
            if(!lay->UnitGpIdxIsValid(sgpidx)) continue;

            TwoDCoord del = suc - ruc; // don't use wrap!
            float dst = del.Mag();
            if(dst > (float)radius) continue; // out of bounds
            if(dst == 0.0f) continue;         // no selfs

            float nrmdst = dst / (float)radius;
            float gang = atan2f(del.y, del.x); // group angle
            if(gang >= taMath_float::pi) gang -= taMath_float::pi;
            if(gang < 0.0f) gang += taMath_float::pi;

            float gauswt = taMath_float::gauss_den_nonorm(nrmdst, dist_sigma);

            TwoDCoord run;
            for(run.x = 0; run.x < un_geo.x; run.x++) {
              float rang = taMath_float::pi * ((float)run.x / n_angles);
              float gangwt = powf(fabsf(cosf(gang-rang)), ang_pow);

              TwoDCoord sun;
              for(sun.x = 0; sun.x < un_geo.x; sun.x++) {
                float sang = taMath_float::pi * ((float)sun.x / n_angles);
                float sangwt = powf(fabsf(cosf(sang-rang)), ang_pow);
                float wt = sangwt * gangwt * gauswt;
                if(wt < con_thr) continue;

                for(run.y = 0; run.y < un_geo.y; run.y++) {
                  for(sun.y = 0; sun.y < un_geo.y; sun.y++) {
                    int rui = run.y * un_geo.x + run.x;
                    int sui = sun.y * un_geo.x + sun.x;

                    float feat_wt = 1.0f;
                    if(run.y != sun.y)
                      feat_wt = oth_feat_wt;
                    float eff_wt = wt * feat_wt;
                    if(eff_wt < con_thr) continue;

                    Unit* ru_u = lay->UnitAtUnGpIdx(rui, rgpidx);
                    if(!ru_u) continue;
                    Unit* su_u = lay->UnitAtUnGpIdx(sui, sgpidx);
                    if(!su_u) continue;
                    if(alloc_loop) {
                      ru_u->RecvConsAllocInc(1, prjn);
                      su_u->SendConsAllocInc(1, prjn);
                    }
                    else {
                      Connection* cn = ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                      cn->wt = eff_wt;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do allocations
      prjn->layer->RecvConsPostAlloc(prjn);
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void V1LateralContourPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* lay = prjn->from;
  TwoDCoord gp_geo = lay->gp_geom;
  TwoDCoord un_geo = lay->un_geom;
  float n_angles = (float)un_geo.x;

  TwoDCoord gp_geo_half = gp_geo / 2;

  int rgpidx;
  int rui;
  lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
  TwoDCoord ruc = lay->UnitGpPosFmIdx(rgpidx);
  TwoDCoord run;
  run.SetFmIndex(rui, un_geo.x);
  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int sgpidx;
    int sui;
    lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
    TwoDCoord suc = lay->UnitGpPosFmIdx(sgpidx);
    TwoDCoord sun;
    sun.SetFmIndex(sui, un_geo.x);

    TwoDCoord del = suc - ruc; // don't use wrap!
    if(wrap) {                 // dist may be closer in wrapped case..
      suc.WrapMinDist(del, gp_geo, ruc, gp_geo_half);
    }

    float dst = del.Mag();
    float nrmdst = dst / (float)radius;
    float gang = atan2f((float)del.y, (float)del.x); // group angle
    if(gang >= taMath_float::pi) gang -= taMath_float::pi;
    if(gang < 0.0f) gang += taMath_float::pi;
    float rang = taMath_float::pi * ((float)run.x / n_angles);
    float sang = taMath_float::pi * ((float)sun.x / n_angles);
    float sangwt = powf(fabsf(cosf(sang-rang)), ang_pow);
    float gangwt = powf(fabsf(cosf(gang-rang)), ang_pow);
    float gauswt = taMath_float::gauss_den_nonorm(nrmdst, dist_sigma);
    float feat_wt = 1.0f;
    if(run.y != sun.y)
      feat_wt = oth_feat_wt;
    float wt = feat_wt * sangwt * gangwt * gauswt;
    cg->Cn(i)->wt = wt;
  }
}


//////////////////////////////////////////////////////////
//              V2BoLateralPrjnSpec

void V2BoLateralPrjnSpec::Initialize() {
  init_wts = true;

  radius = 8;
  wrap = true;
  t_on = true;
  opp_on = true;
  ang_sig = 0.5f;
  dist_sig_line = 0.8f;
  dist_sig_oth = 0.5f;
  line_mag = 0.8f;
  weak_mag = 0.5f;
  con_thr = 0.2f;
  test_ang = -1;
}

void V2BoLateralPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

// static int dbg_hit_cnt = 0;

void V2BoLateralPrjnSpec::CreateStencils() {
  int n_angles = 4;
  int max_cnt = (2 * radius + 1);
  v2ffbo_weights.SetGeom(6, max_cnt, max_cnt, 2, n_angles, 2, n_angles);
  TwoDCoord suc;                        // send coords
  for(int rang_dx = 0; rang_dx < n_angles; rang_dx++) {
    for(int rdir = 0; rdir < 2; rdir++) {
      for(int sang_dx = 0; sang_dx < n_angles; sang_dx++) {
        for(suc.y = -radius; suc.y <= radius; suc.y++) {
          int ysuc_dx = suc.y + radius;
          for(suc.x = -radius; suc.x <= radius; suc.x++) {
            int xsuc_dx = suc.x + radius;
            for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
              float wt = ConWt(suc, rang_dx, sang_dx, rdir, sdir);
              v2ffbo_weights.FastEl(xsuc_dx, ysuc_dx, sdir, sang_dx, rdir, rang_dx) = wt;
            }
          }
        }
      }
    }
  }
}

float V2BoLateralPrjnSpec::ConWt(TwoDCoord& suc, int rang_dx, int sang_dx, int rdir, int sdir) {
  float n_angles = 4.0f;
  // integer angles -- useful for precise conditionals..
  int rang_n = rang_dx + rdir * 4;
  int sang_n = sang_dx + sdir * 4;
  int dang_n;
  if(sang_n < rang_n)
    dang_n = (8 + sang_n) - rang_n;
  else
    dang_n = sang_n - rang_n;
  int dang_n_pi = dang_n;
  if(dang_n >= 4) dang_n_pi = 8 - dang_n;
  int abs_dang_n_pi = dang_n_pi < 0 ? -dang_n_pi : dang_n_pi;

  if(!opp_on && dang_n == 4) return 0.0f;       // no opposite angle cons

  if((test_ang >= 0) && (dang_n != test_ang))
    return 0.0f; // don't continue

  TwoDCoord del = suc;
  float dst = del.Mag();
  if(dst > (float)radius) return 0.0f;
  if(dst == 0.0f) return 0.0f;  // no self con
  float nrmdst = dst / (float)radius;

  float gang = atan2f(del.y, del.x); // group angle -- 0..pi or -pi
  if(gang < 0.0f) gang += 2.0f * taMath_float::pi; // keep it positive

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float rang = taMath_float::pi * ((float)rang_dx / n_angles) + taMath_float::pi * (float)rdir;
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < rang)
    dang = (2.0f * taMath_float::pi + sang) - rang;
  else
    dang = sang - rang;
  float dang_pi = dang; // this determines type of projection -- equal fabs(dang_pi) are same type
  if(dang >= taMath_float::pi) dang_pi = (2.0f * taMath_float::pi) - dang;
  float abs_dang_pi = fabs(dang_pi);

  float op_mag = 0.0f;
  if(abs_dang_pi < 0.499f * taMath_float::pi)
    op_mag = ((0.5f * taMath_float::pi - abs_dang_pi) / (0.5f * taMath_float::pi)); // 1 for 0, .5 for 45

  float tang = rang + 0.5f * dang; // target angle

  float gtang = gang - tang;
  if(gtang > taMath_float::pi) gtang -= 2.0f * taMath_float::pi;
  if(gtang < -taMath_float::pi) gtang += 2.0f * taMath_float::pi;

  // make symmetric around half sphere
  bool op_side = false;
  if(gtang > taMath_float::pi * 0.5f) { gtang -= taMath_float::pi; op_side = true; }
  if(gtang < -taMath_float::pi * 0.5f){ gtang += taMath_float::pi; op_side = true; }

  float eff_mag = 1.0f;
  if(abs_dang_pi > 0.501f * taMath_float::pi) eff_mag = weak_mag;

  float netwt = eff_mag * taMath_float::gauss_den_nonorm(gtang, ang_sig);
  if(abs_dang_n_pi == 0)
    netwt *= line_mag * taMath_float::gauss_den_nonorm(nrmdst, dist_sig_line);
  else
    netwt *= taMath_float::gauss_den_nonorm(nrmdst, dist_sig_oth);

  if(op_side)
    netwt *= op_mag;

  if(t_on && abs_dang_n_pi == 2 && dst <= 2.9f) {
    float grang = gang - rang;
//     if(dbg_hit_cnt++ <= 100) {
//      taMisc::Info("rang_n: " + String(rang_n), "  sang_n: " + String(sang_n),
//                "   dang_n: " + String(dang_n), "  dang_n_pi: " + String(dang_n_pi),
//                "gang: " + String(gang), "  grang:" + String(grang));
//     }
    if(fabsf(grang - (1.5f * taMath_float::pi)) < .1f ||
       fabsf(grang - (-0.5f * taMath_float::pi)) < .1f) {
      netwt = 1.0f;
    }
  }

  return netwt;
}

void V2BoLateralPrjnSpec::Connect_impl(Projection* prjn) {
//   dbg_hit_cnt = 0;           // debugging

  if(!(bool)prjn->from) return;

  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }
  if(TestWarning(prjn->from != prjn->layer, "Connect_impl",
                 "requires send and recv to be the same layer -- lateral projection!")) {
    return;
  }
  if(TestWarning(prjn->con_spec->wt_limits.sym, "Connect_impl",
                 "cannot have wt_limits.sym on in conspec -- turning off in spec:",
                 prjn->con_spec->name,
                 "This might affect children of this spec.")) {
    prjn->con_spec->SetUnique("wt_limits", true);
    prjn->con_spec->wt_limits.sym = false;
    prjn->con_spec->UpdateAfterEdit();
  }

  CreateStencils();

  Layer* lay = prjn->from;
  TwoDCoord gp_geo = lay->gp_geom;
  TwoDCoord un_geo = lay->un_geom;
  float n_angles = (float)un_geo.x;

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {
        TwoDCoord suc;
        TwoDCoord suc_wrp;
        for(suc.y = ruc.y-radius; suc.y <= ruc.y+radius; suc.y++) {
          for(suc.x = ruc.x-radius; suc.x <= ruc.x+radius; suc.x++) {
            suc_wrp = suc;
            if(suc_wrp.WrapClip(wrap, gp_geo) && !wrap)
              continue;
            int sgpidx = lay->UnitGpIdxFmPos(suc_wrp);
            if(!lay->UnitGpIdxIsValid(sgpidx)) continue;

            TwoDCoord del = suc - ruc; // don't use wrap!
            float dst = del.Mag();
            if(dst > (float)radius) continue; // out of bounds
            if(dst == 0.0f) continue;         // no selfs

            TwoDCoord run;
            TwoDCoord sun;

            for(run.x = 0; run.x < un_geo.x; run.x++) {
              for(sun.x = 0; sun.x < un_geo.x; sun.x++) {
                for(run.y = 0; run.y < un_geo.y; run.y++) {
                  int syst = (run.y / 2) * 2;
                  int syed = syst+2;
                  for(sun.y = syst; sun.y < syed; sun.y++) {
                    // mod 2 on y allows for multiple depths to be replicated
                    float wt = v2ffbo_weights.FastEl(del.x +radius, del.y+radius,
                                                     sun.y % 2, sun.x, run.y % 2, run.x);
                    if(wt <= con_thr) continue;

                    int rui = run.y * un_geo.x + run.x;
                    int sui = sun.y * un_geo.x + sun.x;

                    Unit* ru_u = lay->UnitAtUnGpIdx(rui, rgpidx);
                    if(!ru_u) continue;
                    Unit* su_u = lay->UnitAtUnGpIdx(sui, sgpidx);
                    if(!su_u) continue;
                    if(alloc_loop) {
                      ru_u->RecvConsAllocInc(1, prjn);
                      su_u->SendConsAllocInc(1, prjn);
                    }
                    else {
                      Connection* cn = ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                      cn->wt = wt;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do allocations
      prjn->layer->RecvConsPostAlloc(prjn);
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void V2BoLateralPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* lay = prjn->from;
  TwoDCoord gp_geo = lay->gp_geom;
  TwoDCoord un_geo = lay->un_geom;
  float n_angles = (float)un_geo.x;

  TwoDCoord gp_geo_half = gp_geo / 2;

  int rgpidx;
  int rui;
  lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
  TwoDCoord ruc = lay->UnitGpPosFmIdx(rgpidx);
  TwoDCoord run;
  run.SetFmIndex(rui, un_geo.x);
  for(int i=0; i<cg->size; i++) {
    Unit* su = cg->Un(i);
    int sgpidx;
    int sui;
    lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
    TwoDCoord suc = lay->UnitGpPosFmIdx(sgpidx);
    TwoDCoord sun;
    sun.SetFmIndex(sui, un_geo.x);

    TwoDCoord del = suc - ruc;
    if(wrap) {                 // dist may be closer in wrapped case..
      suc.WrapMinDist(del, gp_geo, ruc, gp_geo_half);
    }

    float wt = v2ffbo_weights.FastEl(del.x + radius, del.y + radius,
                                     sun.y % 2, sun.x, run.y % 2, run.x);
    cg->Cn(i)->wt = wt;
  }
}



//////////////////////////////////////////////////////////
//              FgBoEllipseGpPrjnSpec

void FgBoEllipseGpPrjnEl::Initialize() {
  con_radius = 14;
  wt_radius = 0.7f;
  dist_sig = 0.3f;
  ang_sig = 1.0f;
  ellipse_ratio = 1.0f;
  ellipse_angle = 0.0f;
  max_wt = 1.0f;
  min_wt = 0.1f;
  con_thr = 0.2f;
}

void FgBoEllipseGpPrjnEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CreateStencil();
}

void FgBoEllipseGpPrjnEl::CreateStencil() {
  int n_angles = 4;
  int max_cnt = (2 * con_radius + 1);
  fgbo_weights.SetGeom(4, max_cnt, max_cnt, 2, n_angles);
  TwoDCoord suc;                        // send coords
  int cnt = 0;
  for(suc.y = -con_radius; suc.y <= con_radius; suc.y++) {
    int ysuc_dx = suc.y + con_radius;
    for(suc.x = -con_radius; suc.x <= con_radius; suc.x++) {
      int xsuc_dx = suc.x + con_radius;
      for(int sang_dx = 0; sang_dx < n_angles; sang_dx++) {
        for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
          float wt = ConWt(suc, sang_dx, sdir);
          fgbo_weights.FastEl(xsuc_dx, ysuc_dx, sdir, sang_dx) = wt;
        }
      }
    }
  }
}

float FgBoEllipseGpPrjnEl::ConWt(TwoDCoord& suc, int sang_dx, int sdir) {
  float n_angles = 4.0f;

  TwoDCoord del = suc;
  float dst = del.Mag();
  if(dst > (float)con_radius) return 0.0f;
  if(dst == 0.0f) return 0.0f;  // no self con
  float nrmdst = dst / (float)con_radius;

  float gang = atan2f(del.y, del.x); // group angle -- 0..pi or -pi
  if(gang < 0.0f) gang += 2.0f * taMath_float::pi; // keep it positive

  float pang = gang + 0.5f * taMath_float::pi; // perpendicular angle
  if(pang > 2.0f * taMath_float::pi) pang -= 2.0f * taMath_float::pi;

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < pang)
    dang = (2.0f * taMath_float::pi + sang) - pang;
  else
    dang = sang - pang;
  if(dang >= taMath_float::pi) dang = (2.0f * taMath_float::pi) - dang;

  if(ellipse_ratio < 1.0f) {
    float a = (float)con_radius;
    float b = ellipse_ratio * a;
    float elang = ellipse_angle * taMath_float::rad_per_deg;
    float R = (b*b - a*a) * cosf(2.0f * gang - 2.0f * elang) + a*a + b*b;
    float Q = sqrtf(2) * a * b * sqrtf(R);
    float r = Q / R;
    nrmdst = dst / r;           // normalize relative to outer radius!
  }

  float netwt = max_wt * taMath_float::gauss_den_nonorm(dang, ang_sig)
    * taMath_float::gauss_den_nonorm((nrmdst-wt_radius), dist_sig);

  return netwt;
}

void FgBoEllipseGpPrjnSpec::Initialize() {
  init_wts = true;
  wrap = true;
  reciprocal = false;
  group_specs.SetBaseType(&TA_FgBoEllipseGpPrjnEl);
}

void FgBoEllipseGpPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CreateStencils();
}

// static int dbg_hit_cnt = 0;

FgBoEllipseGpPrjnEl* FgBoEllipseGpPrjnSpec::NewGroupSpec() {
  return (FgBoEllipseGpPrjnEl*)group_specs.New(1);
}

void FgBoEllipseGpPrjnSpec::CreateStencils() {
  for(int i=0; i < group_specs.size; i++) {
    FgBoEllipseGpPrjnEl* el = group_specs.FastEl(i);
    el->CreateStencil();
  }
}

void FgBoEllipseGpPrjnSpec::Connect_impl(Projection* prjn) {
//   dbg_hit_cnt = 0;           // debugging

  if(!(bool)prjn->from) return;

  if(TestWarning(group_specs.size == 0, "Connect_impl",
                 "group_specs must have at least one item!")) {
    return;
  }
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }
//   if(TestWarning(prjn->con_spec->wt_limits.sym, "Connect_impl",
//               "cannot have wt_limits.sym on in conspec -- turning off in spec:",
//               prjn->con_spec->name,
//               "This might affect children of this spec.")) {
//     prjn->con_spec->SetUnique("wt_limits", true);
//     prjn->con_spec->wt_limits.sym = false;
//     prjn->con_spec->UpdateAfterEdit();
//   }

  CreateStencils();

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {              // switch!
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }
  TwoDCoord rgp_geo = recv_lay->gp_geom;
  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sgp_geo = send_lay->gp_geom;
  TwoDCoord sun_geo = send_lay->un_geom;

  int n_depth = sun_geo.y / 2;  // number of different depths represented
  int ruy_per_depth = run_geo.y / n_depth;
  int run_per_depth = recv_lay->un_geom.n / n_depth;
  if(TestWarning(ruy_per_depth * n_depth != run_geo.y, "Connect_impl",
                 "recv layer un_geom.y must be even multiple of number of depths represented in the input:", String(n_depth))) {
    return;
  }

  float n_angles = (float)sun_geo.x;

  if(TestWarning(run_per_depth != group_specs.size, "Connect_impl",
                 "recv layer units per depth is not same as number of group specs -- should be -- redundant or incomplete connections will be created")) {
  }

  TwoDCoord rgp_sc = sgp_geo / rgp_geo;
  TwoDCoord rgp_chk = rgp_geo * rgp_sc;

  if(TestWarning(rgp_chk != sgp_geo, "Connect_impl",
                 "recv layer gp_geom is not an even multiple of send layer gp_geom -- should be!  Some units will not be connected", rgp_sc.GetStr(), rgp_chk.GetStr())) {
  }


  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    int rcnt = 0;
    TwoDCoord run;
    for(run.y = 0; run.y < run_geo.y; run.y++) {
      int depth = run.y / ruy_per_depth;
      int suy_st = depth * 2;
      for(run.x = 0; run.x < run_geo.x; run.x++, rcnt++) {
        FgBoEllipseGpPrjnEl* el = group_specs.FastEl(rcnt % group_specs.size);

        float wt_renorm = (1.0f - el->min_wt);

        int rgpidx = 0;
        for(ruc.y = 0; ruc.y < rgp_geo.y; ruc.y++) {
          for(ruc.x = 0; ruc.x < rgp_geo.x; ruc.x++, rgpidx++) {
            TwoDCoord ruc_s = ruc * rgp_sc; // project ruc into s coords

            TwoDCoord suc;
            TwoDCoord suc_wrp;
            for(suc.y = ruc_s.y-el->con_radius; suc.y <= ruc_s.y+el->con_radius; suc.y++) {
              for(suc.x = ruc_s.x-el->con_radius; suc.x <= ruc_s.x+el->con_radius; suc.x++) {
                suc_wrp = suc;
                if(suc_wrp.WrapClip(wrap, sgp_geo) && !wrap)
                  continue;
                int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
                if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

                TwoDCoord del = suc - ruc_s; // don't use wrap!
                float dst = del.Mag();
                if(dst > (float)el->con_radius) continue; // out of bounds
                if(dst == 0.0f) continue;             // no selfs

                TwoDCoord sun;

                for(sun.x = 0; sun.x < sun_geo.x; sun.x++) {
                  for(sun.y = suy_st; sun.y < suy_st+2; sun.y++) {
                    float wt = el->fgbo_weights.FastEl(del.x +el->con_radius,
                                               del.y+el->con_radius, sun.y-suy_st, sun.x);
                    if(wt <= el->con_thr) continue;
                    wt = wt * wt_renorm + el->min_wt; // renorm to min wt range

                    int rui = run.y * run_geo.x + run.x;
                    int sui = sun.y * sun_geo.x + sun.x;

                    Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
                    if(!ru_u) continue;
                    Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
                    if(!su_u) continue;
                    if(alloc_loop) {
                      if(reciprocal) {
                        su_u->RecvConsAllocInc(1, prjn);
                        ru_u->SendConsAllocInc(1, prjn);
                      }
                      else {
                        ru_u->RecvConsAllocInc(1, prjn);
                        su_u->SendConsAllocInc(1, prjn);
                      }
                    }
                    else {
                      if(reciprocal) {
                        Connection* cn = su_u->ConnectFrom(ru_u, prjn, alloc_loop);
                        if(cn) cn->wt = wt;
                      }
                      else {
                        Connection* cn = ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                        if(cn) cn->wt = wt;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do allocations
      prjn->layer->RecvConsPostAlloc(prjn);
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void FgBoEllipseGpPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  if(reciprocal) {              // switch!
    recv_lay = prjn->from;
    send_lay = prjn->layer;
  }
  TwoDCoord rgp_geo = recv_lay->gp_geom;
  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sgp_geo = send_lay->gp_geom;
  TwoDCoord sun_geo = send_lay->un_geom;
  float n_angles = (float)sun_geo.x;
  int n_depth = sun_geo.y / 2;  // number of different depths represented
  int ruy_per_depth = run_geo.y / n_depth;

  TwoDCoord rgp_sc = sgp_geo / rgp_geo;
  TwoDCoord sgp_geo_half = sgp_geo / 2;

  if(reciprocal) {
    int sgpidx;
    int sui;
    send_lay->UnGpIdxFmUnitIdx(ru->idx, sui, sgpidx); // send = recv!
    TwoDCoord suc = send_lay->UnitGpPosFmIdx(sgpidx);
    TwoDCoord sun;
    sun.SetFmIndex(sui, sun_geo.x);
    int depth = sun.y / 2;
    int suy_st = depth * 2;

    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      int rgpidx;
      int rui;
      recv_lay->UnGpIdxFmUnitIdx(su->idx, rui, rgpidx); // recv = send!
      TwoDCoord ruc = recv_lay->UnitGpPosFmIdx(rgpidx);
      TwoDCoord ruc_s = ruc * rgp_sc; // project ruc into s coords
      TwoDCoord run;
      run.SetFmIndex(rui, run_geo.x);
      FgBoEllipseGpPrjnEl* el = group_specs.FastEl(rui % group_specs.size);
      float wt_renorm = (1.0f - el->min_wt);

      TwoDCoord del = suc - ruc_s;
      if(wrap) {                       // dist may be closer in wrapped case..
        suc.WrapMinDist(del, sgp_geo, ruc_s, sgp_geo_half);
      }

      float wt = el->fgbo_weights.FastEl(del.x + el->con_radius, del.y + el->con_radius,
                                         sun.y-suy_st, sun.x);
      wt = wt * wt_renorm + el->min_wt; // renorm to min wt range
      cg->Cn(i)->wt = wt;
    }
  }
  else {
    int rgpidx;
    int rui;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
    TwoDCoord ruc = recv_lay->UnitGpPosFmIdx(rgpidx);
    TwoDCoord ruc_s = ruc * rgp_sc; // project ruc into s coords
    TwoDCoord run;
    run.SetFmIndex(rui, run_geo.x);
    int depth = run.y / ruy_per_depth;
    int suy_st = depth * 2;

    FgBoEllipseGpPrjnEl* el = group_specs.FastEl(rui % group_specs.size);
    float wt_renorm = (1.0f - el->min_wt);

    for(int i=0; i<cg->size; i++) {
      Unit* su = cg->Un(i);
      int sgpidx;
      int sui;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
      TwoDCoord suc = send_lay->UnitGpPosFmIdx(sgpidx);
      TwoDCoord sun;
      sun.SetFmIndex(sui, sun_geo.x);

      TwoDCoord del = suc - ruc_s;
      if(wrap) {                       // dist may be closer in wrapped case..
        suc.WrapMinDist(del, sgp_geo, ruc_s, sgp_geo_half);
      }

      float wt = el->fgbo_weights.FastEl(del.x + el->con_radius, del.y + el->con_radius,
                                         sun.y-suy_st, sun.x);
      wt = wt * wt_renorm + el->min_wt; // renorm to min wt range
      cg->Cn(i)->wt = wt;
    }
  }
}



//////////////////////////////////////////////////////////
//              FgBoWedgeGpPrjnSpec

void FgBoWedgeGpPrjnSpec::Initialize() {
  wrap = true;
  init_wts = true;
  dist_sigma = 0.8f;
  ang_sigma = 1.0f;
  wt_base = 0.25f;
  wt_range = 0.5f;
}

void FgBoWedgeGpPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CreateStencil();
}

void FgBoWedgeGpPrjnSpec::CreateStencil() {
  int n_angles = 4;
  int n_wedges = 8;
  fgbo_weights.SetGeom(5, send_gp_size.x, send_gp_size.y, 2, n_angles, n_wedges);
  TwoDCoord suc;                        // send coords
  for(int wedge=0; wedge < n_wedges; wedge++) {
    for(suc.y = 0; suc.y < send_gp_size.y; suc.y++) {
      for(suc.x = 0; suc.x < send_gp_size.x; suc.x++) {
        for(int sang_dx = 0; sang_dx < n_angles; sang_dx++) {
          for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
            float wt;
            if(wedge < 4)
              wt = ConWt_Wedge(wedge, suc, send_gp_size, sang_dx, sdir);
            else
              wt = ConWt_Line(wedge-4, suc, send_gp_size, sang_dx, sdir);
            fgbo_weights.FastEl(suc.x, suc.y, sdir, sang_dx, wedge) = wt;
          }
        }
      }
    }
  }
}

float FgBoWedgeGpPrjnSpec::ConWt_Wedge(int wedge, TwoDCoord& suc, TwoDCoord& su_geo, int sang_dx, int sdir) {
  float n_angles = 4.0f;

  TwoDCoord ctr;
  switch(wedge) {
  case 0:
    ctr.SetXY(0,0);
    break;
  case 1:
    ctr.SetXY(su_geo.x-1,0);
    break;
  case 2:
    ctr.SetXY(su_geo.x-1,su_geo.y-1);
    break;
  case 3:
    ctr.SetXY(0,su_geo.y-1);
    break;
  }

  TwoDCoord del = suc - ctr;
  float dst = del.Mag();
  float nrmdst = dst / (float)su_geo.x; // assume square

  float gang = atan2f(del.y, del.x); // group angle -- 0..pi or -pi
  if(gang < 0.0f) gang += 2.0f * taMath_float::pi; // keep it positive

  float pang = gang + 0.5f * taMath_float::pi; // perpendicular angle
  if(pang > 2.0f * taMath_float::pi) pang -= 2.0f * taMath_float::pi;

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < pang)
    dang = (2.0f * taMath_float::pi + sang) - pang;
  else
    dang = sang - pang;
  if(dang >= taMath_float::pi) dang = (2.0f * taMath_float::pi) - dang;

  float netwt = wt_base + wt_range * taMath_float::gauss_den_nonorm(dang, ang_sigma)
    * taMath_float::gauss_den_nonorm((nrmdst-1.0f), dist_sigma);

  return netwt;
}

float FgBoWedgeGpPrjnSpec::ConWt_Line(int line, TwoDCoord& suc, TwoDCoord& su_geo, int sang_dx, int sdir) {
  float n_angles = 4.0f;

  TwoDCoord su_geo_half = su_geo / 2;

  float dst;
  switch(line) {
  case 0:
  case 2:
    dst = (float)(suc.y - su_geo_half.y);
    break;
  case 1:
  case 3:
    dst = (float)(suc.x - su_geo_half.x);
    break;
  }

  float pang = (float)(line) * 0.5f * taMath_float::pi;
  float nrmdst = dst / (float)su_geo.x; // assume square

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < pang)
    dang = (2.0f * taMath_float::pi + sang) - pang;
  else
    dang = sang - pang;
  if(dang >= taMath_float::pi) dang = (2.0f * taMath_float::pi) - dang;

  float netwt = wt_base + wt_range * taMath_float::gauss_den_nonorm(dang, ang_sigma)
    * taMath_float::gauss_den_nonorm(nrmdst, dist_sigma);

  return netwt;
}

void FgBoWedgeGpPrjnSpec::Connect_impl(Projection* prjn) {
  CreateStencil();
  inherited::Connect_impl(prjn);
}

void FgBoWedgeGpPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                Layer* send_lay, int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;
  int n_wedges = 8;

  TwoDCoord sug;
  if(reciprocal) {              // reciprocal is backwards!
    for(int sui=0; sui < su_nunits; sui++) {
      sug.SetFmIndex(sui, send_lay->un_geom.x);
      int cur_depth = sug.y / 2; // v2bo has 2 per depth always
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        int ru_depth = rui / n_wedges;
        if(cur_depth != ru_depth) continue;
        if(!self_con && (su_u == ru_u)) continue;
        su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      int ru_depth = rui / n_wedges;
      for(int sui=0; sui < su_nunits; sui++) {
        sug.SetFmIndex(sui, send_lay->un_geom.x);
        int cur_depth = sug.y / 2; // v2bo has 2 per depth always
        if(cur_depth != ru_depth) continue;
        Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
      }
    }
  }
}

void FgBoWedgeGpPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  if(cg->size == 0) return;
  int n_wedges = 8;

  if(reciprocal) {
    Layer* recv_lay = prjn->from;
    Layer* send_lay = prjn->layer;
    TwoDCoord sgp_geo = send_lay->gp_geom;
    TwoDCoord sgp_geo_half = sgp_geo / 2;

    int sgpidx;
    int sui;
    send_lay->UnGpIdxFmUnitIdx(ru->idx, sui, sgpidx);  // recv = send!
    TwoDCoord suc;
    suc.SetFmIndex(sui, send_lay->un_geom.x);
    TwoDCoord sgpc;
    sgpc.SetFmIndex(sgpidx, send_lay->gp_geom.x);
    int sdir = suc.y % 2;
    int sang_dx = suc.x;

    for(int i=0; i < cg->size; i++) {
      Unit* su = cg->Un(i);     // this is actually ru
      int rgpidx;
      int rui;
      recv_lay->UnGpIdxFmUnitIdx(su->idx, rui, rgpidx); // recv = send!
      int wedge = rui % n_wedges;

      RecvCons* rucg = su->recv.FindFrom(send_lay); // recip prjn
      if(!rucg) continue;
      Unit* ssu = rucg->Un(0);  // first sending unit in that guy's prjn
      int ssgpidx;
      int ssui;
      send_lay->UnGpIdxFmUnitIdx(ssu->idx, ssui, ssgpidx);
      TwoDCoord ssgpc;
      ssgpc.SetFmIndex(ssgpidx, send_lay->gp_geom.x);

      TwoDCoord del = sgpc - ssgpc;
      sgpc.WrapMinDist(del, sgp_geo, ssgpc, sgp_geo_half);

      float wt = fgbo_weights.FastEl(del.x, del.y, sdir, sang_dx, wedge);
      cg->Cn(i)->wt = wt;
    }
  }
  else {
    Layer* recv_lay = prjn->layer;
    Layer* send_lay = prjn->from;
    TwoDCoord sgp_geo = send_lay->gp_geom;
    TwoDCoord sgp_geo_half = sgp_geo / 2;

    int rgpidx;
    int rui;
    recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
    int wedge = rui % n_wedges;

    Unit* ssu = cg->Un(0);
    int ssgpidx;
    int ssui;
    send_lay->UnGpIdxFmUnitIdx(ssu->idx, ssui, ssgpidx);
    TwoDCoord ssgpc;
    ssgpc.SetFmIndex(ssgpidx, send_lay->gp_geom.x);

    for(int i=0; i < cg->size; i++) {
      Unit* su = cg->Un(i);
      int sgpidx;
      int sui;
      send_lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
      TwoDCoord suc;
      suc.SetFmIndex(sui, send_lay->un_geom.x);
      TwoDCoord sgpc;
      sgpc.SetFmIndex(sgpidx, send_lay->gp_geom.x);

      TwoDCoord del = sgpc - ssgpc;
      sgpc.WrapMinDist(del, sgp_geo, ssgpc, sgp_geo_half);

      int sdir = suc.y % 2;
      int sang_dx = suc.x;
      float wt = fgbo_weights.FastEl(del.x, del.y, sdir, sang_dx, wedge);
      cg->Cn(i)->wt = wt;
    }
  }
}

//////////////////////////////////////////////////////////
//              V1EndStopPrjnSpec

void V1EndStopPrjnSpec::Initialize() {
  n_angles = 4;
  end_stop_dist = 2;
  adjang_wt = 0.2f;
  wrap = true;
  init_wts = true;
}

void V1EndStopPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  wrap = true;
}

static void es_get_angles(float angf, float& cosx, float& siny) {
  cosx = taMath_float::cos(angf);
  siny = taMath_float::sin(angf);
  // always normalize by the largest value so that it is equal to 1
  // not for these guys!!
//   if(fabsf(cosx) > fabsf(siny)) {
//     siny = siny / fabsf(cosx);                       // must come first!
//     cosx = cosx / fabsf(cosx);
//   }
//   else {
//     cosx = cosx / fabsf(siny);
//     siny = siny / fabsf(siny);
//   }
}

void V1EndStopPrjnSpec::InitStencils(Projection* prjn) {
  v1s_ang_slopes.SetGeom(3,2,2,n_angles);
  float ang_inc = taMath_float::pi / (float)n_angles;
  for(int ang=0; ang<n_angles; ang++) {
    float cosx, siny;
    float angf = (float)ang * ang_inc;
    es_get_angles(angf, cosx, siny);
    v1s_ang_slopes.FastEl(X, LINE, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, LINE, ang) = siny;

    es_get_angles(angf + taMath_float::pi * .5f, cosx, siny);
    v1s_ang_slopes.FastEl(X, ORTHO, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, ORTHO, ang) = siny;
  }

  v1c_es_stencils.SetGeom(4, 2, 2, 2, n_angles);

  if(n_angles == 4) {
    v1c_es_stencils.FastEl(X, 0, 0, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 0, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 0) = end_stop_dist;
    v1c_es_stencils.FastEl(X, 0, 1, 0) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 1, 0) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 0) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 1, 0) = end_stop_dist;

    v1c_es_stencils.FastEl(X, 0, 0, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 0, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 2) = end_stop_dist;
    v1c_es_stencils.FastEl(X, 0, 1, 2) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 1, 2) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 2) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 1, 2) = end_stop_dist;

    v1c_es_stencils.FastEl(X, 0, 0, 1) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 1) = 0;
    v1c_es_stencils.FastEl(X, 1, 0, 1) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 1) = 0;
    v1c_es_stencils.FastEl(X, 0, 1, 1) = 0;
    v1c_es_stencils.FastEl(Y, 0, 1, 1) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 1) = 0;
    v1c_es_stencils.FastEl(Y, 1, 1, 1) = end_stop_dist;

    v1c_es_stencils.FastEl(X, 0, 0, 3) = -end_stop_dist;
    v1c_es_stencils.FastEl(Y, 0, 0, 3) = 0;
    v1c_es_stencils.FastEl(X, 1, 0, 3) = end_stop_dist;
    v1c_es_stencils.FastEl(Y, 1, 0, 3) = 0;
    v1c_es_stencils.FastEl(X, 0, 1, 3) = 0;
    v1c_es_stencils.FastEl(Y, 0, 1, 3) = -end_stop_dist;
    v1c_es_stencils.FastEl(X, 1, 1, 3) = 0;
    v1c_es_stencils.FastEl(Y, 1, 1, 3) = end_stop_dist;
  }
  else {
    for(int ang=0; ang < n_angles; ang++) {
      for(int sidx=0; sidx < 2; sidx++) {
        int side = (sidx == 0) ? -1 : 1;

        int sx = (int)(end_stop_dist * (float)side * v1s_ang_slopes.FastEl(X, LINE, ang));
        int sy = (int)(end_stop_dist * (float)side * v1s_ang_slopes.FastEl(Y, LINE, ang));
        for(int lpdx=0; lpdx < 2; lpdx++) {
          int lpt = (lpdx == 0) ? -1 : 1;
          v1c_es_stencils.FastEl(X, lpdx, sidx, ang) = sx +
            (int)(end_stop_dist *(float)lpt * v1s_ang_slopes.FastEl(X, ORTHO, ang));
          v1c_es_stencils.FastEl(Y, lpdx, sidx, ang) = sy +
            (int)(end_stop_dist * (float)lpt * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
        }
      }
    }
  }

  v1c_es_angwts.SetGeom(2, n_angles, n_angles);
  for(int ang1=0; ang1 < n_angles; ang1++) {
    for(int ang2=0; ang2 < n_angles; ang2++) {
      int ang_dst = ABS(ang1-ang2);
      {
        int wrap_dst = ABS(ang1 - (ang2 + n_angles));
        if(wrap_dst < ang_dst) ang_dst = wrap_dst;
      }
      {
        int wrap_dst = ABS(ang1 - (ang2 - n_angles));
        if(wrap_dst < ang_dst) ang_dst = wrap_dst;
      }
      float wt;
      if(ang_dst == 0)
        wt = 0.0f;
      else if(ang_dst == 1)
        wt = adjang_wt;
      else
        wt = 1.0f;
      v1c_es_angwts.FastEl(ang1, ang2) = wt;
    }
  }
}

void V1EndStopPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;

  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires send layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(TestWarning(recv_lay->gp_geom != send_lay->gp_geom, "Connect_impl",
                 "Recv layer does not have same gp geometry as sending layer -- cannot connect!")) {
    return;
  }

  TwoDCoord gp_geo = recv_lay->gp_geom; // same as sgp

  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  if(TestWarning(run_geo.x != sun_geo.x, "Connect_impl",
                 "Recv layer does not have same x-dimension unit geometry within unit groups as sending layer -- cannot connect!")) {
    return;
  }
  if(TestWarning(run_geo.y != 1, "Connect_impl",
                 "Recv layer should have a single y-dimension unit geometry within unit groups")) {
    return;
  }

  InitStencils(prjn);

  int n_cons = sun_geo.y + 2 * 2 * (n_angles-1) * sun_geo.y;

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {

        for(int ang = 0; ang < n_angles; ang++) { // angles
          int rui = ang;
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!ru_u) continue;
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(n_cons, prjn);

          TwoDCoord sun;
          for(sun.y = 0; sun.y < sun_geo.y; sun.y++) {
            // central point (same as ruc -- use rgpidx)
            int sui = sun.y * n_angles + ang;
            Unit* su_u = send_lay->UnitAtUnGpIdx(sui, rgpidx);
            if(!su_u) continue;
            ru_u->ConnectFrom(su_u, prjn, alloc_loop);

            for(int sidx=0; sidx < 2; sidx++) {
              for(int lpdx=0; lpdx < 2; lpdx++) {
                int xp = v1c_es_stencils.FastEl(X,lpdx,sidx,ang);
                int yp = v1c_es_stencils.FastEl(Y,lpdx,sidx,ang);

                TwoDCoord suc = ruc;
                suc.x += xp;  suc.y += yp;
                TwoDCoord suc_wrp = suc;
                if(suc_wrp.WrapClip(wrap, gp_geo) && !wrap)
                  continue;
                int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
                if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

                for(int opang=0; opang<n_angles; opang++) {
                  float angwt = v1c_es_angwts.FastEl(ang, opang);
                  if(angwt == 0.0f) continue;

                  int sui = sun.y * n_angles + opang;
                  Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
                  if(!su_u) continue;
                  ru_u->ConnectFrom(su_u, prjn, alloc_loop);
                }
              }
            }
          }
        }  // for ang
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void V1EndStopPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord gp_geo = recv_lay->gp_geom; // same as sgp

  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  int n_cons = sun_geo.y + 2 * 2 * (n_angles-1) * sun_geo.y;
  if(cg->size != n_cons) return; // no can do

  int rgpidx;
  int rui;
  recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);

  int ang = rui;

  int cnidx = 0;
  TwoDCoord sun;
  for(sun.y = 0; sun.y < sun_geo.y; sun.y++) {
    cg->Cn(cnidx++)->wt = 1.0f; // center point
    for(int sidx=0; sidx < 2; sidx++) {
      for(int lpdx=0; lpdx < 2; lpdx++) {
        for(int opang=0; opang<n_angles; opang++) {
          float angwt = v1c_es_angwts.FastEl(rui, opang);
          if(angwt == 0.0f) continue;
          cg->Cn(cnidx++)->wt = angwt;
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////
//                      VisDisparityPrjnSpec

void VisDisparityPrjnSpec::Initialize() {
  n_disps = 1;
  disp_range_pct = 0.05f;
  gauss_sig = 0.7f;
  disp_spacing = 2.0f;
  end_extra = 2;
  wrap = true;

  init_wts = true;

  tot_disps = 1 + 2 * n_disps;
  UpdateFmV1sSize(24);
}

void VisDisparityPrjnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_disps = 1 + 2 * n_disps;
}


void VisDisparityPrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(TestWarning(!prjn->from->unit_groups, "Connect_impl",
                 "requires sending layer to have unit groups!")) {
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord rgp_geo = recv_lay->gp_geom;
  TwoDCoord sgp_geo = send_lay->gp_geom;

  if(TestWarning(rgp_geo != sgp_geo, "Connect_Gps",
                 "Recv layer does not have same gp geometry as sending layer -- cannot connect!")) {
    return;
  }
  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sun_geo = send_lay->un_geom;

  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  if(TestWarning(ru_n != su_n * tot_disps, "Connect_Gps",
                 "Recv layer unit groups must have n_disps * 2 + 1=",
                 String(tot_disps),"times number of units in send layer unit group=",
                 String(su_n),"  should be:", String(su_n * tot_disps), "is:",
                 String(ru_n))) {
    return;
  }

  if(prjn->recv_idx == 0)       // right eye
    Connect_RightEye(prjn);
  else
    Connect_LeftEye(prjn);
}

void VisDisparityPrjnSpec::Connect_RightEye(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord gp_geo = recv_lay->gp_geom;

  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  // note: could optimize this code b/c sender alloc = tot_disps so could be pre-alloc entirely
  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {
        for(int rui=0; rui<ru_n; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!ru_u) continue;
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(1, prjn); // just one prjn!

          int sui = rui % su_n; // just modulus -- recv from same features tot_disps times
          Unit* su_u = send_lay->UnitAtUnGpIdx(sui, rgpidx); // rgp = sgp
          if(!su_u) continue;
          ru_u->ConnectFrom(su_u, prjn, alloc_loop);
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void VisDisparityPrjnSpec::InitStencils(Projection* prjn) {
  Layer* send_lay = prjn->from;
  TwoDCoord gp_geo = send_lay->gp_geom;

  UpdateFmV1sSize(gp_geo.x);

  v1b_widths.SetGeom(1, tot_disps);
  v1b_weights.SetGeom(2, max_width, tot_disps);
  v1b_stencils.SetGeom(2, max_width, tot_disps);

  v1b_weights.InitVals(0.0f);   // could have some lurkers in there from other settings, which can affect normalization

  int twe = disp_range + end_ext;

  // everything is conditional on the disparity
  for(int disp=-n_disps; disp <= n_disps; disp++) {
    int didx = disp + n_disps;
    int doff = disp * disp_spc;
    if(disp == 0) {             // focal
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range;
      for(int tw=-disp_range; tw<=disp_range; tw++) {
        int twidx = tw + disp_range;
        float fx = (float)tw / (float)disp_range;
        v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == -n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range + end_ext;
      for(int tw=-twe; tw<=disp_range; tw++) {
        int twidx = tw + twe;
        if(tw < 0)
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, gauss_sig);
        else {
          float fx = (float)tw / (float)disp_range;
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        }
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else if(disp == n_disps) {
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range + end_ext;
      for(int tw=-disp_range; tw<=twe; tw++) {
        int twidx = tw + disp_range;
        if(tw > 0)
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(0.0f, gauss_sig);
        else {
          float fx = (float)tw / (float)disp_range;
          v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        }
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
    else {
      v1b_widths.FastEl(didx) = 1 + 2 * disp_range;
      for(int tw=-disp_range; tw<=disp_range; tw++) {
        int twidx = tw + disp_range;
        float fx = (float)tw / (float)disp_range;
        v1b_weights.FastEl(twidx, didx) = taMath_float::gauss_den_sig(fx, gauss_sig);
        v1b_stencils.FastEl(twidx, didx) = doff + tw;
      }
    }
  }

  taMath_float::vec_norm_max(&v1b_weights); // max norm to 1
}

void VisDisparityPrjnSpec::Connect_LeftEye(Projection* prjn) {
  InitStencils(prjn);

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord gp_geo = recv_lay->gp_geom; // same as sgp

  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop >= 0; alloc_loop--) {
    int rgpidx = 0;
    for(ruc.y = 0; ruc.y < gp_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < gp_geo.x; ruc.x++, rgpidx++) {
        for(int didx=0; didx < tot_disps; didx++) {
          int dwd = v1b_widths.FastEl(didx);

          int strui = didx * su_n; // starting index
          for(int sui=0; sui<su_n; sui++) {
            int rui = strui + sui;
            Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
            if(!ru_u) continue;
            if(!alloc_loop)
              ru_u->RecvConsPreAlloc(dwd, prjn);

            for(int twidx = 0; twidx < dwd; twidx++) {
              int off = v1b_stencils.FastEl(twidx, didx);
              // float wt = v1b_weights.FastEl(twidx, didx);

              TwoDCoord suc = ruc;
              suc.x += off;     // offset
              TwoDCoord suc_wrp = suc;
              if(suc_wrp.WrapClip(wrap, gp_geo) && !wrap)
                continue;
              int sgpidx = send_lay->UnitGpIdxFmPos(suc_wrp);
              if(!send_lay->UnitGpIdxIsValid(sgpidx)) continue;

              Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
              if(!su_u) continue;
              ru_u->ConnectFrom(su_u, prjn, alloc_loop);
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      prjn->from->SendConsPostAlloc(prjn);
    }
  }
}

void VisDisparityPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  if(cg->size == 1) {           // right eye
    cg->Cn(0)->wt = 1.0f;
    return;
  }

  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;
  TwoDCoord gp_geo = recv_lay->gp_geom; // same as sgp

  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord sun_geo = send_lay->un_geom;
  int su_n = sun_geo.Product();
  int ru_n = run_geo.Product();

  int rgpidx;
  int rui;
  recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
  int didx = rui / su_n;        // disparity index

  int dwd = v1b_widths.SafeEl(didx);
  if(cg->size == dwd) {
    for(int i=0; i<cg->size; i++) {
      float wt = v1b_weights.SafeEl(i, didx);
      cg->Cn(i)->wt = wt;
    }
  }
  else {
    TwoDCoord ruc = recv_lay->UnitGpPosFmIdx(rgpidx);
    if(ruc.x < gp_geo.x / 2) {
      int st = dwd - cg->size;
      for(int i=0; i<cg->size; i++) {
        float wt = v1b_weights.SafeEl(st + i, didx);
        cg->Cn(i)->wt = wt;
      }
    }
    else {
      // actually just straight up
      for(int i=0; i<cg->size; i++) {
        float wt = v1b_weights.SafeEl(i, didx);
        cg->Cn(i)->wt = wt;
      }
    }
  }
}


///////////////////////////////////////////////////////////////
//                      VisDisparityLayerSpec

void VisDispLaySpec::Initialize() {
  max_l = true;
  incl_other_res = true;
  updt_cycles = 5;
}

void VisDisparityLayerSpec::Initialize() {
//   clamp.hard = false;
}

bool VisDisparityLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);
  if(!rval) return rval;

  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);
  if(lay->CheckError(u == NULL, quiet, rval,
                "leabra vis disparity layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }
  if(lay->CheckError(u->recv.size < 2, quiet, rval,
                     "leabra vis disparity units must have >= 2 projections -- less than that:",
                     lay->name)) {
    return false;               // fatal
  }

  LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(0);
  if(lay->CheckError(!cg->prjn->spec.SPtr()->InheritsFrom(&TA_VisDisparityPrjnSpec), quiet, rval,
                     "leabra vis disparity units must have 1st prjn = VisDisparityPrjnSpec -- not",
                     lay->name)) {
    return false;               // fatal
  }
  cg = (LeabraRecvCons*)u->recv.SafeEl(1);
  if(lay->CheckError(!cg->prjn->spec.SPtr()->InheritsFrom(&TA_VisDisparityPrjnSpec), quiet, rval,
                     "leabra vis disparity units must have 2nd prjn = VisDisparityPrjnSpec -- not",
                     lay->name)) {
    return false;               // fatal
  }

  return rval;
}

void VisDisparityLayerSpec::ComputeDispToExt(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    float right = 0.0f;
    float left = 0.0f;

    LeabraRecvCons* cg_r = (LeabraRecvCons*)u->recv.SafeEl(0);
    if(!cg_r || cg_r->size <= 0) return;
    LeabraUnit* su_r = (LeabraUnit*)cg_r->Un(0);
    right = su_r->act_eq;

    LeabraRecvCons* cg_l = (LeabraRecvCons*)u->recv.SafeEl(1);
    if(!cg_l || cg_l->size <= 0) return;
    for(int i=0; i < cg_l->size; i++) {
      LeabraUnit* su_l = (LeabraUnit*)cg_l->Un(i);
      LeabraCon* cn = (LeabraCon*)cg_l->PtrCn(i); // recv mode
      float itm = cn->wt * su_l->act_eq;
      if(disp.max_l)
        left = MAX(left, itm);
      else
        left += itm;
    }
    if(!disp.max_l) {
      left /= (float)cg_l->size;
    }

    float prod = MIN(left, right); // min = product!
    u->misc_1 = prod;

    if(disp.incl_other_res && net->cycle > 1) {
      for(int j=2; j<u->recv.size; j++) {
        LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.FastEl(j);
        LeabraLayer* fm = (LeabraLayer*)cg->prjn->from.ptr();
        LeabraLayerSpec* ls = (LeabraLayerSpec*)fm->spec.SPtr();
        if(!ls->InheritsFrom(&TA_VisDisparityLayerSpec)) continue;
        float netin = 0.0f;
        for(int i=0; i < cg->size; i++) {
          LeabraUnit* su = (LeabraUnit*)cg->Un(i);
//        LeabraCon* cn = (LeabraCon*)cg->PtrCn(i); // recv mode
//        float itm = cn->wt * su->misc_1;          // note: using misc_1 prod val!
          float itm = su->misc_1; // no need for weight!
          if(disp.max_l)
            netin = MAX(netin, itm);
          else
            netin += itm;
        }
        if(!disp.max_l) {
          if(cg->size > 0)
            netin /= (float)cg->size;
        }
        prod = MIN(netin, prod);
      }
    }

    u->SetExtFlag(Unit::EXT);
    u->ext = prod;
  }
  // todo: deal with horiz apeture prob
}

void VisDisparityLayerSpec::Compute_ExtraNetin(LeabraLayer* lay, LeabraNetwork* net) {
  if(disp.updt_cycles < 0 || net->cycle <= disp.updt_cycles)
    ComputeDispToExt(lay, net); // always do it here -- avail for softclamp
}

void VisDisparityLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(!clamp.hard) {
    inherited::Compute_CycleStats(lay, net);
    return;
  }
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->act = u->ext;
    u->act_eq = u->act_nd = u->act;
    u->da = 0.0f;               // I'm fully settled!
  }
  inherited::Compute_CycleStats(lay, net);
}

///////////////////////////////////////////////////////////////
//              TiledGpRFOneToOnePrjnSpec

void TiledGpRFOneToOnePrjnSpec::Initialize() {
  gauss_sigma = 1.0f;
  su_idx_st = 0;
  ru_idx_st = 0;
  gp_n_cons = -1;
}

void TiledGpRFOneToOnePrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                Layer* send_lay, int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n - ru_idx_st;
  int su_nunits = send_lay->un_geom.n - su_idx_st;
  int maxn = MIN(ru_nunits, su_nunits);
  if(gp_n_cons > 0)
    maxn = MIN(gp_n_cons, maxn);

  if(reciprocal) {              // reciprocal is backwards!
    for(int ui=0; ui < maxn; ui++) {
      Unit* su_u = send_lay->UnitAtUnGpIdx(su_idx_st + ui, sgpidx);
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(ru_idx_st + ui, rgpidx);
      if(!self_con && (su_u == ru_u)) continue;
      su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
    }
  }
  else {
    for(int ui=0; ui < maxn; ui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(ru_idx_st + ui, rgpidx);
      Unit* su_u = send_lay->UnitAtUnGpIdx(su_idx_st + ui, sgpidx);
      if(!self_con && (su_u == ru_u)) continue;
      ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
    }
  }
}

void TiledGpRFOneToOnePrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init

  TwoDCoord rf_half_wd = send_gp_size / 2;
  FloatTwoDCoord rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == send_gp_size) // even
    rf_ctr -= .5f;

  float sig_sq = (float)(rf_half_wd.x * rf_half_wd.x) * gauss_sigma * gauss_sigma;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % send_gp_size.x;
    int su_y = i / send_gp_size.x;

    float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / sig_sq);

    cg->Cn(i)->wt = wt;
  }
}

///////////////////////////////////////////////////////////////
//              TiledGpRFOneToOneWtsPrjnSpec

void TiledGpRFOneToOneWtsPrjnSpec::Initialize() {
  one_to_one_wt = 0.8f;
  other_wt = 0.2f;
  init_wts = true;
}

void TiledGpRFOneToOneWtsPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  inherited::C_Init_Weights(prjn, cg, ru); // always do regular init
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  int rgpidx;
  int rui;
  recv_lay->UnGpIdxFmUnitIdx(ru->idx, rui, rgpidx);
  for(int i=0; i < cg->size; i++) {
    Unit* su = cg->Un(i);
    int sgpidx;
    int sui;
    send_lay->UnGpIdxFmUnitIdx(su->idx, sui, sgpidx);
    if(sui == rui)
      cg->Cn(i)->wt = one_to_one_wt;
    else
      cg->Cn(i)->wt = other_wt;
  }
}


///////////////////////////////////////////////////////////////
//              V2toV4DepthPrjnSpec

void V2toV4DepthPrjnSpec::Initialize() {
  depth_idx = 0;
}

void V2toV4DepthPrjnSpec::Connect_UnitGroup(Projection* prjn, Layer* recv_lay,
                                Layer* send_lay, int rgpidx, int sgpidx, int alloc_loop) {
  int ru_nunits = recv_lay->un_geom.n;
  int su_nunits = send_lay->un_geom.n;
  TwoDCoord sug;

  if(reciprocal) {              // reciprocal is backwards!
    for(int sui=0; sui < su_nunits; sui++) {
      sug.SetFmIndex(sui, send_lay->un_geom.x);
      int cur_depth = sug.y / 2; // v2bo has 2 per depth always
      if(cur_depth != depth_idx) continue;
      Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
      for(int rui=0; rui < ru_nunits; rui++) {
        Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        su_u->ConnectFrom(ru_u, prjn, alloc_loop); // recip!
      }
    }
  }
  else {
    for(int rui=0; rui < ru_nunits; rui++) {
      Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
      for(int sui=0; sui < su_nunits; sui++) {
        sug.SetFmIndex(sui, send_lay->un_geom.x);
        int cur_depth = sug.y / 2; // v2bo has 2 per depth always
        if(cur_depth != depth_idx) continue;
        Unit* su_u = send_lay->UnitAtUnGpIdx(sui, sgpidx);
        if(!self_con && (su_u == ru_u)) continue;
        ru_u->ConnectFrom(su_u, prjn, alloc_loop); // recip!
      }
    }
  }
}


///////////////////////////////////////////////////////////////
//                      V1 Layer
///////////////////////////////////////////////////////////////

void V1FeatInhibSpec::Initialize() {
  on = true;
  n_angles = 4;
  inhib_d = 1;
  inhib_g = 0.8f;
  wrap = true;

  tot_ni_len = 2 * inhib_d + 1;
}

void V1FeatInhibSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_ni_len = 2 * inhib_d + 1;
}

void LeabraV1LayerSpec::Initialize() {
}


void LeabraV1LayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateStencils();
  if(feat_inhib.on) {
    if(inhib_group != UNIT_GROUPS)
      inhib_group = UNIT_GROUPS; // must be!
  }
}

bool LeabraV1LayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  if(!feat_inhib.on) return true;

  bool rval = true;
  lay->CheckError(!lay->unit_groups, quiet, rval,
                  "does not have unit groups -- MUST have unit groups!");

  lay->CheckError(lay->un_geom.x != feat_inhib.n_angles, quiet, rval,
                  "un_geom.x must be = to feat_inhib.n_angles -- is not!");
  return rval;
}

static void v1_get_angles(float angf, float& cosx, float& siny) {
  cosx = taMath_float::cos(angf);
  siny = taMath_float::sin(angf);
  // always normalize by the largest value so that it is equal to 1
  if(fabsf(cosx) > fabsf(siny)) {
    siny = siny / fabsf(cosx);                  // must come first!
    cosx = cosx / fabsf(cosx);
  }
  else {
    cosx = cosx / fabsf(siny);
    siny = siny / fabsf(siny);
  }
}

void LeabraV1LayerSpec::UpdateStencils() {
  v1s_ang_slopes.SetGeom(3,2,2,feat_inhib.n_angles);
  float ang_inc = taMath_float::pi / (float)feat_inhib.n_angles;
  for(int ang=0; ang<feat_inhib.n_angles; ang++) {
    float cosx, siny;
    float angf = (float)ang * ang_inc;
    v1_get_angles(angf, cosx, siny);
    v1s_ang_slopes.FastEl(X, LINE, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, LINE, ang) = siny;

    v1_get_angles(angf + taMath_float::pi * .5f, cosx, siny);
    v1s_ang_slopes.FastEl(X, ORTHO, ang) = cosx;
    v1s_ang_slopes.FastEl(Y, ORTHO, ang) = siny;
  }
  // config: x,y coords by tot_ni_len, by angles
  v1s_ni_stencils.SetGeom(3, 2, feat_inhib.tot_ni_len, feat_inhib.n_angles);

  for(int ang = 0; ang < feat_inhib.n_angles; ang++) { // angles
    for(int lpt=-feat_inhib.inhib_d; lpt <= feat_inhib.inhib_d; lpt++) {
      int lpdx = lpt + feat_inhib.inhib_d;
      v1s_ni_stencils.FastEl(X, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(X, ORTHO, ang)); // ortho
      v1s_ni_stencils.FastEl(Y, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl(Y, ORTHO, ang));
    }
  }
}


void LeabraV1LayerSpec::Compute_FeatGpActive(LeabraLayer* lay, LeabraUnit_Group* fugp,
                                             LeabraNetwork* net) {
//   fugp->active_buf.size = 0;
//   for(int ui=0; ui<fugp->size; ui++) {
//     LeabraUnit* u = (LeabraUnit*)fugp->FastEl(ui);
//     LeabraUnit_Group* u_own = (LeabraUnit_Group*)u->owner; // NOT fugp!
//     if(u->i_thr >= u_own->i_val.g_i) // compare to their own group's inhib val!
//       fugp->active_buf.Add(u);
//   }
}


void LeabraV1LayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(!feat_inhib.on) {          // do the normal
    inherited::Compute_ApplyInhib(lay, net);
    return;
  }
  if((net->cycle >= 0) && lay->hard_clamped)
    return;                     // don't do this during normal processing
  if(inhib.type == LeabraInhibSpec::UNIT_INHIB) return; // otherwise overwrites!

  for(int gpidx=0; gpidx < lay->gp_geom.n; gpidx++) {
    LeabraUnGpData* gpd = lay->ungp_data.FastEl(gpidx);
    LeabraInhib* thr = (LeabraInhib*)gpd;
    TwoDCoord sc;                       // simple coords
    sc.SetFmIndex(gpidx, lay->gp_geom.x);

    int nunits = lay->UnitAccess_NUnits(Layer::ACC_GP);
    float inhib_val = thr->i_val.g_i;
    // note: not doing tie break!

    TwoDCoord fc;               // v1s feature coords
    TwoDCoord oc;               // other coord
    float uidx = 0;
    for(int polclr = 0; polclr < lay->un_geom.y; polclr++) { // polclr features
      fc.y = polclr;
      for(int ang = 0; ang < feat_inhib.n_angles; ang++, uidx++) { // angles
        fc.x = ang;
        float feat_inhib_max = 0.0f;
        for(int lpdx=0; lpdx < feat_inhib.tot_ni_len; lpdx++) { // go out to neighs
          if(lpdx == feat_inhib.inhib_d) continue;                 // skip self
          int xp = v1s_ni_stencils.FastEl(X,lpdx,ang);
          int yp = v1s_ni_stencils.FastEl(Y,lpdx,ang);
          oc.x = sc.x + xp;
          oc.y = sc.y + yp;
          if(oc.WrapClip(feat_inhib.wrap, lay->gp_geom)) {
            if(!feat_inhib.wrap) continue; // bail on clipping only
          }
          LeabraUnit* oth_unit = (LeabraUnit*)lay->UnitAtGpCoord(oc, fc);
          float oth_ithr = oth_unit->i_thr;
          float ogi = feat_inhib.inhib_g * oth_ithr; // note: directly on ithr!
          feat_inhib_max = MAX(feat_inhib_max, ogi);
        }

        float gi_eff = MAX(inhib_val, feat_inhib_max);
        LeabraUnit* u = (LeabraUnit*)lay->UnitAtUnGpIdx((int)uidx, gpidx);
        u->Compute_ApplyInhib(net, gi_eff);
      }
    }
  }
}


///////////////////////////////////////////////////////////////////
//      Cerebellum-related special guys

void CerebConj2PrjnSpec::Initialize() {
  init_wts = true;
  rf_width = 6;
  rf_move = 3.0f;
  wrap = false;
  gauss_sigma = 1.0f;
}

void CerebConj2PrjnSpec::Connect_impl(Projection* prjn) {
  if(!(bool)prjn->from) return;
  if(prjn->layer->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!prjn->layer->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }
  if(prjn->recv_idx == 0)
    Connect_Outer(prjn);
  else
    Connect_Inner(prjn);
}

void CerebConj2PrjnSpec::Connect_Outer(Projection* prjn) {
  int n_cons = rf_width.Product();
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  TwoDCoord rf_half_wd = rf_width / 2;
  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord su_geo = send_lay->flat_geom;

  TwoDCoord ruc;
  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(ruc.y = 0; ruc.y < rug_geo.y; ruc.y++) {
      for(ruc.x = 0; ruc.x < rug_geo.x; ruc.x++) {
        int rgpidx = recv_lay->UnitGpIdxFmPos(ruc);
        if(!recv_lay->UnitGpIdxIsValid(rgpidx)) continue;

        TwoDCoord su_st;
        if(wrap) {
          su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
          su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
        }
        else {
          su_st.x = (int)floor((float)ruc.x * rf_move.x);
          su_st.y = (int)floor((float)ruc.y * rf_move.y);
        }

        su_st.WrapClip(wrap, su_geo);
        TwoDCoord su_ed = su_st + rf_width;
        if(wrap) {
          su_ed.WrapClip(wrap, su_geo); // just wrap ends too
        }
        else {
          if(su_ed.x > su_geo.x) {
            su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
          }
          if(su_ed.y > su_geo.y) {
            su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
          }
        }

        for(int rui=0; rui<recv_lay->un_geom.n; rui++) {
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rgpidx);
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(n_cons, prjn);

          TwoDCoord suc;
          TwoDCoord suc_wrp;
          for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
            for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
              suc_wrp = su_st + suc;
              if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
                continue;
              Unit* su_u = send_lay->UnitAtCoord(suc_wrp);
              if(!su_u) continue;
              if(!self_con && (su_u == ru_u)) continue;

              ru_u->ConnectFrom(su_u, prjn, alloc_loop); // don't check: saves lots of time!
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      send_lay->SendConsPostAlloc(prjn);
    }
  }
}

void CerebConj2PrjnSpec::Connect_Inner(Projection* prjn) {
  Layer* recv_lay = prjn->layer;
  Layer* send_lay = prjn->from;

  if(!(bool)send_lay)   return;
  if(recv_lay->units.leaves == 0) // an empty layer!
    return;
  if(TestWarning(!recv_lay->unit_groups, "Connect_impl",
                 "requires recv layer to have unit groups!")) {
    return;
  }

  int n_cons = rf_width.Product();
  TwoDCoord rf_half_wd = rf_width / 2;
  TwoDCoord rug_geo = recv_lay->gp_geom;
  TwoDCoord run_geo = recv_lay->un_geom;
  TwoDCoord su_geo = send_lay->flat_geom;

  for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
    for(int rug=0;rug<recv_lay->gp_geom.n;rug++) {
      TwoDCoord ruc;
      for(ruc.y = 0; ruc.y < run_geo.y; ruc.y++) {
        for(ruc.x = 0; ruc.x < run_geo.x; ruc.x++) {
          int rui = ruc.y * run_geo.x + ruc.x;
          Unit* ru_u = recv_lay->UnitAtUnGpIdx(rui, rug);
          if(!ru_u) continue;
          if(!alloc_loop)
            ru_u->RecvConsPreAlloc(n_cons, prjn);

          TwoDCoord su_st;
          if(wrap) {
            su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
            su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
          }
          else {
            su_st.x = (int)floor((float)ruc.x * rf_move.x);
            su_st.y = (int)floor((float)ruc.y * rf_move.y);
          }

          su_st.WrapClip(wrap, su_geo);
          TwoDCoord su_ed = su_st + rf_width;
          if(wrap) {
            su_ed.WrapClip(wrap, su_geo); // just wrap ends too
          }
          else {
            if(su_ed.x > su_geo.x) {
              su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
            }
            if(su_ed.y > su_geo.y) {
              su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
            }
          }

          TwoDCoord suc;
          TwoDCoord suc_wrp;
          for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
            for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
              suc_wrp = su_st + suc;
              if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
                continue;
              Unit* su_u = send_lay->UnitAtCoord(suc_wrp);
              if(su_u == NULL) continue;
              if(!self_con && (su_u == ru_u)) continue;
              ru_u->ConnectFrom(su_u, prjn, alloc_loop); // don't check: saves lots of time!
            }
          }
        }
      }
    }
    if(alloc_loop) { // on first pass through alloc loop, do sending allocations
      send_lay->SendConsPostAlloc(prjn);
    }
  }
}

void CerebConj2PrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
  TwoDCoord rf_half_wd = rf_width / 2;
  FloatTwoDCoord rf_ctr = rf_half_wd;
  if(rf_half_wd * 2 == rf_width) // even
    rf_ctr -= .5f;

  for(int i=0; i<cg->size; i++) {
    int su_x = i % rf_width.x;
    int su_y = i / rf_width.x;

    float dst = taMath_float::euc_dist_sq(su_x, su_y, rf_ctr.x, rf_ctr.y);
    float wt = expf(-0.5 * dst / (gauss_sigma * gauss_sigma));

    cg->Cn(i)->wt = wt;
  }
}

bool CerebConj2PrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
  trg_send_geom.x = send_x;
  trg_send_geom.y = send_y;

  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  // now fix it the other way
  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom +1) * rf_move);

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
}

bool CerebConj2PrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
  trg_recv_geom.x = recv_x;
  trg_recv_geom.y = recv_y;

  if(wrap)
    trg_send_geom = (trg_recv_geom * rf_move);
  else
    trg_send_geom = ((trg_recv_geom+1) * rf_move);

  // now fix it the other way
  if(wrap)
    trg_recv_geom = (trg_send_geom / rf_move);
  else
    trg_recv_geom = (trg_send_geom / rf_move) - 1;

  DataChanged(DCR_ITEM_UPDATED);
  return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
}



///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//              Wizard          //
//////////////////////////////////

bool LeabraWizard::UpdateInputDataFmNet(Network* net, DataTable* data_table) {
  if(TestError(!data_table || !net, "UpdateInputDataFmNet",
               "must specify both a network and a data table")) return false;
  data_table->StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->layer_type == Layer::HIDDEN) continue;
    int lay_idx = 0;

    LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();
    if(ls->InheritsFrom(&TA_ScalarValLayerSpec) && !((ScalarValLayerSpec*)ls)->scalar.clamp_pat) {
      if(lay->unit_groups) {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 4, 1, 1,
           MAX(lay->gp_geom.x,1), MAX(lay->gp_geom.y,1));
      }
      else {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 2, 1, 1);
      }
    }
    else if(ls->InheritsFrom(&TA_TwoDValLayerSpec) && !((TwoDValLayerSpec*)ls)->twod.clamp_pat) {
      TwoDValLayerSpec* tdls = (TwoDValLayerSpec*)ls;
      int nx = tdls->twod.n_vals * 2;
      if(lay->unit_groups) {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 4, nx, 1,
           MAX(lay->gp_geom.x,1), MAX(lay->gp_geom.y,1));
      }
      else {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 2, nx, 1);
      }
    }
    else {
      if(lay->unit_groups) {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 4,
           MAX(lay->un_geom.x,1), MAX(lay->un_geom.y,1),
           MAX(lay->gp_geom.x,1), MAX(lay->gp_geom.y,1));
      }
      else {
        data_table->FindMakeColName
          (lay->name, lay_idx, DataTable::VT_FLOAT, 2,
           MAX(lay->un_geom.x,1), MAX(lay->un_geom.y,1));
      }
    }
  }
  data_table->StructUpdate(false);
//   if(taMisc::gui_active) {
//     tabMisc::DelayedFunCall_gui(data_table, "BrowserSelectMe");
//   }

  UpdateLayerWriters(net, data_table);

  return true;
}

///////////////////////////////////////////////////////////////
//                      SRN Context
///////////////////////////////////////////////////////////////

bool LeabraWizard::SRNContext(LeabraNetwork* net) {
  if(TestError(!net, "SRNContext", "must have basic constructed network first")) {
    return false;
  }
  OneToOnePrjnSpec* otop = (OneToOnePrjnSpec*)net->FindMakeSpec("CtxtPrjn", &TA_OneToOnePrjnSpec);
  LeabraContextLayerSpec* ctxts = (LeabraContextLayerSpec*)net->FindMakeSpec("CtxtLayerSpec", &TA_LeabraContextLayerSpec);

  if((otop == NULL) || (ctxts == NULL)) {
    return false;
  }

  LeabraLayer* hidden = (LeabraLayer*)net->FindLayer("Hidden");
  LeabraLayer* ctxt = (LeabraLayer*)net->FindMakeLayer("Context");

  if((hidden == NULL) || (ctxt == NULL)) return false;

  ctxt->SetLayerSpec(ctxts);
  ctxt->un_geom = hidden->un_geom;

  net->layers.MoveAfter(hidden, ctxt);
  net->FindMakePrjn(ctxt, hidden, otop); // one-to-one into the ctxt layer
  net->FindMakePrjn(hidden, ctxt);       // std prjn back into the hidden from context
  net->Build();
  return true;
}

