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

#include "ConGroup.h"
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

#include "ta_vector_ops.h"

using namespace std;

float ConGroup::null_rval = 0.0f;

//int   ConGroup::vec_chunk_targ = TA_VEC_SIZE;
// NOTE: this must be a constant for everyone because otherwise the weight files
// will not be portable across different platforms, builds, etc
int   ConGroup::vec_chunk_targ = 8;

void ConGroup::Initialize(int flgs, Projection* prj, int oth_idx,
                          int own_flt_idx, int own_th_idx) {
  flags = (ConGroupFlags)flgs;
  size = 0;
  vec_chunked_size = 0;
  alloc_size = 0;
  prjn = prj;
  con_spec = prjn->GetConSpec();
  other_idx = oth_idx;
  n_con_vars = prjn->con_type->members.size;
  own_flat_idx = own_flt_idx;
  own_thr_idx = own_th_idx;
  mem_idx = 0;
  mem_start = 0;
}

// void ConGroup::CheckThisConfig_impl(bool quiet, bool& rval) {
//   inherited::CheckThisConfig_impl(quiet, rval);

//   // just to be sure!
//   if(CheckError(sizeof(int) != sizeof(float), quiet, rval,
//                 "Fatal platform build incompatibility (internal error): sizeof(int) != sizeof(float) -- this should not happen -- please report this issue!")) {
//     return;                     // fatal!
//   }

//   if(CheckError(!GetConSpec(), quiet, rval, "No con spec set")) {
//     return;                     // fatal!
//   }
//   if(CheckError(!prjn, quiet, rval, "null projection! (Connect will fix)")) {
//     return;                     // fatal!
//   }
//   if(CheckError((n_con_vars != prjn->con_type->members.size), quiet, rval,
//                 "number of connection vars does not match prjn con_type -- should be:",
//                 String(prjn->con_type->members.size))) {
//     prjn->projected = false;
//   }
// }

void ConGroup::Copy_Weights(const ConGroup* src, Network* net) {
  int mx = MIN(size, src->size);
  for(int i=0; i < mx; i++) {
    Cn(i, WT, net) = src->Cn(i, WT, net);
  }
}

float& ConGroup::SafeCn(int idx, int var_no) const {
  if(!InRange(idx)) {
    taMisc::Error("SafeCn: index out of range:", String(idx),
                  "size:", String(size));
    return null_rval;
  }
  if(!VarInRange(var_no)) {
    taMisc::Error("SafeCn: variable number out of range:", String(var_no),
                  "number of variables:", String(NConVars()));
    return null_rval;
  }
  if(OwnCons()) {
    return OwnCn(idx, var_no);
  }
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return null_rval;
  Network* net = prjn->layer->own_net;
  return UnCons(idx, net)->SafeCn(PtrCnIdx(idx), var_no);
}

float& ConGroup::SafeCnName(int idx, const String& var_nm) const {
  MemberDef* md = prjn->con_type->members.FindName(var_nm);
  if(!md) {
    taMisc::Error("SafeCnName: variable named:", String(var_nm),
                  "not found in connection of type:", prjn->con_type->name);
    return null_rval;
  }
  return SafeCn(idx, md->idx);
}

bool ConGroup::SetCnVal(float val, int idx, int var_no) {
  if(!InRange(idx)) {
    taMisc::Error("SetCnVal: index out of range:", String(idx),
                  "size:", String(size));
    return false;
  }
  if(!VarInRange(var_no)) {
    taMisc::Error("SetCnVal: variable number out of range:", String(var_no),
                  "number of variables:", String(NConVars()));
    return false;
  }
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return null_rval;
  Network* net = prjn->layer->own_net;
  Cn(idx, var_no, net) = val;
  return true;
}

bool ConGroup::SetCnValName(float val, int idx, const String& var_nm) {
  MemberDef* md = prjn->con_type->members.FindName(var_nm);
  if(!md) {
    taMisc::Error("SetCnValName: variable named:", String(var_nm),
                  "not found in connection of type:", prjn->con_type->name);
    return false;
  }
  return SetCnVal(val, idx, md->idx);
}

Unit* ConGroup::SafeUn(int idx) const {
  if(!InRange(idx)) {
    taMisc::Error("SafeUn: index out of range:", String(idx),
                  "size:", String(size));
    return NULL;
  }
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return NULL;
  Network* net = prjn->layer->own_net;
  return Un(idx, net);
}

