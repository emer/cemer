// Copyright 2017, Regents of the University of Colorado,
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

#include "LeabraExtraLayerSpecs.h"


#include <LeabraNetwork>
//#include <taProject>
#include <taMisc>

#include <LeabraConSpec_cpp>
#include <LeabraUnitSpec_cpp>
#include <LeabraLayerSpec_cpp>
#include <ProjectionSpec_cpp>

#include <State_main>

#include <LeabraUnitSpec>
#include <LeabraConSpec>

//#include <LeabraExtraUnitSpecs_cpp> // cpp versions too

#include <State_main>

#include "ScalarValLayerSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(ScalarValBias);
TA_BASEFUNS_CTORS_DEFN(ScalarValSpec);
TA_BASEFUNS_CTORS_DEFN(ScalarValLayerSpec);

void ScalarValLayerSpec::Initialize() {
  unit_range.min = -0.5f;   unit_range.max = 1.5f;
  unit_range.UpdateAfterEdit_NoGui();
  avg_act_range.min = 0.1f; avg_act_range.max = 0.9f;
  avg_act_range.UpdateAfterEdit_NoGui();
  Defaults_init();
  Initialize_core();
}

void ScalarValLayerSpec::Defaults_init() {
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    // kwta.k_from = KWTASpec::USE_K;
    // kwta.k = 3;
    // gp_kwta.k_from = KWTASpec::USE_K;
    // gp_kwta.k = 3;
    lay_inhib.gi = 2.2f;
    unit_gp_inhib.gi = 2.2f;

    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else if(scalar.rep == ScalarValSpec::LOCALIST) {
    // kwta.k_from = KWTASpec::USE_K;
    // kwta.k = 1;
    // gp_kwta.k_from = KWTASpec::USE_K;
    // gp_kwta.k = 1;
    lay_inhib.gi = 2.2f;
    unit_gp_inhib.gi = 2.2f;

    val_range.min = unit_range.min;
    val_range.max = unit_range.max;

    scalar.min_sum_act = .2f;
  }
  else if(scalar.rep == ScalarValSpec::AVG_ACT) {
    scalar.min_sum_act = 0.0f;

    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
  }
  val_range.UpdateAfterEdit_NoGui();
}

void ScalarValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  unit_range.UpdateAfterEdit_NoGui();
  scalar.UpdateAfterEdit_NoGui();
  if(scalar.rep == ScalarValSpec::GAUSSIAN) {
    scalar.InitRange(unit_range.min, unit_range.range); // needed for un_width_eff
    val_range.min = unit_range.min + (.5f * scalar.un_width_eff);
    val_range.max = unit_range.max - (.5f * scalar.un_width_eff);
  }
  else {
    val_range.min = unit_range.min;
    val_range.max = unit_range.max;
    if(scalar.rep == ScalarValSpec::AVG_ACT) {
      scalar.min_sum_act = 0.0f;
    }
  }
  val_range.UpdateAfterEdit_NoGui();
}

