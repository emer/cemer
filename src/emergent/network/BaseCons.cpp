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

#include "BaseCons.h"
#include <Network>
#include <NetMonitor>
#include <SimpleMathSpec>
#include <float_Array>
#include <int_Array>
#include <taFiler>
#include <MemberDef>
#include <taProject>
#include <DataTable>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

TA_BASEFUNS_CTORS_DEFN(BaseCons);

using namespace std;

float BaseCons::null_rval = 0.0f;
int   BaseCons::vec_chunk_targ = 4;

void BaseCons::Initialize() {
  // derived classes need to set new basic con types
  size = 0;
  vec_chunked_size = 0;
  alloc_size = 0;
  prjn = NULL;
  other_idx = -1;
  m_con_spec = NULL;
  con_type = &TA_Connection;
  mem_start = 0;
}

void BaseCons::Destroy() {
  CutLinks();
}

void BaseCons::CutLinks() {
  FreeCons();
  prjn = NULL;
  m_con_spec = NULL;
  inherited::CutLinks();
}

void BaseCons::Copy_(const BaseCons& cp) {
  // just do a full copy here
  SetConType(cp.con_type);
  m_con_spec = cp.m_con_spec;
  prjn = cp.prjn;
  other_idx = cp.other_idx;

  AllocCons(cp.alloc_size);
  CopyCons(cp);
}

void BaseCons::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);

  if(CheckError(!GetConSpec(), quiet, rval, "No con spec set")) {
    return;                     // fatal!
  }
  if(CheckError(!prjn, quiet, rval, "null projection! (Connect will fix)")) {
    return;                     // fatal!
  }
  if(IsRecv()) {
    if(CheckError((GetTypeDef() != prjn->recvcons_type), quiet, rval,
                  "type does not match recvcons_type for projection -- should be:",
                  prjn->recvcons_type->name)) {
      prjn->projected = false;
    }
  }
  else {
    if(CheckError((GetTypeDef() != prjn->sendcons_type), quiet, rval,
                  "type does not match sendcons_type for projection -- should be:",
                  prjn->recvcons_type->name)) {
      prjn->projected = false;
    }
  }
  if(CheckError((con_type != prjn->con_type), quiet, rval,
                "connection type does not match prjn con_type -- should be:",
                prjn->con_type->name)) {
    prjn->projected = false;
  }
}

void BaseCons::Copy_Weights(const BaseCons* src, Network* net) {
  int mx = MIN(size, src->size);
  for(int i=0; i < mx; i++) {
    Cn(i, WT, net) = src->Cn(i, WT, net);
  }
}

bool BaseCons::ChangeMyType(TypeDef*) {
  TestError(true, "ChangeMyType", "Cannot change type of con_groups -- change type setting in projection and reconnect network instead");
  return false;
}

float& BaseCons::SafeCn(int idx, int var_no) const {
  if(TestError(!InRange(idx),"SafeCn", "index out of range:", String(idx),
               "size:", String(size))) {
    return null_rval;
  }
  if(TestError(!VarInRange(var_no),
               "SafeCn", "variable number out of range:", String(var_no),
               "number of variables:", String(NConVars()))) {
    return null_rval;
  }
  Unit* un = GET_MY_OWNER(Unit);
  if(!un) return null_rval;
  Network* net = un->own_net();
  if(!net) return null_rval;
  if(OwnCons()) {
    return OwnCn(idx, var_no);
  }
  return UnCons(idx, net)->SafeCn(PtrCnIdx(idx), var_no);
}

float& BaseCons::SafeCnName(int idx, const String& var_nm) const {
  MemberDef* md = con_type->members.FindName(var_nm);
  if(TestError(!md, "SafeCnName", "variable named:", String(var_nm),
               "not found in connection of type:", con_type->name)) {
    return null_rval;
  }
  return SafeCn(idx, md->idx);
}

bool BaseCons::SetCnVal(float val, int idx, int var_no) {
  if(TestError(!InRange(idx),"SetCnVal", "index out of range:", String(idx),
               "size:", String(size))) {
    return false;
  }
  if(TestError(!VarInRange(var_no),
               "SetCnVal", "variable number out of range:", String(var_no),
               "number of variables:", String(NConVars()))) {
    return false;
  }
  Unit* un = GET_MY_OWNER(Unit);
  if(!un) return false;
  Network* net = un->own_net();
  if(!net) return false;
  Cn(idx, var_no, net) = val;
  return true;
}

bool BaseCons::SetCnValName(float val, int idx, const String& var_nm) {
  MemberDef* md = con_type->members.FindName(var_nm);
  if(TestError(!md, "SetCnValName", "variable named:", String(var_nm),
               "not found in connection of type:", con_type->name)) {
    return false;
  }
  return SetCnVal(val, idx, md->idx);
}

Unit* BaseCons::SafeUn(int idx) const {
  if(TestError(!InRange(idx),"SafeUn", "index out of range:", String(idx),
               "size:", String(size))) {
    return NULL;
  }
  Unit* un = GET_MY_OWNER(Unit);
  if(!un) return NULL;
  Network* net = un->own_net();
  if(!net) return NULL;
  return Un(idx, net);
}

