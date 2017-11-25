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

#include "Projection.h"
#include <Network>
#include <Layer>
#include <taProject>
#include <NetMonitor>
#include <taFiler>
#include <DataTable>
#include <taiEdit>

#include <ProjectionSpec_cpp>

#include <State_main>

#include <tabMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PrjnConStats);
TA_BASEFUNS_CTORS_DEFN(Projection);

using namespace std;


void Projection::Initialize() {
  Initialize_core();
  disp = true;
  layer = NULL;
  from_type = INIT; //was: PREV;
  con_type = &TA_Connection;
  dir_fixed = false;
  direction = DIR_UNKNOWN;
  m_prv_con_spec = NULL;
  m_prv_off = false;
  prjn_clr.Set(1.0f, .9f, .5f); // very light orange
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
#endif

  prjn_idx = -1;
}

void Projection::Destroy(){
  CutLinks();
}

void Projection::CutLinks() {
  if(owner == NULL) return;
  if((bool)layer && layer->own_net) {
    layer->own_net->ClearIntact();
  }
  if((bool)from) {
    // remove from sending links, being sure to protect against a spurious re-delete
    taBase::Ref(this);
    from->send_prjns.RemoveEl(this);
    taBase::unRef(this);
  }
  from = NULL;
  spec.CutLinks();
  con_spec.CutLinks();
  m_prv_con_spec = NULL;
  if(((bool)layer) && taMisc::gui_active) {
    owner = NULL;               // tell view that we're not really here
    if(layer->own_net) {
      layer->own_net->RemoveCons_impl(); // get rid of connections in any other layers!
    }
  }
  layer = NULL;
  inherited::CutLinks();
}

void Projection::InitLinks() {
  inherited::InitLinks();
  taBase::Own(spec, this);
  taBase::Own(con_spec, this);
  taBase::Own(prjn_clr, this);
  taBase::Own(recv_con_stats, this);
  taBase::Own(send_con_stats, this);
  
  Network* mynet = GET_MY_OWNER(Network);
  if(mynet) {
    mynet->ClearIntact();
    mynet->SetProjectionDefaultTypes(this);
  }

  layer = GET_MY_OWNER(Layer);
  if(mynet) {
    int myindex = mynet->layers.FindLeafEl(layer);
    if(!(myindex == 0) && (from_type == PREV)) { // is it not the first?
      SetFrom();
      UpdateName();
    }
  }
  spec.SetDefaultSpec(this);
  con_spec.SetDefaultSpec(this);
  
#ifdef DMEM_COMPILE
  taBase::Own(dmem_agg_sum, this);
  dmem_agg_sum.agg_op = MPI_SUM;
#endif
}

void Projection::Copy_(const Projection& cp) {
  off = cp.off;
  disp = cp.disp;
  notes = cp.notes;
  from_type = cp.from_type;
  from = cp.from;
  spec = cp.spec;
  con_type = cp.con_type;
  con_spec = cp.con_spec;
  prjn_clr = cp.prjn_clr;
  dir_fixed = cp.dir_fixed;
  direction = cp.direction;

  m_prv_con_spec = cp.m_prv_con_spec;
  m_prv_off = off;
}

void Projection::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);

  if(!old_owner) return;
  Network* mynet = GET_MY_OWNER(Network);
  Network* otnet = (Network*)old_owner->GetOwner(&TA_Network);
  if(!mynet || !otnet || mynet == otnet) return;  // don't update if not relevant
  UpdatePointers_NewPar(otnet, mynet);
}

void Projection::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(!(taMisc::is_loading || (from_type == INIT)))
    SetFrom();

  Network* mynet = GET_MY_OWNER(Network);
  
  if(taMisc::is_loading) {
    m_prv_off = off;
  }
  else {
    if(off != m_prv_off) {
      if(taMisc::gui_active && mynet && mynet->IsBuiltIntact()) {
        int chs = taMisc::Choice("Can only change the off status of a projection when the network is *not* Built -- this determines if a projection is built -- do you want to UnBuild the network and proceed?", "UnBuild and Proceed", "Cancel");
        if(chs == 0) {
          mynet->UnBuild();
          m_prv_off = off;
        }
        else {
          off = m_prv_off;
        }
      }
      else {
        m_prv_off = off;
      }
    }
  }

  if((bool)from) {
    Network* fmnet = GET_OWNER(from, Network);
    if(fmnet != mynet) {
      Layer* ly = mynet->FindLayer(from->name);
      if(ly) {
        from_type = CUSTOM;     // most likely already is..
        from = ly;
      }
      else {
        TestWarning(true, "UAE", "'from' layer is not within this Network, and layer of same name was not found, so setting from = PREV -- most likely you need to fix this!");
        from_type = PREV;
        SetFrom();
      }
    }
    UpdateName();
  }

  UpdateLesioned();
  ConSpecUpdated();

  // somehow the colors get out-of-whack on these guys
  if(fabsf(prjn_clr.g - .9f) <= .011f) prjn_clr.g = .9f;
  if(fabsf(prjn_clr.b - .5f) <= .011f) prjn_clr.b = .5f;
  prjn_clr.QuantizeFloats(); // fix floats
}

