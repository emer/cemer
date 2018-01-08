// Copyright 2017-22018 Regents of the University of Colorado,
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

#include "LeabraExtraUnitSpecs.h"

#include <LeabraNetwork>
#include <taProject>
#include <taMisc>

#include <LeabraConSpec_cpp>
#include <LeabraUnitSpec_cpp>
#include <LeabraLayerSpec_cpp>

#include <LeabraExtraUnitSpecs_cpp> // cpp versions too

#include <State_main>


#include "LayerActUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(LayerActUnitSpec);



#include "DeepCopyUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(DeepCopyUnitSpec);

void DeepCopyUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void DeepCopyUnitSpec::Defaults_init() {
}

bool DeepCopyUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Unit(lay, quiet);

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);
  
  LEABRA_CON_STATE* cg = un->RecvConStateSafe(net, 0);
  if(lay->CheckError(!cg, quiet, rval,
                     "Requires one recv projection!")) {
    return false;
  }
  LEABRA_UNIT_STATE* su = cg->UnStateSafe(0, net);
  if(lay->CheckError(!su, quiet, rval, 
                     "Requires one unit in recv projection!")) {
    return false;
  }

  return rval;
}


#include "ThalSendUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(ThalSendUnitSpec);

void ThalSendUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void ThalSendUnitSpec::Defaults_init() {
  deep.role = DeepSpec::TRC;
}


#include "PoolInputsUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(PoolInputsUnitSpec);

void PoolInputsUnitSpec::Initialize() {
  pool_fun = MAX_POOL;
  Defaults_init();
}

void PoolInputsUnitSpec::Defaults_init() {
}

bool PoolInputsUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Unit(lay, quiet);

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);
  
  LEABRA_CON_STATE* cg = un->RecvConStateSafe(net, 0);
  if(lay->CheckError(!cg, quiet, rval,
                   "Requires one recv projection!")) {
    return false;
  }
  LEABRA_LAYER_STATE* fmlay = cg->GetSendLayer(net);
  if(lay->CheckError(fmlay->lesioned(), quiet, rval,
                   "Sending layer is lesioned -- we should be lesioned too!")) {
    return false;
  }

  return rval;
}


////////////////////////////////////////////////////////////////////
//              TD

#include "TDRewPredUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(TDRewPredUnitSpec);

void TDRewPredUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void TDRewPredUnitSpec::Defaults_init() {
  SetUnique("act_range", true);
  act_range.min = -100.0f;
  act_range.max = 100.0f;
}

void TDRewPredUnitSpec::HelpConfig() {
  String help = "TDRewPredUnitSpec Computation:\n\
 Computes expected rewards according to the TD algorithm: predicts V(t+1) at time t. \n\
 - Minus phase = previous expected reward V^(t) clamped\
 - Plus phase = free-running expected reward computed (over settlng, fm recv wts)\n\
 - Learning is da_p * act_q0: dopamine from TDDeltaUnitSpec times sender activations at (t-1).\n\
 \nTDRewPredUnitSpec Configuration:\n\
 - Sending connection to a TDRewIntegUnitSpec to integrate predictions with external rewards\n\
 - Recv connection from TDDeltaUnitSpec to receive da_p TD training signal\n\
 - UnitSpec for this layer must have act_range set to -100 and 100 \
     (because negative td = negative activation signal here)";
  taMisc::Confirm(help);
}

bool TDRewPredUnitSpec::CheckConfig_Unit(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Unit(lay, quiet))
    return false;

  bool rval = true;

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = lay->GetUnitState(net, 0);
  
  const int nrg = un->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = un->RecvConState(net, g);
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    if(cs->IsMarkerCon()) continue;
    if(lay->CheckError
       (cs->GetStateSpecType() != LEABRA_NETWORK_STATE::T_TDRewPredConSpec, quiet, rval,
                  "requires recv connections to be of type TDRewPredConSpec")) {
      return false;
    }
  }
  return true;
}


#include "TDRewIntegUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(TDRewIntegSpec);
TA_BASEFUNS_CTORS_DEFN(TDRewIntegUnitSpec);

void TDRewIntegUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void TDRewIntegUnitSpec::Defaults_init() {
  SetUnique("act_range", true);
  act_range.min = -100.0f;
  act_range.max = 100.0f;
}

void TDRewIntegUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew_integ.UpdateAfterEdit_NoGui();
}

void TDRewIntegUnitSpec::HelpConfig() {
  String help = "TDRewIntegUnitSpec Computation:\n\
 Integrates reward predictions from TDRewPred layer, and external actual rewards from\
 ExtRew layer.  Plus-minus phase difference is td value.\n\
 - Minus phase = previous expected reward V^(t) copied directly from TDRewPred\n\
 - Plus phase = integration of ExtRew r(t) and new TDRewPred computing V^(t+1)).\n\
 - No learning.\n\
 \nTDRewIntegUnitSpec Configuration:\n\
 - Requires 2 input projections, from TDRewPred, ExtRew layers.\n\
 - Sending connection to TDDeltaUnitSpec(s) (marked with MarkerConSpec)\n\
 (to compute the td change in expected rewards as computed by this layer)\n\
 - UnitSpec for this layer must have act_range set to -100 and 100 \
     (because negative td = negative activation signal here)";
  taMisc::Confirm(help);
}

