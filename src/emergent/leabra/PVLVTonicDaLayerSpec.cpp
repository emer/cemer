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

#include "PVLVTonicDaLayerSpec.h"
#include <LeabraNetwork>
#include <LeabraTdUnit>
#include <ExtRewLayerSpec>

#include <taMisc>


void PVLVTonicDaLayerSpec::Initialize() {
  SetUnique("decay", true);
  decay.event = 0.0f;
  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 3;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_INHIB;
  inhib.kwta_pt = 0.25f;
  SetUnique("ct_inhib_mod", true);
  ct_inhib_mod.use_sin = true;
  ct_inhib_mod.burst_i = 0.0f;
  ct_inhib_mod.trough_i = 0.0f;
  SetUnique("scalar", true);
  scalar.rep = ScalarValSpec::GAUSSIAN;
  scalar.un_width = 0.2f;
  scalar.norm_width = true;
  SetUnique("unit_range", true);
  unit_range.min = -0.2f;
  unit_range.max = 1.2f;
}

void PVLVTonicDaLayerSpec::HelpConfig() {
  String help = "PVLVTonicDaLayerSpec (DA value) Computation:\n\
 - Computes DA value based on inputs from PVLV layers.\n\
 - No Learning\n\
 \nPVLVTonicDaLayerSpec Configuration:\n\
 - Use the Wizard BG_PFC button to automatically configure BG_PFC layers.\n\
 - Recv cons marked with a MarkerConSpec from PVLV\n\
 - This layer must be after recv layers in list of layers\n\
 - UnitSpec for this layer must have act_range and clamp_range set to -1 and 1 \
     (because negative da = negative activation signal here";
  taMisc::Confirm(help);
}

bool PVLVTonicDaLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
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
  if(lay->CheckError(us->act.avg_dt != 0.0f, quiet, rval,
                "requires UnitSpec act.avg_dt = 0, I just set it for you in spec:",
                us->name,"(make sure this is appropriate for all layers that use this spec!)")) {
    us->SetUnique("act", true);
    us->act.avg_dt = 0.0f;
  }

  return true;
}

void PVLVTonicDaLayerSpec::Clamp_Da(LeabraLayer* lay, LeabraNetwork* net) {
  lay->SetExtFlag(Unit::EXT);
  UNIT_GP_ITR(lay,
              LeabraTdUnit* u = (LeabraTdUnit*)lay->UnitAccess(acc_md, 0, gpidx);
              u->ext = net->pvlv_tonic_da;
              ClampValue_ugp(lay, acc_md, gpidx, net);
              );
  HardClampExt(lay, net);
}

void PVLVTonicDaLayerSpec::BuildUnits_Threads(LeabraLayer* lay, LeabraNetwork* net) {
  // that's it: don't do any processing on this layer: set all idx to 0
  lay->units_flat_idx = 0;
  FOREACH_ELEM_IN_GROUP(Unit, un, lay->units) {
    if(un->lesioned()) continue;
    un->flat_idx = 0;
  }
}

void PVLVTonicDaLayerSpec::Compute_ApplyInhib(LeabraLayer* lay, LeabraNetwork* net) {
  Clamp_Da(lay, net);
}