void Projection::UpdateName() {
  if(from) {
    String nwnm = "Fm_" + from->name;
    if(con_spec.SPtr()) {
      con_spec.SPtr()->GetPrjnName(*this, nwnm);
    }
    if(!name.startsWith(nwnm) || !name.after(nwnm).startsWith('_') ||
       name.endsWith("_copy")) {
      SetName(nwnm);            // setname ensures uniqueness
      SigEmitUpdated();
    }
  }
}

void Projection::UpdateLesioned() {
  if(!(bool)from || !layer) {
    lesioned = true;
  }
  else {
    if(layer->lesioned() || from->lesioned()) {
      lesioned = true;
    }
    else {
      lesioned = false;
    }
  }
}

bool Projection::ChangeMyType(TypeDef* new_typ) {
  if(layer && layer->own_net) {
    layer->own_net->ClearIntact();
  }
  return inherited::ChangeMyType(new_typ);
}

#ifdef DMEM_COMPILE

void Projection::DMem_InitAggs() {
  // important: cannot check for valid as this is called during building!
  // NetworkState_cpp* net = GetValidNetState();
  if(!layer || !layer->own_net || !layer->own_net->net_state) return;
  Network* mynet = layer->own_net;
  NetworkState_cpp* net = mynet->net_state;
  PrjnState_cpp* pst = GetPrjnState(net);
  if(!pst) return;
  dmem_agg_sum.agg_op = MPI_SUM;
  dmem_agg_sum.ScanMembers(mynet->PrjnStateType(), (void*)pst);
  dmem_agg_sum.CompileVars();
}

void Projection::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}

#endif


void Projection::ToggleOff() {
  NetworkState_cpp* net = GetValidNetState();
  if(net) {
    int chs = taMisc::Choice("Can only change the off status of a projection when the network is *not* Built -- this determines if a projection is built -- do you want to UnBuild the network and proceed?", "UnBuild and Proceed", "Cancel");
    if(chs == 0) {
      Network* mynet = layer->own_net;
      mynet->UnBuild();
    }
    else {
      return;
    }
  }
  off = !off;
  m_prv_off = off;
  UpdateAfterEdit();
}

void Projection::Lesion() {
  if(lesioned) return;
  NetworkState_cpp* net = GetValidNetState();
  if(TestError(!net, "Lesion", "Can only Lesion a projection when network is built -- otherwise has no effect")) {
    return;
  }
  if(TestError(prjn_idx < 0, "Lesion",
               "can only lesion projections that have been built")) {
    return;
  }
  lesioned = true;
  PrjnState_cpp* pj = GetPrjnState(net);
  pj->LesionState(net);
  Network* mynet = layer->own_net;
  mynet->UpdtAfterNetMod();
  SigEmitUpdated();
}

void Projection::UnLesion() {
  if(!lesioned) return;
  NetworkState_cpp* net = GetValidNetState();
  if(TestError(!net, "UnLesion", "Can only UnLesion a projection when network is built -- otherwise has no effect")) {
    return;
  }
  if(TestError(prjn_idx < 0, "UnLesion",
               "can only unlesion projections that have been built")) {
    return;
  }
  UpdateLesioned();
  PrjnState_cpp* pj = GetPrjnState(net);
  pj->UnLesionState(net);
  Network* mynet = layer->own_net;
  mynet->UpdtAfterNetMod();
  SigEmitUpdated();
}

    
    