int BaseCons::ConnectUnOwnCn(Unit* un, bool ignore_alloc_errs,
                             bool allow_null_unit) {
  static bool warned_already = false;
  if(TestError(!OwnCons(), "ConnectUnOwnCn", "does not own cons!"))
    return -1;
  if(!allow_null_unit && un->flat_idx == 0)
    return -1; // null unit -- don't even connect!
  if(size >= alloc_size) {
    if(!taMisc::err_cancel) {
      TestError(!ignore_alloc_errs && !warned_already, "ConnectUnOwnCn",
                "size already at maximum allocated of",
                String(alloc_size),"this is a programmer error -- please report the bug");
    }
    warned_already = true;
    return -1;
  }
  warned_already = false;
  int rval = size;
  UnIdx(size++) = (int32_t)un->flat_idx;
  return rval;
}

bool BaseCons::ConnectUnPtrCn(Unit* un, int con_idx, bool ignore_alloc_errs) {
  static bool warned_already = false;
  if(TestError(OwnCons(), "ConnectUnPtrCn", "is not a ptr cons!"))
    return false;
  if(un->flat_idx == 0)
    return false; // null unit -- don't even connect!
  if(size >= alloc_size) {
    if(!taMisc::err_cancel) {
      TestError(!ignore_alloc_errs && !warned_already, "ConnectUnPtrCn",
                "size already at maximum allocated of",
                String(alloc_size),"this is a programmer error -- please report the bug");
    }
    warned_already = true;
    return false;
  }
  warned_already = false;
  PtrCnIdx(size) = con_idx;
  UnIdx(size++) = (int32_t)un->flat_idx;
  return true;
}

int BaseCons::ConnectUnits(Unit* our_un, Unit* oth_un, BaseCons* oth_cons,
                           bool ignore_alloc_errs, bool set_init_wt, float init_wt) {
  Network* net = our_un->own_net();
  int con = -1;
  if(OwnCons()) {
    con = ConnectUnOwnCn(oth_un, ignore_alloc_errs);
    if(con >= 0) {
      if(!oth_cons->ConnectUnPtrCn(our_un, con, ignore_alloc_errs)) {
        con = -1;
        RemoveConIdx(size-1, our_un, net);   // remove last guy!  otherwise it is a dangler
      }
    }
    if(con >= 0 && set_init_wt) {
      SafeFastCn(con, WT, net) = init_wt;
    }
  }
  else {
    con = oth_cons->ConnectUnOwnCn(our_un, ignore_alloc_errs);
    if(con >= 0) {
      if(!ConnectUnPtrCn(oth_un, con, ignore_alloc_errs)) {
        con = -1;
        oth_cons->RemoveConIdx(size-1, oth_un, net); // remove last guy!  otherwise it is a dangler
      }
    }
    if(con >= 0 && set_init_wt) {
      SafeFastCn(size-1, WT, net) = init_wt; // our connection is last one: size-1
    }
  }
  return con;
}

void BaseCons::ConnectAllocInc(int inc_n) {
  size += inc_n;
}

void BaseCons::AllocConsFmSize() {
  AllocCons(size);              // this sets size back to zero and does full alloc
}

bool BaseCons::SetConType(TypeDef* cn_tp) {
  if(con_type == cn_tp) return true;
  if(alloc_size > 0) {
    taMisc::Warning("BaseCons SetConType error: attempt to set new type after connections were allocated -- this is not allowed -- report bug!");
    return false;
  }
  con_type = cn_tp;
  return true;
}

void BaseCons::AllocCons(int sz) {
  mem_start = 0;
  size = 0;
  vec_chunked_size = 0;
  alloc_size = sz;
}

void BaseCons::FreeCons() {
  mem_start = 0;
  size = 0;
  vec_chunked_size = 0;
  alloc_size = 0;
}

bool BaseCons::CopyCons(const BaseCons& cp) {
  if(con_type != cp.con_type || OwnCons() != cp.OwnCons()) return false;

  size = MIN(alloc_size, cp.size); // cannot go bigger than our alloc
  vec_chunked_size = 0;              // reset
  if(size == 0) return true;

  if(OwnCons()) {
    int ncv = NConVars() + 1;
    for(int i=0; i< ncv; i++) {
      memcpy(MemBlock(i), (char*)cp.MemBlock(i), size * sizeof(float));
    }
  }
  else {
    for(int i=0; i< 2; i++) {
      memcpy(MemBlock(i), (char*)cp.MemBlock(i), size * sizeof(float));
    }
  }

  return true;
}

bool BaseCons::RemoveConIdx(int i, Unit* myun, Network* net) {
  if(!InRange(i)) return false;

  if(OwnCons()) {
    for(int j=i; j<size-1; j++) {
      // first, have to ensure that other side's indexes are updated for our connections
      BaseCons* othcn = UnCons(j+1, net); 
      int myidx = othcn->FindConFromIdx(myun);
      if(myidx >= 0) {
        othcn->PtrCnIdx(myidx)--; // our index is decreased by 1
      }
      int ncv = NConVars();
      for(int k=0; k<ncv; k++) {
        OwnCn(j,k) = OwnCn(j+1,k);
      }
    }
  }
  else {
    for(int j=i; j<size-1; j++) {
      PtrCnIdx(j) = PtrCnIdx(j+1);
    }
  }

  for(int j=i; j<size-1; j++) {
    UnIdx(j) = UnIdx(j+1);
  }

  size--;
  return true;
}

bool BaseCons::RemoveConUn(Unit* un, Unit* myun, Network* net) {
  int idx = FindConFromIdx(un);
  if(idx < 0) return false;
  return RemoveConIdx(idx, myun, net);
}

