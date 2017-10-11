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

// bp.cc

#include "bp.h"
eTypeDef_Of(BpWizard);

#include <DumpFileCvt>
#include <DataTable>
#include <MemberDef>

#include <taMisc>

#include <math.h>
#include <limits.h>
#include <float.h>

#ifdef DMEM_COMPILE
#include <mpi.h>
#endif

#ifdef CUDA_COMPILE
#include "bp_cuda.h"
#endif




TA_BASEFUNS_CTORS_DEFN(HebbBpConSpec);
TA_BASEFUNS_CTORS_DEFN(BpLayer);

TA_BASEFUNS_CTORS_DEFN(ErrScaleBpConSpec);
TA_BASEFUNS_CTORS_DEFN(DeltaBarDeltaBpConSpec);
TA_BASEFUNS_CTORS_DEFN(BpContextSpec);


/////////////////////////////////////////////////////////////////////
//              Unit, Spec  


//////////////////////////////////////////
//      Additional Con Types            //
//////////////////////////////////////////

void DeltaBarDeltaBpConSpec::Initialize() {
  min_obj_type = &TA_DeltaBarDeltaBpCon;
  Defaults_init();
}

void DeltaBarDeltaBpConSpec::Defaults_init() {
  lrate_incr = .1f;
  lrate_decr = .9f;
  act_lrate_incr = lrate * lrate_incr;
}

void DeltaBarDeltaBpConSpec::UpdateAfterEdit_impl() {
  BpConSpec::UpdateAfterEdit_impl();
  act_lrate_incr = lrate * lrate_incr;
}


//////////////////////////////////////////
//      Additional Unit Types           //
//////////////////////////////////////////

//////////////////////////
//      Context         //
//////////////////////////

void BpContextSpec::Initialize() {
  hysteresis = .3f;
  hysteresis_c = .7f;
  initial_act.var = 0;
  initial_act.mean = .5;
  variable = "act";
  unit_flags = UnitState::NO_EXTERNAL;
}

void BpContextSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  hysteresis_c = 1.0f - hysteresis;
  var_md = TA_BpUnitState.members.FindName(variable);
  if(var_md == NULL)
    taMisc::Error("BpContextSpec: could not find variable:",variable,"in BpUnitStatep type");
}

void BpContextSpec::Init_Acts(UnitState* u, Network* net, int thr_no) {
  BpUnitSpec::Init_Acts(u, net, thr_no);
  u->act = initial_act.Gen(thr_no);
}

void BpContextSpec::Compute_Act(UnitState* u, Network* net, int thr_no) {
  // todo: add a checkconfig to ensure this congroup exists!
  ConState* rgp = net->ThrUnRecvConState(thr_no, u->thr_un_idx, 0); // first group
  UnitState* hu = (UnitState*)rgp->Un(0, net);
  float* varptr = (float*)var_md->GetOff((void*)u);
  *varptr = hysteresis_c * hu->act + hysteresis * (*varptr);
  u->SetExtFlag(unit_flags);
}


////////////////////////////////////////////////////////
//              CUDA  Code

#ifdef CUDA_COMPILE

bool BpNetwork::Cuda_MakeCudaNet() {
  if(cuda_net) return false;
  cuda_net = new Bp_cuda;  // note: this will leak memory..
  return true;
}

void BpNetwork::Cuda_CopyUnitSpec(void* cuda_us, const UnitSpec* src) {
  BpUnitSpec_cuda* cus = (BpUnitSpec_cuda*)cuda_us;
  BpUnitSpec* us = (BpUnitSpec*)src;
  cus->act_fun = (BpUnitSpec_cuda::BpActFun)us->act_fun;
  cus->error_fun = (BpUnitSpec_cuda::BpErrFun)us->error_fun;
  cus->err_tol = us->err_tol;
}

void BpNetwork::Cuda_CopyConSpec(void* cuda_cs, const ConSpec* src) {
  BpConSpec_cuda* ccs = (BpConSpec_cuda*)cuda_cs;
  BpConSpec* cs = (BpConSpec*)src;
  ccs->cur_lrate = cs->cur_lrate;
  ccs->momentum = cs->momentum;
  ccs->decay_type = (BpConSpec_cuda::DecayType)cs->decay_type;
  ccs->decay = cs->decay;
  ccs->wt_updt = (BpConSpec_cuda::WtUpdtType)cs->wt_updt;
}

void BpNetwork::Cuda_Trial_Run() {
  Cuda_Compute_NetinAct();
  Cuda_Compute_dEdA_dEdNet();
  Cuda_Compute_dWt();
  if(cuda.sync_units) {
    Cuda_UnitStateToHost();
  }
  else {
    cuda_net->TargUnitsToHost(true);       // send output layer data back to host for stats..
  }
}

void BpNetwork::Cuda_Compute_NetinAct() {
  ((Bp_cuda*)cuda_net)->Compute_NetinAct();
}

void BpNetwork::Cuda_Compute_dEdA_dEdNet() {
  ((Bp_cuda*)cuda_net)->Compute_dEdA_dEdNet();
}

void BpNetwork::Cuda_Compute_dWt() {
  if(cuda.timers_on)
    net_timing[0]->dwt.StartTimer(true); // reset

  ((Bp_cuda*)cuda_net)->Compute_dWt(true); // sync

  if(cuda.timers_on)
    net_timing[0]->dwt.EndIncrAvg();
}

void BpNetwork::Cuda_Compute_Weights() {
  ((Bp_cuda*)cuda_net)->Compute_Weights(true); // sync
  if(cuda.sync_cons) {
    Cuda_ConStateToHost();
    cuda_net->TargUnitsToHost(true);       // send output layer data back to host for stats..
    // important to send target units back to host so that their bias weights can update!!      
  }
}


#endif // CUDA_COMPILE

