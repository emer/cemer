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



// bp.cc

#include "bp.h"
eTypeDef_Of(BpWizard);

#include <DumpFileCvt>
#include <DataTable>
#include <MemberDef>

#include <taMisc>

#include <math.h>
#include <limits.h>
#include <float.h>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

TA_BASEFUNS_CTORS_DEFN(BpConSpec);
TA_BASEFUNS_CTORS_DEFN(BpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(BpUnit);
TA_BASEFUNS_CTORS_DEFN(HebbBpConSpec);
TA_BASEFUNS_CTORS_DEFN(LinearBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(StochasticBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(ExpBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(SoftMaxBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(BpLayer);
TA_BASEFUNS_CTORS_DEFN(BpProject);
TA_BASEFUNS_CTORS_DEFN(ErrScaleBpConSpec);
TA_BASEFUNS_CTORS_DEFN(DeltaBarDeltaBpConSpec);
TA_BASEFUNS_CTORS_DEFN(BpContextSpec);
TA_BASEFUNS_CTORS_DEFN(ThreshLinBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(XX1BpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(NoisyBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(RBFBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(BumpBpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(BpNetwork);

static void bp_converter_init() {
  DumpFileCvt* cvt = new DumpFileCvt("Bp", "BpUnit");
  cvt->repl_strs.Add(NameVar("_MGroup", "_Group"));
  cvt->repl_strs.Add(NameVar("Project", "V3BpProject"));
  cvt->repl_strs.Add(NameVar("V3BpProject_Group", "Project_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("V3BpProjection", "Projection"));
  cvt->repl_strs.Add(NameVar("BpPrjn_Group", "Projection_Group"));
  cvt->repl_strs.Add(NameVar("Network", "BpNetwork"));
  cvt->repl_strs.Add(NameVar("BpNetwork_Group", "Network_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("BpWiz", "BpWizard"));
  cvt->repl_strs.Add(NameVar("Layer", "BpLayer"));
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

void bp_module_init() {
  bp_converter_init();          // configure converter
}

// module initialization
InitProcRegistrar mod_init_bp(bp_module_init);

//////////////////////////
//      Con, Spec       //
//////////////////////////

void BpConSpec::Initialize() {
  min_obj_type = &TA_BpCon;
  Defaults_init();
}

void BpConSpec::Defaults_init() {
  lrate = .2f;
  cur_lrate = .2f;
  lrate_sched.interpolate = false;
  momentum_type = BEFORE_LRATE;
  momentum = 0.0f;
  momentum_c = .1f;
  decay = 0.0f;
  decay_fun = NULL;
}

void BpConSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_BpConSpec); // allow any of this basic type here
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(lrate_sched, this);
}

void BpConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lrate_sched.UpdateAfterEdit_NoGui();
  momentum_c = 1.0f - momentum;
}

void BpConSpec::SetCurLrate(int epoch) {
  cur_lrate = lrate * lrate_sched.GetVal(epoch);
}

void BpConSpec::LogLrateSched(int epcs_per_step, float n_steps) {
  float log_ns[3] = {1, .5f, .2f};

  lrate_sched.SetSize((int)n_steps);
  for(int i=0;i<n_steps;i++) {
    lrate_sched[i]->start_ctr = i * epcs_per_step;
    lrate_sched[i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
  }
  UpdateAfterEdit();            // needed to update the sub guys
}

void Bp_Simple_WtDecay(BpConSpec* spec, float& wt, float& dwt) {
  dwt -= spec->decay * wt;
}

void Bp_WtElim_WtDecay(BpConSpec* spec, float& wt, float& dwt) {
  const float wt_sq = wt * wt;
  float denom = (1.0f + wt_sq);
  dwt -= spec->decay * ((2.0f * wt_sq) / (denom * denom));
}


//////////////////////////
//      Unit, Spec      //
//////////////////////////

void BpUnitSpec::Initialize() {
  min_obj_type = &TA_BpUnit;
  bias_spec.SetBaseType(&TA_BpConSpec);
  err_tol = 0.0f;
  err_fun = Bp_Squared_Error;
}

void BpUnitSpec::Defaults_init() {
  err_tol = 0.05f;
  sse_tol = 0.5f;
  err_fun = Bp_CrossEnt_Error;
}

void BpUnitSpec::InitLinks() {
  bias_spec.type = &TA_BpConSpec;
  inherited::InitLinks();
  children.SetBaseType(&TA_BpUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(sig, this);
}

void BpUnitSpec::SetCurLrate(BpUnitVars* uv, BpNetwork* net, int thr_no) {
  if(bias_spec) {
    ((BpConSpec*)bias_spec.SPtr())->SetCurLrate(net->epoch);
  }
  const int nrcg = net->ThrUnNRecvConGps(thr_no, uv->thr_un_idx);
  for(int g=0; g<nrcg; g++) {
    ConGroup* rgp = net->ThrUnRecvConGroup(thr_no, uv->thr_un_idx, g);
    if(rgp->NotActive()) continue;
    ((BpConSpec*)rgp->con_spec)->SetCurLrate(net->epoch);
  }
}

void BpUnitSpec::Init_Acts(UnitVars* u, Network* net, int thr_no) {
  inherited::Init_Acts(u, net, thr_no);
  BpUnitVars* bu = (BpUnitVars*)u;
  bu->err = bu->dEdA = bu->dEdNet = 0.0f;
}

void BpUnitSpec::Compute_Netin(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT) return; // don't compute on clamped inputs
  inherited::Compute_Netin(u, net, thr_no);
}

void BpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  // this does the sigmoid
  if(u->ext_flag & UnitVars::EXT)
    u->act = u->ext;
  else
    u->act = act_range.Project(sig.Eval(u->net));
}

void BpUnitSpec::Compute_Error(BpUnitVars* u, BpNetwork* net, int thr_no) {
  if(u->ext_flag & UnitVars::TARG) (*err_fun)(this, u);
}

void BpUnitSpec::Compute_dEdA(BpUnitVars* u, BpNetwork* net, int thr_no) {
  // note: this has to be done at unit level b/c of sequencing with dEdNet etc
  // don't compute to inputs by default
  if(net->bp_to_inputs || (u->ext_flag & UnitVars::EXT)) return;
  u->dEdA = 0.0f;
  u->err = 0.0f;
  const int nscg = net->ThrUnNSendConGps(thr_no, u->thr_un_idx);
  for(int g=0; g<nscg; g++) {
    ConGroup* sgp = net->ThrUnSendConGroup(thr_no, u->thr_un_idx, g);
    if(sgp->NotActive()) continue;
    u->dEdA += ((BpConSpec*)sgp->con_spec)->Compute_dEdA(sgp, net, thr_no);
  }
}

void BpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  u->dEdNet = u->dEdA * sig.gain * (u->act - act_range.min) *
    (act_range.max - u->act) * act_range.scale;
}

void BpUnitSpec::Compute_dWt(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)  return; // don't compute dwts for clamped units
  inherited::Compute_dWt(u, net, thr_no);
}

void BpUnitSpec::Compute_Weights(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)  return; // don't update for clamped units
  inherited::Compute_Weights(u, net, thr_no);
}

void BpUnitSpec::GraphActFun(DataTable* graph_data, float min, float max) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* netin = graph_data->FindMakeColName("Netin", idx, VT_FLOAT);
  DataCol* act = graph_data->FindMakeColName("Act", idx, VT_FLOAT);

  BpUnitVars un;
  float x;
  for(x = min; x <= max; x += .01f) {
    un.net = x;
    Compute_Act(&un, NULL, 0);
    graph_data->AddBlankRow();
    netin->SetValAsFloat(x, -1);
    act->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void Bp_Squared_Error(BpUnitSpec* spec, BpUnitVars* u) {
  float err = u->targ - u->act;
  if(fabs(err) < spec->err_tol) {
    u->err = 0.0f;
  }
  else {
    u->dEdA += err;
    u->err = err * err;
  }
}

void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnitVars* u) {
  float err = u->targ - u->act;
  if(fabs(err) < spec->err_tol) {
    u->err = 0.0f;
  }
  else {
    err /= (u->act - spec->act_range.min) * (spec->act_range.max - u->act)
      * spec->act_range.scale;
    float a = spec->sig.Clip(spec->act_range.Normalize(u->act));
    float t = spec->act_range.Normalize(u->targ);
    u->dEdA += err;
    u->err = (t * logf(a) + (1.0f - t) * logf(1.0f - a));
  }
}

//////////////////////////////////////////
//      Additional Con Types            //
//////////////////////////////////////////

void DeltaBarDeltaBpConSpec::Initialize() {
  min_obj_type = &TA_DeltaBarDeltaBpCon;
  Defaults_init();
}

void DeltaBarDeltaBpConSpec::Defaults_init() {
  lrate_incr = .1f;
  lrate_decr = .9f;
  act_lrate_incr = lrate * lrate_incr;
}

void DeltaBarDeltaBpConSpec::UpdateAfterEdit_impl() {
  BpConSpec::UpdateAfterEdit_impl();
  act_lrate_incr = lrate * lrate_incr;
}


//////////////////////////////////////////
//      Additional Unit Types           //
//////////////////////////////////////////

//////////////////////////
//      Context         //
//////////////////////////

void BpContextSpec::Initialize() {
  hysteresis = .3f;
  hysteresis_c = .7f;
  initial_act.var = 0;
  initial_act.mean = .5;
  variable = "act";
  unit_flags = UnitVars::NO_EXTERNAL;
}

void BpContextSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  hysteresis_c = 1.0f - hysteresis;
  var_md = TA_BpUnitVars.members.FindName(variable);
  if(var_md == NULL)
    taMisc::Error("BpContextSpec: could not find variable:",variable,"in BpUnitVarsp type");
}

/*obs bool BpContextSpec::CheckConfig(Unit* un, Layer* lay, TrialProcess* tp) {
  if(!BpUnitSpec::CheckConfig(un, lay, tp)) return false;
  if(var_md == NULL) {
    taMisc::Error("BpContextSpec: could not find variable:",variable,"in BpUnit type");
    return false;
  }
  RecvCons* recv_gp = (RecvCons*)un->recv.SafeGp(0); // first group
  if(recv_gp == NULL) {
    taMisc::Error("BpContextSpec: expecting one one-to-one projection from layer",
                   "did not find con group");
    return false;
  }
  Unit* hu = (Unit*)recv_gp->Un(0);
  if(hu == NULL) {
    taMisc::Error("BpContextSpec: expecting one one-to-one projection from layer",
                   "did not find unit");
    return false;
  }
  int myidx = lay->own_net->layers.FindLeaf(lay);
  int fmidx = lay->own_net->layers.FindLeaf(recv_gp->prjn->from);
  if(myidx < fmidx) {
    taMisc::Error("BpContextSpec: context layer:", lay->name, "must be AFTER layer it copies from:",
                  recv_gp->prjn->from->name, "in list of .layers");
    return false;
  }
  return true;
} */

void BpContextSpec::Init_Acts(UnitVars* u, Network* net, int thr_no) {
  BpUnitSpec::Init_Acts(u, net, thr_no);
  u->act = initial_act.Gen(thr_no);
}

void BpContextSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  // todo: add a checkconfig to ensure this congroup exists!
  ConGroup* rgp = net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, 0); // first group
  UnitVars* hu = (UnitVars*)rgp->Un(0, net);
  float* varptr = (float*)var_md->GetOff((void*)u);
  *varptr = hysteresis_c * hu->act + hysteresis * (*varptr);
  u->SetExtFlag(unit_flags);
}


//////////////////////////
//      Linear          //
//////////////////////////

void LinearBpUnitSpec::Initialize() {
  SetUnique("act_range", true);
  act_range.min = -1e20f;
  act_range.max = 1e20f;
}

void LinearBpUnitSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  if(err_fun == Bp_CrossEnt_Error) {
    taMisc::Error("LinearBpUnitSpec: Cross entropy error is incompatible with Linear Units!  I switched to Squared_Error for you.");
    SetUnique("err_fun", true);
    err_fun = Bp_Squared_Error;
  }
}

void LinearBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)
    u->act = act_range.Clip(u->ext);
  else
    u->act = act_range.Clip(u->net);
}

void LinearBpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  u->dEdNet = u->dEdA;          // that's pretty easy!
}

//////////////////////////
//      ThreshLinear    //
//////////////////////////

void ThreshLinBpUnitSpec::Initialize() {
  SetUnique("act_range", true);
  act_range.min = 0.0f;
  act_range.max = 1e6f;
}

void ThreshLinBpUnitSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  if(err_fun == Bp_CrossEnt_Error) {
    taMisc::Error("ThreshLinBpUnitSpec: Cross entropy error is incompatible with Linear Units!  I switched to Squared_Error for you.");
    SetUnique("err_fun", true);
    err_fun = Bp_Squared_Error;
  }
}

void ThreshLinBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT) {
    u->act = act_range.Clip(u->ext);
  }
  else {
    float del = u->net - sig.off;
    if(del < 0.0f) del = 0.0f;
    if(sig.gain_eq_1) {
      u->act = act_range.Clip(del);
    }
    else {
      u->act = act_range.Clip(sig.gain * del);
    }
  }
}

void ThreshLinBpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  // derivative is 1 in linear part, 0 elsewhere
  if(sig.gain_eq_1) {
    u->dEdNet = (u->net > sig.off) ? u->dEdA : 0.0f;
  }
  else {
    u->dEdNet = (u->net > sig.off) ? sig.gain * u->dEdA : 0.0f;
  }
}

//////////////////////////
//             XX1      //
//////////////////////////

void XX1BpUnitSpec::Initialize() {
  SetUnique("act_range", true);
  act_range.min = 0.0f;
  act_range.max = 1.0f;
}

void XX1BpUnitSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
}

void XX1BpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT) {
    u->act = act_range.Clip(u->ext);
  }
  else {
    float del = u->net - sig.off;
    if(del <= 0.0f) {
      u->act = 0.0f;
    }
    else {
      del *= sig.gain;
      u->act = act_range.Clip((del / (del + 1.0f)));
    }
  }
}

void XX1BpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  if(u->net <= sig.off) {
    u->dEdNet = 0.0f;
  }
  else {
    // derivative of x/x+1 = 1/(1+x)^2
    float del_p_1 = 1.0f + sig.gain * (u->net - sig.off);
    u->dEdNet = (sig.gain  * u->dEdA) / (del_p_1 * del_p_1);
  }
}