void ScalarValLayerSpec::HelpConfig() {
  String help = "ScalarValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent a single scalar value.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current scalar value is displayed in the misc_1 variable of first unit in the layer, which can be clamped\
 and compared, etc (i.e., set the environment patterns to have just one unit and provide\
 the actual scalar value and it will automatically establish the appropriate distributed\
 representation in the rest of the units).  Unlike previous implementations, all units including the first are part of the distributed representation -- first unit is not special except in receiving the input and displaying output as misc_1.\n\
 \nScalarValLayerSpec Configuration:\n\
 - Default UnitSpec and LayerSpec params with FF_FB_INHIB, gi = 2.2 generally works well\n\
 - For 0-1 range, GAUSSIAN: 11 or 21 units works well, LOCALIST: 3 units\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the ScalarValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool ScalarValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  
  if(scalar.rep != ScalarValSpec::AVG_ACT) {
    if(lay->CheckError(lay->un_geom.n < 2, quiet, rval,
                       "coarse-coded scalar representation requires at least 2 units, I just set un_geom.n")) {
      if(scalar.rep == ScalarValSpec::LOCALIST) {
        lay->un_geom.n = 3;
        lay->un_geom.x = 3;
      }
      else if(scalar.rep == ScalarValSpec::GAUSSIAN) {
        lay->un_geom.n = 11;
        lay->un_geom.x = 11;
      }
    }

    if(scalar.rep == ScalarValSpec::LOCALIST) {
      // kwta.k = 1;         // localist means 1 unit active!!
      // gp_kwta.k = 1;
    }
  }

  LeabraUnitSpec* us = (LeabraUnitSpec*)lay->GetMainUnitSpec();
  if(us->act_misc.rec_nd) {
    taMisc::Warning("Scalar val must have UnitSpec.act_misc.rec_nd = false, to record value in act_eq of first unit.  I changed this for you in spec:", us->name, "make sure this is appropriate for all layers that use this spec");
    us->SetUnique("act_misc", true);
    us->act_misc.rec_nd = false;
  }
  if(!us->act_misc.avg_nd) {
    taMisc::Warning("Scalar val must have UnitSpec.act_misc.avg_nd = true, so learning is based on act_nd and NOT act_eq, which is used to record value in first unit.  I changed this for you in spec:", us->name, "make sure this is appropriate for all layers that use this spec");
    us->SetUnique("act_misc", true);
    us->act_misc.avg_nd = true;
  }
  
  // check for conspecs with correct params
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)lay->GetUnitStateSafe(net, 0);
  if(lay->CheckError(u == NULL, quiet, rval,
                "scalar val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int i=0; i<lay->projections.size; i++) {
    Projection* pj = lay->projections.FastEl(i);
    LeabraConSpec* cs = (LeabraConSpec*)pj->GetMainConSpec();
    ProjectionSpec* pspec = pj->GetMainPrjnSpec();
    if(pspec->GetStateSpecType() == LEABRA_NETWORK_STATE::T_ScalarValSelfPrjnSpec) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "scalar val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "scalar val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
        cs->SetUnique("lrate", true);
        cs->lrate = 0.0f;
      }
    }
  }
  return rval;
}

void ScalarValLayerSpec::LabelUnits_ugp
(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {

  LeabraNetwork* lnet = (LeabraNetwork*)net->net_owner;
  LeabraLayer* llay = (LeabraLayer*)lnet->LayerFromState(lay);
  llay->SetUnitNames(true);     // use names
  
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  LEABRA_UNIT_SPEC_CPP* us = (LEABRA_UNIT_SPEC_CPP*)lay->GetUnitSpec(net);
  const int nunits = ug->n_units;
  scalar.InitVal(0.0f, nunits, unit_range.min, unit_range.range);
  if(nunits < 1) return;        // must be at least a few units..
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float cur = scalar.GetUnitVal(i);
    llay->SetUnitName(u, (String)cur);
  }
}

void ScalarValLayerSpec::LabelUnits(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(lay, net, gpidx); );
}

void ScalarValLayerSpec::LabelUnitsNet() {
  LeabraNetwork* lnet = GET_MY_OWNER(LeabraNetwork);
  if(!lnet) return;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)lnet->net_state;
  for(int i=0; i < net->n_layers_built; i++) {
    LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)net->GetLayerState(i);
    if(lay->lesioned()) continue;
    LAYER_SPEC_CPP* ls = lay->GetLayerSpec(net);
    if(ls->spec_idx == this->spec_idx) { // same..
      LabelUnits(lay, net);
    }
  }
}



#include "ExtRewLayerSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(ExtRewSpec);
TA_BASEFUNS_CTORS_DEFN(OutErrSpec);
TA_BASEFUNS_CTORS_DEFN(ExtRewLayerSpec);


void ExtRewLayerSpec::Initialize() {
  rew_type = OUT_ERR_REW;
  Defaults_init();
  Initialize_core();
}

void ExtRewLayerSpec::Defaults_init() {
  SetUnique("lay_inhib", true);
  lay_inhib.on = true;
  lay_inhib.gi = 1.0f;
  lay_inhib.fb = 0.0f;
  SetUnique("avg_act", true);
  avg_act.targ_init = 0.25f;
  avg_act.fixed = true;
  SetUnique("inhib_misc", true);
  inhib_misc.self_fb = 0.3f;

  SetUnique("scalar", true);
  scalar.rep = ScalarValSpec::LOCALIST;
  SetUnique("unit_range", true);
  unit_range.min = 0.0f;  unit_range.max = 1.0f;
  unit_range.UpdateAfterEdit_NoGui();
  val_range.min = unit_range.min;
  val_range.max = unit_range.max;
}
  
void ExtRewLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  rew.UpdateAfterEdit_NoGui();
  out_err.UpdateAfterEdit_NoGui();
}

void ExtRewLayerSpec::HelpConfig() {
  String help = "ExtRewLayerSpec Computation:\n\
 Computes external rewards based on network performance on an output layer or directly provided rewards.\n\
 - Minus phase = zero reward represented\n\
 - Plus phase = external reward value (computed at start of 1+) is clamped as distributed scalar-val representation.\n\
 \nExtRewLayerSpec Configuration:\n\
 - OUT_ERR_REW: A recv connection from the output layer(s) where error is computed (marked with MarkerConSpec)\n\
 AND a MarkerConSpec from a layer called RewTarg that signals (>.5 act) when output errors count\n\
 - EXT_REW: external TARGET inputs to targ values deliver the reward value (e.g., input pattern or script)\n\
 - DA_REW: A recv connection or other means of setting da values = reward values.\n\
 - Recommend using LOCALIST, 3 units, unit_range = 0,1 for simplest encoding\n\
 \n(After pressing OK here, you will see information for configuring the ScalarValLayerSpec\
 which this layer is based on)";
  taMisc::Confirm(help);
  inherited::HelpConfig();
}

bool ExtRewLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  
  // sometimes this is used as a target layer for ext rew and it should NEVER drive sse
  lay->SetLayerFlag(Layer::NO_ADD_SSE);
  lay->SetLayerFlag(Layer::NO_ADD_COMP_SSE);

  // check for conspecs with correct params
  bool got_marker = false;
  LEABRA_LAYER_STATE* rew_targ_lay = NULL;
  if(lay->n_units < 2) return false;
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 1);
  const int nrg = u->NRecvConGps(net);
  for(int g=0; g<nrg; g++) {
    LEABRA_CON_STATE* recv_gp = (LEABRA_CON_STATE*)u->RecvConState(net, g);
    if(recv_gp->NotActive()) continue;
    LEABRA_CON_SPEC_CPP* cs = (LEABRA_CON_SPEC_CPP*)recv_gp->GetConSpec(net);
    if(!cs->IsMarkerCon()) continue;
    LEABRA_LAYER_STATE* rew_lay = (LEABRA_LAYER_STATE*)recv_gp->GetSendLayer(net);
    if(rew_lay->LayerNameIs("RewTarg")) {
      rew_targ_lay = rew_lay;
    }
    else {
      got_marker = true;
    }
    continue;
  }
  if(!got_marker) {
    lay->CheckError(rew_type == DA_REW, quiet, rval,
               "requires at least one recv MarkerConSpec connection from DA layer",
               "to get reward based on performance.  This was not found -- please fix!");
    lay->CheckError(rew_type == OUT_ERR_REW, quiet, rval,
               "requires at least one recv MarkerConSpec connection from output/response layer(s) to compute",
               "reward based on performance.  This was not found -- please fix!");
  }
  if(rew_type == OUT_ERR_REW) {
    if(lay->CheckError(rew_targ_lay == NULL, quiet, rval,
                  "requires a recv MarkerConSpec connection from layer called RewTarg",
                  "that signals (act > .5) when output error should be used for computing rewards.  This was not found -- please fix!")) {
      return false;                     // fatal
    }
    lay->CheckError(rew_targ_lay->n_units == 0, quiet, rval,
               "RewTarg layer must have one unit (has zero) -- please fix!");
    lay->CheckError(rew_targ_lay->layer_idx > lay->layer_idx, quiet, rval,
               "reward target (RewTarg) layer must be *before* this layer in list of layers -- it is now after, won't work");
  }
  return rval;
}



#include "TwoDValLayerSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(TwoDValSpec);
TA_BASEFUNS_CTORS_DEFN(TwoDValBias);
TA_BASEFUNS_CTORS_DEFN(TwoDValLayerSpec);

