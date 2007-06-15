// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "leabra.h"

#include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

static void leabra_converter_init() {
  DumpFileCvt* cvt = new DumpFileCvt("Leabra", "LeabraUnit");
  cvt->repl_strs.Add(NameVar("_MGroup", "_Group"));
  cvt->repl_strs.Add(NameVar("Project", "V3LeabraProject"));
  cvt->repl_strs.Add(NameVar("V3LeabraProject_Group", "Project_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("V3LeabraProjection", "LeabraPrjn"));       // new prjn type!
  cvt->repl_strs.Add(NameVar("LeabraPrjn_Group", "Projection_Group"));
  cvt->repl_strs.Add(NameVar("Network", "LeabraNetwork"));
  cvt->repl_strs.Add(NameVar("LeabraNetwork_Group", "Network_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("LeabraWiz", "LeabraWizard"));
  // obsolete types get replaced with taBase..
  cvt->repl_strs.Add(NameVar("WinView_Group", "taBase_Group"));
  cvt->repl_strs.Add(NameVar("ProjViewState_List", "taBase_List"));
  cvt->repl_strs.Add(NameVar("NetView", "taNBase"));
  cvt->repl_strs.Add(NameVar("DataTable", "taNBase"));
  cvt->repl_strs.Add(NameVar("EnviroView", "taNBase"));
  cvt->repl_strs.Add(NameVar("Xform", "taBase"));
  cvt->repl_strs.Add(NameVar("ImageEnv", "ScriptEnv"));
  cvt->repl_strs.Add(NameVar("unique/w=", "unique"));
  taMisc::file_converters.Add(cvt);
}

void leabra_module_init() {
  ta_Init_leabra();		// initialize types 
  leabra_converter_init();	// configure converter
}

// module initialization
InitProcRegistrar mod_init_leabra(leabra_module_init);

//////////////////////////
//  	Con, Spec	//
//////////////////////////

void WtScaleSpec::Initialize() {
  rel = 1.0f;
  abs = 1.0f;
}

void WtSigSpec::Initialize() {
  gain = 6.0f;
  off = 1.25f;
}

void LearnMixSpec::Initialize() {
  hebb = .001f;
  err = 1.0f - hebb;
  err_sb = true;
}

void LearnMixSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  err = 1.0f - hebb;
}

void SAvgCorSpec::Initialize() {
  cor = .4f;
  thresh = .001f;
  norm_con_n = false;
}

void AdaptRelNetinSpec::Initialize() {
  on = false;
  trg_fm_input = .85f;
  trg_fm_output = .15f;
  trg_lateral = 0.0f;
  trg_sum = 1.0f;
  tol_lg = 0.05f;
  tol_sm = 0.2f;
  rel_lrate = .1f;
}

void AdaptRelNetinSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  trg_sum = trg_fm_input + trg_fm_output + trg_lateral;
  TestWarning(fabsf(trg_sum - 1.0f) > .01, "UAE", "target values do not sum to 1");
}

bool AdaptRelNetinSpec::CheckInTolerance(float trg, float val) {
  float tol;
  if(trg > .25f)
    tol = tol_lg * trg;
  else
    tol = tol_sm * trg;
  if(fabsf(trg - val) <= tol) return true;
  return false;
}


void LeabraConSpec::Initialize() {
  min_obj_type = &TA_LeabraCon;
  wt_limits.min = 0.0f;
  wt_limits.max = 1.0f;
  wt_limits.sym = true;
  wt_limits.type = WeightLimits::MIN_MAX;
  inhib = false;

  wt_sig_fun.x_range.min = 0.0f;
  wt_sig_fun.x_range.max = 1.0f;
  wt_sig_fun.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun.UpdateAfterEdit();

  wt_sig_fun_inv.x_range.min = 0.0f;
  wt_sig_fun_inv.x_range.max = 1.0f;
  wt_sig_fun_inv.res = 1.0e-5f;	// 1e-6 = 1.9Mb & 33% slower!, but 4x more accurate; 1e-5 = .19Mb
  wt_sig_fun_inv.UpdateAfterEdit();

  wt_sig_fun_lst.off = -1;   wt_sig_fun_lst.gain = -1; // trigger an update
  wt_sig_fun_res = -1.0;

  rnd.mean = .5f;
  rnd.var = .25f;
  lrate = .01f;
  cur_lrate = .01f;
  lrs_value = EPOCH;
  lrate_sched.interpolate = false;
}

void LeabraConSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_LeabraConSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(wt_scale, this);
  taBase::Own(wt_sig, this);
  taBase::Own(lrate_sched, this);
  taBase::Own(lmix, this);
  taBase::Own(savg_cor, this);
  taBase::Own(rel_net_adapt, this);
  taBase::Own(wt_sig_fun, this);
  taBase::Own(wt_sig_fun_inv, this);
  taBase::Own(wt_sig_fun_lst, this);
  CreateWtSigFun();
}

void LeabraConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lrate_sched.UpdateAfterEdit();
  CreateWtSigFun();
  lmix.UpdateAfterEdit();
}

void LeabraConSpec::Defaults() {
  wt_scale.Defaults();
  wt_sig.Defaults();
  lmix.Defaults();
  savg_cor.Defaults();
  Initialize();
}

void LeabraConSpec::SetCurLrate(int epoch, LeabraNetwork* net) {
  cur_lrate = lrate;		// as a backup..
  if(lrs_value == NO_LRS) return;

  if(lrs_value == EXT_REW_AVG) {
    LeabraLayer* er_lay = LeabraLayerSpec::FindLayerFmSpecNet(net, &TA_ExtRewLayerSpec);
    if(er_lay != NULL) {
      LeabraUnit* un = (LeabraUnit*)er_lay->units.Leaf(0);
      float avg_rew = un->act_avg;
      int ar_pct = (int)(100.0f * avg_rew);
      cur_lrate = lrate * lrate_sched.GetVal(ar_pct);
      return;
    }
    else {
      TestWarning(true, "SetCurLrate", "appropriate ExtRew layer not found for EXT_REW_AVG, reverting to EPOCH!");
      SetUnique("lrs_value", true);
      lrs_value = EPOCH;
      UpdateAfterEdit();
    }
  }
  if(lrs_value == EXT_REW_STAT) {
    int arval = 0;
    if(net->epoch < 1) {
      arval = lrate_sched.last_ctr;
    }
    else {
      arval = (int)(100.0f * net->avg_ext_rew);
    }
    cur_lrate = lrate * lrate_sched.GetVal(arval);
  }

  // this is no longer relevant
//   if(lrs_value == SE_STAT) {
//     int seval = 0;
//     if(net->epoch < 1) {
//       seval = lrate_sched.last_ctr;
//     }
//     else {
//       seval = (int);
//     }
//     cur_lrate = lrate * lrate_sched.GetVal(seval);
//   }

  if(lrs_value == EPOCH) {
    cur_lrate = lrate * lrate_sched.GetVal(epoch);
  }
}

void LeabraConSpec::CreateWtSigFun() {
  if((wt_sig_fun_lst.gain == wt_sig.gain) && (wt_sig_fun_lst.off == wt_sig.off)
     && (wt_sig_fun_res == wt_sig_fun.res))
    return;
  wt_sig_fun.AllocForRange();
  int i;
  for(i=0; i<wt_sig_fun.size; i++) {
    float w = wt_sig_fun.Xval(i);
    wt_sig_fun[i] = WtSigSpec::SigFun(w, wt_sig.gain, wt_sig.off);
  }
  wt_sig_fun_inv.AllocForRange();
  for(i=0; i<wt_sig_fun_inv.size; i++) {
    float w = wt_sig_fun_inv.Xval(i);
    wt_sig_fun_inv[i] = WtSigSpec::SigFunInv(w, wt_sig.gain, wt_sig.off);
  }
  // prevent needless recomputation of this lookup table..
  wt_sig_fun_lst.gain = wt_sig.gain; wt_sig_fun_lst.off = wt_sig.off;
  wt_sig_fun_res = wt_sig_fun.res;
}

void LeabraConSpec::GraphWtSigFun(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_WtSigFun", true);
    newguy = true;
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* lnwt = graph_data->FindMakeColName("LinWt", idx, VT_FLOAT);
  DataCol* efwt = graph_data->FindMakeColName("EffWt", idx, VT_FLOAT);
  lnwt->SetUserData("MIN", 0.0f);
  lnwt->SetUserData("MAX", 1.0f);
  efwt->SetUserData("MIN", 0.0f);
  efwt->SetUserData("MAX", 1.0f);

  float x;
  for(x = 0.0f; x <= 1.0f; x += .01f) {
    float y = WtSigSpec::SigFun(x, wt_sig.gain, wt_sig.off);
    graph_data->AddBlankRow();
    lnwt->SetValAsFloat(x, -1);
    efwt->SetValAsFloat(y, -1);
  }
  graph_data->StructUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}

void LeabraRecvCons::Initialize() {
  SetConType(&TA_LeabraCon);
  scale_eff = 0.0f;
  savg_cor = 1.0f;
  net = 0.0;
}

void LeabraRecvCons::Copy_(const LeabraRecvCons& cp) {
  scale_eff = cp.scale_eff;
  savg_cor = cp.savg_cor;
  net = cp.net;
}

void LeabraSendCons::Initialize() {
  SetConType(&TA_LeabraCon);
}

void LeabraBiasSpec::Initialize() {
  //   min_obj_type = &TA_RecvCons; // don't bother..
  SetUnique("rnd", true);
  SetUnique("wt_limits", true);
  SetUnique("wt_scale", true);
  rnd.mean = 0.0f;
  rnd.var = 0.0f;
  wt_limits.min = -1.0f;
  wt_limits.max = 5.0f;
  wt_limits.sym = false;
  wt_limits.type = WeightLimits::NONE;
  dwt_thresh = .1f;
}

bool LeabraBiasSpec::CheckObjectType_impl(TAPtr obj) {
  // don't allow anything to point to a biasspec except the unitspec!
  if(!obj->InheritsFrom(TA_BaseSpec) &&
     !obj->InheritsFrom(TA_LeabraCon)) return false;
  return true;
}

void LeabraBiasSpec::Defaults() {
  inherited::Defaults();
  Initialize();
}

//////////////////////////
//  	Unit, Spec	//
//////////////////////////

void LeabraChannels::Initialize() {
  e = l = i = h = a = 0.0f;
}

void LeabraChannels::Copy_(const LeabraChannels& cp) {
  e = cp.e;
  l = cp.l;
  i = cp.i;
  h = cp.h;
  a = cp.a;
}

void VChanSpec::Initialize() {
  on = false;
  b_dt = .01f;
  a_thr = .5f;
  d_thr = .1f;
  g_dt = .1f;
  init = false;
}

void VChanBasis::Initialize() {
  hyst = acc = 0.0f;
  hyst_on = acc_on = false;
  g_h = g_a = 0.0f;
}

void VChanBasis::Copy_(const VChanBasis& cp) {
  hyst = cp.hyst;
  acc = cp.acc;
  hyst_on = cp.hyst_on;
  acc_on = cp.acc_on;
  g_h = cp.g_h;
  g_a = cp.g_a;
}

void ActFunSpec::Initialize() {
  thr = .25f;
  gain = 600.0f;
  nvar = .005f;
  avg_dt = .005f;
  send_delta = true;
  i_thr = STD;
}

void SpikeFunSpec::Initialize() {
  decay = 0.05f;
  v_m_r = 0.0f;
  eq_gain = 10.0f;
  eq_dt = 0.02f;
  hard_gain = .4f;
  // vm_dt of .1 should also be used; vm_noise var .002???
}

void DepressSpec::Initialize() {
  p_spike = P_NXX1;
  rec = .2f;
  asymp_act = .5f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  max_amp = (.95f * depl + rec) / rec;
}

void DepressSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(rec < .00001f) rec = .00001f;
  if(asymp_act < .00001f) asymp_act = .00001f;
  if(asymp_act > 1.0f) asymp_act = 1.0f;
  depl = rec * (1.0f - asymp_act) / (asymp_act * .95f);
  depl = MAX(depl, 0.0f);
  max_amp = (.95f * depl + rec) / rec;
}

void OptThreshSpec::Initialize() {
  send = .1f;
  delta = 0.005f;
  learn = 0.01f;
  updt_wts = true;
  phase_dif = 0.0f;		// .8 also useful
}

void DtSpec::Initialize() {
  vm = 0.3f;
  net = 0.7f;
  d_vm_max = 0.025f;
  vm_eq_cyc = 0;
  vm_eq_dt = 1.0f;
}

void ActRegSpec::Initialize() {
  on = false;
  min = 0.0f;
  max = .35f;
  wt_dt = 0.2f;
}

void MaxDaSpec::Initialize() {
  val = INET_DA;
  inet_scale = 1.0;
  lay_avg_thr = 0.01f;
}

void LeabraUnitSpec::Initialize() {
  min_obj_type = &TA_LeabraUnit;
  bias_con_type = &TA_LeabraCon;
  bias_spec.SetBaseType(&TA_LeabraConSpec);

  sse_tol = .5f;

  act_fun = NOISY_XX1;

  clamp_range.min = .0f;
  clamp_range.max = .95f;
  clamp_range.UpdateAfterEdit();

  vm_range.max = 1.0f;
  vm_range.min = 0.0f;
  vm_range.UpdateAfterEdit();

  v_m_init.type = Random::UNIFORM;
  v_m_init.mean = .15f;
  v_m_init.var = 0.0f;

  g_bar.e = 1.0f;
  g_bar.l = 0.1f;
  g_bar.i = 1.0f;
  g_bar.h = 0.01f;
  g_bar.a = 0.03f;
  e_rev.e = 1.0f;
  e_rev.l = 0.15f;
  e_rev.i = 0.15f;
  e_rev.h = 1.0f;
  e_rev.a = 0.0f;

  hyst.b_dt = .05f;
  hyst.a_thr = .8f;
  hyst.d_thr = .7f;

  noise_type = NO_NOISE;
  noise.type = Random::GAUSSIAN;
  noise.var = .001f;

  noise_conv.x_range.min = -.05f;
  noise_conv.x_range.max = .05f;
  noise_conv.res = .001f;
  noise_conv.UpdateAfterEdit();

  nxx1_fun.x_range.min = -.03f;
  nxx1_fun.x_range.max = .20f;
  nxx1_fun.res = .001f;
  nxx1_fun.UpdateAfterEdit();

  CreateNXX1Fun();
}

void LeabraUnitSpec::Defaults() {
  act.Defaults();
  spike.Defaults();
  depress.Defaults();
  opt_thresh.Defaults();
  dt.Defaults();
  act_reg.Defaults();
  Initialize();
  //  bias_spec.SetSpec(bias_spec.SPtr());
}

void LeabraUnitSpec::InitLinks() {
  bias_spec.type = &TA_LeabraBiasSpec;
  inherited::InitLinks();
  children.SetBaseType(&TA_LeabraUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(act, this);
  taBase::Own(spike, this);
  taBase::Own(depress, this);
  taBase::Own(opt_thresh, this);
  taBase::Own(clamp_range, this);
  taBase::Own(vm_range, this);
  taBase::Own(v_m_init, this);
  taBase::Own(dt, this);
  taBase::Own(g_bar, this);
  taBase::Own(e_rev, this);
  taBase::Own(hyst, this);
  taBase::Own(acc, this);
  taBase::Own(act_reg, this);
  taBase::Own(maxda, this);
  taBase::Own(noise, this);
  taBase::Own(noise_sched, this);
  taBase::Own(nxx1_fun, this);
  taBase::Own(noise_conv, this);
}

void LeabraUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  clamp_range.UpdateAfterEdit();
  vm_range.UpdateAfterEdit();
  depress.UpdateAfterEdit();
  noise_sched.UpdateAfterEdit();
  CreateNXX1Fun();
  if(act_fun == DEPRESS)
    act_range.max = depress.max_amp;
}

