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
  thr_un_idx = 0;
  ug_idx = -1;
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
  ug_idx = -1;
  in_subgp = false;
  if(voxels) {
    taBase::DelPointer((taBase**)&voxels);
  }
  inherited::CutLinks();
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
  GetUnitVars()->SetExtFlag(UnitVars::LESIONED);
  UpdtActiveCons();
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Unit::UnLesion()  {
  if(!lesioned()) return;
  StructUpdate(true);
  ClearUnitFlag(LESIONED);
  GetUnitVars()->ClearExtFlag(UnitVars::LESIONED);
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

int Unit::GetMyLeafIndex() {
  if(idx < 0 || !owner) return idx;
  Unit_Group* ug = (Unit_Group*)owner;
  if(!in_subgp)
    return idx; // simple: we're the only unit group
  // note: this assumes only one layer of subgroups, which is all that is supported anyway
  return ug->idx * ug->size + idx; // our unit group index within its owning list, times number of items per group (presumably same throughout), plus our own index..
}

void Unit::LayerLogPos(taVector2i& log_pos) {
  Layer* mlay = own_lay();
  if(mlay) {
    mlay->UnitLogPos(this, log_pos);
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

void Unit::ApplyInputData(float val, UnitVars::ExtFlags act_ext_flags, Random* ran,
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

  UnitVars* uv = GetUnitVars();
  if (act_ext_flags & UnitVars::EXT) {
    uv->ext = val;
    uv->SetExtFlag(UnitVars::EXT);
  }
  else {
    uv->targ = val;
    if (act_ext_flags & UnitVars::TARG)
      uv->SetExtFlag(UnitVars::TARG);
    else if (act_ext_flags & UnitVars::COMP)
      uv->SetExtFlag(UnitVars::COMP);
  }
}

void Unit::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  // recv.CheckConfig(quiet, rval);
  // send.CheckConfig(quiet, rval);
}

void Unit::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  if(CheckError(!GetUnitSpec(), quiet, rval, "No unit spec set for unit")) {
    return;                     // fatal
  }
  if(!GetUnitSpec()->CheckConfig_Unit(this, quiet)) {
    rval = false;
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
  if(!own_net()) return;
  if(own_net()->n_units_built == 0 || own_net()->units_thrs == NULL) return;
  GetUnitVars()->unit_spec = us; // keep synchronized!
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
        ConGroup* tcong = RecvConGroup(g);
        if(tcong->NotActive()) continue;
        MemberDef* act_md = tcong->ConType()->members.FindName(cvar);
        if(!act_md) continue;
        int con = tcong->FindConFromIdx(src_u);
        if(con < 0) continue;
        val = tcong->Cn(con, act_md->idx, net);
        break;
      }
    }
    else {
      const int ssz = NSendConGps();
      for(int g=0;g<ssz;g++) {
        ConGroup* tcong = SendConGroup(g);
        if(tcong->NotActive()) continue;
        MemberDef* act_md = tcong->ConType()->members.FindName(cvar);
        if(!act_md)     continue;
        int con = tcong->FindConFromIdx(src_u);
        if(con < 0) continue;
        val = tcong->Cn(con, act_md->idx, net);
        break;
      }
    }
  }
  else {
    MemberDef* md = NULL;
    val = GetValFromPath(var, md, true); // true = warn
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

ConGroup* Unit::FindRecvConGroupFrom(Layer* fm_lay) const {
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->prjn && cg->prjn->from.ptr() == fm_lay)
      return cg;
  }
  return NULL;
}

ConGroup* Unit::FindRecvConGroupFromName(const String& fm_nm) const {
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->prjn && cg->prjn->from->name == fm_nm)
      return cg;
  }
  return NULL;
}

ConGroup* Unit::RecvConGroupPrjn(Projection* prjn) {
  if(TestError(prjn->recv_idx < 0, "CheckPrjnRecvIdx",
               "Projection recv_idx is -1 -- attempt to connect to an inactive projection -- programmer error probably -- please report!"))
    return NULL;
  if(TestError(prjn->recv_idx >= NRecvConGps(), "CheckPrjnRecvIdx",
               "Projection recv_idx is >= number of allocated recv con groups for this u nit -- programmer error probably -- please report!"))
    return NULL;
  return RecvConGroup(prjn->recv_idx);
}

ConGroup* Unit::SendConGroupPrjn(Projection* prjn) {
  if(TestError(prjn->send_idx < 0, "CheckPrjnSendIdx",
               "Projection send_idx is -1 -- attempt to connect to an inactive projection -- programmer error probably -- please report!"))
    return NULL;
  if(TestError(prjn->send_idx >= NSendConGps(), "CheckPrjnSendIdx",
               "Projection send_idx is >= number of allocated send con groups for this u nit -- programmer error probably -- please report!"))
    return NULL;
  return SendConGroup(prjn->send_idx);
}

