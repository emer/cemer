// Copyright 2017, Regents of the University of Colorado,
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


#include "so.h"
#include <DumpFileCvt>
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(SoConSpec);
TA_BASEFUNS_CTORS_DEFN(SoUnitSpec);
TA_BASEFUNS_CTORS_DEFN(SoUnit);
TA_BASEFUNS_CTORS_DEFN(SoLayer);
TA_BASEFUNS_CTORS_DEFN(SoNetwork);
TA_BASEFUNS_CTORS_DEFN(HebbConSpec);
TA_BASEFUNS_CTORS_DEFN(SoProject);
TA_BASEFUNS_CTORS_DEFN(SoWizard);
TA_BASEFUNS_CTORS_DEFN(ThreshLinSoUnitSpec);
TA_BASEFUNS_CTORS_DEFN(SoftMaxLayerSpec);
TA_BASEFUNS_CTORS_DEFN(SoLayerSpec);
TA_BASEFUNS_CTORS_LITE_DEFN(SoLayerSpec_SPtr);
SMARTREF_OF_CPP(SoLayerSpec);


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
  so_converter_init();          // configure converter
}

// module initialization
InitProcRegistrar mod_init_so(so_module_init);

//////////////////////////
//      Con,Spec        //
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

void HebbConSpec::Initialize() {
  wt_limits.min = -1.0f;
  wt_limits.max = 1.0f;
}

//////////////////////////
//      Unit,Spec       //
//////////////////////////

void SoUnitSpec::Initialize() {
  min_obj_type = &TA_SoUnit;
}

void SoUnitSpec::InitLinks() {
  inherited::InitLinks();
  children.SetBaseType(&TA_SoUnitSpec); // allow any of this basic type here
  children.el_typ = GetTypeDef(); // but make the default to be me!
}

void SoUnitSpec::Init_Acts(UnitVars* u, Network* net, int thr_no) {
  inherited::Init_Acts(u, net, thr_no);
  ((SoUnitVars*)u)->act_i = 0.0f;
}

void SoUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  // simple linear function
  if(u->ext_flag & UnitVars::EXT)
    u->act = u->ext;
  else
    u->act = u->net;
}

void SoUnitSpec::Compute_AvgInAct(SoUnitVars* u, SoNetwork* net, int thr_no) {
  const int nrcg = net->ThrUnNRecvConGps(thr_no, u->thr_un_idx);
  for(int g=0; g<nrcg; g++) {
    SoConGroup* rgp = (SoConGroup*)net->ThrUnRecvConGroup(thr_no, u->thr_un_idx, g);
    if(rgp->NotActive()) continue;
    ((SoConSpec*)rgp->con_spec)->Compute_AvgInAct(rgp, net, thr_no);
  }
}

void SoUnitSpec::GraphActFun(DataTable* graph_data, float min, float max) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* netin = graph_data->FindMakeColName("Netin", idx, VT_FLOAT);
  DataCol* act = graph_data->FindMakeColName("Act", idx, VT_FLOAT);

  SoUnitVars un;
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

void ThreshLinSoUnitSpec::Initialize() {
  threshold = 0.0f;
}

void ThreshLinSoUnitSpec::Compute_Act(UnitVars* u, Network* net, int thr_no) {
  if(u->ext_flag & UnitVars::EXT)
    u->act = u->ext;
  else
    u->act = (u->net > threshold) ? (u->net - threshold) : 0.0f;
}

void SoUnit::Initialize() {
}

//////////////////////////
//      Layer,Spec      //
//////////////////////////

void SoLayerSpec::Initialize() {
  min_obj_type = &TA_SoLayer;
  netin_type = MAX_NETIN_WINS;  // competitive learning style
}

SoUnit* SoLayerSpec::FindMaxNetIn(SoLayer* lay) {
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();
  float max_val = -1.0e20f;
  SoUnit* max_val_u = NULL;
  FOREACH_ELEM_IN_GROUP(SoUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->act() = uspec->act_range.min;
    u->act_i() = uspec->act_range.min;
    if(u->net() > max_val) {
      max_val_u = u;
      max_val = u->net();
    }
  }
  return max_val_u;
}

