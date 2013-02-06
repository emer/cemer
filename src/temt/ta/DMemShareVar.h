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

#ifndef DMemShareVar_h
#define DMemShareVar_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef DMEM_COMPILE
#include <mpi.h>

#include <voidptr_Array>
#include <int_Array>
#include <double_Array>
#include <float_Array>

TypeDef_Of(DMemShareVar);

class TA_API DMemShareVar : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem definition of a variable of a given type (FLOAT, DOUBLE, INT) that each proc has some instances of (can be multiple members of a given object) -- these can all be Allgather'ed to sync across procs
  INHERITED(taBase)
public:
  MPI_Comm	comm;		// #IGNORE communicator associated with these shared objs
  MPI_Datatype	mpi_type;	// #IGNORE mpi's type for this variable
  int		max_per_proc;	// #IGNORE maximum number of vars per any one proc
  int		n_procs;	// #IGNORE number of processors in this communicator (set during Compile_Var)
  int		this_proc;	// #IGNORE proc id (rank) of this processor in communicator (set during Compile_Var)

  // the following two must be initialized for all the data to be shared prior to calling Compile_Var
  voidptr_Array	addrs;		// #IGNORE addresses for each item to be shared (one for each individual data item)
  int_Array 	local_proc; 	// #IGNORE which proc each guy is local to (one for each individual data item)

  int_Array	n_local; 	// #IGNORE number of local variables for each process (size nprocs)
  int_Array	recv_idx;	// #IGNORE starting indicies into addrs_recv list for each proc (size nprocs)
  voidptr_Array addrs_recv; 	// #IGNORE addresses in recv format (size nprocs * max_per_proc; 000..111..222...)

  // the following are contiguous data arrays for sending and gathering from all other procs
  // (size for both = n_procs * max_per_proc; send only needs max_per_proc for allgather, but needs all for allreduce)
  float_Array	float_send; 	// #IGNORE
  float_Array	float_recv; 	// #IGNORE
  double_Array	double_send; 	// #IGNORE
  double_Array	double_recv; 	// #IGNORE
  int_Array	int_send; 	// #IGNORE
  int_Array	int_recv; 	// #IGNORE

  virtual void	Compile_Var(MPI_Comm cm); // #IGNORE call this after updating the variable info
  virtual void 	SyncVar();	// #IGNORE synchronize variable across procs (allgather, so all procs have all data)
  virtual void 	AggVar(MPI_Op op); // #IGNORE aggregate variable across procs (allreduce: each sends and recvs all data using op to merge w/ existing vals)

  virtual void 	ResetVar();	// #IGNORE reset variable info

  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DMemShareVar& cp);
  TA_BASEFUNS(DMemShareVar);
private:
  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
};

#else

TypeDef_Of(DMemShareVar);

class TA_API DMemShareVar : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem definition of a variable of a given type (FLOAT, DOUBLE, INT) that each proc has some instances of (can be multiple members of a given object) -- these can all be Allgather'ed to sync across procs
INHERITED(taBase)
public:

  void	Dummy(const DMemShareVar&) { };
  TA_BASEFUNS(DMemShareVar);
private:
  NOCOPY(DMemShareVar)
  void 	Initialize() { };
  void 	Destroy()	{ };
};

#endif // DMEM_COMPILE

#endif // DMemShareVar_h
