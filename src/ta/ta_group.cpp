// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.


// ta_group.cc

#include <sstream>

#include "ta_group.h"
#include "ta_TA_type.h"

#ifdef TA_GUI
  #include "ta_qtgroup.h"
  #include "ta_qtbrowse.h"
  #include "ta_qtclipdata.h"
#endif



////////////////////////////
//      taSubGroup  	  //
////////////////////////////

// not only sub-groups cause dirtiness, because if you add an el to a
// previously null group, it needs added, etc..

void taSubGroup::Dirty() {
  if(owner != NULL) ((TAGPtr)owner)->Dirty();
}

void taSubGroup::DataChanged(int dcr, void* op1, void* op2) {
  if (owner == NULL) return;
  // send LIST events to the owning group as a GROUP_ITEM event
  if ((dcr >= DCR_LIST_ITEM_MIN) && (dcr <= DCR_LIST_ITEM_MAX))
    ((TAGPtr)owner)->DataChanged(dcr + DCR_List_Group_Offset, op1, op2);
}

bool taSubGroup::Transfer(taBase* it) {
  // need to leaf count on parent group
  TAGPtr myown = (TAGPtr)owner;
  taGroup_impl* git = (taGroup_impl*)it;
  if((git->super_gp == myown) || (git->super_gp == NULL))
    return false;
  taGroup_impl* old_own = git->super_gp;
  bool rval = TALOG::Transfer(git);
  if(rval) {
    old_own->UpdateLeafCount_(-git->leaves);
    old_own->Dirty();
    if(myown != NULL) {
      myown->UpdateLeafCount_(git->leaves);
      myown->Dirty();
    }
  }
  return rval;
}


////////////////////////////
//      taGroup_impl  	  //
////////////////////////////


void taGroup_impl::Initialize() {
  leaves = 0;
  super_gp = NULL;
  leaf_gp = NULL;
  root_gp = NULL;
}

void taGroup_impl::Destroy() {
  if (leaf_gp != NULL) {
    taBase::unRefDone(leaf_gp);
    leaf_gp = NULL;
  }
  RemoveAll();
  CutLinks();
}

void taGroup_impl::InitLinks() {
  inherited::InitLinks();
  gp.SetBaseType(GetTypeDef());	// more of the same type of group
  taBase::Own(gp, this);

  super_gp = GetSuperGp_();
  if (super_gp) {
    root_gp = super_gp->root_gp;
    SetBaseType(super_gp->el_base);
    el_typ = super_gp->el_typ;
  } else {
    root_gp = this;
  }
}

void taGroup_impl::CutLinks() {
  root_gp = this; //prob not needed, but maintains the strict contract of root_gp not null
  inherited::CutLinks();
}


void taGroup_impl::DataChanged(int dcr, void* op1, void* op2) {
  taList_impl::DataChanged(dcr, op1, op2); // normal processing
  // send LIST events to the root group as a GROUP_ITEM event
/*HUH???  if ((dcr >= DCR_LIST_ITEM_MIN) && (dcr <= DCR_LIST_ITEM_MAX)) {
    root_gp->DataChanged(dcr + (DCR_GROUP_ITEM_MIN - DCR_LIST_ITEM_MIN) , op1, op2);
  } */
}

void taGroup_impl::Dirty() {
  if(leaf_gp != NULL) {
    taBase::unRefDone(leaf_gp);
    leaf_gp = NULL;
  }
  if(super_gp != NULL)
    super_gp->Dirty();
}

void taGroup_impl::InitLeafGp() const {
  if(leaf_gp != NULL)
    return;
  taGroup_impl* ncths = (taGroup_impl*)this;
  ncths->leaf_gp = new TALOG;
  taBase::Own(leaf_gp, ncths);
  InitLeafGp_impl(ncths->leaf_gp);
}

void taGroup_impl::InitLeafGp_impl(TALOG* lg) const {
  if(size > 0)
    lg->Push((taGroup_impl*)this);
  int i;
  for(i=0; i<gp.size; i++)
    FastGp_(i)->InitLeafGp_impl(lg);
}

void taGroup_impl::Copy(const taGroup_impl& cp) {
  taList_impl::Copy(cp);
  gp.Copy(cp.gp);
}

void taGroup_impl::AddEl_(void* it) {
  taList_impl::AddEl_(it);
  UpdateLeafCount_(1);		// not the most efficient, but gets it at a low level
}

void taGroup_impl::Borrow(const taGroup_impl& cp) {
  taList_impl::Borrow(cp);
  gp.Borrow(cp.gp);
}

void taGroup_impl::BorrowUnique(const taGroup_impl& cp) {
  taList_impl::BorrowUnique(cp);
  gp.BorrowUnique(cp.gp);
}

void taGroup_impl::BorrowUniqNameOld(const taGroup_impl& cp) {
  taList_impl::BorrowUniqNameOld(cp);
  gp.BorrowUniqNameOld(cp.gp);
}

void taGroup_impl::BorrowUniqNameNew(const taGroup_impl& cp) {
  taList_impl::BorrowUniqNameNew(cp);
  gp.BorrowUniqNameNew(cp.gp);
}

void taGroup_impl::Copy_Common(const taGroup_impl& cp) {
  taList_impl::Copy_Common(cp);
  gp.Copy_Common(cp.gp);
}

void taGroup_impl::Copy_Duplicate(const taGroup_impl& cp) {
  taList_impl::Copy_Duplicate(cp);
  gp.Copy_Duplicate(cp.gp);
}

void taGroup_impl::Copy_Borrow(const taGroup_impl& cp) {
  taList_impl::Copy_Borrow(cp);
  gp.Copy_Borrow(cp.gp);
}