bool LeabraUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

  Network* net = GET_MY_OWNER(Network);
  bool rval = true;

  act.send_delta = ((LeabraNetwork*)net)->send_delta; // always copy from network, so it is global..
  
  for(int g=0; g<un->send.size; g++) {
    LeabraSendCons* send_gp = (LeabraSendCons*)un->send.FastEl(g);
    if(send_gp->cons.size < 2) continue;
    Unit* ru = send_gp->Un(0);
    float first_sc = ((LeabraRecvCons*)ru->recv.FastEl(send_gp->recv_idx))->scale_eff;
    for(int j=1; j<send_gp->cons.size; j++) {
      float sc = ((LeabraRecvCons*)ru->recv.FastEl(send_gp->recv_idx))->scale_eff;
      if(CheckError((sc != first_sc), quiet, rval,
		    "the effective weight scales for different sending connections within a group are not all the same!  Sending Layer:",
		    send_gp->prjn->from->name, ", Rev Layer:", send_gp->prjn->layer->name,
		    ", first_sc: ", String(first_sc), ", sc: ", String(sc)))
	return false;
    }
  }

  if(CheckError((opt_thresh.updt_wts &&
		 (net->wt_update != Network::ON_LINE) && (net->train_mode != Network::TEST)),
		quiet, rval,
		"cannot use opt_thresh.updt_wts when wt_update is not ON_LINE",
		"I turned this flag off for you")) {
    SetUnique("opt_thresh", true);
    opt_thresh.updt_wts = false;
  }

  if(CheckError((opt_thresh.updt_wts && act_reg.on && (act_reg.min > 0.0f)), quiet, rval,
		"cannot use opt_thresh.updt_wts when act_reg is on and min > 0",
		"I turned this flag off for you")) {
    SetUnique("opt_thresh", true);
    opt_thresh.updt_wts = false;
  }
  return true;
}

void LeabraUnitSpec::CreateNXX1Fun() {
  // first create the gaussian noise convolver
  noise_conv.AllocForRange();
  int i;
  float var = act.nvar * act.nvar;
  for(i=0; i < noise_conv.size; i++) {
    float x = noise_conv.Xval(i);
    noise_conv[i] = expf(-((x * x) / var));
  }

  // normalize it
  float sum = 0.0f;
  for(i=0; i < noise_conv.size; i++)
    sum += noise_conv[i];
  for(i=0; i < noise_conv.size; i++)
    noise_conv[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fun.x_range.min + noise_conv.x_range.min;
  fun.x_range.max = nxx1_fun.x_range.max + noise_conv.x_range.max;
  fun.res = nxx1_fun.res;
  fun.UpdateAfterEdit();
  fun.AllocForRange();

  if(act_fun == LeabraUnitSpec::NOISY_LINEAR) {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
	val = act.gain * x;
      fun[i] = val;
    }
  }
  else {
    for(i=0; i<fun.size; i++) {
      float x = fun.Xval(i);
      float val = 0.0f;
      if(x > 0.0f)
	val = (act.gain * x) / ((act.gain * x) + 1.0f);
      fun[i] = val;
    }
  }

  nxx1_fun.Convolve(fun, noise_conv);
}

void LeabraUnitSpec::Init_Weights(Unit* u) {
  inherited::Init_Weights(u);
  LeabraUnit* lu = (LeabraUnit*)u;
  lu->act_avg = .5 * (act_reg.max + MAX(act_reg.min, 0.0f));
  lu->misc_1 = 0.0f;
  if(act_fun != DEPRESS)
    lu->spk_amp = 0.0f;
  lu->vcb.hyst = lu->vcb.g_h = 0.0f;
  lu->vcb.hyst_on = false;
  lu->vcb.acc = lu->vcb.g_a = 0.0f;
  lu->vcb.acc_on = false;
}

void LeabraUnitSpec::Init_ActAvg(LeabraUnit* u) {
  u->act_avg = .5 * (act_reg.max + MAX(act_reg.min, 0.0f));
}  

void LeabraUnitSpec::SetCurLrate(LeabraUnit* u, LeabraNetwork* net, int epoch) {
  ((LeabraConSpec*)bias_spec.SPtr())->SetCurLrate(epoch, net);
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(!recv_gp->cons.size) continue;
    recv_gp->SetCurLrate(epoch, net);
  }
}

////////////////////////////////////////////
//	Stage 0: at start of settling	  // 
////////////////////////////////////////////

void LeabraUnitSpec::Init_Netin(LeabraUnit* u) {
  if(act.send_delta) {
    u->net_delta = 0.0f;
    u->g_i_delta = 0.0f;
    u->net = 0.0f;		// important for soft-clamped layers
    u->gc.i = 0.0f;
  }
  else {
    u->gc.i = 0.0f;
    u->net = u->clmp_net;
  }

  // not initialized by default.
//   LeabraRecvCons* recv_gp;
//   int g;
//   FOR_ITR_GP(LeabraRecvCons, recv_gp, u->recv., g) {
//     recv_gp->net = 0.0f;
//   } 
}

void LeabraUnitSpec::Init_Acts(LeabraUnit* ru, LeabraLayer*) {
  inherited::Init_Acts(ru);
  //  ru->clmp_net = 0.0f;
  ru->net_scale = 0.0f;
  ru->bias_scale = 0.0f;
  ru->prv_net = 0.0f;
  ru->prv_g_i = 0.0f;
  if(hyst.init) {
    ru->vcb.hyst = ru->vcb.g_h = 0.0f;
    ru->vcb.hyst_on = false;
  }
  if(acc.init) {
    ru->vcb.acc = ru->vcb.g_a = 0.0f;
    ru->vcb.acc_on = false;
  }
  ru->gc.l = 0.0f;
  ru->gc.i = 0.0f;
  ru->gc.h = 0.0f;
  ru->gc.a = 0.0f;
  ru->I_net = 0.0f;
  ru->v_m = v_m_init.Gen();
  ru->da = 0.0f;
  ru->act = 0.0f;
  ru->act_eq = 0.0f;
  ru->act_p = ru->act_m = ru->act_dif = 0.0f;

  ru->act_sent = 0.0f;
  ru->act_delta = 0.0f;
  ru->net_raw = 0.0f;
  ru->net_delta = 0.0f;
  ru->g_i_delta = 0.0f;
  ru->g_i_raw = 0.0f;

  ru->i_thr = 0.0f;
  if(act_fun == DEPRESS)
    ru->spk_amp = act_range.max;
}

void LeabraUnitSpec::Compute_NetinScale(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  // this is all receiver-based and done only at beginning of settling
  u->net_scale = 0.0f;	// total of scale values for this unit's inputs

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesioned() || !recv_gp->cons.size)	continue;
     // this is the normalization value: takes into account target activity of layer
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    WtScaleSpec& wt_scale = cs->wt_scale;
    float savg = lay->kwta.pct;
    if(cs->savg_cor.norm_con_n)	// sometimes it makes sense to just do it by the group n
      recv_gp->scale_eff = wt_scale.NetScale() * (1.0f / ((float)recv_gp->cons.size * savg));
    else
      recv_gp->scale_eff = wt_scale.NetScale() * (1.0f / ((float)lay->units.leaves * savg));
    u->net_scale += wt_scale.rel;
  }
  // add the bias weight into the netinput, scaled by 1/n
  if(u->bias.cons.size) {
    LeabraConSpec* bspec = (LeabraConSpec*)bias_spec.SPtr();
    u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
    if(u->n_recv_cons > 0)
      u->bias_scale /= (float)u->n_recv_cons; // one over n scaling for bias!
  }
  // now renormalize
  if(u->net_scale == 0.0f) return;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesioned() || !recv_gp->cons.size)	continue;
    recv_gp->scale_eff /= u->net_scale; // normalize by total connection scale
  }
}

void LeabraUnitSpec::Compute_NetinRescale(LeabraUnit* u, LeabraLayer*, LeabraNetwork*, float new_scale) {
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesioned() || !recv_gp->cons.size)	continue;
    recv_gp->scale_eff *= new_scale;
  }
  // rescale existing netins so that the delta is not even noticed!
  u->net *= new_scale;
  u->clmp_net *= new_scale;
  u->net_raw *= new_scale;
}

void LeabraUnitSpec::Init_ClampNet(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  u->clmp_net = 0.0f;
  if(u->bias.cons.size) {		// add bias weight
    u->clmp_net += u->bias_scale * u->bias.Cn(0)->wt;
  }
}

void LeabraUnitSpec::Send_ClampNet(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(u->act > opt_thresh.send) {
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->prjn->layer->lesioned() || !send_gp->cons.size) continue;
      if(TestWarning(((LeabraConSpec*)send_gp->GetConSpec())->inhib, "Send_ClampNet",
		     "cannot send inhibition from a hard-clamped layer!  Set layerspec clamp.hard off!")) {
	continue;
      }
      send_gp->Send_ClampNet(u);
    }
  }
}

////////////////////////////////////
//	Stage 1: netinput 	  //
////////////////////////////////////

void LeabraUnitSpec::Send_Netin(LeabraUnit* u, LeabraLayer*) {
  // sender-based (and this fun not called on hard_clamped EXT layers)
  if(u->act > opt_thresh.send) {
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size) continue;
      send_gp->Send_Netin(u);
    }
  }
}

void LeabraUnitSpec::Send_NetinDelta(LeabraUnit* u, LeabraLayer*) {
  if(u->act > opt_thresh.send) {
    if(fabsf(u->act_delta) > opt_thresh.delta) {
      for(int g=0; g<u->send.size; g++) {
	LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
	LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
	if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size)	continue;
	send_gp->Send_NetinDelta(u);
      }
      u->act_sent = u->act;	// cache the last sent value
    }
  }
  else if(u->act_sent > opt_thresh.send) {
    u->act_delta = - u->act_sent; // un-send the last above-threshold activation to get back to 0
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned() || tol->hard_clamped || !send_gp->cons.size)	continue;
      send_gp->Send_NetinDelta(u);
    }
    u->act_sent = 0.0f;		// now it effectively sent a 0..
  }
}

//////////////////////////////////////////////////////////////////
//	Stage 2: netinput averages and clamping (if necc)	//
//////////////////////////////////////////////////////////////////

void LeabraUnitSpec::Compute_HardClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  u->net = u->prv_net = u->ext * lay->stm_gain;
  u->act_eq = clamp_range.Clip(u->ext);
  if(act_fun == SPIKE)
    u->act = spike.hard_gain * u->act_eq;
  else
    u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
}

// NOTE: these two functions should always be the same modulo the clamp_range.Clip

void LeabraUnitSpec::Compute_HardClampNoClip(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  u->net = u->prv_net = u->ext * lay->stm_gain;
  //  u->act_eq = clamp_range.Clip(u->ext);
  u->act_eq = u->ext;
  if(act_fun == SPIKE)
    u->act = spike.hard_gain * u->act_eq;
  else
    u->act = u->act_eq;
  if(u->act_eq == 0.0f)
    u->v_m = e_rev.l;
  else
    u->v_m = act.thr + u->act_eq / act.gain;
  u->da = u->I_net = 0.0f;
}

bool LeabraUnitSpec::Compute_SoftClamp(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork*) {
  bool inc_gain = false;
  if(u->ext_flag & Unit::EXT) {
    if((u->ext > .5f) && (u->act < .5f))
      inc_gain = true;	// must increase gain because targ unit is < .5..

    u->net += u->ext * lay->stm_gain;
  }
  return inc_gain;
}

//////////////////////////////////////////
//	Stage 3: inhibition		//
//////////////////////////////////////////

