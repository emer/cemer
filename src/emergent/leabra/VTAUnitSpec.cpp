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
  patch_cur = false;
  rec_data = false;
  se_gain = 0.1f;
  Defaults_init();
}

void PVLVDaSpec::Defaults_init() {
  tonic_da = 0.0f;
  pv_thr = 0.1f;
  vsp_thr = 0.1f;
}
  
void PVLVDaGains::Initialize() {
  Defaults_init();
}

void PVLVDaGains::Defaults_init() {
  da_gain = 1.0f;
  pptg_gain = 1.0f;
  lhb_gain = 1.0f;
  pv_gain = 1.0f;
  subtract_d2r = true;
  pvi_d1_gain = 1.2f;
  pvi_d2_gain = 1.0f;
  pvi_gain = 1.0f;
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
  //GetRecvLayers(u, pptg_lay, lhb_lay, pospv_lay, vspatch_lay, negpv_lay);
  
  if(da_val == DA_P) {
    LeabraLayer* pptg_lay_p = NULL;
    LeabraLayer* lhb_lay = NULL;
    LeabraLayer* pospv_lay = NULL;
    LeabraLayer* vspatch_lay = NULL;
    LeabraLayer* vspatch_d2_lay = NULL;
  
    GetRecvLayers_P(un, pospv_lay, pptg_lay_p, lhb_lay, vspatch_lay, vspatch_d2_lay);
  
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
    if(lay->CheckError(!vspatch_lay, quiet, rval,
                     "did not find VS Patch Direct layer to get pos PV shunting (cancelling) signal from (looks for layer with Patch in name)")) {
      rval = false;
    }
    if(gains.subtract_d2r) {
      if(lay->CheckError(!vspatch_d2_lay, quiet, rval,
                       "did not find VS Patch indirect layer to subtract from direct pos PV shunting (cancelling) signal from (not clear what should look for here)")) {
        rval = false;
      }
    }
  }
  else {
    // TODO: add more recv layer checks as we determine which layers VTAn should receive from - only sure of NegPV currently
    LeabraLayer* negpv_lay = NULL;
    LeabraLayer* pptg_lay_n = NULL;
    LeabraLayer* lhb_lay_n = NULL;
    LeabraLayer* vspatch_lay_n = NULL;
    LeabraLayer* vspatch_d1_lay_n = NULL;
    
    GetRecvLayers_N(un, negpv_lay, pptg_lay_n, lhb_lay_n, vspatch_lay_n, vspatch_d1_lay_n);

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
    if(lay->CheckError(!vspatch_lay_n, quiet, rval,
                     "did not find VSPatch D2 layer projection (looks for MSNUnitSpec and D2")) {
      rval = false;
    }
    if(lay->CheckError(!vspatch_d1_lay_n, quiet, rval,
                     "did not find VSPatch D1 layer projection (looks for MSNUnitSpec and D1)")) {
      rval = false;
    }
  }
  return rval;
}

bool VTAUnitSpec::GetRecvLayers_P(LeabraUnit* u, LeabraLayer*& pospv_lay,
                                  LeabraLayer*& pptg_lay_p, LeabraLayer*& lhb_lay,
                                  LeabraLayer*& vspatch_lay, LeabraLayer*& vspatch_d2_lay) {
  pospv_lay = NULL;
  pptg_lay_p = NULL;
  lhb_lay = NULL;
  vspatch_lay = NULL;
  vspatch_d2_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      if(us->InheritsFrom(TA_PPTgUnitSpec)) pptg_lay_p = fmlay;
      else if(us->InheritsFrom(TA_LHbRMTgUnitSpec)) lhb_lay = fmlay;
      else if(fmlay->name.contains("PV")) {
        pospv_lay = fmlay;
      }
      else if(us->InheritsFrom(TA_MSNUnitSpec)) {
        MSNUnitSpec* msus = (MSNUnitSpec*)us;
        if(msus->dar == MSNUnitSpec::D1R) {
          vspatch_lay = fmlay;
        }
        else if(msus->dar == MSNUnitSpec::D2R) {
          vspatch_d2_lay = fmlay;
        }
      }
    }
  }
  return true;
}