// save the path of all the elements in the group
int taGroup_impl::Dump_Save_PathR_impl(ostream& strm, TAPtr par, int indent) {
  int rval = taList_impl::Dump_Save_PathR_impl(strm, par, indent); // save first-level
  if(rval == false)
    rval = gp.Dump_Save_PathR_impl(strm, par, indent);
  else
    gp.Dump_Save_PathR_impl(strm, par, indent);
  return rval;
}

int taGroup_impl::Dump_SaveR(ostream& strm, TAPtr par, int indent) {
  taList_impl::Dump_SaveR(strm, par, indent);
  gp.Dump_SaveR(strm, par, indent); // subgroups get saved
  return true;
}

void taGroup_impl::DupeUniqNameOld(const taGroup_impl& cp) {
  taList_impl::DupeUniqNameOld(cp);
  gp.DupeUniqNameOld(cp.gp);
}

void taGroup_impl::DupeUniqNameNew(const taGroup_impl& cp) {
  taList_impl::DupeUniqNameNew(cp);
  gp.DupeUniqNameNew(cp.gp);
}

void taGroup_impl::Duplicate(const taGroup_impl& cp) {
  taList_impl::Duplicate(cp);
  gp.Duplicate(cp.gp);
}

void taGroup_impl::EnforceLeaves(int sz){
  if(sz > leaves)  New(sz - leaves,el_typ);
  while(leaves > sz) RemoveLeaf(leaves-1);
}

void taGroup_impl::EnforceSameStru(const taGroup_impl& cp) {
  taList_impl::EnforceSameStru(cp);
  gp.EnforceSameStru(cp.gp);
  int i;
  for(i=0; i<gp.size; i++) {
    FastGp_(i)->EnforceSameStru(*(cp.FastGp_(i)));
  }
}

