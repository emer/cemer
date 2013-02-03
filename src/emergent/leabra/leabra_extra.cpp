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

#include "leabra_extra.h"
#include <DataGroupSpec>
#include <DataGroupEl>
#include <Aggregate>
#include <taDataProc>
#include <taDataAnal>
#include <ValIdx_Array>
#include <OneToOnePrjnSpec>
#include <FullPrjnSpec>

#include <taMisc>

//////////////////////////////////
//      MarkerConSpec           //
//////////////////////////////////

//////////////////////////////////
//      ContextLayerSpec        //
//////////////////////////////////


//////////////////////////////////
//     LeabraTISpec	        //
//////////////////////////////////


//////////////////////////////////
//      MultCopyLayerSpec



//////////////////////////////////////////
//      Misc Special Objects            //
//////////////////////////////////////////

//////////////////////////////////
//      Linear Unit             //
//////////////////////////////////


//////////////////////////
//      NegBias         //
//////////////////////////

//////////////////////////////////
//      XCalSRAvgConSpec
//////////////////////////////////


//////////////////////////////////
//      XCalHebbConSpec
//////////////////////////////////

// //////////////////////////////////
// //   XCalMlTraceConSpec
// //////////////////////////////////

// void XCalMlTraceConSpec::Initialize() {
//   ml_mix = 0.0f;
//   sm_mix = 1.0f - ml_mix;
// }

// void XCalMlTraceConSpec::UpdateAfterEdit_impl() {
//   inherited::UpdateAfterEdit_impl();
//   sm_mix = 1.0f - ml_mix;
// }

//////////////////////////////////
//      TrialSynDepConSpec      //
//////////////////////////////////

//////////////////////////////////
//      CycleSynDepConSpec      //
//////////////////////////////////

//////////////////////////////////
//      CaiSynDepCon
//////////////////////////////////


//// SRAvg version



//////////////////////////////////
//      FastWtConSpec           //
//////////////////////////////////

///////////////////////////////////////////////////////////////
//   ActAvgHebbConSpec

///////////////////////////////////////////////////////////////
//   LeabraDeltaConSpec

///////////////////////////////////////////////////////////////
//   LeabraXCALSpikeConSpec


///////////////////////////////////////////////////////////////
//   LeabraLimPrecConSpec


//////////////////////////////////
//      Scalar Value Layer      //
//////////////////////////////////


//////////////////////////////////
//      Scalar Value Self Prjn  //
//////////////////////////////////

//////////////////////////////////
//      MotorForceLayerSpec     //
//////////////////////////////////

//////////////////////////////////
//      TwoD Value Layer        //
//////////////////////////////////


///////////////////////////////////////////////////////
//              TwoDValLayerSpec


///////////////////////////////////////////////////////////////
//   DecodeTwoDValLayerSpec