bool VTAUnitSpec::GetRecvLayers_N(LeabraUnit* u, LeabraLayer*& negpv_lay,
                                  LeabraLayer*& pptg_lay_n, LeabraLayer*& lhb_lay,
                                  LeabraLayer*& vspatch_lay_n, LeabraLayer*& vspatch_d1_lay_n) {
  // TODO: add more recv layer checks as we determine which layers VTAn should receive from...
  negpv_lay = NULL;
  pptg_lay_n = NULL;
  lhb_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      if(us->InheritsFrom(TA_PPTgUnitSpec)) { pptg_lay_n = fmlay; }
      else if(us->InheritsFrom(TA_LHbRMTgUnitSpec)) { lhb_lay = fmlay; }
      else if(fmlay->name.contains("PV")) { negpv_lay = fmlay; }
      else if(us->InheritsFrom(TA_MSNUnitSpec)) {
        MSNUnitSpec* msus = (MSNUnitSpec*)us;
        if(msus->dar == MSNUnitSpec::D1R) {
          vspatch_d1_lay_n = fmlay;
        }
        else if(msus->dar == MSNUnitSpec::D2R) {
          vspatch_lay_n = fmlay;
        }
      }
    }
  }
  return true;
}

void VTAUnitSpec::Compute_DaP(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* pptg_lay_p = NULL;
  LeabraLayer* lhb_lay = NULL;
  LeabraLayer* pospv_lay = NULL;
  LeabraLayer* vspatch_lay = NULL;
  LeabraLayer* vspatch_d2_lay = NULL;
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  
  LeabraLayer* lay = un->own_lay();

  GetRecvLayers_P(un, pospv_lay, pptg_lay_p, lhb_lay, vspatch_lay, vspatch_d2_lay);
    
  // use total activation over whole layer
  float pptg_da_p = pptg_lay_p->acts_eq.avg * pptg_lay_p->units.size;
  float lhb_da = lhb_lay->acts_eq.avg * lhb_lay->units.size;
  float pospv = pospv_lay->acts_eq.avg * pospv_lay->units.size;
  float vspvi;
  if(da.patch_cur)
    if(!gains.subtract_d2r) {
      vspvi = vspatch_lay->acts_eq.avg * vspatch_lay->units.size;
    }
    else {
      vspvi = (gains.pvi_d1_gain * vspatch_lay->acts_eq.avg * vspatch_lay->units.size) -
      (gains.pvi_d2_gain * vspatch_d2_lay->acts_eq.avg * vspatch_d2_lay->units.size);
    }
  else {
    if(!gains.subtract_d2r) {
      vspvi = vspatch_lay->acts_q0.avg * vspatch_lay->units.size;
    }
    else {
      vspvi = (gains.pvi_d1_gain * vspatch_lay->acts_q0.avg * vspatch_lay->units.size) -
      (gains.pvi_d2_gain * vspatch_d2_lay->acts_q0.avg * vspatch_d2_lay->units.size);
    }
}
  float burst_lhb_da = MIN(lhb_da, 0.0f); // if neg, promotes bursting
  float dip_lhb_da = MAX(lhb_da, 0.0f);   // else, promotes dipping
    
  // absorbing PosPV value - prevents double counting
  float tot_burst_da = MAX(gains.pv_gain * pospv, gains.pptg_gain * pptg_da_p);
  // likewise for lhb contribution to bursting (burst_lhb_da non-positive)
  tot_burst_da = MAX(tot_burst_da, -gains.lhb_gain * burst_lhb_da);
   
  // PVi shunting
  float net_burst_da = tot_burst_da - (gains.pvi_gain * vspvi);
    
  net_burst_da = MAX(net_burst_da, 0.0f);
    
  float net_block = (1.0f - (lv_block.pos_pv * pospv + lv_block.lhb_dip * lhb_da));
  net_block = MAX(0.0f, net_block);
    
  float net_da = net_burst_da - gains.lhb_gain * dip_lhb_da;

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
    lay->SetUserData("pospv", pospv);
    lay->SetUserData("pptg_da_p", pptg_da_p);
    lay->SetUserData("lhb_da", lhb_da);
    lay->SetUserData("tot_burst_da", tot_burst_da);
    lay->SetUserData("vs_patch_dir_pos", vspvi);
    lay->SetUserData("net_burst_da", net_burst_da);
    lay->SetUserData("net_da", net_da);
  }
}

