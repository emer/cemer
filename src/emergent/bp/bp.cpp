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

#include <math.h>
#include <limits.h>
#include <float.h>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

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
  ta_Init_bp();			// initialize types 
  bp_converter_init();		// configure converter
}

// module initialization
InitProcRegistrar mod_init_bp(bp_module_init);

//////////////////////////
//  	Con, Spec	//
//////////////////////////

void BpConSpec::Initialize() {
  min_obj_type = &TA_BpCon;
  lrate = .25f;
  cur_lrate = .25f;
  lrate_sched.interpolate = false;
  momentum_type = BEFORE_LRATE;
  momentum = .9f;
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

void BpRecvCons::Initialize() {
  SetConType(&TA_BpCon);
}

void BpSendCons::Initialize() {
  SetConType(&TA_BpCon);
}

void Bp_Simple_WtDecay(BpConSpec* spec, BpCon* cn, BpUnit*, BpUnit*) {
  cn->dwt -= spec->decay * cn->wt;
}

void Bp_WtElim_WtDecay(BpConSpec* spec, BpCon* cn, BpUnit*, BpUnit*) {
  float denom = (1.0f + (cn->wt * cn->wt));
  cn->dwt -= spec->decay * ((2.0f * cn->wt * cn->wt) / (denom * denom));
}


//////////////////////////
//  	Unit, Spec	//
//////////////////////////

void BpUnitSpec::Initialize() {
  min_obj_type = &TA_BpUnit;
  bias_con_type = &TA_BpCon;
  bias_spec.SetBaseType(&TA_BpConSpec);
  err_tol = 0.0f;
  err_fun = Bp_Squared_Error;
}

void BpUnitSpec::InitLinks() {
  bias_spec.type = &TA_BpConSpec;
  inherited::InitLinks();
  children.SetBaseType(&TA_BpUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(sig, this);
}

void BpUnitSpec::SetCurLrate(BpUnit* u, int epoch) {
  ((BpConSpec*)bias_spec.SPtr())->SetCurLrate(epoch);
  for(int g=0; g<u->recv.size; g++) {
    BpRecvCons* recv_gp = (BpRecvCons*)u->recv.FastEl(g);
    recv_gp->SetCurLrate(epoch);
  }
}

void BpUnitSpec::Init_Acts(Unit* u, Network* net) {
  UnitSpec::Init_Acts(u, net);
  BpUnit* bu = (BpUnit*)u;
  bu->err = bu->dEdA = bu->dEdNet = 0.0f;
}

void BpUnitSpec::Compute_Netin(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT) return; // don't compute on clamped inputs
  inherited::Compute_Netin(u, net, thread_no);
}

void BpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  // this does the sigmoid
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = act_range.Project(sig.Eval(u->net));
}

void BpUnitSpec::Compute_Error(BpUnit* u, BpNetwork* net, int thread_no) {
  if(u->ext_flag & Unit::TARG) (*err_fun)(this, u);
}

void BpUnitSpec::Compute_dEdA(BpUnit* u, BpNetwork* net, int thread_no) {
  // don't compute to inputs by default
  if(net->bp_to_inputs || (u->ext_flag & Unit::EXT)) return;
  u->dEdA = 0.0f;
  u->err = 0.0f;
  for(int g=0; g<u->send.size; g++) {
    BpSendCons* send_gp = (BpSendCons*)u->send.FastEl(g);
    if(!send_gp->prjn->layer->lesioned())
      u->dEdA += send_gp->Compute_dEdA(u);
  }
}

void BpUnitSpec::Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no) {
  u->dEdNet = u->dEdA * sig.gain * (u->act - act_range.min) *
    (act_range.max - u->act) * act_range.scale;
}

void BpUnitSpec::Compute_dWt(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)  return; // don't compute dwts for clamped units
  UnitSpec::Compute_dWt(u, net, thread_no);
  ((BpConSpec*)bias_spec.SPtr())->B_Compute_dWt((BpCon*)u->bias.OwnCn(0), (BpUnit*)u);
}

