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

#ifdef CUDA_COMPILE
#include "bp_cuda.h"
#endif


TA_BASEFUNS_CTORS_DEFN(BpConSpec);
TA_BASEFUNS_CTORS_DEFN(NLXX1ActSpec);
TA_BASEFUNS_CTORS_DEFN(GaussActSpec);
TA_BASEFUNS_CTORS_DEFN(BpUnitSpec);
TA_BASEFUNS_CTORS_DEFN(BpUnit);
TA_BASEFUNS_CTORS_DEFN(HebbBpConSpec);
TA_BASEFUNS_CTORS_DEFN(BpLayer);
TA_BASEFUNS_CTORS_DEFN(BpProject);
TA_BASEFUNS_CTORS_DEFN(ErrScaleBpConSpec);
TA_BASEFUNS_CTORS_DEFN(DeltaBarDeltaBpConSpec);
TA_BASEFUNS_CTORS_DEFN(BpContextSpec);
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
  momentum = 0.0f;
  decay_type = NO_DECAY;
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

  if(taMisc::is_loading && decay_fun) {
    if(decay_fun == Bp_Simple_WtDecay)
      decay_type = SIMPLE_DECAY;
    else if(decay_fun == Bp_WtElim_WtDecay)
      decay_type = ELIMINATION;
    decay_fun = NULL;
  }

  if(momentum > 0.0f && decay_type != NO_DECAY) {
    if(decay_type == SIMPLE_DECAY)
      wt_updt = WU_MOMENT_SIMPLE;
    else
      wt_updt = WU_MOMENT_ELIM;
  }
  else if(momentum > 0.0f) {
    wt_updt = WU_MOMENT;
  }
  else if(decay_type == SIMPLE_DECAY) {
    wt_updt = WU_SIMPLE_DECAY;
  }
  else if(decay_type == ELIMINATION) {
    wt_updt = WU_ELIMINATION;
  }
  else {
    wt_updt = WU_DWT_ONLY;
  }
}

void BpConSpec::SetCurLrate(BpNetwork* net) {
  float prv_cur_lrate = cur_lrate;
  cur_lrate = lrate * lrate_sched.GetVal(net->epoch);
  if(cur_lrate != prv_cur_lrate) {
    net->lrate_updtd = true;
  }
}

void BpConSpec::LogLrateSched(int epcs_per_step, int n_steps) {
  float log_ns[3] = {1, .5f, .2f};

  lrate_sched.SetSize(n_steps);
  for(int i=0;i<n_steps;i++) {
    lrate_sched[i]->start_ctr = i * epcs_per_step;
    lrate_sched[i]->start_val = log_ns[i%3] * powf(10.0f,-(i/3));
    lrate_sched[i]->UpdateAfterEdit();
  }
  UpdateAfterEdit();            // needed to update the sub guys
}

// these are both OBSOLETE as of 8.0, 8/2016 -- delete at some future point (
void Bp_Simple_WtDecay(BpConSpec* spec, float& wt, float& dwt) {
  dwt -= spec->decay * wt;
}

void Bp_WtElim_WtDecay(BpConSpec* spec, float& wt, float& dwt) {
  float denom = (1.0f + wt * wt);
  dwt -= (spec->decay * wt) / (denom * denom); // note: before 8.0, had wt_sq in numerator!  bad error -- only decays for positive weights!
}


/////////////////////////////////////////////////////////////////////
//              Unit, Spec  

void NLXX1ActSpec::Initialize() {
  Defaults_init();
}

void NLXX1ActSpec::Defaults_init() {
  ramp_start = -3.0f;
  ramp_max = 0.1f;
  ramp_gain = ramp_max / ramp_start;
}  

void NLXX1ActSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ramp_gain = ramp_max / ramp_start;
}

void GaussActSpec::Initialize() {
  Defaults_init();
}

void GaussActSpec::Defaults_init() {
  mean = 0.0f;
  std_dev = 1.0f;
  std_dev_r = 1.0f;
}

void GaussActSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  std_dev_r = 1.0f / std_dev;
}



const float BpUnitSpec::SIGMOID_MAX_VAL = 0.999999f;
const float BpUnitSpec::SIGMOID_MIN_VAL = 0.000001f;
const float BpUnitSpec::SIGMOID_MAX_NET = 13.81551f;

