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



// bp.cc

#include "bp.h"

#include <math.h>
#include <limits.h>
#include <float.h>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

// module initialization
InitProcRegistrar mod_init(ta_Init_bp);



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
  ConSpec::InitLinks();
  taBase::Own(lrate_sched, this);
}

void BpConSpec::UpdateAfterEdit() {
  ConSpec::UpdateAfterEdit();
  lrate_sched.UpdateAfterEdit();
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
  cn->dEdW -= spec->decay * cn->wt;
}

void Bp_WtElim_WtDecay(BpConSpec* spec, BpCon* cn, BpUnit*, BpUnit*) {
  float denom = (1.0f + (cn->wt * cn->wt));
  cn->dEdW -= spec->decay * ((2.0f * cn->wt * cn->wt) / (denom * denom));
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
  UnitSpec::InitLinks();
  taBase::Own(sig, this);
}

void BpUnitSpec::CutLinks() {
  UnitSpec::CutLinks();
}

void BpUnitSpec::Copy_(const BpUnitSpec& cp) {
  sig = cp.sig;
  err_tol = cp.err_tol;
  err_fun = cp.err_fun;
}

void BpUnitSpec::SetCurLrate(BpUnit* u, int epoch) {
  ((BpConSpec*)bias_spec.SPtr())->SetCurLrate(epoch);
  for(int g=0; g<u->recv.size; g++) {
    BpRecvCons* recv_gp = (BpRecvCons*)u->recv.FastEl(g);
    recv_gp->SetCurLrate(epoch);
  }
}

void BpUnitSpec::Init_Acts(Unit* u) {
  UnitSpec::Init_Acts(u);
  BpUnit* bu = (BpUnit*)u;
  bu->err = bu->dEdA = bu->dEdNet = 0.0f;
}

void BpUnitSpec::Compute_Act(Unit* u) { // this does the sigmoid
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = act_range.Project(sig.Eval(u->net));
}

void BpUnitSpec::Compute_Error(BpUnit* u) {
  if(u->ext_flag & Unit::TARG) (*err_fun)(this, u);
}

void BpUnitSpec::Compute_dEdA(BpUnit* u) {
  u->dEdA = 0.0f;
  u->err = 0.0f;
  for(int g=0; g<u->send.size; g++) {
    BpSendCons* send_gp = (BpSendCons*)u->send.FastEl(g);
    if(!send_gp->prjn->layer->lesion)
      u->dEdA += send_gp->Compute_dEdA(u);
  }
}

void BpUnitSpec::Compute_dEdNet(BpUnit* u) {
  u->dEdNet = u->dEdA * sig.gain * (u->act - act_range.min) *
    (act_range.max - u->act) * act_range.scale;
}

void BpUnitSpec::Compute_dWt(Unit* u) {
  if(u->ext_flag & Unit::EXT)  return; // don't compute dwts for clamped units
  UnitSpec::Compute_dWt(u);
  ((BpConSpec*)bias_spec.SPtr())->B_Compute_dWt((BpCon*)u->bias.Cn(0), (BpUnit*)u);
}

void BpUnitSpec::Compute_Weights(Unit* u) {
  if(u->ext_flag & Unit::EXT)  return; // don't update for clamped units
  UnitSpec::Compute_Weights(u);
  ((BpConSpec*)bias_spec.SPtr())->B_Compute_Weights((BpCon*)u->bias.Cn(0), (BpUnit*)u);
}

void BpUnitSpec::GraphActFun(DataTable* graph_data, float min, float max) {
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFun", true);
    newguy = true;
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
    Compute_Act(&un);
    graph_data->AddBlankRow();
    netin->SetValAsFloat(x, -1);
    act->SetValAsFloat(un.act, -1);
  }
  graph_data->StructUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
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

void DeltaBarDeltaBpConSpec::UpdateAfterEdit() {
  BpConSpec::UpdateAfterEdit();
  act_lrate_incr = lrate * lrate_incr;
}