/*

//////////////////////////////////
//      FourD Value Layer       //
//////////////////////////////////

void FourDValLeabraLayer::Initialize() {
}

void FourDValLeabraLayer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateFourDValsGeom();
}

void FourDValLeabraLayer::UpdateFourDValsGeom() {
  FourDValLayerSpec* ls = (FourDValLayerSpec*)GetLayerSpec();
  if(!ls) return;
  if(ls->InheritsFrom(&TA_FourDValLayerSpec)) {
    if(unit_groups)
      fourd_vals.SetGeom(5, 2, FOURD_N, ls->fourd.n_vals, gp_geom.x, gp_geom.y);
    else
      fourd_vals.SetGeom(5, 2, FOURD_N, ls->fourd.n_vals, 1, 1);
  }
}

void FourDValLeabraLayer::ApplyInputData_2d(taMatrix* data, Unit::ExtType ext_flags,
                              Random* ran, const taVector2i& offs, bool na_by_range) {
  // only no unit_group supported!
  if(TestError(unit_groups, "ApplyInputData_2d",
               "input data must be 4d for layers with unit_groups: outer 2 are group dims, inner 2 are x,y vals and n_vals")) {
    return;
  }
  for(int d_y = 0; d_y < data->dim(1); d_y++) {
    int val_idx = offs.y + d_y;
    for(int d_x = 0; d_x < data->dim(0); d_x++) {
      int xy_idx = offs.x + d_x;
      Variant val = data->SafeElAsVar(d_x, d_y);
      if(ext_flags & Unit::EXT)
        fourd_vals.SetFmVar(val, xy_idx, FOURD_EXT, val_idx, 0, 0);
      else
        fourd_vals.SetFmVar(val, xy_idx, FOURD_TARG, val_idx, 0, 0);
    }
  }
}

void FourDValLeabraLayer::ApplyInputData_Flat4d(taMatrix* data, Unit::ExtType ext_flags,
                                  Random* ran, const taVector2i& offs, bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  if(TestError(!unit_groups, "ApplyInputData_Flat4d",
               "input data must be 2d for layers without unit_groups: x,y vals and n_vals")) {
    return;
  }
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int u_y = offs.y + dg_y * data->dim(1) + d_y; // multiply out data indicies
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int u_x = offs.x + dg_x * data->dim(0) + d_x; // multiply out data indicies
          Unit* un = UnitAtCoord(u_x, u_y);
          if(un) {
            float val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y).toFloat();
            un->ApplyInputData(val, ext_flags, ran, na_by_range);
          }
        }
      }
    }
  }
}

void FourDValLeabraLayer::ApplyInputData_Gp4d(taMatrix* data, Unit::ExtType ext_flags, Random* ran,
                                bool na_by_range) {
  // outer-loop is data-group (groups of x-y data items)
  for(int dg_y = 0; dg_y < data->dim(3); dg_y++) {
    for(int dg_x = 0; dg_x < data->dim(2); dg_x++) {

      for(int d_y = 0; d_y < data->dim(1); d_y++) {
        int val_idx = d_y;
        for(int d_x = 0; d_x < data->dim(0); d_x++) {
          int xy_idx = d_x;
          Variant val = data->SafeElAsVar(d_x, d_y, dg_x, dg_y);
          if(ext_flags & Unit::EXT)
            fourd_vals.SetFmVar(val, xy_idx, FOURD_EXT, val_idx, dg_x, dg_y);
          else
            fourd_vals.SetFmVar(val, xy_idx, FOURD_TARG, val_idx, dg_x, dg_y);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////
//              FourDValLayerSpec

void FourDValSpec::Initialize() {
  rep = GAUSSIAN;
  n_vals = 1;
  un_width = .3f;
  norm_width = false;
  clamp_pat = false;
  min_sum_act = 0.2f;
  mn_dst = 0.5f;
  clip_val = true;

  x_min = x_val = y_min = y_val = 0.0f;
  x_range = x_incr = y_range = y_incr = 1.0f;
  x_size = y_size = 1;
  un_width_x = un_width_y = un_width;
}

void FourDValSpec::InitRange(float xmin, float xrng, float ymin, float yrng) {
  x_min = xmin; x_range = xrng; y_min = ymin; y_range = yrng;
  un_width_x = un_width;
  un_width_y = un_width;
  if(norm_width) {
    un_width_x *= x_range;
    un_width_y *= y_range;
  }
}

void FourDValSpec::InitVal(float xval, float yval, int xsize, int ysize, float xmin, float xrng, float ymin, float yrng) {
  InitRange(xmin, xrng, ymin, yrng);
  x_val = xval; y_val = yval;
  x_size = xsize; y_size = ysize;
  x_incr = x_range / (float)(x_size - 1); // DON'T skip 1st row, and count end..
  y_incr = y_range / (float)(y_size - 1); // DON'T skip 1st row, and count end..
  //  incr -= .000001f;         // round-off tolerance..
}

float FourDValSpec::GetUnitAct(int unit_idx) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  if(rep == GAUSSIAN) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float x_dist = (x_cur - x_val) / un_width_x;
    float y_cur = y_min + y_incr * (float)y_idx;
    float y_dist = (y_cur - y_val) / un_width_y;
    float dist = x_dist * x_dist + y_dist * y_dist;
    return expf(-dist);
  }
  else if(rep == LOCALIST) {
    float x_cur = x_min + x_incr * (float)x_idx;
    float y_cur = y_min + y_incr * (float)y_idx;
    float x_dist = fabs(x_val - x_cur);
    float y_dist = fabs(y_val - y_cur);
    if((x_dist > x_incr) && (y_dist > y_incr)) return 0.0f;

    return 1.0f - .5 * ((x_dist / x_incr) + (y_dist / y_incr)); // todo: no idea if this is right.
  }
  return 0.0f;
}

void FourDValSpec::GetUnitVal(int unit_idx, float& x_cur, float& y_cur) {
  int x_idx = unit_idx % x_size;
  int y_idx = (unit_idx / x_size);
  x_cur = x_min + x_incr * (float)x_idx;
  y_cur = y_min + y_incr * (float)y_idx;
}

void FourDValBias::Initialize() {
  un = NO_UN;
  un_gain = 1.0f;
  wt = NO_WT;
  wt_gain = 1.0f;
  x_val = 0.0f;
  y_val = 0.0f;
}

void FourDValLayerSpec::Initialize() {
  min_obj_type = &TA_FourDValLeabraLayer;

  SetUnique("kwta", true);
  kwta.k_from = KWTASpec::USE_K;
  kwta.k = 9;
  gp_kwta.k_from = KWTASpec::USE_K;
  gp_kwta.k = 9;
  SetUnique("inhib_group", true);
  inhib_group = ENTIRE_LAYER;
  SetUnique("inhib", true);
  inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
  inhib.kwta_pt = .6f;

  if(fourd.rep == FourDValSpec::GAUSSIAN) {
    x_range.min = -0.5f;   x_range.max = 1.5f; x_range.UpdateAfterEdit_NoGui();
    y_range.min = -0.5f;   y_range.max = 1.5f; y_range.UpdateAfterEdit_NoGui();
    fourd.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * fourd.un_width_x);
    x_val_range.max = x_range.max - (.5f * fourd.un_width_x);
    y_val_range.min = y_range.min + (.5f * fourd.un_width_y);
    y_val_range.max = y_range.max - (.5f * fourd.un_width_y);
  }
  else if(fourd.rep == FourDValSpec::LOCALIST) {
    x_range.min = 0.0f;  x_range.max = 1.0f;  x_range.UpdateAfterEdit_NoGui();
    y_range.min = 0.0f;  y_range.max = 1.0f;  y_range.UpdateAfterEdit_NoGui();
    x_val_range.min = x_range.min;  x_val_range.max = x_range.max;
    y_val_range.min = y_range.min;  y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void FourDValLayerSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  x_range.UpdateAfterEdit_NoGui(); y_range.UpdateAfterEdit_NoGui();
  fourd.UpdateAfterEdit_NoGui();
  if(fourd.rep == FourDValSpec::GAUSSIAN) {
    fourd.InitRange(x_range.min, x_range.range, y_range.min, y_range.range);
    x_val_range.min = x_range.min + (.5f * fourd.un_width_x);
    y_val_range.min = y_range.min + (.5f * fourd.un_width_y);
    x_val_range.max = x_range.max - (.5f * fourd.un_width_x);
    y_val_range.max = y_range.max - (.5f * fourd.un_width_y);
  }
  else {
    x_val_range.min = x_range.min;    y_val_range.min = y_range.min;
    x_val_range.max = x_range.max;    y_val_range.max = y_range.max;
  }
  x_val_range.UpdateAfterEdit_NoGui(); y_val_range.UpdateAfterEdit_NoGui();
}

void FourDValLayerSpec::HelpConfig() {
  String help = "FourDValLayerSpec Computation:\n\
 Uses distributed coarse-coding units to represent two-dimensional values.  Each unit\
 has a preferred value arranged evenly between the min-max range, and decoding\
 simply computes an activation-weighted average based on these preferred values.  The\
 current fourd value is encoded in the fourd_vals member of the FourDValLeabraLayer (x1,y1, x2,y2, etc),\
 which are set by input data, and updated to reflect current values encoded over layer.\
 For no unit groups case, input data should be 2d with inner dim of size 2 (x,y) and outer dim\
 of n_vals size.  For unit_groups, data should be 4d with two extra outer dims of gp_x, gp_y.\
 Provide the actual fourd values in input data and it will automatically establish the \
 appropriate distributed representation in the rest of the units.\n\
 \nFourDValLayerSpec Configuration:\n\
 - The bias_val settings allow you to specify a default initial and ongoing bias value\
 through a constant excitatory current (GC) or bias weights (BWT) to the unit, and initial\
 weight values.  These establish a distributed representation that represents the given .val\n\
 - A self connection using the FourDValSelfPrjnSpec can be made, which provides a bias\
 for neighboring units to have similar values.  It should usually have a fairly small wt_scale.rel\
 parameter (e.g., .1)";
  taMisc::Confirm(help);
}

bool FourDValLayerSpec::CheckConfig_Layer(Layer* ly, bool quiet) {
  LeabraLayer* lay = (LeabraLayer*)ly;
  bool rval = inherited::CheckConfig_Layer(lay, quiet);

  if(lay->CheckError(lay->un_geom.n < 3, quiet, rval,
                "coarse-coded fourd representation requires at least 3 units, I just set un_geom.n")) {
    if(fourd.rep == FourDValSpec::LOCALIST) {
      lay->un_geom.n = 9;
      lay->un_geom.x = 3;
      lay->un_geom.y = 3;
    }
    else if(fourd.rep == FourDValSpec::GAUSSIAN) {
      lay->un_geom.n = 121;
      lay->un_geom.x = 11;
      lay->un_geom.y = 11;
    }
  }

  if(lay->InheritsFrom(&TA_FourDValLeabraLayer)) { // inh will be flagged above
    ((FourDValLeabraLayer*)lay)->UpdateFourDValsGeom();
  }

  if(fourd.rep == FourDValSpec::LOCALIST) {
    kwta.k = 1;         // localist means 1 unit active!!
    gp_kwta.k = 1;
  }

  if(bias_val.un == FourDValBias::GC) {
    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    if(lay->CheckError(us->hyst.init, quiet, rval,
                  "bias_val.un = GCH requires UnitSpec hyst.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("hyst", true);
      us->hyst.init = false;
    }
    if(lay->CheckError(us->acc.init, quiet, rval,
                  "bias_val.un = GC requires UnitSpec acc.init = false, I just set it for you in spec:", us->name, "(make sure this is appropriate for all layers that use this spec!)")) {
      us->SetUnique("acc", true);
      us->acc.init = false;
    }
  }

  // check for conspecs with correct params
  LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);      // taking 1st unit as representative
  if(lay->CheckError(u == NULL, quiet, rval,
                "fourd val layer doesn't have any units:", lay->name)) {
    return false;               // fatal
  }

  for(int g=0; g<u->recv.size; g++) {
    LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
    if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
    LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
    if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec)) {
      if(lay->CheckError(cs->wt_scale.rel > 0.5f, quiet, rval,
                    "fourd val self connections should have wt_scale < .5, I just set it to .1 for you (make sure this is appropriate for all connections that use this spec!)")) {
        cs->SetUnique("wt_scale", true);
        cs->wt_scale.rel = 0.1f;
      }
      if(lay->CheckError(cs->lrate > 0.0f, quiet, rval,
                    "fourd val self connections should have lrate = 0, I just set it for you in spec:", cs->name, "(make sure this is appropriate for all layers that use this spec!)")) {
        cs->SetUnique("lrate", true);
        cs->lrate = 0.0f;
      }
    }
    else if(cs->InheritsFrom(TA_MarkerConSpec)) {
      continue;
    }
  }
  return rval;
}

void FourDValLayerSpec::ReConfig(Network* net, int n_units) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, lay, net->layers) {
    if(lay->spec.SPtr() != this) continue;

    if(n_units > 0) {
      lay->SetNUnits(n_units);
    }

    LeabraUnitSpec* us = (LeabraUnitSpec*)lay->unit_spec.SPtr();
    LeabraUnit* u = (LeabraUnit*)lay->units.Leaf(0);    // taking 1st unit as representative

    if(fourd.rep == FourDValSpec::LOCALIST) {
      fourd.min_sum_act = .2f;
      kwta.k = 1;
      inhib.type = LeabraInhibSpec::KWTA_AVG_INHIB;
      inhib.kwta_pt = 0.9f;
      us->g_bar.h = .03f; us->g_bar.a = .09f;
      us->act_fun = LeabraUnitSpec::NOISY_LINEAR;
      us->act.thr = .17f;
      us->act.gain = 220.0f;
      us->act.nvar = .01f;
      us->dt.vm = .05f;
      bias_val.un = FourDValBias::GC; bias_val.wt = FourDValBias::NO_WT;
      x_range.min = 0.0f; x_range.max = 1.0f;
      y_range.min = 0.0f; y_range.max = 1.0f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = false; // false: this is critical for linear mapping of vals..
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    else if(fourd.rep == FourDValSpec::GAUSSIAN) {
      inhib.type = LeabraInhibSpec::KWTA_INHIB;
      inhib.kwta_pt = 0.25f;
      us->g_bar.h = .015f; us->g_bar.a = .045f;
      us->act_fun = LeabraUnitSpec::NOISY_XX1;
      us->act.thr = .25f;
      us->act.gain = 600.0f;
      us->act.nvar = .005f;
      us->dt.vm = .2f;
      bias_val.un = FourDValBias::GC;  bias_val.wt = FourDValBias::NO_WT;
      x_range.min = -.5f; x_range.max = 1.5f;
      y_range.min = -.5f; y_range.max = 1.5f;

      for(int g=0; g<u->recv.size; g++) {
        LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
        if((recv_gp->prjn == NULL) || (recv_gp->prjn->spec.SPtr() == NULL)) continue;
        LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
        if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
           cs->InheritsFrom(TA_MarkerConSpec)) {
          continue;
        }
        cs->lmix.err_sb = true;
        cs->rnd.mean = 0.1f;
        cs->rnd.var = 0.0f;
        cs->wt_sig.gain = 1.0; cs->wt_sig.off = 1.0;
      }
    }
    us->UpdateAfterEdit();
  }
  UpdateAfterEdit();
}

// todo: deal with lesion flag in lots of special purpose code like this!!!

void FourDValLayerSpec::Compute_WtBias_Val(Unit_Group* ugp, float x_val, float y_val) {
  if(nunits < 3) return;        // must be at least a few units..
  Layer* lay = ugp->own_lay;
  fourd.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = .03f * bias_val.wt_gain * fourd.GetUnitAct(i);
    for(int g=0; g<u->recv.size; g++) {
      LeabraRecvCons* recv_gp = (LeabraRecvCons*)u->recv.FastEl(g);
      LeabraConSpec* cs = (LeabraConSpec*)recv_gp->GetConSpec();
      if(recv_gp->prjn->spec.SPtr()->InheritsFrom(TA_ScalarValSelfPrjnSpec) ||
         cs->InheritsFrom(TA_MarkerConSpec)) continue;
      for(int ci=0;ci<recv_gp->size;ci++) {
        LeabraCon* cn = (LeabraCon*)recv_gp->Cn(ci);
        cn->wt += act;
        if(cn->wt < cs->wt_limits.min) cn->wt = cs->wt_limits.min;
        if(cn->wt > cs->wt_limits.max) cn->wt = cs->wt_limits.max;
      }
      recv_gp->Init_Weights_post(u);
    }
  }
}

void FourDValLayerSpec::Compute_UnBias_Val(Unit_Group* ugp, float x_val, float y_val) {
  if(nunits < 3) return;        // must be at least a few units..
  Layer* lay = ugp->own_lay;
  fourd.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float act = bias_val.un_gain * fourd.GetUnitAct(i);
    if(bias_val.un == FourDValBias::GC)
      u->vcb.g_h = act;
    else if(bias_val.un == FourDValBias::BWT)
      u->bias.OwnCn(0)->wt = act;
  }
}

void FourDValLayerSpec::Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net) {
  if(bias_val.un != FourDValBias::NO_UN) {
    UNIT_GP_ITR(lay, Compute_UnBias_Val(ugp, bias_val.x_val, bias_val.y_val););
  }
  if(bias_val.wt == FourDValBias::WT) {
    UNIT_GP_ITR(lay, Compute_WtBias_Val(ugp, bias_val.x_val, bias_val.y_val););
  }
}

void FourDValLayerSpec::Init_Weights(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Init_Weights(lay, net);
  Compute_BiasVal(lay, net);
}

void FourDValLayerSpec::ClampValue_ugp(Unit_Group* ugp, LeabraNetwork*, float rescale) {
  if(nunits < 3) return;        // must be at least a few units..
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ugp->own_lay;
  taVector2i gp_geom_pos = ugp->GpLogPos();
  // first initialize to zero
  LeabraUnitSpec* us = (LeabraUnitSpec*)ugp->FastEl(0)->GetUnitSpec();
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    u->SetExtFlag(Unit::EXT);
    u->ext = 0.0;
  }
  for(int k=0;k<fourd.n_vals;k++) {
    float x_val = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    float y_val = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_EXT,
                                  k, gp_geom_pos.x, gp_geom_pos.y);
    if(fourd.clip_val) {
      x_val = x_val_range.Clip(x_val);
      y_val = y_val_range.Clip(y_val);
    }
    fourd.InitVal(x_val, y_val, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      float act = rescale * fourd.GetUnitAct(i);
      if(act < us->opt_thresh.send)
        act = 0.0f;
      u->ext += act;
    }
  }
}

void FourDValLayerSpec::ReadValue(FourDValLeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, ReadValue_ugp(lay, ugp, net); );
}

void FourDValLayerSpec::ReadValue_ugp(FourDValLeabraLayer* lay, Unit_Group* ugp, LeabraNetwork* net) {
  if(nunits < 3) return;        // must be at least a few units..
  fourd.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  taVector2i gp_geom_pos = ugp->GpLogPos();
  if(fourd.n_vals == 1) {       // special case
    float x_avg = 0.0f; float y_avg = 0.0f;
    float sum_act = 0.0f;
    for(int i=0;i<nunits;i++) {
      LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
      LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
      float x_cur, y_cur;  fourd.GetUnitVal(i, x_cur, y_cur);
      float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
      x_avg += x_cur * act_val;
      y_avg += y_cur * act_val;
      sum_act += act_val;
    }
    sum_act = MAX(sum_act, fourd.min_sum_act);
    if(sum_act > 0.0f) {
      x_avg /= sum_act; y_avg /= sum_act;
    }
    // encode the value
    lay->SetFourDVal(x_avg, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
    lay->SetFourDVal(y_avg, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT,
                    0, gp_geom_pos.x, gp_geom_pos.y);
  }
  else {                        // multiple items
    // first find the max values, using sum of -1..+1 region
    static ValIdx_Array sort_ary;
    sort_ary.Reset();
    for(int i=0;i<nunits;i++) {
      float sum = 0.0f;
      float nsum = 0.0f;
      for(int x=-1;x<=1;x++) {
        for(int y=-1;y<=1;y++) {
          int idx = i + y * lay->un_geom.x + x;
          if(idx < 0 || idx >= nunits) continue;
          LeabraUnit* u = (LeabraUnit*)ugp->FastEl(idx);
          LeabraUnitSpec* us = (LeabraUnitSpec*)u->GetUnitSpec();
          float act_val = us->clamp_range.Clip(u->act_eq) / us->clamp_range.max; // clipped & normalized!
          nsum += 1.0f;
          sum += act_val;
        }
      }
      if(nsum > 0.0f) sum /= nsum;
      ValIdx vi(sum, i);
      sort_ary.Add(vi);
    }
    sort_ary.Sort();
    float mn_x = fourd.mn_dst * fourd.un_width_x * x_range.Range();
    float mn_y = fourd.mn_dst * fourd.un_width_y * y_range.Range();
    float mn_dist = mn_x * mn_x + mn_y * mn_y;
    int outi = 0;  int j = 0;
    while((outi < fourd.n_vals) && (j < sort_ary.size)) {
      ValIdx& vi = sort_ary[sort_ary.size - j - 1]; // going backward through sort_ary
      float x_cur, y_cur;  fourd.GetUnitVal(vi.idx, x_cur, y_cur);
      // check distance from all previous!
      float my_mn = x_range.Range() + y_range.Range();
      for(int k=0; k<j; k++) {
        ValIdx& vo = sort_ary[sort_ary.size - k - 1];
        if(vo.val == -1.0f) continue; // guy we skipped over before
        float x_prv, y_prv;  fourd.GetUnitVal(vo.idx, x_prv, y_prv);
        float x_d = x_cur - x_prv; float y_d = y_cur - y_prv;
        float dist = x_d * x_d + y_d * y_d;
        my_mn = MIN(dist, my_mn);
      }
      if(my_mn < mn_dist) { vi.val = -1.0f; j++; continue; } // mark with -1 so we know we skipped it

      // encode the value
      lay->SetFourDVal(x_cur, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetFourDVal(y_cur, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT,
                      0, gp_geom_pos.x, gp_geom_pos.y);
      j++; outi++;
    }
  }
}

void FourDValLayerSpec::LabelUnits_ugp(Unit_Group* ugp) {
  if(nunits < 3) return;        // must be at least a few units..
  Layer* lay = ugp->own_lay;
  fourd.InitVal(0.0f, 0.0f, lay->un_geom.x, lay->un_geom.y, x_range.min, x_range.range, y_range.min, y_range.range);
  for(int i=0;i<nunits;i++) {
    LeabraUnit* u = (LeabraUnit*)ugp->FastEl(i);
    float x_cur, y_cur; fourd.GetUnitVal(i, x_cur, y_cur);
    u->name = (String)x_cur + "," + String(y_cur);
  }
}

void FourDValLayerSpec::LabelUnits(LeabraLayer* lay, LeabraNetwork* net) {
  UNIT_GP_ITR(lay, LabelUnits_ugp(ugp); );
}

void FourDValLayerSpec::LabelUnitsNet(LeabraNetwork* net) {
  FOREACH_ELEM_IN_GROUP(LeabraLayer, l, net->layers) {
    if(l->spec.SPtr() == this)
      LabelUnits(l, net);
  }
}

void FourDValLayerSpec::HardClampExt(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_HardClamp(lay, net);
}

void FourDValLayerSpec::Settle_Init_TargFlags_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_TargFlags_Layer(lay, net);
  // need to actually copy over targ to ext vals!
  FourDValLeabraLayer* tdlay = (FourDValLeabraLayer*)lay;
  if(lay->ext_flag & Unit::TARG) {      // only process target layers..
    if(net->phase == LeabraNetwork::PLUS_PHASE) {
      UNIT_GP_ITR(tdlay,
                  for(int k=0;k<fourd.n_vals;k++) {
                    taVector2i gp_geom_pos = ugp->GpLogPos();
                    float x_val = tdlay->GetFourDVal(FourDValLeabraLayer::FOURD_X,
                                                    FourDValLeabraLayer::FOURD_TARG,
                                                    k, gp_geom_pos.x, gp_geom_pos.y);
                    float y_val = tdlay->GetFourDVal(FourDValLeabraLayer::FOURD_Y,
                                                    FourDValLeabraLayer::FOURD_TARG,
                                                    k, gp_geom_pos.x, gp_geom_pos.y);
                    tdlay->SetFourDVal(x_val, FourDValLeabraLayer::FOURD_X,
                                      FourDValLeabraLayer::FOURD_EXT,
                                      k, gp_geom_pos.x, gp_geom_pos.y);
                    tdlay->SetFourDVal(y_val, FourDValLeabraLayer::FOURD_Y,
                                      FourDValLeabraLayer::FOURD_EXT,
                                      k, gp_geom_pos.x, gp_geom_pos.y);
                  }
                  );
    }
  }
}


void FourDValLayerSpec::Settle_Init_Layer(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Settle_Init_Layer(lay, net);

  FourDValLeabraLayer* tdlay = (FourDValLeabraLayer*)lay;
  tdlay->UpdateFourDValsGeom(); // quick, make sure no mismatch

  if(bias_val.un == ScalarValBias::BWT) {
    // if using bias-weight bias, keep a constant scaling (independent of layer size)
    FOREACH_ELEM_IN_GROUP(LeabraUnit, u, lay->units) {
      LeabraConSpec* bspec = (LeabraConSpec*)u->GetUnitSpec()->bias_spec.SPtr();
      u->bias_scale = bspec->wt_scale.abs;  // still have absolute scaling if wanted..
      u->bias_scale /= 100.0f;              // keep a constant scaling so it doesn't depend on network size!
    }
  }
}

void FourDValLayerSpec::Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) {
  if(fourd.clamp_pat) {
    inherited::Compute_HardClamp(lay, net);
    return;
  }
  if(!(lay->ext_flag & Unit::EXT)) {
    lay->hard_clamped = false;
    return;
  }
  // allow for soft-clamping: translates pattern into exts first
  UNIT_GP_ITR(lay, if(nunits > 2) { ClampValue_ugp(ugp, net); } );
  // now check for actual hard clamping
  if(!clamp.hard) {
    lay->hard_clamped = false;
    return;
  }
  HardClampExt(lay, net);
}

void FourDValLayerSpec::Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) {
  inherited::Compute_CycleStats(lay, net);
  ReadValue((FourDValLeabraLayer*)lay, net);            // always read out the value
}

void FourDValLayerSpec::PostSettle(LeabraLayer* ly, LeabraNetwork* net) {
  inherited::PostSettle(ly, net);
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ly;
  UNIT_GP_ITR(lay, PostSettle_ugp(lay, ugp, net); );
}

void FourDValLayerSpec::PostSettle_ugp(FourDValLeabraLayer* lay, Unit_Group* ugp, LeabraNetwork* net) {
  taVector2i gp_geom_pos = ugp->GpLogPos();

  bool no_plus_testing = false;
  if(net->no_plus_test && (net->train_mode == LeabraNetwork::TEST)) {
    no_plus_testing = true;
  }

  for(int k=0;k<fourd.n_vals;k++) {
    float x_val, y_val, x_m, y_m, x_p, y_p;
    lay->GetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT,
                      k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetFourDVals(x_m, y_m, FourDValLeabraLayer::FOURD_ACT_M,
                      k, gp_geom_pos.x, gp_geom_pos.y);
    lay->GetFourDVals(x_p, y_p, FourDValLeabraLayer::FOURD_ACT_P,
                      k, gp_geom_pos.x, gp_geom_pos.y);

    switch(net->phase_order) {
    case LeabraNetwork::MINUS_PLUS:
      if(no_plus_testing) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(0.0f, 0.0f, FourDValLeabraLayer::FOURD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVals(x_val - x_m, y_val - y_m, FourDValLeabraLayer::FOURD_ACT_DIF,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_MINUS:
      if(no_plus_testing) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(0.0f, 0.0f, FourDValLeabraLayer::FOURD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        if(net->phase == LeabraNetwork::MINUS_PHASE) {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVals(x_p - x_val, y_p - y_val, FourDValLeabraLayer::FOURD_ACT_DIF,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
        else {
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      break;
    case LeabraNetwork::PLUS_ONLY:
      lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      lay->SetFourDVals(0.0f, 0.0f, FourDValLeabraLayer::FOURD_ACT_DIF,
                      k, gp_geom_pos.x, gp_geom_pos.y);
      break;
    case LeabraNetwork::MINUS_PLUS_NOTHING:
    case LeabraNetwork::MINUS_PLUS_MINUS:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                         k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else if(net->phase_no == 1) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_val - x_m, y_val - y_m, FourDValLeabraLayer::FOURD_ACT_DIF,
                         k, gp_geom_pos.x, gp_geom_pos.y);
        if(no_plus_testing) {
          // update act_m because it is actually another test case!
          lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                           k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      else {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M2,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_p - x_val, y_p - y_val, FourDValLeabraLayer::FOURD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    case LeabraNetwork::PLUS_NOTHING:
      // don't use actual phase values because pluses might be minuses with testing
      if(net->phase_no == 0) {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_P,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      else {
        lay->SetFourDVals(x_val, y_val, FourDValLeabraLayer::FOURD_ACT_M,
                        k, gp_geom_pos.x, gp_geom_pos.y);
        lay->SetFourDVals(x_p - x_val, y_p - y_val, FourDValLeabraLayer::FOURD_ACT_DIF,
                        k, gp_geom_pos.x, gp_geom_pos.y);
      }
      break;
    }
  }
}

float FourDValLayerSpec::Compute_SSE_ugp(Unit_Group* ugp, LeabraLayer* ly, int& n_vals) {
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ly;
  taVector2i gp_geom_pos = ugp->GpLogPos();
  LeabraUnitSpec* us = (LeabraUnitSpec*)ugp->FastEl(0)->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<fourd.n_vals;k++) { // first loop over and find potential target values
    float x_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_TARG, k,
                                   gp_geom_pos.x, gp_geom_pos.y);
    float y_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_TARG, k,
                                   gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      n_vals++;
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<fourd.n_vals;j++) {
        float x_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float y_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = dx * dx + dy * dy;
        if(dist < mn_dist) {
          mn_dist = dist;
          lay->SetFourDVal(dx, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVal(dy, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVal(dx*dx, FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_SQERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
          lay->SetFourDVal(dy*dy, FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_SQERR,
                          k, gp_geom_pos.x, gp_geom_pos.y);
        }
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float FourDValLayerSpec::Compute_SSE(LeabraLayer* lay, LeabraNetwork*,
                                    int& n_vals, bool unit_avg, bool sqrt) {
  n_vals = 0;
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return 0.0f;
  lay->sse = 0.0f;
  UNIT_GP_ITR(lay,
              lay->sse += Compute_SSE_ugp(ugp, lay, n_vals);
              );
  float rval = lay->sse;
  if(unit_avg && n_vals > 0)
    lay->sse /= (float)n_vals;
  if(sqrt)
    lay->sse = sqrtf(lay->sse);
  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE))) {
    rval = 0.0f;
    n_vals = 0;
  }
  return rval;
}

float FourDValLayerSpec::Compute_NormErr_ugp(LeabraLayer* ly, Unit_Group* ugp,
                                           LeabraInhib* thr, LeabraNetwork* net) {
  FourDValLeabraLayer* lay = (FourDValLeabraLayer*)ly;
  taVector2i gp_geom_pos = ugp->GpLogPos();
  LeabraUnitSpec* us = (LeabraUnitSpec*)ugp->FastEl(0)->GetUnitSpec();
  float rval = 0.0f;
  for(int k=0;k<fourd.n_vals;k++) { // first loop over and find potential target values
    float x_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_TARG,
                                   k, gp_geom_pos.x, gp_geom_pos.y);
    float y_targ = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_TARG,
                                   k, gp_geom_pos.x, gp_geom_pos.y);
    // only count if target value is within range -- otherwise considered a non-target
    if(x_val_range.RangeTestEq(x_targ) && y_val_range.RangeTestEq(y_targ)) {
      // now find minimum dist actual activations
      float mn_dist = taMath::flt_max;
      for(int j=0;j<fourd.n_vals;j++) {
        float x_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_X, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float y_act_m = lay->GetFourDVal(FourDValLeabraLayer::FOURD_Y, FourDValLeabraLayer::FOURD_ACT_M,
                                        j, gp_geom_pos.x, gp_geom_pos.y);
        float dx = x_targ - x_act_m;
        float dy = y_targ - y_act_m;
        if(fabsf(dx) < us->sse_tol) dx = 0.0f;
        if(fabsf(dy) < us->sse_tol) dy = 0.0f;
        float dist = fabsf(dx) + fabsf(dy); // only diff from sse!
        if(dist < mn_dist)
          mn_dist = dist;
      }
      rval += mn_dist;
    }
  }
  return rval;
}

float FourDValLayerSpec::Compute_NormErr(LeabraLayer* lay, LeabraNetwork* net) {
  lay->norm_err = -1.0f;                                         // assume not contributing
  if(!(lay->ext_flag & (Unit::TARG | Unit::COMP))) return -1.0f; // indicates not applicable

  float nerr = 0.0f;
  float ntot = 0;
  if((inhib_group != ENTIRE_LAYER) && (lay->unit_groups)) {
    for(int g=0; g<lay->units.gp.size; g++) {
      LeabraUnit_Group* rugp = (LeabraUnit_Group*)lay->units.gp[g];
      nerr += Compute_NormErr_ugp(lay, rugp, (LeabraInhib*)rugp, net);
      ntot += x_range.range + y_range.range;
    }
  }
  else {
    nerr += Compute_NormErr_ugp(lay, &(lay->units), (LeabraInhib*)lay, net);
    ntot += x_range.range + y_range.range;
  }
  if(ntot == 0.0f) return -1.0f;

  lay->norm_err = nerr / ntot;
  if(lay->norm_err > 1.0f) lay->norm_err = 1.0f;

  if(lay->HasLayerFlag(Layer::NO_ADD_SSE) ||
     ((lay->ext_flag & Unit::COMP) && lay->HasLayerFlag(Layer::NO_ADD_COMP_SSE)))
    return -1.0f;               // no contributarse

  return lay->norm_err;
}

*/

