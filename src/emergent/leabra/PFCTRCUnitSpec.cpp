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

#include "PFCTRCUnitSpec.h"

#include <LeabraNetwork>

TA_BASEFUNS_CTORS_DEFN(PFCTRCUnitSpec);

void PFCTRCUnitSpec::Initialize() {
  
}

void PFCTRCUnitSpec::Defaults_init() {
  
}

void PFCTRCUnitSpec::Compute_NetinInteg(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) {
  if(net->deep.mod_net) {       // if anyone's doing it, we need to integrate!
    DeepModNetin_Integ(u, net, thr_no);
  }
  
  LeabraLayer* lay = (LeabraLayer*)u->Un(net, thr_no)->own_lay();
  if(lay->hard_clamped) {
    TestWrite(u->thal, 0.0f);     // reset here before thalamic writing
    return;
  }

  Compute_NetinRaw(u, net, thr_no);
  // u->net_raw and u->gi_syn now have proper values integrated from deltas

  float net_syn = u->net_raw;
  float net_ex = 0.0f;
  if(deep.IsTRC()) {            // should ONLY be this guy!
    if(u->deep_mod_net > 0.1f) {    // in maintenance mode -- IGNORE all inputs!
      net_syn = 0.0f;
    }
    else {
      if(Quarter_DeepRawNow(net->quarter)) {
        LeabraUnit* un = (LeabraUnit*)u->Un(net, thr_no);
        LeabraUnGpData* ugd = lay->UnGpDataUn(un);
        if(deep.trc_p_only_m) { // todo could move this option into this class..
          if(ugd->acts_m.max > 0.1f) {  // only activate if we got minus phase activation!
            net_syn = u->deep_raw_net; // only gets from deep!  and no extras!
          }
        }
        else {                       // always do it
          net_syn = u->deep_raw_net; // only gets from deep!  and no extras!
        }
      }
    }
  }
  else {
    net_ex = Compute_NetinExtras(u, net, thr_no, net_syn);  // this could modify net_syn if it wants..
  }
  
  float net_tot = net_syn + net_ex;

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = MAX(net_tot, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u, net, thr_no);
  }
  else {
    u->net += dt.integ * dt.net_dt * (net_tot - u->net);
    if(u->net < 0.0f) u->net = 0.0f; // negative netin doesn't make any sense
  }

  // add after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE)) {
    u->net += Compute_Noise(u, net, thr_no);
  }
  else if((noise_type == NET_MULT_NOISE) && (noise.type != Random::NONE)) {
    u->net *= Compute_Noise(u, net, thr_no);
  }

  TestWrite(u->thal, 0.0f);     // reset here before thalamic writing
}

