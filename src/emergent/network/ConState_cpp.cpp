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

#include <Network>
#include <Projection>
#include <NetMonitor>
#include <SimpleMathSpec>
#include <float_Array>
#include <int_Array>
#include <taFiler>
#include <MemberDef>
#include <taProject>
#include <DataTable>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include "ta_vector_ops.h"

#include "ConState_cpp.h"

#include "ConState_core.cpp"

// using namespace std;

//int   ConState_cpp::vec_chunk_targ = TA_VEC_SIZE;
// NOTE: this must be a constant for everyone because otherwise the weight files
// will not be portable across different platforms, builds, etc

TypeDef* ConState_cpp::ConType(Network* net) const {
  PrjnState_cpp* pjs = GetPrjnState(net->net_state);
  Projection* prjn = net->PrjnFromState(pjs);
  return prjn->con_type;
}


bool ConState_cpp::CopyCons(const ConState_cpp& cp) {
  if(NConVars() != cp.NConVars() || OwnCons() != cp.OwnCons()) return false;

  size = MIN(alloc_size, cp.size); // cannot go bigger than our alloc
  vec_chunked_size = 0;              // reset
  if(size == 0) return true;

  if(OwnCons()) {
    memcpy(MemBlock(0), (char*)cp.MemBlock(0), size * sizeof(float));
    int ncv = NConVars();
    for(int i=0; i < ncv; i++) {
      memcpy(CnMemBlock(0), (char*)cp.CnMemBlock(0), size * sizeof(float));
    }
  }
  else {
    for(int i=0; i< 2; i++) {
      memcpy(MemBlock(i), (char*)cp.MemBlock(i), size * sizeof(float));
    }
  }

  return true;
}

// int ConState_cpp::FindConFromNameIdx(const String& unit_nm, NetworkState_cpp* net) const {
//   for(int i=0; i<size; i++) {
//     UnitState_cpp* u = UnState(i, net);
//     if(u && (u->name == unit_nm)) return i;
//   }
//   return -1;
// }



/////////////////////////////////////////////////////////////
//      Weight ops

void ConState_cpp::TransformWeights(NetworkState_cpp* net, const SimpleMathSpec& trans) {
  if(!NotActive()) return;
  ConSpec_cpp* cs = GetConSpec(net);
  for(int i=0; i < size; i++) {
    float& wt = Cn(i, WT, net);
    wt = trans.Evaluate(wt);
    cs->C_ApplyLimits(wt);
  }
}

void ConState_cpp::RenormWeights(NetworkState_cpp* net, bool mult_norm, float avg_wt) {
  if(!NotActive()) return;
  ConSpec_cpp* cs = GetConSpec(net);
  cs->RenormWeights(this, net, 0, mult_norm, avg_wt); // doesn't use thr_no
}

void ConState_cpp::RescaleWeights(NetworkState_cpp* net, const float rescale_factor) {
  if(!NotActive()) return;
  ConSpec_cpp* cs = GetConSpec(net);
  for(int i=0; i < size; i++) {
    float& wt = Cn(i, WT, net);
    wt *= rescale_factor;
    cs->C_ApplyLimits(wt);
  }
}

void ConState_cpp::AddNoiseToWeights(NetworkState_cpp* net, const Random& noise_spec) {
  if(!NotActive()) return;
  ConSpec_cpp* cs = GetConSpec(net);
  for(int i=0; i < size; i++) {
    float& wt = Cn(i, WT, net);
    wt += noise_spec.Gen();
    cs->C_ApplyLimits(wt);
  }
}

int ConState_cpp::PruneCons(NetworkState_cpp* net, UnitState_cpp* un, const SimpleMathSpec& pre_proc,
                            Relation::Relations rel, float cmp_val)
{
  PrjnState_cpp* prjn = GetPrjnState(net);
  Relation cond;
  cond.rel = rel; cond.val = cmp_val;
  int rval = 0;
  for(int j=size-1; j>=0; j--) {
    if(cond.Evaluate(pre_proc.Evaluate(Cn(j, WT, net)))) {
      un->DisConnectFrom(net, UnState(j, net), prjn);
      rval++;
    }
  }
  return rval;
}

int ConState_cpp::LesionCons(NetworkState_cpp* net, UnitState_cpp* un, float p_lesion, bool permute) {
  PrjnState_cpp* prjn = GetPrjnState(net);
  int rval = 0;
  if(permute) {
    rval = (int) (p_lesion * (float)size);
    if(rval == 0) return 0;
    int_Array ary;
    int j;
    for(j=0; j<size; j++)
      ary.Add(j);
    ary.Permute();
    ary.size = rval;
    ary.Sort();
    for(j=ary.size-1; j>=0; j--) {
      un->DisConnectFrom(net, UnState(ary.FastEl(j), net), prjn);
    }
  }
  else {
    int j;
    for(j=size-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
        un->DisConnectFrom(net, UnState(j, net), prjn);
        rval++;
      }
    }
  }
  return rval;
}

/////////////////////////////////////////////////////////////
//      To/From Arrays/Matrix

bool ConState_cpp::ConValuesToArray(NetworkState_cpp* net, float_Array& ary, const String& variable) {
  CON_SPEC_CPP* cs = GetConSpec(net);
  int var_no = cs->FindConVar(this, variable);
  if(var_no < 0) {
    net->StateError("ConValuesToArray: variable named:", variable,
                    "not found in connection");
    return false;
  }
  if(!NotActive()) return false;
  for(int i=0; i<size; i++) {
    float val = Cn(i, var_no, net);
    ary.Add(val);
  }
  return true;
}

