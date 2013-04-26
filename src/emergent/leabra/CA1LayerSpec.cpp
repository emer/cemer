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

#include "CA1LayerSpec.h"
#include <LeabraNetwork>
#include <CA3LayerSpec>
#include <ECinLayerSpec>

void CA1LayerSpec::Initialize() {
  recall_decay = 1.0f;
  use_test_mode = true;
}

bool CA1LayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

  bool rval = true;

  int in_prjn_idx;
  LeabraLayer* in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_CA3LayerSpec);
  if(lay->CheckError(!in_lay, quiet, rval,
                "no projection from CA3 Layer found: must recv from layer with CA3LayerSpec!")) {
    return false;
  }
  in_lay = FindLayerFmSpec(lay, in_prjn_idx, &TA_ECinLayerSpec);
  if(lay->CheckError(!in_lay, quiet, rval,
                "no projection from EC_in Layer found: must recv from layer with ECinLayerSpec!")) {
    return false;
  }

  return true;
}

void CA1LayerSpec::ModulateCA3Prjn(LeabraLayer* lay, LeabraNetwork* net, bool ca3_on) {
  int ca3_prjn_idx;
  LeabraLayer* ca3_lay = FindLayerFmSpec(lay, ca3_prjn_idx, &TA_CA3LayerSpec);
  if(!ca3_lay) return;
//  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    if(ca3_on) {
      u->Compute_NetinScale(net,0);
    }
    else {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.SafeEl(ca3_prjn_idx);
    if(recv_gp)
      recv_gp->scale_eff = 0.0f;
    }
  }

  // re-trigger netinput sending for all senders
  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, ca3_lay->units) {
    if(u->lesioned()) continue;
    u->Compute_NetinScale_Senders(net,-1);
  }
}

void CA1LayerSpec::ModulateECinPrjn(LeabraLayer* lay, LeabraNetwork* net, bool ecin_on) {
  int ecin_prjn_idx;
  LeabraLayer* ecin_lay = FindLayerFmSpec(lay, ecin_prjn_idx, &TA_ECinLayerSpec);
  if(!ecin_lay) return;
//  LeabraUnitSpec* rus = (LeabraUnitSpec*)lay->GetUnitSpec();

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
    if(u->lesioned()) continue;
    if(ecin_on) {
      u->Compute_NetinScale(net,0);
    }
    else {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.SafeEl(ecin_prjn_idx);
      if(recv_gp) {
	recv_gp->scale_eff = 0.0f;
      }
    }
  }

  FOREACH_ELEM_IN_GROUP(LeabraUnit, u, ecin_lay->units) {
    if(u->lesioned()) continue;
    u->Compute_NetinScale_Senders(net,-1);
  }
}

void CA1LayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  // always off at start of minus and plus phase
  ModulateCA3Prjn(lay, net, false); // turn off ca3 in minus phase until further notice
  if(net->phase == LeabraNetwork::PLUS_PHASE)
    lay->DecayState(net, recall_decay); // decay at start of plus phase too
  inherited::Settle_Init_Layer(lay, net);
}

void CA1LayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->ct_cycle == auto_m_cycles)
    RecordActM2(lay,net);
  if(net->ct_cycle == auto_m_cycles+1) {
    if(!(use_test_mode && net->train_mode == Network::TEST))
      lay->DecayState(net, recall_decay); // specifically CA1 activations at recall
    ModulateCA3Prjn(lay, net, true);    // turn on ca3 -- calls netinscale
    if(!(use_test_mode && net->train_mode == Network::TEST))
      ModulateECinPrjn(lay, net, false); // turn off ecin -- must be after ca3 to specifically turn off

    net->init_netins_cycle_stat = true; // call net->Init_Netins() when done..
  }
  inherited::Compute_CycleStats(lay, net);
}