int ConGroup::ConnectUnOwnCn(Unit* un, bool ignore_alloc_errs,
                             bool allow_null_unit) {
  static bool warned_already = false;
  if(!OwnCons()) {
    taMisc::Error("ConnectUnOwnCn: does not own cons!");
    return -1;
  }
  if(!allow_null_unit && un->flat_idx == 0) {
    return -1; // null unit -- don't even connect!
  }
  if((size >= alloc_size) || (!mem_start)) {
    if(!taMisc::err_cancel && !ignore_alloc_errs && !warned_already) {
      taMisc::Error("ConnectUnOwnCn: mem not allocated or size already at maximum allocated of",
                String(alloc_size),"this is a programmer error -- please report the bug");
    }
    warned_already = true;
    return -1;
  }
  warned_already = false;
  int rval = size;
  UnIdx(size++) = (int)un->flat_idx;
  return rval;
}

bool ConGroup::ConnectUnPtrCn(Unit* un, int con_idx, bool ignore_alloc_errs) {
  static bool warned_already = false;
  if(OwnCons()) {
    taMisc::Error("ConnectUnPtrCn: is not a ptr cons!");
    return false;
  }
  if(un->flat_idx == 0) {
    return false; // null unit -- don't even connect!
  }
  if((size >= alloc_size) || (!mem_start)) {
    if(!taMisc::err_cancel && !ignore_alloc_errs && !warned_already) {
      taMisc::Error("ConnectUnPtrCn: mem not allocated or size already at maximum allocated of",
                String(alloc_size),"this is a programmer error -- please report the bug");
    }
    warned_already = true;
    return false;
  }
  warned_already = false;
  PtrCnIdx(size) = con_idx;
  UnIdx(size++) = (int)un->flat_idx;
  return true;
}

