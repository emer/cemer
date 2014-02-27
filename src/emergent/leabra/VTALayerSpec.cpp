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

#include "VTALayerSpec.h"

#include <LeabraNetwork>
#include <MarkerConSpec>
#include <OneToOnePrjnSpec>
#include <PPTgUnitSpec>
#include <LHbRMTgUnitSpec>
#include <LearnDriverLayerSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(gdPVLVDaSpec);

TA_BASEFUNS_CTORS_DEFN(LVBlockSpec);

TA_BASEFUNS_CTORS_DEFN(VTALayerSpec);

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
  pos_pv = 1.0f;
  dip = 1.0f;
}

void VTALayerSpec::Initialize() {
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .25;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
}

void VTALayerSpec::HelpConfig() {
  String help = "VTALayerSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from gdPVLV layers: PPTg (bursts) and LHbRMTg (dips).\n\
 - No Learning\n\
 \nVTALayerSpec Configuration:\n\
 - Use the Wizard gdPVLV button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from inputs\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  taMisc::Confirm(help);
}

bool VTALayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->SetUnique("maxda", true);
  us->maxda.val = MaxDaSpec::NO_MAX_DA;

  if(lay->CheckError((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f), quiet, rval,
                "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:",
                us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act_range", true);
    us->act_range.max = 2.0f;
    us->act_range.min = -2.0f;
    us->act_range.UpdateAfterEdit();
  }
  if(lay->CheckError((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f), quiet, rval,
                "requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:",
                us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("clamp_range", true);
    us->clamp_range.max = 2.0f;
    us->clamp_range.min = -2.0f;
    us->clamp_range.UpdateAfterEdit();
  }

  // check recv connection
  if(lay->units.leaves == 0) return false;
  LeabraLayer* pptg_lay = NULL;
  LeabraLayer* lhb_lay = NULL;
  LeabraLayer* pospv_lay = NULL;
  LeabraLayer* vspatch_lay = NULL;
  GetRecvLayers(lay, pptg_lay, lhb_lay, pospv_lay, vspatch_lay);

  if(lay->CheckError(!pptg_lay, quiet, rval,
                "did not find PPTg layer to get Da bursts from (looks for PPTgUnitSpec)")) {
    rval = false;
  }
  if(lay->CheckError(!lhb_lay, quiet, rval,
                "did not find LHbRMTg layer to get Da dips from (looks for LHbRMTgUnitSpec)")) {
    rval = false;
  }
  if(lay->CheckError(!pospv_lay, quiet, rval,
                "did not find PosPV layer to get positive PV from -- looks for LearnDriverLayerSpec")) {
    rval = false;
  }
  if(lay->CheckError(!vspatch_lay, quiet, rval,
                "did not find VS Patch Indir layer to get pos PV shunting (cancelling) signal from (looks for layer with Patch in name)")) {
    rval = false;
  }

  return rval;
}

bool VTALayerSpec::GetRecvLayers(LeabraLayer* lay,
                                 LeabraLayer*& pptg_lay, LeabraLayer*& lhb_lay,
                                 LeabraLayer*& pospv_lay, LeabraLayer*& vspatch_lay) {
  pptg_lay = NULL;
  lhb_lay = NULL;
  pospv_lay = NULL;
  vspatch_lay = NULL;
  if(lay->units.leaves == 0) return false;
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    if(cs->InheritsFrom(TA_MarkerConSpec)) {
      if(fmlay->GetUnitSpec()->InheritsFrom(TA_PPTgUnitSpec)) pptg_lay = fmlay;
      else if(fmlay->GetUnitSpec()->InheritsFrom(TA_LHbRMTgUnitSpec)) lhb_lay = fmlay;
      else if(fls->InheritsFrom(TA_LearnDriverLayerSpec)) pospv_lay = fmlay;
      else if(fmlay->name.contains("Patch")) vspatch_lay = fmlay;
    }
  }
  return true;
}

void VTALayerSpec::Compute_Da(LeabraLayer* lay, LeabraNetwork* net) {
  LeabraLayer* pptg_lay = NULL;
  LeabraLayer* lhb_lay = NULL;
  LeabraLayer* pospv_lay = NULL;
  LeabraLayer* vspatch_lay = NULL;
  GetRecvLayers(lay, pptg_lay, lhb_lay, pospv_lay, vspatch_lay);

  // use avg act over layer..
  float burst_da = pptg_lay->acts.avg;
  float dip_da = lhb_lay->acts.avg;
  float pospv = pospv_lay->acts.avg;
  float vspvi = vspatch_lay->acts.avg;

  float pospv_da = pospv - vspvi;
  pospv_da = MAX(pospv_da, 0.0f); // can't dip through this!

  float net_block = (1.0f - (lv_block.pos_pv * pospv_da + lv_block.dip * dip_da));
  net_block = MAX(0.0f, net_block);

  float net_da = da.pv_gain * pospv_da + net_block * da.burst_gain * burst_da -
    da.dip_gain * dip_da;
  net_da *= da.da_gain;

  // also set the network ext rew pv settings
  bool pv_over_thr = (pospv >= da.pv_thr);
  bool vsp_over_thr = (vspvi >= da.vsp_thr);

  net->ext_rew_avail = true;    // always record pv values
  net->ext_rew = pospv;
  if(pv_over_thr || vsp_over_thr) {
    net->pv_detected = true;
  }
  else {
    net->pv_detected = false;
  }

  net->pvlv_dav = net_da;
  lay->dav = net_da;
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    u->dav = net_da;
    u->ext = da.tonic_da + u->dav;
    u->act_lrn = u->act_eq = u->act_nd = u->act = u->net = u->ext;
    u->da = 0.0f;
  }
}

void VTALayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    const float snd_val = u->act;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      if(tol->lesioned())       continue;
      for(int j=0;j<send_gp->size; j++) {
        ((LeabraUnit*)send_gp->Un(j,net))->dav = snd_val;
      }
    }
  }
}

void VTALayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  Compute_Da(lay, net);
  Send_Da(lay, net);
  inherited::Compute_CycleStats(lay, net);
}

void VTALayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = false;
  lay->UnSetExtFlag(Unit::EXT);
  inherited::Compute_HardClamp(lay, net);
}