bool ConState_cpp::ConValuesToMatrix(NetworkState_cpp* net, float_Matrix& mat, const String& variable) {
  CON_SPEC_CPP* cs = GetConSpec(net);
  int var_no = cs->FindConVar(this, variable);
  if(var_no < 0) {
    net->StateError("ConValuesToArray: variable named:", variable,
                    "not found in connection");
    return false;
  }
  if(mat.size < size) {
    taMisc::Warning("ConValuesToMatrix: matrix size too small");
    return false;
  }

  if(!NotActive()) return false;
  for(int i=0; i<size; i++) {
    float val = Cn(i, var_no, net);
    mat.FastEl_Flat(i) = val;
  }
  return true;
}

bool ConState_cpp::ConValuesFromArray(NetworkState_cpp* net, float_Array& ary, const String& variable) {
  CON_SPEC_CPP* cs = GetConSpec(net);
  int var_no = cs->FindConVar(this, variable);
  if(var_no < 0) {
    net->StateError("ConValuesToArray: variable named:", variable,
                    "not found in connection");
    return false;
  }
  if(!NotActive()) return false;
  int mx = MIN(size, ary.size);
  for(int i=0; i<mx; i++) {
    float& val = Cn(i, var_no, net);
    val = ary[i];
  }
  return true;
}

bool ConState_cpp::ConValuesFromMatrix(NetworkState_cpp* net, float_Matrix& mat, const String& variable) {
  CON_SPEC_CPP* cs = GetConSpec(net);
  int var_no = cs->FindConVar(this, variable);
  if(var_no < 0) {
    net->StateError("ConValuesToArray: variable named:", variable,
                    "not found in connection");
    return false;
  }
  if(!NotActive()) return false;
  int mx = MIN(size, mat.size);
  for(int i=0; i<mx; i++) {
    float& val = Cn(i, var_no, net);
    val = mat.FastEl_Flat(i);
  }
  return true;
}

DataTable* ConState_cpp::ConVarsToTable(DataTable* dt, UnitState_cpp* ru, NetworkState_cpp* net,
                              const String& var1, const String& var2,
                              const String& var3, const String& var4, const String& var5,
                              const String& var6, const String& var7, const String& var8,
                              const String& var9, const String& var10, const String& var11,
                              const String& var12, const String& var13, const String& var14) {
  if(!ru) {
    taMisc::Error("ConVarsToTable: recv unit is NULL -- bailing");
    return NULL;
  }
  if(size <= 0) return NULL;            // nothing here

  bool new_table = false;
  // if (!dt) {
  //   taProject* proj = net->GetMyProj();
  //   dt = proj->GetNewAnalysisDataTable("ConVars", true);
  //   new_table = true;
  // }
  dt->StructUpdate(true);
  const int nvars = 14;
  DataCol* cols[nvars];
  const String vars[nvars] = {var1, var2, var3, var4, var5, var6, var7, var8,
                              var9, var10, var11, var12, var13, var14};
  MemberDef* mds[nvars];
  bool ruv[nvars];              // recv unit var
  bool suv[nvars];              // send unit var

  TypeDef* rutd = ((Network*)net->net_owner)->UnitStateType();
  TypeDef* sutd = sutd;
  TypeDef* con_type = ConType((Network*)net->net_owner);

  return NULL;

  int idx;
  for(int i=0;i<nvars;i++) {
    if(vars[i].nonempty()) {
      String colnm = taMisc::StringCVar(vars[i]);
      cols[i] = dt->FindMakeColName(colnm, idx, taBase::VT_FLOAT);
      ruv[i] = suv[i] = false;
      if(vars[i].startsWith("r.")) {
        ruv[i] = true;
        String varnxt = vars[i].after("r.");
        mds[i] = rutd->members.FindName(varnxt);
        if(!mds[i]) {
          taMisc::Warning("ConVarsToTable",
                          "recv unit variable named:", varnxt,
                          "not found in type:", rutd->name);
          continue;
        }
      }
      else if(vars[i].startsWith("s.")) {
        suv[i] = true;
        String varnxt = vars[i].after("s.");
        mds[i] = sutd->members.FindName(varnxt);
        if(!mds[i]) {
          taMisc::Warning("ConVarsToTable",
                          "send unit variable named:", varnxt,
                          "not found in type:", sutd->name);
          continue;
        }
      }
      else {
        mds[i] = con_type->members.FindName(vars[i]);
        if(!mds[i]) {
          taMisc::Warning("ConVarsToTable",
                          "connection variable named:", vars[i],
                          "not found in type:", con_type->name);
          continue;
        }
      }
    }
    else {
      cols[i] = NULL;
      mds[i] = NULL;
    }
  }
  for(int j=0;j<size;j++) {
    dt->AddBlankRow();
    for(int i=0;i<nvars;i++) {
      if(!mds[i]) continue;
      Variant val;
      if(ruv[i]) {
        val = mds[i]->GetValVar((void*)ru);
      }
      else if(suv[i]) {
        val = mds[i]->GetValVar((void*)UnState(j,net));
      }
      else {
        val = Cn(j, mds[i]->idx, net);
      }
      cols[i]->SetVal(val, -1);
    }
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