int taGroup_impl::FindLeaf(const char* nm) const {
  int idx;
  if((idx = Find(nm)) >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if((idx = sbg->FindLeaf(nm)) >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

int taGroup_impl::FindLeaf(TAPtr it) const {
  int idx;
  if((idx = Find(it)) >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if((idx = sbg->FindLeaf(it)) >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

int taGroup_impl::FindLeaf(TypeDef* it) const {
  int idx;
  if((idx = Find(it)) >= 0)
    return idx;

  int new_idx = size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if((idx = sbg->FindLeaf(it)) >= 0)
      return idx + new_idx;
    new_idx += (int)sbg->leaves;
  }
  return -1;
}

TAPtr taGroup_impl::FindLeafName_(const char* it, int& idx) const {
  idx = FindLeaf(it);
  if(idx >= 0) return Leaf_(idx);
  return NULL;
}

TAPtr taGroup_impl::FindLeafType_(TypeDef* it, int& idx) const {
  idx = FindLeaf(it);
  if(idx >= 0) return Leaf_(idx);
  return NULL;
}

MemberDef* taGroup_impl::FindMembeR(const char* nm, void*& ptr) const {
  String idx_str = nm;
  idx_str = idx_str.before(']');
  if(idx_str != "") {
    idx_str = idx_str.after('[');
    int idx = atoi(idx_str);
    if((size == 0) || (idx >= size)) {
      ptr = NULL;
      return NULL;
    }
    ptr = el[idx];
    return ReturnFindMd();
  }

  int i;
  if((i = FindLeaf(nm)) >= 0) {
    ptr = Leaf_(i);
    return ReturnFindMd();
  }

  MemberDef* rval;
  if((rval = GetTypeDef()->members.FindNameAddrR(nm, (void*)this, ptr)) != NULL)
    return rval;
  int max_srch = MIN(taMisc::search_depth, size);
  for(i=0; i<max_srch; i++) {
    TAPtr first_el = (TAPtr)FastEl_(i);
    if((first_el != NULL) && // only search owned objects
       ((first_el->GetOwner()==NULL) || (first_el->GetOwner() == (taBase *) this))) {
      return first_el->FindMembeR(nm, ptr);
    }
  }
  ptr = NULL;
  return NULL;
}

MemberDef* taGroup_impl::FindMembeR(TypeDef* it, void*& ptr) const {
  int i;
  if((i = FindLeaf(it)) >= 0) {
    ptr = Leaf_(i);
    return ReturnFindMd();
  }

  MemberDef* rval;
  if((rval = GetTypeDef()->members.FindTypeAddrR(it, (void*)this, ptr)) != NULL)
    return rval;
  int max_srch = MIN(taMisc::search_depth, size);
  for(i=0; i<max_srch; i++) {
    TAPtr first_el = (TAPtr)FastEl_(i);
    if((first_el != NULL) && // only search owned objects
       ((first_el->GetOwner()==NULL) || (first_el->GetOwner() == (taBase *) this))) {
      return first_el->FindMembeR(it, ptr);
    }
  }
  ptr = NULL;
  return NULL;
}

TAGPtr taGroup_impl::GetSuperGp_() {
  if(owner == NULL)
    return NULL;
  if(owner->InheritsFrom(TA_taList)) {
    TAPtr ownr = owner->GetOwner();
    if((ownr != NULL) && (ownr->InheritsFrom(TA_taGroup_impl)))
      return (TAGPtr)ownr;
  }
  return NULL;
}

String taGroup_impl::GetValStr(const TypeDef* td, void* par,
	MemberDef* memb_def) const
{
  String nm = " Size: ";
  nm += String(size);
  if(gp.size > 0)
    nm += String(".") + String(gp.size);
  if(leaves != size)
    nm += String(".") + String((int) leaves);
  nm += String(" (") + el_typ->name + ")";
  return nm;
 }

TAPtr taGroup_impl::Leaf_(int idx) const {
  if(idx >= leaves)
    return NULL;
  if(size && (idx < size))
    return (TAPtr)el[idx];

  int nw_idx = (int)idx - size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if(sbg->leaves && (sbg->leaves > nw_idx))
      return sbg->Leaf_(nw_idx);
    nw_idx -= (int)sbg->leaves;
  }
  return NULL;
}

TAGPtr taGroup_impl::LeafGp_(int idx) const {
  if(idx >= leaves)
    return NULL;
  if(size && (idx < size))
    return (TAGPtr)this;

  int nw_idx = (int)idx - size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if(sbg->leaves && (sbg->leaves > nw_idx))
      return sbg->LeafGp_(nw_idx);
    nw_idx -= (int)sbg->leaves;
  }
  return NULL;
}

void taGroup_impl::List(ostream& strm) const {
  taList_impl::List(strm);
  int i;
  for(i=0; i<gp.size; i++)
    FastGp_(i)->List(strm);
}

TAPtr taGroup_impl::New(int no, TypeDef* typ) {
  if (typ == NULL)
    typ = el_typ;
  if(no == 0) {
#ifdef TA_GUI
  if(taMisc::gui_active)
    return gpiGroupNew::New(this, NULL, no, typ);
#endif
    return NULL;
  }

  // if requested typ inherits from the list el type, then 
  // we assume it is for a list el, and create the instances
  if (typ->InheritsFrom(el_base)) {
    TAPtr rval = taList_impl::New(no, typ);
    return rval;
  }
  
  // otherwise, if it is for a group type, we check to make sure
  // it either inherits from the current group type, or the current
  // groups inherits from it -- in the latter case, we create the derived type
  // (there is no officially supported member for specifying group type,
  // so we have to be conservative and assume group must contain subgroups of 
  // at least its own type)
  if (typ->InheritsFrom(&TA_taGroup_impl)) {
    if (GetTypeDef()->InheritsFrom(typ)) {
      typ = GetTypeDef(); 
    } else if (!typ->InheritsFrom(GetTypeDef()))
      goto err;
    TAPtr rval = gp.New(no, typ);
//    UpdateAfterEdit();
    return rval;
  }
err: 
  taMisc::Error("*** Attempt to create type:", typ->name,
		   "in group of type:", GetTypeDef()->name,
		   "with base element type:", el_base->name);
  return NULL;
}

TAPtr taGroup_impl::NewEl_(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
  if(taMisc::gui_active)
    return gpiGroupNew::New(this, NULL, no, typ);
#endif
    return NULL;
  }
  if(typ == NULL)
    typ = el_typ;
  TAPtr rval = taList_impl::New(no, typ);
  return rval;
}

TAGPtr taGroup_impl::NewGp_(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
  if(taMisc::gui_active) {
    gpiGroupNew::New(this, NULL, no, typ);
    return NULL;		// not sure if rval is a group or not
  }
#endif
    return NULL;
  }
  if(typ == NULL)
    typ = GetTypeDef();		// always create one of yourself..
  TAGPtr rval = (TAGPtr)gp.New(no, typ);
//  UpdateAfterEdit();
  return rval;
}

ostream& taGroup_impl::OutputR(ostream& strm, int indent) const {
  taMisc::indent(strm, indent) << name << "[" << size << "] = {\n";
  TypeDef* td = GetTypeDef();
  int i;
  for(i=0; i < td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if(md->HasOption("EDIT_IN_GROUP"))
      md->Output(strm, (void*)this, indent+1);
  }

  for(i=0; i<size; i++) {
    if(el[i] == NULL)	continue;
    ((TAPtr)el[i])->OutputR(strm, indent+1);
  }

  gp.OutputR(strm, indent+1);

  taMisc::indent(strm, indent) << "}\n";
  return strm;
}

bool taGroup_impl::Remove(int i) {
  if(taList_impl::Remove(i)) {
    UpdateLeafCount_(-1);
    return true;
  }
  return false;
}

void taGroup_impl::RemoveAll() {
  gp.RemoveAll();
  taList_impl::RemoveAll();
  leaves = 0;
}

bool taGroup_impl::RemoveLeaf(int idx) {
  if(idx >= leaves)
    return false;
  if(size && (idx < size))
    return Remove(idx);

  int nw_idx = (int)idx - size;
  int i;
  TAGPtr sbg;
  for(i=0; i<gp.size; i++) {
    sbg = FastGp_(i);
    if(sbg->leaves && (sbg->leaves > nw_idx))
      return sbg->RemoveLeaf(nw_idx);
    nw_idx -= (int)sbg->leaves;
  }
  return false;
}

bool taGroup_impl::RemoveLeaf(const char* it) {
  int i;
  if((i = FindLeaf(it)) < 0)
    return false;
  return RemoveLeaf(i);
}

bool taGroup_impl::RemoveLeaf(TAPtr it) {
  int i;
  if((i = FindLeaf(it)) < 0)
    return false;
  return RemoveLeaf(i);
}

int taGroup_impl::ReplaceType(TypeDef* old_type, TypeDef* new_type) {
  int nchanged = taList_impl::ReplaceType(old_type, new_type);
  nchanged += gp.ReplaceType(old_type, new_type);
  int i;
  for(i=0; i<gp.size; i++) {
    nchanged += FastGp_(i)->ReplaceType(old_type, new_type);
  }
  return nchanged;
}

int taGroup_impl::ReplaceAllPtrsThis(TypeDef* obj_typ, void* old_ptr, void* new_ptr) {
  int nchg = taList_impl::ReplaceAllPtrsThis(obj_typ, old_ptr, new_ptr);
  int i;
  for(i=0; i<gp.size; i++) {
    nchg += FastGp_(i)->ReplaceAllPtrsThis(obj_typ, old_ptr, new_ptr);
  }
  return nchg;
}

void taGroup_impl::UpdateLeafCount_(int no) {
  leaves += no;
  if(super_gp != NULL)
    super_gp->UpdateLeafCount_(no);
}


//////////////////////////////////////////////////////////
// 			DMem Stuff			//
//////////////////////////////////////////////////////////

 //note: IMPL does not depend on DMEM or not

#ifdef DMEM_COMPILE

#include <mpi.h>

static String dmem_mpi_decode_err(int ercd) {
  char errstr[MPI_MAX_ERROR_STRING];
  int errlen;
  MPI_Error_string(ercd, errstr, &errlen);
  return String(ercd) + " msg: " + String(errstr);
}

void DMemShare::DebugCmd(const char* function, const char* mpi_call) {
//   String fn = function;
//   if(fn.contains("Symmetrize")) return;
//   if(fn.contains("Sync set") || fn.contains("Symmetrize")) return;
  if(taMisc::dmem_debug) {
    cerr << "proc: " << taMisc::dmem_proc << " fun: "
	 << function << " MPI_" << mpi_call
	 << " start..." << endl;
  }
}

bool DMemShare::ProcErr(int ercd, const char* function, const char* mpi_call) {
  if(ercd == MPI_SUCCESS) {
//     String fn = function;
//     if(fn.contains("Symmetrize")) return true;
//     if(fn.contains("Sync set") || fn.contains("Symmetrize")) return true;
    if(taMisc::dmem_debug) {
      cerr << "proc: " << taMisc::dmem_proc << " fun: "
	   << function << " MPI_" << mpi_call
	   << " SUCCESS!" << endl;
    }
    return true;
  }
  cerr << "proc: " << taMisc::dmem_proc << " fun: "
       << function << " MPI_" << mpi_call
       << " FAILED with code: " << dmem_mpi_decode_err(ercd) << endl;
  return false;
}

void DMemShareVar::Initialize() {
  comm = -1;
  mpi_type = -1;
  max_per_proc = -1;
  n_procs = -1;
  this_proc = -1;
}

void DMemShareVar::InitLinks() {
  taBase::InitLinks();
  taBase::Own(addrs, this);
  taBase::Own(local_proc, this);
  taBase::Own(n_local, this);
  taBase::Own(recv_idx, this);
  taBase::Own(addrs_recv, this);
  taBase::Own(float_send, this);
  taBase::Own(float_recv, this);
  taBase::Own(double_send, this);
  taBase::Own(double_recv, this);
  taBase::Own(int_send, this);
  taBase::Own(int_recv, this);
  taBase::Own(long_send, this);
  taBase::Own(long_recv, this);
}

void DMemShareVar::CutLinks() {
  ResetVar();
  taBase::CutLinks();
}

void DMemShareVar::Copy_(const DMemShareVar& cp) {
  // do we want to actually copy anything here?  ok.
  comm = cp.comm;
  mpi_type = cp.mpi_type;
  max_per_proc = cp.max_per_proc;
  n_procs = cp.n_procs;
  this_proc = cp.this_proc;

  addrs = cp.addrs;
  local_proc = cp.local_proc;
  n_local = cp.n_local;
  recv_idx = cp.recv_idx;
  addrs_recv = cp.addrs_recv;
}

void DMemShareVar::ResetVar() {
  comm = -1;
  mpi_type = -1;
  n_procs = -1;
  this_proc = -1;
  addrs.Reset();
  local_proc.Reset();
  n_local.Reset();
  recv_idx.Reset();
  addrs_recv.Reset();
  float_send.Reset();
  float_recv.Reset();
  double_send.Reset();
  double_recv.Reset();
  int_send.Reset();
  int_recv.Reset();
  long_send.Reset();
  long_recv.Reset();
}

void DMemShareVar::Compile_Var(MPI_Comm cm) {
  comm = cm;
  n_procs = 0; MPI_Comm_size(comm, &n_procs);
  this_proc = 0; MPI_Comm_rank(comm, &this_proc);

  if(n_procs <= 1) return;

  n_local.EnforceSize(n_procs);

  // initialize counts
  for(int i=0;i<n_procs;i++) {
    n_local[i] = 0;
  }

  for(int i=0;i<local_proc.size;i++) {
    n_local[local_proc[i]]++;	// increment counts
  }

  // find max
  max_per_proc = 0;
  for(int i=0;i<n_procs;i++) {
    if(n_local[i] > max_per_proc) max_per_proc = n_local[i];
  }

  static int_Array proc_ctr;
  proc_ctr.EnforceSize(n_procs);
  recv_idx.EnforceSize(n_procs);
  for(int i=0;i<n_procs;i++) {
    proc_ctr[i] = 0;
    recv_idx[i] = max_per_proc * i;
  }

  // allocate the addrs_recv array (max_per_proc * nprocs; 000..111..222...)
  // enforce size:
  int ar_size = max_per_proc * n_procs;
  addrs_recv.EnforceSize(ar_size);
  for(int i=0;i<local_proc.size;i++) {
    int lproc = local_proc[i];
    addrs_recv[recv_idx[lproc] + proc_ctr[lproc]] = addrs[i];
    proc_ctr[lproc]++;
  }

  switch(mpi_type) {
  case MPI_FLOAT: {
    float_send.EnforceSize(addrs_recv.size);
    float_recv.EnforceSize(addrs_recv.size);
    break;
  }
  case MPI_DOUBLE: {
    double_send.EnforceSize(addrs_recv.size);
    double_recv.EnforceSize(addrs_recv.size);
    break;
  }
  case MPI_INT: {
    int_send.EnforceSize(addrs_recv.size);
    int_recv.EnforceSize(addrs_recv.size);
    break;
  }
  case MPI_LONG: {
    long_send.EnforceSize(addrs_recv.size);
    long_recv.EnforceSize(addrs_recv.size);
    break;
  }
  }
}

void DMemShareVar::SyncVar() {
  // basic computation here is to send all of my stuff to all other nodes
  // and for them to send all of their stuff to me
  // 0: send: 0000
  // 1: send: 1111
  // 2: send: 2222
  // all recv: 0000 1111 2222

  if(n_procs <= 1) return;

  if((comm == -1) || (addrs_recv.size != n_procs * max_per_proc)) {
    taMisc::Error("ERROR: SyncVar called before Complie_Var initialized!");
    return;
  }

  int my_idx = recv_idx[this_proc];
  int my_n = n_local[this_proc];
  switch(mpi_type) {
  case MPI_FLOAT: {
    for(int i=0; i< my_n; i++, my_idx++)  float_send[i] = *((float*)addrs_recv[my_idx]);

//     DMEM_MPICALL(MPI_Allgather(float_send.el, max_per_proc, mpi_type, float_recv.el, max_per_proc,
// 			       mpi_type, comm), "SyncVar", "Allgather");

    int errcd = MPI_Allgather(float_send.el, max_per_proc, mpi_type, float_recv.el, max_per_proc,
			       mpi_type, comm);
    if(errcd != MPI_SUCCESS) {
      cerr << "proc: " << taMisc::dmem_proc << " fun: "
	   << "SyncVar" << " MPI_Allgather"
	   << " FAILED with code: " << dmem_mpi_decode_err(errcd)
	   << " max_per_proc: " << max_per_proc
	   << " mpi_type: " << mpi_type
	   << " send.size: " << float_send.size
	   << " recv.size: " << float_recv.size
	   << " comm: " << comm
	   << " this_proc: " << this_proc
	   << " n_procs: " << n_procs
	   << endl;
    }

    for(int proc=0;proc<n_procs;proc++) {
      if(proc == this_proc) continue;
      int p_idx = recv_idx[proc];
      for(int i=0; i<n_local[proc]; i++, p_idx++) *((float*)addrs_recv[p_idx]) = float_recv[p_idx];
    }
    break;
  }
  case MPI_DOUBLE: {
    for(int i=0;i<my_n;i++, my_idx++)  double_send[i] = *((double*)addrs_recv[my_idx]);
    DMEM_MPICALL(MPI_Allgather(double_send.el, max_per_proc, mpi_type, double_recv.el, max_per_proc,
			       mpi_type, comm), "SyncVar", "Allgather");
    for(int proc=0;proc<n_procs;proc++) {
      if(proc == this_proc) continue;
      int p_idx = recv_idx[proc];
      for(int i=0; i<n_local[proc]; i++, p_idx++) *((double*)addrs_recv[p_idx]) = double_recv[p_idx];
    }
    break;
  }
  case MPI_INT: {
    for(int i=0;i<my_n;i++, my_idx++)  int_send[i] = *((int*)addrs_recv[my_idx]);
    DMEM_MPICALL(MPI_Allgather(int_send.el, max_per_proc, mpi_type, int_recv.el, max_per_proc,
			       mpi_type, comm), "SyncVar", "Allgather");
    for(int proc=0;proc<n_procs;proc++) {
      if(proc == this_proc) continue;
      int p_idx = recv_idx[proc];
      for(int i=0; i<n_local[proc]; i++, p_idx++) *((int*)addrs_recv[p_idx]) = int_recv[p_idx];
    }
    break;
  }
  case MPI_LONG: {
    for(int i=0;i<my_n;i++, my_idx++)  long_send[i] = *((long*)addrs_recv[my_idx]);
    DMEM_MPICALL(MPI_Allgather(long_send.el, max_per_proc, mpi_type, long_recv.el, max_per_proc,
			       mpi_type, comm), "SyncVar", "Allgather");
    for(int proc=0;proc<n_procs;proc++) {
      if(proc == this_proc) continue;
      int p_idx = recv_idx[proc];
      for(int i=0; i<n_local[proc]; i++, p_idx++) *((long*)addrs_recv[p_idx]) = long_recv[p_idx];
    }
    break;
  }
  }
}

void DMemShareVar::AggVar(MPI_Op op) {
  // basic computation here is to send all of my stuff to all other nodes
  // and for them to send all of their stuff to me
  // 0: send: 0000
  // 1: send: 1111
  // 2: send: 2222
  // all recv: 0000 1111 2222

  if(n_procs <= 1) return;

  if((comm == -1) || (addrs_recv.size != n_procs * max_per_proc)) {
    taMisc::Error("ERROR: AggVar called before Complie_Var initialized!");
    return;
  }

  switch(mpi_type) {
  case MPI_FLOAT: {
    for(int i=0; i< addrs.size; i++)  float_send[i] = *((float*)addrs[i]);
    DMEM_MPICALL(MPI_Allreduce(float_send.el, float_recv.el, addrs.size, mpi_type, op, comm),
		 "AggVar", "Allreduce");
    for(int i=0; i< addrs.size; i++) *((float*)addrs[i]) = float_recv[i];
    break;
  }
  case MPI_DOUBLE: {
    for(int i=0; i< addrs.size; i++)  double_send[i] = *((double*)addrs[i]);
    DMEM_MPICALL(MPI_Allreduce(double_send.el, double_recv.el, addrs.size, mpi_type, op, comm),
		 "AggVar", "Allreduce");
    for(int i=0; i< addrs.size; i++) *((double*)addrs[i]) = double_recv[i];
    break;
  }
  case MPI_INT: {
    for(int i=0; i< addrs.size; i++)  int_send[i] = *((int*)addrs[i]);
    DMEM_MPICALL(MPI_Allreduce(int_send.el, int_recv.el, addrs.size, mpi_type, op, comm),
		 "AggVar", "Allreduce");
    for(int i=0; i< addrs.size; i++) *((int*)addrs[i]) = int_recv[i];
    break;
  }
  case MPI_LONG: {
    for(int i=0; i< addrs.size; i++)  long_send[i] = *((long*)addrs[i]);
    DMEM_MPICALL(MPI_Allreduce(long_send.el, long_recv.el, addrs.size, mpi_type, op, comm),
		 "AggVar", "Allreduce");
    for(int i=0; i< addrs.size; i++) *((long*)addrs[i]) = long_recv[i];
    break;
  }
  }
}

//////////////////////////////////////////////////////////

void DMemShare::Initialize() {
  comm = MPI_COMM_WORLD;
  vars.SetBaseType(&TA_DMemShareVar);
}

void DMemShare::InitLinks() {
  taList<taBase>::InitLinks();
  taBase::Own(vars, this);
}

void DMemShare::CutLinks() {
  vars.Reset();
  taBase_List::CutLinks();
}

void DMemShare::Copy_(const DMemShare& cp) {
  vars = cp.vars;
}

void DMemShare::SetLocal_Sequential() {
  int np = 0; MPI_Comm_size(comm, &np);
  if(size <= 0) return;
  int this_proc = 0; MPI_Comm_rank(comm, &this_proc);
  for (int i=0; i < size; i++) {
    taBase* shr_item = FastEl(i);
    shr_item->DMem_SetLocalProc(i % np);
    shr_item->DMem_SetThisProc(this_proc);
  }
}

void DMemShare::Compile_ShareVar(TypeDef* td, taBase* shr_item, MemberDef* par_md) {
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members.FastEl(m);
    String shrstr = md->OptionAfter("DMEM_SHARE_SET_");
    if(shrstr.empty()) continue;
    int shrset = (int)shrstr;

    if(shrset >= vars.size) {
      taMisc::Error("WARNING: DMEM_SHARE_SET_# number is greater than max specified in object typedef DMEM_SHARE_SETS_# !");
      continue;
    }

    DMemShareVar* var = (DMemShareVar*)vars[shrset];

    int new_type = -1;
    if(md->type->ptr > 0) {
      taMisc::Error("WARNING: DMEM_SHARE_SET Specified for a pointer in type:",
		    td->name, ", member:", md->name,
		    "Pointers can not be shared.");
      continue;
    }
    if(md->type->InheritsFormal(TA_class)) {
      if(par_md != NULL) {
	taMisc::Error("WARNING: DMEM_SHARE_SET in too many nested objects: only one level of subobject nesting allowed!  type:",
		      td->name, ", member:", md->name);
	continue;
      }
      Compile_ShareVar(md->type, shr_item, md);
      continue;
    }
    else if (md->type->InheritsFrom(TA_double)) {
      new_type = (int)MPI_DOUBLE;
    }
    else if (md->type->InheritsFrom(TA_float)) {
      new_type = (int)MPI_FLOAT;
    }
    else if (md->type->InheritsFrom(TA_int)) {
      new_type = (int)MPI_INT;
    }
    else if (md->type->InheritsFrom(TA_enum)) {
      new_type = (int)MPI_INT;
    }
    else if (md->type->InheritsFrom(TA_long)) {
      new_type = (int)MPI_LONG;
    }
    else {
      taMisc::Error("WARNING: DMEM_SHARE_SET Specified for an unrecognized type.",
		    td->name, ", member:", md->name,
		    "unrecoginized types can not be shared.");
      continue;
    }
    if(var->mpi_type == -1)
      var->mpi_type = new_type;
    if(var->mpi_type != new_type) {
      taMisc::Error("WARNING: Two different types specified for the same DMEM_SHARE_SET.",
		    "All variables in same share set must be of same type!  Type:",
		    td->name, ", member:", md->name);
      continue;
    }

    void* addr = NULL;
    if(par_md == NULL) {
      addr = md->GetOff(shr_item);
    }
    else {
      void* par_base = par_md->GetOff(shr_item);
      addr = md->GetOff(par_base);
    }

    var->addrs.Add(addr);
    var->local_proc.Add(shr_item->DMem_GetLocalProc());
  }
}