////////////////////////////////////////////////////////////
//      V1RFPrjnSpec

// void V1RFPrjnSpec::Initialize() {
//   init_wts = true;
//   wrap = false;
//   dog_surr_mult = 1.0f;
// }

// void V1RFPrjnSpec::UpdateAfterEdit_impl() {
//   inherited::UpdateAfterEdit_impl();
//   rf_spec.name = name + "_rf_spec";
// }

// void V1RFPrjnSpec::Connect_impl(Projection* prjn) {
//   if(!(bool)prjn->from)      return;
//   if(prjn->layer->units.leaves == 0) // an empty layer!
//     return;
//   if(TestWarning(prjn->layer->units.gp.size == 0, "Connect_impl",
//               "requires recv layer to have unit groups!")) {
//     return;
//   }

//   rf_spec.InitFilters();     // this one call initializes all filter info once and for all!
//   // renorm the dog net filter to 1 abs max!
//   if(rf_spec.filter_type == GaborV1Spec::BLOB) {
//     for(int i=0;i<rf_spec.blob_specs.size;i++) {
//       DoGFilter* df = (DoGFilter*)rf_spec.blob_specs.FastEl(i);
//       taMath_float::vec_norm_abs_max(&(df->net_filter));
//     }
//   }
//   TestWarning(rf_spec.n_filters != prjn->layer->un_geom.n,
//            "number of filters from rf_spec:", (String)rf_spec.n_filters,
//            "does not match layer un_geom.n:", (String)prjn->layer->un_geom.n);