void Projection::SetFrom() {
  if(!(bool)layer) {
    from = NULL;
    return;
  }
  Network* mynet = layer->own_net;
  if(mynet == NULL)
    return;
  int myindex = mynet->layers.FindLeafEl(layer);

  switch(from_type) { // this is where the projection is coming from
  case NEXT:
    if(TestWarning(myindex == (mynet->layers.leaves - 1), "SetFrom",
                   "Last Layer projects from NEXT layer")) {
      return;
    }
    else {
      Layer* nwly = (Layer*)mynet->layers.Leaf(myindex+1);
      if(from.ptr() == nwly) return;
      from = nwly;
      SigEmitUpdated();
    }
    break;
  case PREV:
    if(TestWarning(myindex == 0, "SetFrom",
                   "First Layer recieves projection from PREV layer")) {
      return;
    }
    else {
      Layer* nwly = (Layer*)mynet->layers.Leaf(myindex-1);
      if(from.ptr() == nwly) return;
      from = nwly;
      SigEmitUpdated();
    }
    break;
  case SELF:
    if(from.ptr() == layer) return;
    from = layer;
    SigEmitUpdated();
    break;
  case CUSTOM:
    TestWarning(!(bool)from, "SetFrom", "CUSTOM projection and from is NULL");
    break;
  case INIT:
    break;
  }
}

void Projection::SetCustomFrom(Layer* fm_lay) {
  from = fm_lay;
  if(fm_lay == layer)
    from_type = SELF;
  else
    from_type = CUSTOM;
  UpdateAfterEdit();
}

void Projection::CheckSpecs() {
  spec.CheckSpec();
  con_spec.CheckSpec(con_type);
  ConSpec* sp = con_spec.SPtr();
  if(sp) {
    if(TestWarning(!con_type->InheritsFrom(sp->min_obj_type), "UpdateConSpec",
                   "connection type set to:",sp->min_obj_type->name,
                   "as required by the connection spec:", sp->name)) {
      con_type = sp->min_obj_type;
    }
  }
  // NOTE: checkspecs does NOT go into units or connections!
}

bool Projection::ConSpecUpdated() {
  ConSpec* sp = con_spec.SPtr();
  if(!sp) return false;
  if(sp == m_prv_con_spec) return false;
  m_prv_con_spec = sp;          // don't redo it
  if((!(bool)layer) || (!(bool)from)) return false;
  Network* mynet = GET_MY_OWNER(Network);
  if(!mynet || !mynet->IsBuiltIntact()) return false;
  NetworkState_cpp* net_state = mynet->net_state;
  if(sp) {
    if(TestWarning(!con_type->InheritsFrom(sp->min_obj_type), "UpdateConSpec",
                   "connection type set to:",sp->min_obj_type->name,
                   "as required by the connection spec:", sp->name)) {
      con_type = sp->min_obj_type;
    }
  }
  mynet->ClearIntact();         // spec updated -- need to rebuild network
  
  return true;
}

bool Projection::ApplySpecToMe(BaseSpec* apspec) {
  if(apspec == NULL) return false;
  if(apspec->InheritsFrom(&TA_ProjectionSpec)) {
    SetPrjnSpec((ProjectionSpec*)apspec);
    return true;
  }
  else if(apspec->InheritsFrom(&TA_ConSpec)) {
    SetConSpec((ConSpec*)apspec);
    return true;
  }
  return false;
}

bool Projection::SetPrjnSpec(ProjectionSpec* sp) {
  if(sp == NULL)        return false;
  spec.SetSpec(sp);
  return true;
}

bool Projection::SetConSpec(ConSpec* sp) {
  if(!sp)       return false;
  if(!con_spec.SetSpec(sp)) return false;
  if(!con_spec.CheckObjTypeForSpec(con_type)) {
    if(taMisc::Choice("The con spec you are setting is not compatible with the con type for connections in this projection -- should I change the con type to be: " +  con_spec->min_obj_type->name
                      + " (if you answer No, you will continue to get errors until a compatible selection is made)",
                      "Yes", "No") == 0) {
      con_type = con_spec->min_obj_type;
    }
  }
  return ConSpecUpdated();
}

bool Projection::SetConType(TypeDef* td) {
  if(con_type == td) return false;
  if(layer && layer->own_net) {
    layer->own_net->ClearIntact();
  }
  con_type = td;
  return true;
}

bool Projection::EditState() {
  if(!taMisc::gui_active) return false;
  Network* own_net = GET_MY_OWNER(Network);
  if(!own_net) return false;
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return false;
  
  if (taiEdit *ie = own_net->PrjnStateType()->ie) {
    PrjnState_cpp* pj = GetPrjnState(net);
    if(!pj) return false;
    return ie->Edit((void*)pj, false);
  }
  return true;
}

bool Projection::EditConState(int unit_no, bool recv) {
  if(recv && layer) {
    return layer->EditConState(unit_no, recv_idx, true);
  }
  else if((bool)from) {
    return from->EditConState(unit_no, send_idx, false);
  }
  return false;
}