void DMemShare::Compile_ShareTypes() {
  if(size <= 0) return;

  int np = 0; MPI_Comm_size(comm, &np);

  int max_share_sets = 0;

  TypeDef* last_type = NULL;
  for(int i=0; i < size; i++) {
    taBase* shr_item = FastEl(i);
    TypeDef* td = shr_item->GetTypeDef();
    if(td == last_type) continue;
    last_type = td;
    for(int j=0;j<td->opts.size;j++) {
      String shrstr = td->opts[j].after("DMEM_SHARE_SETS_");
      if(shrstr.empty()) continue;
      int shrsets = (int)shrstr;
      max_share_sets = MAX(shrsets, max_share_sets);
    }
  }

  vars.EnforceSize(max_share_sets);
  for(int i=0; i < vars.size; i++) {
    DMemShareVar* var = (DMemShareVar*)vars[i];
    var->ResetVar();
    var->n_procs = np;
  }

  if(np <= 1) return;

  for(int i=0; i < size; i++) {
    taBase* shr_item = FastEl(i);
    TypeDef* td = shr_item->GetTypeDef();
    Compile_ShareVar(td, shr_item);
  }

  for(int i=0; i < vars.size; i++) {
    ((DMemShareVar*)vars[i])->Compile_Var(comm);
  }
}

