// Copyright 2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "LeabraPrjn.h"
#include <LeabraNetwork>
#include <LeabraConSpec>

TA_BASEFUNS_CTORS_DEFN(LeabraPrjn);

void LeabraPrjn::Initialize() {
  Initialize_core();
  
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
  DMem_InitAggs();
#endif
}

void LeabraPrjn::Destroy() {
}

void LeabraPrjn::Trial_Init_Specs(LeabraNetwork* net) {
  if(con_spec.SPtr())
    ((LeabraConSpec*)con_spec.SPtr())->Trial_Init_Specs(net);
}

void LeabraPrjn::CheckInhibCons(LeabraNetwork* net) {
  LeabraLayer* fmlay = (LeabraLayer*)from.ptr();
  if(!fmlay || fmlay->lesioned()) return;
  if(con_spec.SPtr()) {
    if(((LeabraConSpec*)con_spec.SPtr())->inhib)
      net->net_misc.inhib_cons = true;
  }
}

#ifdef DMEM_COMPILE
void LeabraPrjn::DMem_InitAggs() {
  dmem_agg_sum.ScanMembers(GetTypeDef(), (void*)this);
  dmem_agg_sum.CompileVars();
}
void LeabraPrjn::DMem_ComputeAggs(MPI_Comm comm) {
  dmem_agg_sum.AggVar(comm, MPI_SUM);
}
#endif