void Projection::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

void Projection::SaveWeights(const String& fname) {
  if(!(layer && layer->own_net && layer->own_net->net_state)) return;
  NetworkState_cpp* net = layer->own_net->net_state;
  if(!net->IsBuiltIntact()) return;
  LayerState_cpp* lay = layer->GetLayerState(net);
  PrjnState_cpp* prjn = GetPrjnState(net);
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm) {
    net->LayerSaveWeights_strm(*flr->ostrm, lay, NetworkState_cpp::TEXT, prjn);
  }
  flr->Close();
  taRefN::unRefDone(flr);
}

bool Projection::LoadWeights(const String& fname, bool quiet) {
  if(!(layer && layer->own_net && layer->own_net->net_state)) return false;
  NetworkState_cpp* net = layer->own_net->net_state;
  if(!net->IsBuiltIntact()) return false;
  LayerState_cpp* lay = GetRecvLayer(net);
  PrjnState_cpp* prjn = GetPrjnState(net);
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  bool rval = false;
  if(flr->istrm) {
    rval = net->LayerLoadWeights_strm(*flr->istrm, lay, NetworkState_cpp::TEXT, quiet, prjn);
  }
  else {
    TestError(true, "LoadWeights", "aborted due to inability to load weights file");
    // the above should be unnecessary but we're not getting the error sometimes..
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

int Projection::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp, bool prompt) {
  if(GetMainConSpec() != old_sp) return 0;
  String act_nm = "ConSpec: " + old_sp->name + " with: " + new_sp->name
    + " in layer: " + layer->name + " prjn: " + name;
  if(prompt) {
    BrowserSelectMe();
    int ok = taMisc::Choice("Replace " + act_nm + "?",
                            "Ok", "Skip");
    if(ok != 0) return 0;
  }
  con_spec.SetSpec(new_sp);
  ConSpecUpdated();
  taMisc::Info("Replaced", act_nm);
  return 1;
}

int Projection::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp, bool prompt) {
  if(GetMainPrjnSpec() != old_sp) return 0;
  String act_nm = "PrjnSpec: " + old_sp->name + " with: " + new_sp->name
    + " in layer: " + layer->name + " prjn: " + name;
  if(prompt) {
    BrowserSelectMe();
    int ok = taMisc::Choice("Replace " + act_nm + "?",
                            "Ok", "Skip");
    if(ok != 0) return 0;
  }
  spec.SetSpec(new_sp);
  taMisc::Info("Replaced", act_nm);
  return 1;
}

void Projection::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  if(!layer || !layer->own_net || !layer->own_net->net_state)
    return;

  NetworkState_cpp* net_state = layer->own_net->net_state;
  
  if(!IsActive(net_state)) return;
  
  CheckSpecs();                 // just check!

  ConSpec* cs = GetMainConSpec();
  if(cs) {
    bool chk = cs->CheckConfig_RecvCons(this, quiet);
    if(!chk) rval = false;
  }
}


NetworkState_cpp* Projection::GetValidNetState() const {
  if(!layer) return NULL;
  NetworkState_cpp* net = layer->GetValidNetState();
  if(!net) return NULL;
  if(prjn_idx < 0) return NULL;
  return net;
}

void Projection::Init_Weights() {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  PRJN_STATE* prjn = GetPrjnState(net);
  prjn->Init_Weights(net);
}

void Projection::Copy_Weights(Projection* src) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  PRJN_STATE* prjn = GetPrjnState(net);
  PRJN_STATE* src_prjn = src->GetPrjnState(net);
  prjn->Copy_Weights(net, src_prjn);
}

void Projection::TransformWeights(const SimpleMathSpec& trans) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;
  
  PRJN_STATE* prjn = GetPrjnState(net);
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    if(ru->lesioned()) continue;
    ru->TransformWeights(net, trans, prjn);
  }
}

void Projection::RenormWeights(bool mult_norm, float avg_wt) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  PRJN_STATE* prjn = GetPrjnState(net);
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    if(ru->lesioned()) continue;
    ru->RenormWeights(net, mult_norm, avg_wt, prjn);
  }
}

void Projection::RescaleWeights(const float rescale_factor) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  PRJN_STATE* prjn = GetPrjnState(net);
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    if(ru->lesioned()) continue;
    ru->RescaleWeights(net, rescale_factor, prjn);
  }
}

