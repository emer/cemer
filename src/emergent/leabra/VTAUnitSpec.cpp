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
#include <MarkerConSpec>
#include <OneToOnePrjnSpec>
#include <PPTgUnitSpec>
#include <LHbRMTgUnitSpec>
#include <LearnModUnitSpec>
#include <VSPatchUnitSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PVLVDaSpec);
TA_BASEFUNS_CTORS_DEFN(LVBlockSpec);
TA_BASEFUNS_CTORS_DEFN(VTAUnitSpec);

void PVLVDaSpec::Initialize() {
  da_gain = 1.0f;
  tonic_da = 0.0f;
  pptg_gain = 1.0f;
  lhb_gain = 1.0f;
  pv_gain = 1.0f;
  pv_thr = 0.1f;
  pvi_gain = 1.0f;
  vsp_thr = 0.1f;
}

void LVBlockSpec::Initialize() {
  pos_pv = 5.0f;
  lhb_dip = 2.0f;
  rec_data = false;
}

void VTAUnitSpec::Initialize() {
  da_val = DA_P;
  SetUnique("deep_qtr", true);
  deep_qtr = Q4;
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

bool VTAUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;
  LeabraUnit* u = (LeabraUnit*)un;

  bool rval = true;

  if(un->CheckError((act_range.max != 2.0f) || (act_range.min != -2.0f), quiet, rval,
                "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
                name,"(make sure this is appropriate for all layers that use this spec!)")) {
    SetUnique("act_range", true);
    act_range.max = 2.0f;
    act_range.min = -2.0f;
    act_range.UpdateAfterEdit();
  }
  if(un->CheckError((clamp_range.max != 2.0f) || (clamp_range.min != -2.0f), quiet, rval,
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
  
    GetRecvLayers_P(u, pptg_lay_p, lhb_lay, pospv_lay, vspatch_lay);
  
    if(u->CheckError(!pptg_lay_p, quiet, rval,
                     "did not find PPTg layer to get DA bursts from (looks for PPTgUnitSpec)")) {
      rval = false;
    }
    if(u->CheckError(!lhb_lay, quiet, rval,
                     "did not find LHbRMTg layer to get DA dips from (looks for LHbRMTgUnitSpec)")) {
      rval = false;
    }
    if(u->CheckError(!pospv_lay, quiet, rval,
                     "did not find PosPV layer to get positive PV from -- looks for LearnModUnitSpec and Pos in layer name")) {
      rval = false;
    }
    if(u->CheckError(!vspatch_lay, quiet, rval,
                     "did not find VS Patch Indir layer to get pos PV shunting (cancelling) signal from (looks for layer with Patch in name)")) {
      rval = false;
    }
  }
  else {
    // TODO: add more recv layer checks as we determine which layers VTAn should receive from - only sure of NegPV currently
    LeabraLayer* negpv_lay = NULL;
    LeabraLayer* pptg_lay_n = NULL;
    GetRecvLayers_N(u, negpv_lay, pptg_lay_n);

    if(u->CheckError(!negpv_lay, quiet, rval,
                     "did not find NegPV layer to get negative PV from -- looks for LearnModUnitSpec and Neg in layer name")) {
      rval = false;
    }
  }
  return rval;
}

bool VTAUnitSpec::GetRecvLayers_P(LeabraUnit* u,
                                LeabraLayer*& pptg_lay_p, LeabraLayer*& lhb_lay,
                                LeabraLayer*& pospv_lay, LeabraLayer*& vspatch_lay) {
  pptg_lay_p = NULL;
  lhb_lay = NULL;
  pospv_lay = NULL;
  vspatch_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->InheritsFrom(TA_MarkerConSpec)) {
      if(us->InheritsFrom(TA_PPTgUnitSpec)) pptg_lay_p = fmlay;
      else if(us->InheritsFrom(TA_LHbRMTgUnitSpec)) lhb_lay = fmlay;
      else if(us->InheritsFrom(TA_LearnModUnitSpec)) {
        pospv_lay = fmlay;
      }
      else if(us->InheritsFrom(TA_VSPatchUnitSpec)) vspatch_lay = fmlay;
    }
  }
  return true;
}

bool VTAUnitSpec::GetRecvLayers_N(LeabraUnit* u, LeabraLayer*& negpv_lay, LeabraLayer*& pptg_lay_n) {
  // TODO: add more recv layer checks as we determine which layers VTAn should receive from - only sure of NegPV currently
  negpv_lay = NULL;
  
  const int nrg = u->NRecvConGps();
  for(int g=0; g<nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->InheritsFrom(TA_MarkerConSpec)) {
      if(us->InheritsFrom(TA_PPTgUnitSpec)) pptg_lay_n = fmlay;
//      else if(us->InheritsFrom(TA_LHbRMTgUnitSpec)) lhb_lay = fmlay;
//      else if(us->InheritsFrom(TA_LearnModUnitSpec) && fmlay->name.contains("Pos")) {
//        pospv_lay = fmlay;
//      }
//      else if(us->InheritsFrom(TA_LearnModUnitSpec) && fmlay->name.contains("Neg")) {
//        negpv_lay = fmlay;
//      }
      if(us->InheritsFrom(TA_LearnModUnitSpec)) {
        negpv_lay = fmlay;
      }
//      else if(us->InheritsFrom(TA_VSPatchUnitSpec)) vspatch_lay = fmlay;
    }
  }
  return true;
}