void BpUnitSpec::Compute_Weights(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)  return; // don't update for clamped units
  UnitSpec::Compute_Weights(u, net, thread_no);
  ((BpConSpec*)bias_spec.SPtr())->B_Compute_Weights((BpCon*)u->bias.OwnCn(0), (BpUnit*)u);
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

  BpUnit un;
  float x;
  for(x = min; x <= max; x += .01f) {
    un.net = x;
    Compute_Act(&un, NULL, -1);
    graph_data->AddBlankRow();
    netin->SetValAsFloat(x, -1);
    act->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void BpUnit::Initialize() {
  err = dEdA = dEdNet = 0.0f;
}

void Bp_Squared_Error(BpUnitSpec* spec, BpUnit* u) {
  float err = u->targ - u->act;
  if(fabs(err) < spec->err_tol) {
    u->err = 0.0f;
  }
  else {
    u->dEdA += err;
    u->err = err * err;
  }
}

void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnit* u) {
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


void BpUnit::Copy_(const BpUnit& cp) {
  err = cp.err;
  dEdA = cp.dEdA;
  dEdNet = cp.dEdNet;
}

//////////////////////////////////////////
//	Additional Con Types		//
//////////////////////////////////////////

void DeltaBarDeltaBpConSpec::Initialize() {
  min_obj_type = &TA_DeltaBarDeltaBpCon;
  lrate_incr = .1f;
  lrate_decr = .9f;
  act_lrate_incr = lrate * lrate_incr;
}

void DeltaBarDeltaBpConSpec::UpdateAfterEdit_impl() {
  BpConSpec::UpdateAfterEdit_impl();
  act_lrate_incr = lrate * lrate_incr;
}


//////////////////////////////////////////
//	Additional Unit Types		//
//////////////////////////////////////////

//////////////////////////
//  	Context		//
//////////////////////////

void BpContextSpec::Initialize() {
  hysteresis = .3f;
  hysteresis_c = .7f;
  initial_act.var = 0;
  initial_act.mean = .5;
  variable = "act";
  unit_flags = Unit::NO_EXTERNAL;
}

void BpContextSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  hysteresis_c = 1.0f - hysteresis;
  var_md = TA_BpUnit.members.FindName(variable);
  if(var_md == NULL)
    taMisc::Error("BpContextSpec: could not find variable:",variable,"in BpUnit type");
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

void BpContextSpec::Init_Acts(Unit* u, Network* net) {
  BpUnitSpec::Init_Acts(u, net);
  u->act = initial_act.Gen();
}

void BpContextSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
// todo: add a checkconfig to ensure this congroup exists!
  RecvCons* recv_gp = (RecvCons*)u->recv.SafeEl(0); // first group
  Unit* hu = (Unit*)recv_gp->Un(0);
  float* varptr = (float*)var_md->GetOff((void*)u);
  *varptr = hysteresis_c * hu->act + hysteresis * (*varptr);
  u->SetExtFlag(unit_flags);
}


//////////////////////////
//  	Linear		//
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

void LinearBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)
    u->act = act_range.Clip(u->ext);
  else
    u->act = act_range.Clip(u->net);
}

void LinearBpUnitSpec::Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no) {
  u->dEdNet = u->dEdA;		// that's pretty easy!
}

//////////////////////////
//  	ThreshLinear	//
//////////////////////////

void ThreshLinBpUnitSpec::Initialize() {
  SetUnique("act_range", true);
  act_range.min = -1e20f;
  act_range.max = 1e20f;
  threshold = 0.0f;
}

void ThreshLinBpUnitSpec::UpdateAfterEdit_impl() {
  BpUnitSpec::UpdateAfterEdit_impl();
  if(err_fun == Bp_CrossEnt_Error) {
    taMisc::Error("ThreshLinBpUnitSpec: Cross entropy error is incompatible with Linear Units!  I switched to Squared_Error for you.");
    SetUnique("err_fun", true);
    err_fun = Bp_Squared_Error;
  }
}

void ThreshLinBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)
    u->act = act_range.Clip(u->ext);
  else
    u->act = act_range.Clip((u->net > threshold) ? (u->net - threshold) : 0.0f);
}

void ThreshLinBpUnitSpec::Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no) {
  // derivative is 1 in linear part, 0 elsewhere
  u->dEdNet = (u->net > threshold) ? u->dEdA : 0.0f;
}



//////////////////
//    Noisy	//
//////////////////

void NoisyBpUnitSpec::Initialize() {
  noise.type = Random::GAUSSIAN;
  noise.var = .1f;
}

void NoisyBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)
    u->act = act_range.Clip(u->ext + noise.Gen());
  else   // need to keep in SigmoidSpec clipped range!
    u->act = act_range.min + act_range.range *
      SigmoidSpec::Clip(sig.Eval(u->net) + noise.Gen());
}


//////////////////////////
// Stochastic Unit Spec //
//////////////////////////


void StochasticBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  // this does the probabiltiy on sigmoid
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else {
    float prob =  sig.Eval(u->net);
    float nw_act = (Random::ZeroOne() < prob) ? 1.0f : 0.0f;
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

void RBFBpUnitSpec::Compute_Netin(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT) return; // don't compute on clamped inputs
  // do distance instead of net input
  u->net = 0.0f;
  for(int g=0; g<u->recv.size; g++) {
    RecvCons* recv_gp = (RecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->size) continue;
    u->net += recv_gp->Compute_Dist(u);
  }
  if(u->bias.size)
    u->net += u->bias.OwnCn(0)->wt;
}

void RBFBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = norm_const * expf(-denom_const * u->net);
}

void RBFBpUnitSpec::Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no) {
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


void BumpBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else {
    float val = std_dev_r * (u->net - mean);
    u->act = expf(- (val * val));
  }
}

void BumpBpUnitSpec::Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no) {
  // dadnet = a * 2 * (net - mean) / std_dev
  u->dEdNet = - u->dEdA * u->act * 2.0f * (u->net - mean) * std_dev_r;
}

//////////////////////////
//   Exp, SoftMax       //
//////////////////////////

void ExpBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  float netin = sig.gain * u->net;
  netin = MAX(netin, -50.0f);
  netin = MIN(netin, 50.0f);
  u->act = expf(netin);
}

