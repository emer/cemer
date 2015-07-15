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

#include "DMemAggVars.h"

TA_BASEFUNS_CTORS_LITE_DEFN(DMemAggVars);

#ifdef DMEM_COMPILE

#include <MemberDef>
#include <BuiltinTypeDefs>
#include <taMisc>

void DMemAggVars::Initialize() {
  comm = MPI_COMM_NULL;
  agg_op = MPI_OP_NULL;
}

void DMemAggVars::InitLinks() {
  taBase::InitLinks();
  taBase::Own(addrs, this);
//not tab  taBase::Own(types, this);
  taBase::Own(data_idx, this);
  taBase::Own(float_send, this);
  taBase::Own(float_recv, this);
  taBase::Own(double_send, this);
  taBase::Own(double_recv, this);
  taBase::Own(int_send, this);
  taBase::Own(int_recv, this);
}

void DMemAggVars::CutLinks() {
  ResetVar();
  taBase::CutLinks();
}

void DMemAggVars::Copy_(const DMemAggVars& cp) {
  // do we want to actually copy anything here?  ok.
  comm = cp.comm;
  agg_op = cp.agg_op;

  addrs = cp.addrs;
  types = cp.types;
}

void DMemAggVars::ResetVar() {
  comm = MPI_COMM_NULL;
  agg_op = MPI_OP_NULL;
  addrs.Reset();
  types.Reset();
  data_idx.Reset();
  float_send.Reset();
  float_recv.Reset();
  double_send.Reset();
  double_recv.Reset();
  int_send.Reset();
  int_recv.Reset();
}

String DMemAggVars::OpToStr(MPI_Op op) {
  String op_str;
  if (op == MPI_SUM)
    op_str = "SUM";
  else if (op == MPI_PROD)
    op_str = "PROD";
  else if (op == MPI_MAX)
    op_str = "MAX";
  else if (op == MPI_MIN)
    op_str = "MIN";
  //otherwise, nothing
  return op_str;
}

void DMemAggVars::ScanMembers(TypeDef* td, void* base) {
  addrs.Reset();
  types.Reset();
  ScanMembers_impl(td, base);
}

void DMemAggVars::ScanMembers_impl(TypeDef* td, void* base) {
  String trg_op_str = OpToStr(agg_op);
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members.FastEl(m);
    String opstr = md->OptionAfter("DMEM_AGG_");
    if(opstr.empty()) continue;
    if(!((trg_op_str.empty() && (opstr == "DYN")) || (opstr == trg_op_str))) continue;

    MPI_Datatype new_type = MPI_DATATYPE_NULL;
    if(md->type->IsActualClassNoEff()) {
      ScanMembers_impl(md->type, md->GetOff(base));
      continue;
    }
    else if(md->type->InheritsFrom(TA_double)) {
      new_type = MPI_DOUBLE;
    }
    else if(md->type->InheritsFrom(TA_float)) {
      new_type = MPI_FLOAT;
    }
    else if(md->type->InheritsFrom(TA_int)) {
      new_type = MPI_INT;
    }
    else if(md->type->IsEnum()) {
      new_type = MPI_INT;
    }
    else {
      taMisc::Error("WARNING: DMEM_AGG Specified for an unrecognized type.",
		    td->name, ", member:", md->name,
		    "unrecoginized types can not be shared.");
      continue;
    }
    void* addr = md->GetOff(base);
    types.Add(new_type);
    addrs.Add(addr);
  }
}

void DMemAggVars::CompileVars() {
  data_idx.Reset();
  float_send.Reset();
  float_recv.Reset();
  double_send.Reset();
  double_recv.Reset();
  int_send.Reset();
  int_recv.Reset();

  for(int i=0;i<types.size;i++) {
    MPI_Datatype mpi_type = types[i];
    if (mpi_type == MPI_FLOAT) {
      float_send.Add(0.0f);
      float_recv.Add(0.0f);
      data_idx.Add(float_send.size-1);
    }
    else if (mpi_type == MPI_DOUBLE) {
      double_send.Add(0.0f);
      double_recv.Add(0.0f);
      data_idx.Add(double_send.size-1);
    }
    else if (mpi_type == MPI_INT) {
      int_send.Add(0);
      int_recv.Add(0);
      data_idx.Add(int_send.size-1);
    }
  }
}

void DMemAggVars::AggVar(MPI_Comm cm, MPI_Op op) {
  if(op < 0)
    op = agg_op;
  if(op < 0) {
    taMisc::Error("ERROR: DMemAggVars::AggVar -- no default agg op!");
    return;
  }

  for(int i=0; i< types.size; i++)  {
    MPI_Datatype mpi_type = types[i];
    if (mpi_type == MPI_FLOAT) {
      float_send[data_idx[i]] = *((float*)addrs[i]);
    }
    else if (mpi_type == MPI_DOUBLE) {
      double_send[data_idx[i]] = *((double*)addrs[i]);
    }
    else if (mpi_type == MPI_INT) {
      int_send[data_idx[i]] = *((int*)addrs[i]);
    }
  }
  if(float_send.size > 0) {
    DMEM_MPICALL(MPI_Allreduce(float_send.el, float_recv.el, float_send.size, MPI_FLOAT, op, cm),
		 "DMemAggVars::AggVar", "Allreduce");
  }
  if(double_send.size > 0) {
    DMEM_MPICALL(MPI_Allreduce(double_send.el, double_recv.el, double_send.size, MPI_DOUBLE, op, cm),
		 "DMemAggVars::AggVar", "Allreduce");
  }
  if(int_send.size > 0) {
    DMEM_MPICALL(MPI_Allreduce(int_send.el, int_recv.el, int_send.size, MPI_INT, op, cm),
		 "DMemAggVars::AggVar", "Allreduce");
  }

  for(int i=0; i< types.size; i++)  {
    MPI_Datatype mpi_type = types[i];
    if (mpi_type == MPI_FLOAT) {
      *((float*)addrs[i]) = float_recv[data_idx[i]];
    }
    else if (mpi_type == MPI_DOUBLE) {
      *((double*)addrs[i]) = double_recv[data_idx[i]];
    }
    if (mpi_type == MPI_INT) {
      *((int*)addrs[i]) = int_recv[data_idx[i]];
    }
  }
}

#endif // DMEM_COMPILE
