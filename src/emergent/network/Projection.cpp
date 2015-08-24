// Copyright, 1995-2013, Regents of the University of Colorado,
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
#include <taProject>
#include <NetMonitor>
#include <taFiler>
#include <DataTable>

#include <tabMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Projection);

using namespace std;


void Projection::Initialize() {
  off = false;
  disp = true;
  layer = NULL;
  from_type = INIT; //was: PREV;
  con_type = &TA_Connection;
  recv_idx = -1;
  send_idx = -1;
  recv_n = 1;
  send_n = 1;
  projected = false;
  dir_fixed = false;
  direction = DIR_UNKNOWN;
  m_prv_con_spec = NULL;
  prjn_clr.Set(1.0f, .9f, .5f); // very light orange
}

void Projection::Destroy(){
  CutLinks();
}

void Projection::CutLinks() {
  if(owner == NULL) return;
  RemoveCons();         // remove actual connections
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
      layer->own_net->RemoveCons(); // get rid of connections in any other layers!
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
  Network* mynet = GET_MY_OWNER(Network);
  if(mynet) {
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
}

void Projection::Copy_(const Projection& cp) {
  off = cp.off;
  disp = cp.disp;
  from_type = cp.from_type;
  from = cp.from;
  spec = cp.spec;
  con_type = cp.con_type;
  con_spec = cp.con_spec;
  prjn_clr = cp.prjn_clr;
  // note: these are not copied; fixed after network copy
//   recv_idx = cp.recv_idx;
//   send_idx = cp.send_idx;
//   recv_n = cp.recv_n;
//   send_n = cp.send_n;
//   projected = cp.projected;
  dir_fixed = cp.dir_fixed;
  direction = cp.direction;

  m_prv_con_spec = cp.m_prv_con_spec;

  // this will update all pointers under us to new network if we are copied from other guy
  // only if the network is not otherwise already copying too!!
  UpdatePointers_NewPar_IfParNotCp(&cp, &TA_Network);
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

  if((bool)from) {
    Network* mynet = GET_MY_OWNER(Network);
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

  UpdateConSpecs((bool)taMisc::is_loading);
//   if(taMisc::is_loading) return;
//   if(!taMisc::gui_active) return;
//   Network* net = GET_MY_OWNER(Network);
//   if(!net) return;
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

void Projection::RemoveCons() {
  if(layer) {
    // FOREACH_ELEM_IN_GROUP(Unit, u, layer->units) {
    //   u->recv.RemovePrjn(this);
    // }
  }

  if(from) {
    // FOREACH_ELEM_IN_GROUP(Unit, u, from->units)
    //   u->send.RemovePrjn(this);
  }

  recv_idx = -1;
  send_idx = -1;
  projected = false;
}

bool Projection::ChangeMyType(TypeDef* new_typ) {
  if(TestError(layer && layer->units.leaves > 0, "ChangeMyType", "You must first remove all units in the network before changing the Projection type -- otherwise it takes FOREVER -- do Network/Structure/Remove Units"))
    return false;
  return inherited::ChangeMyType(new_typ);
}

DataTable* Projection::WeightsToTable(DataTable* dt, const String& col_nm_,
                                      bool recv_wts) {
  if(!(bool)from) return NULL;
  Network* net = layer->own_net;
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
    FOREACH_ELEM_IN_GROUP(Unit, ru, layer->units) {
      ConGroup* cg = ru->RecvConGroupPrjn(this);
      if(cg == NULL)
        break;
      dt->AddBlankRow();
      int wi;
      for(wi=0;wi<cg->size;wi++) {
        Unit* ou = cg->Un(wi, net);
        ou->LayerLogPos(log_pos);
        scol->SetMatrixVal(cg->Cn(wi,ConGroup::WT,net), -1, log_pos.x, log_pos.y);
      }
    }
  }
  else {
    DataCol* scol = dt->FindMakeColName(col_nm, idx, VT_FLOAT, 2,
                                      layer->flat_geom.x, layer->flat_geom.y);

    FOREACH_ELEM_IN_GROUP(Unit, ru, from->units) {
      ConGroup* cg = ru->SendConGroupPrjn(this);
      if(cg == NULL)
        break;
      dt->AddBlankRow();
      int wi;
      for(wi=0;wi<cg->size;wi++) {
        Unit* ou = cg->Un(wi, net);
        ou->LayerLogPos(log_pos);
        scol->SetMatrixVal(cg->Cn(wi,ConGroup::WT,net), -1, log_pos.x, log_pos.y);
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
  taBase::Own(nm, this);
  nm.AddProjection(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, net);
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
  if(!(bool)layer) return NULL;
  bool new_table = false;
  if(!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  layer->ConVarsToTable(dt, var1, var2, var3, var4, var5, var6, var7, var8,
                        var9, var10, var11, var12, var13, var14, this);
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

void Projection::ToggleOff() {
  off = !off;
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
  UpdateConSpecs();
}

bool Projection::UpdateConSpecs(bool force) {
  if((!(bool)layer) || (!(bool)from)) return false;
  Network* mynet = GET_MY_OWNER(Network);
  if(!mynet || !mynet->HasNetFlag(Network::BUILT_INTACT)) return false;
  ConSpec* sp = con_spec.SPtr();
  if(sp) {
    if(TestWarning(!con_type->InheritsFrom(sp->min_obj_type), "UpdateConSpec",
                   "connection type set to:",sp->min_obj_type->name,
                   "as required by the connection spec:", sp->name)) {
      con_type = sp->min_obj_type;
    }
  }
  if(!force && (sp == m_prv_con_spec)) return false;
  if(!sp) return false;
  m_prv_con_spec = sp;          // don't redo it
  FOREACH_ELEM_IN_GROUP(Unit, u, layer->units) {
    for(int g=0; g<u->NRecvConGps(); g++) {
      ConGroup* recv_gp = u->RecvConGroup(g);
      if(recv_gp->prjn == this) {
        recv_gp->SetConSpec(sp);
      }
    }
  }
  // also do the from!
  FOREACH_ELEM_IN_GROUP(Unit, u, from->units) {
    int g;
    for(g=0; g<u->NSendConGps(); g++) {
      ConGroup* send_gp = u->SendConGroup(g);
      if(send_gp->prjn == this) {
        send_gp->SetConSpec(sp);
      }
    }
  }
  return true;
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
  return UpdateConSpecs();
}

bool Projection::SetConType(TypeDef* td) {
  if(con_type == td) return false;
  projected = false;
  con_type = td;
  return true;
}

void Projection::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

void Projection::SaveWeights(const String& fname) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    layer->SaveWeights_strm(*flr->ostrm, ConGroup::TEXT, this);
  flr->Close();
  taRefN::unRefDone(flr);
}

bool Projection::LoadWeights(const String& fname, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  bool rval = false;
  if(flr->istrm) {
    rval = layer->LoadWeights_strm(*flr->istrm, ConGroup::TEXT, quiet, this);
  }
  else {
    TestError(true, "LoadWeights", "aborted due to inability to load weights file");
    // the above should be unnecessary but we're not getting the error sometimes..
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

int Projection::ReplaceConSpec(ConSpec* old_sp, ConSpec* new_sp) {
  if(con_spec.SPtr() != old_sp) return 0;
  con_spec.SetSpec(new_sp);
  UpdateConSpecs();
  return 1;
}

int Projection::ReplacePrjnSpec(ProjectionSpec* old_sp, ProjectionSpec* new_sp) {
  if(spec.SPtr() != old_sp) return 0;
  spec.SetSpec(new_sp);
  return 1;
}

void Projection::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  CheckSpecs();                 // just check!
}

void Projection::Copy_Weights(const Projection* src) {
  Unit* u, *su;
  taLeafItr i,si;
  for(u = (Unit*)layer->units.FirstEl(i), su = (Unit*)src->layer->units.FirstEl(si);
      (u) && (su);
      u = (Unit*)layer->units.NextEl(i), su = (Unit*)src->layer->units.NextEl(si))
  {
    u->Copy_Weights(su, this);
  }
}

void Projection::TransformWeights(const SimpleMathSpec& trans) {
  FOREACH_ELEM_IN_GROUP(Unit, u, layer->units) {
    u->TransformWeights(trans, this);
  }
}

void Projection::RescaleWeights(const float rescale_factor) {
  FOREACH_ELEM_IN_GROUP(Unit, u, layer->units) {
    u->RescaleWeights(rescale_factor, this);
  }
}

void Projection::AddNoiseToWeights(const Random& noise_spec) {
  FOREACH_ELEM_IN_GROUP(Unit, u, layer->units) {
    u->AddNoiseToWeights(noise_spec, this);
  }
}

int Projection::PruneCons(const SimpleMathSpec& pre_proc,
                              Relation::Relations rel, float cmp_val)
{
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Unit, u, layer->units) {
    rval += u->PruneCons(pre_proc, rel, cmp_val, this);
  }
  return rval;
}

int Projection::LesionCons(float p_lesion, bool permute) {
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Unit, u, layer->units) {
    rval += u->LesionCons(p_lesion, permute, this);
  }
  return rval;
}

