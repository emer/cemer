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
  BuildUnits();
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

void Unit::BuildUnits() {
}

void Unit::Lesion() {
  if(lesioned()) return;
  StructUpdate(true);
  SetUnitFlag(LESIONED);
  GetUnitState()->SetExtFlag(UnitState_cpp::LESIONED);
  UpdtActiveCons();
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Unit::UnLesion()  {
  if(!lesioned()) return;
  StructUpdate(true);
  ClearUnitFlag(LESIONED);
  GetUnitState()->ClearExtFlag(UnitState_cpp::LESIONED);
  UpdtActiveCons();
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

void Unit::ApplyInputData(float val, UnitState_cpp::ExtFlags act_ext_flags, Random* ran,
                          bool na_by_range) {
  // note: not all flag values are valid, so following is a fuzzy cascade
  // ext is the default place, so we check for
  if(na_by_range) {
    UnitSpec* us = GetUnitSpec();
    if(us) {
      if(!us->act_range.RangeTestEq(val))
        return;
    }
  }

  if (ran && (ran->type != Random::NONE)) {
    val += ran->Gen();
  }

  UnitState_cpp* uv = GetUnitState();
  if (act_ext_flags & UnitState_cpp::EXT) {
    uv->ext = val;
    uv->SetExtFlag(UnitState_cpp::EXT);
  }
  else {
    uv->targ = val;
    if (act_ext_flags & UnitState_cpp::TARG)
      uv->SetExtFlag(UnitState_cpp::TARG);
    else if (act_ext_flags & UnitState_cpp::COMP)
      uv->SetExtFlag(UnitState_cpp::COMP);
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

void Unit::RecvConsPreAlloc(int no, Projection* prjn) {
  ConState_cpp* cgp = RecvConStatePrjnSafe(prjn);
  if(cgp)
    cgp->AllocCons(own_net()->net_state, no);
}

void Unit::SendConsPreAlloc(int no, Projection* prjn) {
  ConState_cpp* cgp = SendConStatePrjnSafe(prjn);
  if(cgp)
    cgp->AllocCons(own_net()->net_state, no);
}

void Unit::SendConsAllocInc(int no, Projection* prjn) {
  ConState_cpp* cgp = SendConStatePrjnSafe(prjn);
  if(cgp)
    cgp->ConnectAllocInc(no);
}

void Unit::SendConsPostAlloc(Projection* prjn) {
  ConState_cpp* cgp = SendConStatePrjnSafe(prjn);
  if(cgp)
    cgp->AllocConsFmSize(own_net()->net_state);
}

void Unit::RecvConsAllocInc(int no, Projection* prjn) {
  ConState_cpp* cgp = RecvConStatePrjnSafe(prjn);
  if(cgp)
    cgp->ConnectAllocInc(no);
}

void Unit::RecvConsPostAlloc(Projection* prjn) {
  ConState_cpp* cgp = RecvConStatePrjnSafe(prjn);
  if(cgp)
    cgp->AllocConsFmSize(own_net()->net_state);
}

int Unit::ConnectFrom(Unit* su, Projection* prjn, bool alloc_send,
                      bool ignore_alloc_errs, bool set_init_wt, float init_wt) {
  ConState_cpp* rcgp = RecvConStatePrjnSafe(prjn);
  if(!rcgp) return -1;
  ConState_cpp* scgp = su->SendConStatePrjnSafe(prjn);
  if(!scgp) return -1;

  if(alloc_send) {
    scgp->ConnectAllocInc(); // just do alloc increment
    return -1;
  }

  int con = rcgp->ConnectUnits(this, su, scgp, ignore_alloc_errs,
                               set_init_wt, init_wt);
  return con;
}

int Unit::ConnectFromCk(Unit* su, Projection* prjn,
                        bool ignore_alloc_errs, bool set_init_wt, float init_wt) {
  ConState_cpp* rcgp = RecvConStatePrjnSafe(prjn);
  if(!rcgp) return -1;
  ConState_cpp* scgp = su->SendConStatePrjnSafe(prjn);
  if(!scgp) return -1;

  if(rcgp->FindConFromIdx(su->flat_idx) >= 0) // already connected!
    return -1;

  int con = rcgp->ConnectUnits(this, su, scgp, ignore_alloc_errs,
                               set_init_wt, init_wt);
  return con;
}

bool Unit::DisConnectFrom(Unit* su, Projection* prjn) {
  Network* net = own_net();

  if(!prjn) {
    Layer* su_lay = su->own_lay();
    prjn = own_lay()->projections.FindPrjnFrom(su_lay);
    if(!prjn)        return false;
  }

  ConState_cpp* rcgp = RecvConStatePrjnSafe(prjn);
  if(!rcgp) return false;
  ConState_cpp* scgp = su->SendConStatePrjnSafe(prjn);
  if(!scgp) return false;

  rcgp->RemoveConUn(su->flat_idx, net->net_state);
  return scgp->RemoveConUn(su->flat_idx, net->net_state);
}

void Unit::DisConnectAll() {
  ConState_cpp* recv_gp;
  ConState_cpp* send_gp;
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;
  int g;
  int i;
  const int rsz = NRecvConGps();
  for(g=0; g<rsz; g++) { // the removes cause the leaf_gp to crash..
    recv_gp = RecvConState(g);
    for(i=recv_gp->size-1; i>=0; i--) {
      UnitState_cpp* su = recv_gp->UnState(i,net_state);
      if(recv_gp->other_idx >= 0)
        send_gp = su->SendConState(net_state, recv_gp->other_idx);
      else
        send_gp = NULL;
      if(send_gp)
        send_gp->RemoveConUn(su->flat_idx, net_state);
      recv_gp->RemoveConIdx(i, net_state);
    }
  }
  const int ssz = NSendConGps();
  for(g=0; g<ssz; g++) { // the removes cause the leaf_gp to crash..
    send_gp = SendConState(g);
    for(i=send_gp->size-1; i>=0; i--) {
      UnitState_cpp* ru = send_gp->UnState(i,net_state);
      if(send_gp->other_idx >= 0)
        recv_gp = ru->RecvConState(net_state, send_gp->other_idx);
      else
        recv_gp = NULL;
      if(recv_gp)
        recv_gp->RemoveConUn(ru->flat_idx, net_state);
      send_gp->RemoveConIdx(i, net_state);
    }
  }
  n_recv_cons = 0;
  n_send_cons = 0;
}

int Unit::CountCons(Network* net) {
  n_recv_cons = 0;
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    if(cg->NotActive()) continue;
    n_recv_cons += cg->size;
  }
  n_send_cons = 0;
  const int ssz = NSendConGps();
  for(int g = 0; g < ssz; g++) {
    ConState_cpp* cg = SendConState(g);
    if(cg->NotActive()) continue;
    n_send_cons += cg->size;
  }
  return n_recv_cons;
}

void Unit::UpdtActiveCons() {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;
  if(lesioned()) {
    const int rsz = NRecvConGps();
    for(int g = 0; g < rsz; g++) {
      ConState_cpp* cg = RecvConState(g);
      cg->SetInactive();
    }
    const int ssz = NSendConGps();
    for(int g = 0; g < ssz; g++) {
      ConState_cpp* cg = SendConState(g);
      cg->SetInactive();
    }
  }
  else {
    const int rsz = NRecvConGps();
    for(int g = 0; g < rsz; g++) {
      ConState_cpp* cg = RecvConState(g);
      cg->UpdtIsActive(net_state);
    }
    const int ssz = NSendConGps();
    for(int g = 0; g < ssz; g++) {
      ConState_cpp* cg = SendConState(g);
      cg->UpdtIsActive(net_state);
    }
  }
}

bool Unit::ShareRecvConsFrom(Unit* shu, Projection* prjn) {
  ConState_cpp* rcgp = RecvConStatePrjnSafe(prjn);
  if(!rcgp) return false;
  Network* net = own_net();
  return rcgp->SetShareFrom(net->net_state, shu);
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

/////////////////////////////////////////////////////////////
//      Save/Load Weights

void Unit::ConsSaveWeights_strm(ostream& strm, ConState_cpp* cg, Unit* un, Network* net,
                                WtSaveFormat fmt) {
  if(cg->NotActive()) {
    strm << "<Cn 0>\n" << "</Cn>\n";
    return;
  }

  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;
  
  PrjnState_cpp* pj = cg->GetPrjnState(net->net_state);
  // todo! need a way to get projection
  Projection* prjn = NULL;
  TypeDef* ct = prjn->con_type;
  ConSpec* con_spec = prjn->GetConSpec();
  int n_vars = 0;
  MemberDef* smds[10];          // no more than 10!
  for(int i=0; i<ct->members.size; i++) {
    MemberDef* md = ct->members[i];
    if(con_spec->SaveConVarToWeights(net, cg, md)) {
      smds[n_vars++] = md;
    }
  }
  
  strm << "<Cn " << cg->size << ">\n";
  switch(fmt) {
  case TEXT:
    for(int i=0; i < cg->size; i++) {
      int lidx = cg->UnState(i,net_state)->lay_un_idx;
      strm << lidx;
      for(int mi=0; mi < n_vars; mi++) {
        strm << " " << cg->Cn(i,smds[mi]->idx,net_state);
      }
      strm << "\n";
    }
    break;
  case BINARY:
    for(int i=0; i < cg->size; i++) {
      int lidx = cg->UnState(i,net_state)->lay_un_idx;
      strm.write((char*)&(lidx), sizeof(lidx));
      for(int mi=0; mi < n_vars; mi++) {
        strm.write((char*)&(cg->Cn(i,smds[mi]->idx,net_state)), sizeof(float));
      }
    }
    strm << "\n";
    break;
  }
  strm << "</Cn>\n";
}

// return values:
// TAG_END = successfully got to end of thing;
// TAG_NONE = some kind of error
// TAG_EOF = EOF

int Unit::LoadWeights_StartTag(istream& strm, const String& tag, String& val,
                                   bool quiet) {
  String in_tag;
  int stat = taMisc::read_tag(strm, in_tag, val);
  if(stat == taMisc::TAG_END) return taMisc::TAG_NONE; // some other end -- not good
  if(stat != taMisc::TAG_GOT) {
    if(!quiet) taMisc::Warning("ConState::LoadWeights: bad read of start tag:", tag);
    return stat;
  }
  if(in_tag != tag) {
    if(!quiet) taMisc::Warning("ConState::LoadWeights: read different start tag:", in_tag,
                               "expecting:", tag);
    return taMisc::TAG_NONE; // bumping up against some other tag
  }
  return stat;
}

int Unit::LoadWeights_EndTag(istream& strm, const String& trg_tag, String& cur_tag,
                                 int& stat, bool quiet) {
  String val;
  if(stat != taMisc::TAG_END)   // haven't already hit the end
    stat = taMisc::read_tag(strm, cur_tag, val);
  if((stat != taMisc::TAG_END) || (cur_tag != trg_tag)) {
    if(!quiet) taMisc::Warning("ConState::LoadWeights: bad read of end tag:", trg_tag, "got:",
                               cur_tag, "stat:", String(stat));
    if(stat == taMisc::TAG_END) stat = taMisc::TAG_NONE;
  }
  return stat;
}

int Unit::ConsLoadWeights_strm(istream& strm, ConState_cpp* cg, Unit* ru, Network* net,
                               WtSaveFormat fmt, bool quiet) {
  static bool warned_already = false;
  static bool sz_warned_already = false;

  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) {
    return ConsSkipWeights_strm(strm, fmt, quiet); // bail
  }
  
  PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
  Projection* prjn = net->PrjnFromState(pjs);
  
  TypeDef* ct = prjn->con_type;
  ConSpec* con_spec = prjn->GetConSpec();
  if((prjn == NULL) || (!(bool)prjn->from) || !con_spec) {
    return ConsSkipWeights_strm(strm, fmt, quiet); // bail
  }
  String tag, val;
  int stat = Unit::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    if(!quiet) taMisc::Warning("LoadWeights_strm: read size < 0");
    return taMisc::TAG_NONE;
  }
  if(sz < cg->size) {
    if(!quiet && !sz_warned_already) {
      taMisc::Warning("LoadWeights_strm: weights file has fewer connections:", String(sz),
                      "than existing group size of:", String(cg->size));
      sz_warned_already = true;
    // doesn't really make sense to nuke these -- maybe add a flag overall to enable this
//     for(int i=size-1; i >= sz; i--) {
//       Unit* su = Un(i);
//       ru->DisConnectFrom(su, prjn);
//     }
    }
  }
  else if(sz > cg->size) {
    if(sz > cg->alloc_size) {
      if(!quiet && !sz_warned_already) {
        taMisc::Warning("LoadWeights_strm: weights file has more connections:", String(sz),
                  "than allocated size:",
                  String(cg->alloc_size), "-- only alloc_size will be loaded");
        sz_warned_already = true;
        sz = cg->alloc_size;
      }
    }
    else {
      if(!quiet && !sz_warned_already) {
        taMisc::Warning("LoadWeights_strm: weights file has more connections:", String(sz),
                  "than existing group size of:", String(cg->size),
                        "-- but these will fit within alloc_size and will be loaded");
        sz_warned_already = true;
      }
    }
  }
  else {
    sz_warned_already = false;
  }


  int n_vars = 0;
  MemberDef* smds[10];          // no more than 10!
  for(int i=0; i<ct->members.size; i++) {
    MemberDef* md = ct->members[i];
    if(con_spec->SaveConVarToWeights(net, cg, md)) {
      smds[n_vars++] = md;
    }
  }

  float wtvals[10];
  int n_wts_loaded = 0;

  for(int i=0; i < sz; i++) {   // using load size as key factor
    int lidx;
    if(fmt == TEXT) {
      taMisc::read_till_eol(strm);
      int vidx = 0;
      int last_ci = 0;
      const int lbln = taMisc::LexBuf.length();
      int ci;
      for(ci = 1; ci < lbln; ci++) {
        if(taMisc::LexBuf[ci] != ' ') continue;
        if(last_ci == 0) {
          lidx = (int)taMisc::LexBuf.before(ci);
        }
        else {
          wtvals[vidx++] = (float)taMisc::LexBuf.at(last_ci, ci-last_ci);
        }
        last_ci = ci+1;
      }
      if(ci > last_ci) {
        wtvals[vidx++] = (float)taMisc::LexBuf.at(last_ci, ci-last_ci);
      }
      n_wts_loaded = MIN(vidx, n_vars); // can't effectively load more than we can use!
    }
    else {                      // binary
      strm.read((char*)&(lidx), sizeof(lidx));
      for(int mi=0; mi<n_vars; mi++) { // no way to check! MUST be right format..
        strm.read((char*)&(wtvals[mi]), sizeof(float));
      }
      n_wts_loaded = n_vars;
    }
    Unit* su = prjn->from->units.Leaf(lidx);
    if(!su) {
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: unit at leaf index: ",
                        String(lidx), "not found in layer:", prjn->from->name,
                        "removing this connection");
        warned_already = true;
      }
      if(cg->size > i) {
        Unit* un = net->UnitFromState(cg->UnState(i,net_state));
        ru->DisConnectFrom(un, prjn); // remove this guy to keep total size straight
      }
      sz--;                            // now doing less..
      i--;
      continue;
    }
    ConState_cpp* send_gp = su->SendConStatePrjn(prjn);
    if(!send_gp) {
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: unit at leaf index: ",
                        String(lidx), "does not have proper send group:",
                        String(prjn->send_idx));
        warned_already = true;
      }
      if(cg->size > i) {
        Unit* un = net->UnitFromState(cg->UnState(i,net_state));
        ru->DisConnectFrom(un, prjn); // remove this guy to keep total size straight
      }
      sz--;                            // now doing less..
      i--;
      continue;
    }
    if(i >= cg->size) {             // new connection
      // too many msgs with this:
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: attempting to load beyond size of allocated connections -- cannot do this");
        warned_already = true;
      }
      //      ru->ConnectFromCk(su, prjn, false, true, wtval); // set init wt
    }
    else if(su != net->UnitFromState(cg->UnState(i,net_state))) {
      // not same unit -- note that at this point, the only viable strategy is to discon
      // all existing cons and start over, as otherwise everything will be hopelessly out
      // of whack
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: unit at index:",
                        String(i),
                        "in cons group does not match the loaded unit -- weights will be off");
        warned_already = true;
      }

      for(int mi=1; mi<n_wts_loaded; mi++) { // set non-weight params first!
        cg->Cn(i,smds[mi]->idx,net_state) = wtvals[mi];
      }
      con_spec->LoadWeightVal(wtvals[0], cg, i, net_state);

      // this is not viable:
      // for(int j=cg->size-1; j >= i; j--) {
      //   Unit* su = Un(j,net);
      //   ru->DisConnectFrom(su, prjn);
      // }
      // ru->ConnectFromCk(su, prjn, false, true, wtval); // set init wt
    }
    else {                      // all good normal case, just set the weights!
      warned_already = false;
      for(int mi=1; mi<n_wts_loaded; mi++) { // set non-weight params first!
        cg->Cn(i,smds[mi]->idx,net_state) = wtvals[mi];
      }
      con_spec->LoadWeightVal(wtvals[0], cg, i, net_state);
    }
  }
  Unit::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;                  // should be tag end!
}