float LeabraUnitSpec::Compute_IThreshAll(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // including the ga and gh terms
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)
	   + u->gc.a * (e_rev.a - act.thr) + u->gc.h * (e_rev.h - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoA(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // NOT including the ga term
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)
	   + u->gc.h * (e_rev.h - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoH(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // NOT including the gh terms
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)
	   + u->gc.a * (e_rev.a - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThreshNoAH(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  float non_bias_net = u->net;
  if(u->bias.cons.size)		// subtract out bias weights so they can change k
    non_bias_net -= u->bias_scale * u->bias.Cn(0)->wt;

  // NOT including the ga and gh terms
  return ((non_bias_net * (e_rev.e - act.thr) + u->gc.l * (e_rev.l - act.thr)) /
	  (act.thr - e_rev.i));
} 

float LeabraUnitSpec::Compute_IThresh(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  switch(act.i_thr) {
  case ActFunSpec::STD:
    return Compute_IThreshAll(u, lay, net);
  case ActFunSpec::NO_A:
    return Compute_IThreshNoA(u, lay, net);
  case ActFunSpec::NO_H:
    return Compute_IThreshNoH(u, lay, net);
  case ActFunSpec::NO_AH:
    return Compute_IThreshNoAH(u, lay, net);
  }
  return 0.0f;
} 

//////////////////////////////////////////
//	Stage 4: the final activation 	//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_Act(LeabraUnit* u, LeabraLayer* lay, LeabraInhib* thr, LeabraNetwork* net) {
  Compute_Conduct(u, lay, thr, net);
  Compute_Vm(u, lay, thr, net);
  Compute_ActFmVm(u, lay, thr, net);
  Compute_SelfReg(u, lay, thr, net);
  u->act_delta = u->act - u->act_sent;
}

void LeabraUnitSpec::Compute_Conduct(LeabraUnit* u, LeabraLayer* lay, LeabraInhib*, LeabraNetwork*) {
  // total conductances
  float g_bar_i_val = g_bar.i;
  float	g_bar_e_val = g_bar.e;
  float	g_bar_l_val = g_bar.l;
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->spec.SPtr();
  if((ls->adapt_i.type == AdaptISpec::G_BAR_I) || (ls->adapt_i.type == AdaptISpec::G_BAR_IL))
    g_bar_i_val = lay->adapt_i.g_bar_i; // adapting value..
  if(ls->adapt_i.type == AdaptISpec::G_BAR_IL)
    g_bar_l_val = lay->adapt_i.g_bar_l; // adapting value..
  u->gc.i *= g_bar_i_val;
  u->net *= g_bar_e_val;
  u->gc.l = g_bar_l_val;
  u->gc.h = g_bar.h * u->vcb.g_h;
  u->gc.a = g_bar.a * u->vcb.g_a;
}

void LeabraUnitSpec::Compute_Vm(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork* net) {
  if(dt.vm_eq_cyc > net->cycle) {
    // directly go to equilibrium value
    float new_v_m= (((u->net * e_rev.e) + (u->gc.l * e_rev.l) + (u->gc.i * e_rev.i) +
		     (u->gc.h * e_rev.h) + (u->gc.a * e_rev.a)) / 
		    (u->net + u->gc.l + u->gc.i + u->gc.h + u->gc.a));
    u->I_net = new_v_m - u->v_m; // time integrate: not really I_net but hey
    u->v_m += dt.vm_eq_dt * u->I_net;
  }
  else {
    u->I_net = 
      (u->net * (e_rev.e - u->v_m)) + (u->gc.l * (e_rev.l - u->v_m)) + 
      (u->gc.i * (e_rev.i - u->v_m)) + (u->gc.h * (e_rev.h - u->v_m)) +
      (u->gc.a * (e_rev.a - u->v_m));

    float dvm = dt.vm * u->I_net;
    if(dvm > dt.d_vm_max) dvm = dt.d_vm_max;
    else if(dvm < -dt.d_vm_max) dvm = -dt.d_vm_max;
    u->v_m += dvm;
  }

  if((noise_type == VM_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->v_m += noise_sched.GetVal(net->cycle) * noise.Gen();
  }

  u->v_m = vm_range.Clip(u->v_m);
}

void LeabraUnitSpec::Compute_ActFmVm(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork* net) {
  float new_act = u->v_m - act.thr;
  switch(act_fun) {
  case NOISY_XX1: {
    if(new_act <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(new_act >= nxx1_fun.x_range.max) {
      new_act *= act.gain;
      new_act = new_act / (new_act + 1.0f);
    }
    else {
      new_act = nxx1_fun.Eval(new_act);
    }
  }
  break;
  case XX1: {
    if(new_act < 0.0f)
      new_act = 0.0f;
    else {
      new_act *= act.gain;
      new_act = new_act / (new_act + 1.0f);
    }
  }
  break;
  case NOISY_LINEAR: {
    if(new_act <= nxx1_fun.x_range.min)
      new_act = 0.0f;
    else if(new_act >= nxx1_fun.x_range.max) {
      new_act *= act.gain;
    }
    else {
      new_act = nxx1_fun.Eval(new_act);
    }
  }
  break;
  case LINEAR: {
    if(new_act < 0.0f)
      new_act = 0.0f;
    else
      new_act *= act.gain;
  }
  break;
  case DEPRESS: {
    // new_act = spike probability
    if(depress.p_spike == DepressSpec::P_NXX1) {
      if(new_act <= nxx1_fun.x_range.min)
	new_act = 0.0f;
      else if(new_act >= nxx1_fun.x_range.max) {
	new_act *= act.gain;
	new_act = new_act / (new_act + 1.0f);
      }
      else {
	new_act = nxx1_fun.Eval(new_act);
      }
    }
    else {
      if(new_act < 0.0f)
	new_act = 0.0f;
      else {
	new_act *= act.gain;
	new_act = MIN(new_act, 1.0f);
      }
    }
    new_act *= u->spk_amp; // actual output = probability * amplitude
    u->spk_amp += -new_act * depress.depl + (act_range.max - u->spk_amp) * depress.rec;
    u->spk_amp = act_range.Clip(u->spk_amp);
  }
  break;
  case SPIKE: {
    float spike_act = 0.0f;
    if(u->v_m > act.thr) {
      u->act = 1.0f;
      u->v_m = spike.v_m_r;
      spike_act = 1.0f;
    }
    else {
      u->act *= (1.0f - spike.decay);
    }
    float new_eq = u->act_eq / spike.eq_gain;
    if(spike.eq_dt > 0.0f) {
      new_eq = act_range.Clip(spike.eq_gain * ((1.0f - spike.eq_dt) * new_eq + spike.eq_dt * spike_act));
    }
    else {
      if(net->cycle > 0)
	new_eq *= (float)net->cycle;
      new_eq = act_range.Clip(spike.eq_gain * (new_eq + spike_act) / (float)(net->cycle+1));
    }
    u->da = new_eq - u->act_eq;	// da is on equilibrium activation
    u->act_eq = new_eq;
  }
  break;
  }

  if(act_fun != SPIKE) {
    u->da = new_act - u->act;
    if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
      new_act += noise_sched.GetVal(net->cycle) * noise.Gen();
    }
    u->act = u->act_eq = act_range.Clip(new_act);
  }
}

void LeabraUnitSpec::Compute_SelfReg(LeabraUnit* u, LeabraLayer*, LeabraInhib*, LeabraNetwork*) {
  // fast-time scale updated during settling
  hyst.UpdateBasis(u->vcb.hyst, u->vcb.hyst_on, u->vcb.g_h, u->act_eq);
  acc.UpdateBasis(u->vcb.acc, u->vcb.acc_on, u->vcb.g_a, u->act_eq);
}

void LeabraUnitSpec::Compute_MaxDa(LeabraUnit* u, LeabraLayer* lay, LeabraInhib*, LeabraNetwork* net) {
  float fda;
  if(maxda.val == MaxDaSpec::DA_ONLY)
    fda = fabsf(u->da);
  else if(maxda.val == MaxDaSpec::INET_ONLY)
    fda = fabsf(maxda.inet_scale * u->I_net);
  else {
    if(lay->acts.avg <= maxda.lay_avg_thr)
      fda = fabsf(maxda.inet_scale * u->I_net);
    else
      fda = fabsf(u->da);
  }
  net->maxda = MAX(fda, net->maxda);
}

//////////////////////////////////////////
//	Stage 5: Between Events 	//
//////////////////////////////////////////

void LeabraUnitSpec::PhaseInit(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if(!(u->ext_flag & Unit::TARG))
    return;

  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    if(!(lay->ext_flag & Unit::TARG)) {	// layer isn't a target but unit is..
      u->targ = u->ext;
    }
    u->ext = 0.0f;
    u->UnSetExtFlag(Unit::EXT);
  }
  else {
    u->ext = u->targ;
    u->SetExtFlag(Unit::EXT);
  }
}

void LeabraUnitSpec::DecayPhase(LeabraUnit* u, LeabraLayer*, LeabraNetwork*, float decay) {
  u->v_m -= decay * (u->v_m - v_m_init.mean);
  u->act -= decay * u->act;
  u->act_eq -= decay * u->act_eq;
  u->prv_net -= decay * u->prv_net;
  u->prv_g_i -= decay * u->prv_g_i;
  u->vcb.hyst -= decay * u->vcb.hyst;
  u->vcb.acc -= decay * u->vcb.acc;
  if(act_fun == DEPRESS)
    u->spk_amp += (act_range.max - u->spk_amp) * decay;

  // reset the rest of this stuff just for clarity
  u->act_sent = 0.0f;
  u->act_delta = u->act;
  u->net_raw = 0.0f;
  u->net_delta = 0.0f;
  u->g_i_raw = 0.0f;
  u->g_i_delta = 0.0f;
  
  u->net = 0.0f;
  u->net_scale = u->bias_scale = 0.0f;
  u->da = u->I_net = 0.0f;
}

void LeabraUnitSpec::DecayEvent(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net, float decay) {
  LeabraUnitSpec::DecayPhase(u, lay, net, decay);
}

void LeabraUnitSpec::ExtToComp(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(!(u->ext_flag & Unit::EXT))
    return;
  u->ext_flag = Unit::COMP;
  u->targ = u->ext;
  u->ext = 0.0f;
}

void LeabraUnitSpec::TargExtToComp(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  if(!(u->ext_flag & Unit::TARG_EXT))
    return;
  u->ext_flag = Unit::COMP;
  u->targ = u->ext;
  u->ext = 0.0f;
}

void LeabraUnitSpec::PostSettle(LeabraUnit* u, LeabraLayer*, LeabraInhib*,
				LeabraNetwork* net, bool set_both)
{
  if(set_both) {
    u->act_m = u->act_p = u->act_eq;
    u->act_dif = 0.0f;
    if(act.avg_dt > 0.0f)
      u->act_avg += act.avg_dt * (u->act_eq - u->act_avg);
  }
  else {
    if(net->phase == LeabraNetwork::MINUS_PHASE)
      u->act_m = u->act_eq;
    else if((net->phase == LeabraNetwork::PLUS_PHASE) && (net->phase_no < 2)) {
      // act_p is only for first plus phase: others require something else
      u->act_p = u->act_eq;
      if(act.avg_dt > 0.0f)
	u->act_avg += act.avg_dt * (u->act_eq - u->act_avg);
      u->act_dif = u->act_p - u->act_m;
    }
  }
}

//////////////////////////////////////////
//	Stage 6: Learning 		//
//////////////////////////////////////////

void LeabraUnitSpec::Compute_dWt(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  if((u->act_p <= opt_thresh.learn) && (u->act_m <= opt_thresh.learn))
    return;
  if(lay->phase_dif_ratio < opt_thresh.phase_dif)
    return;
  Compute_dWt_impl(u, lay, net);
}

void LeabraUnitSpec::Compute_dWt_impl(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  // don't adapt bias weights on clamped inputs..: why?  what possible consequence could it have!?
  // furthermore: it is not right for units that are clamped in 2nd plus phase!
  //  if(!((u->ext_flag & Unit::EXT) && !(u->ext_flag & Unit::TARG))) {
  ((LeabraConSpec*)bias_spec.SPtr())->B_Compute_dWt((LeabraCon*)u->bias.Cn(0), u);
    //  }
  inherited::Compute_dWt(u);
}

void LeabraUnitSpec::Compute_WtFmLin(LeabraUnit* u, LeabraLayer*, LeabraNetwork*) {
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* lay = (LeabraLayer*) recv_gp->prjn->from;
    if(lay->lesioned() || !recv_gp->cons.size)	continue;
    recv_gp->Compute_WtFmLin();
  }
}

void LeabraUnitSpec::Compute_Weights(Unit* u) {
  LeabraUnit* lu = (LeabraUnit*)u;
  // see above commment
  //  if(!((lu->ext_flag & Unit::EXT) && !(lu->ext_flag & Unit::TARG))) {
  ((LeabraConSpec*)bias_spec.SPtr())->B_Compute_Weights((LeabraCon*)u->bias.Cn(0), lu, this);
    //  }
  if(opt_thresh.updt_wts && 
     ((lu->act_p <= opt_thresh.learn) && (lu->act_m <= opt_thresh.learn)))
    return;
  inherited::Compute_Weights(lu);
}

float LeabraUnitSpec::Compute_SSE(bool& has_targ, Unit* u) {
  has_targ = false;
  LeabraUnit* lu = (LeabraUnit*)u;
  if(lu->ext_flag & Unit::TARG) {
    has_targ = true;
    float uerr = lu->targ - lu->act_m;
    if(fabsf(uerr) < sse_tol)
      return 0.0f;
    return uerr * uerr;
  }
  else
    return 0.0f;
}

//////////////////////////////////////////
//	 Misc Functions 		//
//////////////////////////////////////////

void LeabraUnitSpec::GraphVmFun(DataTable* graph_data, float g_i, float min, float max, float incr) {
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_VmFun", true);
    newguy = true;
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Vm", idx, VT_FLOAT);

  float x;
  for(x = min; x <= max; x += incr) {
    float y = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
      ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(y, -1);
  }
  graph_data->StructUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}

void LeabraUnitSpec::GraphActFmVmFun(DataTable* graph_data, float min, float max, float incr) {
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFmVmFun", true);
    newguy = true;
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Vm", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Act", idx, VT_FLOAT);

  LeabraUnit un;
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  float x;
  for(x = min; x <= max; x += incr) {
    un.v_m = x;
    Compute_ActFmVm(&un, NULL, NULL, net);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}

void LeabraUnitSpec::GraphActFmNetFun(DataTable* graph_data, float g_i, float min, float max, float incr) {
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFmNetFun", true);
    newguy = true;
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* nt = graph_data->FindMakeColName("Net", idx, VT_FLOAT);
  DataCol* vm = graph_data->FindMakeColName("Act", idx, VT_FLOAT);

  LeabraUnit un;
  LeabraNetwork* net = GET_MY_OWNER(LeabraNetwork);

  float x;
  for(x = min; x <= max; x += incr) {
    un.v_m = ((g_bar.e * x * e_rev.e) + (g_bar.i * g_i * e_rev.i) + (g_bar.l * e_rev.l)) /
      ((g_bar.e * x) + (g_bar.i * g_i) + g_bar.l);
    Compute_ActFmVm(&un, NULL, NULL, net);
    graph_data->AddBlankRow();
    nt->SetValAsFloat(x, -1);
    vm->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}

//////////////////////////
//  	Unit 		//
//////////////////////////


void LeabraUnitChans::Initialize() {
  l = i = h = a = 0.0f;
}

void LeabraUnitChans::Copy_(const LeabraUnitChans& cp) {
  l = cp.l;
  i = cp.i;
  h = cp.h;
  a = cp.a;
}

void LeabraUnit::Initialize() {
  bias.con_type = &TA_LeabraCon;

  act_eq = 0.0f;
  act_avg = 0.1f;
  act_p = act_m = act_dif = 0.0f;
  da = 0.0f;
  I_net = 0.0f;
  v_m = 0.0f;

  in_subgp = false;
  clmp_net = 0.0f;
  net_scale = 0.0f;
  bias_scale = 0.0f;
  prv_net = 0.0f;
  prv_g_i = 0.0f;

  act_sent = 0.0f;
  act_delta = 0.0f;
  net_raw = 0.0f;
  net_delta = 0.0f;
  g_i_delta = 0.0f;
  g_i_raw = 0.0f;

  i_thr = 0.0f;
  spk_amp = 2.0f;
  misc_1 = 0.0f;
}

void LeabraUnit::InitLinks() {
  inherited::InitLinks();
  taBase::Own(vcb, this);
  taBase::Own(gc, this);
  GetInSubGp();
}

void LeabraUnit::GetInSubGp() {
  Unit_Group* ownr = (Unit_Group*)owner;  
  if((ownr != NULL) && (ownr->owner != NULL) && ownr->owner->InheritsFrom(TA_taSubGroup))
    in_subgp = true;
  else
    in_subgp = false;
}

void LeabraUnit::Copy_(const LeabraUnit& cp) {
  act_eq = cp.act_eq;
  act_avg = cp.act_avg;
  act_m = cp.act_m;
  act_p = cp.act_p;
  act_dif = cp.act_dif;
  da = cp.da;
  vcb = cp.vcb;
  gc = cp.gc;
  I_net = cp.I_net;
  v_m = cp.v_m;
  // not: in_subgp
  clmp_net = cp.clmp_net;
  net_scale = cp.net_scale;
  bias_scale = cp.bias_scale;
  prv_net = cp.prv_net;
  prv_g_i = cp.prv_g_i;
  act_sent = cp.act_sent;
  act_delta = cp.act_delta;
  net_raw = cp.net_raw;
  net_delta = cp.net_delta;
  g_i_raw = cp.g_i_raw;
  g_i_delta = cp.g_i_delta;
  i_thr = cp.i_thr;
  spk_amp = cp.spk_amp;
  misc_1 = cp.misc_1;
}


//////////////////////////////////////////////////////////////////////////
//			Projection Level Code

void LeabraPrjn::Initialize() {
  netin_avg = 0.0f;
  netin_rel = 0.0f;

  avg_netin_avg = 0.0f;
  avg_netin_avg_sum = 0.0f;
  avg_netin_rel = 0.0f;
  avg_netin_rel_sum = 0.0f;
  avg_netin_n = 0;

  trg_netin_rel = -1.0f;		// indicates not set
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
  DMem_InitAggs();
#endif
}

void LeabraPrjn::Destroy() {
}

void LeabraPrjn::Copy_(const LeabraPrjn& cp) {
  netin_avg = cp.netin_avg;
  netin_rel = cp.netin_rel;

  avg_netin_avg = cp.avg_netin_avg;
  avg_netin_avg_sum = cp.avg_netin_avg_sum;
  avg_netin_rel = cp.avg_netin_rel;
  avg_netin_rel_sum = cp.avg_netin_rel_sum;
  avg_netin_n = cp.avg_netin_n;

  trg_netin_rel = cp.trg_netin_rel;
}

void LeabraPrjn::Init_Stats() {
  netin_avg = 0.0f;
  netin_rel = 0.0f;

  avg_netin_avg = 0.0f;
  avg_netin_avg_sum = 0.0f;
  avg_netin_rel = 0.0f;
  avg_netin_rel_sum = 0.0f;
  avg_netin_n = 0;
}

#ifdef DMEM_COMPILE
void LeabraPrjn::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraPrjn::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif

//////////////////////////////////////////////////////////////////////////
//			Layer Level Code

//////////////////////////
//  	Layer, Spec	//
//////////////////////////

void KWTASpec::Initialize() {
  k_from = USE_PCT;
  k = 12;
  pct = .25f;
  pat_q = .5f;
  diff_act_pct = false;
  act_pct = .1f;
  gp_i = false;
  gp_g = .5f;
}

void AdaptISpec::Initialize() {
  type = NONE;
  tol = .02f;			// allow to be off by this amount before taking action
  p_dt = .1f;			// take reasonably quick action..
  mx_d = .9f;			// move this far in either direction
  l = .2f;			// proportion to assign to leak..
  a_dt = .005f;			// time averaging
}

void KwtaTieBreak::Initialize() {
  on = false;
  k_thr = 1.0f;
  diff_thr = 0.2f;
}

void ClampSpec::Initialize() {
  hard = true;
  gain = .5f;
  d_gain = 0.0f;
}

void DecaySpec::Initialize() {
  event = 1.0f;
  phase = 1.0f;
  phase2 = 0.0f;
  clamp_phase2 = false;
}

void LayNetRescaleSpec::Initialize() {
  on = false;
  max_net = 0.6f;
  net_extra = 0.2f;
}

void LayAbsNetAdaptSpec::Initialize() {
  on = false;
  trg_net = .5f;
  tol = .1f;
  abs_lrate = .1f;
}

void LeabraLayerSpec::Initialize() {
  min_obj_type = &TA_LeabraLayer;
  inhib_group = ENTIRE_LAYER;
  compute_i = KWTA_INHIB;
  i_kwta_pt = .25f;
  gp_i_pt = .2f;
}

void LeabraLayerSpec::Defaults() {
  adapt_i.Defaults();
  clamp.Defaults();
  decay.Defaults();
  net_rescale.Defaults();
  Initialize();
}

void LeabraLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void LeabraLayerSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_LeabraLayerSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(kwta, this);
  taBase::Own(gp_kwta, this);
  taBase::Own(tie_brk, this);
  taBase::Own(adapt_i, this);
  taBase::Own(clamp, this);
  taBase::Own(decay, this);
  taBase::Own(net_rescale, this);
  taBase::Own(abs_net_adapt, this);
}

bool LeabraLayerSpec::CheckConfig_Layer(LeabraLayer* lay, bool quiet) {
  bool rval = true;
  if(CheckError(!lay->projections.el_base->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
		"does not have LeabraPrjn projection base type!",
		"project must be updated and projections remade"))
    return false;
  for(int i=0;i<lay->projections.size;i++) {
    Projection* prjn = (Projection*)lay->projections[i];
    CheckError(!prjn->InheritsFrom(&TA_LeabraPrjn), quiet, rval,
	       "does not have LeabraPrjn projection base type!",
	       "Projection must be re-made");
  }
  return rval;
}

void LeabraLayerSpec::HelpConfig() {
  String help = "LeabraLayerSpec Configuration:\n\
The layer spec sets the target level of activity, k, for each layer.  \
Therefore, you must have a different layer spec with an appropriate activity \
level for layers that have different activity levels.  Note that if you set \
the activity level by percent this will work for different sized layers that \
have the same percent activity level.";
  cerr << help << endl << flush;
  taMisc::Confirm(help);
}

void LeabraLayerSpec::Init_Weights(LeabraLayer* lay) {
  Compute_Active_K(lay);	// need kwta.pct for init
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_Weights();
  if(lay->units.gp.size > 0) {
    int gi;
    for(gi=0;gi<lay->units.gp.size;gi++) {
      LeabraUnit_Group* gp = (LeabraUnit_Group*)lay->units.gp[gi];
      gp->misc_state = gp->misc_state1 = gp->misc_state2 = 0;
    }
  }
  Init_Inhib(lay);		// initialize inhibition at start..
  Init_Stats(lay);
}

void LeabraLayerSpec::Init_Stats(LeabraLayer* lay) {
  lay->avg_netin.avg = 0.0f;
  lay->avg_netin.max = 0.0f;

  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;
  lay->avg_netin_n = 0;

  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    prjn->Init_Stats();
  }
}

