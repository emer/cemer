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

#ifndef DMemShare_h
#define DMemShare_h 1

// parent includes:
#include <taBase_List>

// member includes:

// declare all other types mentioned but not required to include:

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

#ifndef __MAKETA__
class TA_API MPI_Datatype_PArray: public taPlainArray<MPI_Datatype> {
  // #NO_TOKENS only used in TypeDef dmem_type
public:
  void	operator=(const MPI_Datatype_PArray& cp) { Copy_Duplicate(cp); }
  MPI_Datatype_PArray()				{ };
  MPI_Datatype_PArray(const MPI_Datatype_PArray& cp) { Copy_Duplicate(cp); }

protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((MPI_Datatype*)a) == *((MPI_Datatype*)b)) rval=0; return rval; } // only groks ==
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((MPI_Datatype*)a) == *((MPI_Datatype*)b)); }
  String	El_GetStr_(const void* it) const { return _nilString; }
  void		El_SetFmStr_(void* it, const String& val)
  {  }
};
#endif


TypeDef_Of(DMemShare);

class TA_API DMemShare : public taBase_List {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem collection of objects that are shared across distributed processes: link the objects into this list to share them
  INHERITED(taBase_List)
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
  TA_BASEFUNS(DMemShare);
};

#else

// dummy versions of these guys to keep the _TA files the same..

#define DMEM_GUI_RUN_IF
typedef int MPI_Comm;		// mpi communicator

TypeDef_Of(DMemShare);

class TA_API DMemShare : public taBase_List {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem collection of objects that are shared across distributed processes: link the objects into this list to share them
INHERITED(taBase_List)
public:
  static void 	ExtractLocalFromList(taPtrList_impl&, taPtrList_impl&) { };
  // #IGNORE

  void	Dummy(const DMemShare&) { };
  TA_BASEFUNS_NOCOPY(DMemShare);
private:
  void 	Initialize() 	{ };
  void 	Destroy()	{ };
};

#endif // DMEM_COMPILE

#endif // DMemShare_h
