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


//////////////////////////////////////////////////////////
// 			DMem Stuff			//
//////////////////////////////////////////////////////////

 //note: IMPL does not depend on DMEM or not

#include "ta_dmem.h"

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

#endif // DMEM_COMPILE