bool TDRewIntegUnitSpec::CheckConfig_Unit(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Unit(lay, quiet))
    return false;

  bool rval = true;

  // check for conspecs with correct params
  LEABRA_LAYER_STATE* rew_pred_lay = NULL;
  LEABRA_LAYER_STATE* ext_rew_lay = NULL;

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = lay->GetUnitState(net, 0);
  
  const int nrg = un->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = un->RecvConState(net, g);
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* fmlay = recv_gp->GetSendLayer(net);
    LEABRA_LAYER_SPEC_CPP* fls = fmlay->GetLayerSpec(net);
    LEABRA_UNIT_SPEC_CPP* us = fmlay->GetUnitSpec(net);
    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_TDRewPredUnitSpec) {
      rew_pred_lay = fmlay;
    }
    else if(fls->GetStateSpecType() == LEABRA_NETWORK_STATE::T_ExtRewLayerSpec) {
      ext_rew_lay = fmlay;
    }
  }

  if(lay->CheckError(rew_pred_lay == NULL, quiet, rval,
                "requires recv projection from layer with TDRewPredUnitSpec units!")) {
    return false;
  }

  // not required..
  // int myidx = lay->own_net->layers.FindLeafEl(lay);
  // int rpidx = lay->own_net->layers.FindLeafEl(rew_pred_lay);
  // if(lay->CheckError(rpidx > myidx, quiet, rval,
  //               "reward prediction layer must be *before* this layer in list of layers -- it is now after, won't work")) {
  //   return false;
  // }

  if(lay->CheckError(ext_rew_lay == NULL, quiet, rval,
                "TD requires recv projection from layer with ExtRewLayerSpec!")) {
    return false;
  }
  // int eridx = lay->own_net->layers.FindLeafEl(ext_rew_lay);
  // if(lay->CheckError(eridx > myidx, quiet, rval,
  //               "external reward layer must be *before* this layer in list of layers -- it is now after, won't work")) {
  //   return false;
  // }
  return true;
}


#include "TDDeltaUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(TDDeltaUnitSpec);

void TDDeltaUnitSpec::Initialize() {
  Defaults_init();
}

void TDDeltaUnitSpec::Defaults_init() {
  SetUnique("act_range", true);
  act_range.min = -100.0f;
  act_range.max = 100.0f;
}

void TDDeltaUnitSpec::HelpConfig() {
  String help = "TDDeltaUnitSpec Computation:\n\
 - act of unit(s) = act_dif of unit(s) in reward integration layer we recv from\n\
 - td is dynamically computed in plus phaes and sent all layers that recv from us\n\
 - No Learning\n\
 \nTDDeltaUnitSpec Configuration:\n\
 - Single recv connection marked with a MarkerConSpec from reward integration layer\
     (computes expectations and actual reward signals)\n\
 - This layer must be after corresp. reward integration layer in list of layers\n\
 - Sending connections must connect to units of type LeabraTdUnit/Spec \
     (td signal from this layer put directly into td var on units)\n\
 - UnitSpec for this layer must have act_range set to -100 and 100 \
     (because negative td = negative activation signal here)";
  taMisc::Confirm(help);
}

bool TDDeltaUnitSpec::CheckConfig_Unit(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = lay->GetUnitState(net, 0);

  LEABRA_LAYER_STATE* rewinteg_lay = NULL;
  
  const int nrg = un->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = un->RecvConState(net, g);
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* fmlay = recv_gp->GetSendLayer(net);
    LEABRA_UNIT_SPEC_CPP* us = fmlay->GetUnitSpec(net);
    if(us->GetStateSpecType() == LEABRA_NETWORK_STATE::T_TDRewIntegUnitSpec) {
      rewinteg_lay = fmlay;
    }
  }

  if(lay->CheckError(rewinteg_lay == NULL, quiet, rval,
                "did not find layer with TDRewIntegUnitSpec units to get TD from!")) {
    return false;
  }

  // int myidx = lay->own_net->layers.FindLeafEl(lay);
  // int rpidx = lay->own_net->layers.FindLeafEl(rewinteg_lay);
  // if(lay->CheckError(rpidx > myidx, quiet, rval,
  //               "reward integration layer must be *before* this layer in list of layers -- it is now after, won't work")) {
  //   return false;
  // }
  return true;
}


////////////////////////////////////////////////////////////////////
//              PVLV -- Amyg


#include "D1D2UnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(D1D2UnitSpec);

void D1D2UnitSpec::Initialize() {
  Defaults_init();
}

void D1D2UnitSpec::Defaults_init() {
  deep.mod_thr = 0.1f;         // default is .1
}


#include "BasAmygUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(BasAmygUnitSpec);

void BasAmygUnitSpec::Initialize() {
  acq_ext = ACQ;
  valence = APPETITIVE;
  dar = D1R;
  Defaults_init();
  Initialize_core();
}

void BasAmygUnitSpec::Defaults_init() {
  SetUnique("deep", true);
  deep.on = true;
  deep.role = DeepSpec::DEEP;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f;
  deep.mod_thr = 0.01f;         // default is .1
}

void BasAmygUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(acq_ext == ACQ) {
    if(valence == APPETITIVE) {
      dar = D1R;
    }
    else {     // AVERSIVE
      dar = D2R;
    }
  }
  else {      // EXT
    if(valence == APPETITIVE) {
      dar = D2R;                // reversed!
    }
    else {     // AVERSIVE
      dar = D1R;
    }
  }
}


#include "BLAmygUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(BLAmygDaMod);
TA_BASEFUNS_CTORS_DEFN(BLAmygAChMod);
TA_BASEFUNS_CTORS_DEFN(BLAmygUnitSpec);

void BLAmygUnitSpec::Initialize() {
  dar = D1R;
  Defaults_init();
  Initialize_core();
}

void BLAmygUnitSpec::Defaults_init() {
  SetUnique("deep", true);
  deep.on = true;
  deep.role = DeepSpec::DEEP;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f;
  deep.mod_thr = 0.1f;          // note: was .01 in hard code
}


#include "CElAmygUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(CElAmygDaMod);
TA_BASEFUNS_CTORS_DEFN(CElAmygUnitSpec);

void CElAmygUnitSpec::Initialize() {
  acq_ext = ACQ;
  valence = APPETITIVE;
  dar = D1R;
  Defaults_init();
  Initialize_core();
}

void CElAmygUnitSpec::Defaults_init() {
  SetUnique("deep", true);
  deep.on = true;
  deep.role = DeepSpec::DEEP;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f;
  deep.mod_thr = 0.01f;         // default .1

  // these are params to produce low-level baseline tonic activation
  SetUnique("init", true);
  init.v_m = 0.55f;
  SetUnique("g_bar", true);
  g_bar.l = 0.6f;               // todo: maybe higher?
  SetUnique("e_rev", true);
  e_rev.l = 0.55f;
  e_rev.i = 0.4f;
}

void CElAmygUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(acq_ext == ACQ) {
    if(valence == APPETITIVE) {
      dar = D1R;
    }
    else {     // AVERSIVE
      dar = D2R;
    }
  }
  else {      // EXT
    if(valence == APPETITIVE) {
      dar = D2R;                // reversed!
    }
    else {     // AVERSIVE
      dar = D1R;
    }
  }
}


////////////////////////////////////////////////////////////////////
//              PVLV -- DA

#include "PPTgUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(PPTgUnitSpec);

void PPTgUnitSpec::Initialize() {
  d_net_gain = 1.0f;
  clamp_act = true;
  act_thr = 0.0f;
  Initialize_core();
}

void PPTgUnitSpec::Defaults_init() {
}


#include "LHbRMTgUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(LHbRMTgSpecs);
TA_BASEFUNS_CTORS_DEFN(LHbRMTgGains);
TA_BASEFUNS_CTORS_DEFN(LHbRMTgUnitSpec);

void LHbRMTgUnitSpec::Initialize() {
  SetUnique("deep_raw_qtr", true);
  deep_raw_qtr = Q4;
  SetUnique("act_range", true);
  act_range.max = 2.0f;
  act_range.min = -2.0f;
  act_range.UpdateAfterEdit();
  SetUnique("clamp_range", true);
  clamp_range.max = 2.0f;
  clamp_range.min = -2.0f;
  clamp_range.UpdateAfterEdit();
  Initialize_core();
}

void LHbRMTgUnitSpec::Defaults_init() {
}

void LHbRMTgUnitSpec::HelpConfig() {
  String help = "LHbRMTgUnitSpec (DA value) Computation:\n\
 - Computes DA dips and related phenomena based on inputs from VS Matrix, Patch Direct and Indirect, and PV Pos & Neg.\n\
 \nLHbRMTgUnitSpec Configuration:\n\
 - Use the Wizard gdPVLV button to automatically configure layers.\n\
 - Recv cons should be standard, with no learning";
  taMisc::Confirm(help);
}

bool LHbRMTgUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;
  
  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  main_net->SetNetFlag(Network::NETIN_PER_PRJN); // this is required for this computation!
  net->SetNetFlag(NETWORK_STATE::NETIN_PER_PRJN); // this is required for this computation!
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);

  LEABRA_LAYER_STATE* pv_pos_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_pos_D1_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_pos_D2_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_pos_D1_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_pos_D2_lay = NULL;
  
  LEABRA_LAYER_STATE* pv_neg_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_neg_D1_lay = NULL;
  LEABRA_LAYER_STATE* vspatch_neg_D2_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_neg_D1_lay = NULL;
  LEABRA_LAYER_STATE* vsmatrix_neg_D2_lay = NULL;
  
  GetRecvLayers(un, net, pv_pos_lay, vspatch_pos_D1_lay, vspatch_pos_D2_lay, vsmatrix_pos_D1_lay,
                vsmatrix_pos_D2_lay, pv_neg_lay, vspatch_neg_D1_lay, vspatch_neg_D2_lay,
                vsmatrix_neg_D1_lay, vsmatrix_neg_D2_lay);

  if(lay->CheckError(!vspatch_pos_D1_lay, quiet, rval,
                   "did not find VS Patch D1R recv projection -- searches for MSNUnitSpec::PATCH, D1R")) {
    rval = false;
  }
  if(lay->CheckError(!vspatch_pos_D2_lay, quiet, rval,
                   "did not find VS Patch D2R recv projection -- searches for MSNUnitSpec::PATCH, D2R")) {
    rval = false;
  }
  // TODO: all the AVERSIVE guys optional?
  
  // matrix is optional
  // if(lay->CheckError(!matrix_dir_lay, quiet, rval,
  //                  "did not find VS Matrix Direct recv projection -- searches for Matrix and *not* Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  // if(lay->CheckError(!matrix_ind_lay, quiet, rval,
  //                  "did not find VS Matrix Indirect recv projection -- searches for Matrix and Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  
  if(lay->CheckError(!pv_pos_lay, quiet, rval,
                   "did not find PV Positive recv projection -- searches for PosPV in layer name")) {
    rval = false;
  }
  if(lay->CheckError(!pv_neg_lay, quiet, rval,
                   "did not find PV Negative recv projection -- searches for NegPV in layer name")) {
    rval = false;
  }

  return rval;
}