int BaseCons::FindConFromIdx(Unit* un) const {
  const int trg_idx = un->flat_idx;
  for(int i=0; i<size; i++) {
    if(UnIdx(i) == trg_idx) return i;
  }
  return -1;
}

int BaseCons::FindConFromNameIdx(const String& unit_nm, Network* net) const {
  for(int i=0; i<size; i++) {
    Unit* u = Un(i,net);
    if(u && (u->name == unit_nm)) return i;
  }
  return -1;
}

SendCons* BaseCons::GetPrjnSendCons(Unit* su) const {
  if(!IsRecv()) return NULL;
  SendCons* send_gp = NULL;
  if(other_idx >= 0)
    send_gp = su->send.SafeEl(other_idx);
  if(!send_gp)
    send_gp = su->send.FindPrjn(prjn);
  return send_gp;
}

RecvCons* BaseCons::GetPrjnRecvCons(Unit* ru) const {
  if(!IsSend()) return NULL;
  RecvCons* recv_gp = NULL;
  if(other_idx >= 0)
    recv_gp = ru->recv.SafeEl(other_idx);
  if(!recv_gp)
    recv_gp = ru->recv.FindPrjn(prjn);
  return recv_gp;
}

// static
RecvCons* BaseCons::FindRecipRecvCon(int& con_idx, Unit* su, Unit* ru, Layer* ru_lay) {
  con_idx = -1;
  Network* net = ru_lay->own_net;
  for(int g=0; g<su->recv.size; g++) {
    RecvCons* cg = su->recv.FastEl(g);
    if(!cg->prjn || (cg->prjn->from.ptr() != ru_lay)) continue;
    con_idx = cg->FindConFromIdx(ru);
    if(con_idx >= 0) return cg;
  }
  return NULL;
}

// static
SendCons* BaseCons::FindRecipSendCon(int& con_idx, Unit* ru, Unit* su, Layer* su_lay) {
  con_idx = -1;
  Network* net = su_lay->own_net;
  for(int g=0; g<ru->send.size; g++) {
    SendCons* cg = ru->send.FastEl(g);
    if(!cg->prjn || (cg->prjn->layer != su_lay)) continue;
    con_idx = cg->FindConFromIdx(su);
    if(con_idx >= 0) return cg;
  }
  return NULL;
}

void BaseCons::FixConPtrs_SendOwns(Unit* su, Network* net) {
  if(!IsSend() || !OwnCons()) return; // must be these two things for this to work

  for(int i=0; i < size; i++) {
    Unit* ru = Un(i, net);
    RecvCons* recv_gp = GetPrjnRecvCons(ru);
    if(!recv_gp) continue;      // shouldn't happen
    int ru_ci = recv_gp->FindConFromIdx(su);
    if(ru_ci >= 0) {
      recv_gp->PtrCnIdx(ru_ci) = i; // point to us..
    }
  }
}

void BaseCons::FixConPtrs_RecvOwns(Unit* ru, Network* net) {
  if(!IsRecv() || !OwnCons()) return; // must be these two things for this to work

  for(int i=0; i < size; i++) {
    Unit* su = Un(i, net);
    SendCons* send_gp = GetPrjnSendCons(su);
    if(!send_gp) continue;      // shouldn't happen
    int su_ci = send_gp->FindConFromIdx(ru);
    if(su_ci >= 0) {
      send_gp->PtrCnIdx(su_ci) = i; // point to us..
    }
  }
}

void BaseCons::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}


float BaseCons::VecChunk_SendOwns(Unit* su, Network* net, 
                                  int* tmp_chunks, int* tmp_not_chunks,
                                  float* tmp_con_mem) {
  if(!IsSend() || !OwnCons()) return 0.0f; // must be these two things for this to work

  if(size < vec_chunk_targ || mem_start == 0) {
    vec_chunked_size = 0;
    return 0.0f;
  }

  // first find all the chunks and not-chunks
  int n_chunks = 0;
  int n_not_chunks = 0;
  int prv_uni = -1;
  int cur_seq_st = -1;
  int i;
  for(i=0; i < size; i++) {
    int uni = UnIdx(i);
    if(prv_uni < 0) {
      prv_uni = uni;
      continue;                 // new start
    }
    if(uni == prv_uni+1) {      // sequential
      prv_uni = uni;
      if(cur_seq_st < 0) {
        cur_seq_st = i-1;       // new chunk start..
      }
      else {
        if(i-cur_seq_st == (vec_chunk_targ-1)) { // got a chunk's worth
          tmp_chunks[n_chunks] = cur_seq_st; // record
          n_chunks++;
          prv_uni = -1;
          cur_seq_st = -1;      // start again
        }
        // otherwise keep going..
      }
    }
    else {
      prv_uni = uni;
      if(cur_seq_st < 0) {
        // last guy is definitely a non-chunk
        tmp_not_chunks[n_not_chunks++] = i-1;
      }
      else {
        // everybody from cur_seq_st to before me is a non-matcher
        for(int j=cur_seq_st; j < i; j++) {
          tmp_not_chunks[n_not_chunks++] = j;
        }
        cur_seq_st = -1;          // reset
      }
    }
  }
  if(prv_uni >= 0) {            // was working on something
    if(cur_seq_st < 0) {
      // last guy is definitely a non-chunk
      tmp_not_chunks[n_not_chunks++] = i-1;
    }
    else {
      // everybody from cur_seq_st to before me is a non-matcher
      for(int j=cur_seq_st; j < i; j++) {
        tmp_not_chunks[n_not_chunks++] = j;
      }
    }
  }

  if(n_chunks == 0) return 0.0f;
  
  int ncv = NConVars()+1;       // include unit idx

  // now construct new reorganized data
  int cur_sz = 0;
  for(i=0; i<n_chunks; i++) {
    int seq_st = tmp_chunks[i];
    for(int j=seq_st; j< seq_st+vec_chunk_targ; j++) {
      for(int v=0; v<ncv; v++) {
        tmp_con_mem[alloc_size * v + cur_sz] = mem_start[alloc_size * v + j];
      }
      cur_sz++;
    }
  }
  for(i=0; i<n_not_chunks; i++) {
    int j = tmp_not_chunks[i];
    for(int v=0; v<ncv; v++) {
      tmp_con_mem[alloc_size * v + cur_sz] = mem_start[alloc_size * v + j];
    }
    cur_sz++;
  }

  if(TestError(cur_sz != size, "VecChunk_SendOwns", "new size != orig size -- oops!")) {
    // this is just for debugging
    return 0.0f;
  }

  // then copy over the newly reorganized guys..
  for(int i=0; i< ncv; i++) {
    memcpy(MemBlock(i), (char*)(tmp_con_mem + alloc_size * i), size * sizeof(float));
  }

  // finally, fix all the other guy con pointers
  FixConPtrs_SendOwns(su, net);

  vec_chunked_size = n_chunks * vec_chunk_targ; // we are now certfied chunkable..

  // rval is percent of our cons that are vec chunked
  return (float)vec_chunked_size / (float)size;
}


