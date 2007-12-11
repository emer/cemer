// Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "leabra_ct.h"

// #include "netstru_extra.h"

#include <math.h>
#include <limits.h>
#include <float.h>

//////////////////////////////////
// 	Ct cons
//////////////////////////////////

void CtLeabraConSpec::Initialize() {
  min_obj_type = &TA_CtLeabraCon;
  avg_srp_dwt = false;
  savg_cor.thresh = -1.0f;
}

void CtLeabraRecvCons::Initialize() {
  SetConType(&TA_CtLeabraCon);
}

void CtLeabraBiasSpec::Initialize() {
  //  min_obj_type = &TA_CtLeabraCon;
}

//////////////////////////////////
// 	Ct Unit
//////////////////////////////////

void CtLeabraUnit::Initialize() {
  bias.con_type = &TA_CtLeabraCon;
}

void CtLeabraUnitSpec::Initialize() {
  min_obj_type = &TA_CtLeabraUnit;
  bias_con_type = &TA_CtLeabraCon;
  opt_thresh.learn = -1.0f;
  opt_thresh.updt_wts = false;
}

void CtLeabraUnitSpec::Compute_CtCycle(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork* net) {
  for(int g=0; g<u->recv.size; g++) {
    CtLeabraRecvCons* recv_gp = (CtLeabraRecvCons*)u->recv.FastEl(g);
    if(recv_gp->prjn->from->lesioned() || !recv_gp->cons.size) continue;
    recv_gp->Compute_CtCycle(u);
  }
  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_CtCycle((CtLeabraCon*)u->bias.Cn(0), u);
}

void CtLeabraUnitSpec::Compute_ActMP(CtLeabraUnit* u, CtLeabraLayer*, CtLeabraNetwork*) {
  u->p_act_m = u->p_act_p;	// update activations for learning
  u->p_act_p = u->act_eq;
}

void CtLeabraUnitSpec::Compute_dWt_impl(LeabraUnit* u, LeabraLayer* lay, LeabraNetwork* net) {
  // don't adapt bias weights on clamped inputs..: why?  what possible consequence could it have!?
  // furthermore: it is not right for units that are clamped in 2nd plus phase!
  //  if(!((u->ext_flag & Unit::EXT) && !(u->ext_flag & Unit::TARG))) {

  float srp_norm = ((CtLeabraNetwork*)net)->srp_norm;
  ((CtLeabraBiasSpec*)bias_spec.SPtr())->B_Compute_dWt_Ct((CtLeabraCon*)u->bias.Cn(0), u, srp_norm);
    //  }
  UnitSpec::Compute_dWt(u);
}

//////////////////////////////////
// 	Ct Layer
//////////////////////////////////

void CtLeabraLayer::Initialize() {
  spec.SetBaseType(&TA_CtLeabraLayerSpec);
  units.SetBaseType(&TA_CtLeabraUnit);
  unit_spec.SetBaseType(&TA_CtLeabraUnitSpec);
}  

void CtLeabraLayerSpec::Initialize() {
  min_obj_type = &TA_CtLeabraLayer;
  decay.event = 0.0f;
  decay.phase = 0.0f;
}

void CtLeabraLayerSpec::Compute_CtCycle(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  if(lay->hard_clamped) return;

  CtLeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
    u->Compute_CtCycle(lay, net);
  }
}

void CtLeabraLayerSpec::Compute_ActMP(CtLeabraLayer* lay, CtLeabraNetwork* net) {
  CtLeabraUnit* u;
  taLeafItr i;
  FOR_ITR_EL(CtLeabraUnit, u, lay->units., i) {
    u->Compute_ActMP(lay, net);
  }
}


//////////////////////////////////
// 	Ct Network
//////////////////////////////////

void CtLeabraNetwork::Initialize() {
  layers.SetBaseType(&TA_CtLeabraLayer);
  cycles_per_tick = 10;
  srp_norm = 1.0f / (float)cycles_per_tick;
}

void CtLeabraNetwork::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  srp_norm = 1.0f / (float)cycles_per_tick;
}

void CtLeabraNetwork::SetProjectionDefaultTypes(Projection* prjn) {
  inherited::SetProjectionDefaultTypes(prjn);
  prjn->con_type = &TA_CtLeabraCon;
  prjn->recvcons_type = &TA_CtLeabraRecvCons;
  prjn->sendcons_type = &TA_LeabraSendCons;
  prjn->con_spec.SetBaseType(&TA_CtLeabraConSpec);
}

void CtLeabraNetwork::Compute_CtCycle() {
  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_CtCycle(this);
  }
}
  
void CtLeabraNetwork::Cycle_Run() {
  inherited::Cycle_Run();
  Compute_CtCycle();
  // program code needs to do this after incrementing time++:
//   if(time % cycles_per_tick == 0) {
//     tick++;
//     Compute_dWt();
//   }
}

void CtLeabraNetwork::Compute_ActMP() {
  CtLeabraLayer* lay;
  taLeafItr l;
  FOR_ITR_EL(CtLeabraLayer, lay, layers., l) {
    if(lay->lesioned())	continue;
    lay->Compute_ActMP(this);
  }
}
  

////////////////////////////////////////////////
//		TODO

// 1. Need CtLeabraPrjn or something to get types for prjns -- is this in network?
//    auto-creating didn't work!
// 2. can't set the CtLeabraBiasSpec in UnitSpec.