#include "VTAUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(PVLVDaSpec);
TA_BASEFUNS_CTORS_DEFN(PVLVDaGains);
TA_BASEFUNS_CTORS_DEFN(LVBlockSpec);
TA_BASEFUNS_CTORS_DEFN(VTAUnitSpec);

void VTAUnitSpec::Initialize() {
  da_val = DA_P;
  SetUnique("deep_raw_qtr", true);
  deep_raw_qtr = Q4;
  SetUnique("act_range", true);
  act_range.max = 2.0f;
  act_range.min = -2.0f;
  act_range.UpdateAfterEdit();
  SetUnique("clamp_range", true);
  clamp_range.max = 2.0f;
  clamp_range.min = -2.0f;
  clamp_range.UpdateAfterEdit();
  Initialize_core();
}

void VTAUnitSpec::HelpConfig() {
  String help = "VTAUnitSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from PVLV layers: PPTg (bursts) and LHbRMTg (dips).\n\
 - No Learning\n\
 \nVTAUnitSpec Configuration:\n\
 - Use the Wizard PVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -2.0 and 2.0 \
     (because negative da = negative activation signal here";
  taMisc::Confirm(help);
}

bool VTAUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;
  bool rval = true;

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);

  if(lay->CheckError((act_range.max != 2.0f) || (act_range.min != -2.0f), quiet, rval,
                "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
                name,"(make sure this is appropriate for all layers that use this spec!)")) {
    SetUnique("act_range", true);
    act_range.max = 2.0f;
    act_range.min = -2.0f;
    act_range.UpdateAfterEdit();
  }
  if(lay->CheckError((clamp_range.max != 2.0f) || (clamp_range.min != -2.0f), quiet, rval,
                "requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
                name,"(make sure this is appropriate for all layers that use this spec!)")) {
    SetUnique("clamp_range", true);
    clamp_range.max = 2.0f;
    clamp_range.min = -2.0f;
    clamp_range.UpdateAfterEdit();
  }

  // check recv connection
  //GetRecvLayers(u, pptg_lay, lhb_lay, pospv_lay, vspatchposd1_lay, negpv_lay);
  
  if(da_val == DA_P) {
    LEABRA_LAYER_STATE* pptg_lay_p = NULL;
    LEABRA_LAYER_STATE* lhb_lay = NULL;
    LEABRA_LAYER_STATE* pospv_lay = NULL;
    LEABRA_LAYER_STATE* vspatchposd1_lay = NULL;
    LEABRA_LAYER_STATE* vspatchposd2_lay = NULL;
    LEABRA_LAYER_STATE* vspatchnegd1_lay = NULL;
    LEABRA_LAYER_STATE* vspatchnegd2_lay = NULL;
  
    GetRecvLayers_P(un, net, pospv_lay, pptg_lay_p, lhb_lay, vspatchposd1_lay,
                    vspatchposd2_lay, vspatchnegd1_lay, vspatchnegd2_lay);
  
    if(lay->CheckError(!pptg_lay_p, quiet, rval,
                     "did not find PPTg layer to get DA bursts from (looks for PPTgUnitSpec)")) {
      rval = false;
    }
    if(lay->CheckError(!lhb_lay, quiet, rval,
                     "did not find LHbRMTg layer to get DA dips from (looks for LHbRMTgUnitSpec)")) {
      rval = false;
    }
    if(lay->CheckError(!pospv_lay, quiet, rval,
                     "did not find PosPV layer to get positive PV from -- looks for PV and Pos in layer name")) {
      rval = false;
    }
    if(lay->CheckError(!vspatchposd1_lay, quiet, rval,
                     "did not find VS Patch Direct layer to get pos PV shunting (cancelling) signal from (looks for layer with MSNUnitSpec APPETITIVE D1R)")) {
      rval = false;
    }
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      if(lay->CheckError(!vspatchposd2_lay, quiet, rval,
                       "did not find VSPatchPosD2 layer to subtract from direct VSPatchPosD1 PV shunting (cancelling) signal from (looks for layer with MSNUnitSpec APPETITIVE D2R)")) {
        rval = false;
      }
    }
    if(gains.pvi_dip_shunt_gain > 0.0f) {
      if(lay->CheckError(!vspatchnegd2_lay, quiet, rval,
                       "did not find VSPatchNegD2 layer to subtract from direct VSPatchPosD1 PV shunting (cancelling) signal from (looks for layer with MSNUnitSpec AVERSIVE D2R)")) {
        rval = false;
      }
    }
    if(gains.pvi_anti_dip_shunt_gain > 0.0f) {
      if(lay->CheckError(!vspatchnegd1_lay, quiet, rval,
                       "did not find VSPatchNegD1 layer to subtract from direct VSPatchPosD1 PV shunting (cancelling) signal from (looks for layer with MSNUnitSpec AVERSIVE D1R)")) {
        rval = false;
      }
    }
  }
  else {
    // TODO: add more recv layer checks as we determine which layers VTAn should receive from - only sure of NegPV currently
    LEABRA_LAYER_STATE* negpv_lay = NULL;
    LEABRA_LAYER_STATE* pptg_lay_n = NULL;
    LEABRA_LAYER_STATE* lhb_lay_n = NULL;
    LEABRA_LAYER_STATE* vspatchnegd1_lay = NULL;
    LEABRA_LAYER_STATE* vspatchnegd2_lay = NULL;
    
    GetRecvLayers_N(un, net, negpv_lay, pptg_lay_n, lhb_lay_n, vspatchnegd1_lay, vspatchnegd2_lay);

    if(lay->CheckError(!negpv_lay, quiet, rval,
                     "did not find NegPV layer to get negative PV from -- looks for PV and Neg in layer name")) {
      rval = false;
    }
    if(lay->CheckError(!pptg_lay_n, quiet, rval,
                     "did not find PPTg_n layer to get DA bursts from (looks for PPTgUnitSpec)")) {
      rval = false;
    }
    if(lay->CheckError(!lhb_lay_n, quiet, rval,
                     "did not find LHbRMTg layer projection (looks for LHbRMTgUnitSpec)")) {
      rval = false;
    }
    if(lay->CheckError(!vspatchnegd2_lay, quiet, rval,
                     "did not find VSPatch D2 layer projection (looks for MSNUnitSpec AVERSIVE D2R")) {
      rval = false;
    }
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      if(lay->CheckError(!vspatchnegd1_lay, quiet, rval,
                         "did not find VSPatch D1 layer projection (looks for MSNUnitSpec AVERSIVE D1R)")) {
        rval = false;
      }
    }
  }
  return rval;
}