void LeabraLayerSpec::Init_ActAvg(LeabraLayer* lay) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_ActAvg();
}

void LeabraLayerSpec::SetCurLrate(LeabraLayer* lay, LeabraNetwork* net, int epoch) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->SetCurLrate(net, epoch);
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpec(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  Projection* p;
  taLeafItr pi;
  FOR_ITR_EL(Projection, p, lay->projections., pi) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from;
    if(fmlay->spec.SPtr()->InheritsFrom(layer_spec)) {	// inherits - not excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecExact(LeabraLayer* lay, int& prjn_idx, TypeDef* layer_spec) {
  LeabraLayer* rval = NULL;
  prjn_idx = -1;
  Projection* p;
  taLeafItr pi;
  FOR_ITR_EL(Projection, p, lay->projections., pi) {
    LeabraLayer* fmlay = (LeabraLayer*)p->from;
    if(fmlay->spec.SPtr()->GetTypeDef() == layer_spec) {	// not inherits - excact match!
      prjn_idx = p->recv_idx;
      rval = fmlay;
      break;
    }
  }
  return rval;
}

LeabraLayer* LeabraLayerSpec::FindLayerFmSpecNet(Network* net, TypeDef* layer_spec) {
  LeabraLayer* lay;
  taLeafItr li;
  FOR_ITR_EL(LeabraLayer, lay, net->layers., li) {
    if(lay->spec.SPtr()->InheritsFrom(layer_spec)) {	// inherits - not excact match!
      return lay;
    }
  }
  return NULL;
}

//////////////////////////////////////////
//	Stage 0: at start of settling	// 
//////////////////////////////////////////

void LeabraLayerSpec::Compute_Active_K(LeabraLayer* lay) {
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int totk = 0;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_Active_K_impl(lay, rugp, (LeabraInhib*)rugp, gp_kwta);
      totk += rugp->kwta.k;
    }
    if(inhib_group == UNIT_GROUPS) {
      if(lay->kwta.k != totk)
	lay->Inhib_ResetSortBuf();
      lay->kwta.k = totk;
      lay->kwta.Compute_Pct(lay->units.leaves);
      if(gp_kwta.diff_act_pct)
	lay->kwta.pct = gp_kwta.act_pct;	// override!!
    }
  }
  if(inhib_group != UNIT_GROUPS) {
    Compute_Active_K_impl(lay, &(lay->units), (LeabraInhib*)lay, kwta);
    if(kwta.diff_act_pct)
      lay->kwta.pct = kwta.act_pct;	// override!!
  }
}

void LeabraLayerSpec::Compute_Active_K_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					    KWTASpec& kwtspec)
{
  int new_k = 0;
  if(kwtspec.k_from == KWTASpec::USE_PCT)
    new_k = (int)(kwtspec.pct * (float)ug->leaves);
  else if((kwtspec.k_from == KWTASpec::USE_PAT_K) && 
	  ((lay->ext_flag & (Unit::TARG | Unit::COMP)) || (lay->ext_flag & Unit::EXT)))
    new_k = Compute_Pat_K(lay, ug, thr);
  else
    new_k = kwtspec.k;

  if(compute_i == KWTA_INHIB)
    new_k = MIN(ug->leaves - 1, new_k);
  else
    new_k = MIN(ug->leaves, new_k);
  new_k = MAX(1, new_k);

  if(thr->kwta.k != new_k)
    thr->Inhib_ResetSortBuf();

  thr->kwta.k = new_k;
  thr->kwta.Compute_Pct(ug->leaves);
}

int LeabraLayerSpec::Compute_Pat_K(LeabraLayer* lay, Unit_Group* ug, LeabraInhib*) {
  bool use_comp = false;
  if(lay->ext_flag & Unit::COMP) // only use comparison vals if entire lay is COMP!
    use_comp = true;
  int pat_k = 0;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    // use either EXT or TARG information...
    if(u->ext_flag & Unit::EXT) {
      if(u->ext >= kwta.pat_q)
	pat_k++;
    }
    else if(u->ext_flag & Unit::TARG) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }
    else if(use_comp && (u->ext_flag | Unit::COMP)) {
      if(u->targ >= kwta.pat_q)
	pat_k++;
    }	      
  }
  return pat_k;
}

void LeabraLayerSpec::Init_Acts(LeabraLayer* lay) {
  lay->ext_flag = Unit::NO_EXTERNAL;
  lay->stm_gain = clamp.gain;
  lay->hard_clamped = false;
  lay->net_rescale = 1.0f;
  lay->ResetSortBuf();
  Compute_Active_K(lay);	// need kwta.pct for init
  lay->Inhib_Init_Acts(this);
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->Inhib_Init_Acts(this);
    }
  }
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_Acts(lay);
}

void LeabraLayerSpec::Compute_HardClampPhase2(LeabraLayer* lay, LeabraNetwork* net) {
  // special case to speed up processing by clamping 2nd plus phase to prior + phase
  lay->SetExtFlag(Unit::EXT);
  lay->hard_clamped = true;
  lay->Inhib_SetVals(i_kwta_pt);		// assume 0 - 1 clamped inputs

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->SetExtFlag(Unit::EXT);
    u->ext = u->act_p;
    u->Compute_HardClampNoClip(lay, net); // important: uses no clip here: not really external values!
  }
  Compute_ActAvg(lay, net);
}

void LeabraLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(decay.clamp_phase2 && (net->phase != LeabraNetwork::MINUS_PHASE) && (net->phase_no >= 2)) {
    Compute_HardClampPhase2(lay, net);
    return;
  }
  if(!(clamp.hard && (lay->ext_flag & Unit::EXT))) {
    lay->hard_clamped = false;
    return;
  }
  lay->hard_clamped = true;	// cache this flag
  lay->Inhib_SetVals(i_kwta_pt);		// assume 0 - 1 clamped inputs

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_HardClamp(lay, net);
  }
  Compute_ActAvg(lay, net);
  Compute_OutputName(lay, net);
}

void LeabraLayerSpec::Compute_NetinScale(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_NetinScale(lay, net);
  }
}

void LeabraLayerSpec::Init_ClampNet(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Init_ClampNet(lay, net);
}

void LeabraLayerSpec::Send_ClampNet(LeabraLayer* lay, LeabraNetwork* net) {
  if(!lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_ClampNet(lay, net);
}

//////////////////////////////////
//	Stage 1: netinput 	//
//////////////////////////////////

void LeabraLayerSpec::Send_Netin(LeabraLayer* lay) {
  // hard-clamped input layers are already computed in the clmp_net value
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_Netin(lay);
}

void LeabraLayerSpec::Send_NetinDelta(LeabraLayer* lay) {
  // hard-clamped input layers are already computed in the clmp_net value
  if(lay->hard_clamped) return;

  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Send_NetinDelta(lay);
}

//////////////////////////////////////////////////////////////////
//	Stage 2: netinput averages and clamping (if necc)	//
//////////////////////////////////////////////////////////////////

void LeabraLayerSpec::Compute_Clamp_NetAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped) return;

  if(lay->ext_flag & Unit::EXT)
    Compute_SoftClamp(lay, net);

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    lay->netin.avg = 0.0f; lay->netin.max = -FLT_MAX; lay->netin.max_i = -1;
    lay->i_thrs.avg = 0.0f; lay->i_thrs.max = -FLT_MAX; lay->i_thrs.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_NetinAvg(lay, rugp, (LeabraInhib*)rugp, net);
      // keep maximums for linking purposes
      lay->netin.avg = MAX(lay->netin.avg, rugp->netin.avg);
      if(rugp->netin.max > lay->netin.max) {
	lay->netin.max = rugp->netin.max; lay->netin.max_i = g;
      }
      lay->i_thrs.avg = MAX(lay->i_thrs.avg, rugp->i_thrs.avg);
      if(rugp->i_thrs.max > lay->i_thrs.max) {
	lay->i_thrs.max = rugp->i_thrs.max; lay->i_thrs.max_i = g;
      }
    }
  }
  else {
    Compute_NetinAvg(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_NetinAvg(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  thr->netin.avg = 0.0f; thr->netin.max = -FLT_MAX; thr->netin.max_i = -1;
  thr->i_thrs.avg = 0.0f; thr->i_thrs.max = -FLT_MAX; thr->i_thrs.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_NetinAvg(lay, thr, net);
    thr->netin.avg += u->net;
    if(u->net > thr->netin.max) {
      thr->netin.max = u->net;  thr->netin.max_i = lf;
    }
    thr->i_thrs.avg += u->i_thr;
    if(u->i_thr > thr->i_thrs.max) {
      thr->i_thrs.max = u->i_thr;  thr->i_thrs.max_i = lf;
    }
    lf++;
  }

  if(ug->leaves > 0) {
    thr->netin.avg /= (float)ug->leaves;	// turn it into an average
    thr->i_thrs.avg /= (float)ug->leaves;	// turn it into an average
  }
}

void LeabraLayerSpec::Compute_SoftClamp(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  bool inc_gain = false;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    if(!inc_gain)
      inc_gain = u->Compute_SoftClamp(lay, net);
    else
      u->Compute_SoftClamp(lay, net);
  }
  if(inc_gain && (net->cycle > 1)) // only increment after it has a chance to activate
    lay->stm_gain += clamp.d_gain;
}


//////////////////////////////////////////
//	Stage 3: inhibition		//
//////////////////////////////////////////

void LeabraSort::FastInsertLink(void* it, int where) {
  if((where >= size) || (where < 0)) {
    AddOnly_(it);
    return;
  }
  if(size > 0)
    AddOnly_(NULL);
  int i;
  for(i=size-1; i>where; i--)
    el[i] = el[i-1];
  el[where] = it;
}

int LeabraSort::FindNewNetPos(float nw_net) {
  int st = 0;
  int n = size;
  while(true) {
    if(n <= 2) {
      if(FastEl(st)->i_thr < nw_net)
	return st;
      else if((st+1 < size) && (FastEl(st+1)->i_thr < nw_net))
	return st + 1;
      else if((st+2 < size) && (FastEl(st+2)->i_thr < nw_net))
	return st + 2;
      return st;
    }
    int n2 = n / 2;
    if(FastEl(st + n2)->i_thr < nw_net)
      n = n2;			// search in upper 1/2 of list
    else {
      st += n2; n -= n2;	// search in lower 1/2 of list
    }
  }
}

void LeabraLayerSpec::Init_Inhib(LeabraLayer* lay) {
  lay->adapt_i.avg_avg = lay->kwta.pct;
  lay->adapt_i.i_kwta_pt = i_kwta_pt;
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
  lay->adapt_i.g_bar_i = us->g_bar.i;
  lay->adapt_i.g_bar_l = us->g_bar.l;
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->adapt_i.avg_avg = rugp->kwta.pct;
      if(((compute_i == AVG_MAX_PT_INHIB) || (compute_i == MAX_INHIB))
	 && (inhib_group != ENTIRE_LAYER))
	rugp->adapt_i.i_kwta_pt = gp_i_pt;
      else
	rugp->adapt_i.i_kwta_pt = i_kwta_pt;
      rugp->adapt_i.g_bar_i = us->g_bar.i;
      rugp->adapt_i.g_bar_l = us->g_bar.l;
    }
  }
}

void LeabraLayerSpec::Compute_Inhib(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->hard_clamped)	return;	// say no more..

  if(inhib_group != UNIT_GROUPS) {
    Compute_Inhib_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
  if(lay->units.gp.size > 0) {
    if(inhib_group == UNIT_GROUPS) {
      lay->Inhib_SetVals(0.0f);
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	Compute_Inhib_impl(lay, rugp, (LeabraInhib*)rugp, net);
	float gp_g_i = rugp->i_val.g_i;
	if(gp_kwta.gp_i)
	  gp_g_i *= gp_kwta.gp_g;
	lay->i_val.g_i = MAX(lay->i_val.g_i, gp_g_i);
      }
    }
    else if(inhib_group == LAY_AND_GPS) {
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	Compute_Inhib_impl(lay, rugp, (LeabraInhib*)rugp, net);
      }
    }
    Compute_LayInhibToGps(lay, net);
  }
}

void LeabraLayerSpec::Compute_LayInhibToGps(LeabraLayer* lay, LeabraNetwork*) {
  if(lay->units.gp.size == 0) return;

  if(inhib_group == ENTIRE_LAYER) {
    // propagate g_i to all subgroups even if doing ENTIRE_LAYER
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i = lay->i_val.g_i;
    }
  }
  else if(inhib_group == UNIT_GROUPS) {
    if(gp_kwta.gp_i) {	// linking groups: get max from layer
      for(int g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	rugp->i_val.gp_g_i = lay->i_val.g_i;
	rugp->i_val.g_i = MAX(rugp->i_val.g_i, lay->i_val.g_i);
      }
    }
  }
  else if(inhib_group == LAY_AND_GPS) {
    // actual inhibition is max of layer and group
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      rugp->i_val.g_i = MAX(rugp->i_val.g_i, lay->i_val.g_i);
    }
  }
}

