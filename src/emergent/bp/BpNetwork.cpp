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

#include "BpNetwork.h"

#include <BpLayer>
#include <BpUnit>
#include <BpUnitSpec_cpp>
#include <BpUnitSpec>
#include <BpConSpec>
#include <BpCon>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(BpNetwork);

void BpNetwork::Initialize() {
  layers.SetBaseType(&TA_BpLayer);

  Initialize_net_core();
}

void BpNetwork::Init_Weights() {
  inherited::Init_Weights();
  prev_epoch = -1;
}
  
void BpNetwork::BuildNullUnit() {
  if(!null_unit) {
    taBase::OwnPointer((taBase**)&null_unit, new BpUnit, this);
  }
}

NetworkState_cpp* BpNetwork::NewNetworkState() const {
  return new BpNetworkState_cpp;
}

TypeDef* BpNetwork::NetworkStateType() const {
  return &TA_BpNetworkState_cpp;
}

TypeDef* BpNetwork::UnitStateType() const { return &TA_BpUnitState_cpp; }

TypeDef* BpNetwork::ConStateType() const { return &TA_ConState_cpp; }

void BpNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_BpCon;
  prjn->con_spec.SetBaseType(&TA_BpConSpec);
}

void BpNetwork::SetCurLrate_Thr(int thr_no) {
  const int nu = ThrNUnits(thr_no);
  for(int i=0; i<nu; i++) {
    BpUnitState_cpp* uv = (BpUnitState_cpp*)ThrUnitState(thr_no, i);
    if(uv->lesioned()) continue;
    BpUnitSpec_cpp* bus = (BpUnitSpec_cpp*)uv->GetUnitSpec(net_state);
    BpUnitSpec* bs = (BpUnitSpec*)UnitSpecFromState(bus);
    bs->SetCurLrate(uv, (BpNetworkState_cpp*)net_state, thr_no);
  }
}

void BpNetwork::Trial_Run() {
  if(prev_epoch != epoch) {
    lrate_updtd = false;
    NET_THREAD_CALL(BpNetwork::SetCurLrate_Thr);
    prev_epoch = epoch;
    if(lrate_updtd) {
      taMisc::Info("cur_lrate updated at epoch:", String(epoch));
      Cuda_UpdateSpecs();
    }
  }
#ifdef CUDA_COMPILE
  if(cuda.on) {
    DataUpdate(true);
    Cuda_Trial_Run();
    DataUpdate(false);
    return;
  }
#endif
  DataUpdate(true);
  NET_THREAD_CALL(BpNetwork::Trial_Run_Thr);
  DataUpdate(false);
}