void VTAUnitSpec::Compute_DaN(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* negpv_lay = NULL;
  LeabraLayer* pptg_lay_n = NULL;
  LeabraLayer* lhb_lay_n = NULL;
  LeabraLayer* vspatch_lay_n = NULL;
  LeabraLayer* vspatch_d1_lay_n = NULL;
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  
  LeabraLayer* lay = un->own_lay();

  GetRecvLayers_N(un, negpv_lay, pptg_lay_n, lhb_lay_n, vspatch_lay_n, vspatch_d1_lay_n);
  float negpv = negpv_lay->acts_eq.avg * negpv_lay->units.size;
  float pptg_da_n = pptg_lay_n->acts_eq.avg * pptg_lay_n->units.size;
  float lhb_da_n = lhb_lay_n->acts_eq.avg * lhb_lay_n->units.size;

  float vspvi_n;
  if(da.patch_cur) {
    vspvi_n = vspatch_lay_n->acts_eq.avg * vspatch_lay_n->units.size;
  }
  else {
    if(!gains.subtract_d2r) {
      vspvi_n = vspatch_lay_n->acts_q0.avg * vspatch_lay_n->units.size;
    }
    else {
      vspvi_n = (gains.pvi_d2_gain * vspatch_lay_n->acts_q0.avg * vspatch_lay_n->units.size) -
      (gains.pvi_d1_gain * vspatch_d1_lay_n->acts_q0.avg * vspatch_d1_lay_n->units.size);
    }
  }
  float burst_lhb_da_n = MAX(lhb_da_n, 0.0f); // if pos, promotes bursting
  float dip_lhb_da_n = MIN(lhb_da_n, 0.0f);   // else, promotes dipping
  
  // absorbing NegPV value - prevents double counting
  float negpv_da = negpv;
  negpv_da = MAX(negpv_da, 0.0f); // in case we add PVi-like shunting later...
    
    
  float tot_burst_da = MAX(gains.pv_gain * negpv_da, gains.pptg_gain * pptg_da_n);
  tot_burst_da = MAX(tot_burst_da, gains.lhb_gain * burst_lhb_da_n);
  
  // PVi shunting
  float net_burst_da = tot_burst_da - (gains.pvi_gain * vspvi_n);
  net_burst_da = MAX(net_burst_da, 0.0f);
    
  float tot_dip_da = gains.lhb_gain * dip_lhb_da_n;
  
//  float net_da = MAX(gains.pv_gain * negpv_da, gains.pptg_gain * pptg_da_n);
  
  float net_da = net_burst_da + tot_dip_da;
  
  net_da *= gains.da_gain;
    
  u->da_n = net_da;
  lay->da_n = u->da_n;
  u->ext = da.tonic_da + u->da_n;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;

  if(da.rec_data) {
    lay->SetUserData("negpv", negpv);
    lay->SetUserData("pptg_da_n", pptg_da_n);
    lay->SetUserData("lhb_da_n", lhb_da_n);
    lay->SetUserData("tot_burst_da", tot_burst_da);
    lay->SetUserData("tot_dip_da", tot_dip_da);
    lay->SetUserData("net_da", net_da);
  }
}

void VTAUnitSpec::Send_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float snd_val = u->act;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      if(da_val == DA_P) {
        ((LeabraUnitVars*)send_gp->UnVars(j,net))->da_p = snd_val;
      }
      else {
        ((LeabraUnitVars*)send_gp->UnVars(j,net))->da_n = snd_val;
      }
    }
  }
}

void VTAUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(Quarter_DeepRawNow(net->quarter)) {
    if(da_val == DA_P) {
      Compute_DaP(u, net, thr_no);
    }
    else {
      Compute_DaN(u, net, thr_no);
    }
    Send_Da(u, net, thr_no);
  }
  else {
    u->act = 0.0f;
    Send_Da(u, net, thr_no);    // send nothing
    // Compute_Da(u, net, thr_no); // then compute just for kicks
  }
}

void VTAUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}