void LeabraLayerSpec::Compute_Inhib_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net) {
  if(compute_i == UNIT_INHIB) {
    thr->i_val.g_i = 0.0f;	// make sure it's zero, cuz this gets added to units.. 
  }
  else {
    if(compute_i == KWTA_INHIB)
      Compute_Inhib_kWTA(lay, ug, thr, net);
    else if(compute_i == KWTA_AVG_INHIB)
      Compute_Inhib_kWTA_Avg(lay, ug, thr, net);
    else if(compute_i == AVG_MAX_PT_INHIB)
      Compute_Inhib_AvgMaxPt(lay, ug, thr, net);
    else if(compute_i == MAX_INHIB)
      Compute_Inhib_Max(lay, ug, thr, net);
    thr->i_val.g_i = thr->i_val.kwta;
  }

  thr->i_val.g_i_orig = thr->i_val.g_i;	// retain original values..
}

void LeabraLayerSpec::Compute_Inhib_kWTA(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(i_kwta_pt);
    return;
  }

  int k_plus_1 = thr->kwta.k + 1;	// expand cutoff to include N+1th one
  k_plus_1 = MIN(ug->leaves,k_plus_1);

  float net_k1 = FLT_MAX;
  int k1_idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  int j;
  if(thr->active_buf.size != k_plus_1) { // need to fill the sort buf..
    thr->active_buf.size = 0;
    j = 0;
    for(u = (LeabraUnit*)ug->FirstEl(i); u && (j < k_plus_1);
	u = (LeabraUnit*)ug->NextEl(i), j++)
    {
      thr->active_buf.Add(u);		// add unit to the list
      if(u->i_thr < net_k1) {
	net_k1 = u->i_thr;	k1_idx = j;
      }
    }
    thr->inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; u; u = (LeabraUnit*)ug->NextEl(i)) {
      if(u->i_thr <=  net_k1) {	// not bigger than smallest one in sort buffer
	thr->inact_buf.Add(u);
	continue;
      }
      thr->inact_buf.Add(thr->active_buf[k1_idx]); // now inactive
      thr->active_buf.ReplaceIdx(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      for(j=0; j < k_plus_1; j++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[j]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find net_k1
    for(j=0; j < k_plus_1; j++) { 	// these should be the top ones, very fast!!
      float tmp = thr->active_buf[j]->i_thr;
      if(tmp < net_k1) {
	net_k1 = tmp;		k1_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < thr->inact_buf.size; j++) {
      u = thr->inact_buf[j];
      if(u->i_thr <=  net_k1)		// not bigger than smallest one in sort buffer
	continue;
      thr->inact_buf.ReplaceIdx(j, thr->active_buf[k1_idx]);	// now inactive
      thr->active_buf.ReplaceIdx(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      int i;
      for(i=0; i < k_plus_1; i++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[i]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = i;
	}
      }
    }
  }

  // active_buf now has k+1 most active units, get the next-highest one
  int k_idx = -1;
  float net_k = FLT_MAX;
  for(j=0; j < k_plus_1; j++) {
    float tmp = thr->active_buf[j]->i_thr;
    if((tmp < net_k) && (j != k1_idx)) {
      net_k = tmp;		k_idx = j;
    }
  }
  if(k_idx == -1) {		// we didn't find the next one
    k_idx = k1_idx;
    net_k = net_k1;
  }

  LeabraUnit* k1_u = (LeabraUnit*)thr->active_buf[k1_idx];
  LeabraUnit* k_u = (LeabraUnit*)thr->active_buf[k_idx];

  float k1_i = k1_u->i_thr;
  float k_i = k_u->i_thr;
  thr->kwta.k_ithr = k_i;
  thr->kwta.k1_ithr = k1_i;

  Compute_Inhib_BreakTie(thr);

  // place kwta inhibition between k and k+1
  float nw_gi = thr->kwta.k1_ithr + i_kwta_pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, 0.0f);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Avg(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  if(ug->leaves <= 1) {	// this is undefined
    thr->Inhib_SetVals(i_kwta_pt);
    return;
  }

  int k_plus_1 = thr->kwta.k;	// keep cutoff at k

  float net_k1 = FLT_MAX;
  int k1_idx = 0;
  LeabraUnit* u;
  taLeafItr i;
  int j;
  if(thr->active_buf.size != k_plus_1) { // need to fill the sort buf..
    thr->active_buf.size = 0;
    j = 0;
    for(u = (LeabraUnit*)ug->FirstEl(i); u && (j < k_plus_1);
	u = (LeabraUnit*)ug->NextEl(i), j++)
    {
      thr->active_buf.Add(u);		// add unit to the list
      if(u->i_thr < net_k1) {
	net_k1 = u->i_thr;	k1_idx = j;
      }
    }
    thr->inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; u; u = (LeabraUnit*)ug->NextEl(i)) {
      if(u->i_thr <=  net_k1) {	// not bigger than smallest one in sort buffer
	thr->inact_buf.Add(u);
	continue;
      }
      thr->inact_buf.Add(thr->active_buf[k1_idx]); // now inactive
      thr->active_buf.ReplaceIdx(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      for(j=0; j < k_plus_1; j++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[j]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find net_k1
    for(j=0; j < k_plus_1; j++) { 	// these should be the top ones, very fast!!
      float tmp = thr->active_buf[j]->i_thr;
      if(tmp < net_k1) {
	net_k1 = tmp;		k1_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < thr->inact_buf.size; j++) {
      u = thr->inact_buf[j];
      if(u->i_thr <=  net_k1)		// not bigger than smallest one in sort buffer
	continue;
      thr->inact_buf.ReplaceIdx(j, thr->active_buf[k1_idx]);	// now inactive
      thr->active_buf.ReplaceIdx(k1_idx, u);// replace the smallest with it
      net_k1 = u->i_thr;		// assume its the smallest
      int i;
      for(i=0; i < k_plus_1; i++) { 	// and recompute the actual smallest
	float tmp = thr->active_buf[i]->i_thr;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = i;
	}
      }
    }
  }

  // active_buf now has k most active units, get averages of both groups
  float k_avg = 0.0f;
  for(j=0; j < k_plus_1; j++)
    k_avg += thr->active_buf[j]->i_thr;
  k_avg /= (float)k_plus_1;

  float oth_avg = 0.0f;
  for(j=0; j < thr->inact_buf.size; j++)
    oth_avg += thr->inact_buf[j]->i_thr;
  if(thr->inact_buf.size > 0)
    oth_avg /= (float)thr->inact_buf.size;

  // place kwta inhibition between two averages
  // this uses the adapting point!
  float pt = i_kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;

  Compute_Inhib_BreakTie(thr);

  float nw_gi = thr->kwta.k1_ithr + pt * (thr->kwta.k_ithr - thr->kwta.k1_ithr);
  nw_gi = MAX(nw_gi, 0.0f);
  thr->i_val.kwta = nw_gi;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_BreakTie(LeabraInhib* thr) {
  thr->kwta.ithr_diff = (thr->kwta.k_ithr - thr->kwta.k1_ithr) / thr->kwta.k_ithr;
  thr->kwta.tie_brk = 0;
  if(tie_brk.on && (thr->kwta.k_ithr > tie_brk.k_thr)) {
    if(thr->kwta.ithr_diff < tie_brk.diff_thr) {
      // we now have an official tie: break it by reducing firing of "others"
      thr->kwta.k1_ithr = (1.0f - tie_brk.diff_thr) * thr->kwta.k_ithr;
      thr->kwta.tie_brk = 1;
    }
  }
}

void LeabraLayerSpec::Compute_Inhib_kWTA_Gps(LeabraLayer* lay, LeabraNetwork* net) {
  // computing the top *groups*, not units here!
  int k_plus_1 = lay->kwta.k;	// only get top k

  float net_k1 = FLT_MAX;
  int k1_idx = 0;
  LeabraUnit_Group* u;
  int i;
  int j;
  if(lay->active_buf.size != k_plus_1) { // need to fill the sort buf..
    lay->active_buf.size = 0;
    for(i = 0; i < k_plus_1; i++) {
      u = (LeabraUnit_Group*)lay->units.gp[i], 
      lay->active_buf.Add((LeabraUnit*)u);		// add unit to the list
      if(u->i_val.g_i < net_k1) {
	net_k1 = u->i_val.g_i;	k1_idx = i;
      }
    }
    lay->inact_buf.size = 0;
    // now, use the "replace-the-lowest" sorting technique
    for(; i<lay->units.gp.size; i++) {
      u = (LeabraUnit_Group*)lay->units.gp[i];
      if(u->i_val.g_i <=  net_k1) {	// not bigger than smallest one in sort buffer
	lay->inact_buf.Add((LeabraUnit*)u);
	continue;
      }
      lay->inact_buf.Add(lay->active_buf[k1_idx]); // now inactive
      lay->active_buf.ReplaceIdx(k1_idx, (LeabraUnit*)u);// replace the smallest with it
      net_k1 = u->i_val.g_i;		// assume its the smallest
      for(j=0; j < k_plus_1; j++) { 	// and recompute the actual smallest
	float tmp = ((LeabraUnit_Group*)lay->active_buf[j])->i_val.g_i;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = j;
	}
      }
    }
  }
  else {				// keep the ones around from last time, find net_k1
    for(j=0; j < k_plus_1; j++) { 	// these should be the top ones, very fast!!
      float tmp = ((LeabraUnit_Group*)lay->active_buf[j])->i_val.g_i;
      if(tmp < net_k1) {
	net_k1 = tmp;		k1_idx = j;
      }
    }
    // now, use the "replace-the-lowest" sorting technique (on the inact_list)
    for(j=0; j < lay->inact_buf.size; j++) {
      u = (LeabraUnit_Group*)lay->inact_buf[j];
      if(u->i_val.g_i <=  net_k1)		// not bigger than smallest one in sort buffer
	continue;
      lay->inact_buf.ReplaceIdx(j, lay->active_buf[k1_idx]);	// now inactive
      lay->active_buf.ReplaceIdx(k1_idx, (LeabraUnit*)u);// replace the smallest with it
      net_k1 = u->i_val.g_i;		// assume its the smallest
      int i;
      for(i=0; i < k_plus_1; i++) { 	// and recompute the actual smallest
	float tmp = ((LeabraUnit_Group*)lay->active_buf[i])->i_val.g_i;
	if(tmp < net_k1) {
	  net_k1 = tmp;		k1_idx = i;
	}
      }
    }
  }

  // active_buf now has k most active groups - go through groups and set large inhib in 
  // inactive ones!
  for(j=0; j < lay->inact_buf.size; j++) {
    u = (LeabraUnit_Group*)lay->inact_buf[j];
    // set inhib to more than what is needed to keep *max* unit below threshold..
    LeabraUnit* un = (LeabraUnit*)u->Leaf(0);// get first unit 
    float tnet = un->net;
    un->net = u->netin.max;	// set to max
    u->i_val.g_i = 1.2f * un->Compute_IThresh(lay, net);
    un->net = tnet;
  }
}

void LeabraLayerSpec::Compute_Inhib_AvgMaxPt(LeabraLayer* lay, Unit_Group*, LeabraInhib* thr, LeabraNetwork*) {
  float pt = i_kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  else if((inhib_group != ENTIRE_LAYER) && ((LeabraInhib*)lay != thr))
    pt = gp_i_pt;		// use sub-group version for sub-groups..
  
  float oth_avg = thr->i_thrs.avg; // put between the avg
  float k_avg = thr->i_thrs.max; // and the max..

  float nw_gi = oth_avg + pt * (k_avg - oth_avg);
  nw_gi = MAX(nw_gi, 0.0f);
  thr->i_val.kwta = nw_gi;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = oth_avg;
  thr->kwta.Compute_IThrR();
}

void LeabraLayerSpec::Compute_Inhib_Max(LeabraLayer* lay, Unit_Group*, LeabraInhib* thr, LeabraNetwork*) {
  float pt = i_kwta_pt;
  if(adapt_i.type == AdaptISpec::KWTA_PT)
    pt = thr->adapt_i.i_kwta_pt;
  else if((inhib_group != ENTIRE_LAYER) && ((LeabraInhib*)lay != thr))
    pt = gp_i_pt;		// use sub-group version for sub-groups..
  
  float k_avg = thr->i_thrs.max; // and the max..
  float nw_gi = k_avg - pt;
  nw_gi = MAX(nw_gi, 0.0f);
  thr->i_val.kwta = nw_gi;
  thr->kwta.k_ithr = k_avg;
  thr->kwta.k1_ithr = nw_gi;
  thr->kwta.Compute_IThrR();
}


//////////////////////////////////////////
//	Stage 3.5: Inhib Avg	 	//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_InhibAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  if(lay->units.gp.size > 0) {	// even if ENTIRE_LAYER, do it by sub-group to get stats..
    lay->un_g_i.avg = 0.0f;
    lay->un_g_i.max = -FLT_MAX;
    lay->un_g_i.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_InhibAvg_impl(lay, rugp, (LeabraInhib*)rugp, net);
      lay->un_g_i.avg += rugp->un_g_i.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->un_g_i.max > lay->un_g_i.max) {
	lay->un_g_i.max = rugp->un_g_i.max;
	lay->un_g_i.max_i = g;
      }
    }
  }
  else {
    Compute_InhibAvg_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_InhibAvg_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  // now actually go through and set the gc.i for each unit
  thr->un_g_i.avg = 0.0f;
  thr->un_g_i.max = -FLT_MAX;
  thr->un_g_i.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_InhibAvg(lay, thr, net);
    thr->un_g_i.avg += u->gc.i;
    if(u->gc.i > thr->un_g_i.max) {
      thr->un_g_i.max = u->gc.i;
      thr->un_g_i.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0)
    thr->un_g_i.avg /= (float)ug->leaves;	// turn it into an average
}

//////////////////////////////////////////
//	Stage 4: the final activation 	//
//////////////////////////////////////////

void LeabraLayerSpec::Compute_ActAvg_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					 LeabraNetwork* net) {
  thr->acts.avg = 0.0f;
  thr->acts.max = -FLT_MAX;
  thr->acts.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts.avg += u->act_eq;
    if(u->act_eq > thr->acts.max) {
      thr->acts.max = u->act_eq;  thr->acts.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts.avg = 0.0f;
    lay->acts.max = -FLT_MAX;
    lay->acts.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts.avg += rugp->acts.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts.max > lay->acts.max) {
	lay->acts.max = rugp->acts.max; lay->acts.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_MaxDa_ugp(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr,
					 LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_MaxDa(lay, thr, net);
  }
}

void LeabraLayerSpec::Compute_MaxDa(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_MaxDa_ugp(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_MaxDa_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ActMAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr,
					  LeabraNetwork*) {
  thr->acts_m.avg = 0.0f;
  thr->acts_m.max = -FLT_MAX;
  thr->acts_m.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts_m.avg += u->act_m;
    if(u->act_m > thr->acts_m.max) {
      thr->acts_m.max = u->act_m;  thr->acts_m.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_m.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActMAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts_m.avg = 0.0f;
    lay->acts_m.max = -FLT_MAX;
    lay->acts_m.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActMAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts_m.avg += rugp->acts_m.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts_m.max > lay->acts_m.max) {
	lay->acts_m.max = rugp->acts_m.max; lay->acts_m.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts_m.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActMAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_ActPAvg_ugp(LeabraLayer*, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork*) {
  thr->acts_p.avg = 0.0f;
  thr->acts_p.max = -FLT_MAX;
  thr->acts_p.max_i = -1;
  LeabraUnit* u;
  taLeafItr i;
  int lf = 0;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    thr->acts_p.avg += u->act_p;
    if(u->act_p > thr->acts_p.max) {
      thr->acts_p.max = u->act_p;  thr->acts_p.max_i = lf;
    }
    lf++;
  }
  if(ug->leaves > 0) thr->acts_p.avg /= (float)ug->leaves;
}

void LeabraLayerSpec::Compute_ActPAvg(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    lay->acts_p.avg = 0.0f;
    lay->acts_p.max = -FLT_MAX;
    lay->acts_p.max_i = -1;
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_ActPAvg_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      lay->acts_p.avg += rugp->acts_p.avg * (float)rugp->leaves; // weight by no of units
      if(rugp->acts_p.max > lay->acts_p.max) {
	lay->acts_p.max = rugp->acts_p.max; lay->acts_p.max_i = g;
      }
    }
    if(lay->units.leaves > 0) lay->acts_p.avg /= (float)lay->units.leaves;
  }
  else {
    Compute_ActPAvg_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

void LeabraLayerSpec::Compute_Act(LeabraLayer* lay, LeabraNetwork* net) {
  if((net->cycle >= 0) && lay->hard_clamped)
    return;			// don't do this during normal processing

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_Act_impl(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_Act_impl(lay, &(lay->units), (LeabraInhib*)lay, net);
  }

  Compute_ActAvg(lay, net);
  Compute_MaxDa(lay, net);
  Compute_NetinRescale(lay, net);
  if(lay->ext_flag & Unit::TARG) {
    net->trg_max_act = MAX(net->trg_max_act, lay->acts.max);
  }
  Compute_OutputName(lay, net);
  if(lay->Iconified()) {
    lay->icon_value = lay->acts.avg;
  }
}

void LeabraLayerSpec::Compute_Act_impl(LeabraLayer* lay, Unit_Group* ug, LeabraInhib* thr, LeabraNetwork* net)
{
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, ug->, i) {
    u->Compute_Act(lay,thr,net);
  }
}

void LeabraLayerSpec::Compute_NetinRescale(LeabraLayer* lay, LeabraNetwork* net) {
  if(!net_rescale.on) return;
  if(lay->netin.max <= net_rescale.max_net) return;
  float old_scale = lay->net_rescale;
  lay->net_rescale *= net_rescale.max_net / (lay->netin.max + net_rescale.net_extra);
  float new_scale = lay->net_rescale / old_scale;
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->Compute_NetinRescale(lay, net, new_scale);
  }
}

void LeabraLayerSpec::Compute_OutputName_ugp(LeabraLayer* lay, Unit_Group* ug,
					     LeabraInhib* thr, LeabraNetwork* net) {
  String* onm;
  if(lay->unit_groups)
    onm = &(ug->output_name);
  else
    onm = &(lay->output_name);

  if(thr->acts.max_i < 0) {
    *onm = "n/a";
    return;
  }
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(thr->acts.max_i);
  if(!u) {
    *onm = "n/a";
    return;
  }
  *onm = u->name;	// if it is something..
  // for target/output layers, if we set something, set network name!
  if(u->name.empty() || 
     ((lay->layer_type != Layer::OUTPUT) && (lay->layer_type != Layer::TARGET))) return;
  if(!net->output_name.empty())
    net->output_name += "_";
  net->output_name += u->name;
}

void LeabraLayerSpec::Compute_OutputName(LeabraLayer* lay, LeabraNetwork* net) {
  if(lay->units.gp.size > 0) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      Compute_OutputName_ugp(lay, rugp, (LeabraInhib*)rugp, net);
    }
  }
  else {
    Compute_OutputName_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
  }
}