#include "DRNUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(DRN5htSpec);
TA_BASEFUNS_CTORS_DEFN(DRNUnitSpec);

void DRNUnitSpec::Initialize() {
  Initialize_core();
}

void DRNUnitSpec::HelpConfig() {
  String help = "DRNUnitSpec (5HT serotonin value) Computation:\n\
 - Computes SE value based on inputs from PV and State layers.\n\
 - No Learning\n\
 \nDRNUnitSpec Configuration:\n\
 - Use the Wizard PVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs";
  taMisc::Confirm(help);
}


#include "ClampDaUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(ClampDaUnitSpec);

void ClampDaUnitSpec::Initialize() {
  send_da = CYCLE;
  da_val = DA_P;
  Initialize_core();
}


#include "BFCSUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(BFCSAChSpec);
TA_BASEFUNS_CTORS_DEFN(BFCSUnitSpec);

void BFCSUnitSpec::Initialize() {
  // SetUnique("deep_raw_qtr", true);
  // deep_raw_qtr = Q4;
  // SetUnique("act_range", true);
  // act_range.max = 2.0f;
  // act_range.min = -2.0f;
  // act_range.UpdateAfterEdit();
  // SetUnique("clamp_range", true);
  // clamp_range.max = 2.0f;
  // clamp_range.min = -2.0f;
  // clamp_range.UpdateAfterEdit();
}

void BFCSUnitSpec::HelpConfig() {
  String help = "BFCSUnitSpec (ACh value) Computation:\n\
 - Computes ACh value based on inputs from CEL, VS.\n\
 - No Learning\n\
 \nBFCSUnitSpec Configuration:\n\
 - Use the Wizard PVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs";
  taMisc::Confirm(help);
}

bool BFCSUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;
  bool rval = true;

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  // LEABRA_UNIT_STATE* un = lay->GetUnitState(net, 0);
  
  // if(lay->CheckError((act_range.max != 2.0f) || (act_range.min != -2.0f), quiet, rval,
  //               "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
  //               name,"(make sure this is appropriate for all layers that use this spec!)")) {
  //   SetUnique("act_range", true);
  //   act_range.max = 2.0f;
  //   act_range.min = -2.0f;
  //   act_range.UpdateAfterEdit();
  // }
  // if(lay->CheckError((clamp_range.max != 2.0f) || (clamp_range.min != -2.0f), quiet, rval,
  //               "requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
  //               name,"(make sure this is appropriate for all layers that use this spec!)")) {
  //   SetUnique("clamp_range", true);
  //   clamp_range.max = 2.0f;
  //   clamp_range.min = -2.0f;
  //   clamp_range.UpdateAfterEdit();
  // }
  return rval;
}


////////////////////////////////////////////////////////////////////
//              PVLV -- BG / VS

#include "MSNUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(MatrixActSpec);
TA_BASEFUNS_CTORS_DEFN(MSNUnitSpec);

