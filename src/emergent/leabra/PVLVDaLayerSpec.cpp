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

#include "PVLVDaLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>
#include <PVrLayerSpec>
#include <PViLayerSpec>
#include <LVeLayerSpec>
#include <NVLayerSpec>
#include <ExtRewLayerSpec>
#include <OneToOnePrjnSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PVLVDaSpec);

TA_BASEFUNS_CTORS_DEFN(PVLVDaLayerSpec);

void PVLVDaSpec::Initialize() {
  da_gain = 1.0f;
  tonic_da = 0.0f;
  pv_gain = 1.0f;
  add_pv_lv = false;
}

void PVLVDaLayerSpec::Initialize() {
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 1;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = .25;
}

void PVLVDaLayerSpec::HelpConfig() {
  String help = "PVLVDaLayerSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from PVLV layers.\n\
 - No Learning\n\
 \nPVLVDaLayerSpec Configuration:\n\
 - Use the Wizard PVLV or PBWM button to automatically configure layers.\n\
 - Recv cons marked with a MarkerConSpec from PVLV\n\
 - This layer must be after recv layers in list of layers\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  taMisc::Confirm(help);
}

bool PVLVDaLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  // must have the appropriate ranges for unit specs..
  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

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
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  LeabraLayer* lve_lay = NULL;
  LeabraLayer* lvi_lay = NULL;
  LeabraLayer* pvi_lay = NULL;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraLayerSpec* fls = (LeabraLayerSpec*)fmlay->spec.SPtr();
    if(cs->InheritsFrom(TA_MarkerConSpec)) {
      if(recv_gp->size <= 0) {
        OneToOnePrjnSpec* pspec = (OneToOnePrjnSpec*)recv_gp->prjn->spec.SPtr();
        BaseSpec_Group* spgp = GET_OWNER(pspec, BaseSpec_Group);
        if(spgp) {
          bool made_new = false;
          OneToOnePrjnSpec* vtasp =
            (OneToOnePrjnSpec*)spgp->FindMakeNameType("VTAOneToOnePrjn",
                                                      &TA_OneToOnePrjnSpec, made_new);
          vtasp->send_start = 1; // set offset to 1
          recv_gp->prjn->SetPrjnSpec(vtasp);
          if(lay->CheckError(made_new, quiet, rval,
                             "requires a special projection spec that I just made -- you will have to re-build the network and re-init -- save project after this change")) {
          }
        }
      }
      // if(lay->CheckError(recv_gp->size <= 0, quiet, rval,
      //                    "requires one recv projection with at least one unit!  from:", 
      //                    fmlay->name)) {
      //   return false;
      // }
      if(fls->InheritsFrom(TA_LVeLayerSpec)) lve_lay = fmlay;
      if(fls->InheritsFrom(TA_LViLayerSpec)) lvi_lay = fmlay;
      if(fls->InheritsFrom(TA_PViLayerSpec)) pvi_lay = fmlay;
    }
  }

  if(lay->CheckError(!lve_lay, quiet, rval,
                "did not find LVe layer to get Da from!")) {
    rval = false;
  }
  // optional!
  // if(lay->CheckError(!lvi_lay, quiet, rval,
  //               "did not find LVi layer to get Da from!")) {
  //   rval = false;
  // }
  if(lay->CheckError(!pvi_lay, quiet, rval,
                "did not find PVi layer to get Da from!")) {
    rval = false;
  }

  return rval;
}

void PVLVDaLayerSpec::Compute_Da(LeabraLayer* lay, LeabraNetwork* net) {
  float net_da = 0.0f;
  int lve_prjn_idx;
  LeabraLayer* lve_lay = FindLayerFmSpec(lay, lve_prjn_idx, &TA_LVeLayerSpec);
  LVeLayerSpec* lve_sp = (LVeLayerSpec*)lve_lay->GetLayerSpec();
  int lvi_prjn_idx;
  LeabraLayer* lvi_lay = FindLayerFmSpec(lay, lvi_prjn_idx, &TA_LViLayerSpec);

  int pvi_prjn_idx;
  LeabraLayer* pvi_lay = FindLayerFmSpec(lay, pvi_prjn_idx, &TA_PViLayerSpec);
  PViLayerSpec* pvils = (PViLayerSpec*)pvi_lay->spec.SPtr();

  int nv_prjn_idx;
  LeabraLayer* nv_lay = FindLayerFmSpec(lay, nv_prjn_idx, &TA_NVLayerSpec);
  NVLayerSpec* nvls = NULL;
  if(nv_lay) nvls = (NVLayerSpec*)nv_lay->spec.SPtr();

  float lv_da = lve_sp->Compute_LVDa(lve_lay, lvi_lay, net);
  // nv only contributes to lv, not pv..
  if(nv_lay) {
    lv_da += nvls->Compute_NVDa(nv_lay, net);
  }
  float pv_da = da.pv_gain * pvils->Compute_PVDa(pvi_lay, net);
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  if(er_avail) {
    net_da = da.da_gain * pv_da;
    if(da.add_pv_lv) {
      net_da += da.da_gain * lv_da;
    }
  }
  else {
    net_da = da.da_gain * lv_da;
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

void PVLVDaLayerSpec::Send_Da(LeabraLayer* lay, LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    const float snd_val = u->act;
    for(int g=0; g<u->send.size; g++) {
      LeabraSendCons* send_gp = (LeabraSendCons*)u->send.FastEl(g);
      if(send_gp->NotActive()) continue;
      LeabraLayer* tol = (LeabraLayer*) send_gp->prjn->layer;
      for(int j=0;j<send_gp->size; j++) {
        ((LeabraUnit*)send_gp->Un(j,net))->dav = snd_val;
      }
    }
  }
}

void PVLVDaLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  Compute_Da(lay, net);
  Send_Da(lay, net);
  inherited::Compute_CycleStats(lay, net, thread_no);
}

void PVLVDaLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  lay->hard_clamped = false;
  lay->UnSetExtFlag(Unit::EXT);
  inherited::Compute_HardClamp(lay, net);
}