//////////////////////////////////////////
//	Stage 5: Between Events 	//
//////////////////////////////////////////

void LeabraLayerSpec::PhaseInit(LeabraLayer* lay, LeabraNetwork* net) {
  lay->net_rescale = 1.0f;
  if(lay->ext_flag & Unit::TARG) {	// only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE)
      lay->SetExtFlag(Unit::EXT);
  }
  else {
    if(clamp.hard) return;	// not target and hard-clamped -- bail
  }
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->PhaseInit(lay, net);
  }
}

void LeabraLayerSpec::DecayEvent(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayEvent(lay, net, decay.event);
  }
}
  
void LeabraLayerSpec::DecayPhase(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayPhase(lay, net, decay.phase);
  }
}

void LeabraLayerSpec::DecayPhase2(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i) {
    u->DecayPhase(lay, net, decay.phase2);
  }
}
  
void LeabraLayerSpec::ExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(lay->ext_flag & Unit::EXT))	// only process ext
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->ExtToComp(lay, net);
}

void LeabraLayerSpec::TargExtToComp(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(lay->ext_flag & Unit::TARG_EXT))	// only process w/ external input
    return;
  lay->ext_flag = Unit::COMP;	// totally reset to comparison
    
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->TargExtToComp(lay, net);
}

void LeabraLayerSpec::AdaptGBarI(LeabraLayer* lay, LeabraNetwork*) {
  float diff = lay->kwta.pct - lay->acts.avg;
  if(fabsf(diff) > adapt_i.tol) {
    float p_i = 1.0f;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      p_i = 1.0f - adapt_i.l;
    }
    lay->adapt_i.g_bar_i -= p_i * adapt_i.p_dt * diff;
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    float minv = us->g_bar.i * (1.0 - adapt_i.mx_d);
    float maxv = us->g_bar.i * (1.0 + adapt_i.mx_d);
    if(lay->adapt_i.g_bar_i < minv) lay->adapt_i.g_bar_i = minv;
    if(lay->adapt_i.g_bar_i > maxv) lay->adapt_i.g_bar_i = maxv;
    if(adapt_i.type == AdaptISpec::G_BAR_IL) {
      lay->adapt_i.g_bar_l -= adapt_i.l * adapt_i.p_dt * diff;
      LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
      float minv = us->g_bar.l * (1.0 - adapt_i.mx_d);
      float maxv = us->g_bar.l * (1.0 + adapt_i.mx_d);
      if(lay->adapt_i.g_bar_l < minv) lay->adapt_i.g_bar_l = minv;
      if(lay->adapt_i.g_bar_l > maxv) lay->adapt_i.g_bar_l = maxv;
    }
  }
}

void LeabraLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net, bool set_both) {
  if(set_both) {
    lay->acts_m = lay->acts;
    lay->acts_p = lay->acts;
    lay->phase_dif_ratio = 1.0f;

    if(lay->units.gp.size > 0) {
      int g;
      for(g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	rugp->acts_m = rugp->acts;
	rugp->acts_p = rugp->acts;
	rugp->phase_dif_ratio = 1.0f;
      }
    }
  }
  else {
    if(net->phase == LeabraNetwork::MINUS_PHASE)
      lay->acts_m = lay->acts;
    else
      lay->acts_p = lay->acts;
    if(lay->acts_p.avg > 0.0f)
      lay->phase_dif_ratio = lay->acts_m.avg / lay->acts_p.avg;
    else
      lay->phase_dif_ratio = 1.0f;

    if(lay->units.gp.size > 0) {
      int g;
      for(g=0; g<lay->units.gp.size; g++) {
	LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
	if(net->phase == LeabraNetwork::MINUS_PHASE)
	  rugp->acts_m = rugp->acts;
	else
	  rugp->acts_p = rugp->acts;
	if(rugp->acts_p.avg > 0.0f)
	  rugp->phase_dif_ratio = rugp->acts_m.avg / rugp->acts_p.avg;
	else
	  rugp->phase_dif_ratio = 1.0f;
      }
    }
  }

  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      LeabraUnit* u;
      taLeafItr i;
      FOR_ITR_EL(LeabraUnit, u, rugp->, i)
	u->PostSettle(lay, (LeabraInhib*)rugp, net, set_both);
    }
  }
  else {
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i)
      u->PostSettle(lay, (LeabraInhib*)lay, net, set_both);
  }

  if((adapt_i.type == AdaptISpec::G_BAR_I) || (adapt_i.type == AdaptISpec::G_BAR_IL)) {
    AdaptGBarI(lay, net);
  }
}

float LeabraLayerSpec::Compute_SSE(LeabraLayer* lay, int& n_vals, bool unit_avg, bool sqrt) {
  return lay->Layer::Compute_SSE(n_vals, unit_avg, sqrt);
}

void LeabraLayerSpec::Compute_AbsRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  lay->avg_netin_sum.avg += lay->netin.avg;
  lay->avg_netin_sum.max += lay->netin.max;
  lay->avg_netin_n++;

  float sum_net = 0.0f;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    prjn->netin_avg = 0.0f;
    int netin_avg_n = 0;
    LeabraUnit* u;
    taLeafItr ui;
    FOR_ITR_EL(LeabraUnit, u, lay->units., ui) {
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      if(u->act_eq < us->opt_thresh.send) continue; // ignore if not above sending thr
      LeabraRecvCons* cg = (LeabraRecvCons*)u->recv.SafeEl(prjn->recv_idx);
      if(!cg) continue;
      float net = cg->Compute_Netin(u);
      cg->net = net;
      prjn->netin_avg += net;
      netin_avg_n++;
    }
    if(netin_avg_n > 0)
      prjn->netin_avg /= (float)netin_avg_n;
    sum_net += prjn->netin_avg;
  }

  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(sum_net > 0.0f)
      prjn->netin_rel = prjn->netin_avg / sum_net;
    // increment epoch-level
    prjn->avg_netin_avg_sum += prjn->netin_avg;
    prjn->avg_netin_rel_sum += prjn->netin_rel;
    prjn->avg_netin_n++;
  }
}

void LeabraLayerSpec::Compute_AvgAbsRelNetin(LeabraLayer* lay, LeabraNetwork* net) {
#ifdef DMEM_COMPILE
  lay->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
  if(lay->avg_netin_n > 0) {
    lay->avg_netin.avg = lay->avg_netin_sum.avg / (float)lay->avg_netin_n;
    lay->avg_netin.max = lay->avg_netin_sum.max / (float)lay->avg_netin_n;
  }
  lay->avg_netin_sum.avg = 0.0f;
  lay->avg_netin_sum.max = 0.0f;
  lay->avg_netin_n = 0;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
#ifdef DMEM_COMPILE
    prjn->DMem_ComputeAggs(net->dmem_trl_comm.comm);
#endif
    if(prjn->avg_netin_n > 0) {
      prjn->avg_netin_avg = prjn->avg_netin_avg_sum / (float)prjn->avg_netin_n;
      prjn->avg_netin_rel = prjn->avg_netin_rel_sum / (float)prjn->avg_netin_n;
    }
    prjn->avg_netin_n = 0;
    prjn->avg_netin_avg_sum = 0.0f;
    prjn->avg_netin_rel_sum = 0.0f;
  }
}

void LeabraLayerSpec::Compute_TrgRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  int n_in = 0;
  int n_out = 0;
  int n_lat = 0;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) {
      prjn->trg_netin_rel = 0.0f;
      continue;
    }
    if(prjn->con_spec->InheritsFrom(&TA_MarkerConSpec)) { // fix these guys
      prjn->trg_netin_rel = 0.0f;
      prjn->direction = Projection::DIR_UNKNOWN;
      continue;
    }
    if(prjn->direction == Projection::FM_INPUT) n_in++;
    else if(prjn->direction == Projection::FM_OUTPUT) n_out++;
    else if(prjn->direction == Projection::LATERAL) n_lat++;
  }
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    float in_trg = cs->rel_net_adapt.trg_fm_input;
    float out_trg = cs->rel_net_adapt.trg_fm_output;
    float lat_trg = cs->rel_net_adapt.trg_lateral;
    if(prjn->direction == Projection::FM_INPUT) {
      if(n_out == 0 && n_lat == 0) in_trg = 1.0;
      else if(n_out == 0) in_trg = in_trg / (in_trg + lat_trg);
      else if(n_lat == 0) in_trg = in_trg / (in_trg + out_trg);
      prjn->trg_netin_rel = in_trg / (float)n_in;
    }
    else if(prjn->direction == Projection::FM_OUTPUT) {
      if(n_in == 0 && n_lat == 0) out_trg = 1.0;
      else if(n_in == 0) out_trg = out_trg / (out_trg + lat_trg);
      else if(n_lat == 0) out_trg = out_trg / (out_trg + in_trg);
      prjn->trg_netin_rel = out_trg / (float)n_out;
    }
    else if(prjn->direction == Projection::LATERAL) {
      if(n_in == 0 && n_out == 0) lat_trg = 1.0;
      else if(n_in == 0) lat_trg = lat_trg / (lat_trg + out_trg);
      else if(n_out == 0) lat_trg = lat_trg / (lat_trg + in_trg);
      prjn->trg_netin_rel = lat_trg / (float)n_lat;
    }
  }
}

void LeabraLayerSpec::Compute_AdaptRelNetin(LeabraLayer* lay, LeabraNetwork*) {
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    if(prjn->trg_netin_rel <= 0.0f) continue; // not set
    if(!cs->rel_net_adapt.on) continue;
    if(cs->rel_net_adapt.CheckInTolerance(prjn->trg_netin_rel, prjn->avg_netin_rel))
      continue;
    cs->SetUnique("wt_scale", true);
    cs->wt_scale.rel += cs->rel_net_adapt.rel_lrate * 
      (prjn->trg_netin_rel - prjn->avg_netin_rel);
    if(cs->wt_scale.rel <= 0.0f) cs->wt_scale.rel = 0.0f;
  }
}

void LeabraLayerSpec::Compute_AdaptAbsNetin(LeabraLayer* lay, LeabraNetwork*) {
  if(!abs_net_adapt.on) return;
  float dst = abs_net_adapt.trg_net - lay->avg_netin.max;
  if(fabsf(dst) < abs_net_adapt.tol) return;
  for(int i=0;i<lay->projections.size;i++) {
    LeabraPrjn* prjn = (LeabraPrjn*)lay->projections[i];
    if(!prjn->from || prjn->from->lesioned()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)prjn->con_spec.SPtr();
    cs->SetUnique("wt_scale", true);
    cs->wt_scale.abs += abs_net_adapt.abs_lrate * dst;
  }
}


//////////////////////////////////////////
//	Stage 6: Learning 		//
//////////////////////////////////////////

void LeabraLayerSpec::AdaptKWTAPt(LeabraLayer* lay, LeabraNetwork*) {
  if((lay->ext_flag & Unit::EXT) && !(lay->ext_flag & Unit::TARG))
    return;			// don't adapt points for input-only layers
  if((inhib_group != ENTIRE_LAYER) && (lay->units.gp.size > 0)) {
    int g;
    for(g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      // use minus phase activations to adapt
      rugp->adapt_i.avg_avg += adapt_i.a_dt * (rugp->acts_m.avg - rugp->adapt_i.avg_avg);
      float dif = rugp->adapt_i.avg_avg - rugp->kwta.pct;
      if(dif < -adapt_i.tol) {	// average is less than target
	// so reduce the point towards lower value
	rugp->adapt_i.i_kwta_pt += adapt_i.p_dt * 
	  ((i_kwta_pt - adapt_i.mx_d) - rugp->adapt_i.i_kwta_pt);
      }
      else if(dif > adapt_i.tol) {	// average is more than target
	// so increase point towards higher value
	rugp->adapt_i.i_kwta_pt += adapt_i.p_dt * 
	  ((i_kwta_pt + adapt_i.mx_d) - rugp->adapt_i.i_kwta_pt);
      }
    }
  }
  lay->adapt_i.avg_avg += adapt_i.a_dt * (lay->acts_m.avg - lay->adapt_i.avg_avg);
  float dif = lay->adapt_i.avg_avg - lay->kwta.pct;
  if(dif < -adapt_i.tol) {	// average is less than target
    // so reduce the point towards lower value
    lay->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((i_kwta_pt - adapt_i.mx_d) - lay->adapt_i.i_kwta_pt);
  }
  else if(dif > adapt_i.tol) {	// average is more than target
    // so increase point towards higher value
    lay->adapt_i.i_kwta_pt += adapt_i.p_dt * 
      ((i_kwta_pt + adapt_i.mx_d) - lay->adapt_i.i_kwta_pt);
  }
}

void LeabraLayerSpec::Compute_dWt(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_dWt(lay, net);
  AdaptKWTAPt(lay, net);
}

void LeabraLayerSpec::Compute_WtFmLin(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(LeabraUnit, u, lay->units., i)
    u->Compute_WtFmLin(lay, net);
}

void LeabraLayer::Compute_Weights() {
  Layer::Compute_Weights();
}

