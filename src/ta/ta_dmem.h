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


#ifndef ta_dmem_h
#define ta_dmem_h 1

#include "ta_base.h"

#ifdef DMEM_COMPILE

#include <mpi.h>

// add the following code into any object that is going to be shared across processors
// #ifdef DMEM_COMPILE
//   int 		dmem_local_proc; // #IGNORE processor on which these units are local
//   static int	dmem_this_proc;	// #IGNORE processor rank for this processor RELATIVE TO COMMUNICATOR for the network
//   virtual bool 	DMem_IsLocalProc(int proc)   	{ return dmem_local_proc == proc; } // #IGNORE
//   virtual bool 	DMem_IsLocal()       		{ return dmem_local_proc == dmem_proc; }  // #IGNORE
//   virtual int 	DMem_GetLocalProc() 		{ return dmem_local_proc; } // #IGNORE
//   virtual void 	DMem_SetLocalProc(int lproc) 	{ dmem_local_proc = lproc; } // #IGNORE
//   virtual void 	DMem_SetThisProc(int proc) 	{ dmem_this_proc = proc; } // #IGNORE
// #endif

#define DMEM_MPICALL(mpicmd, fun, mpi_call) \
  DMemShare::DebugCmd(fun, mpi_call); \
  DMemShare::ProcErr(mpicmd, fun, mpi_call)

// use the following to conditionalize running of functions directly as opposed
// to having them be called later via the cmdstream script calls
// it is necessary to do this to sychronize all dmem procs so they all call the
// exact same function scripts at exactly the same time!
#define DMEM_GUI_RUN_IF if(taMisc::dmem_nprocs == 1)

/////////////////////////////////////////////////////////////////
//	communicator: use this to define groups of communicating units

class TA_API DMemComm : public taNBase {
  // defines a communicator group for dmem communication
  INHERITED(taNBase)
public:
  MPI_Comm	comm;		// #IGNORE the mpi communicator id
  MPI_Group	group;		// #IGNORE the mpi group id
  int		nprocs;		// #IGNORE number of processors in comm
  int_Array	ranks;		// #IGNORE proc numbers of members of the group
  int		this_proc;	// #IGNORE the rank of this processor within communicator

  void	CommAll();
  // #IGNORE use all the processors (world group)
  void	CommSubGpInner(int sub_gp_size);
  // #IGNORE procs are organized into subgroups of given size, with nprocs / sub_gp_size such groups -- get the inner group for this processor (gp size = sub_gp_size)
  void	CommSubGpOuter(int sub_gp_size);
  // #IGNORE procs are organized into subgroups of given size, with nprocs / sub_gp_size such groups -- get the outer group for this processor

  int	GetThisProc();
  // #IGNORE get the rank of this processor relative to communicator

  void	MakeCommFmRanks();
  // #IGNORE make the comm from the ranks
  void	FreeComm();
  // #IGNORE free the comm & group 

  void 	Initialize();
  void 	Destroy();
  TA_BASEFUNS(DMemComm);
};

class TA_API DMemShareVar : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS definition of a variable of a given type (FLOAT, DOUBLE, INT) that each proc has some instances of (can be multiple members of a given object) -- these can all be Allgather'ed to sync across procs
public:
  MPI_Comm	comm;		// #IGNORE communicator associated with these shared objs
  int		mpi_type;	// #IGNORE mpi's type for this variable
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
  int_Array	long_send; 	// #IGNORE
  int_Array	long_recv; 	// #IGNORE

  virtual void	Compile_Var(MPI_Comm cm); // #IGNORE call this after updating the variable info
  virtual void 	SyncVar();	// #IGNORE synchronize variable across procs (allgather, so all procs have all data)
  virtual void 	AggVar(MPI_Op op); // #IGNORE aggregate variable across procs (allreduce: each sends and recvs all data using op to merge w/ existing vals)

  virtual void 	ResetVar();	// #IGNORE reset variable info

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DMemShareVar& cp);
  COPY_FUNS(DMemShareVar, taBase);
  TA_BASEFUNS(DMemShareVar);
};

class TA_API DMemShare : public taBase_List {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS collection of objects that are shared across distributed processes: link the objects into this list to share them
public:
  taBase_List	vars;		// #IGNORE list of DMemShareVar variables, one for each share_set (set of variables to be shared at the same time)
  // NOTE: a share_set must all have the same variable type (e.g., all must be FLOAT or DOUBLE, etc)!

  MPI_Comm	comm;		// #IGNORE communicator associated with these shared objs

#ifndef __MAKETA__
  static stringstream*	cmdstream;	// #IGNORE command stream: communicating commands across dmem procs
#endif

  static void 	InitCmdStream();	// #IGNORE initialize command stream
  static void 	CloseCmdStream(); 	// #IGNORE close command stream

  static void	DebugCmd(const char* fun, const char* mpi_call);
  // #IGNORE provide debugging trace at start of mpi command call
  static bool	ProcErr(int ercd, const char* fun, const char* mpi_call);
  // #IGNORE process any errors from command, and provide done message if debugging

  virtual void 	SetLocal_Sequential();	// #IGNORE set local processor on shared objects in sequence: 0 1 2..n 0 1 2..

  virtual void 	Compile_ShareVar(TypeDef* td, taBase* shr_item, MemberDef* par_md=NULL);
  // #IGNORE compile current set of objects and type info into set of types used in share/aggregate calls (MPI types)
  virtual void 	Compile_ShareTypes();   // #IGNORE compile current set of objects and type info into set of types used in share/aggregate calls (MPI types)

  virtual void 	DistributeItems(); // #IGNORE distribute the items across the nodes: calls above two functions

  virtual void 	Sync(int share_set);
  // #IGNORE synchronize across all processors for specific set of shared variables
  virtual void 	Aggregate(int share_set, MPI_Op op);
  // #IGNORE aggregate across all processors for specific set of shared variables: this only works for one floating point variable per object

  static void 	ExtractLocalFromList(taPtrList_impl& global_list, taPtrList_impl& local_list);
  // #IGNORE

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DMemShare& cp);
  COPY_FUNS(DMemShare, taBase_List);
  TA_BASEFUNS(DMemShare);
};

#else // DMEM_COMPILE

// dummy versions of these guys to keep the _TA files the same..

#define DMEM_GUI_RUN_IF
typedef int MPI_Comm;		// mpi communicator

class TA_API DMemComm : public taNBase {
  // defines a communicator group for dmem communication
  INHERITED(taNBase)
public:

  //  void	Dummy(const DMemComm&) { };
  void 	Initialize() { };
  void 	Destroy() { };
  TA_BASEFUNS(DMemComm);
};

class TA_API DMemShareVar : public taBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS definition of a variable of a given type (FLOAT, DOUBLE, INT) that each proc has some instances of (can be multiple members of a given object) -- these can all be Allgather'ed to sync across procs
public:

  void	Dummy(const DMemShareVar&) { };
  void 	Initialize() { };
  void 	Destroy()	{ };
  TA_BASEFUNS(DMemShareVar);
};

class TA_API DMemShare : public taBase_List {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS collection of objects that are shared across distributed processes: link the objects into this list to share them
public:
  static void 	ExtractLocalFromList(taPtrList_impl&, taPtrList_impl&) { };
  // #IGNORE

  void	Dummy(const DMemShare&) { };
  void 	Initialize() 	{ };
  void 	Destroy()	{ };
  TA_BASEFUNS(DMemShare);
};

#endif // DMEM_COMPILE

#endif // ta_dmem_h
