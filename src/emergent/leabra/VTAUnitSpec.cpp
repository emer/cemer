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

#include "VTAUnitSpec.h"

#include <LeabraNetwork>
#include <PPTgUnitSpec>
#include <LHbRMTgUnitSpec>
#include <MSNUnitSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PVLVDaSpec);
TA_BASEFUNS_CTORS_DEFN(PVLVDaGains);
TA_BASEFUNS_CTORS_DEFN(LVBlockSpec);
TA_BASEFUNS_CTORS_DEFN(VTAUnitSpec);

void PVLVDaSpec::Initialize() {
  patch_cur = true;
  rec_data = false;
  se_gain = 0.1f;
  Defaults_init();
}

void PVLVDaSpec::Defaults_init() {
  tonic_da = 0.0f;
}
  
void PVLVDaGains::Initialize() {
  Defaults_init();
}

void PVLVDaGains::Defaults_init() {
  da_gain = 1.0f;
  pptg_gain = 1.0f;
  lhb_gain = 1.0f;
  pv_gain = 1.0f;
  pvi_burst_shunt_gain = 1.05f;
  pvi_anti_burst_shunt_gain = 1.0f;
  pvi_anti_dip_shunt_gain = 0.0f;
  pvi_dip_shunt_gain = 0.0f;
}

void LVBlockSpec::Initialize() {
  Defaults_init();
}

void LVBlockSpec::Defaults_init() {
  pos_pv = 1.0f;
  lhb_dip = 2.0f;
}

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

  if(lay->units.leaves == 0) return rval;
  LeabraUnit* un = (LeabraUnit*)lay->units.Leaf(0); // take first one
  
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
    LeabraLayer* pptg_lay_p = NULL;
    LeabraLayer* lhb_lay = NULL;
    LeabraLayer* pospv_lay = NULL;
    LeabraLayer* vspatchposd1_lay = NULL;
    LeabraLayer* vspatchposd2_lay = NULL;
    LeabraLayer* vspatchnegd1_lay = NULL;
    LeabraLayer* vspatchnegd2_lay = NULL;
  
    GetRecvLayers_P(un, pospv_lay, pptg_lay_p, lhb_lay, vspatchposd1_lay,
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
    LeabraLayer* negpv_lay = NULL;
    LeabraLayer* pptg_lay_n = NULL;
    LeabraLayer* lhb_lay_n = NULL;
    LeabraLayer* vspatchnegd1_lay = NULL;
    LeabraLayer* vspatchnegd2_lay = NULL;
    
    GetRecvLayers_N(un, negpv_lay, pptg_lay_n, lhb_lay_n, vspatchnegd1_lay, vspatchnegd2_lay);

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

bool VTAUnitSpec::GetRecvLayers_P
(LeabraUnit* u, LeabraLayer*& pospv_lay, LeabraLayer*& pptg_lay_p, LeabraLayer*& lhb_lay,
 LeabraLayer*& vspatchposd1_lay, LeabraLayer*& vspatchposd2_lay, 
 LeabraLayer*& vspatchnegd1_lay, LeabraLayer*& vspatchnegd2_lay) {
  pospv_lay = NULL;
  pptg_lay_p = NULL;
  lhb_lay = NULL;
  vspatchposd1_lay = NULL;
  vspatchposd2_lay = NULL;
  vspatchnegd1_lay = NULL;
  vspatchnegd2_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)u->RecvConState(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      if(us->InheritsFrom(TA_PPTgUnitSpec)) {
        pptg_lay_p = fmlay;
      }
      else if(us->InheritsFrom(TA_LHbRMTgUnitSpec)) {
        lhb_lay = fmlay;
      }
      else if(fmlay->name.contains("PV")) {
        pospv_lay = fmlay;
      }
      else if(us->InheritsFrom(TA_MSNUnitSpec)) {
        MSNUnitSpec* msus = (MSNUnitSpec*)us;
        if(msus->valence == MSNUnitSpec::APPETITIVE && msus->dar == MSNUnitSpec::D1R) {
          vspatchposd1_lay = fmlay;
        }
        else if(msus->valence == MSNUnitSpec::APPETITIVE && msus->dar == MSNUnitSpec::D2R) {
          vspatchposd2_lay = fmlay;
        }
        else if(msus->valence == MSNUnitSpec::AVERSIVE && msus->dar == MSNUnitSpec::D1R) {
          vspatchnegd1_lay = fmlay;
        }
        else if(msus->valence == MSNUnitSpec::AVERSIVE && msus->dar == MSNUnitSpec::D2R) {
          vspatchnegd2_lay = fmlay;
        }
      }
    }
  }
  return true;
}