void ExpBpUnitSpec::Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no) {
  u->dEdNet = u->dEdA * sig.gain * u->act;
}

void SoftMaxBpUnitSpec::Compute_Act(Unit* u, Network* net, int thread_no) {
  // todo: move this to a check config:
  if((u->recv.size < 2) || (((RecvCons*)u->recv[0])->size == 0)
     || (((RecvCons*)u->recv[1])->size == 0)) {
    taMisc::Error("*** SoftMaxBpUnitSpec: expecting one one-to-one projection from",
		  "exponential units (in first projection) and from linear sum unit (in second), did not find these.");
    return;
  }
  BpUnit* exp_unit = (BpUnit*)((RecvCons*)u->recv[0])->Un(0);
  BpUnit* sum_unit = (BpUnit*)((RecvCons*)u->recv[1])->Un(0);

  float sum_act = sum_unit->act;
  if(sum_act < FLT_MIN)
    sum_act = FLT_MIN;
  u->act = exp_unit->act / sum_unit->act;
  if(u->act < FLT_MIN)
    u->act = FLT_MIN;
}

void SoftMaxBpUnitSpec::Compute_dEdNet(BpUnit* u, BpNetwork* net, int thread_no) {
  // effectively linear
  u->dEdNet = u->dEdA;
}

//////////////////////////
//   BpLayer	        //
//////////////////////////

void BpLayer::Initialize() {
  units.SetBaseType(&TA_BpUnit);
  unit_spec.SetBaseType(&TA_BpUnitSpec);
}

//////////////////////////
//   BpNetwork	        //
//////////////////////////

void BpNetwork::Initialize() {
  layers.SetBaseType(&TA_BpLayer);
  bp_to_inputs = false;
}

void BpNetwork::UpdateAfterEdit_impl() {
  if(TestWarning(dmem_nprocs > 1,
	      "Note: you cannot currently use dmem (MPI) to compute in Bp networks,",
	      "due to incompatibilities with the new threading mechanism.",
		 "dmem_nprocs was set back to 1 for you.")) {
    dmem_nprocs = 1;
  }
  inherited::UpdateAfterEdit_impl();
}

void BpNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_BpCon;
  prjn->recvcons_type = &TA_BpRecvCons;
  prjn->sendcons_type = &TA_BpSendCons;
  prjn->con_spec.SetBaseType(&TA_BpConSpec);
}

void BpNetwork::SetCurLrate() {
  Layer* layer;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, layer, layers., l_itr) {
    if (layer->lesioned())	continue;
    BpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(BpUnit, u, layer->units., u_itr)
      u->SetCurLrate(epoch);
  }
}

void BpNetwork::Compute_NetinAct() {
  ThreadUnitCall un_call(&Unit::Compute_NetinAct);
  threads.Run(&un_call, 0.7f, false, true); // backwards = false, layer_sync=true
}

void BpNetwork::Compute_dEdA_dEdNet() {
  ThreadUnitCall un_call((ThreadUnitMethod)(BpUnitMethod)&BpUnit::Compute_dEdA_dEdNet);
  threads.Run(&un_call, 0.9f, true, true); // backwards = true, layer_sync=true

// #ifdef DMEM_COMPILE
//     // first compute dEdA from connections and share it
//     FOR_ITR_EL(BpUnit, u, lay->units., u_itr)
//       u->Compute_dEdA();
//     lay->dmem_share_units.Aggregate(3, MPI_SUM);

//     // then compute error to add to dEdA, and dEdNet
//     FOR_ITR_EL(BpUnit, u, lay->units., u_itr) {
//       u->Compute_Error();
//       u->Compute_dEdNet();
//     }
// #else
}

void BpNetwork::Compute_Error() {
  // compute errors -- definitely not worth threading due to very limited units it run on
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, layers., l_itr) {
    if (lay->lesioned() || !(lay->ext_flag & Unit::TARG)) // only compute err on targs
      continue;

    BpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(BpUnit, u, lay->units., u_itr) {
      u->dEdA = 0.0f;		// must reset -- error is incremental!
      u->Compute_Error(this);
    }
  }
}

void BpNetwork::Compute_dWt() {
  ThreadUnitCall un_call(&Unit::Compute_dWt);
  threads.Run(&un_call, 0.8f);
}

void BpNetwork::Compute_Weights_impl() {
  ThreadUnitCall un_call(&Unit::Compute_Weights);
  threads.Run(&un_call, 1.0f);
}

void BpNetwork::Trial_Run() {
  DataUpdate(true);
  SetCurLrate();

  Compute_NetinAct();
  Compute_dEdA_dEdNet();

  // compute the weight err derivatives (only if not testing...)
  if(train_mode == TRAIN) {
    Compute_dWt();
  } else {
    Compute_Error();		// for display purposes only..
  }
  // weight update taken care of by the epoch program
  DataUpdate(false);
}


//////////////////////////
//   BpProject	        //
//////////////////////////

void BpProject::Initialize() {
  networks.SetBaseType(&TA_BpNetwork);
  wizards.SetBaseType(&TA_BpWizard);
}
