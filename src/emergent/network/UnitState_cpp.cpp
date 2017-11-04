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

#include <SimpleMathSpec>
#include <taFiler>
#include <taProject>
#include <DataTable>
#include <NetMonitor>

#include <taMisc>
#include <tabMisc>

// all inline
#include "UnitState_cpp.h"

#include <LayerState_cpp>

#include <State_cpp>

#include "UnitState_core.cpp"


// todo: ifdef all of these -- ok for emergent but not for standalone
  
void UnitState_cpp::TransformWeights(NetworkState_cpp* nnet, const SimpleMathSpec& trans, PrjnState_cpp* prjn) {
  const int rsz = NRecvConGps(nnet);
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->TransformWeights(nnet, trans);
  }
}

void UnitState_cpp::RenormWeights(NetworkState_cpp* nnet, bool mult_norm, float avg_wt, PrjnState_cpp* prjn) {
  const int rsz = NRecvConGps(nnet);
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->RenormWeights(nnet, mult_norm, avg_wt);
  }
}

void UnitState_cpp::RescaleWeights(NetworkState_cpp* nnet, const float rescale_factor, PrjnState_cpp* prjn) {
  const int rsz = NRecvConGps(nnet);
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->RescaleWeights(nnet, rescale_factor);
  }
}

void UnitState_cpp::AddNoiseToWeights(NetworkState_cpp* nnet, const Random& noise_spec, PrjnState_cpp* prjn) {
  const int rsz = NRecvConGps(nnet);
  for(int g = 0; g < rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->AddNoiseToWeights(nnet, noise_spec);
  }
}

int UnitState_cpp::PruneCons(NetworkState_cpp* nnet, const SimpleMathSpec& pre_proc, Relation::Relations rel,
                       float cmp_val, PrjnState_cpp* prjn)
{
  int rval = 0;
  int g;
  const int rsz = NRecvConGps(nnet);
  for(g=0; g<rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    rval += cg->PruneCons(nnet, this, pre_proc, rel, cmp_val);
  }
  //  n_recv_cons -= rval;
  return rval;
}

int UnitState_cpp::LesionCons(NetworkState_cpp* nnet, float p_lesion, bool permute, PrjnState_cpp* prjn) {
  int rval = 0;
  const int rsz = NRecvConGps(nnet);
  for(int g=0; g<rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    rval += cg->LesionCons(nnet, this, p_lesion, permute);
  }
  //  n_recv_cons -= rval;
  return rval;
}

DataTable* UnitState_cpp::VarToTable(NetworkState_cpp* nnet, DataTable* dt, const String& variable) {
  bool new_table = false;
  // if (!dt) {
  //   taProject* proj = GetMyProj();
  //   dt = proj->GetNewAnalysisDataTable("Unit_Var_" + variable, true);
  //   new_table = true;
  // }
  NetMonitor nm;
  nm.OwnTempObj();
  nm.SetDataNetwork(dt, (Network*)nnet->net_owner);
  // nm.AddUnit(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* UnitState_cpp::ConVarsToTable(NetworkState_cpp* nnet, DataTable* dt, const String& var1, const String& var2,
                          const String& var3, const String& var4, const String& var5,
                          const String& var6, const String& var7, const String& var8,
                          const String& var9, const String& var10, const String& var11,
                          const String& var12, const String& var13, const String& var14,
                          PrjnState_cpp* prjn) {
  bool new_table = false;
  // if(!dt) {
  //   taProject* proj = GetMyProj();
  //   dt = proj->GetNewAnalysisDataTable("ConVars", true);
  //   new_table = true;
  // }
  dt->StructUpdate(true);
  const int rsz = NRecvConGps(nnet);
  for(int g=0; g<rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->ConVarsToTable(dt, this, nnet, var1, var2, var3, var4, var5, var6, var7, var8,
                       var9, var10, var11, var12, var13, var14);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

void  UnitState_cpp::MonitorVar(NetMonitor* net_mon, const String& variable) {
  // todo: impl
}

bool UnitState_cpp::Snapshot(NetworkState_cpp* nnet, const String& variable, SimpleMathSpec& math_op, bool arg_is_snap) {
  return false;
}
