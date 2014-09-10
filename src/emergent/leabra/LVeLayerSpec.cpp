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

#include "LVeLayerSpec.h"
#include <LeabraNetwork>
#include <MarkerConSpec>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(LViLayerSpec);

TA_BASEFUNS_CTORS_DEFN(LVeLayerSpec);

TA_BASEFUNS_CTORS_DEFN(LVMiscSpec);

void LVMiscSpec::Initialize() {
  gd_pvlv = true;
  min_lvi = 0.1f;
  prior_gain = 1.0f;
  er_reset_prior = true;
  pos_y_dot_only = false;

  if(taMisc::is_loading) {
    taVersion v636(6, 3, 6);
    if(taMisc::loading_version < v636) { // default prior to 636 is off
      gd_pvlv = false;
    }
  }
}

void LVeLayerSpec::Initialize() {
  // SetUnique("decay", true);
  decay.phase = 0.0f;

  bias_val.un = ScalarValBias::NO_UN;
  bias_val.val = 0.5f;
}

void LVeLayerSpec::HelpConfig() {
  String help = "LVeLayerSpec Learned Value computation (excitatory/fast and inhibitory/slow):\n\
 Learns values (LV) according to the PVLV algorithm: looks at current network state\
 and computes how much it resembles states that have been associated with primary value (PV) in the past\n\
 - Activation is always learned values\n\
 - At very end of trial, training value is clamped onto unit act_p values to provide training signal:\n\
 - (training only occurs when primary reward is detected by PVr as either present or expected\n\
 - Learning is (ru->act_p - ru->act_m) * su->act_p: delta on recv units times sender activations.\n\
 \nLVeLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons from relevant network state layers (must be LVConSpec)\n\
 - [Optional: Marker recv con from PVi layer -- for display only\n\
 - Sending cons to Da/SNc layers\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool LVeLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  if(!inherited::CheckConfig_Layer(lay, quiet))
    return false;

//  LeabraNetwork* net = (LeabraNetwork*)lay->own_net;
  bool rval = true;

  decay.phase = 0.0f;

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();

  us->UpdateAfterEdit();

  if(lay->CheckError(lay->units.leaves == 0, quiet, rval,
                     "requires at least one unit in the layer"))
    return false;

  // note: increased flexibility here: we don't check for things we don't actually depend
  // on -- allows more modular re-use of elements

  return true;
}

void LVeLayerSpec::Init_Weights_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights_Layer(lay, net);
  Layer::AccessMode acc_md = Layer::ACC_GP;
  if(lv.gd_pvlv) {  // set as though we just had a primrary reward
    UNIT_GP_ITR
      (lay,
       LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
       u->misc_1 = -1;
     );
  }
}

void LVeLayerSpec::Compute_LVPlusPhaseDwt(LeabraLayer* lay, LeabraNetwork* net) {
  Layer::AccessMode acc_md = Layer::ACC_GP;
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  float pve_val = net->norew_val;
  if(net->ext_rew_avail) {
    pve_val = net->ext_rew;
  }

  if(er_avail) {
    UNIT_GP_ITR
      (lay,
       LeabraUnit* u = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
       u->ext = pve_val;
       if(lv.gd_pvlv) {
         u->dav = net->pvlv_dav; // update our dav so we can use da modulated learning!
       }
       ClampValue_ugp(lay, acc_md, gpidx, net);                 // apply new value
       Compute_ExtToPlus_ugp(lay, acc_md, gpidx, net);  // copy ext values to act_p
     );
  }
}

float LVeLayerSpec::Compute_LVDa_ugp(LeabraLayer* lve_lay, LeabraLayer* lvi_lay,
                                     Layer::AccessMode lve_acc_md, int lve_gpidx,
                                     Layer::AccessMode lvi_acc_md, int lvi_gpidx,
                                     LeabraNetwork* net) {
  LeabraUnit* lveu = (LeabraUnit*)lve_lay->UnitAccess(lve_acc_md, 0, lve_gpidx);
  float lv_da = 0.0f;
  if(lv.gd_pvlv) {
    if(lveu->misc_1 >= 0.0f)               // if -1, then trial after er..
      lv_da = lveu->act_eq - lveu->misc_1; // straight y-dot

    int nunits = lve_lay->UnitAccess_NUnits(lve_acc_md);
    for(int i=0;i<nunits;i++) {
      LeabraUnit* du = (LeabraUnit*)lve_lay->UnitAccess(lve_acc_md, i, lve_gpidx);
      if(du->lesioned()) continue;
      du->dav = lv_da;
    }
  }
  else {
    LeabraUnit* lviu = (LeabraUnit*)lvi_lay->UnitAccess(lvi_acc_md, 0, lvi_gpidx);

    float lvd = lveu->act_eq - MAX(lviu->act_eq, lv.min_lvi);
  
    lv_da = lvd - lveu->misc_1;

    int nunits = lve_lay->UnitAccess_NUnits(lve_acc_md);
    for(int i=0;i<nunits;i++) {
      LeabraUnit* du = (LeabraUnit*)lve_lay->UnitAccess(lve_acc_md, i, lve_gpidx);
      if(du->lesioned()) continue;
      du->dav = lvd;              // store in all units for visualization and prior update (NOT lv_da which already has misc1 subtracted!)
    }
  }
  if(lv_da < 0.0f && lv.pos_y_dot_only) lv_da = 0.0f; // positive rectify if user-selected
  return lv_da;
}