void BpUnitSpec::Initialize() {
  min_obj_type = &TA_BpUnit;
  bias_spec.SetBaseType(&TA_BpConSpec);

  act_fun = SIGMOID;
  error_fun = SQUARED_ERR;
  save_err = false;
  noise_type = NO_NOISE;

  act_range.min = -100.0f;       // general ranges for linear -- keep it bounded!
  act_range.max = 100.0f;

  Defaults_init();
  
  err_fun = NULL;               // obsolete
}

void BpUnitSpec::Defaults_init() {
  err_tol = 0.05f;
  sse_tol = 0.5f;
}

void BpUnitSpec::InitLinks() {
  bias_spec.type = &TA_BpConSpec;
  inherited::InitLinks();
  children.SetBaseType(&TA_BpUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(nlxx1, this);
  taBase::Own(gauss, this);
  taBase::Own(noise, this);
}

void BpUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(taMisc::is_loading && err_fun) {
    if(err_fun == Bp_Squared_Error)
      error_fun = SQUARED_ERR;
    else if(err_fun == Bp_CrossEnt_Error)
      error_fun = CROSS_ENTROPY;
    err_fun = NULL;
  }
  
  // update activation ranges from fun type
  switch(act_fun) {
  case SIGMOID:
  case NLXX1:
  case BINARY:
  case SOFTMAX:
    act_range.min = 0.0f; act_range.max = 1.0f;
    break;
  case TANH:
    act_range.min = -1.0f; act_range.max = 1.0f;
    break;
  case RELU:
    act_range.min = 0.0f;
    break;
  default:
    break;
  }
  act_range.UpdateAfterEdit_NoGui();

  if(error_fun == CROSS_ENTROPY) {
    if(act_fun == LINEAR || act_fun == RELU) {
      taMisc::Error("rBpUnitSpec: Cross entropy error is incompatible with Linear or ReLu Units!  I switched to Squared_Error for you.");
      SetUnique("error_fun", true);
      error_fun = SQUARED_ERR;
    }
  }
}


void BpUnitSpec::SetCurLrate(BpUnitVars* uv, BpNetwork* net, int thr_no) {
  if(bias_spec) {
    ((BpConSpec*)bias_spec.SPtr())->SetCurLrate(net);
  }
  const int nrcg = net->ThrUnNRecvConGps(thr_no, uv->thr_un_idx);
  for(int g=0; g<nrcg; g++) {
    ConGroup* rgp = net->ThrUnRecvConGroup(thr_no, uv->thr_un_idx, g);
    if(rgp->NotActive()) continue;
    ((BpConSpec*)rgp->con_spec)->SetCurLrate(net);
  }
}

void BpUnitSpec::Init_Acts(UnitVars* u, Network* net, int thr_no) {
  inherited::Init_Acts(u, net, thr_no);
  BpUnitVars* bu = (BpUnitVars*)u;
  bu->err = bu->dEdA = bu->dEdNet = 0.0f;
  u->ClearExtFlag(UnitVars::UN_FLAG_1);
}

void BpUnitSpec::Compute_MaxPoolNetin(BpUnitVars* u, Network* net, int thr_no) {
  const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
  if(TestError(nrcg != 1, "Compute_MaxPoolNetin", "must have exactly 1 recv prjn for MAX_POOL units")) {
    
    return;
  }
  ConGroup* cg = net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, 0);
  float max_act = cg->UnVars(0,net)->act;
  int max_i = cg->UnVars(0,net)->flat_idx;
  for(int i=1; i<cg->size; i++) {
    float su_act = cg->UnVars(i,net)->act;
    if(su_act > max_act) {
      max_act = su_act;
      max_i = cg->UnVars(i,net)->flat_idx;
    }
  }
  u->net = max_act;
  u->misc1 = (float)max_i;
}

