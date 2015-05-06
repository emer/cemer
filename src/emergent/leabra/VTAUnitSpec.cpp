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

TA_BASEFUNS_CTORS_DEFN(gdPVLVDaSpec);
TA_BASEFUNS_CTORS_DEFN(LVBlockSpec);
TA_BASEFUNS_CTORS_DEFN(VTAUnitSpec);

void gdPVLVDaSpec::Initialize() {
  da_gain = 1.0f;
  tonic_da = 0.0f;
  burst_gain = 1.0f;
  dip_gain = 1.0f;
  pv_gain = 1.0f;
  pv_thr = 0.1f;
  vsp_thr = 0.1f;
}

void LVBlockSpec::Initialize() {
  pos_pv = 2.0f;
  dip = 2.0f;
  rec_data = false;
}

void VTAUnitSpec::Initialize() {
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
 - Computes DA value based on inputs from gdPVLV layers: PPTg (bursts) and LHbRMTg (dips).\n\
 - No Learning\n\
 \nVTAUnitSpec Configuration:\n\
 - Use the Wizard gdPVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
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
  LeabraLayer* pptg_lay = NULL;
  LeabraLayer* lhb_lay = NULL;
  LeabraLayer* pospv_lay = NULL;
  LeabraLayer* vspatch_lay = NULL;
  GetRecvLayers(u, pptg_lay, lhb_lay, pospv_lay, vspatch_lay);

  if(u->CheckError(!pptg_lay, quiet, rval,
                "did not find PPTg layer to get Da bursts from (looks for PPTgUnitSpec)")) {
    rval = false;
  }
  if(u->CheckError(!lhb_lay, quiet, rval,
                "did not find LHbRMTg layer to get Da dips from (looks for LHbRMTgUnitSpec)")) {
    rval = false;
  }
  if(u->CheckError(!pospv_lay, quiet, rval,
                "did not find PosPV layer to get positive PV from -- looks for LearnModUnitSpec")) {
    rval = false;
  }
  if(u->CheckError(!vspatch_lay, quiet, rval,
                "did not find VS Patch Indir layer to get pos PV shunting (cancelling) signal from (looks for layer with Patch in name)")) {
    rval = false;
  }

  return rval;
}

bool VTAUnitSpec::GetRecvLayers(LeabraUnit* u,
                                LeabraLayer*& pptg_lay, LeabraLayer*& lhb_lay,
                                LeabraLayer*& pospv_lay, LeabraLayer*& vspatch_lay) {
  pptg_lay = NULL;
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
      if(us->InheritsFrom(TA_PPTgUnitSpec)) pptg_lay = fmlay;
      else if(us->InheritsFrom(TA_LHbRMTgUnitSpec)) lhb_lay = fmlay;
      else if(us->InheritsFrom(TA_LearnModUnitSpec)) pospv_lay = fmlay;
      else if(us->InheritsFrom(TA_VSPatchUnitSpec)) vspatch_lay = fmlay;
    }
  }
  return true;
}

void VTAUnitSpec::Compute_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  LeabraLayer* pptg_lay = NULL;
  LeabraLayer* lhb_lay = NULL;
  LeabraLayer* pospv_lay = NULL;
  LeabraLayer* vspatch_lay = NULL;
  LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
  GetRecvLayers(un, pptg_lay, lhb_lay, pospv_lay, vspatch_lay);

  // use avg act over layer..
  float burst_da = pptg_lay->acts_eq.avg;
  float dip_da = lhb_lay->acts_eq.avg;
  float pospv = pospv_lay->acts_eq.avg;
  float vspvi = vspatch_lay->acts_eq.avg;

  float pospv_da = pospv - vspvi;
  pospv_da = MAX(pospv_da, 0.0f); // can't dip through this!

  float net_block = (1.0f - (lv_block.pos_pv * pospv + lv_block.dip * dip_da));
  net_block = MAX(0.0f, net_block);

  float net_da = da.pv_gain * pospv_da + net_block * da.burst_gain * burst_da -
    da.dip_gain * dip_da;
  net_da *= da.da_gain;

  if(lv_block.rec_data) {
    LeabraLayer* lay = un->own_lay();
    lay->SetUserData("burst_da", burst_da);
    lay->SetUserData("dip_da", dip_da);
    lay->SetUserData("pos_pv", pospv);
    lay->SetUserData("vs_patch_indir_pos", vspvi);
    lay->SetUserData("pospv_da", pospv_da);
    lay->SetUserData("net_block", net_block);
    lay->SetUserData("net_da", net_da);
  }

  // also set the network ext rew pv settings
  bool pv_over_thr = (pospv >= da.pv_thr);
  bool vsp_over_thr = (vspvi >= da.vsp_thr);

  net->ext_rew_avail = true;    // always record pv values
  net->ext_rew = pospv;
  // if(pv_over_thr || vsp_over_thr) {
  //   net->pv_detected = true;
  // }
  // else {
  //   net->pv_detected = false;
  // }

  // net->pvlv_dav = net_da;
  //  lay->dav = net_da;
  u->dav = net_da;
  u->ext = da.tonic_da + u->dav;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
  u->da = 0.0f;
}

void VTAUnitSpec::Send_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  const float snd_val = u->act;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitVars*)send_gp->UnVars(j,net))->dav = snd_val;
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


