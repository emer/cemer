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

#include "BpUnitSpec.h"

#include <BpNetwork>
#include <BpConSpec>
#include <taProject>
#include <DataTable>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(BpUnitSpec);

TA_BASEFUNS_CTORS_DEFN(NLXX1ActSpec);
TA_BASEFUNS_CTORS_DEFN(GaussActSpec);


// const float BpUnitSpec::SIGMOID_MAX_VAL = 0.999999f;
// const float BpUnitSpec::SIGMOID_MIN_VAL = 0.000001f;
// const float BpUnitSpec::SIGMOID_MAX_NET = 13.81551f;

void BpUnitSpec::Initialize() {
  min_obj_type = &TA_BpUnitState_cpp;
  bias_spec.SetBaseType(&TA_BpConSpec);

  Initialize_core();

  Defaults_init();
  
  err_fun = NULL;               // obsolete
}


void BpUnitSpec::InitLinks() {
  bias_spec.type = &TA_BpConSpec;
  inherited::InitLinks();
  children.SetBaseType(&TA_BpUnitSpec);
  children.el_typ = GetTypeDef(); // but make the default to be me!
  taBase::Own(nlxx1, this);
  taBase::Own(gauss, this);
  taBase::Own(noise, this);
}

void BpUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(taMisc::is_loading && err_fun) {
    if(err_fun == Bp_Squared_Error)
      error_fun = SQUARED_ERR;
    else if(err_fun == Bp_CrossEnt_Error)
      error_fun = CROSS_ENTROPY;
    err_fun = NULL;
  }
  
  // update activation ranges from fun type
  switch(act_fun) {
  case SIGMOID:
  case NLXX1:
  case BINARY:
  case GAUSS:
  case RBF:
  case SOFTMAX:
    act_range.min = 0.0f; act_range.max = 1.0f;
    if(act_fun == SOFTMAX)
      err_tol = 0.0f;             // very bad with softmax
    break;
  case TANH:
    act_range.min = -1.0f; act_range.max = 1.0f;
    break;
  case RELU:
    act_range.min = 0.0f;
    if(act_range.max == 1.0f)
      act_range.max = 1.0e4f;
    break;
  default:
    break;
  }
  act_range.UpdateAfterEdit_NoGui();

  if(error_fun == CROSS_ENTROPY) {
    if(act_fun == LINEAR || act_fun == RELU) {
      taMisc::Error("rBpUnitSpec: Cross entropy error is incompatible with Linear or ReLu Units!  I switched to Squared_Error for you.");
      SetUnique("error_fun", true);
      error_fun = SQUARED_ERR;
    }
  }
}

void BpUnitSpec::GraphActFun(DataTable* graph_data, float min, float max) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ActFun", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* netin = graph_data->FindMakeColName("Netin", idx, VT_FLOAT);
  DataCol* act = graph_data->FindMakeColName("Act", idx, VT_FLOAT);
  DataCol* deriv = graph_data->FindMakeColName("Deriv", idx, VT_FLOAT);

  float x;
  for(x = min; x <= max; x += .01f) {
    float av = ActFromNetin(x, 0);
    float dv = ActDeriv(x, av, 0);
    graph_data->AddBlankRow();
    netin->SetValAsFloat(x, -1);
    act->SetValAsFloat(av, -1);
    deriv->SetValAsFloat(dv, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

// NOTE: OBSOLETE as of 8.0, 8/2016
void Bp_Squared_Error(BpUnitSpec* spec, BpUnitState_cpp* u) {
  float err = u->targ - u->act;
  if(fabsf(err) < spec->err_tol) {
    u->err = 0.0f;
  }
  else {
    u->dEdA += err;
    u->err = err * err;
  }
}

void Bp_CrossEnt_Error(BpUnitSpec* spec, BpUnitState_cpp* u) {
  float err = u->targ - u->act;
  if(fabsf(err) < spec->err_tol) {
    u->err = 0.0f;
  }
  else {
    err /= (u->act - spec->act_range.min) * (spec->act_range.max - u->act)
      * spec->act_range.scale;
    float a = spec->ClipSigAct(spec->act_range.Normalize(u->act));
    float t = spec->act_range.Normalize(u->targ);
    u->dEdA += err;
    u->err = (t * logf(a) + (1.0f - t) * logf(1.0f - a));
  }
}
