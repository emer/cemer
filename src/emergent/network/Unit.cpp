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

#include "Unit.h"
#include <Network>
#include <NetMonitor>
#include <MemberDef>
#include <SimpleMathSpec>
#include <taFiler>
#include <taProject>
#include <DataTable>

#include <taMisc>
#include <tabMisc>

TA_BASEFUNS_CTORS_DEFN(Unit);

using namespace std;

void Unit::Initialize() {
  flags = UF_NONE;
  wt_prjn = tmp_calc1 = 0.0f;
  snap = 0.0f;
  n_recv_cons = 0;
  n_send_cons = 0;
  // pos = ??
  idx = -1;
  flat_idx = 0;
  lay_un_idx = 0;
  gp_idx = -1;
  ungp_un_idx = 0;
  
  thread_no = 0;
  thr_un_idx = 0;
  own_lay_idx = 0;
  own_ungp_idx = 0;
  in_subgp = false;
  voxels = NULL;
  m_unit_spec = NULL;
  m_own_net = NULL;
}

void Unit::Destroy() {
  CutLinks();
}

void Unit::InitLinks() {
  inherited::InitLinks();
  taBase::Own(pos, this);
  m_own_net = GET_MY_OWNER(Network);
  GetInSubGp();
}

void Unit::CutLinks() {
  m_unit_spec = NULL;
  idx = -1;
  flat_idx = 0;
  gp_idx = -1;
  in_subgp = false;
  if(voxels) {
    taBase::DelPointer((taBase**)&voxels);
  }
  inherited::CutLinks();
}

UnitState_cpp* Unit::MyUnitState() const {
  return GetUnitState();
}

void Unit::MakeVoxelsList() {
  if(voxels) return;
  taBase::OwnPointer((taBase**)&voxels, new Voxel_List, this);
}

void Unit::Copy_(const Unit& cp) {
  pos = cp.pos;
  wt_prjn = cp.wt_prjn;
  tmp_calc1 = cp.tmp_calc1;
  snap = cp.snap;
  n_recv_cons = cp.n_recv_cons;
  n_send_cons = cp.n_send_cons;
  m_unit_spec = cp.m_unit_spec;
}

void Unit::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // no negative positions
  pos.x = MAX(0,pos.x);
  pos.y = MAX(0,pos.y);
  pos.z = MAX(0,pos.z);
  // stay within layer->un_geom
//   Layer* lay = GET_MY_OWNER(Layer);
//   if(!lay) return;
//   pos.x = MIN(lay->un_geom.x-1,pos.x); pos.y = MIN(lay->un_geom.y-1,pos.y);
  pos.z = 0;                    // always zero: can't go out of plane
}

void Unit::Lesion() {
  if(lesioned()) return;
  StructUpdate(true);
  SetUnitFlag(LESIONED);
  GetUnitState()->SetExtFlag(UnitState_cpp::LESIONED);
  // UpdtActiveCons();
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Unit::UnLesion()  {
  if(!lesioned()) return;
  StructUpdate(true);
  ClearUnitFlag(LESIONED);
  GetUnitState()->ClearExtFlag(UnitState_cpp::LESIONED);
  // UpdtActiveCons();
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Unit::UpdtAfterNetModIfNecc() {
  if(!owner || owner->InStructUpdate()) return;
  if(!own_lay() || own_lay()->InStructUpdate()) return;
  if(!own_net() || own_net()->InStructUpdate()) return;
  own_net()->UpdtAfterNetMod();
}

void Unit::LayerLogPos(taVector2i& log_pos) {
  Layer* mlay = own_lay();
  if(mlay) {
    mlay->UnitLogPos(this, log_pos);
  }
}

void Unit::UnitGpLogPos(taVector2i& log_pos) {
  Layer* mlay = own_lay();
  if(mlay) {
    mlay->UnitInGpLogPos(this, log_pos);
  }
}

void Unit::LayerDispPos(taVector2i& disp_pos) {
  Layer* mlay = own_lay();
  if(mlay) {
    mlay->UnitDispPos(this, disp_pos);
  }
}

void Unit::AddRelPos(taVector3i& rel_pos) {
  Unit_Group* ugp = GET_MY_OWNER(Unit_Group);
  if (ugp) {
    rel_pos += ugp->pos;
    ugp->AddRelPos(rel_pos);
  }
}

void Unit::AddRelPos2d(taVector2i& rel_pos) {
  Unit_Group* ugp = GET_MY_OWNER(Unit_Group);
  if (ugp) {
    rel_pos += ugp->pos;
    ugp->AddRelPos2d(rel_pos);
  }
}


void Unit::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  // recv cons are now checked at the projection level, not unit level
}

void Unit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  if(CheckError(!GetUnitSpec(), quiet, rval, "No unit spec set for unit")) {
    return;                     // fatal
  }
}