int ConGroup::ConnectUnits(Unit* our_un, Unit* oth_un, ConGroup* oth_cons,
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

void ConGroup::ConnectAllocInc(int inc_n) {
  size += inc_n;
}

void ConGroup::AllocConsFmSize() {
  AllocCons(size);              // this sets size back to zero and does full alloc
}

void ConGroup::AllocCons(int sz) {
  if(mem_start != 0) {
    taMisc::Error("AllocCons: mem_start is not null -- re-allocating already allocated connection -- this is a programmer error in the ProjectionSpec, usually from not following make_cons flag");
  }

  mem_start = 0;
  mem_idx = 0;
  size = 0;
  vec_chunked_size = 0;
  alloc_size = sz;
  SetInactive();
}

void ConGroup::FreeCons() {
  mem_start = 0;
  mem_idx = 0;
  size = 0;
  vec_chunked_size = 0;
  alloc_size = 0;
  SetInactive();
}

bool ConGroup::CopyCons(const ConGroup& cp) {
  if(ConType() != cp.ConType() || OwnCons() != cp.OwnCons()) return false;

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

bool ConGroup::RemoveConIdx(int i, Unit* myun, Network* net) {
  if(!InRange(i)) return false;

  if(OwnCons()) {
    for(int j=i; j<size-1; j++) {
      // first, have to ensure that other side's indexes are updated for our connections
      ConGroup* othcn = UnCons(j+1, net); 
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
  UpdtIsActive();
  return true;
}

bool ConGroup::RemoveConUn(Unit* un, Unit* myun, Network* net) {
  int idx = FindConFromIdx(un);
  if(idx < 0) return false;
  return RemoveConIdx(idx, myun, net);
}

int ConGroup::FindConFromIdx(Unit* un) const {
  const int trg_idx = un->flat_idx;
  if(size > 10) {
    // starting point for search: proportional location of unit in its own layer, 
    // mapped on to size of connections
    // then search in both directions out from there
    int proploc = (int)(((float)un->idx / (float)un->own_lay()->units.leaves) *
                        (float)size);
    int upi = proploc+1;
    int dni = proploc;
    while(true) {
      bool upo = false;
      if(upi < size) {
        if(UnIdx(upi) == trg_idx) return upi;
        ++upi;
      }
      else {
        upo = true;
      }
      if(dni >= 0) {
        if(UnIdx(dni) == trg_idx) return dni;
        --dni;
      }
      else if(upo) {
        break;
      }
    }        
  }
  else {
    for(int i=0; i<size; i++) {
      if(UnIdx(i) == trg_idx) return i;
    }
  }
  return -1;
}

int ConGroup::FindConFromNameIdx(const String& unit_nm, Network* net) const {
  for(int i=0; i<size; i++) {
    Unit* u = Un(i,net);
    if(u && (u->name == unit_nm)) return i;
  }
  return -1;
}

ConGroup* ConGroup::GetPrjnSendCons(Unit* su) const {
  if(!IsRecv()) return NULL;
  ConGroup* send_gp = su->SendConGroupPrjn(prjn);
  return send_gp;
}

ConGroup* ConGroup::GetPrjnRecvCons(Unit* ru) const {
  if(!IsSend()) return NULL;
  ConGroup* recv_gp = ru->RecvConGroupPrjn(prjn);
  return recv_gp;
}

// static
ConGroup* ConGroup::FindRecipRecvCon(int& con_idx, Unit* su, Unit* ru, Layer* ru_lay) {
  Projection* prj = su->own_lay()->projections.FindPrjnFrom(ru_lay);
  if(!prj) return NULL;
  ConGroup* rcg = su->RecvConGroupPrjn(prj);
  con_idx = rcg->FindConFromIdx(ru);
  if(con_idx >= 0) return rcg;
  return NULL;
}

// static
ConGroup* ConGroup::FindRecipSendCon(int& con_idx, Unit* ru, Unit* su, Layer* su_lay) {
  Projection* prj = ru->own_lay()->send_prjns.FindPrjnTo(su_lay);
  if(!prj) return NULL;
  ConGroup* scg = ru->SendConGroupPrjn(prj);
  con_idx = scg->FindConFromIdx(su);
  if(con_idx >= 0) return scg;
  return NULL;
}  

void ConGroup::FixConPtrs_SendOwns(Network* net, int st_idx) {
  if(!IsSend() || !OwnCons()) return; // must be these two things for this to work

  Unit* su = net->UnFmIdx(own_flat_idx); 

  for(int i=st_idx; i < size; i++) {
    Unit* ru = Un(i, net);
    ConGroup* recv_gp = GetPrjnRecvCons(ru);
    if(!recv_gp) continue;      // shouldn't happen
    int ru_ci = recv_gp->FindConFromIdx(su);
    if(ru_ci >= 0) {
      recv_gp->PtrCnIdx(ru_ci) = i; // point to us..
    }
  }
}

void ConGroup::FixConPtrs_RecvOwns(Network* net, int st_idx) {
  if(!IsRecv() || !OwnCons()) return; // must be these two things for this to work

  Unit* ru = net->UnFmIdx(own_flat_idx); 

  for(int i=st_idx; i < size; i++) {
    Unit* su = Un(i, net);
    ConGroup* send_gp = GetPrjnSendCons(su);
    if(!send_gp) continue;      // shouldn't happen
    int su_ci = send_gp->FindConFromIdx(ru);
    if(su_ci >= 0) {
      send_gp->PtrCnIdx(su_ci) = i; // point to us..
    }
  }
}

void ConGroup::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  // todo: not possible anymore
  // net_mon->AddObject(this, variable);
}

void ConGroup::VecChunk_SendOwns(Network* net, 
                                 int* tmp_chunks, int* tmp_not_chunks,
                                 float* tmp_con_mem) {
  vec_chunked_size = 0;
  if(!IsSend() || !OwnCons()) {  // must be these two things for this to work
    return;
  }
  if(vec_chunk_targ <= 1 || size < vec_chunk_targ || mem_start == 0) {
    return;
  }

  int first_change = VecChunk_impl(tmp_chunks, tmp_not_chunks, tmp_con_mem);

  // fix all the other guy con pointers
  if(first_change >= 0 && first_change < size) {
    FixConPtrs_SendOwns(net, first_change);
  }
}

void ConGroup::VecChunk_RecvOwns(Network* net, 
                                 int* tmp_chunks, int* tmp_not_chunks,
                                 float* tmp_con_mem) {
  vec_chunked_size = 0;
  if(!IsRecv() || !OwnCons()) {  // must be these two things for this to work
    return;
  }
  if(vec_chunk_targ <= 1 || size < vec_chunk_targ || mem_start == 0) {
    return;
  }

  int first_change = VecChunk_impl(tmp_chunks, tmp_not_chunks, tmp_con_mem);

  // fix all the other guy con pointers
  if(first_change >= 0 && first_change < size) {
    FixConPtrs_RecvOwns(net, first_change);
  }
}


int ConGroup::VecChunk_impl(int* tmp_chunks, int* tmp_not_chunks,
                            float* tmp_con_mem) {
  vec_chunked_size = 0;

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

  if(n_chunks == 0)
    return size;                // no changes
  
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

#ifdef DEBUG
  if(cur_sz != size) {
    taMisc::Error("VecChunk_SendOwns: new size != orig size -- oops!");
    return size;
  }
#endif

  int first_change = -1;
  for(i=0; i<size; i++) {
    if(UnIdx(i) != ((int*)tmp_con_mem)[i]) {
      first_change = i;
      break;
    }
  }

  // then copy over the newly reorganized guys..
  for(int i=0; i< ncv; i++) {
    memcpy(MemBlock(i), (char*)(tmp_con_mem + alloc_size * i), size * sizeof(float));
  }

  vec_chunked_size = n_chunks * vec_chunk_targ; // we are now certfied chunkable..

  return first_change;
}


/////////////////////////////////////////////////////////////
//      Weight ops

void ConGroup::TransformWeights(const SimpleMathSpec& trans) {
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return;
  Network* net = prjn->layer->own_net;
  for(int i=0; i < size; i++) {
    float& wt = Cn(i, WT,net);
    wt = trans.Evaluate(wt);
  }
  // todo:
  // Init_Weights_post(ru, net);        // update weights after mod
}

void ConGroup::AddNoiseToWeights(const Random& noise_spec) {
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return;
  Network* net = prjn->layer->own_net;
  for(int i=0; i < size; i++)
    Cn(i, WT, net) += noise_spec.Gen();
  // todo:
  //  Init_Weights_post(ru, net);        // update weights after mod
}

int ConGroup::PruneCons(Unit* un, const SimpleMathSpec& pre_proc,
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

int ConGroup::LesionCons(Unit* un, float p_lesion, bool permute) {
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

bool ConGroup::ConValuesToArray(float_Array& ary, const String& variable) {
  MemberDef* md = ConType()->members.FindName(variable);
  if(!md || !md->type->InheritsFrom(TA_float)) {
    taMisc::Warning("ConValuesToArray: Variable:", variable,
                    "not found or not a float on units of type:",
                    ConType()->name);
    return false;
  }
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return false;
  Network* net = prjn->layer->own_net;
  for(int i=0; i<size; i++) {
    float val = Cn(i, md->idx, net);
    ary.Add(val);
  }
  return true;
}

bool ConGroup::ConValuesToMatrix(float_Matrix& mat, const String& variable) {
  MemberDef* md = ConType()->members.FindName(variable);
  if(!md || !md->type->InheritsFrom(TA_float)) {
    taMisc::Warning("ConValuesToMatrix: Variable:", variable,
                    "not found or not a float on units of type:",
                    ConType()->name);
    return false;
  }
  if(mat.size < size) {
    taMisc::Warning("ConValuesToMatrix: matrix size too small");
    return false;
  }

  if(!prjn || !prjn->layer || !prjn->layer->own_net) return false;
  Network* net = prjn->layer->own_net;
  for(int i=0; i<size; i++) {
    float val = Cn(i, md->idx, net);
    mat.FastEl_Flat(i) = val;
  }
  return true;
}

bool ConGroup::ConValuesFromArray(float_Array& ary, const String& variable) {
  MemberDef* md = ConType()->members.FindName(variable);
  if(!md || !md->type->InheritsFrom(TA_float)) {
    taMisc::Warning("ConValuesFromArray: Variable:", variable,
                    "not found or not a float on units of type:",
                    ConType()->name);
    return false;
  }
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return false;
  Network* net = prjn->layer->own_net;
  int mx = MIN(size, ary.size);
  for(int i=0; i<mx; i++) {
    float& val = Cn(i, md->idx, net);
    val = ary[i];
  }
  // Init_Weights_post(ru, net);        // update weights after mod
  return true;
}

bool ConGroup::ConValuesFromMatrix(float_Matrix& mat, const String& variable) {
  MemberDef* md = ConType()->members.FindName(variable);
  if(!md || !md->type->InheritsFrom(TA_float)) {
    taMisc::Warning("ConValuesFromMatrix: Variable:", variable,
                    "not found or not a float on units of type:",
                    ConType()->name);
    return false;
  }
  if(!prjn || !prjn->layer || !prjn->layer->own_net) return false;
  Network* net = prjn->layer->own_net;
  int mx = MIN(size, mat.size);
  for(int i=0; i<mx; i++) {
    float& val = Cn(i,md->idx, net);
    val = mat.FastEl_Flat(i);
  }
  // Init_Weights_post(ru, net);        // update weights after mod
  return true;
}

/////////////////////////////////////////////////////////////
//      Save/Load Weights

void ConGroup::SaveWeights_strm(ostream& strm, Unit* un, Network* net,
                                ConGroup::WtSaveFormat fmt) {
  if((prjn == NULL) || (!(bool)prjn->from)) {
    strm << "<Cn 0>\n";
    goto end_tag;               // don't do anything
  }
  strm << "<Cn " << size << ">\n";
  switch(fmt) {
  case ConGroup::TEXT:
    for(int i=0; i < size; i++) {
      int lidx = Un(i,net)->GetMyLeafIndex();
      if(lidx < 0) {
        taMisc::Warning("SaveWeights_strm: can't find unit");
        lidx = 0;
      }
      strm << lidx << " " << Cn(i,WT,net) << "\n";
    }
    break;
  case ConGroup::BINARY:
    for(int i=0; i < size; i++) {
      int lidx = Un(i,net)->GetMyLeafIndex();
      if(lidx < 0) {
        taMisc::Warning("SaveWeights_strm: can't find unit");
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

int ConGroup::LoadWeights_StartTag(istream& strm, const String& tag, String& val,
                                   bool quiet) {
  String in_tag;
  int stat = taMisc::read_tag(strm, in_tag, val);
  if(stat == taMisc::TAG_END) return taMisc::TAG_NONE; // some other end -- not good
  if(stat != taMisc::TAG_GOT) {
    if(!quiet) taMisc::Warning("ConGroup::LoadWeights: bad read of start tag:", tag);
    return stat;
  }
  if(in_tag != tag) {
    if(!quiet) taMisc::Warning("ConGroup::LoadWeights: read different start tag:", in_tag,
                               "expecting:", tag);
    return taMisc::TAG_NONE; // bumping up against some other tag
  }
  return stat;
}

int ConGroup::LoadWeights_EndTag(istream& strm, const String& trg_tag, String& cur_tag,
                                 int& stat, bool quiet) {
  String val;
  if(stat != taMisc::TAG_END)   // haven't already hit the end
    stat = taMisc::read_tag(strm, cur_tag, val);
  if((stat != taMisc::TAG_END) || (cur_tag != trg_tag)) {
    if(!quiet) taMisc::Warning("ConGroup::LoadWeights: bad read of end tag:", trg_tag, "got:",
                               cur_tag, "stat:", String(stat));
    if(stat == taMisc::TAG_END) stat = taMisc::TAG_NONE;
  }
  return stat;
}

int ConGroup::LoadWeights_strm(istream& strm, Unit* ru, Network* net,
                               ConGroup::WtSaveFormat fmt, bool quiet) {
  static bool warned_already = false;
  static bool sz_warned_already = false;
  if((prjn == NULL) || (!(bool)prjn->from)) {
    return SkipWeights_strm(strm, fmt, quiet); // bail
  }
  String tag, val;
  int stat = ConGroup::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    if(!quiet) taMisc::Warning("LoadWeights_strm: read size < 0");
    return taMisc::TAG_NONE;
  }
  if(sz < size) {
    if(!quiet && !sz_warned_already) {
      taMisc::Warning("LoadWeights_strm: weights file has fewer connections:", String(sz),
                      "than existing group size of:", String(size));
      sz_warned_already = true;
    // doesn't really make sense to nuke these -- maybe add a flag overall to enable this
//     for(int i=size-1; i >= sz; i--) {
//       Unit* su = Un(i);
//       ru->DisConnectFrom(su, prjn);
//     }
    }
  }
  else if(sz > size) {
    if(sz > alloc_size) {
      if(!quiet && !sz_warned_already) {
        taMisc::Warning("LoadWeights_strm: weights file has more connections:", String(sz),
                  "than allocated size:",
                  String(alloc_size), "-- only alloc_size will be loaded");
        sz_warned_already = true;
        sz = alloc_size;
      }
    }
    else {
      if(!quiet && !sz_warned_already) {
        taMisc::Warning("LoadWeights_strm: weights file has more connections:", String(sz),
                  "than existing group size of:", String(size),
                        "-- but these will fit within alloc_size and will be loaded");
        sz_warned_already = true;
      }
    }
  }
  else {
    sz_warned_already = false;
  }
  for(int i=0; i < sz; i++) {   // using load size as key factor
    int lidx;
    float wtval;
    if(fmt == ConGroup::TEXT) {
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
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: unit at leaf index: ",
                        String(lidx), "not found in layer:", prjn->from->name,
                        "removing this connection");
        warned_already = true;
      }
      if(size > i) {
        ru->DisConnectFrom(Un(i,net), prjn); // remove this guy to keep total size straight
      }
      sz--;                            // now doing less..
      i--;
      continue;
    }
    ConGroup* send_gp = su->SendConGroupPrjn(prjn);
    if(!send_gp) {
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: unit at leaf index: ",
                        String(lidx), "does not have proper send group:",
                        String(prjn->send_idx));
        warned_already = true;
      }
      if(size > i)
        ru->DisConnectFrom(Un(i,net), prjn); // remove this guy to keep total size straight
      sz--;                            // now doing less..
      i--;
      continue;
    }
    if(i >= size) {             // new connection
      // too many msgs with this:
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: attempting to load beyond size of allocated connections -- cannot do this");
        warned_already = true;
      }
      //      ru->ConnectFromCk(su, prjn, false, true, wtval); // set init wt
    }
    else if(su != Un(i,net)) {
      // not same unit -- note that at this point, the only viable strategy is to discon
      // all existing cons and start over, as otherwise everything will be hopelessly out
      // of whack
      if(!quiet && !warned_already) {
        taMisc::Warning("LoadWeights_strm: unit at index:",
                        String(i),
                        "in cons group does not match the loaded unit -- weights will be off");
        warned_already = true;
      }
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
  ConGroup::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;                  // should be tag end!
}

int ConGroup::SkipWeights_strm(istream& strm, ConGroup::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = ConGroup::LoadWeights_StartTag(strm, "Cn", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  int sz = (int)val;
  if(sz < 0) {
    return taMisc::TAG_NONE;
  }

  for(int i=0; i < sz; i++) {
    int lidx;
    float wt;
    if(fmt == ConGroup::TEXT) {
      taMisc::read_till_eol(strm);
    }
    else {                      // binary
      strm.read((char*)&(lidx), sizeof(lidx));
      strm.read((char*)&(wt), sizeof(wt));
    }
  }
  ConGroup::LoadWeights_EndTag(strm, "Cn", tag, stat, quiet);
  return stat;
}

DataTable* ConGroup::ConVarsToTable(DataTable* dt, Unit* ru, Network* net,
                              const String& var1, const String& var2,
                              const String& var3, const String& var4, const String& var5,
                              const String& var6, const String& var7, const String& var8,
                              const String& var9, const String& var10, const String& var11,
                              const String& var12, const String& var13, const String& var14) {
  if(!ru) {
    taMisc::Error("ConVarsToTable: recv unit is NULL -- bailing");
    return NULL;
  }
  if(size <= 0) return NULL;            // nothing here

  bool new_table = false;
  if (!dt) {
    taProject* proj = GET_OWNER(net, taProject);
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

  TypeDef* rutd = net->unit_vars_built;
  TypeDef* sutd = net->unit_vars_built;
  TypeDef* con_type = ConType();

  int idx;
  for(int i=0;i<nvars;i++) {
    if(vars[i].nonempty()) {
      String colnm = taMisc::StringCVar(vars[i]);
      cols[i] = dt->FindMakeColName(colnm, idx, taBase::VT_FLOAT);
      ruv[i] = suv[i] = false;
      if(vars[i].startsWith("r.")) {
        ruv[i] = true;
        String varnxt = vars[i].after("r.");
        mds[i] = rutd->members.FindName(varnxt);
        if(!mds[i]) {
          taMisc::Warning("ConVarsToTable",
                          "recv unit variable named:", varnxt,
                          "not found in type:", rutd->name);
          continue;
        }
      }
      else if(vars[i].startsWith("s.")) {
        suv[i] = true;
        String varnxt = vars[i].after("s.");
        mds[i] = sutd->members.FindName(varnxt);
        if(!mds[i]) {
          taMisc::Warning("ConVarsToTable",
                          "send unit variable named:", varnxt,
                          "not found in type:", sutd->name);
          continue;
        }
      }
      else {
        mds[i] = con_type->members.FindName(vars[i]);
        if(!mds[i]) {
          taMisc::Warning("ConVarsToTable",
                          "connection variable named:", vars[i],
                          "not found in type:", con_type->name);
          continue;
        }
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
        val = mds[i]->GetValVar((void*)ru);
      }
      else if(suv[i]) {
        val = mds[i]->GetValVar((void*)Un(j,net));
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