//   taVector2i rf_width = rf_spec.rf_width;
//   int n_cons = rf_width.Product();
//   taVector2i rf_half_wd = rf_width / 2;
//   taVector2i ru_geo = prjn->layer->gp_geom;

//   taVector2i su_geo = prjn->from->un_geom;

//   taVector2i ruc;
//   for(int alloc_loop=1; alloc_loop>=0; alloc_loop--) {
//     for(ruc.y = 0; ruc.y < ru_geo.y; ruc.y++) {
//       for(ruc.x = 0; ruc.x < ru_geo.x; ruc.x++) {

//      Unit_Group* ru_gp = prjn->layer->UnitGpAtCoord(ruc);
//      if(ru_gp == NULL) continue;

//      taVector2i su_st;
//      if(wrap) {
//        su_st.x = (int)floor((float)ruc.x * rf_move.x) - rf_half_wd.x;
//        su_st.y = (int)floor((float)ruc.y * rf_move.y) - rf_half_wd.y;
//      }
//      else {
//        su_st.x = (int)floor((float)ruc.x * rf_move.x);
//        su_st.y = (int)floor((float)ruc.y * rf_move.y);
//      }

//      su_st.WrapClip(wrap, su_geo);
//      taVector2i su_ed = su_st + rf_width;
//      if(wrap) {
//        su_ed.WrapClip(wrap, su_geo); // just wrap ends too
//      }
//      else {
//        if(su_ed.x > su_geo.x) {
//          su_ed.x = su_geo.x; su_st.x = su_ed.x - rf_width.x;
//        }
//        if(su_ed.y > su_geo.y) {
//          su_ed.y = su_geo.y; su_st.y = su_ed.y - rf_width.y;
//        }
//      }