void TwoDValLayerSpec::Initialize() {
  // SetUnique("kwta", true);
  // kwta.k_from = KWTASpec::USE_K;
  // kwta.k = 9;
  // gp_kwta.k_from = KWTASpec::USE_K;
  // gp_kwta.k = 9;
  // SetUnique("inhib_group", true);
  // inhib_group = ENTIRE_LAYER;
  // SetUnique("inhib", true);
  // inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  // inhib.kwta_pt = .6f;

  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateAfterEdit_NoGui();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateAfterEdit_NoGui();
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else if(twod.rep == TwoDValSpec::LOCALIST) {
    x_range.min = 0.0f;  x_range.max = 1.0f;  x_range.UpdateAfterEdit_NoGui();
    y_range.min = 0.0f;  y_range.max = 1.0f;  y_range.UpdateAfterEdit_NoGui();
    x_val_range.min = x_range.min;  x_val_range.max = x_range.max;
    y_val_range.min = y_range.min;  y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void TwoDValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  x_range.UpdateAfterEdit_NoGui(); y_range.UpdateAfterEdit_NoGui();
  twod.UpdateAfterEdit_NoGui();
  if(twod.rep == TwoDValSpec::GAUSSIAN) {
    twod.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * twod.un_width_x);
    y_val_range.min = y_range.min + (.5f * twod.un_width_y);
    x_val_range.max = x_range.max - (.5f * twod.un_width_x);
    y_val_range.max = y_range.max - (.5f * twod.un_width_y);
  }
  else {
    x_val_range.min = x_range.min;    y_val_range.min = y_range.min;
    x_val_range.max = x_range.max;    y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void TwoDValLayerSpec::HelpConfig() {
  String help = "TwoDValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent two-dimensional values.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current twod value is encoded in the twod_vals member of the LeabraLayer (x1,y1, x2,y2, etc),\
 which are set by input data, and updated to reflect current values encoded over layer.\
 For no unit groups case, input data should be 2d with inner dim of size 2 (x,y) and outer dim\
 of n_vals size.  For unit_groups, data should be 4d with two extra outer dims of gp_x, gp_y.\
 Provide the actual twod values in input data and it will automatically establish the \
 appropriate distributed representation in the rest of the units.\n\
 \nTwoDValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the TwoDValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool TwoDValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  LeabraNetwork* main_net = (LeabraNetwork*)lay->own_net;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  
  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
                "coarse-coded twod representation requires at least 3 units, I just set un_geom.n")) {
    if(twod.rep == TwoDValSpec::LOCALIST) {
      lay->un_geom.n = 9;
      lay->un_geom.x = 3;
      lay->un_geom.y = 3;
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      lay->un_geom.n = 121;
      lay->un_geom.x = 11;
      lay->un_geom.y = 11;
    }
  }

  if(twod.rep == TwoDValSpec::LOCALIST) {
    // kwta.k = 1;         // localist means 1 unit active!!
    // gp_kwta.k = 1;
  }

  // check for conspecs with correct params
  LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)lay->GetUnitStateSafe(net, 0);
  if(lay->CheckError(u == NULL, quiet, rval,
                "scalar val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int i=0; i<lay->projections.size; i++) {
    Projection* pj = lay->projections.FastEl(i);
    LeabraConSpec* cs = (LeabraConSpec*)pj->GetMainConSpec();
    ProjectionSpec* pspec = pj->GetMainPrjnSpec();
    if(pspec->GetStateSpecType() == LEABRA_NETWORK_STATE::T_ScalarValSelfPrjnSpec) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "scalar val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "scalar val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
        cs->SetUnique("lrate", true);
        cs->lrate = 0.0f;
      }
    }
  }
  return rval;
}