void MSNUnitSpec::Initialize() {
  dar = D1R;
  matrix_patch = MATRIX;
  dorsal_ventral = DORSAL;
  valence = APPETITIVE;
  Initialize_core();
}

void MSNUnitSpec::Defaults_init() {
  deep_mod_zero = true;
}

void MSNUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // todo: could enforce various combinations here..
}


#include "TANUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(TANUnitSpec);
TA_BASEFUNS_CTORS_DEFN(TANActSpec);

void TANUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void TANUnitSpec::Defaults_init() {
  SetUnique("deep_raw_qtr", true);
  deep_raw_qtr = QALL;
}

void TANUnitSpec::HelpConfig() {
  String help = "TANUnitSpec Computation:\n\
 - Send ACh value to receiving units.\n\
 - Can be driven from OFC or learn itself from MarkerConSpecs from PV, VSPatch units\n\
 \nTANUnitSpec Configuration:\n\
 - For learning, use MarkerConSpecs from PosPV, VSPatchPosD1, or just RewTarg if avail.\n\
 - Learning cons from stimulus predictive inputs should be simple LeabraDeltaConSpec";
  taMisc::Confirm(help);
}


#include "PatchUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(PatchUnitSpec);

void PatchUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void PatchUnitSpec::Defaults_init() {
}


#include "GPiInvUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(GPiGateSpec);
TA_BASEFUNS_CTORS_DEFN(GPiMiscSpec);
TA_BASEFUNS_CTORS_DEFN(GPiInvUnitSpec);

void GPiInvUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void GPiInvUnitSpec::Defaults_init() {
  SetUnique("dt", true);
  dt.net_tau = 3.0f;
  gate_qtr = Q1_Q3;
}

void GPiInvUnitSpec::HelpConfig() {
  String help = "GPiInvUnitSpec Computation:\n\
 - Computes BG output as a positive activation signal with competition inhibition\n\
 select best gating candidate -- should receive inputs from MatrixGo and NoGo layers.\n\
 NoGo is indicated just by the presence of NoGo in the layer name.\n\
 \nGPiInvUnitSpec Configuration:\n\
 - Use the Wizard PBWM button to automatically configure layers.\n\
 - Recv cons should be standard, with no learning.\n\
 - Should send to PFC, and back to Matrix Go and NoGo, to deliver thal gating signal to drive learning.";
  taMisc::Confirm(help);
}

bool GPiInvUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;
  
  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  main_net->SetNetFlag(Network::NETIN_PER_PRJN); // this is required for this computation!
  net->SetNetFlag(NETWORK_STATE::NETIN_PER_PRJN); // this is required for this computation!
  if(lay->n_units == 0) return rval;
  // LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);

  return rval;
}


#include "InvertUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(InvertUnitSpec);

void InvertUnitSpec::Initialize() {
  Defaults_init();
}

void InvertUnitSpec::Defaults_init() {
}

bool InvertUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Unit(lay, quiet);

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);

  LEABRA_CON_STATE* cg = un->RecvConStateSafe(net, 0);
  if(lay->CheckError(!cg, quiet, rval,
                   "Requires one recv projection!")) {
    return false;
  }
  LEABRA_UNIT_STATE* su = cg->UnStateSafe(0, net);
  if(lay->CheckError(!su, quiet, rval, 
                   "Requires one unit in recv projection!")) {
    return false;
  }

  return rval;
}


////////////////////////////////////////////////////////////////////
//              PBWM

#include "PFCUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(PFCGateSpec);
TA_BASEFUNS_CTORS_DEFN(PFCMaintSpec);
TA_BASEFUNS_CTORS_DEFN(PFCDynEl);
TA_BASEFUNS_CTORS_DEFN(PFCUnitSpec);

void PFCUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void PFCUnitSpec::Defaults_init() {
  DefaultDynTable();
  SetUnique("deep", true);
  deep_raw_qtr = Q2_Q4;
  deep.on = true;
  deep.raw_thr_rel = 0.1f;
  deep.raw_thr_abs = 0.1f; // todo??
  deep.mod_min = 1.0f;
}

void PFCUnitSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdtDynTable();
}

void  PFCUnitSpec::FormatDynTable() {
  DataCol* dc;

  int idx;
  if((idx = dyn_table.FindColNameIdx("rise_dt")) >= 0)
    dyn_table.RemoveCol(idx);
  if((idx = dyn_table.FindColNameIdx("decay_dt")) >= 0)
    dyn_table.RemoveCol(idx);
  
  dc = dyn_table.FindMakeCol("name", VT_STRING);
  dc->desc = "name for this dynamic profile";

  dc = dyn_table.FindMakeCol("desc", VT_STRING);
  dc->desc = "description of this dynamic profile";

  dc = dyn_table.FindMakeCol("init", VT_FLOAT);
  dc->desc = "initial value at point when gating starts";

  dc = dyn_table.FindMakeCol("rise_tau", VT_FLOAT);
  dc->desc = "time constant for linear rise in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first";

  dc = dyn_table.FindMakeCol("decay_tau", VT_FLOAT);
  dc->desc = "time constant for linear decay in maintenance activation (per quarter when deep is updated) -- use integers -- if both rise and decay then rise comes first";

  dyn_table.EnforceRows(n_dyns);
}