void DeltaBarDeltaBpConSpec::Copy_(const DeltaBarDeltaBpConSpec& cp) {
  lrate_incr = cp.lrate_incr;
  lrate_decr = cp.lrate_decr;
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

void BpContextSpec::InitLinks() {
  BpUnitSpec::InitLinks();
  taBase::Own(initial_act, this);
}

void BpContextSpec::Copy_(const BpContextSpec& cp) {
  hysteresis = cp.hysteresis;
  hysteresis_c = cp.hysteresis_c;
  initial_act = cp.initial_act;
  variable = cp.variable;
  unit_flags = cp.unit_flags;
}

void BpContextSpec::UpdateAfterEdit() {
  BpUnitSpec::UpdateAfterEdit();
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

void BpContextSpec::Init_Acts(Unit* u) {
  BpUnitSpec::Init_Acts(u);
  u->act = initial_act.Gen();
}

void BpContextSpec::Compute_Act(Unit* u) {
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

void LinearBpUnitSpec::UpdateAfterEdit() {
  BpUnitSpec::UpdateAfterEdit();
  if(err_fun == Bp_CrossEnt_Error) {
    taMisc::Error("LinearBpUnitSpec: Cross entropy error is incompatible with Linear Units!  I switched to Squared_Error for you.");
    SetUnique("err_fun", true);
    err_fun = Bp_Squared_Error;
  }
}

void LinearBpUnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = act_range.Clip(u->ext);
  else
    u->act = act_range.Clip(u->net);
}

void LinearBpUnitSpec::Compute_dEdNet(BpUnit* u) {
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

void ThreshLinBpUnitSpec::UpdateAfterEdit() {
  BpUnitSpec::UpdateAfterEdit();
  if(err_fun == Bp_CrossEnt_Error) {
    taMisc::Error("ThreshLinBpUnitSpec: Cross entropy error is incompatible with Linear Units!  I switched to Squared_Error for you.");
    SetUnique("err_fun", true);
    err_fun = Bp_Squared_Error;
  }
}

void ThreshLinBpUnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = act_range.Clip(u->ext);
  else
    u->act = act_range.Clip((u->net > threshold) ? (u->net - threshold) : 0.0f);
}

void ThreshLinBpUnitSpec::Compute_dEdNet(BpUnit* u) {
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

void NoisyBpUnitSpec::InitLinks() {
  BpUnitSpec::InitLinks();
  taBase::Own(noise, this);
}

void NoisyBpUnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = act_range.Clip(u->ext + noise.Gen());
  else   // need to keep in SigmoidSpec clipped range!
    u->act = act_range.min + act_range.range *
      SigmoidSpec::Clip(sig.Eval(u->net) + noise.Gen());
}


//////////////////////////
// Stochastic Unit Spec //
//////////////////////////


void StochasticBpUnitSpec::Compute_Act(Unit* u) { // this does the probabiltiy on sigmoid
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

void RBFBpUnitSpec::UpdateAfterEdit() {
  BpUnitSpec::UpdateAfterEdit();
  norm_const = 1.0f / sqrtf(2.0f * 3.14159265358979323846 * var);
  denom_const = 0.5f / var;
}

void RBFBpUnitSpec::Compute_Netin(Unit* u) {
  // do distance instead of net input
  u->net = 0.0f;
  for(int g=0; g<u->recv.size; g++) {
    RecvCons* recv_gp = (RecvCons*)u->recv.FastEl(g);
    u->net += recv_gp->Compute_Dist(u);
  }
}

void RBFBpUnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = norm_const * expf(-denom_const * u->net);
}

void RBFBpUnitSpec::Compute_dEdNet(BpUnit* u) {
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

void BumpBpUnitSpec::UpdateAfterEdit() {
  BpUnitSpec::UpdateAfterEdit();
  std_dev_r = 1.0f / std_dev;
}


void BumpBpUnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else {
    float val = std_dev_r * (u->net - mean);
    u->act = expf(- (val * val));
  }
}

void BumpBpUnitSpec::Compute_dEdNet(BpUnit* u) {
  // dadnet = a * 2 * (net - mean) / std_dev
  u->dEdNet = - u->dEdA * u->act * 2.0f * (u->net - mean) * std_dev_r;
}

//////////////////////////
//   Exp, SoftMax       //
//////////////////////////

void ExpBpUnitSpec::Compute_Act(Unit* u) {
  float net = sig.gain * u->net;
  net = MAX(net, -50.0f);
  net = MIN(net, 50.0f);
  u->act = expf(net);
}

void ExpBpUnitSpec::Compute_dEdNet(BpUnit* u) {
  u->dEdNet = u->dEdA * sig.gain * u->act;
}

void SoftMaxBpUnitSpec::Compute_Act(Unit* u) {
  if((u->recv.size < 2) || (((RecvCons*)u->recv[0])->cons.size == 0)
     || (((RecvCons*)u->recv[1])->cons.size == 0)) {
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

void SoftMaxBpUnitSpec::Compute_dEdNet(BpUnit* u) {
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
    if (layer->lesion)	continue;
    BpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(BpUnit, u, layer->units., u_itr)
      u->SetCurLrate(epoch);
  }
}

void BpNetwork::Compute_Act() {
  // compute activations; replaces generic
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, layers., l_itr) {
    if (lay->lesion)	continue;
    if (!(lay->ext_flag & Unit::EXT)) {
      lay->Compute_Netin();
#ifdef DMEM_COMPILE
      lay->DMem_SyncNet();
#endif
    }
    lay->Compute_Act();
  }
  taiMiscCore::RunPending();
}

void BpNetwork::Compute_dEdA_dEdNet() {
  // send the error back
  Layer* lay;
  int i;//
  for (i = layers.leaves-1; i>= 0; i--) {
    lay = ((Layer*) layers.Leaf(i));
    if(lay->lesion || (!bp_to_inputs && (lay->ext_flag & Unit::EXT))) // don't compute err on inputs
      continue;

    BpUnit* u;
    taLeafItr u_itr;
#ifdef DMEM_COMPILE
    // first compute dEdA from connections and share it
    FOR_ITR_EL(BpUnit, u, lay->units., u_itr)
      u->Compute_dEdA();
    lay->dmem_share_units.Aggregate(3, MPI_SUM);

    // then compute error to add to dEdA, and dEdNet
    FOR_ITR_EL(BpUnit, u, lay->units., u_itr) {
      u->Compute_Error();
      u->Compute_dEdNet();
    }
#else
    FOR_ITR_EL(BpUnit, u, lay->units., u_itr)
      u->Compute_dEdA_dEdNet();
#endif
  }
  taiMiscCore::RunPending();
}

void BpNetwork::Compute_Error() {
  // compute errors
  Layer* lay;
  taLeafItr l_itr;
  FOR_ITR_EL(Layer, lay, layers., l_itr) {
    if (lay->lesion || !(lay->ext_flag & Unit::TARG)) // only compute err on targs
      continue;

    BpUnit* u;
    taLeafItr u_itr;
    FOR_ITR_EL(BpUnit, u, lay->units., u_itr) {
      u->dEdA = 0.0f;		// must reset -- error is incremental!
      u->Compute_Error();
    }
  }
  taiMiscCore::RunPending();
}

void BpNetwork::Trial_Run() {
  DataUpdate(true);
  SetCurLrate();

  Compute_Act();
  Compute_dEdA_dEdNet();

/*NOTE: from 3.x, we put this in the program
  // compute the weight err derivatives (only if not testing...)
  if (net_context == TRAIN) {
    Compute_dWt();
  } else {
    BpCompute_Error();		// for display purposes only..
  }
*/
//TODO: verify below comment from 3.x
// weight update taken care of by the epoch process
  DataUpdate(false);
}


//////////////////////////
//   BpProject	        //
//////////////////////////

void BpProject::Initialize() {
  networks.SetBaseType(&TA_BpNetwork);
  wizards.SetBaseType(&TA_BpWizard);
}