//      for(int rui=0;rui<ru_gp->size;rui++) {
//        Unit* ru_u = (Unit*)ru_gp->FastEl(rui);
//        if(!alloc_loop)
//          ru_u->RecvConsPreAlloc(n_cons, prjn);

//        taVector2i suc;
//        taVector2i suc_wrp;
//        for(suc.y = 0; suc.y < rf_width.y; suc.y++) {
//          for(suc.x = 0; suc.x < rf_width.x; suc.x++) {
//            suc_wrp = su_st + suc;
//            if(suc_wrp.WrapClip(wrap, su_geo) && !wrap)
//              continue;
//            Unit* su_u = prjn->from->UnitAtCoord(suc_wrp);
//            if(su_u == NULL) continue;
//            if(!self_con && (su_u == ru_u)) continue;
//            ru_u->ConnectFrom(su_u, prjn, alloc_loop); // don't check: saves lots of time!
//          }
//        }
//      }
//       }
//     }
//     if(alloc_loop) { // on first pass through alloc loop, do sending allocations
//       prjn->from->SendConsPostAlloc(prjn);
//     }
//   }
// }

// void V1RFPrjnSpec::C_Init_Weights(Projection* prjn, RecvCons* cg, Unit* ru) {
//   Unit_Group* rugp = (Unit_Group*)ru->GetOwner();
//   int recv_idx = ru->pos.y * rugp->geom.x + ru->pos.x;