void  PFCUnitSpec::DefaultDynTable(float std_tau) {
  if(taBase::GetRefn(&dyn_table) == 0) {     // own it -- can be formatted in init
    taBase::Own(dyn_table, this);
  }
  
  if(gate.out_gate)
    n_dyns = 1;
  else
    n_dyns = 1; 
  FormatDynTable();

  int cur = 0;
  SetDynVal("maint_flat", DYN_NAME, cur);
  SetDynVal("maintained, flat stable sustained activation", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  if(n_dyns <= 1) return;       // only do below for 5 default case..
  
  cur++;
  SetDynVal("phasic", DYN_NAME, cur);
  SetDynVal("immediate phasic response to gating event", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(1.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_rise", DYN_NAME, cur);
  SetDynVal("maintained, rising value over time", DYN_DESC, cur);
  SetDynVal(0.1f, DYN_INIT, cur);
  SetDynVal(std_tau, DYN_RISE_TAU, cur);
  SetDynVal(0.0f, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_decay", DYN_NAME, cur);
  SetDynVal("maintained, decaying value over time", DYN_DESC, cur);
  SetDynVal(1.0f, DYN_INIT, cur);
  SetDynVal(0.0f, DYN_RISE_TAU, cur);
  SetDynVal(std_tau, DYN_DECAY_TAU, cur);

  cur++;
  SetDynVal("maint_updn", DYN_NAME, cur);
  SetDynVal("maintained, rising then falling alue over time", DYN_DESC, cur);
  SetDynVal(0.1f, DYN_INIT, cur);
  SetDynVal(.5f * std_tau, DYN_RISE_TAU, cur);
  SetDynVal(std_tau, DYN_DECAY_TAU, cur);

  UpdtDynTable();
}

void  PFCUnitSpec::UpdtDynTable() {
  dyn_table.StructUpdate(true);
  FormatDynTable();
  for(int i=0; i<dyn_table.rows; i++) {
    float init = GetDynVal(DYN_INIT, i);
    if(init == 0.0f) {          // init must be a minimum val -- deep_raw = 0 is non-gated
      SetDynVal(.1f, DYN_INIT, i);
    }
  }
  dyn_table.StructUpdate(false);

  int nd = n_dyns;
  AllocDyns(nd);
  for(int i=0; i<nd; i++) {
    PFCDynEl* so = dyns_m + i;
    so->SetVals(GetDynVal(DYN_INIT, i), GetDynVal(DYN_RISE_TAU, i), GetDynVal(DYN_DECAY_TAU, i));
  }
  n_dyns = nd;
}


// note: following requires access to PFCDynEl_cpp -- hence _mbrs include

void PFCUnitSpec::CopyToState_Dyns(void* state_spec, const char* state_suffix) {
  String ss = state_suffix;
  if(ss == "_cpp") {
    PFCUnitSpec_cpp* tcc = (PFCUnitSpec_cpp*)state_spec;
    tcc->AllocDyns(n_dyns);
    for(int i=0; i<n_dyns; i++) {
      PFCDynEl* so = dyns_m + i;
      PFCDynEl_cpp* cp = tcc->dyns_m + i;
      cp->SetVals(so->init, so->rise_tau, so->decay_tau);
    }
    tcc->n_dyns = n_dyns;
  }
}

void PFCUnitSpec::CopyToState(void* state_spec, const char* state_suffix) {
  inherited::CopyToState(state_spec, state_suffix);
  CopyToState_Dyns(state_spec, state_suffix);
}

void PFCUnitSpec::UpdateStateSpecs() {
  UpdtDynTable();
  inherited::UpdateStateSpecs();
}

void PFCUnitSpec::GraphPFCDyns(DataTable* graph_data, int n_trials) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_PFCDyns", true);
  }
  int idx;
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* rw = graph_data->FindMakeColName("trial", idx, VT_FLOAT);

  for(int nd=0; nd < n_dyns; nd++) {
    graph_data->FindMakeColName("deep_raw_" + String(nd), idx, VT_FLOAT);
  }

  for(int x = 0; x <= n_trials; x++) {
    graph_data->AddBlankRow();
    rw->SetValAsFloat(x, -1);
    for(int nd=0; nd < n_dyns; nd++) {
      float nw;
      if(x == 0) {
        nw = GetDynVal(DYN_INIT, nd);
      }
      else {
        nw = UpdtDynVal(nd, (float)x);
      }
      graph_data->SetValAsFloat(nw, nd+1, -1);
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}



////////////////////////////////////////////////////////////////////
//              Hippo

#include "ECoutUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(ECoutUnitSpec);

void ECoutUnitSpec::Initialize() {
  Defaults_init();
  Initialize_core();
}

void ECoutUnitSpec::Defaults_init() {

}

bool ECoutUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);
  
  bool got_ec_in = false;
  const int nrg = un->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = un->RecvConState(net, g);
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    if(cs->IsMarkerCon() && recv_gp->size >= 1) {
      got_ec_in = true;
    }
  }

  if(lay->CheckError(!got_ec_in, quiet, rval,
                "no projection from ECin Layer found: must recv a MarkerConSpec prjn from it, with at least one unit")) {
    return false;
  }

  return true;
}


#include "CA1UnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(ThetaPhaseSpecs);
TA_BASEFUNS_CTORS_DEFN(CA1UnitSpec);

void CA1UnitSpec::Initialize() {
  Initialize_core();
}

bool CA1UnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);
  
  bool got_ec_in = false;
  bool got_ec_out = false;
  bool got_ca3 = false;
  const int nrg = un->NRecvConGps(net); 
  for(int g=0; g< nrg; g++) {
    LEABRA_CON_STATE* recv_gp = un->RecvConState(net, g);
    if(!recv_gp->PrjnIsActive(net)) continue; // key!! just check for prjn, not con group!
    LEABRA_LAYER_STATE* from = recv_gp->GetSendLayer(net);
    LEABRA_CON_SPEC_CPP* cs = recv_gp->GetConSpec(net);
    // cs->SetUnique("wt_scale", true); // be sure!
    if(from->LayerNameContains("EC")) {
      if(from->LayerNameContains("out")) {
        got_ec_out = true;
      }
      else if(from->LayerNameContains("in")) {
        got_ec_in = true;
      }
    }
    if(from->LayerNameContains("CA3")) {
      got_ca3 = true;
    }
  }

  // if(lay->CheckError(!got_ca3, quiet, rval,
  //                    "no projection from CA3 Layer found: must recv from layer with a name that contains the string 'CA3'")) {
  //   return false;
  // }
  // it may be OK to not include ECout as that pathway is a bit more suspect?
  // if(lay->CheckError(!got_ec_out, quiet, rval,
  //               "no projection from ECout Layer found: must recv from layer with a name that contains 'EC' and 'out'")) {
  //   return false;
  // }
  if(lay->CheckError(!got_ec_in, quiet, rval,
                "no projection from ECin Layer found: must recv from layer with a name that contains 'EC' and 'in'")) {
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////
//              Cerebellum

#include "CerebGranuleUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(CerebGranuleUnitSpec);
TA_BASEFUNS_CTORS_DEFN(CerebGranuleSpecs);

void CerebGranuleUnitSpec::Initialize() {
}

bool CerebGranuleUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;

  bool rval = true;

  if(lay->CheckError(stp.on, quiet, rval,
                     "Cannot have stp activated for Granule neurons -- using the stp parameters for special variables in granule cells -- I just turned it back off.")) {
    stp.on = false;
  }
  return true;
}