//////////////////
//    Noisy     //
//////////////////

void NoisyBpUnitSpec::Initialize() {
  noise.type = Random::GAUSSIAN;
  noise.var = .1f;
}

void NoisyBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)
    u->act = act_range.Clip(u->ext + noise.Gen(thr_no));
  else   // need to keep in SigmoidSpec clipped range!
    u->act = act_range.min + act_range.range *
      SigmoidSpec::Clip(sig.Eval(u->net) + noise.Gen(thr_no));
}


//////////////////////////
// Stochastic Unit Spec //
//////////////////////////


void StochasticBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  // this does the probabiltiy on sigmoid
  if(u->ext_flag & UnitVars::EXT)
    u->act = u->ext;
  else {
    float prob =  sig.Eval(u->net);
    float nw_act = (Random::ZeroOne(thr_no) < prob) ? 1.0f : 0.0f;
    u->act = act_range.Project(nw_act);
  }
}


//////////////////////////
//         RBF          //
//////////////////////////

void RBFBpUnitSpec::Initialize() {
  var = 1.0f;
  norm_const = 1.0f / sqrtf(2.0f * 3.14159265358979323846 * var);
  denom_const = 0.5f / var;
}

void RBFBpUnitSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  norm_const = 1.0f / sqrtf(2.0f * 3.14159265358979323846 * var);
  denom_const = 0.5f / var;
}

