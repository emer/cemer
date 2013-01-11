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

#include "DMemComm.h"

#ifdef DMEM_COMPILE

void DMemComm::Initialize() {
  comm = MPI_COMM_WORLD;
  group = MPI_GROUP_NULL;
  nprocs = taMisc::dmem_nprocs;
  this_proc = taMisc::dmem_proc;
}

void DMemComm::Destroy() {
  FreeComm();
  CutLinks();
}

void DMemComm::FreeComm() {
  if((comm != MPI_COMM_WORLD) && (comm != MPI_COMM_SELF)) {
    DMEM_MPICALL(MPI_Comm_free((MPI_Comm*)&comm), "DMemComm::FreeComm", "Comm free");
    DMEM_MPICALL(MPI_Group_free((MPI_Group*)&group), "DMemComm::FreeComm", "Group free");
  }
  comm = MPI_COMM_WORLD;
  group = MPI_GROUP_NULL;
  nprocs = taMisc::dmem_nprocs;
  this_proc = taMisc::dmem_proc;
}

void DMemComm::MakeCommFmRanks() {
  int cursz = 0; MPI_Comm_size((MPI_Comm)comm, &cursz);
  if(cursz == nprocs) return;	// already configured!
  FreeComm();			// free any existing
  MPI_Group worldgp;
  DMEM_MPICALL(MPI_Comm_group(MPI_COMM_WORLD, &worldgp), "DMemComm::CommSubGrouped",
	       "Comm_group");

  DMEM_MPICALL(MPI_Group_incl(worldgp, nprocs, ranks.el, (MPI_Group*)&group),
	       "DMemComm::CommSubGrouped", "Group_incl");
  DMEM_MPICALL(MPI_Comm_create(MPI_COMM_WORLD, (MPI_Group)group, (MPI_Comm*)&comm),
	       "DMemComm::CommSubGrouped", "Comm_create");
  GetThisProc();
}

void DMemComm::CommAll() {
  FreeComm();			// defaults to world!
}

void DMemComm::CommSelf() {
  FreeComm();
  comm = MPI_COMM_SELF;
  group = MPI_GROUP_NULL;
  nprocs = 1;
  this_proc = 0;
}

int DMemComm::GetThisProc() {
  MPI_Comm_rank(comm, &this_proc);
  return this_proc;
}

void DMemComm::CommSubGpInner(int sub_gp_size) {
  if(taMisc::dmem_nprocs <= 1 || sub_gp_size <= 1) {
    CommSelf();
    return;
  }
  if(sub_gp_size > taMisc::dmem_nprocs)
    sub_gp_size = taMisc::dmem_nprocs;
  if(taMisc::dmem_nprocs % sub_gp_size != 0) {
    taMisc::Error("CommSubGrouped: the total number of processes:",
		  String(taMisc::dmem_nprocs),
		  "is not an even multiple of the subgroup size:", String(sub_gp_size));
    CommSelf();
    return;
  }

  nprocs = sub_gp_size; // inner-group size
  int cursz = 0; MPI_Comm_size((MPI_Comm)comm, &cursz);
  if(cursz == nprocs) return;	// already configured!

  // o0:   o1:    <- outer loop
  // i0 i1 i0 i1  <- inner loop
  // 0  1  2  3   <- proc no
  // *  *         <- in group, e.g. if I'm an odd #'d proc
  ranks.SetSize(nprocs);
  int myouter = taMisc::dmem_proc / sub_gp_size;
  int stinner = myouter * sub_gp_size;
  for(int i = 0;i<nprocs; i++)
    ranks[i] = stinner + i;
  MakeCommFmRanks();
}

void DMemComm::CommSubGpOuter(int sub_gp_size) {
  if(taMisc::dmem_nprocs <= 1 || sub_gp_size <= 1) {
    CommAll();
    return;
  }
  if(sub_gp_size > taMisc::dmem_nprocs)
    sub_gp_size = taMisc::dmem_nprocs;
  if(taMisc::dmem_nprocs % sub_gp_size != 0) {
    taMisc::Error("CommSubGrouped: the total number of processes:",
		  String(taMisc::dmem_nprocs),
		  "is not an even multiple of the subgroup size:", String(sub_gp_size));
    CommAll();
    return;
  }

  nprocs = taMisc::dmem_nprocs / sub_gp_size; // outer-group size
  int cursz = 0; MPI_Comm_size((MPI_Comm)comm, &cursz);
  if(cursz == nprocs) return;	// already configured!

  // o0:   o1:    <- outer loop
  // i0 i1 i0 i1  <- inner loop
  // 0  1  2  3   <- proc no
  //    *     *   <- in group, e.g. if I'm an odd #'d proc
  ranks.SetSize(nprocs);
  int myinner = taMisc::dmem_proc % sub_gp_size;
  for(int i = 0;i<nprocs; i++)
    ranks[i] = myinner + (i * sub_gp_size);
  MakeCommFmRanks();
}

#endif // DMEM_COMPILE