bool Unit::CheckBuild(bool quiet) {
  bool rval;
  if(CheckError(!GetUnitSpec(), quiet, rval,
                "Unit CheckBuild: no unit spec set for unit")) {
    return false;               // fatal
  }
  UnitSpec* us = GetUnitSpec();
  return true;
}

void Unit::SetUnitSpec(UnitSpec* us) {
  m_unit_spec = us;
  if(!own_net() || !own_net()->net_state) return;
  if(own_net()->net_state->n_units_built == 0 || own_net()->net_state->units_thrs == NULL) return;
  GetUnitState()->spec_idx = us->spec_idx; // keep synchronized!
}

void Unit::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

bool Unit::Snapshot(const String& var, SimpleMathSpec& math_op, bool arg_is_snap) {
  Network* net = own_net();

  Variant val = 0.0f;
  if(var.startsWith("r.") || var.startsWith("s.")) {
    Unit* src_u = NULL;
    if(own_lay() && own_lay()->own_net)
      src_u = own_lay()->own_net->GetViewSrcU();
    if(!src_u) return false;
    String cvar = var.after(".");
    bool is_send = var.startsWith("s.");
    if(is_send) {
      const int rsz = NRecvConGps();
      for(int g=0;g<rsz;g++) {
        ConState_cpp* tcong = RecvConState(g);
        if(tcong->NotActive()) continue;
        MemberDef* act_md = tcong->ConType(net)->members.FindName(cvar);
        if(!act_md) continue;
        int con = tcong->FindConFromIdx(src_u->flat_idx);
        if(con < 0) continue;
        val = tcong->Cn(con, act_md->idx, net->net_state);
        break;
      }
    }
    else {
      const int ssz = NSendConGps();
      for(int g=0;g<ssz;g++) {
        ConState_cpp* tcong = SendConState(g);
        if(tcong->NotActive()) continue;
        MemberDef* act_md = tcong->ConType(net)->members.FindName(cvar);
        if(!act_md)     continue;
        int con = tcong->FindConFromIdx(src_u->flat_idx);
        if(con < 0) continue;
        val = tcong->Cn(con, act_md->idx, net->net_state);
        break;
      }
    }
  }
  else {
    UnitState_cpp* uv = GetUnitState();
    TypeDef* ustd = net->UnitStateType();
    MemberDef* md = ustd->members.FindName(var);
    if(!md) {
      // if(TestWarning(!md, "Snapshot", "variable named:", var,
      //                "not found in unit variables")) {
      return false;
    }
    val = *((float*)md->GetOff(uv));
    if(val.isNull() || val.isInvalid()) return false;  // already warned
  }
  if(math_op.opr == SimpleMathSpec::NONE) {
    snap = val.toFloat();
  }
  else {
    if(arg_is_snap)
      math_op.arg = snap;
    snap = (float)math_op.Evaluate(val.toDouble());
  }
  return true;
}

bool Unit::SetUnValName(float val, const String& var_nm) {
  Network* net = own_net();
  if(TestError(!net->IsBuiltIntact(), "SetUnValName",
               "Network is not built or intact -- cannot access unit variables until built!")) {
    return false;
  }

  UnitState_cpp* uv = GetUnitState();
  TypeDef* ustd = net->UnitStateType();
  MemberDef* md = ustd->members.FindName(var_nm);
  if(TestError(!md, "SetUnValName", "variable named:", var_nm,
               "not found in unit variables, of type:", ustd->name)) {
    return false;
  }
  if(TestError(!md->type->IsFloat(), "SetUnValName", "variable named:", var_nm,
               "is not of float type -- must be -- is:", md->name)) {
    return false;
  }
  *((float*)md->GetOff(uv)) = val;
  return true;
}