void RBFBpUnitSpec::Compute_Netin(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT) return; // don't compute on clamped inputs
  // do distance instead of net input
  u->net = 0.0f;
  // todo: this doesn't work anymore at this point -- need the exception case!
  // for(int g=0; g<u->recv.size; g++) {
  //   RecvCons* recv_gp = (RecvCons*)u->recv.FastEl(g);
  //   if(recv_gp->NotActive()) continue;
  //   u->net += recv_gp->Compute_Dist(u, net);
  // }
  if(bias_spec)
    u->net += u->bias_wt;
}

void RBFBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)
    u->act = u->ext;
  else
    u->act = norm_const * expf(-denom_const * u->net);
}

void RBFBpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  u->dEdNet = - u->dEdA * u->act * denom_const;
}


//////////////////////////
//       Bump           //
//////////////////////////

void BumpBpUnitSpec::Initialize() {
  mean = 0.0f;
  std_dev = 1.0f;
  std_dev_r = 1.0f;
}

void BumpBpUnitSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  std_dev_r = 1.0f / std_dev;
}


void BumpBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)
    u->act = u->ext;
  else {
    float val = std_dev_r * (u->net - mean);
    u->act = expf(- (val * val));
  }
}

void BumpBpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  // dadnet = a * 2 * (net - mean) / std_dev
  u->dEdNet = - u->dEdA * u->act * 2.0f * (u->net - mean) * std_dev_r;
}

