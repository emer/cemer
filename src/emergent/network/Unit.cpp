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

#include <taMisc>
#include <tabMisc>

using namespace std;

void Unit::Initialize() {
  flags = UF_NONE;
  ext_flag = NO_EXTERNAL;
  targ = 0.0f;
  ext = 0.0f;
  act = 0.0f;
  net = 0.0f;
  wt_prjn = tmp_calc1 = 0.0f;
  snap = 0.0f;
  // recv, send, bias = ??
  n_recv_cons = 0;
  // pos = ??
  idx = -1;
  flat_idx = 0;
  voxels = NULL;
  m_unit_spec = NULL;
}

void Unit::Destroy() {
  CutLinks();
}

void Unit::InitLinks() {
  inherited::InitLinks();
  taBase::Own(recv, this);      // always own your constitutents
  taBase::Own(send, this);
  taBase::Own(bias, this);
  taBase::Own(pos, this);
  BuildUnits();
}

void Unit::CutLinks() {
  recv.CutLinks();
  send.CutLinks();
  bias.CutLinks();
  m_unit_spec = NULL;
  idx = -1;
  flat_idx = 0;
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
  ext_flag = cp.ext_flag;
  targ = cp.targ;
  ext = cp.ext;
  act = cp.act;
  net = cp.net;
  wt_prjn = cp.wt_prjn;
  tmp_calc1 = cp.tmp_calc1;
  snap = cp.snap;
  recv = cp.recv;
  send = cp.send;
  bias = cp.bias;
  n_recv_cons = cp.n_recv_cons;
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
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Unit::UnLesion()  {
  if(!lesioned()) return;
  StructUpdate(true);
  ClearUnitFlag(LESIONED);
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
  if(ug->owner->InheritsFrom(&TA_Layer))
    return idx; // simple: we're the only unit group
  // note: this assumes only one layer of subgroups, which is all that is supported anyway
  Layer* lay = (Layer*)ug->own_lay;
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

#ifdef DMEM_COMPILE
int Unit::dmem_this_proc = 0;
#endif

void Unit::ApplyInputData(float val, ExtType act_ext_flags, Random* ran, bool na_by_range) {
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
  if (act_ext_flags & Unit::EXT) {
    ext = val;
    SetExtFlag(Unit::EXT);
  } else {
    targ = val;
    if (act_ext_flags & Unit::TARG)
      SetExtFlag(Unit::TARG);
    else if (act_ext_flags & Unit::COMP)
      SetExtFlag(Unit::COMP);
  }
}

bool Unit::BuildUnits() {
  bool rval = false;
  if(!GetUnitSpec())
    return false;
  TypeDef* bstd = GetUnitSpec()->bias_con_type;
  if(bstd == NULL) {
    bias.Reset();
    rval = true;
    bias.SetConSpec(NULL);
  }
  else {
    bias.SetConType(bstd);
    if(bias.size == 0) {
      bias.AllocCons(1);
      bias.ConnectUnOwnCn(this);
    }
    bias.SetConSpec(GetUnitSpec()->bias_spec.SPtr()); // not generally used, but could be!
  }
  return rval;
}

void Unit::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  recv.CheckConfig(quiet, rval);
  send.CheckConfig(quiet, rval);
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
  if(!us->bias_con_type) {
    if(CheckError(bias.size > 0, quiet, rval,
                  "Unit CheckBuild: bias weight exists but no type")) {
      return false;
    }
  }
  else {
    if(CheckError((!bias.size || (bias.con_type != us->bias_con_type)),
                  quiet, rval,
                  "Unit CheckBuild: bias weight null or not same type as specified in UnitSpec:", us->name)) {
      return false;
    }
  }
  return true;
}

void Unit::RemoveCons() {
  recv.RemoveAll();             // blunt, but effective
  send.RemoveAll();
  n_recv_cons = 0;
}

void Unit::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

bool Unit::Snapshot(const String& var, SimpleMathSpec& math_op, bool arg_is_snap) {
  Variant val = 0.0f;
  if(var.startsWith("r.") || var.startsWith("s.")) {
    Unit* src_u = NULL;
    if(own_lay() && own_lay()->own_net)
      src_u = own_lay()->own_net->GetViewSrcU();
    if(!src_u) return false;
    String cvar = var.after(".");
    bool is_send = var.startsWith("s.");
    if(is_send) {
      for(int g=0;g<recv.size;g++) {
        RecvCons* tcong = recv.FastEl(g);
        MemberDef* act_md = tcong->con_type->members.FindName(cvar);
        if(!act_md) continue;
        Connection* con = tcong->FindConFrom(src_u);
        if(!con) continue;
        val = *((float*)act_md->GetOff(con));
        break;
      }
    }
    else {
      for(int g=0;g<send.size;g++) {
        SendCons* tcong = send.FastEl(g);
        MemberDef* act_md = tcong->con_type->members.FindName(cvar);
        if(!act_md)     continue;
        Connection* con = tcong->FindConFrom(src_u);
        if(!con) continue;
        val = *((float*)act_md->GetOff(con));
        break;
      }
    }
  }
  else if(var.startsWith("bias.")) {
    if(bias.size == 0) return false;
    String cvar = var.after(".");
    MemberDef* act_md = bias.con_type->members.FindName(cvar);
    if(!act_md) return false;
    Connection* con = bias.Cn(0);
    if(!con) return false;
    val = *((float*)act_md->GetOff(con));
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

void Unit::LinkPtrCons() {
  // its going to be one or the other of these two depending on who has OwnCons -- just do both
  for(int g=0; g<recv.size; g++) {
    RecvCons* recv_gp = recv.FastEl(g);
    recv_gp->LinkPtrCons(this);
  }
  for(int g=0; g<send.size; g++) {
    SendCons* send_gp = send.FastEl(g);
    send_gp->LinkPtrCons(this);
  }
}

void Unit::RecvConsPreAlloc(int no, Projection* prjn) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return;
#endif
  RecvCons* cgp = NULL;
  if((prjn->recv_idx < 0) || ((cgp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    cgp = recv.NewPrjn(prjn); // sets the type
    prjn->recv_idx = recv.size-1;
  }
  cgp->AllocCons(no);
}

void Unit::SendConsPreAlloc(int no, Projection* prjn) {
  SendCons* cgp = NULL;
  if((prjn->send_idx < 0) || ((cgp = send.SafeEl(prjn->send_idx)) == NULL)) {
    cgp = send.NewPrjn(prjn); // sets the type
    prjn->send_idx = send.size-1;
  }
  cgp->AllocCons(no);
}

void Unit::SendConsAllocInc(int no, Projection* prjn) {
  SendCons* cgp = NULL;
  if((prjn->send_idx < 0) || ((cgp = send.SafeEl(prjn->send_idx)) == NULL)) {
    cgp = send.NewPrjn(prjn); // sets the type
    prjn->send_idx = send.size-1;
  }
  cgp->ConnectAllocInc(no);
}

void Unit::SendConsPostAlloc(Projection* prjn) {
  SendCons* cgp = NULL;
  if((prjn->send_idx < 0) || ((cgp = send.SafeEl(prjn->send_idx)) == NULL)) {
    cgp = send.NewPrjn(prjn); // sets the type
    prjn->send_idx = send.size-1;
  }
  cgp->AllocConsFmSize();
}

void Unit::RecvConsAllocInc(int no, Projection* prjn) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return;
#endif
  RecvCons* cgp = NULL;
  if((prjn->recv_idx < 0) || ((cgp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    cgp = recv.NewPrjn(prjn); // sets the type
    prjn->recv_idx = recv.size-1;
  }
  cgp->ConnectAllocInc(no);
}

void Unit::RecvConsPostAlloc(Projection* prjn) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return;
#endif
  RecvCons* cgp = NULL;
  if((prjn->recv_idx < 0) || ((cgp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    cgp = recv.NewPrjn(prjn); // sets the type
    prjn->recv_idx = recv.size-1;
  }
  cgp->AllocConsFmSize();
}

Connection* Unit::ConnectFrom(Unit* su, Projection* prjn, bool alloc_send,
                              bool ignore_alloc_errs) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  RecvCons* recv_gp = NULL;
  SendCons* send_gp = NULL;
  if((prjn->recv_idx < 0) || ((recv_gp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn);
    prjn->recv_idx = recv.size-1;
  }
  if((prjn->send_idx < 0) || ((send_gp = su->send.SafeEl(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn);
    prjn->send_idx = su->send.size-1;
  }
  if(recv_gp->send_idx() < 0)
    recv_gp->other_idx = prjn->send_idx;
  if(send_gp->recv_idx() < 0)
    send_gp->other_idx = prjn->recv_idx;

  if(alloc_send) {
    send_gp->ConnectAllocInc(); // just do alloc increment
    return NULL;
  }

  Connection* con = recv_gp->ConnectUnits(this, su, send_gp, ignore_alloc_errs);
  if(con)
    n_recv_cons++;
  return con;
}

Connection* Unit::ConnectFromCk(Unit* su, Projection* prjn,
                                bool ignore_alloc_errs) {
#ifdef DMEM_COMPILE
  if(!DMem_IsLocal() && !prjn->con_spec->DMem_AlwaysLocal()) return NULL;
#endif
  RecvCons* recv_gp = NULL;
  SendCons* send_gp = NULL;
  if((prjn->recv_idx < 0) || ((recv_gp = recv.SafeEl(prjn->recv_idx)) == NULL)) {
    recv_gp = recv.NewPrjn(prjn);
    prjn->recv_idx = recv.size-1;
  }
  if((prjn->send_idx < 0) || ((send_gp = su->send.SafeEl(prjn->send_idx)) == NULL)) {
    send_gp = su->send.NewPrjn(prjn);
    prjn->send_idx = su->send.size-1;
  }
  if(recv_gp->send_idx() < 0)
    recv_gp->other_idx = prjn->send_idx;
  if(send_gp->recv_idx() < 0)
    send_gp->other_idx = prjn->recv_idx;

  if(recv_gp->FindConFromIdx(su) >= 0) // already connected!
    return NULL;

  Connection* con = recv_gp->ConnectUnits(this, su, send_gp, ignore_alloc_errs);
  if(con)
    n_recv_cons++;
  return con;
}

bool Unit::DisConnectFrom(Unit* su, Projection* prjn) {
  RecvCons* recv_gp;
  SendCons* send_gp;
  if(prjn) {
    if((prjn->recv_idx < 0) || ((recv_gp = recv.SafeEl(prjn->recv_idx)) == NULL))
      return false;
    if((prjn->send_idx < 0) || ((send_gp = su->send.SafeEl(prjn->send_idx)) == NULL))
      return false;
  }
  else {
    Layer* su_lay = GET_OWNER(su,Layer);
    recv_gp = recv.FindFrom(su_lay);
    if(!recv_gp)        return false;
    if(recv_gp->send_idx() >= 0)
      send_gp = su->send.SafeEl(recv_gp->send_idx());
    else
      send_gp = NULL;
    if(send_gp == NULL)
      send_gp = su->send.FindPrjn(recv_gp->prjn);
    if(send_gp == NULL) return false;
    prjn = recv_gp->prjn;
  }

  recv_gp->RemoveConUn(su);
  n_recv_cons--;
  return send_gp->RemoveConUn(this);
}

void Unit::DisConnectAll() {
  RecvCons* recv_gp;
  SendCons* send_gp;
  int g;
  int i;
  for(g=0; g<recv.size; g++) { // the removes cause the leaf_gp to crash..
    recv_gp = recv.FastEl(g);
    for(i=recv_gp->size-1; i>=0; i--) {
      if(recv_gp->send_idx() >= 0)
        send_gp = recv_gp->Un(i)->send.SafeEl(recv_gp->send_idx());
      else
        send_gp = NULL;
      if(send_gp == NULL)
        send_gp = recv_gp->Un(i)->send.FindPrjn(recv_gp->prjn);
      if(send_gp)
        send_gp->RemoveConUn(this);
      recv_gp->RemoveConIdx(i);
    }
    recv_gp->other_idx = -1;
  }
  for(g=0; g<send.size; g++) { // the removes cause the leaf_gp to crash..
    send_gp = send.FastEl(g);
    for(i=send_gp->size-1; i>=0; i--) {
      if(send_gp->recv_idx() >= 0)
        recv_gp = send_gp->Un(i)->recv.SafeEl(send_gp->recv_idx());
      else
        recv_gp = NULL;
      if(recv_gp == NULL)
        recv_gp = send_gp->Un(i)->recv.FindPrjn(send_gp->prjn);
      if(recv_gp)
        recv_gp->RemoveConUn(this);
      send_gp->RemoveConIdx(i);
    }
    send_gp->other_idx = -1;
  }
  n_recv_cons = 0;
}

int Unit::CountRecvCons() {
  n_recv_cons = 0;
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    n_recv_cons += cg->size;
  }
  return n_recv_cons;
}

void Unit::Copy_Weights(const Unit* src, Projection* prjn) {
  if((bias.size) && (src->bias.size)) {
    bias.OwnCn(0)->wt = src->bias.OwnCn(0)->wt;
  }
  int mx = MIN(recv.size, src->recv.size);
  for(int i=0; i<mx; i++) {
    RecvCons* cg = recv.FastEl(i);
    RecvCons* scg = src->recv.FastEl(i);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->Copy_Weights(scg);
  }
}

void Unit::SaveWeights_strm(ostream& strm, Projection* prjn, RecvCons::WtSaveFormat fmt) {
  strm << "<Un>\n";
  float bwt = 0.0;
  if(bias.size) bwt = bias.OwnCn(0)->wt;
  // always write this for a consistent format
  switch(fmt) {
  case RecvCons::TEXT:
    strm << bwt << "\n";
    break;
  case RecvCons::BINARY:
    strm.write((char*)&(bwt), sizeof(bwt));
    strm << "\n";
    break;
  }
  // not using ITR here in case of DMEM where we write separate files for
  // each process -- need to include size=0 place holders for non-local units
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || (prjn && (cg->prjn != prjn))) continue;
    strm << "<Cg " << g << " Fm:" << cg->prjn->from->name << ">\n";
    cg->SaveWeights_strm(strm, this, fmt);
    strm << "</Cg>\n";
  }
  strm << "</Un>\n";
}

int Unit::LoadWeights_strm(istream& strm, Projection* prjn, RecvCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  //   String lidx = val.before(' ');
  // todo: could compare lidx with GetMyLeafIdx()...
  float bwt = 0.0;
  switch(fmt) {
  case RecvCons::TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case RecvCons::BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  if(bias.size) {
    bias.OwnCn(0)->wt = bwt;
  }

#ifdef DMEM_COMPILE
  if(!DMem_IsLocal()) {
    // bypass non-local connections!
    while(true) {
      stat = taMisc::read_tag(strm, tag, val);
      if(stat != taMisc::TAG_GOT) break;                // *should* break at TAG_END of Un
      if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
      stat = RecvCons::SkipWeights_strm(strm, fmt, quiet); // skip over
      if(stat != taMisc::TAG_END) break; // something is wrong
      stat = taMisc::TAG_NONE;         // reset so EndTag will definitely read new tag
      RecvCons::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
      if(stat != taMisc::TAG_END) break;
    }
  }
  else {
#endif
  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;                  // *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    int gi = (int)val.before(' ');
    String fm = val.after("Fm:");
    RecvCons* cg = NULL;
    if(recv.size > gi) {
      cg = recv.FastEl(gi);
      if(cg->prjn->from->name != fm)
        cg = recv.FindFromName(fm);
    }
    else {
      cg = recv.FindFromName(fm);
    }
    if(cg) {
      stat = cg->LoadWeights_strm(strm, this, fmt, quiet);
    }
    else {
      stat = RecvCons::SkipWeights_strm(strm, fmt, quiet); // skip over
    }
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
#ifdef DMEM_COMPILE
  }
#endif
  RecvCons::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

int Unit::SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Un", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  float bwt = 0.0;
  switch(fmt) {
  case RecvCons::TEXT:
    taMisc::read_till_eol(strm);
    bwt = (float)taMisc::LexBuf;
    break;
  case RecvCons::BINARY:
    strm.read((char*)&bwt, sizeof(bwt));
    strm.get();         // get the /n
    break;
  }
  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "Cg") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    stat = RecvCons::SkipWeights_strm(strm, fmt, quiet); // skip over
    if(stat != taMisc::TAG_END) break; // something is wrong
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "Cg", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  RecvCons::LoadWeights_EndTag(strm, "Un", tag, stat, quiet);
  return stat;
}

void Unit::SaveWeights(const String& fname, Projection* prjn, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, prjn, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int Unit::LoadWeights(const String& fname, Projection* prjn, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, prjn, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Unit::GetLocalistName() {
  if(name.nonempty()) return;   // only if not otherwise named!
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned()) continue;
    if(cg->size != 1) continue; // only 1-to-1
    Unit* un = cg->Un(0);
    if(!un->name.empty()) {
      SetName(un->name);
      break;                    // done!
    }
  }
}

void Unit::TransformWeights(const SimpleMathSpec& trans, Projection* prjn) {
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->TransformWeights(trans);
  }
}

void Unit::AddNoiseToWeights(const Random& noise_spec, Projection* prjn) {
  for(int g = 0; g < recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->AddNoiseToWeights(noise_spec);
  }
}

int Unit::PruneCons(const SimpleMathSpec& pre_proc, Relation::Relations rel,
                       float cmp_val, Projection* prjn)
{
  int rval = 0;
  int g;
  for(g=0; g<recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->PruneCons(this, pre_proc, rel, cmp_val);
  }
  n_recv_cons -= rval;
  return rval;
}

int Unit::LesionCons(float p_lesion, bool permute, Projection* prjn) {
  int rval = 0;
  for(int g=0; g<recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    rval += cg->LesionCons(this, p_lesion, permute);
  }
  n_recv_cons -= rval;
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
  bool new_table = false;
  if(!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  for(int g=0; g<recv.size; g++) {
    RecvCons* cg = recv.FastEl(g);
    if(cg->prjn->from->lesioned() || ((prjn) && (cg->prjn != prjn))) continue;
    cg->ConVarsToTable(dt, this, var1, var2, var3, var4, var5, var6, var7, var8,
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
