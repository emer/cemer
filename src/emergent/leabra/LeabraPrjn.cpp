// Copyright, 1995-2013, Regents of the University of Colorado,
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

TA_BASEFUNS_CTORS_DEFN(LeabraPrjn);

void LeabraPrjn::Initialize() {
  netin_avg = 0.0f;
  netin_rel = 0.0f;

  avg_netin_avg = 0.0f;
  avg_netin_avg_sum = 0.0f;
  avg_netin_rel = 0.0f;
  avg_netin_rel_sum = 0.0f;
  avg_netin_n = 0;

  trg_netin_rel = -1.0f;                // indicates not set
#ifdef DMEM_COMPILE
  dmem_agg_sum.agg_op = MPI_SUM;
  DMem_InitAggs();
#endif
}

void LeabraPrjn::Destroy() {
}

void LeabraPrjn::Copy_(const LeabraPrjn& cp) {
  netin_avg = cp.netin_avg;
  netin_rel = cp.netin_rel;

  avg_netin_avg = cp.avg_netin_avg;
  avg_netin_avg_sum = cp.avg_netin_avg_sum;
  avg_netin_rel = cp.avg_netin_rel;
  avg_netin_rel_sum = cp.avg_netin_rel_sum;
  avg_netin_n = cp.avg_netin_n;

  trg_netin_rel = cp.trg_netin_rel;
}

void LeabraPrjn::SetCurLrate(LeabraNetwork* net, int epoch) {
  if(con_spec.SPtr())
    ((LeabraConSpec*)con_spec.SPtr())->SetCurLrate(net, epoch);
}

void LeabraPrjn::SetLearnRule(LeabraNetwork* net) {
  if(con_spec.SPtr())
    ((LeabraConSpec*)con_spec.SPtr())->SetLearnRule(net);
}

void LeabraPrjn::CheckInhibCons(LeabraNetwork* net) {
  LeabraLayer* fmlay = (LeabraLayer*)from.ptr();
  if(!fmlay || fmlay->lesioned()) return;
  if(con_spec.SPtr()) {
    if(((LeabraConSpec*)con_spec.SPtr())->inhib)
      net->inhib_cons_used = true;
  }
}

void LeabraPrjn::Init_Stats() {
  netin_avg = 0.0f;
  netin_rel = 0.0f;

  avg_netin_avg = 0.0f;
  avg_netin_avg_sum = 0.0f;
  avg_netin_rel = 0.0f;
  avg_netin_rel_sum = 0.0f;
  avg_netin_n = 0;
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