/////////////////////////////////////////////////////////////
//      Weight ops

void BaseCons::TransformWeights(const SimpleMathSpec& trans) {
  Unit* ru = GET_MY_OWNER(Unit);
  Network* net = ru->own_net();
  for(int i=0; i < size; i++) {
    float& wt = Cn(i, WT,net);
    wt = trans.Evaluate(wt);
  }
  Init_Weights_post(ru, net);        // update weights after mod
}

void BaseCons::AddNoiseToWeights(const Random& noise_spec) {
  Unit* ru = GET_MY_OWNER(Unit);
  Network* net = ru->own_net();
  for(int i=0; i < size; i++)
    Cn(i, WT, net) += noise_spec.Gen();
  Init_Weights_post(ru, net);        // update weights after mod
}

int BaseCons::PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
                            Relation::Relations rel, float cmp_val)
{
  Network* net = un->own_net();
  Relation cond;
  cond.rel = rel; cond.val = cmp_val;
  int rval = 0;
  for(int j=size-1; j>=0; j--) {
    if(cond.Evaluate(pre_proc.Evaluate(Cn(j, WT, net)))) {
      un->DisConnectFrom(Un(j, net), prjn);
      rval++;
    }
  }
  return rval;
}

int BaseCons::LesionCons(Unit* un, float p_lesion, bool permute) {
  Network* net = un->own_net();
  int rval = 0;
  if(permute) {
    rval = (int) (p_lesion * (float)size);
    if(rval == 0) return 0;
    int_Array ary;
    int j;
    for(j=0; j<size; j++)
      ary.Add(j);
    ary.Permute();
    ary.size = rval;
    ary.Sort();
    for(j=ary.size-1; j>=0; j--) {
      un->DisConnectFrom(Un(ary.FastEl(j), net), prjn);
    }
  }
  else {
    int j;
    for(j=size-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
        un->DisConnectFrom(Un(j, net), prjn);
        rval++;
      }
    }
  }
  return rval;
}

/////////////////////////////////////////////////////////////
//      To/From Arrays/Matrix

bool BaseCons::ConValuesToArray(float_Array& ary, const String& variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesToArray",
                 "Variable:", variable, "not found or not a float on units of type:",
                 con_type->name)) {
    return false;
  }
  Unit* ru = GET_MY_OWNER(Unit);
  Network* net = ru->own_net();
  for(int i=0; i<size; i++) {
    float val = Cn(i, md->idx, net);
    ary.Add(val);
  }
  return true;
}

bool BaseCons::ConValuesToMatrix(float_Matrix& mat, const String& variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesToMatrix",
                 "Variable:", variable, "not found or not a float on units of type:",
                 con_type->name)) {
    return false;
  }
  if(TestWarning(mat.size < size, "ConValuesToMatrix", "matrix size too small")) {
    return false;
  }

  Unit* ru = GET_MY_OWNER(Unit);
  Network* net = ru->own_net();
  for(int i=0; i<size; i++) {
    float val = Cn(i, md->idx, net);
    mat.FastEl_Flat(i) = val;
  }
  return true;
}

bool BaseCons::ConValuesFromArray(float_Array& ary, const String& variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesFromArray",
                 "Variable:", variable, "not found or not a float on units of type:",
                 con_type->name)) {
    return false;
  }
  Unit* ru = GET_MY_OWNER(Unit);
  Network* net = ru->own_net();
  int mx = MIN(size, ary.size);
  for(int i=0; i<mx; i++) {
    float& val = Cn(i, md->idx, net);
    val = ary[i];
  }
  Init_Weights_post(ru, net);        // update weights after mod
  return true;
}