void Unit::RecvConsPreAlloc(int no, Projection* prjn) {
  ConGroup* cgp = RecvConGroupPrjn(prjn);
  if(cgp)
    cgp->AllocCons(no);
}

void Unit::SendConsPreAlloc(int no, Projection* prjn) {
  ConGroup* cgp = SendConGroupPrjn(prjn);
  if(cgp)
    cgp->AllocCons(no);
}

void Unit::SendConsAllocInc(int no, Projection* prjn) {
  ConGroup* cgp = SendConGroupPrjn(prjn);
  if(cgp)
    cgp->ConnectAllocInc(no);
}

void Unit::SendConsPostAlloc(Projection* prjn) {
  ConGroup* cgp = SendConGroupPrjn(prjn);
  if(cgp)
    cgp->AllocConsFmSize();
}

void Unit::RecvConsAllocInc(int no, Projection* prjn) {
  ConGroup* cgp = RecvConGroupPrjn(prjn);
  if(cgp)
    cgp->ConnectAllocInc(no);
}

void Unit::RecvConsPostAlloc(Projection* prjn) {
  ConGroup* cgp = RecvConGroupPrjn(prjn);
  if(cgp)
    cgp->AllocConsFmSize();
}

int Unit::ConnectFrom(Unit* su, Projection* prjn, bool alloc_send,
                      bool ignore_alloc_errs, bool set_init_wt, float init_wt) {
  ConGroup* rcgp = RecvConGroupPrjn(prjn);
  if(!rcgp) return -1;
  ConGroup* scgp = su->SendConGroupPrjn(prjn);
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
  ConGroup* rcgp = RecvConGroupPrjn(prjn);
  if(!rcgp) return -1;
  ConGroup* scgp = SendConGroupPrjn(prjn);
  if(!scgp) return -1;

  if(rcgp->FindConFromIdx(su) >= 0) // already connected!
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

  ConGroup* rcgp = RecvConGroupPrjn(prjn);
  if(!rcgp) return false;
  ConGroup* scgp = su->SendConGroupPrjn(prjn);
  if(!scgp) return false;

  rcgp->RemoveConUn(su, this, net);
  return scgp->RemoveConUn(this, su, net);
}

void Unit::DisConnectAll() {
  ConGroup* recv_gp;
  ConGroup* send_gp;
  Network* net = own_net();
  int g;
  int i;
  const int rsz = NRecvConGps();
  for(g=0; g<rsz; g++) { // the removes cause the leaf_gp to crash..
    recv_gp = RecvConGroup(g);
    for(i=recv_gp->size-1; i>=0; i--) {
      Unit* su = recv_gp->Un(i,net);
      if(recv_gp->other_idx >= 0)
        send_gp = su->SendConGroup(recv_gp->other_idx);
      else
        send_gp = NULL;
      if(send_gp)
        send_gp->RemoveConUn(this, su, net);
      recv_gp->RemoveConIdx(i, this, net);
    }
  }
  const int ssz = NSendConGps();
  for(g=0; g<ssz; g++) { // the removes cause the leaf_gp to crash..
    send_gp = SendConGroup(g);
    for(i=send_gp->size-1; i>=0; i--) {
      Unit* ru = send_gp->Un(i,net);
      if(send_gp->other_idx >= 0)
        recv_gp = ru->RecvConGroup(send_gp->other_idx);
      else
        recv_gp = NULL;
      if(recv_gp)
        recv_gp->RemoveConUn(this, ru, net);
      send_gp->RemoveConIdx(i, this, net);
    }
  }
  n_recv_cons = 0;
  n_send_cons = 0;
}

int Unit::CountCons(Network* net) {
  n_recv_cons = 0;
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive()) continue;
    n_recv_cons += cg->size;
  }
  n_send_cons = 0;
  const int ssz = NSendConGps();
  for(int g = 0; g < ssz; g++) {
    ConGroup* cg = SendConGroup(g);
    if(cg->NotActive()) continue;
    n_send_cons += cg->size;
  }
  return n_recv_cons;
}

void Unit::UpdtActiveCons() {
  if(lesioned()) {
    const int rsz = NRecvConGps();
    for(int g = 0; g < rsz; g++) {
      ConGroup* cg = RecvConGroup(g);
      cg->SetInactive();
    }
    const int ssz = NSendConGps();
    for(int g = 0; g < ssz; g++) {
      ConGroup* cg = SendConGroup(g);
      cg->SetInactive();
    }
  }
  else {
    const int rsz = NRecvConGps();
    for(int g = 0; g < rsz; g++) {
      ConGroup* cg = RecvConGroup(g);
      cg->UpdtIsActive();
    }
    const int ssz = NSendConGps();
    for(int g = 0; g < ssz; g++) {
      ConGroup* cg = SendConGroup(g);
      cg->UpdtIsActive();
    }
  }
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
    ConGroup* cg = RecvConGroup(i);
    ConGroup* scg = src->RecvConGroup(i);
    if(cg->NotActive() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->Copy_Weights(scg, net);
  }
}