void DMemShare::DistributeItems() {
  SetLocal_Sequential();
  Compile_ShareTypes();
}

void DMemShare::Sync(int share_set) {
  if(share_set >= vars.size) {
    taMisc::Error("DMemShare::Sync -- attempt to sync for share set beyond number allocated:",
		  String(share_set));
    return;
  }
  DMemShareVar* var = (DMemShareVar*)vars[share_set];
  var->SyncVar();
}

void DMemShare::Aggregate(int share_set, MPI_Op op) {
  if(share_set >= vars.size) {
    taMisc::Error("DMemShare::Sync -- attempt to sync for share set beyond number allocated:",
		  String(share_set));
    return;
  }
  DMemShareVar* var = (DMemShareVar*)vars[share_set];
  var->AggVar(op);
}

void DMemShare::ExtractLocalFromList(taPtrList_impl& global_list, taPtrList_impl& local_list) {
  local_list.Reset();
  for (int j=0; j<global_list.size; j++) {
    if (((taBase *)global_list.FastEl_(j))->DMem_IsLocal()) {
      local_list.Link_(global_list.FastEl_(j));
    }
  }
}

stringstream* DMemShare::cmdstream = NULL;

void DMemShare::InitCmdStream() {
  CloseCmdStream();
  cmdstream = new stringstream(ios::in | ios::out);
}