void VTAUnitSpec::Compute_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* pptg_lay_p = NULL;
  LeabraLayer* pptg_lay_n = NULL;
  LeabraLayer* lhb_lay = NULL;
  LeabraLayer* pospv_lay = NULL;
  LeabraLayer* negpv_lay = NULL;
  LeabraLayer* vspatch_lay = NULL;
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  
  // TODO: add other recv layer inputs as we determine which layers VTAn should receive from based on empical data and phenomenology - really only sure of NegPV currently
  LeabraLayer* lay = un->own_lay();

  // use avg act over layer..
  float pptg_da_p = 0.0f;
  float pptg_da_n = 0.0f;
  float lhb_da = 0.0f;
  float pospv = 0.0f;
  float negpv = 0.0f;
  float vspvi = 0.0f;
  
  float negpv_da = 0.0f;
  float pospv_da = 0.0f;
  float net_block = 0.0f;
  float net_da = 0.0f;
  
  if(da_val == DA_N) { // atypical burst for NegPV case
    GetRecvLayers_N(un, negpv_lay, pptg_lay_n);
    LeabraLayer* lay = un->own_lay();
    negpv = negpv_lay->acts_eq.avg * negpv_lay->units.size;
    pptg_da_n = pptg_lay_n->acts_eq.avg * pptg_lay_n->units.size;

    net_da = 0.0f;
    
    negpv_da = negpv;
    negpv_da = MAX(negpv_da, 0.0f); // in case we add shunting later...
    // may add LV block later...
//    net_da = da.pv_gain * negpv_da + da.pptg_gain * pptg_da_n;
    net_da = MAX(da.pv_gain * negpv_da, da.pptg_gain * pptg_da_n);
    // don't double-count PV signal
    //TODO: probably need separate pptg_p_ and pptg_n_ gain factors
    
    net_da *= da.da_gain;
    
    //net_da *= -1.0f; // TODO: eventually delete since vestigial, original implementation; kept temporarily for Randy's short-term reference only
    u->da_n = net_da;
    lay->da_n = u->da_n;
    u->ext = da.tonic_da + u->da_n;
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    u->da = 0.0f;
  }
  else { // classic Schultz guys
    GetRecvLayers_P(un, pptg_lay_p, lhb_lay, pospv_lay, vspatch_lay);
    LeabraLayer* lay = un->own_lay();
    
    // use total activation over whole layer
    pptg_da_p = pptg_lay_p->acts_eq.avg * pptg_lay_p->units.size;
    lhb_da = lhb_lay->acts_eq.avg * lhb_lay->units.size;
    pospv = pospv_lay->acts_eq.avg * pospv_lay->units.size;
    vspvi = vspatch_lay->acts_q0.avg * vspatch_lay->units.size;
    
    pospv_da = 0.0f;
    net_block = 0.0f;
    net_da = 0.0f;
    
    //float pospv_da = pospv - vspvi; // original
    pospv_da = pospv - da.pvi_gain * vspvi; // higher pvi_gain == more shunting
    
    pospv_da = MAX(pospv_da, 0.0f); // shunting should not be able to produce dip!
    
    net_block = (1.0f - (lv_block.pos_pv * pospv + lv_block.lhb_dip * lhb_da));
    net_block = MAX(0.0f, net_block);
    
    net_da = da.pv_gain * pospv_da - da.lhb_gain * lhb_da + net_block * da.pptg_gain * pptg_da_p;
    
    net_da *= da.da_gain;
    
    u->da_p = net_da;
    lay->da_p = u->da_p;
    u->ext = da.tonic_da + u->da_p;
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    u->da = 0.0f;

    // also set the network ext rew pv settings
    // bool pv_over_thr = (pospv >= da.pv_thr);
    // bool vsp_over_thr = (vspvi >= da.vsp_thr);

    net->ext_rew_avail = true;    // always record pv values -- todo: why??
    net->ext_rew = pospv;
  }

  if(lv_block.rec_data) {
    lay->SetUserData("pptg_da_p", pptg_da_p);
    lay->SetUserData("pptg_da_n", pptg_da_n);
    lay->SetUserData("lhb_da", lhb_da);
    lay->SetUserData("pos_pv", pospv);
    lay->SetUserData("vs_patch_dir_pos", vspvi);
    lay->SetUserData("pospv_da", pospv_da);
    lay->SetUserData("negpv_da", negpv_da);
    lay->SetUserData("net_block", net_block);
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
  if(Quarter_DeepNow(net->quarter)) {
    Compute_Da(u, net, thr_no);
    Send_Da(u, net, thr_no);
  }
  else {
    u->act = 0.0f;
    Send_Da(u, net, thr_no);    // send nothing
    Compute_Da(u, net, thr_no); // then compute just for kicks
  }
}

void VTAUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}


