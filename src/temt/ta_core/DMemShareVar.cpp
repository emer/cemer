// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "DMemShareVar.h"

TA_BASEFUNS_CTORS_DEFN(DMemShareVar);

#ifdef DMEM_COMPILE

#include <taMisc>
#include <DMemShare>

using namespace std;

static String dmem_mpi_decode_err(int ercd) {
  char errstr[MPI_MAX_ERROR_STRING];
  int errlen;
  MPI_Error_string(ercd, errstr, &errlen);
  return String(ercd) + " msg: " + String(errstr);
}

void DMemShareVar::Initialize() {
  comm = MPI_COMM_NULL;
  mpi_type = MPI_DATATYPE_NULL;
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
  comm = MPI_COMM_NULL;
  mpi_type = MPI_DATATYPE_NULL;
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
}

void DMemShareVar::Compile_Var(MPI_Comm cm) {
  comm = cm;
  n_procs = 0; MPI_Comm_size(comm, &n_procs);
  this_proc = 0; MPI_Comm_rank(comm, &this_proc);

  if(n_procs <= 1) return;

  n_local.SetSize(n_procs);

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
  proc_ctr.SetSize(n_procs);
  recv_idx.SetSize(n_procs);
  for(int i=0;i<n_procs;i++) {
    proc_ctr[i] = 0;
    recv_idx[i] = max_per_proc * i;
  }

  // allocate the addrs_recv array (max_per_proc * nprocs; 000..111..222...)
  // enforce size:
  int ar_size = max_per_proc * n_procs;
  addrs_recv.SetSize(ar_size);
  for(int i=0;i<local_proc.size;i++) {
    int lproc = local_proc[i];
    addrs_recv[recv_idx[lproc] + proc_ctr[lproc]] = addrs[i];
    proc_ctr[lproc]++;
  }

  if (mpi_type ==  MPI_FLOAT) {
    float_send.SetSize(addrs_recv.size);
    float_recv.SetSize(addrs_recv.size);
  }
  else if (mpi_type == MPI_DOUBLE) {
    double_send.SetSize(addrs_recv.size);
    double_recv.SetSize(addrs_recv.size);
  }
  else if (mpi_type == MPI_INT) {
    int_send.SetSize(addrs_recv.size);
    int_recv.SetSize(addrs_recv.size);
  }
  //note: we don't allow creation of other types
}

void DMemShareVar::SyncVar() {
  // basic computation here is to send all of my stuff to all other nodes
  // and for them to send all of their stuff to me
  // 0: send: 0000
  // 1: send: 1111
  // 2: send: 2222
  // all recv: 0000 1111 2222

  if(n_procs <= 1) return;

  if((comm == MPI_COMM_NULL) || (addrs_recv.size != n_procs * max_per_proc)) {
    taMisc::Error("ERROR: SyncVar called before Complie_Var initialized!");
    return;
  }

  int my_idx = recv_idx[this_proc];
  int my_n = n_local[this_proc];
  if (mpi_type == MPI_FLOAT) {
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
  }
  else if (mpi_type == MPI_DOUBLE) {
    for(int i=0;i<my_n;i++, my_idx++)  double_send[i] = *((double*)addrs_recv[my_idx]);
    DMEM_MPICALL(MPI_Allgather(double_send.el, max_per_proc, mpi_type, double_recv.el, max_per_proc,
			       mpi_type, comm), "SyncVar", "Allgather");
    for(int proc=0;proc<n_procs;proc++) {
      if(proc == this_proc) continue;
      int p_idx = recv_idx[proc];
      for(int i=0; i<n_local[proc]; i++, p_idx++) *((double*)addrs_recv[p_idx]) = double_recv[p_idx];
    }
  }
  else if (mpi_type == MPI_INT) {
    for(int i=0;i<my_n;i++, my_idx++)  int_send[i] = *((int*)addrs_recv[my_idx]);
    DMEM_MPICALL(MPI_Allgather(int_send.el, max_per_proc, mpi_type, int_recv.el, max_per_proc,
			       mpi_type, comm), "SyncVar", "Allgather");
    for(int proc=0;proc<n_procs;proc++) {
      if(proc == this_proc) continue;
      int p_idx = recv_idx[proc];
      for(int i=0; i<n_local[proc]; i++, p_idx++) *((int*)addrs_recv[p_idx]) = int_recv[p_idx];
    }
  }
  // note: we don't allow creation of any other tyeps
}

void DMemShareVar::AggVar(MPI_Op op) {
  // basic computation here is to send all of my stuff to all other nodes
  // and for them to send all of their stuff to me
  // 0: send: 0000
  // 1: send: 1111
  // 2: send: 2222
  // all recv: 0000 1111 2222

  if(n_procs <= 1) return;

  if((comm == MPI_COMM_NULL) || (addrs_recv.size != n_procs * max_per_proc)) {
    taMisc::Error("ERROR: AggVar called before Complie_Var initialized!");
    return;
  }

  if (mpi_type == MPI_FLOAT) {
    for(int i=0; i< addrs.size; i++)  float_send[i] = *((float*)addrs[i]);
    DMEM_MPICALL(MPI_Allreduce(float_send.el, float_recv.el, addrs.size, mpi_type, op, comm),
		 "AggVar", "Allreduce");
    for(int i=0; i< addrs.size; i++) *((float*)addrs[i]) = float_recv[i];
  }
  else if (mpi_type == MPI_DOUBLE) {
    for(int i=0; i< addrs.size; i++)  double_send[i] = *((double*)addrs[i]);
    DMEM_MPICALL(MPI_Allreduce(double_send.el, double_recv.el, addrs.size, mpi_type, op, comm),
		 "AggVar", "Allreduce");
    for(int i=0; i< addrs.size; i++) *((double*)addrs[i]) = double_recv[i];
  }
  else if (mpi_type == MPI_INT) {
    for(int i=0; i< addrs.size; i++)  int_send[i] = *((int*)addrs[i]);
    DMEM_MPICALL(MPI_Allreduce(int_send.el, int_recv.el, addrs.size, mpi_type, op, comm),
		 "AggVar", "Allreduce");
    for(int i=0; i< addrs.size; i++) *((int*)addrs[i]) = int_recv[i];
  }
}

#endif // DMEM_COMPILE
