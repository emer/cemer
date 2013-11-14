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

#include "LHbRMTgUnitSpec.h"

#include <LeabraNetwork>

#include <taMisc>

void LHbRMTgUnitSpec::Initialize() {
  vs_matrix_gain = 1.0f;
}

void LHbRMTgUnitSpec::Defaults_init() {

}

void LHbRMTgUnitSpec::HelpConfig() {
  String help = "LHbRMTgUnitSpec (DA value) Computation:\n\
 - Computes DA dips based on inputs from VS Matrix, Patch Direct and Indirect, and PV Pos & Neg.\n\
 \nLHbRMTgUnitSpec Configuration:\n\
 - Use the Wizard gdPVLV button to automatically configure layers.\n\
 - Recv cons should be standard, with no learning";
  taMisc::Confirm(help);
}

bool LHbRMTgUnitSpec::CheckConfig_Unit(Unit* un, bool quiet) {
  LeabraUnit* u = (LeabraUnit*)un;
  if(!inherited::CheckConfig_Unit(un, quiet)) return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  bool patch_dir = false;
  bool patch_ind = false;
  bool matrix_dir = false;
  bool matrix_ind = false;
  bool pv_pos = false;
  bool pv_neg = false;
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

    if(from->name.contains("Patch")) {
      if(from->name.contains("Ind")) {
        patch_ind = true;
      }
      else {
        patch_dir = true;
      }
    }
    else if(from->name.contains("Matrix")) {
      if(from->name.contains("Ind") || from->name.contains("NoGo")) {
        matrix_ind = true;
      }
      else {
        matrix_dir = true;
      }
    }
    else if(from->name.contains("PosPV")) {
      pv_pos = true;
    }
    else if(from->name.contains("NegPV")) {
      pv_neg = true;
    }
  }

  if(u->CheckError(!patch_dir, quiet, rval,
                   "did not find VS Patch Direct recv projection -- searches for Patch and *not* Ind in layer name")) {
    rval = false;
  }
  if(u->CheckError(!patch_ind, quiet, rval,
                   "did not find VS Patch Indirect recv projection -- searches for Patch and Ind in layer name")) {
    rval = false;
  }
  // matrix is optional
  // if(u->CheckError(!matrix_dir, quiet, rval,
  //                  "did not find VS Matrix Direct recv projection -- searches for Matrix and *not* Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  // if(u->CheckError(!matrix_ind, quiet, rval,
  //                  "did not find VS Matrix Indirect recv projection -- searches for Matrix and Ind or NoGo in layer name")) {
  //   rval = false;
  // }
  if(u->CheckError(!pv_pos, quiet, rval,
                   "did not find PV Positive recv projection -- searches for PosPV in layer name")) {
    rval = false;
  }
  if(u->CheckError(!pv_neg, quiet, rval,
                   "did not find PV Negative recv projection -- searches for NegPV in layer name")) {
    rval = false;
  }

  return rval;
}


void LHbRMTgUnitSpec::Compute_NetinInteg(LeabraUnit* u, LeabraNetwork* net, int thread_no) {
  LeabraLayer* lay = u->own_lay();
  if(lay->hard_clamped) return;
  LeabraLayerSpec* ls = (LeabraLayerSpec*)lay->GetLayerSpec();

  if(net->NetinPerPrjn()) {
    float patch_dir = 0.0f;
    float patch_ind = 0.0f;
    float matrix_dir = 0.0f;
    float matrix_ind = 0.0f;
    float pv_pos = 0.0f;
    float pv_neg = 0.0f;
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();

      recv_gp->net_raw += recv_gp->net_delta;
      recv_gp->net_delta = 0.0f; // clear for next use

      if(from->name.contains("Patch")) {
        if(from->name.contains("Ind")) {
          patch_ind += recv_gp->net_raw;
        }
        else {
          patch_dir += recv_gp->net_raw;
        }
      }
      else if(from->name.contains("Matrix")) {
        if(from->name.contains("Ind") || from->name.contains("NoGo")) {
          matrix_ind += recv_gp->net_raw;
        }
        else {
          matrix_dir += recv_gp->net_raw;
        }
      }
      else if(from->name.contains("PosPV")) {
        pv_pos += recv_gp->net_raw;
      }
      else if(from->name.contains("NegPV")) {
        pv_neg += recv_gp->net_raw;
      }
    }

    // todo: do shunting separately in the VTA layer itself, with direct prjns

    // now do the proper subtractions, and individually rectify each term
    // this individual rectification is important so that system is not 
    // sensitive to overshoot of predictor relative to its comparison value
    float matrix_net = vs_matrix_gain * (matrix_ind - matrix_dir); // net positive dipping action from matrix
    matrix_net = MAX(0.0f, matrix_net);

    float pv_neg_net = pv_neg - patch_dir; // dir cancels neg
    pv_neg_net = MAX(0.0f, pv_neg_net);

    float pv_pos_net = patch_ind - pv_pos; // ind cancels pos
    pv_pos_net = MAX(0.0f, pv_pos_net);

    u->net_raw = matrix_net + pv_neg_net + pv_pos_net;
  }

  // the rest of this should all be standard..

  if(net->inhib_cons_used) {
    u->g_i_raw += u->g_i_delta;
    if(act_fun == SPIKE) {
      u->g_i_syn = MAX(u->g_i_syn, 0.0f);
      Compute_NetinInteg_Spike_i(u, net);
    }
    else {
      u->g_i_syn += dt.net * (u->g_i_raw - u->g_i_syn);
      u->g_i_syn = MAX(u->g_i_syn, 0.0f); // negative netin doesn't make any sense
    }
  }
  else {
    // clear so automatic inhibition can add to these values!
    u->g_i_syn = 0.0f;
    u->g_i_raw = 0.0f;
  }
  
  float tot_net = (u->bias_scale * u->bias.OwnCn(0,LeabraConSpec::WT)) + u->net_raw;

  if(u->HasExtFlag(Unit::EXT)) {
    tot_net += u->ext * ls->clamp.gain;
  }
  else if(u->HasExtFlag(Unit::TARG)) {
    tot_net += u->targ * ls->clamp.minus_targ_gain;
  }

  if(net->ti_mode) {
    tot_net += u->act_ctxt;
  }

  u->net_delta = 0.0f;  // clear for next use
  u->g_i_delta = 0.0f;  // clear for next use

  if(act_fun == SPIKE) {
    // todo: need a mech for inhib spiking
    u->net = MAX(tot_net, 0.0f); // store directly for integration
    Compute_NetinInteg_Spike_e(u, net);
  }
  else {
    u->net += dt.net * (tot_net - u->net);
    u->net = MAX(u->net, 0.0f); // negative netin doesn't make any sense
  }

  // add just before computing i_thr -- after all the other stuff is done..
  if((noise_type == NETIN_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    u->net += Compute_Noise(u, net);
  }
  u->i_thr = Compute_IThresh(u, net);
}