void Unit::SaveWeights_strm(ostream& strm, Projection* prjn, ConGroup::WtSaveFormat fmt) {
  Network* net = own_net();
  strm << "<Un>\n";
  float bwt = bias_wt();
  // always write this for a consistent format
  switch(fmt) {
  case ConGroup::TEXT:
    strm << bwt << "\n";
    break;
  case ConGroup::BINARY:
    strm.write((char*)&(bwt), sizeof(bwt));
    strm << "\n";
    break;
  }
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive() || (prjn && (cg->prjn != prjn))) continue;
    strm << "<Cg " << g << " Fm:" << cg->prjn->from->name << ">\n";
    cg->SaveWeights_strm(strm, this, net, fmt);
    strm << "</Cg>\n";
  }
  strm << "</Un>\n";
}

int Unit::LoadWeights_strm(istream& strm, Projection* prjn, ConGroup::WtSaveFormat fmt, bool quiet) {
  Network* net = own_net();
  String tag, val;
  int stat = ConGroup::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  //   String lidx = val.before(' ');
  // todo: could compare lidx with GetMyLeafIdx()...
  float bwt = 0.0;
  switch(fmt) {
  case ConGroup::TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case ConGroup::BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  bias_wt() = bwt;

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;                  // *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    int gi = (int)val.before(' ');
    String fm = val.after("Fm:");
    ConGroup* cg = NULL;
    const int rsz = NRecvConGps();
    if(rsz > gi) {
      cg = RecvConGroup(gi);
      if(cg->prjn->from->name != fm)
        cg = FindRecvConGroupFromName(fm);
    }
    else {
      cg = FindRecvConGroupFromName(fm);
    }
    if(cg) {
      stat = cg->LoadWeights_strm(strm, this, net, fmt, quiet);
    }
    else {
      stat = ConGroup::SkipWeights_strm(strm, fmt, quiet); // skip over
    }
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    ConGroup::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }

  ConGroup::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

int Unit::SkipWeights_strm(istream& strm, ConGroup::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = ConGroup::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  float bwt = 0.0;
  switch(fmt) {
  case ConGroup::TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case ConGroup::BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    stat = ConGroup::SkipWeights_strm(strm, fmt, quiet); // skip over
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    ConGroup::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  ConGroup::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

void Unit::GetLocalistName() {
  if(name.nonempty()) return;   // only if not otherwise named!
  Network* net = own_net();
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive()) continue;
    if(cg->size != 1) continue; // only 1-to-1
    Unit* un = cg->Un(0,net);
    if(!un->name.empty()) {
      SetName(un->name);
      break;                    // done!
    }
  }
}

void Unit::TransformWeights(const SimpleMathSpec& trans, Projection* prjn) {
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->TransformWeights(trans);
  }
}

void Unit::AddNoiseToWeights(const Random& noise_spec, Projection* prjn) {
  const int rsz = NRecvConGps();
  for(int g = 0; g < rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->AddNoiseToWeights(noise_spec);
  }
}

int Unit::PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel,
                       float cmp_val, Projection* prjn)
{
  int rval = 0;
  int g;
  const int rsz = NRecvConGps();
  for(g=0; g<rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive() || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->PruneCons(this, pre_proc, rel, cmp_val);
  }
  //  n_recv_cons -= rval;
  return rval;
}

int Unit::LesionCons(float p_lesion, bool permute, Projection* prjn) {
  int rval = 0;
  const int rsz = NRecvConGps();
  for(int g=0; g<rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive() || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->LesionCons(this, p_lesion, permute);
  }
  //  n_recv_cons -= rval;
  return rval;
}

DataTable* Unit::VarToTable(DataTable* dt, const String& variable) {
  bool new_table = false;
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("Unit_Var_" + variable, true);
    new_table = true;
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return NULL;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddUnit(this, variable);
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

DataTable* Unit::ConVarsToTable(DataTable* dt, const String& var1, const String& var2,
                          const String& var3, const String& var4, const String& var5,
                          const String& var6, const String& var7, const String& var8,
                          const String& var9, const String& var10, const String& var11,
                          const String& var12, const String& var13, const String& var14,
                          Projection* prjn) {
  Network* net = own_net();
  bool new_table = false;
  if(!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  const int rsz = NRecvConGps();
  for(int g=0; g<rsz; g++) {
    ConGroup* cg = RecvConGroup(g);
    if(cg->NotActive() || ((prjn) && (cg->prjn != prjn))) continue;
    // todo:
    // cg->ConVarsToTable(dt, this, net, var1, var2, var3, var4, var5, var6, var7, var8,
    //                    var9, var10, var11, var12, var13, var14);
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
