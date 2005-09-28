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



// so.cc

#include "so.h"

//////////////////////////
//	Con,Spec	//
//////////////////////////

void SoConSpec::Initialize() {
  min_obj_type = &TA_SoCon_Group;
  lrate = 0.1f;
  wt_limits.min = 0.0f;
  wt_limits.max = 1.0f;
  wt_limits.type = WeightLimits::MIN_MAX;
  avg_act_source = LAYER_AVG_ACT;
  rnd.mean = .5;
}

void SoConSpec::InitLinks() {
  ConSpec::InitLinks();
}

void SoCon_Group::Initialize() {
  spec.SetBaseType(&TA_SoConSpec);
  avg_in_act = 0.0f;
}

void HebbConSpec::Initialize() {
  wt_limits.min = -1.0f;
  wt_limits.max = 1.0f;
}

//////////////////////////
//	Unit,Spec	//
//////////////////////////

void SoUnitSpec::Initialize() {
  min_obj_type = &TA_SoUnit;
}

void SoUnitSpec::InitState(Unit* u) {
  UnitSpec::InitState(u);
  ((SoUnit*)u)->act_i = 0.0f;
}

void SoUnitSpec::Compute_Act(Unit* u) {
  // simple linear function
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = u->net;
}

void SoUnitSpec::Compute_AvgInAct(Unit* u) {
  SoCon_Group* recv_gp;
  int g;
  FOR_ITR_GP(SoCon_Group, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      recv_gp->Compute_AvgInAct(u);
  }
}

void SoUnitSpec::GraphActFun(GraphLog* graph_log, float min, float max) {
  if(graph_log == NULL) {
    graph_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
    if(graph_log == NULL) return;
  }
  graph_log->name = name + ": Act Fun";
  DataTable* dt = &(graph_log->data);
  dt->Reset();
  dt->NewColFloat("netin");
  dt->NewColFloat("act");

  SoUnit un;

  float x;
  for(x = min; x <= max; x += .01f) {
    un.net = x;
    Compute_Act(&un);
    dt->AddBlankRow();
    dt->SetLastFloatVal(x, 0);
    dt->SetLastFloatVal(un.act, 1);
  }
  dt->UpdateAllRanges();
  graph_log->ViewAllData();
}

void ThreshLinSoUnitSpec::Initialize() {
  threshold = 0.0f;
}

void ThreshLinSoUnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = (u->net > threshold) ? (u->net - threshold) : 0.0f;
}

void SoUnit::Initialize() {
  spec.SetBaseType(&TA_SoUnitSpec);
  act_i = 0.0f;
}

//////////////////////////
//	Layer,Spec	//
//////////////////////////

void SoLayerSpec::Initialize() {
  min_obj_type = &TA_SoLayer;
  netin_type = MAX_NETIN_WINS;	// competitive learning style
}

SoUnit* SoLayerSpec::FindMaxNetIn(SoLayer* lay) {
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;
  float max_val = -1.0e20;
  SoUnit* max_val_u = NULL;
  SoUnit* u;
  taLeafItr i;
  FOR_ITR_EL(SoUnit, u, lay->units., i) {
    u->act = uspec->act_range.min;
    u->act_i = uspec->act_range.min;
    if(u->net > max_val) {
      max_val_u = u;
      max_val = u->net;
    }
  }
  return max_val_u;
}

SoUnit* SoLayerSpec::FindMinNetIn(SoLayer* lay) {
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;
  float min_val = 1.0e20;
  SoUnit* min_val_u = NULL;
  SoUnit* u;
  taLeafItr i;
  FOR_ITR_EL(SoUnit, u, lay->units., i) {
    u->act = uspec->act_range.min;
    u->act_i = uspec->act_range.min;
    if(u->net < min_val) {
      min_val_u = u;
      min_val = u->net;
    }
  }
  return min_val_u;
}

SoUnit* SoLayerSpec::FindWinner(SoLayer* lay) {
  if(netin_type == MAX_NETIN_WINS)
    return FindMaxNetIn(lay);
  return FindMinNetIn(lay);
}

// default layerspec just iterates over units
void SoLayerSpec::Compute_Net(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_Net();
}

void SoLayerSpec::Compute_Act(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_Act();
  
  Compute_AvgAct(lay);		// always compute average layer act..
}

void SoLayerSpec::Compute_dWt(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_dWt();
}

void SoLayerSpec::UpdateWeights(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->UpdateWeights();
}

void SoLayerSpec::Compute_AvgAct(SoLayer* lay) {
  lay->sum_act = 0.0f;
  if(lay->units.leaves == 0)	return;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    lay->sum_act += u->act;
  lay->avg_act = lay->sum_act / (float)lay->units.leaves;
}

void SoLayer::Copy_(const SoLayer& cp) {
  spec = cp.spec;
  avg_act = cp.avg_act;
  sum_act = cp.sum_act;
}

void SoLayer::Initialize() {
  spec.SetBaseType(&TA_SoLayerSpec);
  units.SetBaseType(&TA_SoUnit);
  unit_spec.SetBaseType(&TA_SoUnitSpec);
  avg_act = 0.0f;
  sum_act = 0.0f;
  winner = NULL;
}

void SoLayer::InitLinks() {
  Layer::InitLinks();
  spec.SetDefaultSpec(this);
}

void SoLayer::CutLinks() {
  spec.CutLinks();
  Layer::CutLinks();
  winner = NULL;
}

bool SoLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)	return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((SoLayerSpec*)sp);
  else
    return false;
  return true;
} 

//////////////////////////
//	Processes	//
//////////////////////////

void SoTrial::Initialize() {
  min_unit = &TA_SoUnit;
  min_con_group =  &TA_SoCon_Group;
  min_con =  &TA_SoCon;
  min_layer = &TA_SoLayer;
}


void SoTrial::Compute_Act() {
  // compute activations in feed-forward fashion
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, network->layers., l) {
    lay->Compute_Net();
#ifdef DMEM_COMPILE    
    lay->DMem_SyncNet();
#endif
    lay->Compute_Act();
  }
}

void SoTrial::Compute_dWt() {
  network->Compute_dWt();
}

void SoTrial::Loop() {
  network->InitExterns();
  if(cur_event) {      
    cur_event->ApplyPatterns(network);
  }

  Compute_Act();

  // compute the delta - weight (only if not testing...)
  if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST))
    Compute_dWt();

  // weight update taken care of by the process
}

bool SoTrial::CheckNetwork() {
  if(network && (network->dmem_sync_level != Network::DMEM_SYNC_LAYER)) {
    network->dmem_sync_level = Network::DMEM_SYNC_LAYER;
  }
  return TrialProcess::CheckNetwork();
}

//////////////////////////////////
//	Simple SoftMax		//
//////////////////////////////////

void SoftMaxLayerSpec::Initialize() {
  softmax_gain = 1.0f;
}

void SoftMaxLayerSpec::Compute_Act(SoLayer* lay) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act(lay);
    return;
  }

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;

  float sum = 0.0f;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->Compute_Act();
    u->act = expf(softmax_gain * u->net); // e to the net
    sum += u->act;
  }

  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->act = uspec->act_range.Project(u->act / sum);
    // normalize by sum, rescale to act range range
  }

  Compute_AvgAct(lay);
}