bool BaseCons::ConValuesFromMatrix(float_Matrix& mat, const String& variable) {
  MemberDef* md = con_type->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "ConValuesFromMatrix",
                 "Variable:", variable, "not found or not a float on units of type:",
                 con_type->name)) {
    return false;
  }
  Unit* ru = GET_MY_OWNER(Unit);
  Network* net = ru->own_net();
  int mx = MIN(size, mat.size);
  for(int i=0; i<mx; i++) {
    float& val = Cn(i,md->idx, net);
    val = mat.FastEl_Flat(i);
  }
  Init_Weights_post(ru, net);        // update weights after mod
  return true;
}

/////////////////////////////////////////////////////////////
//      Save/Load Weights

void BaseCons::SaveWeights_strm(ostream& strm, Unit* un, Network* net,
                                BaseCons::WtSaveFormat fmt) {
  if((prjn == NULL) || (!(bool)prjn->from)) {
    strm << "<Cn 0>\n";
    goto end_tag;               // don't do anything
  }
  strm << "<Cn " << size << ">\n";
  switch(fmt) {
  case BaseCons::TEXT:
    for(int i=0; i < size; i++) {
      int lidx = Un(i,net)->GetMyLeafIndex();
      if(TestWarning(lidx < 0, "SaveWeights_strm", "can't find unit")) {
        lidx = 0;
      }
      strm << lidx << " " << Cn(i,WT,net) << "\n";
    }
    break;
  case BaseCons::BINARY:
    for(int i=0; i < size; i++) {
      int lidx = Un(i,net)->GetMyLeafIndex();
      if(TestWarning(lidx < 0, "SaveWeights_strm", "can't find unit")) {
        lidx = 0;
      }
      strm.write((char*)&(lidx), sizeof(lidx));
      strm.write((char*)&(Cn(i,WT,net)), sizeof(float));
    }
    strm << "\n";
    break;
  }
 end_tag:
  strm << "</Cn>\n";
}

// return values:
// TAG_END = successfully got to end of thing;
// TAG_NONE = some kind of error
// TAG_EOF = EOF

int BaseCons::LoadWeights_StartTag(istream& strm, const String& tag, String& val,
                                   bool quiet) {
  String in_tag;
  int stat = taMisc::read_tag(strm, in_tag, val);
  if(stat == taMisc::TAG_END) return taMisc::TAG_NONE; // some other end -- not good
  if(stat != taMisc::TAG_GOT) {
    if(!quiet) taMisc::Warning("BaseCons::LoadWeights: bad read of start tag:", tag);
    return stat;
  }
  if(in_tag != tag) {
    if(!quiet) taMisc::Warning("BaseCons::LoadWeights: read different start tag:", in_tag,
                               "expecting:", tag);
    return taMisc::TAG_NONE; // bumping up against some other tag
  }
  return stat;
}

int BaseCons::LoadWeights_EndTag(istream& strm, const String& trg_tag, String& cur_tag,
                                 int& stat, bool quiet) {
  String val;
  if(stat != taMisc::TAG_END)   // haven't already hit the end
    stat = taMisc::read_tag(strm, cur_tag, val);
  if((stat != taMisc::TAG_END) || (cur_tag != trg_tag)) {
    if(!quiet) taMisc::Warning("BaseCons::LoadWeights: bad read of end tag:", trg_tag, "got:",
                               cur_tag, "stat:", String(stat));
    if(stat == taMisc::TAG_END) stat = taMisc::TAG_NONE;
  }
  return stat;
}