////////////////////////////////////////////////////////////////////
//              Misc

#include "LeabraMultCopyUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(LeabraMultCopyUnitSpec);

void LeabraMultCopyUnitSpec::Initialize() {
  one_minus = false;
  mult_gain = 1.0f;
  Initialize_core();
}

bool LeabraMultCopyUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  if(!inherited::CheckConfig_Unit(lay, quiet)) return false;
  bool rval = true;
  
  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);
  
  const int nrg = un->NRecvConGps(net);
  if(lay->CheckError(nrg != 2, quiet, rval,
                "leabra mult copy must have exactly 2 recv prjns, first = act to copy, second = act to multiply")) {
    return false;               // fatal
  }

  LEABRA_CON_STATE* copy_gp = un->RecvConState(net, 0);
  if(lay->CheckError(copy_gp->size != 1, quiet, rval,
                "leabra mult copy first prjn (copy act source) must have exactly 1 connection to copy from")) {
    return false;               // fatal
  }
  LEABRA_CON_STATE* mult_gp = un->RecvConState(net, 1);
  if(lay->CheckError(mult_gp->size != 1, quiet, rval,
                "leabra mult copy second prjn (mult act source) must have exactly 1 connection to get mult act from")) {
    return false;               // fatal
  }
  return rval;
}


#include "LeabraContextUnitSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(CtxtUpdateSpec);
TA_BASEFUNS_CTORS_DEFN(CtxtNSpec);
TA_BASEFUNS_CTORS_DEFN(LeabraContextUnitSpec);

void LeabraContextUnitSpec::Initialize() {
  updt.fm_prv = 0.0f;
  updt.fm_hid = 1.0f;
  updt.to_out = 1.0f;
  update_criteria = UC_TRIAL;
  Defaults_init();
  Initialize_core();
}

void LeabraContextUnitSpec::Defaults_init() {
}

bool LeabraContextUnitSpec::CheckConfig_Unit(Layer* lay, bool quiet) {
  bool rval = inherited::CheckConfig_Unit(lay, quiet);

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  if(lay->n_units == 0) return rval;
  LEABRA_UNIT_STATE* un = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);
  
  LEABRA_CON_STATE* cg = un->RecvConStateSafe(net, 0);
  if(lay->CheckError(!cg, quiet, rval,
                     "Requires one recv projection!")) {
    return false;
  }
  LEABRA_UNIT_STATE* su = cg->UnStateSafe(0, net);
  if(lay->CheckError(!su, quiet, rval, 
                     "Requires one unit in recv projection!")) {
    return false;
  }

  return rval;
}

void LeabraContextUnitSpec::TriggerUpdate(LeabraLayer* lay, bool update) {
  if (!lay) return;
  lay->SetLayerFlagState(Layer::LAY_FLAG_1, update);
  lay->SyncLayerState();
}