//   bool on_rf = true;
//   if(prjn->from->name.contains("_off"))
//     on_rf = false;
//   DoGFilter::ColorChannel col_chan = DoGFilter::BLACK_WHITE;
//   if(prjn->from->name.contains("_rg_"))
//     col_chan = DoGFilter::RED_GREEN;
//   else if(prjn->from->name.contains("_by_"))
//     col_chan = DoGFilter::BLUE_YELLOW;

//   int send_x = rf_spec.rf_width.x;
//   if(rf_spec.filter_type == GaborV1Spec::BLOB) {
//     // color is outer-most dimension, and if it doesn't match, then bail
//     int clr_dx = (recv_idx / (rf_spec.blob_rf.n_sizes * 2) % 2);
//     DoGFilter* df = (DoGFilter*)rf_spec.blob_specs.SafeEl(recv_idx);
//     if(!df) return;          // oops
//     if(col_chan != DoGFilter::BLACK_WHITE) {
//       // outer-most mod is color, after phases (2) and sizes (inner)
//       if((clr_dx == 0 && col_chan == DoGFilter::BLUE_YELLOW) ||
//       (clr_dx == 1 && col_chan == DoGFilter::RED_GREEN)) {
//      for(int i=0; i<cg->size; i++)
//        cg->Cn(i)->wt = 0.0f;
//      return;                 // bail if not our channel.
//       }
//     }
//     for(int i=0; i<cg->size; i++) {
//       int su_x = i % send_x;
//       int su_y = i / send_x;
//       float val = rf_spec.gabor_rf.amp * df->net_filter.SafeEl(su_x, su_y);
//       if(on_rf) {
//      if(df->on_sigma > df->off_sigma) val *= dog_surr_mult;
//      if(val > 0.0f) cg->Cn(i)->wt = val;
//      else           cg->Cn(i)->wt = 0.0f;
//       }
//       else {
//      if(df->off_sigma > df->on_sigma) val *= dog_surr_mult;
//      if(val < 0.0f)  cg->Cn(i)->wt = -val;
//      else            cg->Cn(i)->wt = 0.0f;
//       }
//     }
//   }
//   else {                     // GABOR
//     GaborFilterSpec* gf = (GaborFilterSpec*)rf_spec.gabor_specs.SafeEl(recv_idx);
//     if(!gf) return;          // oops
//     for(int i=0; i<cg->size; i++) {
//       int su_x = i % send_x;
//       int su_y = i / send_x;
//       float val = gf->filter.SafeEl(su_x, su_y);
//       if(on_rf) {
//      if(val > 0.0f) cg->Cn(i)->wt = val;
//      else           cg->Cn(i)->wt = 0.0f;
//       }
//       else {
//      if(val < 0.0f)  cg->Cn(i)->wt = -val;
//      else            cg->Cn(i)->wt = 0.0f;
//       }
//     }
//   }
// }