void BpUnitSpec::Compute_Netin(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT) return; // don't compute on clamped inputs
  if(act_fun == MAX_POOL) {
    Compute_MaxPoolNetin((BpUnitVars*)u, net, thr_no);
  }
  else if(act_fun == RBF) {
    float new_net = 0.0f;
    const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
    for(int g=0; g<nrcg; g++) {
      ConGroup* rgp = net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, g);
      if(rgp->NotActive()) continue;
      new_net += rgp->con_spec->Compute_Dist(rgp, net, thr_no);
    }
    u->net = new_net;
  }
  else {
    inherited::Compute_Netin(u, net, thr_no);

    if(act_fun == SOFTMAX) {
      float expnet = u->net;
      expnet = fmaxf(expnet, -50.0f);
      expnet = fminf(expnet, 50.0f);
      ((BpUnitVars*)u)->misc1 = expf(expnet);
    }
  }
}

void BpUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(noise_type == NETIN_NOISE) {
    u->net += noise.Gen(thr_no);
  }

  if(u->ext_flag & UnitVars::EXT) {
    u->act = u->ext;
  }
  else {
    if(act_fun == SOFTMAX) {
      BpUnitVars* bu = (BpUnitVars*)u;
      u->act = bu->misc1 / bu->err; // err temporarily contains sum of misc1 = exp(net) terms
    }
    else {
      u->act = ActFromNetin(u->net, thr_no);
    }
  }

  if(noise_type == ACT_NOISE) {
    u->act += noise.Gen(thr_no);
    act_range.Clip(u->act);
  }
  else if(noise_type == DROPOUT) {
    if(Random::BoolProb(noise.mean, thr_no)) {
      u->act = 0.0f;            // blank it
      u->SetExtFlag(UnitVars::UN_FLAG_1); // dropout flag
    }
    else {
      u->ClearExtFlag(UnitVars::UN_FLAG_1);
    }      
  }
}

void BpUnitSpec::Compute_Error(BpUnitVars* u, BpNetwork* net, int thr_no) {
  if(!u->ext_flag & UnitVars::TARG) return;
  if(u->ext_flag & UnitVars::UN_FLAG_1) return; // dropout flag

  float err = u->targ - u->act;
  if(fabsf(err) < err_tol) {
    if(save_err)
      u->err = 0.0f;
  }
  else {
    if(error_fun == SQUARED_ERR || act_fun == SOFTMAX) { // softmax always uses this!
      u->dEdA += err;
      if(save_err)
        u->err = err * err;
    }
    else {                        // CROSS_ENTROPY
      err /= (u->act - act_range.min) * (act_range.max - u->act) * act_range.scale;
      u->dEdA += err;
      if(save_err) {
        float a = ClipSigAct(act_range.Normalize(u->act));
        float t = act_range.Normalize(u->targ);
        u->err = (t * logf(a) + (1.0f - t) * logf(1.0f - a));
      }
    }
  }
}

void BpUnitSpec::Compute_dEdA(BpUnitVars* u, BpNetwork* net, int thr_no) {
  // note: this has to be done at unit level b/c of sequencing with dEdNet etc
  // don't compute to inputs by default
  u->dEdA = 0.0f;
  u->err = 0.0f;
  if((u->ext_flag & UnitVars::EXT) && !net->bp_to_inputs) return;
  if(u->ext_flag & UnitVars::UN_FLAG_1) return; // dropout flag
  const int nscg = net->ThrUnNSendConGps(thr_no, u->thr_un_idx);
  for(int g=0; g<nscg; g++) {
    ConGroup* sgp = net->ThrUnSendConGroup(thr_no, u->thr_un_idx, g);
    if(sgp->NotActive()) continue;
    Layer* tolay = sgp->prjn->layer;
    BpUnitSpec* tous = (BpUnitSpec*)tolay->GetUnitSpec();
    if(tous->act_fun == BpUnitSpec::MAX_POOL) {
      for(int i=0; i<sgp->size; i++) {
        int su_idx = (int)((BpUnitVars*)sgp->UnVars(i,net))->misc1;
        if(su_idx == u->flat_idx) { // it likes us!
          u->dEdA += ((BpUnitVars*)sgp->UnVars(i,net))->dEdNet;
        }
      }
    }
    else {
      u->dEdA += ((BpConSpec*)sgp->con_spec)->Compute_dEdA(sgp, net, thr_no);
    }
  }
}

void BpUnitSpec::Compute_dEdNet(BpUnitVars* u, BpNetwork* net, int thr_no) {
  if((u->ext_flag & UnitVars::EXT) && !net->bp_to_inputs) {
    u->dEdNet = 0.0f;
    return;
  }
  if(u->ext_flag & UnitVars::UN_FLAG_1) { // dropout flag
    u->dEdNet = 0.0f;
    return;
  }
  u->dEdNet = u->dEdA * ActDeriv(u->net, u->act, thr_no);
}