bool VTAUnitSpec::GetRecvLayers_N
(LeabraUnit* u, LeabraLayer*& negpv_lay, LeabraLayer*& pptg_lay_n, LeabraLayer*& lhb_lay,
 LeabraLayer*& vspatchnegd1_lay, LeabraLayer*& vspatchnegd2_lay_n) {
  // TODO: add more recv layer checks as we determine which layers VTAn should receive from...
  negpv_lay = NULL;
  pptg_lay_n = NULL;
  lhb_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConState_cpp* recv_gp = (LeabraConState_cpp*)u->RecvConState(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      if(us->InheritsFrom(TA_PPTgUnitSpec)) {
        pptg_lay_n = fmlay;
      }
      else if(us->InheritsFrom(TA_LHbRMTgUnitSpec)) {
        lhb_lay = fmlay;
      }
      else if(fmlay->name.contains("PV")) {
        negpv_lay = fmlay;
      }
      else if(us->InheritsFrom(TA_MSNUnitSpec)) {
        MSNUnitSpec* msus = (MSNUnitSpec*)us;
        if(msus->valence == MSNUnitSpec::AVERSIVE && msus->dar == MSNUnitSpec::D2R) {
          vspatchnegd2_lay_n = fmlay;
        }
        else if(msus->valence == MSNUnitSpec::AVERSIVE && msus->dar == MSNUnitSpec::D1R) {
          vspatchnegd1_lay = fmlay;
        }
      }
    }
  }
  return true;
}