int BaseCons::LoadWeights_strm(istream& strm, Unit* ru, Network* net,
                               BaseCons::WtSaveFormat fmt, bool quiet) {
  static bool warned_already = false;
  static bool sz_warned_already = false;
  if((prjn == NULL) || (!(bool)prjn->from)) {
    return SkipWeights_strm(strm, fmt, quiet); // bail
  }
  String tag, val;
  int stat = BaseCons::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    TestWarning(!quiet, "LoadWeights_strm", "read size < 0");
    return taMisc::TAG_NONE;
  }
  if(sz < size) {
    TestWarning(!quiet && !sz_warned_already, "LoadWeights_strm", "weights file has fewer connections:", String(sz),
                "than existing group size of:", String(size));
    sz_warned_already = true;
    // doesn't really make sense to nuke these -- maybe add a flag overall to enable this
//     for(int i=size-1; i >= sz; i--) {
//       Unit* su = Un(i);
//       ru->DisConnectFrom(su, prjn);
//     }
  }
  else if(sz > size) {
    if(sz > alloc_size) {
      TestWarning(!quiet && !sz_warned_already, "LoadWeights_strm", "weights file has more connections:", String(sz),
                  "than allocated size:",
                  String(alloc_size), "-- only alloc_size will be loaded");
      sz_warned_already = true;
      sz = alloc_size;
    }
    else {
      TestWarning(!quiet && !sz_warned_already, "LoadWeights_strm", "weights file has more connections:", String(sz),
                  "than existing group size of:",
                  String(size), "-- but these will fit within alloc_size and will be loaded");
      sz_warned_already = true;
    }
  }
  else {
    sz_warned_already = false;
  }
  for(int i=0; i < sz; i++) {   // using load size as key factor
    int lidx;
    float wtval;
    if(fmt == BaseCons::TEXT) {
      taMisc::read_till_eol(strm);
      lidx = (int)taMisc::LexBuf.before(' ');
      wtval = (float)taMisc::LexBuf.after(' ');
    }
    else {                      // binary
      strm.read((char*)&(lidx), sizeof(lidx));
      strm.read((char*)&(wtval), sizeof(wtval));
    }
    Unit* su = prjn->from->units.Leaf(lidx);
    if(!su) {
      TestWarning(!quiet && !warned_already, "LoadWeights_strm", "unit at leaf index: ",
                  String(lidx), "not found in layer:", prjn->from->name,
                  "removing this connection");
      warned_already = true;
      if(size > i) {
        ru->DisConnectFrom(Un(i,net), prjn); // remove this guy to keep total size straight
      }
      sz--;                            // now doing less..
      i--;
      continue;
    }
    SendCons* send_gp = su->send.SafeEl(prjn->send_idx);
    if(!send_gp) {
      TestWarning(!quiet && !warned_already, "LoadWeights_strm", "unit at leaf index: ",
                  String(lidx), "does not have proper send group:",
                  String(prjn->send_idx));
      warned_already = true;
      if(size > i)
        ru->DisConnectFrom(Un(i,net), prjn); // remove this guy to keep total size straight
      sz--;                            // now doing less..
      i--;
      continue;
    }
    if(i >= size) {             // new connection
      // too many msgs with this:
      TestWarning(!quiet && !warned_already, "LoadWeights_strm", "attempting to load beyond size of allocated connections -- cannot do this");
      warned_already = true;
      //      ru->ConnectFromCk(su, prjn, false, true, wtval); // set init wt
    }
    else if(su != Un(i,net)) {
      // not same unit -- note that at this point, the only viable strategy is to discon
      // all existing cons and start over, as otherwise everything will be hopelessly out
      // of whack
      TestWarning(!quiet && !warned_already, "LoadWeights_strm", "unit at index:",
                  String(i),
                  "in cons group does not match the loaded unit -- weights will be off");
      warned_already = true;
      Cn(i,WT,net) = wtval;

      // this is not viable:
      // for(int j=size-1; j >= i; j--) {
      //   Unit* su = Un(j,net);
      //   ru->DisConnectFrom(su, prjn);
      // }
      // ru->ConnectFromCk(su, prjn, false, true, wtval); // set init wt
    }
    else {                      // all good normal case, just set the weights!
      warned_already = false;
      Cn(i,WT,net) = wtval;
    }
  }
  BaseCons::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);

  Init_Weights_post(ru,net);        // update weights after loading
  return stat;                  // should be tag end!
}

int BaseCons::SkipWeights_strm(istream& strm, BaseCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = BaseCons::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    return taMisc::TAG_NONE;
  }

  for(int i=0; i < sz; i++) {
    int lidx;
    float wt;
    if(fmt == BaseCons::TEXT) {
      taMisc::read_till_eol(strm);
    }
    else {                      // binary
      strm.read((char*)&(lidx), sizeof(lidx));
      strm.read((char*)&(wt), sizeof(wt));
    }
  }
  BaseCons::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;
}