bool LeabraLayer::CheckConfig_impl(bool quiet) {
  //note: inherited does so much, we only augment with spec
  bool rval = inherited::CheckConfig_impl(quiet);
  if(!spec.CheckSpec()) return false; // fatal
  if(!spec->CheckConfig_Layer(this, quiet))
      rval = false;
  return rval;
}

//////////////////////////
// 	LeabraLayer	//
//////////////////////////
  
void AvgMaxVals::Initialize() {
  avg = max = 0.0f; max_i = -1;
}

void AvgMaxVals::Copy_(const AvgMaxVals& cp) {
  avg = cp.avg; max = cp.max; max_i = cp.max_i;
}

void KWTAVals::Initialize() {
  k = 12;
  pct = .25f;
  pct_c = .75f;
  adth_k = 1;
  k_ithr = 0.0f;
  k1_ithr = 0.0f;
  ithr_r = 0.0f;
  ithr_diff = 0.0f;
  tie_brk = 0;
}

void KWTAVals::Copy_(const KWTAVals& cp) {
  k = cp.k;
  pct = cp.pct;
  pct_c = cp.pct_c;
  adth_k = cp.adth_k;
  k_ithr = cp.k_ithr;
  k1_ithr = cp.k1_ithr;
  ithr_r = cp.ithr_r;
  ithr_diff = cp.ithr_diff;
  tie_brk = cp.tie_brk;
}

void KWTAVals::Compute_Pct(int n_units) {
  if(n_units > 0)
    pct = (float)k / (float)n_units;
  else
    pct = 0.0f;
  pct_c = 1.0f - pct;
}

void KWTAVals::Compute_IThrR() {
  if(k1_ithr <= 0.0f)
    ithr_r = 0.0f;
  else 
    ithr_r = logf(k_ithr / k1_ithr);
}

void AdaptIVals::Initialize() {
  avg_avg = 0.0f;
  i_kwta_pt = 0.0f;
  g_bar_i = 1.0f;
  g_bar_l = .1f;
}

void AdaptIVals::Copy_(const AdaptIVals& cp) {
  avg_avg = cp.avg_avg;
  i_kwta_pt = cp.i_kwta_pt;
  g_bar_i = cp.g_bar_i;
  g_bar_l = cp.g_bar_l;
}

void InhibVals::Initialize() {
  kwta = 0.0f;
  g_i = 0.0f;
  gp_g_i = 0.0f;
  g_i_orig = 0.0f;
}

void InhibVals::Copy_(const InhibVals& cp) {
  kwta = cp.kwta;
  g_i = cp.g_i;
  gp_g_i = cp.gp_g_i;
  g_i_orig = cp.g_i_orig;
}

void LeabraInhib::Inhib_Initialize() {
  kwta.k = 1;
  kwta.pct = .25;
  kwta.pct_c = .75;
  i_val.Defaults();
  phase_dif_ratio = 1.0f;
}

void LeabraInhib::Inhib_Init_Acts(LeabraLayerSpec*) {
  i_val.Defaults();
  netin.Defaults();
  i_thrs.Defaults();
  acts.Defaults();
  un_g_i.Defaults();
}

void LeabraLayer::Initialize() {
  spec.SetBaseType(&TA_LeabraLayerSpec);
  projections.SetBaseType(&TA_LeabraPrjn);
  send_prjns.SetBaseType(&TA_LeabraPrjn);
  units.SetBaseType(&TA_LeabraUnit);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
  unit_spec.SetBaseType(&TA_LeabraUnitSpec);

  Inhib_Initialize();
  stm_gain = .5f;
  hard_clamped = false;
  dav = 0.0f;
  da_updt = false;
  net_rescale = 1.0f;

  avg_netin_n = 0;
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}  

void LeabraLayer::InitLinks() {
  inherited::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);

  taBase::Own(avg_netin, this);
  taBase::Own(avg_netin_sum, this);

  taBase::Own(misc_iar, this);

#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  DMem_InitAggs();
#endif
  spec.SetDefaultSpec(this);
  units.gp.SetBaseType(&TA_LeabraUnit_Group);
}

void LeabraLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();
  misc_iar.CutLinks();
}

void LeabraInhib::Inhib_Copy_(const LeabraInhib& cp) {
  netin = cp.netin;
  i_thrs = cp.i_thrs;
  acts = cp.acts;
  acts_p = cp.acts_p;
  acts_m = cp.acts_m;
  phase_dif_ratio = cp.phase_dif_ratio;
  kwta = cp.kwta;
  i_val = cp.i_val;
  un_g_i = cp.un_g_i;
}

void LeabraLayer::Copy_(const LeabraLayer& cp) {
  Inhib_Copy_(cp);
  spec = cp.spec;
  stm_gain = cp.stm_gain;
  hard_clamped = cp.hard_clamped;
  misc_iar = cp.misc_iar;
}

void LeabraLayer::ResetSortBuf() {
  Inhib_ResetSortBuf();		// reset sort buf after any edit..
  if(units.gp.size > 0) {
    int g;
    for(g=0; g<units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)units.gp[g];
      rugp->Inhib_ResetSortBuf();
    }
  }
}

void LeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ResetSortBuf();
}

bool LeabraLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)	return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((LeabraLayerSpec*)sp);
  else
    return false;
  return true;
} 

void LeabraLayer::BuildUnits() {
  ResetSortBuf();
  inherited::BuildUnits();
}

#ifdef DMEM_COMPILE
void LeabraLayer::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraLayer::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif

void LeabraUnit_Group::Initialize() {
  Inhib_Initialize();
  misc_state = 0;
  misc_state1 = 0;
  misc_state2 = 0;
}

void LeabraUnit_Group::InitLinks() {
  inherited::InitLinks();
  taBase::Own(netin, this);
  taBase::Own(i_thrs, this);
  taBase::Own(acts, this);

  taBase::Own(acts_p, this);
  taBase::Own(acts_m, this);

  taBase::Own(kwta, this);
  taBase::Own(i_val, this);
  taBase::Own(adapt_i, this);
}

void LeabraUnit_Group::Copy_(const LeabraUnit_Group& cp) {
  Inhib_Copy_(cp);
  misc_state = cp.misc_state;
  misc_state1 = cp.misc_state1;
  misc_state2 = cp.misc_state2;
}


//////////////////////////
//  	Network		//
//////////////////////////

void LeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_LeabraLayer);

  phase_order = MINUS_PLUS;
  no_plus_test = true;
  trial_init = DECAY_STATE;
  sequence_init = DO_NOTHING;
  first_plus_dwt = ONLY_FIRST_DWT;
  phase = MINUS_PHASE;
  phase_no = 0;
  phase_max = 2;

  cycle_max = 60;
  min_cycles = 15;
  min_cycles_phase2 = 35;

  minus_cycles = 0.0f;
  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;

  netin_mod = 1;
  //  send_delta = false;
  send_delta = true;		// so much faster -- should always be on by default

  maxda_stopcrit = .005f;
  maxda = 0.0f;
  
  trg_max_act_stopcrit = 1.0f;	// disabled
  trg_max_act = 0.0f;

  ext_rew = 0.0f;
  avg_ext_rew = 0.0f;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::Init_Counters() {
  inherited::Init_Counters();
  phase = MINUS_PHASE;
  phase_no = 0;
}

void LeabraNetwork::Init_Stats() {
  inherited::Init_Stats();
  maxda = 0.0f;
  trg_max_act = 0.0f;

  minus_cycles = 0.0f;
  avg_cycles = 0.0f;
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;

  ext_rew = 0.0f;
  avg_ext_rew = 0.0f;
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_LeabraCon;
  prjn->recvcons_type = &TA_LeabraRecvCons;
  prjn->sendcons_type = &TA_LeabraSendCons;
  prjn->con_spec.SetBaseType(&TA_LeabraConSpec);
}

//////////////////////////////////
// 	Cycle-Level Functions	//
//////////////////////////////////

void LeabraNetwork::Compute_Netin() {
  Init_Netin();		// this is done first because of sender-based net
  if(send_delta) {
    LeabraLayer* l;
    taLeafItr i;
    FOR_ITR_EL(LeabraLayer, l, layers., i) {
      if(!l->lesioned())
	l->Send_NetinDelta();
    }
#ifdef DMEM_COMPILE
    dmem_share_units.Sync(3);
#endif
  }
  else {
    Send_Netin();
  }
}

void LeabraNetwork::Compute_Clamp_NetAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Clamp_NetAvg(this);
  }
}

void LeabraNetwork::Compute_Inhib() {
  bool do_lay_gp = false;
  if(layers.gp.size == 0) do_lay_gp = true; // falsely set to true to prevent further checking
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Inhib(this);
    if(!do_lay_gp) {
      LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
      if(laysp->kwta.gp_i) do_lay_gp = true;
    }
  }
  if(layers.gp.size == 0) do_lay_gp = false; // now override anything
  if(do_lay_gp) {
    for(int lgi = 0; lgi < layers.gp.size; lgi++) {
      Layer_Group* lg = (Layer_Group*)layers.gp[lgi];
      float lay_gp_g_i = 0.0f;
      for(int li = 0; li < lg->size; li++) {
	lay = (LeabraLayer*)lg->FastEl(li);
	LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
	if(lay->lesioned() || !laysp->kwta.gp_i) continue;
	float lay_val = laysp->kwta.gp_g * lay->i_val.g_i;
	lay_gp_g_i = MAX(lay_val, lay_gp_g_i);
      }
      if(lay_gp_g_i > 0.0f) {	// actually got something
	for(int li = 0; li < lg->size; li++) {
	  lay = (LeabraLayer*)lg->FastEl(li);
	  LeabraLayerSpec* laysp = (LeabraLayerSpec*)lay->spec.SPtr();
	  if(lay->lesioned() || !laysp->kwta.gp_i) continue;
	  lay->i_val.gp_g_i = lay_gp_g_i;
	  if(laysp->kwta.gp_g > 1.0)
	    lay->i_val.gp_g_i /= laysp->kwta.gp_g; // if > 1, need to renorm otherwise its own inhib will cancel itself out!!
	  lay->i_val.g_i = MAX(lay->i_val.gp_g_i, lay->i_val.g_i);

	  if(lay->units.gp.size > 0) {
	    lay->Compute_LayInhibToGps(this);
	  }
	}
      }
    }
  }
}

void LeabraNetwork::Compute_InhibAvg() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_InhibAvg(this);
  }
}

void LeabraNetwork::Compute_Act() {
  output_name = "";		// this will be updated by layer compute_act
  maxda = 0.0f;		// initialize
  trg_max_act = 0.0f;
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Act(this);	// note: maxda is updated in here
  }
}

void LeabraNetwork::Cycle_Run() {
  if((cycle % netin_mod) == 0) {
    Compute_Netin();
    Compute_Clamp_NetAvg();
    Compute_Inhib();
    Compute_InhibAvg();
  }
  Compute_Act();
  //  taiMiscCore::RunPending();
}

//////////////////////////////////
// 	Settle-Level Functions	//
//////////////////////////////////

void LeabraNetwork::Compute_Active_K() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_Active_K();	// this gets done at the outset..
  }
}
  
void LeabraNetwork::DecayEvent() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayEvent(this);
  }
}

void LeabraNetwork::DecayPhase() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayPhase(this);
  }
}

void LeabraNetwork::DecayPhase2() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayPhase2(this);
  }
}

void LeabraNetwork::PhaseInit() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->PhaseInit(this);
  }
}

void LeabraNetwork::ExtToComp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->ExtToComp(this);
  }
}

void LeabraNetwork::TargExtToComp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->TargExtToComp(this);
  }
}

void LeabraNetwork::Compute_HardClamp() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_HardClamp(this);
  }
}

void LeabraNetwork::Compute_NetinScale() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_NetinScale(this);
  }
}

void LeabraNetwork::Send_ClampNet() {
  LeabraLayer* lay;
  taLeafItr l;
  // first init
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Init_ClampNet(this);
  }
  // then send
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Send_ClampNet(this);
  }
#ifdef DMEM_COMPILE
  if(send_delta)
    dmem_share_units.Sync(4);
#endif
}

void LeabraNetwork::PostSettle() {
  bool set_both = false;
  if((phase_order == PLUS_ONLY) || (no_plus_test && (train_mode == TEST))) {
    set_both = true;
  }
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->PostSettle(this, set_both);
  }
}

void LeabraNetwork::PostSettle_NStdLay() {
  bool set_both = false;
  if((phase_order == PLUS_ONLY) || (no_plus_test && (train_mode == TEST))) {
    set_both = true;
  }
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned() && (lay->spec.SPtr()->GetTypeDef() != &TA_LeabraLayerSpec))
      lay->PostSettle(this, set_both);
  }
}

void LeabraNetwork::Settle_Init() {
  int tmp_cycle = cycle;
  cycle = -2;			// special signal for settle init

  if(phase_no >= 3) { // second plus phase or more: use phase2..
    DecayPhase2();
  }
  else if(phase_no == 2) {
    DecayPhase2();		// decay before 2nd phase set
    if(phase_order == MINUS_PLUS_NOTHING) {
      TargExtToComp();		// all external input is now 'comparison'
    }
  }
  else if(phase_no == 1) {
    if(phase_order == PLUS_NOTHING) { // actually a nothing phase
      DecayPhase2();
      TargExtToComp();
    }
    else
      DecayPhase();		    // prepare for next phase
  }

  Compute_Active_K();		// compute here because could depend on pat_n
  PhaseInit();
  
  Compute_HardClamp();		// first clamp all hard-clamped input acts
  Compute_NetinScale();		// and then compute net scaling
  Send_ClampNet();		// and send net from clamped inputs
  cycle = tmp_cycle;
}	

void LeabraNetwork::Settle_Final() {
  // compute weight changes at end of second settle..
  PostSettle();
  if((phase_order == MINUS_PLUS_NOTHING) && (phase_no == 1)) {
    if(train_mode != TEST)
      Compute_dWt();
  }
  else if((phase_order == MINUS_PLUS_PLUS) && (phase_no == 1)) {
    if(train_mode != TEST) {
      if(first_plus_dwt == NO_FIRST_DWT)
	Compute_dWt_NStdLay();	// only do non-standard ones
      else if(first_plus_dwt == ALL_DWT)
	Compute_dWt();
      else if(first_plus_dwt == ONLY_FIRST_DWT) {
	Compute_dWt();
      }
    }
  }
  else if(phase_order == MINUS_PLUS_2) {
    if(phase_no == 1)
      Compute_dWt();
    else if(phase_no == 2)
      Compute_dWt_NStdLay();	// only do non-standard ones
  }
}
  
//////////////////////////////////
// 	Trial-Level Functions	//
//////////////////////////////////

// todo: currently called in Trial_Init -- could be in a more efficient location, but
// cost is minimal, so not bothering for now..
void LeabraNetwork::SetCurLrate() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->SetCurLrate(this, epoch);
  }
}

void LeabraNetwork::DecayState() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->DecayEvent(this);
  }
}

void LeabraNetwork::EncodeState() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())     continue;
    LeabraUnit* u;
    taLeafItr i;
    FOR_ITR_EL(LeabraUnit, u, lay->units., i)
      u->EncodeState(lay, this);
  }
}

void LeabraNetwork::Compute_dWt() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_dWt(this);
  }
}

void LeabraNetwork::Compute_dWt_NStdLay() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    if(lay->spec.SPtr()->GetTypeDef() != &TA_LeabraLayerSpec)
      lay->Compute_dWt(this);
  }
}

