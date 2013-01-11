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

#ifndef DMemAggVars_h
#define DMemAggVars_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:

#ifdef DMEM_COMPILE
#include <mpi.h>

#include <float_Array>
#include <double_Array>
#include <int_Array>
#include <voidptr_Array>

class TA_API DMemAggVars : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##CAT_DMem collection of a variables of a different types (FLOAT, DOUBLE, INT) that *each proc has an instance of* (can be multiple members of a given object) -- these can all be Allreduced'ed to aggregate across procs (must all use same agg op -- use diff objs for diff ops if fixed)
INHERITED(taBase)
public:
  MPI_Comm	comm;		// #IGNORE communicator to use in aggregating these variables
  MPI_Op	agg_op;		// #IGNORE operator to use in aggregation, if this is fixed and determined by the member comment directive (if dynamic, leave as MPI_OP_NULL)

  // the following two must be initialized for all the data to be shared prior to calling Compile_Var
  voidptr_Array	addrs;		// #IGNORE local processor addresses for each item to be shared (one for each individual data item)
#ifndef __MAKETA__
  MPI_Datatype_PArray	types; 	// #IGNORE mpi_type of each item (INT, FLOAT, DOUBLE)
#endif  
  int_Array	data_idx;	// #IGNORE index into send/recv array of given type

  // the following are contiguous data arrays for sending and gathering from all other procs
  float_Array	float_send; 	// #IGNORE
  float_Array	float_recv; 	// #IGNORE
  double_Array	double_send; 	// #IGNORE
  double_Array	double_recv; 	// #IGNORE
  int_Array	int_send; 	// #IGNORE
  int_Array	int_recv; 	// #IGNORE

  static String OpToStr(MPI_Op op);
  // get string representation of op

  virtual void	ScanMembers(TypeDef* td, void* base);
  // #IGNORE scan members of type for DMEM_AGG_op comment directives & get their addrs; DMEM_AGG_DYN = dynamic op, for agg_op = -1
    virtual void ScanMembers_impl(TypeDef* td, void* base);
    // #IGNORE scan members of type for DMEM_AGG_op comment directives & get their addrs; DMEM_AGG_DYN = dynamic op, for agg_op = -1
  virtual void	CompileVars();
  // #IGNORE allocate recv/send data and data_idx values based on types info for vars

  virtual void 	AggVar(MPI_Comm cm, MPI_Op op = MPI_OP_NULL);
  // #IGNORE aggregate variable across procs (allreduce: each sends and recvs all data using op to merge w/ existing vals) (MPI_OP_NULL = use pre-set agg_op

  virtual void 	ResetVar();	// #IGNORE reset variable info

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DMemAggVars& cp);
  TA_BASEFUNS_LITE(DMemAggVars);
};

#else

class TA_API DMemAggVars : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem collection of a variables of a different types (FLOAT, DOUBLE, INT) that *each proc has an instance of* (can be multiple members of a given object) -- these can all be Allreduced'ed to aggregate across procs (must all use same agg op -- use diff objs for diff ops if fixed)
INHERITED(taBase)
public:

  void	Dummy(const DMemAggVars&) { };
  TA_BASEFUNS_NOCOPY(DMemAggVars);
private:
  void 	Initialize() { };
  void 	Destroy()	{ };
};

#endif // DMEM_COMPILE

#endif // DMemAggVars_h
