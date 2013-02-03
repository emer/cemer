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

#include "PFCsUnitSpec.h"
#include <LeabraNetwork>
#include <PFCLayerSpec>
#include <PBWMUnGpData>

void PFCsUnitSpec::Initialize() {
  Defaults_init();
}

void PFCsUnitSpec::Defaults_init() {
  g_bar.h = 0.0f;		// don't use it by default -- use mix
}

// this sets default wt scale from deep prjns to super to 0 unless fired go
void PFCsUnitSpec::Compute_NetinScale(LeabraUnit* u, LeabraNetwork* net) {
  inherited::Compute_NetinScale(u, net);

  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return;
  if(!rlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) return;
  PFCLayerSpec* ls = (PFCLayerSpec*) rlay->GetLayerSpec();
  if(ls->pfc_layer != PFCLayerSpec::SUPER) return; // only for supers

  int rgpidx = u->UnitGpIdx();
  PBWMUnGpData* gpd = (PBWMUnGpData*)rlay->ungp_data.FastEl(rgpidx);
  if(gpd->go_fired_trial) return; // only if didn't fire go

  // didn't fire: zero it out
  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    LeabraLayer* from = (LeabraLayer*) recv_gp->prjn->from.ptr();
    if(from->lesioned() || !recv_gp->size)       continue;

    if(from->GetLayerSpec()->GetTypeDef() == &TA_PFCLayerSpec) {
      PFCLayerSpec* fmls = (PFCLayerSpec*)from->GetLayerSpec();
      if(fmls->pfc_layer == PFCLayerSpec::DEEP) {
	recv_gp->scale_eff = 0.0f; // negate!!
      }
    }
  }
}

void PFCsUnitSpec::Compute_ActFmVm_rate(LeabraUnit* u, LeabraNetwork* net) {
  LeabraLayer* rlay = u->own_lay();
  if(rlay->lesioned()) return;
  if(!rlay->GetLayerSpec()->InheritsFrom(&TA_PFCLayerSpec)) return;
  PFCLayerSpec* ls = (PFCLayerSpec*) rlay->GetLayerSpec();
  int rgpidx = u->UnitGpIdx();
  PBWMUnGpData* gpd = (PBWMUnGpData*)rlay->ungp_data.FastEl(rgpidx);

  float new_act;
  if(act.gelin) {
    if(act.old_gelin) {
      float g_e_val = u->net;
      float vm_eq = act.vm_mod_max * (Compute_EqVm(u) - v_m_init.mean); // relative to starting!
      if(vm_eq > 0.0f) {
	float vmrat = (u->v_m - v_m_init.mean) / vm_eq;
	if(vmrat > 1.0f) vmrat = 1.0f;
	else if(vmrat < 0.0f) vmrat = 0.0f;
	g_e_val *= vmrat;
      }
      float g_e_thr = Compute_EThresh(u);
      new_act = Compute_ActValFmVmVal_rate(g_e_val - g_e_thr);
      // NEW CODE: maintaining
      if(gpd->mnt_count > 0) {
	u->misc_1 = new_act;
	new_act = ls->gate.maint_pct * u->maint_h + ls->gate.maint_pct_c * new_act;
      }
      else {
	u->misc_1 = 0.0f;
      }

    }
    else {			// new gelin
      if(u->v_m <= act.thr) {
	new_act = Compute_ActValFmVmVal_rate(u->v_m - act.thr);
      }
      else {
	float g_e_thr = Compute_EThresh(u);
	new_act = Compute_ActValFmVmVal_rate(u->net - g_e_thr);
      }
      // NEW CODE: maintaining
      if(gpd->mnt_count > 0) {
	u->misc_1 = new_act;
	new_act = ls->gate.maint_pct * u->maint_h + ls->gate.maint_pct_c * new_act;
      }
      else {
	u->misc_1 = 0.0f;
      }
	
      if(net->cycle < dt.vm_eq_cyc) {
	new_act = u->act_nd + dt.vm_eq_dt * (new_act - u->act_nd); // eq dt
      }
      else {
	new_act = u->act_nd + dt.vm * (new_act - u->act_nd); // time integral with dt.vm  -- use nd to avoid synd problems
      }
    }
  }
  else {
    new_act = Compute_ActValFmVmVal_rate(u->v_m - act.thr);
    // NEW CODE: maintaining
    if(gpd->mnt_count > 0) {
      u->misc_1 = new_act;
      new_act = ls->gate.maint_pct * u->maint_h + ls->gate.maint_pct_c * new_act;
    }
    else {
      u->misc_1 = 0.0f;
    }
  }
  if(depress.on) {                   // synaptic depression
    u->act_nd = act_range.Clip(new_act); // nd is non-discounted activation!!! solves tons of probs
    new_act *= MIN(u->spk_amp, 1.0f);
    if((net->ct_cycle+1) % depress.interval == 0) {
      u->spk_amp += -new_act * depress.depl + (depress.max_amp - u->spk_amp) * depress.rec;
    }
    if(u->spk_amp < 0.0f)                       u->spk_amp = 0.0f;
    else if(u->spk_amp > depress.max_amp)       u->spk_amp = depress.max_amp;
  }

  u->da = new_act - u->act;
  if((noise_type == ACT_NOISE) && (noise.type != Random::NONE) && (net->cycle >= 0)) {
    new_act += Compute_Noise(u, net);
  }
  u->act = act_range.Clip(new_act);
  u->act_eq = u->act;
  if(!depress.on)
    u->act_nd = u->act_eq;
}