void BaseCons::SaveWeights(const String& fname, Unit* ru, Network* net,
                           BaseCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, ru, net, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int BaseCons::LoadWeights(const String& fname, Unit* ru, Network* net,
                          BaseCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = 0;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, ru, net, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

int BaseCons::Dump_Save_PathR(ostream& strm, taBase* par, int indent) {
  // first save any sub-members (there usually aren't any)
//   int rval = GetTypeDef()->Dump_Save_PathR(strm, (void*)this, (void*)par, indent);

  if(this == par) {                // hack signal to just save as a sub-guy
    strm << "\n";
    taMisc::indent(strm, indent+1, 1) << "[" << size << "]"; // just add the size here
  }
  return true;
}

int BaseCons::Dump_Load_Value(istream& strm, taBase* par) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)  return EOF;
  if(c == ';')  return 2;       // signal that its a path
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if (c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  // now, load members (if we have dims, will exit at that point)
  int rval = GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
  // 3 is a hacky code to tell us that it got the [
  if ((rval != 3) || (rval == EOF)) return rval;

  c = taMisc::read_word(strm);
  if(c == '[') {
    c = taMisc::read_word(strm);
    if(c == ']') {
      int sz = atoi(taMisc::LexBuf);
      FreeCons();               // need to explicitly nuke old guys!
      AllocCons(sz);
    }
  }
  c = taMisc::read_till_rbracket(strm);
  if (c==EOF)   return EOF;
  c = taMisc::read_till_semi(strm);
  if (c==EOF)   return EOF;
  return 2;                     // path signal -- if we got this!
}

/////////////////////////////////////////////////////////////
//      Dump Load/Save

int BaseCons::Dump_Save_Cons(ostream& strm, int indent) {
  // output the units
  taMisc::indent(strm, indent, 1) << "{ con_alloc = " << alloc_size << ";\n";
  taMisc::indent(strm, indent+1, 1) << "units = {";
  Unit* own_ru = GET_MY_OWNER(Unit);
  Network* net = own_ru->own_net();
  for(int i=0; i<size; i++) {
    Unit* u = Un(i,net);
    if(u)
      strm << u->GetMyLeafIndex() << "; ";
    else
      strm << -1 << "; ";       // null..
  }
  strm << "};\n";

  // output the connection values
  int ncv = NConVars();
  for(int j=0; j<ncv; j++) {
    MemberDef* md = con_type->members.FastEl(j);
    if((md->type->IsAnyPtr()) || (md->HasOption("NO_SAVE")))
      continue;
    taMisc::indent(strm, indent+1,1) << md->name << " = {";
    for(int i=0; i<size; i++) {
       strm << Cn(i,md->idx,net) << "; ";
    }
    strm << "};\n";
  }
  return true;
}

int BaseCons::Dump_Load_Cons(istream& strm, bool old_2nd_load) {
  Unit* own_ru = GET_MY_OWNER(Unit);
  if(TestWarning(!own_ru, "Dump_Load_Cons","NULL own_ru -- should not happen")) {
    return false;
  }

  Network* net = own_ru->own_net();

  int c = taMisc::read_till_lbracket(strm);     // get past opening bracket
  if(c == EOF) return EOF;
  c = taMisc::read_word(strm);
  if(TestWarning(taMisc::LexBuf != "con_alloc", "Dump_Load_Cons",
                 "Expecting: 'con_alloc' in load file, got:",
                 taMisc::LexBuf,"instead")) {
    return false;
  }
  // skip =
  c = taMisc::skip_white(strm);
  if(TestWarning(c != '=', "Dump_Load_Cons",
                 "Missing '=' in dump file for con_alloc in RecvCons")) {
    return false;
  }
  c = taMisc::read_till_semi(strm);
  int con_alloc = (int)taMisc::LexBuf;

  bool old_load = false;
  bool bias_con = false;
  if(!prjn && con_alloc == 1) {  // if prjn = NULL, then probably bias con -- just allocate cons
    bias_con = true;
    if(alloc_size != 1)
      AllocCons(con_alloc);
    if(size != 1)
      ConnectUnOwnCn(own_ru);
  }
  else {
    if(alloc_size != con_alloc) {
      // if not allocated yet, we have an old-style dump file that we need to postpone loading
      // otherwise, we might be doing a load-over in which case it is fine, hopefully..
      old_load = true;
    }
  }
  String load_str;              // this will be the load string
  if(old_load) {
    load_str += "{ con_alloc = " + String(con_alloc) + ";\n";
  }
  c = taMisc::read_word(strm);
  if(TestWarning(taMisc::LexBuf != "units",
                 "Dump_Load_Cons", "Expecting 'units' in load file, got:",
                 taMisc::LexBuf,"instead")) {
    return false;
  }
  // skip =
  c = taMisc::skip_white(strm);
  if(TestWarning(c != '=', "Dump_Load_Cons", "Missing '=' in dump file for unit")) {
    return false;
  }
  // skip {
  c = taMisc::skip_white(strm);
  if(TestWarning(c != '{', "Dump_Load_Cons", "Missing '{' in dump file for unit")) {
    return false;
  }

  if(old_load) {
    load_str += "units = {";
  }

  // first read in the units
  Unit_Group* ug = NULL;
  if(prjn && prjn->from.ptr())
    ug = &(prjn->from->units);
  int c_count = 0;              // number of connections
  while(true) {
    c = taMisc::read_till_rb_or_semi(strm);
    if(c == EOF) return EOF;
    if(c == '}') {
      if(old_load)      load_str += "};\n";
      break;
    }
    if(old_load) {
      load_str += taMisc::LexBuf + "; ";
      continue;                 // just load and save
    }
    Unit* un = NULL;
    int lfidx = (int)taMisc::LexBuf;
    if(ug && (lfidx >= 0)) {
      un = (Unit*)ug->Leaf(lfidx);
      if(TestWarning(!un, "Dump_Load_Cons", "Connection unit not found")) {
        continue;
      }
    }
    if(!old_2nd_load && un && size <= c_count) {
      own_ru->ConnectFrom(un, prjn);
    }
    c_count++;
  }

  if(!old_load && c_count > alloc_size) {
    TestWarning(true, "Dump_Load_Cons", "More connections read:", String(c_count),
                "than allocated:", String(alloc_size),
                "-- weights will be incomplete");
  }

  // now read in the values
  while(true) {
    c = taMisc::read_word(strm);
    if(c == EOF) return EOF;
    if(c == '}') {
      if(strm.peek() == ';') strm.get(); // get the semi
      break;            // done
    }
    MemberDef* md = con_type->members.FindName(taMisc::LexBuf);
    if(TestWarning(!md, "Dump_Load_Cons",
                   "Connection member not found:", taMisc::LexBuf)) {
      c = taMisc::skip_past_err(strm);
      if(c == '}') break;
      continue;
    }
    // skip =
    c = taMisc::skip_white(strm);
    if(TestWarning(c != '=', "Dump_Load_Cons",
                   "Missing '=' in dump file for unit")) {
      c = taMisc::skip_past_err(strm);
      continue;
    }
    // skip {
    c = taMisc::skip_white(strm);
    if(TestWarning(c != '{', "Dump_Load_Cons",
                   "Missing '{' in dump file for unit")) {
      c = taMisc::skip_past_err(strm);
      continue;
    }

    if(old_load) load_str += md->name + " = {";

    int i = 0;
    while(true) {
      c = taMisc::read_till_rb_or_semi(strm);
      if(c == EOF) return EOF;
      if(c == '}') {
        if(old_load)    load_str += "};\n";
        break;
      }
      if(old_load) {            // just save it up..
        load_str += taMisc::LexBuf + "; ";
        continue;
      }
      if(i >= size) {
        c = taMisc::skip_past_err_rb(strm); // bail to ending rb
        if(old_load)    load_str += "};\n";
        break;
      }
      float& val = Cn(i,md->idx,net);
      val = (float)taMisc::LexBuf;
      i++;
    }
  }

  if(prjn && prjn->con_spec.spec) {
    SetConSpec(prjn->con_spec.spec); // must set conspec b/c not otherwise saved or set
    if(!old_load)
      Init_Weights_post(own_ru, net);        // update weights after loading
  }

  if(old_load) {
    if(net) {
      int my_idx = own_ru->recv.FindEl(this);
      own_ru->SetUserData("OldLoadCons_" + String(my_idx), load_str);
      // save in user data for loading later -- important: can't save in this because we
      // have to do a Connect later and that nukes us! :(  So, we use the unit instead
      net->old_load_cons = true; // tell network to load later
    }
  }
  return true;
}

DataTable* BaseCons::ConVarsToTable(DataTable* dt, Unit* ru, Network* net,
                              const String& var1, const String& var2,
                              const String& var3, const String& var4, const String& var5,
                              const String& var6, const String& var7, const String& var8,
                              const String& var9, const String& var10, const String& var11,
                              const String& var12, const String& var13, const String& var14) {
  if(TestError(!ru, "ConVarsToTable", "recv unit is NULL -- bailing"))
    return NULL;
  if(size <= 0) return NULL;            // nothing here

  bool new_table = false;
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  const int nvars = 14;
  DataCol* cols[nvars];
  const String vars[nvars] = {var1, var2, var3, var4, var5, var6, var7, var8,
                              var9, var10, var11, var12, var13, var14};
  MemberDef* mds[nvars];
  bool ruv[nvars];              // recv unit var
  bool suv[nvars];              // send unit var
  bool biasv[nvars];            // bias var

  TypeDef* rutd = ru->GetTypeDef();
  Unit* su0 = Un(0, net);
  TypeDef* sutd = su0->GetTypeDef();

  TypeDef* rubiastd = NULL;
  TypeDef* subiastd = NULL;
  if(ru->bias.size) {
    rubiastd = ru->bias.con_type;
  }
  if(su0->bias.size) {
    subiastd = su0->bias.con_type;
  }

  int idx;
  for(int i=0;i<nvars;i++) {
    if(vars[i].nonempty()) {
      String colnm = taMisc::StringCVar(vars[i]);
      cols[i] = dt->FindMakeColName(colnm, idx, VT_FLOAT);
      ruv[i] = suv[i] = biasv[i] = false;
      if(vars[i].startsWith("r.")) {
        ruv[i] = true;
        String varnxt = vars[i].after("r.");
        if(varnxt.startsWith("bias.")) {
          if(TestWarning(!rubiastd, "ConVarstoTable", "recv bias type or con not set"))
            continue;
          biasv[i] = true;
          varnxt = varnxt.after("bias.");
          mds[i] = rubiastd->members.FindName(varnxt);
          if(TestWarning(!mds[i], "ConVarsToTable", "recv unit bias variable named:", varnxt,
                         "not found in type:", rubiastd->name))
            continue;
        }
        else {
          mds[i] = rutd->members.FindName(varnxt);
          if(TestWarning(!mds[i], "ConVarsToTable", "recv unit variable named:", varnxt,
                         "not found in type:", rutd->name))
            continue;
        }
      }
      else if(vars[i].startsWith("s.")) {
        suv[i] = true;
        String varnxt = vars[i].after("s.");
        if(varnxt.startsWith("bias.")) {
          if(TestWarning(!subiastd, "ConVarstoTable", "recv bias type or con not set"))
            continue;
          biasv[i] = true;
          varnxt = varnxt.after("bias.");
          mds[i] = subiastd->members.FindName(varnxt);
          if(TestWarning(!mds[i], "ConVarsToTable", "send unit bias variable named:", varnxt,
                         "not found in type:", subiastd->name))
            continue;
        }
        else {
          mds[i] = sutd->members.FindName(varnxt);
          if(TestWarning(!mds[i], "ConVarsToTable", "send unit variable named:", varnxt,
                         "not found in type:", sutd->name))
            continue;
        }
      }
      else {
        mds[i] = con_type->members.FindName(vars[i]);
        if(TestWarning(!mds[i], "ConVarsToTable", "connection variable named:", vars[i],
                       "not found in type:", con_type->name))
          continue;
      }
    }
    else {
      cols[i] = NULL;
      mds[i] = NULL;
    }
  }
  for(int j=0;j<size;j++) {
    dt->AddBlankRow();
    for(int i=0;i<nvars;i++) {
      if(!mds[i]) continue;
      Variant val;
      if(ruv[i]) {
        if(biasv[i]) {
          val = ru->bias.OwnCn(0, mds[i]->idx);
        }
        else {
          val = mds[i]->GetValVar((void*)ru);
        }
      }
      else if(suv[i]) {
        if(biasv[i]) {
          val = Un(j,net)->bias.OwnCn(0, mds[i]->idx);
        }
        else {
          val = mds[i]->GetValVar((void*)Un(j,net));
        }
      }
      else {
        val = Cn(j, mds[i]->idx, net);
      }
      cols[i]->SetVal(val, -1);
    }
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