void TwoDValLayerSpec::ReConfig(Network* main_net, int n_units) {
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)main_net->net_state;
  
  for(int i=0; i < net->n_layers_built; i++) {
    LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)net->GetLayerState(i);
    if(lay->lesioned()) continue;

    LeabraLayer* llay = (LeabraLayer*)main_net->LayerFromState(lay);
    LeabraLayerSpec* ls = (LeabraLayerSpec*)llay->GetMainLayerSpec();
    if(ls->spec_idx != this->spec_idx) continue;

    if(n_units > 0) {
      llay->SetNUnits(n_units);
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)llay->GetMainUnitSpec();
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)lay->GetUnitState(net, 0);

    if(twod.rep == TwoDValSpec::LOCALIST) {
      twod.min_sum_act = .2f;
      // kwta.k = 1;
      // inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      // inhib.kwta_pt = 0.9f;
      // us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      // us->act.thr = .17f;
      // us->act.gain = 220.0f;
      // us->act.nvar = .01f;
      // us->dt.vm_tau = 20.0f;
      bias_val.un = TwoDValBias::NO_UN; bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = 0.0f; x_range.max = 1.0f;
      y_range.min = 0.0f; y_range.max = 1.0f;

      for(int pi=0; pi<llay->projections.size; pi++) {
        Projection* pj = llay->projections.FastEl(pi);
        LeabraConSpec* cs = (LeabraConSpec*)pj->GetMainConSpec();
        ProjectionSpec* pspec = pj->GetMainPrjnSpec();
        if(pspec->GetStateSpecType() == LEABRA_NETWORK_STATE::T_ScalarValSelfPrjnSpec ||
           cs->IsMarkerCon()) {
          continue;
        }
        // cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
        cs->UpdateAfterEdit();
      }
    }
    else if(twod.rep == TwoDValSpec::GAUSSIAN) {
      // inhib.type = LeabraInhibSpec::KWTA_INHIB;
      // inhib.kwta_pt = 0.25f;
      // us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm_tau = 5.0f;
      bias_val.un = TwoDValBias::NO_UN;  bias_val.wt = TwoDValBias::NO_WT;
      x_range.min = -.5f; x_range.max = 1.5f;
      y_range.min = -.5f; y_range.max = 1.5f;

      for(int pi=0; pi<llay->projections.size; pi++) {
        Projection* pj = llay->projections.FastEl(pi);
        LeabraConSpec* cs = (LeabraConSpec*)pj->GetMainConSpec();
        ProjectionSpec* pspec = pj->GetMainPrjnSpec();
        if(pspec->GetStateSpecType() == LEABRA_NETWORK_STATE::T_ScalarValSelfPrjnSpec ||
           cs->IsMarkerCon()) {
          continue;
        }
        // cs->lmix.err_sb = true;
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
        cs->UpdateAfterEdit();
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}


void TwoDValLayerSpec::LabelUnits_ugp(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net, int gpidx) {
  LeabraNetwork* lnet = (LeabraNetwork*)net->net_owner;
  LeabraLayer* llay = (LeabraLayer*)lnet->LayerFromState(lay);
  llay->SetUnitNames(true);     // use names
  
  LEABRA_UNGP_STATE* ug = (LEABRA_UNGP_STATE*)lay->GetUnGpState(net, gpidx);
  const int nunits = ug->n_units;
  if(nunits < 3) return;        // must be at least a few units..
  twod.InitVal(0.0f, 0.0f, lay->un_geom_x, lay->un_geom_y, x_range.min, x_range.range, y_range.min,
               y_range.range);
  for(int i=0;i<nunits;i++) {
    LEABRA_UNIT_STATE* u = (LEABRA_UNIT_STATE*)ug->GetUnitState(net, i);
    if(u->lesioned()) continue;
    float x_cur, y_cur; twod.GetUnitVal(i, x_cur, y_cur);
    llay->SetUnitName(u, (String)x_cur + "," + String(y_cur));
  }
}

void TwoDValLayerSpec::LabelUnits(LEABRA_LAYER_STATE* lay, LEABRA_NETWORK_STATE* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(lay, net, gpidx); );
}

void TwoDValLayerSpec::LabelUnitsNet() {
  LeabraNetwork* lnet = GET_MY_OWNER(LeabraNetwork);
  if(!lnet) return;
  LEABRA_NETWORK_STATE* net = (LEABRA_NETWORK_STATE*)lnet->net_state;

  for(int i=0; i < net->n_layers_built; i++) {
    LEABRA_LAYER_STATE* lay = (LEABRA_LAYER_STATE*)net->GetLayerState(i);
    if(lay->lesioned()) continue;
    LAYER_SPEC_CPP* ls = lay->GetLayerSpec(net);
    if(ls->spec_idx == this->spec_idx) { // same..
      LabelUnits(lay, net);
    }
  }
}

#include "DecodeTwoDValLayerSpec.cpp"
TA_BASEFUNS_CTORS_DEFN(DecodeTwoDValLayerSpec);