void VTAUnitSpec::Compute_DaP(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* pptg_lay_p = NULL;
  LeabraLayer* lhb_lay = NULL;
  LeabraLayer* pospv_lay = NULL;
  LeabraLayer* vspatchposd1_lay = NULL;
  LeabraLayer* vspatchposd2_lay = NULL;
  LeabraLayer* vspatchnegd1_lay = NULL;
  LeabraLayer* vspatchnegd2_lay = NULL;
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  
  LeabraLayer* lay = un->own_lay();

  GetRecvLayers_P(un, pospv_lay, pptg_lay_p, lhb_lay, vspatchposd1_lay, vspatchposd2_lay,
                  vspatchnegd1_lay, vspatchnegd2_lay);
    
  // use total activation over whole layer
  float pptg_da_p = pptg_lay_p->acts_eq.avg * pptg_lay_p->units.size;
  float lhb_da = lhb_lay->acts_eq.avg * lhb_lay->units.size;
  //float lhb_da = lhb_lay->units[0]->act();
  
  
  float pospv = pospv_lay->acts_eq.avg * pospv_lay->units.size;
  float vspospvi = 0.0f;
  if(da.patch_cur) {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspospvi = (gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActEq()) -
        (gains.pvi_anti_burst_shunt_gain * vspatchposd2_lay->GetTotalActEq());
    }
    else {
      vspospvi = gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActEq();
    }
  }
  else {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspospvi = (gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActQ0()) -
        (gains.pvi_anti_burst_shunt_gain * vspatchposd2_lay->GetTotalActQ0());
    }
    else {
      vspospvi = gains.pvi_burst_shunt_gain * vspatchposd1_lay->GetTotalActQ0();
    }
  }
  
  // vspospvi must be >= 0.0f
  vspospvi = fmaxf(vspospvi, 0.0f);
  
  float vsnegpvi = 0.0f;
  if(da.patch_cur) {
    if(gains.pvi_dip_shunt_gain > 0.0f && gains.pvi_anti_dip_shunt_gain > 0.0f) {
      vsnegpvi = (gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActEq()) -
        (gains.pvi_anti_dip_shunt_gain * vspatchnegd1_lay->GetTotalActEq());
    }
    else if(gains.pvi_dip_shunt_gain > 0.0f) {
      vsnegpvi = gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActEq();
    }
  }
  else {
    if(gains.pvi_dip_shunt_gain > 0.0f && gains.pvi_anti_dip_shunt_gain > 0.0f) {
      vsnegpvi = (gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActQ0()) -
        (gains.pvi_anti_dip_shunt_gain * vspatchnegd1_lay->GetTotalActQ0());
    }
    else if(gains.pvi_dip_shunt_gain > 0.0f) {
      vsnegpvi = gains.pvi_dip_shunt_gain * vspatchnegd2_lay->GetTotalActQ0();
    }
  }

  // ?? vsnegpvi must be >= 0.0f (probably; could be <= so need to think about it...)
  //vsnegpvi = fmaxf(vsnegpvi, 0.0f);
  
  float burst_lhb_da = fminf(lhb_da, 0.0f); // if neg, promotes bursting
  float dip_lhb_da = fmaxf(lhb_da, 0.0f);   // else, promotes dipping
    
  // absorbing PosPV value - prevents double counting
  float tot_burst_da = fmaxf(gains.pv_gain * pospv, gains.pptg_gain * pptg_da_p);
  // likewise for lhb contribution to bursting (burst_lhb_da non-positive)
  tot_burst_da = fmaxf(tot_burst_da, -gains.lhb_gain * burst_lhb_da);
   
  // pos PVi shunting
  float net_burst_da = tot_burst_da - vspospvi;
  net_burst_da = fmaxf(net_burst_da, 0.0f);
  //if(net_burst_da < 0.1f) { net_burst_da = 0.0f; } // debug...

  float tot_dip_da = gains.lhb_gain * dip_lhb_da;

  // neg PVi shunting
  float net_dip_da = tot_dip_da - vsnegpvi;
  net_dip_da = fmaxf(net_dip_da, 0.0f);
  //if(net_dip_da < 0.1f) { net_dip_da = 0.0f; } // debug...
    
  float net_da = net_burst_da - net_dip_da;
  net_da *= gains.da_gain;

  net_da -= da.se_gain * u->sev; // subtract 5HT serotonin -- has its own gain
  
  u->da_p = net_da;
  lay->da_p = u->da_p;
  u->ext = da.tonic_da + u->da_p;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;

  net->ext_rew_avail = true;    // always record pv values -- todo: why??
  net->ext_rew = pospv;

  if(da.rec_data) {
    lay->SetUserData("pospv", pospv, false); // false=no update
    lay->SetUserData("pptg_da_p", pptg_da_p, false); // false=no update
    lay->SetUserData("lhb_da", lhb_da, false); // false=no update
    lay->SetUserData("tot_burst_da", tot_burst_da, false); // false=no update
    lay->SetUserData("vs_patch_burst_shunt_net", vspospvi, false); // false=no update
    lay->SetUserData("vs_patch_dip_shunt_net", vsnegpvi, false); // false=no update
    lay->SetUserData("net_burst_da", net_burst_da, false); // false=no update
    lay->SetUserData("net_da", net_da, false); // false=no update
  }
}

