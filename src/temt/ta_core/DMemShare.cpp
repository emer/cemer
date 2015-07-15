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

#include "DMemShare.h"

TA_BASEFUNS_CTORS_DEFN(DMemShare);

#ifdef DMEM_COMPILE
#include <mpi.h>

#include <taMisc>
#include <taiMiscCore>
#include <MemberDef>
#include <BuiltinTypeDefs>
#include <DMemShareVar>

#include <sstream>

using namespace std;

static String dmem_mpi_decode_err(int ercd) {
  char errstr[MPI_MAX_ERROR_STRING];
  int errlen;
  MPI_Error_string(ercd, errstr, &errlen);
  return String(ercd) + " msg: " + String(errstr);
}

void DMemShare::DebugCmd(const char* function, const char* mpi_call) {
  if(taMisc::dmem_debug) {
    String msg;
    msg << "proc: " << taMisc::dmem_proc << " fun: "
	<< function << " MPI_" << mpi_call
	<< " start...";
    taMisc::Info(msg);
  }
}

bool DMemShare::ProcErr(int ercd, const char* function, const char* mpi_call) {
  if(ercd == MPI_SUCCESS) {
    if(taMisc::dmem_debug) {
      String msg;
      msg << "proc: " << taMisc::dmem_proc << " fun: "
	  << function << " MPI_" << mpi_call
	  << " SUCCESS!";
      taMisc::Info(msg);
    }
    return true;
  }
  String msg;
  msg << "proc: " << taMisc::dmem_proc << " fun: "
      << function << " MPI_" << mpi_call
      << " FAILED with code: " << dmem_mpi_decode_err(ercd)
      << " Now Quitting!";
  taMisc::Error(msg);
  taiMiscCore::Quit();		// bail on error!
  return false;
}

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

    MPI_Datatype new_type = MPI_DATATYPE_NULL;
    if(md->type->IsAnyPtr()) {
      taMisc::Error("WARNING: DMEM_SHARE_SET Specified for a pointer in type:",
		    td->name, ", member:", md->name,
		    "Pointers can not be shared.");
      continue;
    }
    if(md->type->IsActualClassNoEff()) {
      if(par_md != NULL) {
	taMisc::Error("WARNING: DMEM_SHARE_SET in too many nested objects: only one level of subobject nesting allowed!  type:",
		      td->name, ", member:", md->name);
	continue;
      }
      Compile_ShareVar(md->type, shr_item, md);
      continue;
    }
    else if (md->type->InheritsFrom(TA_double)) {
      new_type = MPI_DOUBLE;
    }
    else if (md->type->InheritsFrom(TA_float)) {
      new_type = MPI_FLOAT;
    }
    else if (md->type->InheritsFrom(TA_int)) {
      new_type = MPI_INT;
    }
    else if (md->type->IsEnum()) {
      new_type = MPI_INT;
    }
    else {
      taMisc::Error("WARNING: DMEM_SHARE_SET Specified for an unrecognized type.",
		    td->name, ", member:", md->name,
		    "unrecoginized types can not be shared.");
      continue;
    }
    if(var->mpi_type == MPI_DATATYPE_NULL)
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

  vars.SetSize(max_share_sets);
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