void DMemShare::CloseCmdStream() {
  if(cmdstream != NULL) delete cmdstream;
  cmdstream = NULL;
}

#endif

#ifdef TA_GUI

/*
  CLipboard Operations

  Src: Group
  Dst: Group

*/
void taGroup_impl::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child, const taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  int subgrp_idx = -1;
  if (child) subgrp_idx = gp.Find(child);

  // if it is a group item, or is null, then we can do group operations, so we call our G version
  if ((child == NULL) || (subgrp_idx >= 0))
    ChildQueryEditActionsG_impl(md, subgrp_idx, (taGroup_impl*)child, ms, allowed, forbidden);

  // if child was a group item, then we don't pass the child to the base (since it doesn't boggle group items)
  if (subgrp_idx >=0)
    taList_impl::ChildQueryEditActions_impl(md, NULL, ms, allowed, forbidden);
  else
    taList_impl::ChildQueryEditActions_impl(md, child, ms, allowed, forbidden);
}

void taGroup_impl::ChildQueryEditActionsG_impl(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, const taiMimeSource* ms, int& allowed, int& forbidden)
{
  // SRC ops
  if (subgrp) {
    // CUT generally always allowed (will either DELETE or UNLINK src item, depending on context)
    allowed |= taiClipData::EA_CUT;
    // Delete only allowed if we are the owner
    if (subgrp->GetOwner() == &gp)
      allowed |= taiClipData::EA_DELETE;
    else // otherwise, it is unlinking, not deleting
      allowed |= taiClipData::EA_UNLINK;
  }

  if (ms == NULL) return; // src op query
  // DST ops
  // if not a taBase type of object, no more applicable
  if (!ms->is_tab()) return;
  if (!ms->IsThisProcess())
    forbidden &= taiClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for G action calls

  // generic group paste only allows exact type, so we check for each inheriting from the other, which means same
  bool right_gptype = (ms->td() && (ms->td()->InheritsFrom(GetTypeDef()) && GetTypeDef()->InheritsFrom(ms->td())));

  //TODO: should we allow copy?
  if (right_gptype)
    allowed |= (taiClipData::EA_PASTE | taiClipData::EA_DROP_MOVE);
}