// bool V1RFPrjnSpec::TrgRecvFmSend(int send_x, int send_y) {
//   trg_send_geom.x = send_x;
//   trg_send_geom.y = send_y;

//   if(wrap)
//     trg_recv_geom = (trg_send_geom / rf_move);
//   else
//     trg_recv_geom = (trg_send_geom / rf_move) - 1;

//   // now fix it the other way
//   if(wrap)
//     trg_send_geom = (trg_recv_geom * rf_move);
//   else
//     trg_send_geom = ((trg_recv_geom +1) * rf_move);

//   SigEmitUpdated();
//   return (trg_send_geom.x == send_x && trg_send_geom.y == send_y);
// }

// bool V1RFPrjnSpec::TrgSendFmRecv(int recv_x, int recv_y) {
//   trg_recv_geom.x = recv_x;
//   trg_recv_geom.y = recv_y;

//   if(wrap)
//     trg_send_geom = (trg_recv_geom * rf_move);
//   else
//     trg_send_geom = ((trg_recv_geom+1) * rf_move);

//   // now fix it the other way
//   if(wrap)
//     trg_recv_geom = (trg_send_geom / rf_move);
//   else
//     trg_recv_geom = (trg_send_geom / rf_move) - 1;

//   SigEmitUpdated();
//   return (trg_recv_geom.x == recv_x && trg_recv_geom.y == recv_y);
// }