float Unit::GetUnValName(const String& var_nm) {
  Network* net = own_net();
  if(TestError(!net->IsBuiltIntact(), "GetUnValName",
               "Network is not built or intact -- cannot access unit variables until built!")) {
    return 0.0f;
  }

  UnitState_cpp* uv = GetUnitState();
  TypeDef* ustd = net->UnitStateType();
  MemberDef* md = ustd->members.FindName(var_nm);
  if(TestError(!md, "GetUnValName", "variable named:", var_nm,
               "not found in unit variables, of type:", ustd->name)) {
    return 0.0f;
  }
  if(TestError(!md->type->IsFloat(), "GetUnValName", "variable named:", var_nm,
               "is not of float type -- must be -- is:", md->name)) {
    return 0.0f;
  }
  return *((float*)md->GetOff(uv));
}

ConState_cpp* Unit::FindRecvConStateFrom(Layer* fm_lay) const {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return NULL;
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* prjn = cg->GetPrjnState(net_state);
    if(prjn && prjn->send_lay_idx == fm_lay->layer_idx)
      return cg;
  }
  return NULL;
}

ConState_cpp* Unit::FindRecvConStateFromName(const String& fm_nm) const {
  Layer* lay = own_lay();
  if(!lay) return NULL;
  for(int g = 0; g < lay->projections.size; g++) {
    Projection* prjn = lay->projections[g];
    if(prjn->from->name == fm_nm) {
      ConState_cpp* cg = RecvConState(prjn->recv_idx);
      return cg;
    }
  }
  return NULL;
}

ConState_cpp* Unit::FindSendConStateToName(const String& to_nm) const {
  Layer* lay = own_lay();
  if(!lay) return NULL;
  for(int g = 0; g < lay->send_prjns.size; g++) {
    Projection* prjn = lay->send_prjns[g];
    if(prjn->layer->name == to_nm) {
      ConState_cpp* cg = SendConState(prjn->send_idx);
      return cg;
    }
  }
  return NULL;
}

bool Unit::SetCnValName(float val, const Variant& prjn, int dx, const String& var_nm) {
  Network* net = own_net();
  if(TestError(!net->IsBuiltIntact(), "SetCnValName",
               "Network is not built or intact -- cannot access connection variables until built!")) {
    return false;
  }

  ConState_cpp* cg = NULL;
  if(prjn.isStringType()) {
    cg = FindRecvConStateFromName(prjn.toString());
  }
  else {
    int ridx =prjn.toInt();
    if(ridx < NRecvConGps()) {
      cg = RecvConState(ridx);
    }
  }
  if(TestError(!cg, "SetCnValName", "recv projection not found from:", prjn.toString())) {
    return false;
  }
  return cg->SetCnValName(net, val, dx, var_nm);
}
  
float Unit::GetCnValName(const Variant& prjn, int dx, const String& var_nm) {
  Network* net = own_net();
  if(TestError(!net->IsBuiltIntact(), "GetCnValName",
               "Network is not built or intact -- cannot access connection variables until built!")) {
    return 0.0f;
  }
  ConState_cpp* cg = NULL;
  if(prjn.isStringType()) {
    cg = FindRecvConStateFromName(prjn.toString());
  }
  else {
    int ridx =prjn.toInt();
    if(ridx < NRecvConGps()) {
      cg = RecvConState(ridx);
    }
  }
  if(TestError(!cg, "GetCnValName", "recv projection not found from:", prjn.toString())) {
    return false;
  }
  return cg->SafeCnName(net, dx, var_nm);
}

int Unit::NRecvConGpsSafe() const {
  return own_net()->UnNRecvConGpsSafe(flat_idx);
}

int Unit::NSendConGpsSafe() const {
  return own_net()->UnNSendConGpsSafe(flat_idx);
}

ConState_cpp* Unit::RecvConStateSafe(int rcg_idx) const {
  return own_net()->RecvConStateSafe(flat_idx, rcg_idx);
}