int Unit::ConsSkipWeights_strm(istream& strm, WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = Unit::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    return taMisc::TAG_NONE;
  }

  for(int i=0; i < sz; i++) {
    int lidx;
    float wt;
    if(fmt == TEXT) {
      taMisc::read_till_eol(strm);
    }
    else {                      // binary
      strm.read((char*)&(lidx), sizeof(lidx));
      strm.read((char*)&(wt), sizeof(wt));
    }
  }
  Unit::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;
}

void Unit::SaveWeights_strm(ostream& strm, WtSaveFormat fmt, Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return;

  strm << "<Un>\n";
  float bwt = bias_wt();
  // always write this for a consistent format
  switch(fmt) {
  case TEXT:
    strm << bwt << "\n";
    break;
  case BINARY:
    strm.write((char*)&(bwt), sizeof(bwt));
    strm << "\n";
    break;
  }
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(g);
    PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
    Projection* cg_prjn = net->PrjnFromState(pjs);
    if(cg->NotActive() || (prjn && (cg_prjn != prjn)) || cg->Sharing()) continue;
    Layer* fm = cg_prjn->from;
    if(fm->lesioned()) continue;
    strm << "<Cg " << g << " Fm:" << cg_prjn->from->name << ">\n";
    ConsSaveWeights_strm(strm, cg, this, net, fmt);
    strm << "</Cg>\n";
  }
  strm << "</Un>\n";
}