SoUnit* SoLayerSpec::FindMinNetIn(SoLayer* lay) {
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();
  float min_val = 1.0e20f;
  SoUnit* min_val_u = NULL;
  FOREACH_ELEM_IN_GROUP(SoUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->act() = uspec->act_range.min;
    u->act_i() = uspec->act_range.min;
    if(u->net() < min_val) {
      min_val_u = u;
      min_val = u->net();
    }
  }
  return min_val_u;
}

SoUnit* SoLayerSpec::FindWinner(SoLayer* lay) {
  if(netin_type == MAX_NETIN_WINS)
    return FindMaxNetIn(lay);
  return FindMinNetIn(lay);
}

void SoLayerSpec::Compute_Act_post(SoLayer* lay, SoNetwork* net) {
  Compute_AvgAct(lay, net);
}

void SoLayerSpec::Compute_AvgAct(SoLayer* lay, SoNetwork* net) {
  // todo: this could be done faster as in Leabra, but..
  lay->sum_act = 0.0f;
  if(lay->units.leaves == 0)    return;
  FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
    if(u->lesioned()) continue;
    lay->sum_act += u->act();
  }
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
  taBase::Own(spec, this);
  spec.SetDefaultSpec(this);
}

void SoLayer::CutLinks() {
  inherited::CutLinks();
  spec.CutLinks();
  winner = NULL;
}

bool SoLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)        return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((SoLayerSpec*)sp);
  else
    return false;
  return true;
}

void SoLayer::CheckSpecs() {
  spec.CheckSpec();
  inherited::CheckSpecs();
}

//////////////////////////////////
//      Simple SoftMax          //
//////////////////////////////////

void SoftMaxLayerSpec::Initialize() {
  softmax_gain = 1.0f;
}

void SoftMaxLayerSpec::Compute_Act_post(SoLayer* lay, SoNetwork* net) {
  if(lay->ext_flag & UnitVars::EXT) { // input layer
    SoLayerSpec::Compute_Act_post(lay, net);
    return;
  }

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.SPtr();

  float sum = 0.0f;
  FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
    if(u->lesioned()) continue;
    u->act() = expf(softmax_gain * u->net()); // e to the net
    sum += u->act();
  }

  if(sum > 0.0f) {
    FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
      if(u->lesioned()) continue;
      u->act() = uspec->act_range.Project(u->act() / sum);
      // normalize by sum, rescale to act range range
    }
  }

  Compute_AvgAct(lay, net);
}


//////////////////////////
//  SoNetwork           //
//////////////////////////

void SoNetwork::Initialize() {
  layers.SetBaseType(&TA_SoLayer);
  unit_vars_type = &TA_SoUnitVars;
  con_group_type = &TA_SoConGroup;
}

void SoNetwork::BuildNullUnit() {
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new SoUnit, this);
  }
}

void SoNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_SoCon;
  prjn->con_spec.SetBaseType(&TA_SoConSpec);
  prjn->con_spec.type = &TA_HebbConSpec;
}

void SoNetwork::Compute_NetinAct() {
  inherited::Compute_NetinAct();
  FOREACH_ELEM_IN_GROUP(SoLayer, lay, layers) {
    lay->Compute_Act_post(this);
  }
}

void SoNetwork::Trial_Run() {
  DataUpdate(true);

  Compute_NetinAct();

  if(train_mode == TRAIN)
    Compute_dWt();

  // weight update taken care of by the epoch program
  DataUpdate(false);
}

//////////////////////////
//   SoProject          //
//////////////////////////

void SoProject::Initialize() {
  wizards.el_typ = &TA_SoWizard;
  networks.el_typ = &TA_SoNetwork;
}

void SoWizard::Initialize() {
}

bool SoWizard::StdProgs() {
  if(!StdProgs_impl("SoAll_Std")) return false;
  return true;
}

bool SoWizard::TestProgs(Program* call_test_from, bool call_in_loop, int call_modulus) {
  if(!TestProgs_impl("SoAll_Test", call_test_from, call_in_loop, call_modulus))
    return false;
  return true;
}

