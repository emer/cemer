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

#include "TDDeltaUnitSpec.h"
#include <LeabraNetwork>
#include <TDRewIntegUnitSpec>
#include <OneToOnePrjnSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(TDDeltaUnitSpec);


void TDDeltaUnitSpec::Initialize() {
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
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative td = negative activation signal here";
  taMisc::Confirm(help);
}

// bool TDDeltaUnitSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
//   LeabraLayer* lay = (LeabraLayer*)ly;
//   if(!inherited::CheckConfig_Layer(lay, quiet)) return false;

//   LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
//   bool rval = true;

//   // must have the appropriate ranges for unit specs..
//   LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
//   if(lay->CheckError((us->act_range.max != 2.0f) || (us->act_range.min != -2.0f), quiet, rval,
//                 "requires UnitSpec act_range.max = 2, min = -2, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)") ) {
//     us->SetUnique("act_range", true);
//     us->act_range.max = 2.0f;
//     us->act_range.min = -2.0f;
//     us->act_range.UpdateAfterEdit();
//   }
//   if(lay->CheckError((us->clamp_range.max != 2.0f) || (us->clamp_range.min != -2.0f), quiet, rval,
//                 "requires UnitSpec clamp_range.max = 2, min = -2, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
//     us->SetUnique("clamp_range", true);
//     us->clamp_range.max = 2.0f;
//     us->clamp_range.min = -2.0f;
//     us->clamp_range.UpdateAfterEdit();
//   }

//   // check recv connection
//   if(lay->units.leaves == 0) return false;
//   LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
//   LeabraLayer* rewinteg_lay = NULL;
//   for(int g=0; g<u->recv.size; g++) {
//     LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
//     if(recv_gp->NotActive()) continue;
//     LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
//     if(recv_gp->GetConSpec()->IsMarkerCon()
//         && fmlay->spec.SPtr()->InheritsFrom(TA_TDRewIntegUnitSpec)) {
//       rewinteg_lay = fmlay;
//       // if(lay->CheckError(recv_gp->size <= 0, quiet, rval,
//       //                    "requires one recv projection with at least one unit!")) {
//       //   return false;
//       // }
//       if(recv_gp->size <= 0) {
//         OneToOnePrjnSpec* pspec = (OneToOnePrjnSpec*)recv_gp->prjn->spec.SPtr();
//         pspec->send_start = 1;
//         if(lay->CheckError(true, quiet, rval,
//                            "requires the OneToOnePrjnSpec to have send_start = 1 -- I just set this for you, but you will have to re-build the network and re-init -- save project after this change")) {
//         }
//       }
//       else {
//         if(lay->CheckError(!recv_gp->Un(0,net)->InheritsFrom(TA_LeabraTdUnit), quiet, rval,
//                            "I need to receive from a LeabraTdUnit!")) {
//           return false;
//         }
//       }
//     }
//   }

//   if(lay->CheckError(rewinteg_lay == NULL, quiet, rval,
//                 "did not find TDRewInteg layer to get Td from!")) {
//     return false;
//   }

//   int myidx = lay->own_net->layers.FindLeafEl(lay);
//   int rpidx = lay->own_net->layers.FindLeafEl(rewinteg_lay);
//   if(lay->CheckError(rpidx > myidx, quiet, rval,
//                 "reward integration layer must be *before* this layer in list of layers -- it is now after, won't work")) {
//     return false;
//   }

//   // check sending layer projections for appropriate unit types
//   int si;
//   for(si=0;si<lay->send_prjns.size;si++) {
//     Projection* prjn = (Projection*)lay->send_prjns[si];
//     if(lay->CheckError(!prjn->from->units.el_typ->InheritsFrom(TA_LeabraTdUnit), quiet, rval,
//                   "all layers I send to must have LeabraTdUnits!, layer:",
//                   prjn->from->GetPath(),"doesn't")) {
//       return false;
//     }
//   }
//   return true;
// }

void TDDeltaUnitSpec::Compute_TD(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->phase == LeabraNetwork::MINUS_PHASE) {
    u->da_p = 0.0f;
    u->ext = u->da_p;
    u->act_eq = u->act_nd = u->act = u->net = u->ext;
    return;
  }
  float rew_integ_minus = 0.0f;
  float rew_integ_cur = 0.0f;
  const int nrg = u->NRecvConGps(net, thr_no); 
  for(int g=0; g< nrg; g++) {
    LeabraConGroup* recv_gp = (LeabraConGroup*)u->RecvConGroup(net, thr_no, g);
    if(recv_gp->NotActive()) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    LeabraLayer* fmlay = (LeabraLayer*)recv_gp->prjn->from.ptr();
    LeabraUnitSpec* us = (LeabraUnitSpec*)fmlay->GetUnitSpec();
    if(cs->IsMarkerCon()) {
      if(us->InheritsFrom(&TA_TDRewIntegUnitSpec)) {
        LeabraUnit* su = (LeabraUnit*)fmlay->units.SafeEl(0);
        rew_integ_minus = su->act_m();
        rew_integ_cur = su->act_eq();
        break;
      }
    }
  }
  float delta = rew_integ_cur - rew_integ_minus;
  u->da_p = delta;
  u->ext = u->da_p;
  u->act_eq = u->act_nd = u->act = u->net = u->ext;
}

void TDDeltaUnitSpec::Send_TD(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  float snd_val = u->act;
  const int nsg = u->NSendConGps(net, thr_no); 
  for(int g=0; g<nsg; g++) {
    LeabraConGroup* send_gp = (LeabraConGroup*)u->SendConGroup(net, thr_no, g);
    if(send_gp->NotActive()) continue;
    for(int j=0;j<send_gp->size; j++) {
      ((LeabraUnitVars*)send_gp->UnVars(j,net))->da_p = snd_val;
    }
  }
}

void TDDeltaUnitSpec::Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_TD(u, net, thr_no);
  Send_TD(u, net, thr_no);
}

void TDDeltaUnitSpec::Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  Compute_Act_Rate(u, net, thr_no);
}