ConState_cpp* Unit::SendConStateSafe(int scg_idx) const {
  return own_net()->SendConStateSafe(flat_idx, scg_idx);
}

ConState_cpp* Unit::RecvConStatePrjnSafe(Projection* prjn) const {
  return RecvConStateSafe(prjn->recv_idx);
}

ConState_cpp* Unit::SendConStatePrjnSafe(Projection* prjn) const {
  return SendConStateSafe(prjn->send_idx);
}

void Unit::GetInSubGp() {
  Unit_Group* ownr = (Unit_Group*)owner;
  if((ownr != NULL) && (ownr->owner != NULL) && ownr->owner->InheritsFrom(TA_taSubGroup))
    in_subgp = true;
  else
    in_subgp = false;
}

void Unit::Copy_Weights(const Unit* src, Projection* prjn) {
  Network* net = own_net();
  bias_wt() = ((Unit*)src)->bias_wt();
  const int rsz = NRecvConGps();
  const int srsz = src->NRecvConGps();
  int mx = MIN(rsz, srsz);
  for(int i=0; i<mx; i++) {
    ConState_cpp* cg = RecvConState(i);
    ConState_cpp* scg = src->RecvConState(i);
    if(cg->NotActive()) continue;
    if(prjn) {
      PrjnState_cpp* pj = cg->GetPrjnState(net->net_state);
      if(!pj) continue;
      if(pj->prjn_idx != prjn->prjn_idx) continue;
    }
    cg->Copy_Weights(scg, net->net_state);
  }
}

void Unit::GetLocalistName() {
  if(name.nonempty()) return;   // only if not otherwise named!
  Network* net = own_net();
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    if(cg->NotActive()) continue;
    if(cg->size != 1) continue; // only 1-to-1
    Unit* un = net->UnitFromState(cg->UnState(0,net->net_state));
    if(!un->name.empty()) {
      SetName(un->name);
      break;                    // done!
    }
  }
}

void Unit::TransformWeights(const SimpleMathSpec& trans, Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->TransformWeights(net_state, trans);
  }
}

void Unit::RenormWeights(bool mult_norm, float avg_wt, Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->RenormWeights(net_state, mult_norm, avg_wt);
  }
}

void Unit::RescaleWeights(const float rescale_factor, Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->RescaleWeights(net_state, rescale_factor);
  }
}

void Unit::AddNoiseToWeights(const Random& noise_spec, Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->AddNoiseToWeights(net_state, noise_spec);
  }
}

int Unit::PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel,
                       float cmp_val, Projection* prjn)
{
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return 0;
  int rval = 0;
  int g;
  const int rsz = NRecvConGps();
  for(g=0; g<rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    rval += cg->PruneCons(this, pre_proc, rel, cmp_val);
  }
  //  n_recv_cons -= rval;
  return rval;
}

int Unit::LesionCons(float p_lesion, bool permute, Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return 0;
  int rval = 0;
  const int rsz = NRecvConGps();
  for(int g=0; g<rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    rval += cg->LesionCons(this, p_lesion, permute);
  }
  //  n_recv_cons -= rval;
  return rval;
}

DataTable* Unit::VarToTable(DataTable* dt, const String& variable) {
  bool new_table = false;
  if (!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable("Unit_Var_" + variable, true);
    new_table = true;
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return NULL;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.SetDataNetwork(dt, net);
  nm.AddUnit(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Unit::ConVarsToTable(DataTable* dt, const String& var1, const String& var2,
                          const String& var3, const String& var4, const String& var5,
                          const String& var6, const String& var7, const String& var8,
                          const String& var9, const String& var10, const String& var11,
                          const String& var12, const String& var13, const String& var14,
                          Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return NULL;
  bool new_table = false;
  if(!dt) {
    taProject* proj = GetMyProj();
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  const int rsz = NRecvConGps();
  for(int g=0; g<rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->ConVarsToTable(dt, this, net, var1, var2, var3, var4, var5, var6, var7, var8,
                       var9, var10, var11, var12, var13, var14);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

bool Unit::ChangeMyType(TypeDef*) {
  TestError(true, "ChangeMyType", "Cannot change type of Units -- change el_typ in Layer units group  and rebuild network instead");
  return false;
}