float LVeLayerSpec::Compute_LVDa(LeabraLayer* lve_lay, LeabraLayer* lvi_lay,
                                 LeabraNetwork* net) {
  float lv_da = 0.0f;
  if(lv.gd_pvlv) {
    // ignore lvi -- just two diff cases for lve unit groups or not
    if(lve_lay->gp_geom.n > 1) {
      for(int g=0; g<lve_lay->gp_geom.n; g++) {
        lv_da += Compute_LVDa_ugp(lve_lay, lvi_lay, Layer::ACC_GP, g,
                                  Layer::ACC_LAY, 0, net);
      }
      lv_da /= (float)lve_lay->gp_geom.n; // average!
    }
    else {
      lv_da = Compute_LVDa_ugp(lve_lay, lvi_lay, Layer::ACC_LAY, 0,
                               Layer::ACC_LAY, 0, net);
    }
  }
  else {
    if(lve_lay->unit_groups && (lve_lay->gp_geom.n == lvi_lay->gp_geom.n)) {
      for(int g=0; g<lve_lay->gp_geom.n; g++) {
        lv_da += Compute_LVDa_ugp(lve_lay, lvi_lay, Layer::ACC_GP, g,
                                  Layer::ACC_GP, g, net);
      }
      lv_da /= (float)lve_lay->gp_geom.n; // average!
    }
    else if(lve_lay->gp_geom.n > 1) {
      // one lvi and multiple lve's
      for(int g=0; g<lve_lay->gp_geom.n; g++) {
        lv_da += Compute_LVDa_ugp(lve_lay, lvi_lay, Layer::ACC_GP, g,
                                  Layer::ACC_LAY, 0, net);
      }
      lv_da /= (float)lve_lay->gp_geom.n; // average!
    }
    else if(lvi_lay->gp_geom.n > 1) {
      // one lve and multiple lvi's
      for(int g=0; g<lvi_lay->gp_geom.n; g++) {
        lv_da += Compute_LVDa_ugp(lve_lay, lvi_lay, Layer::ACC_LAY, 0,
                                  Layer::ACC_GP, g, net);
      }
      lv_da /= (float)lvi_lay->gp_geom.n; // average!
    }
    else {
      lv_da = Compute_LVDa_ugp(lve_lay, lvi_lay, Layer::ACC_LAY, 0,
                               Layer::ACC_LAY, 0, net);
    }
   }
   if(lv_da < 0.0f && lv.pos_y_dot_only) lv_da = 0.0f; // just to make sure!
   return lv_da;
}

void LVeLayerSpec::Update_LVPrior_ugp(LeabraLayer* lay, Layer::AccessMode acc_md,
                                      int gpidx, bool er_avail) {
  LeabraUnit* lveu = (LeabraUnit*)lay->UnitAccess(acc_md, 0, gpidx);
  if(er_avail && lv.er_reset_prior) {
    if(lv.gd_pvlv) {
      lveu->misc_1 = -1.0f;     // signal that it is an er trial
    }
    else {
      lveu->misc_1 = 0.0f;
    }
  }
  else {
    if(lv.gd_pvlv) {
      lveu->misc_1 = lv.prior_gain * lveu->act_eq;
    }
    else {
      lveu->misc_1 = lv.prior_gain * lveu->dav;
    }
  }
}

void LVeLayerSpec::Update_LVPrior(LeabraLayer* lay, LeabraNetwork* net) {
  bool er_avail = net->ext_rew_avail || net->pv_detected; // either is good
  UNIT_GP_ITR(lay, Update_LVPrior_ugp(lay, acc_md, gpidx, er_avail); );
}

void LVeLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  inherited::Compute_CycleStats(lay, net, thread_no);
  // take the 1st guy as the overall general guy
  LeabraUnit* lvesu = (LeabraUnit*)lay->units.Leaf(0);
  net->pvlv_lve = lvesu->act_eq;
  // this is primarily used for noise modulation
}

void LVeLayerSpec::PostSettle(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::PostSettle(lay, net);

  if(net->phase_no == net->phase_max-1) { // only at very end!
    Update_LVPrior(lay, net);
  }
}

void LVeLayerSpec::Compute_dWt_Layer_pre(LeabraLayer* lay, LeabraNetwork* net) {
  if(net->phase_no < net->phase_max-1)
    return;
  Compute_LVPlusPhaseDwt(lay, net);
}

bool LVeLayerSpec::Compute_dWt_Test(LeabraLayer* lay, LeabraNetwork* net) {
  if(!(net->ext_rew_avail || net->pv_detected))
    return false; // no learn on no rew
  return true;
}

/////////////
//     LViLayerSpec -- obsolete

void LViLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net, int thread_no) {
  inherited::Compute_CycleStats(lay, net, thread_no);
  // take the 1st guy as the overall general guy
  LeabraUnit* lvisu = (LeabraUnit*)lay->units.Leaf(0);
  net->pvlv_lvi = lvisu->act_eq;
  // this is primarily used for noise modulation
}