void VTAUnitSpec::Compute_DaN(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* negpv_lay = NULL;
  LeabraLayer* pptg_lay_n = NULL;
  LeabraLayer* lhb_lay_n = NULL;
  LeabraLayer* vspatchnegd1_lay = NULL;
  LeabraLayer* vspatchnegd2_lay = NULL;
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  
  LeabraLayer* lay = un->own_lay();

  GetRecvLayers_N(un, negpv_lay, pptg_lay_n, lhb_lay_n, vspatchnegd1_lay,
                  vspatchnegd2_lay);
  float negpv = negpv_lay->acts_eq.avg * negpv_lay->units.size;
  float pptg_da_n = pptg_lay_n->acts_eq.avg * pptg_lay_n->units.size;
  float lhb_da_n = lhb_lay_n->acts_eq.avg * lhb_lay_n->units.size;

  float vspvi_n = 0.0f;
  if(da.patch_cur) {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspvi_n = (gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActEq()) -
        (gains.pvi_anti_burst_shunt_gain * vspatchnegd1_lay->GetTotalActEq());
    }
    else {
      vspvi_n = gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActEq();
    }
  }
  else {
    if(gains.pvi_anti_burst_shunt_gain > 0.0f) {
      vspvi_n = (gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActQ0()) -
        (gains.pvi_anti_burst_shunt_gain * vspatchnegd1_lay->GetTotalActQ0());
    }
    else {
      vspvi_n = gains.pvi_burst_shunt_gain * vspatchnegd2_lay->GetTotalActQ0();
    }
  }
  float burst_lhb_da_n = fmaxf(lhb_da_n, 0.0f); // if pos, promotes bursting
  float dip_lhb_da_n = fminf(lhb_da_n, 0.0f);   // else, promotes dipping
  
  // absorbing NegPV value - prevents double counting
  float negpv_da = negpv;
  negpv_da = fmaxf(negpv_da, 0.0f); // in case we add PVi-like shunting later...
    
    
  float tot_burst_da = fmaxf(gains.pv_gain * negpv_da, gains.pptg_gain * pptg_da_n);
  tot_burst_da = fmaxf(tot_burst_da, gains.lhb_gain * burst_lhb_da_n);
  
  // PVi shunting
  float net_burst_da = tot_burst_da - vspvi_n;
  net_burst_da = fmaxf(net_burst_da, 0.0f);
    
  float tot_dip_da = gains.lhb_gain * dip_lhb_da_n;
  
//  float net_da = fmaxf(gains.pv_gain * negpv_da, gains.pptg_gain * pptg_da_n);
  
  float net_da = net_burst_da + tot_dip_da;
  
  net_da *= gains.da_gain;
    
  u->da_n = net_da;
  lay->da_n = u->da_n;
  u->ext = da.tonic_da + u->da_n;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;

  if(da.rec_data) {
    lay->SetUserData("negpv", negpv, false); // false=no update
    lay->SetUserData("pptg_da_n", pptg_da_n, false); // false=no update
    lay->SetUserData("lhb_da_n", lhb_da_n, false); // false=no update
    lay->SetUserData("tot_burst_da", tot_burst_da, false); // false=no update
    lay->SetUserData("tot_dip_da", tot_dip_da, false); // false=no update
    lay->SetUserData("net_da", net_da, false); // false=no update
  }
}

void VTAUnitSpec::Send_Da(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  const float snd_val = u->act_eq;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConState_cpp* send_gp = (LeabraConState_cpp*)u->SendConState(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      if(da_val == DA_P) {
        ((LeabraUnitState_cpp*)send_gp->UnState(j,net))->da_p = snd_val;
      }
      else {
        ((LeabraUnitState_cpp*)send_gp->UnState(j,net))->da_n = snd_val;
      }
    }
  }
}

void VTAUnitSpec::Compute_Act_Rate(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  if(Quarter_DeepRawNow(net->quarter)) {
    if(da_val == DA_P) {
      Compute_DaP(u, net, thr_no);
    }
    else {
      Compute_DaN(u, net, thr_no);
    }
  }
  else {
    u->act_eq = u->act_nd = u->act = u->net = 0.0f;
  }
  u->da = 0.0f;
}

void VTAUnitSpec::Compute_Act_Spike(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

void VTAUnitSpec::Compute_Act_Post(LeabraUnitState_cpp* u, LeabraNetwork* net, int thr_no) {
  inherited::Compute_Act_Post(u, net, thr_no);
  Send_Da(u, net, thr_no);      // note: can only send modulators during post!!
}

