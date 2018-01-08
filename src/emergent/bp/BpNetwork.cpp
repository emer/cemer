// Copyright 2017-2018, Regents of the University of Colorado,
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
  
NetworkState_cpp* BpNetwork::NewNetworkState() const {
  return new BpNetworkState_cpp;
}

TypeDef* BpNetwork::NetworkStateType() const { return &TA_BpNetworkState_cpp; }
TypeDef* BpNetwork::UnitStateType() const { return &TA_BpUnitState_cpp; }
TypeDef* BpNetwork::ConStateType() const { return &TA_ConState_cpp; }


void BpNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_BpCon;
  prjn->con_spec.SetBaseType(&TA_BpConSpec);
}

void BpNetwork::SetCurLrate() {
  const int nu = n_con_specs_built;
  for(int i=0; i<nu; i++) {
    BpConSpec* cs = (BpConSpec*)StateConSpec(i);
    cs->SetCurLrate((BpNetworkState_cpp*)net_state);
  }
}

void BpNetwork::Trial_Run() {
  if(prev_epoch != epoch) {
    lrate_updtd = false;
    SetCurLrate();
    prev_epoch = epoch;
    if(lrate_updtd) {
      taMisc::Info("cur_lrate updated at epoch:", String(epoch));
      // Cuda_UpdateSpecs();
    }
  }
// #ifdef CUDA_COMPILE
//   if(cuda.on) {
//     DataUpdate(true);
//     Cuda_Trial_Run();
//     DataUpdate(false);
//     return;
//   }
// #endif
  DataUpdate(true);
  NET_STATE_RUN(BpNetworkState, Trial_Run());
  DataUpdate(false);
  MonitorData();
}