//////////////////////////
//   Exp, SoftMax       //
//////////////////////////

void ExpBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  float netin = sig.gain * u->net;
  netin = MAX(netin, -50.0f);
  netin = MIN(netin, 50.0f);
  u->act = expf(netin);
}

void ExpBpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  u->dEdNet = u->dEdA * sig.gain * u->act;
}

void SoftMaxBpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  // todo: move this to a check config:
  // if((u->recv.size < 2) || (((RecvCons*)u->recv[0])->size == 0)
  //    || (((RecvCons*)u->recv[1])->size == 0)) {
  //   taMisc::Error("*** SoftMaxBpUnitSpec: expecting one one-to-one projection from",
  //                 "exponential units (in first projection) and from linear sum unit (in second), did not find these.");
  //   return;
  // }
  ConGroup* rgp0 = net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, 0); // first group
  ConGroup* rgp1 = net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, 1);
  BpUnitVars* exp_unit = (BpUnitVars*)rgp0->UnVars(0, net);
  BpUnitVars* sum_unit = (BpUnitVars*)rgp1->UnVars(0, net);

  float sum_act = sum_unit->act;
  if(sum_act < FLT_MIN)
    sum_act = FLT_MIN;
  u->act = exp_unit->act / sum_unit->act;
  if(u->act < FLT_MIN)
    u->act = FLT_MIN;
}

void SoftMaxBpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  // effectively linear
  u->dEdNet = u->dEdA;
}

//////////////////////////
//   BpLayer            //
//////////////////////////

void BpLayer::Initialize() {
  units.SetBaseType(&TA_BpUnit);
  unit_spec.SetBaseType(&TA_BpUnitSpec);
}

//////////////////////////
//   BpNetwork          //
//////////////////////////

void BpNetwork::Initialize() {
  layers.SetBaseType(&TA_BpLayer);
  unit_vars_type = &TA_BpUnitVars;
  con_group_type = &TA_ConGroup;
  bp_to_inputs = false;
  prev_epoch = -1;
}

void BpNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void BpNetwork::BuildNullUnit() {
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new BpUnit, this);
  }
}

void BpNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_BpCon;
  prjn->con_spec.SetBaseType(&TA_BpConSpec);
}

void BpNetwork::Init_Weights() {
  inherited::Init_Weights();
  prev_epoch = -1;
}

void BpNetwork::SetCurLrate_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    BpUnitVars* uv = (BpUnitVars*)ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    ((BpUnitSpec*)uv->unit_spec)->SetCurLrate(uv, this, thr_no);
  }
}

void BpNetwork::Compute_dEdA_dEdNet_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = nlay-1; li >= 0; li--) { // go in reverse order!
    Layer* lay = ActiveLayer(li);
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    bool has_targ = false;
    for(int ui = ust; ui < ued; ui++) {
      BpUnitVars* uv = (BpUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      ((BpUnitSpec*)uv->unit_spec)->Compute_dEdA_dEdNet(uv, this, thr_no);
    }
    threads.SyncSpin(thr_no);   // need to sync for each layer!
  }
}

void BpNetwork::Compute_Error() {
  // compute errors -- definitely not worth threading due to very limited units it run on
  FOREACH_ELEM_IN_GROUP(Layer, lay, layers) {
    if (!lay->lesioned() && (lay->ext_flag & UnitVars::TARG)) { // only compute err on targs
      FOREACH_ELEM_IN_GROUP(BpUnit, u, lay->units) {
	if(u->lesioned()) continue;
        u->dEdA() = 0.0f;           // must reset -- error is incremental!
        ((BpUnitSpec*)u->GetUnitSpec())->Compute_Error((BpUnitVars*)u->GetUnitVars(), this, 0);
      }
    }
  }
}

void BpNetwork::Compute_dWt_Thr(int thr_no) {
  if(threads.get_timing)
    net_timing[thr_no]->dwt.StartTimer(true); // reset

  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    ConGroup* rcg = ThrRecvConGroup(thr_no, i);
    if(rcg->NotActive()) continue;
    rcg->con_spec->Compute_dWt(rcg, this, thr_no);
  }
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Compute_dWt(uv, this, thr_no);
  }
  
  if(threads.get_timing)
    net_timing[thr_no]->dwt.EndIncrAvg();
}

void BpNetwork::Compute_Weights_Thr(int thr_no) {
  if(threads.get_timing)
    net_timing[thr_no]->wt.StartTimer(true); // reset

  const int nrcg = ThrNRecvConGps(thr_no);
  for(int i=0; i<nrcg; i++) {
    ConGroup* rcg = ThrRecvConGroup(thr_no, i);
    if(rcg->NotActive()) continue;
    rcg->con_spec->Compute_Weights(rcg, this, thr_no);
  }
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    UnitVars* uv = ThrUnitVars(thr_no, i);
    if(uv->lesioned()) continue;
    uv->unit_spec->Compute_Weights(uv, this, thr_no);
  }
  
  if(threads.get_timing)
    net_timing[thr_no]->wt.EndIncrAvg();
}

void BpNetwork::Trial_Run() {
  if(prev_epoch != epoch) {
    NET_THREAD_CALL(BpNetwork::SetCurLrate_Thr);
    prev_epoch = epoch;
  }
  DataUpdate(true);
  NET_THREAD_CALL(BpNetwork::Trial_Run_Thr);
  DataUpdate(false);
}

void BpNetwork::Trial_Run_Thr(int thr_no) {
  Compute_NetinAct_Thr(thr_no);

  threads.SyncSpin(thr_no);
  
  Compute_dEdA_dEdNet_Thr(thr_no);

  threads.SyncSpin(thr_no);

  // compute the weight err derivatives (only if not testing...)
  if(train_mode == TRAIN) {
    Compute_dWt_Thr(thr_no);
  }
  else {
    Compute_Error();            // for display purposes only..
  }
}


//////////////////////////
//   BpProject          //
//////////////////////////

void BpProject::Initialize() {
  networks.el_typ = &TA_BpNetwork;
  // wizards.el_typ = &TA_BpWizard;
}
