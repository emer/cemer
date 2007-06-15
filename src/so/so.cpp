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


#include "so.h"

static void so_converter_init() {
  DumpFileCvt* cvt = new DumpFileCvt("So", "SoUnit");
  cvt->repl_strs.Add(NameVar("_MGroup", "_Group"));
  cvt->repl_strs.Add(NameVar("Project", "V3SoProject"));
  cvt->repl_strs.Add(NameVar("V3SoProject_Group", "Project_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("V3SoProjection", "Projection"));
  cvt->repl_strs.Add(NameVar("SoPrjn_Group", "Projection_Group"));
  cvt->repl_strs.Add(NameVar("Network", "SoNetwork"));
  cvt->repl_strs.Add(NameVar("SoNetwork_Group", "Network_Group")); // fix prev
  cvt->repl_strs.Add(NameVar("SoWiz", "SoWizard"));
  //  cvt->repl_strs.Add(NameVar("Layer", "SoLayer"));
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

void so_module_init() {
  ta_Init_so();			// initialize types 
  so_converter_init();		// configure converter
}

// module initialization
InitProcRegistrar mod_init_so(so_module_init);

//////////////////////////
//	Con,Spec	//
//////////////////////////

void SoConSpec::Initialize() {
  min_obj_type = &TA_SoCon;
  lrate = 0.1f;
  wt_limits.min = 0.0f;
  wt_limits.max = 1.0f;
  wt_limits.type = WeightLimits::MIN_MAX;
  avg_act_source = LAYER_AVG_ACT;
  rnd.mean = .5;
}

void SoConSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_SoConSpec); // allow any of this basic type here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void SoRecvCons::Initialize() {
  SetConType(&TA_SoCon);
  avg_in_act = 0.0f;
  sum_in_act = 0.0f;
}

void SoRecvCons::Copy_(const SoRecvCons& cp) {
  avg_in_act = cp.avg_in_act;
  sum_in_act = cp.sum_in_act;
}

void SoSendCons::Initialize() {
  SetConType(&TA_SoCon);
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

void SoUnitSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_SoUnitSpec); // allow any of this basic type here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void SoUnitSpec::Init_Acts(Unit* u) {
  inherited::Init_Acts(u);
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
  SoRecvCons* recv_gp;
  taListItr g;
  FOR_ITR_EL(SoRecvCons, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesioned())
      recv_gp->Compute_AvgInAct(u);
  }
}

void SoUnitSpec::GraphActFun(DataTable* graph_data, float min, float max) {
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

  SoUnit un;
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
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();
  float max_val = -1.0e20f;
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
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();
  float min_val = 1.0e20f;
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
void SoLayerSpec::Compute_Netin(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_Netin();
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

void SoLayerSpec::Compute_Weights(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_Weights();
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
  inherited::InitLinks();
  spec.SetDefaultSpec(this);
}

void SoLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();
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

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();

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


//////////////////////////
//  SoNetwork		//
//////////////////////////

void SoNetwork::Initialize() {
  layers.SetBaseType(&TA_SoLayer);
}

void SoNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_SoCon;
  prjn->recvcons_type = &TA_SoRecvCons;
  prjn->sendcons_type = &TA_SoSendCons;
  prjn->con_spec.SetBaseType(&TA_SoConSpec);
}

void SoNetwork::Compute_Act() {
  // compute activations in feed-forward fashion
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
    lay->Compute_Netin();
#ifdef DMEM_COMPILE    
    lay->DMem_SyncNet();
#endif
    lay->Compute_Act();
  }
}

void SoNetwork::Trial_Run() {
  DataUpdate(true);
  
  Compute_Act();

  if(train_mode == TRAIN)
    Compute_dWt();

  // weight update taken care of by the epoch program
  DataUpdate(false);
}

//////////////////////////
//   SoProject	        //
//////////////////////////

void SoProject::Initialize() {
  wizards.SetBaseType(&TA_SoWizard);
  networks.SetBaseType(&TA_SoNetwork);
}

void SoWizard::StdProgs() {
  StdProgs_impl("SoAll_Std");
}