void LeabraNetwork::Trial_Init() {
  SetCurLrate();		// todo: this is excessive but not clear where else to call it..

  cycle = -1;
  phase = MINUS_PHASE;
  phase_max = 2;
  bool is_testing = false;

  if(no_plus_test && (train_mode == TEST)) {
    phase_max = 1;		// just do one loop (the minus phase)
    is_testing = true;
  }

  // todo: this is now the responsibility of the prog
//   if(cur_event != NULL) {
//     if(cur_event->spec->InheritsFrom(TA_PhaseOrderEventSpec)) {
//       PhaseOrderEventSpec* es = (PhaseOrderEventSpec*)cur_event->spec.SPtr();
//       if(es->phase_order == PhaseOrderEventSpec::MINUS_PLUS)
// 	phase = MINUS_PHASE;
//       else if(es->phase_order == PhaseOrderEventSpec::MINUS_ONLY) {
// 	phase = MINUS_PHASE;
// 	phase_no.SetMax(1);
//       }
//       else if(es->phase_order == PhaseOrderEventSpec::PLUS_MINUS) {
// 	phase = PLUS_PHASE;
// 	if(is_testing)
// 	  phase_no.SetMax(2);	// need to present plus phase first, then minus..
//       }
//       else if(es->phase_order == PhaseOrderEventSpec::PLUS_ONLY) {
// 	phase = PLUS_PHASE;
// 	phase_no.SetMax(1);
//       }
//     }
//   }

  if(!is_testing) {
    if(phase_order == PLUS_ONLY) {
      phase_max = 1;
      phase = PLUS_PHASE;
    }
    else if((phase_order == MINUS_PLUS_NOTHING) || (phase_order == MINUS_PLUS_PLUS)) {
      phase_max = 3;
    }
    else if(phase_order == MINUS_PLUS_2) {
      phase_max = 4;
    }
    else if(phase_order == PLUS_NOTHING) {
      phase_max = 2;
      phase = PLUS_PHASE;
    }
  }

  // todo: this seems kinda silly and should perhaps just be put in the prog: not worth the extra state vars
  if(trial_init == INIT_STATE)
    Init_Acts();
  else if(trial_init == DECAY_STATE)
    DecayState();
}

void LeabraNetwork::Trial_UpdatePhase() {
  // todo: prog needs to deal
//   if((cur_event != NULL) && (cur_event->spec->InheritsFrom(TA_PhaseOrderEventSpec))) {
//     PhaseOrderEventSpec* es = (PhaseOrderEventSpec*)cur_event->spec.SPtr();
//     if(es->phase_order == PhaseOrderEventSpec::MINUS_PLUS)
//       phase = PLUS_PHASE;
//     else if(es->phase_order == PhaseOrderEventSpec::PLUS_MINUS)
//       phase = MINUS_PHASE;
//   }
//   else {

  if(phase_order == PLUS_NOTHING) {
    phase = MINUS_PHASE;
  }
  else {
    if(phase_no == 1)
      phase = PLUS_PHASE;
    else {
      if(phase_order == MINUS_PLUS_NOTHING)
	phase = MINUS_PHASE;
      else if(phase_order == MINUS_PLUS_PLUS)
	phase = PLUS_PHASE;
      else if(phase_order == MINUS_PLUS_2) {
	if(phase_no == 2) phase = MINUS_2;
	else phase = PLUS_2;
      }	    
    }
  }
}

void LeabraNetwork::Trial_Final() {
  if(train_mode != TEST) {
    if((phase_order == MINUS_PLUS_PLUS) && (first_plus_dwt == ONLY_FIRST_DWT))
      Compute_dWt_NStdLay();	// only update the non-standard layers, which will have checks
    else if(phase_order == MINUS_PLUS_2)
      Compute_dWt_NStdLay();	// only update the non-standard layers, which will have checks
    else {
      if(!((phase_max == 1) && (phase == MINUS_PHASE))) // if only minus phase, don't do it!
	Compute_dWt();		// get them all
    }
  }
  EncodeState();
}

void LeabraNetwork::Compute_ExtRew() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    if(lay->spec.SPtr()->GetTypeDef() != &TA_ExtRewLayerSpec) continue;
    LeabraUnit* eru = (LeabraUnit*)lay->units.Leaf(0);
    if(eru->misc_1 == 0.0f) { // indication of no reward available
      ext_rew = -1.1f;
    }
    else {
      ext_rew = eru->act_eq;	// just set it!
      avg_ext_rew_sum += ext_rew;
      avg_ext_rew_n++;
    }
  }
}

void LeabraNetwork::Compute_MinusCycles() {
  minus_cycles = cycle;
  avg_cycles_sum += minus_cycles;
  avg_cycles_n++;
}

void LeabraNetwork::Compute_TrialStats() {
  inherited::Compute_TrialStats();
  Compute_MinusCycles();
}

void LeabraNetwork::Compute_AbsRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AbsRelNetin(this);
  }
}

void LeabraNetwork::Compute_AvgAbsRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AvgAbsRelNetin(this);
  }
}

void LeabraNetwork::Compute_TrgRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_TrgRelNetin(this);
  }
}

void LeabraNetwork::Compute_AdaptRelNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AdaptRelNetin(this);
  }
}

void LeabraNetwork::Compute_AdaptAbsNetin() {
  LeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(LeabraLayer, lay, layers., l) {
    if(!lay->lesioned())
      lay->Compute_AdaptAbsNetin(this);
  }
}

void LeabraNetwork::Compute_AvgCycles() {
  if(avg_cycles_n > 0) {
    avg_cycles = avg_cycles_sum / (float)avg_cycles_n;
  }
  avg_cycles_sum = 0.0f;
  avg_cycles_n = 0;
}

void LeabraNetwork::Compute_AvgExtRew() {
  if(avg_ext_rew_n > 0) {
    avg_ext_rew = avg_ext_rew_sum / (float)avg_ext_rew_n;
  }
  avg_ext_rew_sum = 0.0f;
  avg_ext_rew_n = 0;
}

void LeabraNetwork::Compute_EpochStats() {
  inherited::Compute_EpochStats();
  Compute_AvgCycles();
  Compute_AvgExtRew();
}

///////////////////////////////////////////////////////////////////////////////////////
// 	LeabraProject

void LeabraProject::Initialize() {
  networks.SetBaseType(&TA_LeabraNetwork);
  wizards.SetBaseType(&TA_LeabraWizard);
}

///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//		Wizard		//
//////////////////////////////////

void LeabraWizard::Initialize() {
  connectivity = BIDIRECTIONAL;
}

void LeabraWizard::StdNetwork(Network* net) {
  if(net == NULL)
    net = pdpMisc::GetNewNetwork(GET_MY_OWNER(LeabraProject));
  if(net == NULL) return;
  inherited::StdNetwork(net);
  StdLayerSpecs((LeabraNetwork*)net);
}

void LeabraWizard::StdLayerSpecs(LeabraNetwork* net) {
  if(net == NULL) return;
  LeabraLayerSpec* hid = (LeabraLayerSpec*)net->FindMakeSpec(NULL, &TA_LeabraLayerSpec);
  hid->name = "HiddenLayer";
  LeabraLayerSpec* inout = (LeabraLayerSpec*)hid->children.FindMakeSpec("Input_Output", &TA_LeabraLayerSpec);
  hid->compute_i = LeabraLayerSpec::KWTA_AVG_INHIB;
  hid->i_kwta_pt = .6f;
  inout->SetUnique("compute_i", true);
  inout->SetUnique("i_kwta_pt", true);
  inout->SetUnique("kwta", true);
  inout->compute_i = LeabraLayerSpec::KWTA_INHIB;
  inout->i_kwta_pt = .25f;
  inout->kwta.k_from = KWTASpec::USE_PAT_K;
  // todo: !!!
//   winbMisc::DelayedMenuUpdate(hid);

  int i;
  if(net->layers.size == layer_cfg.size) {	// likely to be using specs
    for(i=0;i<layer_cfg.size;i++) {
      LayerWizEl* el = (LayerWizEl*)layer_cfg[i];
      Layer* lay = (Layer*)net->layers.FindName(el->name);
      if(lay != NULL) {
	if(el->io_type == LayerWizEl::HIDDEN)
	  lay->SetLayerSpec(hid);
	else
	  lay->SetLayerSpec(inout);
      }
    }
  }
  else {
    for(i=0;i<net->layers.size;i++) {
      Layer* lay = (Layer*)net->layers[i];
      if(lay->layer_type == Layer::HIDDEN)
	lay->SetLayerSpec(hid);
      else
	lay->SetLayerSpec(inout);
    }
  }

  // move the bias spec under the con spec
  LeabraBiasSpec* bs = (LeabraBiasSpec*)net->specs.FindType(&TA_LeabraBiasSpec);
  if(bs != NULL) {
    LeabraConSpec* ps = (LeabraConSpec*)bs->FindParent();
    if(ps != NULL) return;
    ps = (LeabraConSpec*)net->specs.FindSpecTypeNotMe(&TA_LeabraConSpec, bs);
    if(ps != NULL) {
      ps->children.Transfer(bs);
      // todo: !!!
//       winbMisc::DelayedMenuUpdate(ps);
    }
  }
}

///////////////////////////////////////////////////////////////
//			Unit Inhib
///////////////////////////////////////////////////////////////

void LeabraWizard::UnitInhib(LeabraNetwork* net, int n_inhib_units) {
  net->RemoveUnits();
  
  LeabraUnitSpec* basic_us = (LeabraUnitSpec*)net->FindSpecType(&TA_LeabraUnitSpec);
  if(TestError(!basic_us, "UnitInhib", "basic LeabraUnitSpec not found, bailing!")) {
    return;
  }
  LeabraUnitSpec* inhib_us = (LeabraUnitSpec*)basic_us->children.FindMakeSpec("InhibUnits", &TA_LeabraUnitSpec);

  LeabraConSpec* basic_cs = (LeabraConSpec*)net->FindSpecType(&TA_LeabraConSpec);
  if(TestError(!basic_cs, "UnitInhib", "basic LeabraConSpec not found, bailing!")) {
    return;
  }
  LeabraConSpec* inhib_cs = (LeabraConSpec*)basic_cs->children.FindMakeSpec("InhibCons", &TA_LeabraConSpec);

  LeabraConSpec* fb_inhib_cs = (LeabraConSpec*)basic_cs->children.FindMakeSpec("FBtoInhib", &TA_LeabraConSpec);
  LeabraConSpec* ff_inhib_cs = (LeabraConSpec*)fb_inhib_cs->children.FindMakeSpec("FFtoInhib", &TA_LeabraConSpec);

  LeabraLayerSpec* basic_ls = (LeabraLayerSpec*)net->FindSpecType(&TA_LeabraLayerSpec);
  if(TestError(!basic_ls, "UnitInhib", "basic LeabraLayerSpec not found, bailing!")) {
    return;
  }
  LeabraLayerSpec* inhib_ls = (LeabraLayerSpec*)basic_ls->children.FindMakeSpec("InhibLayers", &TA_LeabraLayerSpec);

  FullPrjnSpec* fullprjn = (FullPrjnSpec*)net->FindSpecType(&TA_FullPrjnSpec);
  if(TestError(!fullprjn, "UnitInhib", "basic FullPrjnSpec not found, bailing!")) {
    return;
  }

  // todo: optimize these params..
  basic_us->dt.vm = .04f;
  basic_us->g_bar.i = 10.0f;
  inhib_us->SetUnique("dt", true);
  inhib_us->dt.vm = .07f;

  inhib_cs->SetUnique("rnd", true);
  inhib_cs->rnd.mean = 1.0f;  inhib_cs->rnd.var = 0.0f;
  inhib_cs->SetUnique("wt_limits", true);
  inhib_cs->wt_limits.sym = false;
  inhib_cs->SetUnique("inhib", true);
  inhib_cs->inhib = true;

  fb_inhib_cs->SetUnique("wt_limits", true);
  fb_inhib_cs->wt_limits.sym = false;
  fb_inhib_cs->SetUnique("rnd", true);
  fb_inhib_cs->rnd.mean = .5f;  fb_inhib_cs->rnd.var = 0.05f;
  fb_inhib_cs->SetUnique("lrate", true);
  fb_inhib_cs->lrate = 0.0f;

  // todo: optimize
  ff_inhib_cs->SetUnique("wt_scale", true);
  ff_inhib_cs->wt_scale.abs = .4f;

  basic_ls->compute_i = LeabraLayerSpec::UNIT_INHIB;
  basic_ls->adapt_i.type = AdaptISpec::G_BAR_I;
  basic_ls->adapt_i.tol = .02f;	// these params sometimes get off..
  basic_ls->adapt_i.p_dt = .1f;

  inhib_ls->SetUnique("kwta", true);
  inhib_ls->kwta.k_from = KWTASpec::USE_PCT;
  inhib_ls->kwta.pct = .34f;

  basic_us->UpdateAfterEdit();
  basic_cs->UpdateAfterEdit();
  basic_ls->UpdateAfterEdit();

  int i;
  for(i=0;i<net->layers.size;i++) {
    LeabraLayer* lay = (LeabraLayer*)net->layers[i];
    if(lay->layer_type == Layer::INPUT) continue;
    String nm = lay->name;
    nm.downcase();
    if(nm.contains("_inhib")) continue;

    String inm = lay->name + "_Inhib";
    LeabraLayer* ilay = (LeabraLayer*)net->layers.FindName(inm);
    if(ilay == NULL) {
      ilay = (LeabraLayer*)net->layers.NewEl(1);
      ilay->name = inm;
      ilay->pos.z = lay->pos.z;
      ilay->pos.x = lay->pos.x + lay->act_geom.x + 1;
      ilay->un_geom.n = n_inhib_units;
      if(n_inhib_units <= 20) {
	ilay->un_geom.x = 2; ilay->un_geom.y = n_inhib_units / 2;
	while(ilay->un_geom.x * ilay->un_geom.y < n_inhib_units) ilay->un_geom.y++;
      }
      else if(n_inhib_units <= 40) {
	ilay->un_geom.x = 4; ilay->un_geom.y = n_inhib_units / 4;
	while(ilay->un_geom.x * ilay->un_geom.y < n_inhib_units) ilay->un_geom.y++;
      }
    }
    ilay->SetLayerSpec(inhib_ls);
    ilay->SetUnitSpec(inhib_us);

    int j;
    for(j=0;j<lay->projections.size;j++) {
      LeabraLayer* fmlay = (LeabraLayer*)((Projection*)lay->projections[j])->from;
      if(fmlay->name.contains("_Inhib")) continue;
      if(fmlay == lay) continue;
      net->FindMakePrjn(ilay, fmlay, fullprjn, ff_inhib_cs);
    }
    net->FindMakePrjn(ilay, lay, fullprjn, fb_inhib_cs);
    net->FindMakePrjn(lay, ilay, fullprjn, inhib_cs);
    net->FindMakePrjn(ilay, ilay, fullprjn, inhib_cs);
  }

  net->UpdateAfterEdit();
  // todo:
//   net->InitAllViews();

  // todo: !!!
//   winbMisc::DelayedMenuUpdate(net);

  // set settle cycles to 300
  net->cycle_max = 300;
  net->min_cycles = 150;

  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  SelectEdit* edit = pdpMisc::FindSelectEdit(proj);
  if(edit != NULL) {
    basic_us->SelectForEditNm("dt", edit, "excite");
    inhib_us->SelectForEditNm("dt", edit, "inhib");
    basic_us->SelectForEditNm("g_bar", edit, "excite");
    ff_inhib_cs->SelectForEditNm("wt_scale", edit, "ff_inhib");
    fb_inhib_cs->SelectForEditNm("rnd", edit, "to_inhib");
    inhib_cs->SelectForEditNm("rnd", edit, "fm_inhib");
    basic_ls->SelectForEditNm("adapt_i", edit, "layers");

    net->SelectForEditNm("cycle_max", edit, net->name);
    net->SelectForEditNm("min_cycles", edit, net->name);
  }
}

void LeabraWizard::StdProgs() {
  // todo: could check for subgroups and do LeabraAll_GpData instead
  //Program_Group* pg = 
  StdProgs_impl("LeabraAll_Std");
  // todo: could do something more here..
}

void LeabraWizard::FindObj(const String& nm) {
  taBase_PtrList items;
  taBase_PtrList owners;
  
  LeabraProject* proj = GET_MY_OWNER(LeabraProject);
  proj->SearchNameContains(nm, items, &owners);
  cerr << "items:" << endl;
  items.List(cerr);
  
  cerr << "owners:" << endl;
  owners.List(cerr);
}
