// Copyright 2014-2018, Regents of the University of Colorado,
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
#include <Network>
#include <MemberDef>

#include <taMisc>
#include <tabMisc>

// all inline
#include "UnitState_cpp.h"

#include <LayerState_cpp>

#include <State_cpp>

#include "UnitState_core.cpp"


// todo: ifdef all of these -- ok for emergent but not for standalone
  
bool UnitState_cpp::SetUnValName(NetworkState_cpp* nnet, float val, const String& var_nm) {
  Network* nt = (Network*)nnet->net_owner;
  TypeDef* td = nt->UnitStateType();
  MemberDef* md = td->members.FindName(var_nm);
  if(!md) {
    taMisc::Error("SetUnValName", "variable named:", var_nm,
                  "not found in unit variables, of type:", td->name);
    return false;
  }
  if(!md->type->IsFloat()) {
    taMisc::Error("SetUnValName", "variable named:", var_nm,
                  "is not of float type -- must be -- is:", md->name);
    return false;
  }
  *((float*)md->GetOff(this)) = val;
  return true;
}

float UnitState_cpp::GetUnValName(NetworkState_cpp* nnet, const String& var_nm) const {
  Network* nt = (Network*)nnet->net_owner;
  TypeDef* td = nt->UnitStateType();
  MemberDef* md = td->members.FindName(var_nm);
  if(!md) {
    taMisc::Error("GetUnValName", "variable named:", var_nm,
                  "not found in unit variables, of type:", td->name);
    return 0.0f;
  }
  if(!md->type->IsFloat()) {
    taMisc::Error("GetUnValName", "variable named:", var_nm,
                  "is not of float type -- must be -- is:", md->name);
    return 0.0f;
  }
  return *((float*)md->GetOff(this));
}

bool UnitState_cpp::SetCnValName(NetworkState_cpp* nnet, float val, const Variant& prjn, int dx, const String& var_nm, bool recv) {
  ConState_cpp* cg = NULL;
  if(prjn.isStringType()) {
    String lay_nm = prjn.toString();
    if(lay_nm.startsWith("Fm_")) lay_nm = lay_nm.after('_');
    if(recv) {
      cg = FindRecvConStateFromName(nnet, lay_nm);
    }
    else {
      cg = FindSendConStateToName(nnet, lay_nm);
    }
  }
  else {
    int ridx =prjn.toInt();
    if(recv) {
      if(ridx < NRecvConGps(nnet)) {
        cg = RecvConState(nnet, ridx);
      }
    }
    else {
      if(ridx < NSendConGps(nnet)) {
        cg = SendConState(nnet, ridx);
      }
    }
  }
  if(!cg) {
    if(recv) {
      taMisc::Error("SetCnValName", "recv projection not found from:", prjn.toString());
    }
    else {
      taMisc::Error("SetCnValName", "send projection not found to:", prjn.toString());
    }
    return false;
  }
  return cg->SetCnValName(nnet, val, dx, var_nm);
}
  
float UnitState_cpp::GetCnValName(NetworkState_cpp* nnet, const Variant& prjn, int dx, const String& var_nm, bool recv) const {
  ConState_cpp* cg = NULL;
  if(prjn.isStringType()) {
    String lay_nm = prjn.toString();
    if(lay_nm.startsWith("Fm_")) lay_nm = lay_nm.after('_');
    if(recv) {
      cg = FindRecvConStateFromName(nnet, lay_nm);
    }
    else {
      cg = FindSendConStateToName(nnet, lay_nm);
    }
  }
  else {
    int ridx =prjn.toInt();
    if(recv) {
      if(ridx < NRecvConGps(nnet)) {
        cg = RecvConState(nnet, ridx);
      }
    }
    else {
      if(ridx < NSendConGps(nnet)) {
        cg = SendConState(nnet, ridx);
      }
    }
  }
  if(!cg) {
    if(recv) {
      taMisc::Error("GetCnValName", "recv projection not found from:", prjn.toString());
    }
    else {
      taMisc::Error("GetCnValName", "send projection not found to:", prjn.toString());
    }
    return false;
  }
  return cg->SafeCnName(nnet, dx, var_nm);
}

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

DataTable* UnitState_cpp::VarToTable(Network* nnet, DataTable* dt, const String& variable) {
  bool new_table = false;
  if (!dt) {
    taProject* proj = nnet->GetMyProj();
    dt = proj->GetNewAnalysisDataTable("Unit_Var_" + variable, true);
    new_table = true;
  }
  NetMonitor nm;
  nm.OwnTempObj();
  nm.SetDataNetwork(dt, nnet);
  Layer* lay = nnet->StateLayer(own_lay_idx);
  NetMonItem* it = nm.AddLayer(lay, "units[" + String(lay_un_idx) + "]." + variable);
  // it->name_style = NetMonItem::MY_NAME;
  // it->name = variable;          // makes this more predictable in terms of output
  it->max_name_len = 30;
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* UnitState_cpp::ConVarsToTable(Network* nnet, DataTable* dt, const String& var1, const String& var2,
                          const String& var3, const String& var4, const String& var5,
                          const String& var6, const String& var7, const String& var8,
                          const String& var9, const String& var10, const String& var11,
                          const String& var12, const String& var13, const String& var14,
                          PrjnState_cpp* prjn) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = nnet->GetMyProj();
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  const int rsz = NRecvConGps(nnet->net_state);
  for(int g=0; g<rsz; g++) {
    ConState_cpp* cg = RecvConState(nnet->net_state, g);
    PrjnState_cpp* cg_prjn = cg->GetPrjnState(nnet->net_state);
    if(cg->NotActive() || ((prjn) && (cg_prjn != prjn))) continue;
    cg->ConVarsToTable(dt, this, nnet, var1, var2, var3, var4, var5, var6, var7, var8,
                       var9, var10, var11, var12, var13, var14);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

bool UnitState_cpp::Snapshot(Network* nnet, const String& var, SimpleMathSpec& math_op, bool arg_is_snap) {
  NetworkState_cpp* snet = nnet->net_state;
  float val = 0.0f;
  if(var.startsWith("r.") || var.startsWith("s.")) {
    UnitState_cpp* src_u = nnet->GetViewSrcU();
    if(!src_u) return false;
    String cvar = var.after(".");
    bool is_send = var.startsWith("s.");
    if(is_send) {
      const int rsz = NRecvConGps(snet);
      for(int g=0;g<rsz;g++) {
        ConState_cpp* tcong = RecvConState(snet, g);
        if(tcong->NotActive()) continue;
        int con = tcong->FindConFromIdx(src_u->flat_idx);
        if(con < 0) continue;
        val = tcong->SafeCnName(snet, con, cvar);
        break;
      }
    }
    else {
      const int ssz = NSendConGps(snet);
      for(int g=0;g<ssz;g++) {
        ConState_cpp* tcong = SendConState(snet, g);
        if(tcong->NotActive()) continue;
        int con = tcong->FindConFromIdx(src_u->flat_idx);
        if(con < 0) continue;
        val = tcong->SafeCnName(snet, con, cvar);
        break;
      }
    }
  }
  else {
    val = GetUnValName(snet, var);
  }
  if(math_op.opr == SimpleMathSpec::NONE) {
    snap = val;
  }
  else {
    if(arg_is_snap)
      math_op.arg = snap;
    snap = (float)math_op.Evaluate((double)val);
  }
  return true;
}