// called by a child -- lists etc. can then allow drops on the child, to indicate inserting into the list, etc.
int taGroup_impl::ChildEditAction_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea) {
  // if child exists, but is not a group item, then just delegate down to base
  int subgrp_idx = -1;
  if (child) {
    subgrp_idx = gp.Find(child);
    if (subgrp_idx < 0)
      return taList_impl::ChildEditAction_impl(md, child, ms, ea);
  }

  // we will be calling our own G routines...
  // however, if child is NULL, and our ops don't do anything, then we must call base ops
  // determine the list-only operations allowed/forbidden, and apply to ea
  int rval = taiClipData::ER_IGNORED;
  int allowed = 0;
  int forbidden = 0;
  ChildQueryEditActionsG_impl(md, subgrp_idx, (taGroup_impl*)child, ms, allowed, forbidden);
  if (ea & forbidden) return taiClipData::ER_FORBIDDEN; // requested op was forbidden
  int eax = ea & (allowed & (~forbidden));

  if (eax & taiClipData::EA_SRC_OPS) {
    rval = ChildEditActionGS_impl(md, subgrp_idx, (taGroup_impl*)child, eax);
  } else  if (eax & taiClipData::EA_DST_OPS) {
    if (ms == NULL) return taiClipData::ER_IGNORED;

    // decode src location
    if (ms->IsThisProcess())
      rval = ChildEditActionGD_impl_inproc(md, subgrp_idx, (taGroup_impl*)child, ms, eax);
    else
      // DST OP, SRC OUT OF PROCESS
      rval = ChildEditActionGD_impl_ext(md, subgrp_idx, (taGroup_impl*)child, ms, eax);
  }

  if ((rval == 0) && (child == NULL))
      rval = taList_impl::ChildEditAction_impl(md, NULL, ms, ea);
  return rval;
}