int Unit::LoadWeights_strm(istream& strm, WtSaveFormat fmt, bool quiet, Projection* prjn) {
  Network* net = own_net();
  NetworkState_cpp* net_state = net->net_state;
  if(!net || !net_state) return 0;

  String tag, val;
  int stat = Unit::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  //   String lidx = val.before(' ');
  // todo: could compare lidx with GetMyLeafIdx()...
  float bwt = 0.0;
  switch(fmt) {
  case TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  
  UnitState_cpp* uv = GetUnitState();
  UnitSpec* us = GetUnitSpec();
  if(us) {
    us->LoadBiasWtVal(bwt, uv, net_state);
  }
  else {
    uv->bias_wt = bwt;
  }

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;                  // *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    int gi = (int)val.before(' ');
    String fm = val.after("Fm:");
    ConState_cpp* cg = NULL;
    const int rsz = NRecvConGps();
    if(rsz > gi) {
      cg = RecvConState(gi);
      PrjnState_cpp* pjs = cg->GetPrjnState(net_state);
      Projection* cg_prjn = net->PrjnFromState(pjs);
      if(cg_prjn->from->name != fm)
        cg = FindRecvConStateFromName(fm);
    }
    else {
      cg = FindRecvConStateFromName(fm);
    }
    if(cg) {
      stat = ConsLoadWeights_strm(strm, cg, this, net, fmt, quiet);
    }
    else {
      stat = ConsSkipWeights_strm(strm, fmt, quiet); // skip over
    }
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    Unit::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }

  Unit::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

int Unit::SkipWeights_strm(istream& strm, WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = Unit::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  float bwt = 0.0;
  switch(fmt) {
  case TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    stat = ConsSkipWeights_strm(strm, fmt, quiet); // skip over
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    Unit::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  Unit::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
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