// void V1RFPrjnSpec::GraphFilter(DataTable* graph_data, int recv_unit_no) {
//   rf_spec.GraphFilter(graph_data, recv_unit_no);
// }

// void V1RFPrjnSpec::GridFilter(DataTable* graph_data) {
//   rf_spec.GridFilter(graph_data);
// }


////////////////////////////////////////////////////////////
//      LeabraExtOnlyUnitSpec


////////////////////////////////////////////////////////////
//      SaliencyPrjnSpec


//////////////////////////////////////////////////////////
//              V1LateralContourPrjnSpec


//////////////////////////////////////////////////////////
//              V2BoLateralPrjnSpec



//////////////////////////////////////////////////////////
//              FgBoEllipseGpPrjnSpec



//////////////////////////////////////////////////////////
//              FgBoWedgeGpPrjnSpec


//////////////////////////////////////////////////////////
//              V1EndStopPrjnSpec

///////////////////////////////////////////////////////////////
//                      VisDisparityPrjnSpec

///////////////////////////////////////////////////////////////
//                      VisDisparityLayerSpec


///////////////////////////////////////////////////////////////
//              TiledGpRFOneToOnePrjnSpec


///////////////////////////////////////////////////////////////
//              TiledGpRFOneToOneWtsPrjnSpec

///////////////////////////////////////////////////////////////
//              V2toV4DepthPrjnSpec


///////////////////////////////////////////////////////////////
//                      V1 Layer
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//      Cerebellum-related special guys


///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////
//              Wizard          //
//////////////////////////////////