void BpUnitSpec::Compute_dWt(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)  return; // don't compute dwts for clamped units
  if(u->ext_flag & UnitVars::UN_FLAG_1) return; // dropout flag
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
  DataCol* deriv = graph_data->FindMakeColName("Deriv", idx, VT_FLOAT);

  float x;
  for(x = min; x <= max; x += .01f) {
    float av = ActFromNetin(x, 0);
    float dv = ActDeriv(x, av, 0);
    graph_data->AddBlankRow();
    netin->SetValAsFloat(x, -1);
    act->SetValAsFloat(av, -1);
    deriv->SetValAsFloat(dv, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

// NOTE: OBSOLETE as of 8.0, 8/2016
void Bp_Squared_Error(BpUnitSpec* spec, BpUnitVars* u) {
  float err = u->targ - u->act;
  if(fabsf(err) < spec->err_tol) {
    u->err = 0.0f;
  }
  else {
    u->dEdA += err;
    u->err = err * err;
  }
}

void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnitVars* u) {
  float err = u->targ - u->act;
  if(fabsf(err) < spec->err_tol) {
    u->err = 0.0f;
  }
  else {
    err /= (u->act - spec->act_range.min) * (spec->act_range.max - u->act)
      * spec->act_range.scale;
    float a = spec->ClipSigAct(spec->act_range.Normalize(u->act));
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
  inherited::UpdateAfterEdit_impl();
  hysteresis_c = 1.0f - hysteresis;
  var_md = TA_BpUnitVars.members.FindName(variable);
  if(var_md == NULL)
    taMisc::Error("BpContextSpec: could not find variable:",variable,"in BpUnitVarsp type");
}

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
  lrate_updtd = false;
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

void BpNetwork::Compute_NetinAct_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = 0; li < nlay; li++) {
    Layer* lay = ActiveLayer(li);
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    bool has_softmax = false;
    for(int ui = ust; ui < ued; ui++) {
      UnitVars* uv = ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      BpUnitSpec* us = (BpUnitSpec*)uv->unit_spec;
      if(us->act_fun == BpUnitSpec::SOFTMAX) {
        if(!has_softmax) has_softmax = true;
        us->Compute_Netin(uv, this, thr_no);
      }
      else {
        us->Compute_NetinAct(uv, this, thr_no);
      }
    }
    threads.SyncSpin(thr_no, li % 3);   // need to sync for each layer!

    if(has_softmax) {           //
      if(thr_no == 0) {         // main thread has to do everything
        float sum = 0.0f;
        FOREACH_ELEM_IN_GROUP(BpUnit, u, lay->units) {
          sum += u->misc1();
        }
        FOREACH_ELEM_IN_GROUP(BpUnit, u, lay->units) {
          BpUnitVars* uv = (BpUnitVars*)u->GetUnitVars();
          BpUnitSpec* us = (BpUnitSpec*)uv->unit_spec;
          uv->err = sum;        // temp store
          us->Compute_Act(uv, this, thr_no);
        }
      }
      threads.SyncSpin(thr_no, (li-1) % 3);   // sync again!
    }
  }
}

void BpNetwork::Compute_dEdA_dEdNet_Thr(int thr_no) {
  const int nlay = n_layers_built;
  for(int li = nlay-1; li >= 0; li--) { // go in reverse order!
    Layer* lay = ActiveLayer(li);
    const int ust = ThrLayUnStart(thr_no, li);
    const int ued = ThrLayUnEnd(thr_no, li);
    for(int ui = ust; ui < ued; ui++) {
      BpUnitVars* uv = (BpUnitVars*)ThrUnitVars(thr_no, ui);
      if(uv->lesioned()) continue;
      ((BpUnitSpec*)uv->unit_spec)->Compute_dEdA_dEdNet(uv, this, thr_no);
    }
    threads.SyncSpin(thr_no, li % 3);   // need to sync for each layer!
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

#ifdef CUDA_COMPILE
  if(cuda.on) {
    Cuda_Compute_Weights();
    if(threads.get_timing)
      net_timing[thr_no]->wt.EndIncrAvg();
    return;
  }
#endif
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
    lrate_updtd = false;
    NET_THREAD_CALL(BpNetwork::SetCurLrate_Thr);
    prev_epoch = epoch;
    if(lrate_updtd) {
      taMisc::Info("cur_lrate updated at epoch:", String(epoch));
      Cuda_UpdateSpecs();
    }
  }
#ifdef CUDA_COMPILE
  if(cuda.on) {
    DataUpdate(true);
    Cuda_Trial_Run();
    DataUpdate(false);
    return;
  }
#endif
  DataUpdate(true);
  NET_THREAD_CALL(BpNetwork::Trial_Run_Thr);
  DataUpdate(false);
}