int taGroup_impl::ChildEditActionGS_impl(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, int ea)
{
  // if the child is a group, we handle it, otherwise we let base class handle it
  switch (ea & taiClipData::EA_OP_MASK) {
  //note: COPY is handled by the child object itself
  case taiClipData::EA_CUT: return 1; //nothing to do, just acknowledge -- deletion triggered by the dst, whether local or remote
  case taiClipData::EA_DELETE: {
    if (subgrp) {
      RemoveGp(subgrp_idx);
      return taiClipData::ER_OK;
    } else return taiClipData::ER_ERROR; // error TODO: error message
  }
  case taiClipData::EA_DRAG: return taiClipData::ER_OK; // nothing for us to do on the drag
  default: break; // compiler food
  }
  return taiClipData::ER_IGNORED; // this function never calls down to List
}

int taGroup_impl::ChildEditActionGD_impl_inproc(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp,
  taiMimeSource* ms, int ea)
{
  // if src is not even a taBase, we just stop
  if (!ms->is_tab()) return taiClipData::ER_IGNORED;
  int srcgrp_idx = -1; // -1 means not in this group
  taBase* srcobj = NULL;

  // only fetch obj for ops that require it
  if (ea & (taiClipData::EA_PASTE | taiClipData::EA_LINK  | taiClipData::EA_DROP_COPY |
    taiClipData::EA_DROP_LINK | taiClipData::EA_DROP_MOVE))
  {
    srcobj = (taBase*)ms->object();
    if (srcobj == NULL) {
      taMisc::Error("Could not retrieve object for operation.");
      return taiClipData::ER_ERROR;
    }
    // already in this list? (affects how we do drops/copies, etc.)
    srcgrp_idx = gp.Find(srcobj);
  }
/*TODO: work out logistics for this... maybe this should only be for when the src is a group
  // All non-move paste ops (i.e., copy an object)
  if (
    (ea & (taiClipData::EA_DROP_COPY)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE) && (ms->src_action() & taiClipData::EA_SRC_COPY))
  ) {
    // TODO: instead of cloning, we might be better off just streaming a new copy
    // since this will better guarantee that in-proc and outof-proc behavior is same
    taBase* new_obj = obj->Clone();
    //TODO: maybe the renaming should be delayed until put in list, or maybe better, done by list???
    new_obj->SetDefaultName(); // should give it a new name, so not confused with existing obj
    int new_idx;
    if (itm_idx <= 0) 
      new_idx = 0; // if dest is list, then insert at beginning
    else if (itm_idx == (size - 1)) 
      new_idx = -1; // if clicked on last, then insert at end
    else new_idx = itm_idx + 1;
    subgrp->Insert(new_obj, new_idx);
    return taiClipData::ER_OK;
  } */
  
  // All Move-like ops
  if (
    (ea & (taiClipData::EA_DROP_MOVE)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE) && (ms->src_action() & taiClipData::EA_SRC_CUT))
  ) {
    if (srcobj == subgrp) return taiClipData::ER_OK; // nop
    if (srcgrp_idx >= 0) { // in this group: just do a group move
      // to_idx will differ depending on whether dst is before or after the src object
      if (subgrp_idx < srcgrp_idx) { // for before, to will be dst + 1
        gp.Move(srcgrp_idx, subgrp_idx + 1);
      } else if (subgrp_idx > srcgrp_idx) { // for after, to will just be the dst
        gp.Move(srcgrp_idx, subgrp_idx);
      } else return taiClipData::ER_OK; // do nothing case of drop on self
    } else { // not directly in this group, need to do a transfer
      if (gp.Transfer(srcobj)) { // should always succeed -- only fails if we already own item
      // was added at end, fix up location, if necessary
        gp.Move(gp.size - 1, subgrp_idx + 1);
      } else return taiClipData::ER_ERROR; //TODO: error message
    }
    // NOTE: we don't acknowledge action to source because we moved the item ourself
    return taiClipData::ER_OK;
  }

  // Link ops
  if (ea &
    (taiClipData::EA_LINK | taiClipData::EA_DROP_LINK))
  {
    if (srcgrp_idx >= 0) return taiClipData::ER_FORBIDDEN; // in this list: link forbidden
    gp.InsertLink(srcobj, srcgrp_idx + 1);
    return taiClipData::ER_OK;
  }
  return taiClipData::ER_IGNORED;
}
int taGroup_impl::ChildEditActionGD_impl_ext(const MemberDef* md, int subgrp_idx, taGroup_impl* subgrp, taiMimeSource* ms, int ea)
{
  // if src is not even a taBase, we just stop
  if (!ms->is_tab()) return taiClipData::ER_IGNORED;

  // DST OPS WHEN SRC OBJECT IS OUT OF PROCESS
  switch (ea & taiClipData::EA_OP_MASK) {
  case taiClipData::EA_DROP_COPY:
  case taiClipData::EA_DROP_MOVE:
  case taiClipData::EA_PASTE:
  {
    istringstream istr;
    if (ms->object_data(istr) > 0) {
      TypeDef* td = GetTypeDef();
      int dump_val = td->Dump_Load(istr, this, this);
      if (dump_val == 0) {
        //TODO: error output
        return taiClipData::ER_ERROR; // load failed
      }
      // delete from source if it was a CUT or similar
      if ((ms->src_action() & (taiClipData::EA_SRC_CUT)) || (ea & (taiClipData::EA_DROP_MOVE)))
      {
        ms->rem_data_taken();
      }
      return taiClipData::ER_OK;
    } else { // no data
      return taiClipData::ER_ERROR; //TODO: error message
    }
  }
  }
  return taiClipData::ER_IGNORED;
}


#endif