void Projection::AddNoiseToWeights(const Random& noise_spec) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return;

  PRJN_STATE* prjn = GetPrjnState(net);
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    if(ru->lesioned()) continue;
    ru->AddNoiseToWeights(net, noise_spec, prjn);
  }
}

int Projection::PruneCons(const SimpleMathSpec& pre_proc,
                          Relation::Relations rel, float cmp_val)
{
  int rval = 0;
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return 0;

  PRJN_STATE* prjn = GetPrjnState(net);
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    if(ru->lesioned()) continue;
    rval += ru->PruneCons(net, pre_proc, rel, cmp_val, prjn);
  }
  return rval;
}

int Projection::ProbAddCons(float p_add_con, float init_wt) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return 0;

  PRJN_STATE* prjn = GetPrjnState(net);
  PRJN_SPEC_CPP* pspec = GetPrjnSpec(net);
  int rval = pspec->ProbAddCons(prjn, net, p_add_con, init_wt);
  return rval;
}

int Projection::LesionCons(float p_lesion, bool permute) {
  int rval = 0;
  NetworkState_cpp* net = GetValidNetState();

  PRJN_STATE* prjn = GetPrjnState(net);
  LAYER_STATE* recv_lay = GetRecvLayer(net);
  for(int rui = 0; rui < recv_lay->n_units; rui++) {
    UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
    if(ru->lesioned()) continue;
    rval += ru->LesionCons(net, p_lesion, permute, prjn);
  }
  return rval;
}

DataTable* Projection::WeightsToTable(DataTable* dt, const String& col_nm_,
                                      bool recv_wts) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;

  PRJN_STATE* prjn = GetPrjnState(net);
  bool new_table = false;
  if (!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable(name + "_Weights", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  dt->ResetData();

  String col_nm = col_nm_;
  if(col_nm.empty()) col_nm = from->name;

  taVector2i log_pos;
  int idx;
  if(recv_wts) {
    DataCol* scol = dt->FindMakeColName(col_nm, idx, VT_FLOAT, 2,
                                        from->flat_geom.x, from->flat_geom.y);
    LAYER_STATE* recv_lay = GetRecvLayer(net);
    for(int rui = 0; rui < recv_lay->n_units; rui++) {
      UNIT_STATE* ru = recv_lay->GetUnitState(net, rui);
      if(ru->lesioned()) continue;
      ConState_cpp* cg = ru->RecvConStatePrjn(net, prjn);
      if(cg == NULL) continue;
      dt->AddBlankRow();
      int wi;
      for(wi=0;wi<cg->size;wi++) {
        UNIT_STATE* ou = cg->UnState(wi, net);
        scol->SetMatrixVal(cg->Cn(wi,ConState_cpp::WT,net), -1, ou->pos_x, ou->pos_y);
      }
    }
  }
  else {
    DataCol* scol = dt->FindMakeColName(col_nm, idx, VT_FLOAT, 2,
                                      layer->flat_geom.x, layer->flat_geom.y);

    LAYER_STATE* send_lay = prjn->GetSendLayer(net);
    for(int sui = 0; sui < send_lay->n_units; sui++) {
      UNIT_STATE* su = send_lay->GetUnitState(net, sui);
      if(su->lesioned()) continue;
      ConState_cpp* cg = su->SendConStatePrjn(net, prjn);
      if(cg == NULL) continue;
      dt->AddBlankRow();
      int wi;
      for(wi=0;wi<cg->size;wi++) {
        UNIT_STATE* ou = cg->UnState(wi, net);
        scol->SetMatrixVal(cg->Cn(wi,ConState_cpp::WT,net), -1, ou->pos_x, ou->pos_y);
      }
    }
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Projection::VarToTable(DataTable* dt, const String& variable) {
  bool new_table = false;
  if (!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
    new_table = true;
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return NULL;

  NetMonitor nm;
  nm.OwnTempObj();
  nm.SetDataNetwork(dt, net);
  nm.AddProjection(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Projection::ConVarsToTable(DataTable* dt, const String& var1, const String& var2,
                                const String& var3, const String& var4, const String& var5,
                                const String& var6, const String& var7, const String& var8,
                                const String& var9, const String& var10, const String& var11,
                                const String& var12, const String& var13, const String& var14) {
  NetworkState_cpp* net = GetValidNetState();
  if(!net) return NULL;

  PRJN_STATE* prjn = GetPrjnState(net);
  bool new_table = false;
  if(!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  layer->ConVarsToTable(dt, var1, var2, var3, var4, var5, var6, var7, var8,
                        var9, var10, var11, var12, var13, var14, prjn);
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