void BpNetwork::Trial_Run_Thr(int thr_no) {
  Compute_NetinAct_Thr(thr_no);

  threads.SyncSpin(thr_no, 0);
  
  if(train_mode == TRAIN) {
    Compute_dEdA_dEdNet_Thr(thr_no);
    threads.SyncSpin(thr_no, 1);
    Compute_dWt_Thr(thr_no);
  }
  else {
    Compute_Error();            // for display, stats purposes only..
  }
}



////////////////////////////////////////////////////////
//              CUDA  Code

#ifdef CUDA_COMPILE

bool BpNetwork::Cuda_MakeCudaNet() {
  if(cuda_net) return false;
  cuda_net = new Bp_cuda;  // note: this will leak memory..
  return true;
}

void BpNetwork::Cuda_CopyUnitSpec(void* cuda_us, const UnitSpec* src) {
  BpUnitSpec_cuda* cus = (BpUnitSpec_cuda*)cuda_us;
  BpUnitSpec* us = (BpUnitSpec*)src;
  cus->act_fun = (BpUnitSpec_cuda::BpActFun)us->act_fun;
  cus->error_fun = (BpUnitSpec_cuda::BpErrFun)us->error_fun;
  cus->err_tol = us->err_tol;
}

void BpNetwork::Cuda_CopyConSpec(void* cuda_cs, const ConSpec* src) {
  BpConSpec_cuda* ccs = (BpConSpec_cuda*)cuda_cs;
  BpConSpec* cs = (BpConSpec*)src;
  ccs->cur_lrate = cs->cur_lrate;
  ccs->momentum = cs->momentum;
  ccs->decay_type = (BpConSpec_cuda::DecayType)cs->decay_type;
  ccs->decay = cs->decay;
  ccs->wt_updt = (BpConSpec_cuda::WtUpdtType)cs->wt_updt;
}

void BpNetwork::Cuda_Trial_Run() {
  Cuda_Compute_NetinAct();
  Cuda_Compute_dEdA_dEdNet();
  Cuda_Compute_dWt();
  if(cuda.sync_units) {
    Cuda_UnitVarsToHost();
  }
  else {
    cuda_net->TargUnitsToHost(true);       // send output layer data back to host for stats..
  }
}

void BpNetwork::Cuda_Compute_NetinAct() {
  ((Bp_cuda*)cuda_net)->Compute_NetinAct();
}

void BpNetwork::Cuda_Compute_dEdA_dEdNet() {
  ((Bp_cuda*)cuda_net)->Compute_dEdA_dEdNet();
}

void BpNetwork::Cuda_Compute_dWt() {
  if(cuda.timers_on)
    net_timing[0]->dwt.StartTimer(true); // reset

  ((Bp_cuda*)cuda_net)->Compute_dWt(true); // sync

  if(cuda.timers_on)
    net_timing[0]->dwt.EndIncrAvg();
}

void BpNetwork::Cuda_Compute_Weights() {
  ((Bp_cuda*)cuda_net)->Compute_Weights(true); // sync
  if(cuda.sync_cons) {
    Cuda_ConStateToHost();
    cuda_net->TargUnitsToHost(true);       // send output layer data back to host for stats..
    // important to send target units back to host so that their bias weights can update!!      
  }
}


#endif // CUDA_COMPILE


//////////////////////////
//   BpProject          //
//////////////////////////

void BpProject::Initialize() {
  networks.el_typ = &TA_BpNetwork;
  // wizards.el_typ = &TA_BpWizard;
}
