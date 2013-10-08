// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "V1RegionSpec.h"
#include <taMath_float>
#include <taProject>
#include <RetinaProc>

#include <taMisc>

void V1GaborSpec::Initialize() {
  gain = 2.0f;
  n_angles = 4;
  filter_size = 6;
  spacing = 1;
  wvlen = 6.0f;
  gauss_sig_len = 0.3f;
  gauss_sig_wd = 0.2f;
  phase_off = 0.0f;
  circle_edge = true;
}

void V1GaborSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void V1GaborSpec::RenderFilters(float_Matrix& fltrs) {
  fltrs.SetGeom(3, filter_size, filter_size, n_angles);

  float ctr = (float)(filter_size-1) / 2.0f;
  float ang_inc = taMath_float::pi / (float)n_angles;

  float circ_radius = (float)(filter_size) / 2.0f;

  float gs_len_eff = gauss_sig_len * (float)filter_size;
  float gs_wd_eff = gauss_sig_wd * (float)filter_size;

  float len_norm = 1.0f / (2.0f * gs_len_eff * gs_len_eff);
  float wd_norm = 1.0f / (2.0f * gs_wd_eff * gs_wd_eff);

  float twopinorm = (2.0f * taMath_float::pi) / wvlen;

  for(int ang = 0; ang < n_angles; ang++) {
    float angf = -(float)ang * ang_inc;

    float pos_sum = 0.0f;
    float neg_sum = 0.0f;
    for(int x = 0; x < filter_size; x++) {
      for(int y = 0; y < filter_size; y++) {
        float xf = (float)x - ctr;
        float yf = (float)y - ctr;

        float dist = taMath_float::hypot(xf, yf);
        float val = 0.0f;
        if(!(circle_edge && (dist > circ_radius))) {
          float nx = xf * cosf(angf) - yf * sinf(angf);
          float ny = yf * cosf(angf) + xf * sinf(angf);
          float gauss = expf(-(len_norm * (nx * nx) + wd_norm * (ny * ny)));
          float sin_val = sinf(twopinorm * ny + phase_off);
          val = gauss * sin_val;
          if(val > 0.0f)        { pos_sum += val; }
          else if(val < 0.0f)   { neg_sum += val; }
        }
        fltrs.FastEl3d(x, y, ang) = val;
      }
    }
    // renorm each half
    float pos_norm = 1.0f / pos_sum;
    float neg_norm = -1.0f / neg_sum;
    for(int x = 0; x < filter_size; x++) {
      for(int y = 0; y < filter_size; y++) {
        float& val = fltrs.FastEl3d(x, y, ang);
        if(val > 0.0f)          { val *= pos_norm; }
        else if(val < 0.0f)     { val *= neg_norm; }
      }
    }
  }
}

void V1GaborSpec::GridFilters(float_Matrix& fltrs, DataTable* graph_data, bool reset) {
  RenderFilters(fltrs);         // just to make sure

  String name;
  if(owner) name = owner->GetName();

  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_V1Gabor_GridFilters", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
//   nmda->SetUserData("WIDTH", 10);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, filter_size, filter_size);

  float maxv = taMath_float::vec_abs_max(&fltrs, idx);

  graph_data->SetUserData("N_ROWS", 4);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  int ang_inc = 180 / n_angles;

  for(int ang=0; ang<n_angles; ang++) {
    graph_data->AddBlankRow();
    float_MatrixPtr frm; frm = (float_Matrix*)fltrs.GetFrameSlice(ang);
    matda->SetValAsMatrix(frm, -1);
    nmda->SetValAsString("Angle: " + String(ang * ang_inc), -1);
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

void V1sNeighInhib::Initialize() {
  on = true;
  inhib_d = 1;
  inhib_g = 0.8f;

  tot_ni_len = 2 * inhib_d + 1;
}

void V1sNeighInhib::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_ni_len = 2 * inhib_d + 1;
}

void V1MotionSpec::Initialize() {
  r_only = true;
  n_speeds = 1;
  speed_inc = 1;
  tuning_width = 1;
  gauss_sig = 0.8f;
  opt_thr = 0.01f;

  tot_width = 1 + 2 * tuning_width;
}

void V1MotionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_width = 1 + 2 * tuning_width;
}

void V1BinocularSpec::Initialize() {
  mot_in = false;
  n_disps = 1;
  disp_range_pct = 0.05f;
  gauss_sig = 0.7f;
  disp_spacing = 2.0f;
  end_extra = 2;
  fix_horiz = true;
  horiz_thr = 0.2f;

  tot_disps = 1 + 2 * n_disps;
  ambig_wt = 1.0f / (float)tot_disps; // ambiguous case weighting
  UpdateFmV1sSize(36);
}

void V1BinocularSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_disps = 1 + 2 * n_disps;
  ambig_wt = 1.0f / (float)tot_disps; // ambiguous case weighting
}

void V1ComplexSpec::Initialize() {
  sg4 = false;
  spc4 = true;
  len_sum_len = 1;
  es_thr = 0.2f;

  sg_rf = 4;
  sg_half = sg_rf / 2;
  if(spc4)
    sg_spacing = sg_rf;
  else
    sg_spacing = sg_half;

  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}

void V1ComplexSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  sg_rf = 4;
  sg_half = sg_rf / 2;
  if(spc4)
    sg_spacing = sg_rf;
  else
    sg_spacing = sg_half;

  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}

void V2BordOwnSpec::Initialize() {
  lat_itrs = 10;
  lat_dt = 0.5f;
  act_thr = 0.1f;
  ambig_gain = 0.2f;
  l_t_inhib_thr = 0.2f;
  depths_out = 1;
  depth_idx = -1;

  ffbo_gain = 1.0f - ambig_gain;
}

void V2BordOwnSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  ffbo_gain = 1.0f - ambig_gain;
}

void V2BordOwnStencilSpec::Initialize() {
  gain = 4.0f;
  radius = 4;
  t_on = true;
  opp_on = false;
  ang_sig = 0.5f;
  dist_sig = 0.8f;
  weak_mag = 0.5f;
  con_thr = 0.2f;
}

void VisSpatIntegSpec::Initialize() {
  spat_rf = 6;
  gauss_sig = 0.8f;
  sum_rf = false;

  spat_half = spat_rf / 2;
  spat_spacing = spat_half;
  spat_border = 0;
}

void VisSpatIntegSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  spat_rf.SetGtEq(1);
  spat_half = spat_rf / 2;
  spat_spacing = spat_half;
  spat_spacing.SetGtEq(1);
}

// for thread function calling:
typedef void (V1RegionSpec::*V1RegionMethod)(int, int);

void V1RegionSpec::Initialize() {
  v1s_renorm = LIN_RENORM;
  v1s_save = (DataSave)(SAVE_DATA | ONLY_GUI);
  v1s_feat_geom.SetXYN(4, 2, 8);

  v1m_renorm = LIN_RENORM;

  v1b_filters = BF_DEFAULT;
  v1b_renorm = LIN_RENORM;
  v1b_save = SAVE_DATA;

  v1c_filters = CF_DEFAULT;
  v1c_renorm = LIN_RENORM;
  v1c_save = SAVE_DATA;

  v2_filters = V2_DEFAULT;
  v2_save = SAVE_DATA;

  spat_integ = SI_NONE;
  si_save = SAVE_DATA;

  opt_filters = OF_NONE;
  opt_save = SAVE_DATA;

  v1s_kwta.on = true;
  v1s_kwta.gp_k = 1;
  v1s_kwta.gp_g = 0.02f;

  v1ls_kwta.on = true;
  v1ls_kwta.gp_k = 1;
  v1ls_kwta.gp_g = 0.6f;

  v1ls_neigh_inhib.on = false;
  v1ls_neigh_inhib.inhib_g = 0.8f;

  si_renorm = LIN_RENORM;
  si_kwta.on = true;
  si_kwta.gp_k = 2;
  si_kwta.gp_g = 0.1f;

  n_colors = 1;
  n_polarities = 2;
  n_polclr = n_colors * n_polarities;

  cur_out_acts = NULL;
  cur_still = NULL;
  cur_maxout = NULL;
  cur_hist = NULL;
  cur_v1b_in_r = NULL;
  cur_v1b_in_l = NULL;
}

void V1RegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  v1s_specs.UpdateAfterEdit_NoGui();
  v1s_kwta.UpdateAfterEdit_NoGui();
  v1s_neigh_inhib.UpdateAfterEdit_NoGui();
  v1s_motion.UpdateAfterEdit_NoGui();
  v1b_specs.UpdateAfterEdit_NoGui();
  v1c_specs.UpdateAfterEdit_NoGui();
  v1ls_kwta.UpdateAfterEdit_NoGui();
  v1ls_neigh_inhib.UpdateAfterEdit_NoGui();
  v2_specs.UpdateAfterEdit_NoGui();
  v2_ffbo.UpdateAfterEdit_NoGui();
  // UpdateGeom is called in parent..
}

bool V1RegionSpec::NeedsInit() {
  if((v1s_out_r.dims() < 4) ||
     (v1s_out_r.dim(0) * v1s_out_r.dim(1) != v1s_feat_geom.n) ||
     (v1s_out_r.dim(2) != v1s_img_geom.x) ||
     (v1s_out_r.dim(3) != v1s_img_geom.y))
    return true;

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if((v1s_out_l.dims() < 4) ||
       (v1s_out_l.dim(0) * v1s_out_l.dim(1) != v1s_feat_geom.n) ||
       (v1s_out_l.dim(2) != v1s_img_geom.x) ||
       (v1s_out_l.dim(3) != v1s_img_geom.y))
      return true;
  }
  return false;
}


static void geom_get_angles(float angf, float& cosx, float& siny,
                            float& cosx_raw, float& siny_raw) {
  cosx_raw = taMath_float::cos(angf);
  siny_raw = taMath_float::sin(angf);
  // always normalize by the largest value so that it is equal to 1
  if(fabsf(cosx_raw) > fabsf(siny_raw)) {
    siny = siny_raw / fabsf(cosx_raw);          // must come first!
    cosx = cosx_raw / fabsf(cosx_raw);
  }
  else {
    cosx = cosx_raw / fabsf(siny_raw);
    siny = siny_raw / fabsf(siny_raw);
  }
}

void V1RegionSpec::UpdateGeom() {
  static bool redo = false;
  inherited::UpdateGeom();

  ///////////////////////////////////////////////////////////////
  //                    V1 S

  if(v1b_specs.mot_in) {
    if(motion_frames > 1) {
      if(v1s_motion.r_only) {
        taMisc::Warning("V1RegionSpec:", name, " -- v1b_specs.mot_in cannot have v1s_motion.r_only -- turning off");
        v1s_motion.r_only = false;
      }
    }
    else {
      taMisc::Warning("V1RegionSpec:", name, " -- v1b_specs.mot_in only works when motion_frames > 1 -- turning off for now");
      v1b_specs.mot_in = false;
    }
  }

  n_polarities = 2;             // justin case
  if(region.color == VisRegionParams::COLOR) {
    n_colors = 4;
  }
  else {
    n_colors = 1;
  }
  n_polclr = n_colors * n_polarities;
  v1s_feat_geom.x = v1s_specs.n_angles;
  v1s_feat_geom.y = n_polclr;
  v1s_feat_geom.UpdateNfmXY();

  if(region.edge_mode == VisRegionParams::WRAP) {
    v1s_img_geom = input_size.input_size / v1s_specs.spacing;
  }
  else {
    v1s_img_geom = ((input_size.input_size - 1) / v1s_specs.spacing) + 1;
  }

  v1s_ang_slopes.SetGeom(3,2,2,v1s_specs.n_angles);
  v1s_ang_slopes_raw.SetGeom(3,2,2,v1s_specs.n_angles);
  float ang_inc = taMath_float::pi / (float)v1s_specs.n_angles;
  for(int ang=0; ang<v1s_specs.n_angles; ang++) {
    float cosx, siny;
    float cosx_raw, siny_raw;
    float angf = (float)ang * ang_inc;
    geom_get_angles(angf, cosx, siny, cosx_raw, siny_raw);
    v1s_ang_slopes.FastEl3d(X, LINE, ang) = cosx;
    v1s_ang_slopes.FastEl3d(Y, LINE, ang) = siny;
    v1s_ang_slopes_raw.FastEl3d(X, LINE, ang) = cosx_raw;
    v1s_ang_slopes_raw.FastEl3d(Y, LINE, ang) = siny_raw;

    geom_get_angles(angf + taMath_float::pi * .5f, cosx, siny, cosx_raw, siny_raw);
    v1s_ang_slopes.FastEl3d(X, ORTHO, ang) = cosx;
    v1s_ang_slopes.FastEl3d(Y, ORTHO, ang) = siny;
    v1s_ang_slopes_raw.FastEl3d(X, ORTHO, ang) = cosx_raw;
    v1s_ang_slopes_raw.FastEl3d(Y, ORTHO, ang) = siny_raw;
  }

  ///////////////////////////////////////////////////////////////
  //                    V1M Motion

  // all angles for each gabor
  v1m_in_polarities = 1;        // always using polinvar inputs
  v1m_feat_geom.x = v1s_specs.n_angles;
  v1m_feat_geom.y = 2 * v1m_in_polarities * v1s_motion.n_speeds; // 2 directions
  v1m_feat_geom.UpdateNfmXY();

  ///////////////////////////////////////////////////////////////
  //                    V1B Binocular

  v1b_feat_geom.x = v1s_feat_geom.x; // just the angles
  v1b_feat_geom.y = v1b_specs.tot_disps;
  v1b_feat_geom.UpdateNfmXY();

  v1b_specs.UpdateFmV1sSize(v1s_img_geom.x); // update based on size of v1s

  ///////////////////////////////////////////////////////////////
  //                    V1C Complex

  if(v2_filters & V2_TL) {
    v1c_filters = (ComplexFilters) (v1c_filters | (LEN_SUM | END_STOP)); // must be set
  }

  int n_cmplx = 1;              // assume len sum
  if(v1c_filters & END_STOP) {
    v1c_filters = (ComplexFilters) (v1c_filters | LEN_SUM);     // must be set
    n_cmplx = 3;                                                // 2 es dirs
  }

  v1c_feat_geom.x = v1s_specs.n_angles;
  v1c_feat_geom.y = n_cmplx;
  v1c_feat_geom.UpdateNfmXY();

  ///////       V1C spatial geom
  if(region.edge_mode == VisRegionParams::WRAP) {
    v1c_specs.sg_border = 0;
    if(v1c_specs.sg4) {
      v1sg_img_geom = v1s_img_geom / v1c_specs.sg_spacing;
      v1c_img_geom = v1sg_img_geom;
    }
    else {
      v1sg_img_geom = v1s_img_geom;
      v1c_img_geom = v1s_img_geom;
    }
  }
  else {
    if(v1c_specs.sg4) {
      v1c_specs.sg_border = v1c_specs.sg_spacing;
      v1sg_img_geom = (((v1s_img_geom - 2 * v1c_specs.sg_border)-1) / v1c_specs.sg_spacing) + 1;
      v1c_img_geom = v1sg_img_geom;
    }
    else {
      v1c_specs.sg_border = 0;
      v1sg_img_geom = v1s_img_geom;
      v1c_img_geom = v1s_img_geom;
    }

//      v1c_img_geom = (((v1sg_img_geom - 2 * v1c_specs.spat_border)-1) / v1c_specs.spat_spacing) + 1;
  }

//   if(v2_filters & V2_BO) {
//     v2_filters = (V2Filters) (v2_filters | V2_TL); // must be set
//   }

  ///////////////////////////////////////
  //  Spat Integ Geoms

  if(!(v1c_filters & (LEN_SUM | END_STOP))) {
    // if not doing complex, don't do SI_V1C
    spat_integ = (SpatIntegFilters)(spat_integ & ~SI_V1C);
  }
  if(!(v2_filters & V2_BO)) {
    spat_integ = (SpatIntegFilters)(spat_integ & ~SI_V2BO);
  }

  if(region.edge_mode == VisRegionParams::WRAP) {
    si_specs.spat_border = 0;
    si_v1s_geom = v1s_img_geom / si_specs.spat_spacing;
    si_v1sg_geom = v1sg_img_geom / si_specs.spat_spacing;
    si_v1c_geom = v1c_img_geom / si_specs.spat_spacing;
   }
  else {
    si_specs.spat_border = si_specs.spat_spacing;
    si_v1s_geom = (((v1s_img_geom - 2 * si_specs.spat_border)-1) / si_specs.spat_spacing) + 1;
    si_v1sg_geom = (((v1sg_img_geom - 2 * si_specs.spat_border)-1) / si_specs.spat_spacing) + 1;
    si_v1c_geom = (((v1c_img_geom - 2 * si_specs.spat_border)-1) / si_specs.spat_spacing) + 1;
  }

  ///////////////////////////////////////
  //  Double-check geom from top down

  return;                       // don't do this yet -- wait till spat invar done

//   if(redo) {                 // if doing a redo, stop here and bail
//     redo = false;
//     return;
//   }

//   taVector2i v1s_fm_v1c;
//   if(region.edge_mode == VisRegionParams::WRAP) {
//     v1s_fm_v1c = v1c_specs.net_spacing * v1c_img_geom;
//   }
//   else {
// //     cg = ((sg - 2b - 1) / sp) + 1;
// //     cg - 1 = ((sg - 2b - 1) / sp);
// //     sp (cg - 1) = (sg - 2b - 1);
// //     sp (cg - 1) + 2b + 1 = sg;
//     if(v1c_specs.sg4) {
//       taVector2i v1csg_fm_v1c;
// //       v1sg_fm_v1c = v1c_specs.spat_spacing * (v1c_img_geom - 1) + 2 * v1c_specs.spat_border + 1;
//       v1s_fm_v1c = v1c_specs.sg_spacing * (v1sg_fm_v1c - 1) + 2 * v1c_specs.sg_border + 1;
//     }
//     else {
//       v1s_fm_v1c = v1c_specs.net_spacing * (v1c_img_geom - 1) + 2 * v1c_specs.net_border + 1;
//     }
//   }

//   if(v1s_fm_v1c != v1s_img_geom) { // mismatch!
//     taMisc::Info("V1RegionSpec:", name,
//               "v1s_img_geom:", v1s_img_geom.GetStr(),
//               "is not an even multiple of v1c_specs.net_spacing:",
//               v1c_specs.net_spacing.GetStr(),
//               "this geometry is:", v1s_fm_v1c.GetStr(),
//               "Now recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
//     v1s_img_geom = v1s_fm_v1c;
//     redo = true;             // recompute from here
//   }

//   taVector2i inp_fm_v1s;
//   if(region.edge_mode == VisRegionParams::WRAP) {
//     inp_fm_v1s = v1s_img_geom * v1s_specs.spacing;
//   }
//   else {
//     inp_fm_v1s = v1s_specs.spacing * (v1s_img_geom - 1) + 1;
//   }

//   if(inp_fm_v1s != input_size.input_size) { // mismatch!
//     if(!redo) {                         // only err if not already redoing
//       taMisc::Info("V1RegionSpec:", name,
//                 "input_size:", input_size.input_size.GetStr(),
//                 "is not an even multiple of v1s_specs.spacing:", String(v1s_specs.spacing),
//                 "this geometry is:", inp_fm_v1s.GetStr(),
//                 "Recomputing image size to fit this -- you might want to increment by some multiple of spacing to get closer to desired input size");
//     }
//     input_size.input_size = inp_fm_v1s;
//     input_size.retina_size = input_size.input_size + 2 * input_size.border;
//     redo = true;             // recompute from here
//   }
}

bool V1RegionSpec::InitFilters() {
  inherited::InitFilters();
  InitFilters_V1Simple();
  if(motion_frames > 1)
    InitFilters_V1Motion();
  if(region.ocularity == VisRegionParams::BINOCULAR)
    InitFilters_V1Binocular();
  InitFilters_V1Complex();
  InitFilters_V2();
  InitFilters_SpatInteg();
  return true;
}

bool V1RegionSpec::InitFilters_V1Simple() {

  v1s_specs.RenderFilters(v1s_gabor_filters);

  // config: x,y coords by tot_ni_len, by angles
  v1s_ni_stencils.SetGeom(3, 2, v1s_neigh_inhib.tot_ni_len, v1s_specs.n_angles);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int lpt=-v1s_neigh_inhib.inhib_d; lpt <= v1s_neigh_inhib.inhib_d; lpt++) {
      int lpdx = lpt + v1s_neigh_inhib.inhib_d;
      v1s_ni_stencils.FastEl3d(X, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(X, ORTHO, ang)); // ortho
      v1s_ni_stencils.FastEl3d(Y, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang));
    }
  }

  if(motion_frames <= 1) {
    v1m_stencils.SetGeom(1,1);
    v1m_still_stencils.SetGeom(1,1);
  }

  return true;
}

bool V1RegionSpec::InitFilters_V1Motion() {
  v1m_stencils.SetGeom(6, 2, v1s_motion.tot_width, motion_frames, 2,
                       v1s_specs.n_angles, v1s_motion.n_speeds);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int dir = 0; dir < 2; dir++) { // directions
      float dirsign = (dir == 0) ? -1.0f : 1.0f; // direction sign for multiplying times slope values
      float dx = dirsign * v1s_ang_slopes.FastEl3d(X, ORTHO, ang);
      float dy = dirsign * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang);
      for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
        int spd_off = (speed+1) * v1s_motion.speed_inc;
        for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
          for(int ew = -v1s_motion.tuning_width; ew <= v1s_motion.tuning_width; ew++) {
            int ox = taMath_float::rint((float)(spd_off*mot + ew) * dx);
            int oy = taMath_float::rint((float)(spd_off*mot + ew) * dy);
            v1m_stencils.FastEl(X, v1s_motion.tuning_width+ew, mot, dir, ang, speed) = ox;
            v1m_stencils.FastEl(Y, v1s_motion.tuning_width+ew, mot, dir, ang, speed) = oy;
          }
        }
      }
    }
  }

  v1m_still_stencils.SetGeom(4, 2, v1s_motion.tot_width, motion_frames, v1s_specs.n_angles);
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float dx = v1s_ang_slopes.FastEl3d(X, ORTHO, ang);
    float dy = v1s_ang_slopes.FastEl3d(Y, ORTHO, ang);
    for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
      for(int ew = -v1s_motion.tuning_width; ew <= v1s_motion.tuning_width; ew++) {
        int ox = taMath_float::rint((float)ew * dx);
        int oy = taMath_float::rint((float)ew * dy);
        v1m_still_stencils.FastEl4d(X, v1s_motion.tuning_width+ew, mot, ang) = ox;
        v1m_still_stencils.FastEl4d(Y, v1s_motion.tuning_width+ew, mot, ang) = oy;
      }
    }
  }

  v1m_weights.SetGeom(1, v1s_motion.tot_width);
  if(v1s_motion.tuning_width > 0) {
    int idx = 0;
    for(int x=-v1s_motion.tuning_width; x<=v1s_motion.tuning_width; x++, idx++) {
      float fx = (float)x / (float)v1s_motion.tuning_width;
      v1m_weights.FastEl1d(idx) = taMath_float::gauss_den_sig(fx, v1s_motion.gauss_sig);
    }
  }
  taMath_float::vec_norm_max(&v1m_weights); // max norm to 1
  return true;
}

bool V1RegionSpec::InitFilters_V1Binocular() {
  v1b_widths.SetGeom(1, v1b_specs.tot_disps);
  v1b_weights.SetGeom(2, v1b_specs.max_width, v1b_specs.tot_disps);
  v1b_stencils.SetGeom(2, v1b_specs.max_width, v1b_specs.tot_disps);

  v1b_weights.InitVals(0.0f);   // could have some lurkers in there from other settings, which can affect normalization

  int twe = v1b_specs.disp_range + v1b_specs.end_ext;

  // everything is conditional on the disparity
  for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
    int didx = disp + v1b_specs.n_disps;
    int doff = disp * v1b_specs.disp_spc;
    if(disp == 0) {             // focal
      v1b_widths.FastEl1d(didx) = 1 + 2 * v1b_specs.disp_range;
      for(int tw=-v1b_specs.disp_range; tw<=v1b_specs.disp_range; tw++) {
        int twidx = tw + v1b_specs.disp_range;
        float fx = (float)tw / (float)v1b_specs.disp_range;
        v1b_weights.FastEl2d(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
        v1b_stencils.FastEl2d(twidx, didx) = doff + tw;
      }
    }
    else if(disp == -v1b_specs.n_disps) {
      v1b_widths.FastEl1d(didx) = 1 + 2 * v1b_specs.disp_range + v1b_specs.end_ext;
      for(int tw=-twe; tw<=v1b_specs.disp_range; tw++) {
        int twidx = tw + twe;
        if(tw < 0)
          v1b_weights.FastEl2d(twidx, didx) = taMath_float::gauss_den_sig(0.0f, v1b_specs.gauss_sig);
        else {
          float fx = (float)tw / (float)v1b_specs.disp_range;
          v1b_weights.FastEl2d(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
        }
        v1b_stencils.FastEl2d(twidx, didx) = doff + tw;
      }
    }
    else if(disp == v1b_specs.n_disps) {
      v1b_widths.FastEl1d(didx) = 1 + 2 * v1b_specs.disp_range + v1b_specs.end_ext;
      for(int tw=-v1b_specs.disp_range; tw<=twe; tw++) {
        int twidx = tw + v1b_specs.disp_range;
        if(tw > 0)
          v1b_weights.FastEl2d(twidx, didx) = taMath_float::gauss_den_sig(0.0f, v1b_specs.gauss_sig);
        else {
          float fx = (float)tw / (float)v1b_specs.disp_range;
          v1b_weights.FastEl2d(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
        }
        v1b_stencils.FastEl2d(twidx, didx) = doff + tw;
      }
    }
    else {
      v1b_widths.FastEl1d(didx) = 1 + 2 * v1b_specs.disp_range;
      for(int tw=-v1b_specs.disp_range; tw<=v1b_specs.disp_range; tw++) {
        int twidx = tw + v1b_specs.disp_range;
        float fx = (float)tw / (float)v1b_specs.disp_range;
        v1b_weights.FastEl2d(twidx, didx) = taMath_float::gauss_den_sig(fx, v1b_specs.gauss_sig);
        v1b_stencils.FastEl2d(twidx, didx) = doff + tw;
      }
    }
  }

  taMath_float::vec_norm_max(&v1b_weights); // max norm to 1

  return true;
}

bool V1RegionSpec::InitFilters_V1Complex() {
  // sg4 guys -- center points relative to lower-left corner of 4x4 group
  v1sg_stencils.SetGeom(3, 3, 10, 4);
  // lengths stored in position 2 of first point
  v1sg_stencils.FastEl3d(2,0,0) = 8;
  v1sg_stencils.FastEl3d(2,0,1) = 10;
  v1sg_stencils.FastEl3d(2,0,2) = 8;
  v1sg_stencils.FastEl3d(2,0,3) = 10;
  for(int lpdx=0; lpdx < 10; lpdx++) {
    // 0 = 0 deg
    v1sg_stencils.FastEl3d(X, lpdx, 0) = 1 + lpdx / 4;
    v1sg_stencils.FastEl3d(Y, lpdx, 0) = lpdx % 4;
    // 1 = 45 deg
    v1sg_stencils.FastEl3d(X, lpdx, 1) = 2 + lpdx/5 - (lpdx % 5)/2;
    v1sg_stencils.FastEl3d(Y, lpdx, 1) = lpdx/5 + ((lpdx%5)+1)/2;
    // 2 = 90 deg
    v1sg_stencils.FastEl3d(X, lpdx, 2) = lpdx % 4;
    v1sg_stencils.FastEl3d(Y, lpdx, 2) = 1 + lpdx / 4;
    // 3 = 135 deg
    v1sg_stencils.FastEl3d(X, lpdx, 3) = lpdx/5 + (lpdx % 5)/2;
    v1sg_stencils.FastEl3d(Y, lpdx, 3) = (1 - lpdx/5) + ((lpdx%5)+1)/2;
  }

  // config: x,y coords by points, by angles
  v1ls_stencils.SetGeom(3, 2, v1c_specs.len_sum_width, v1s_specs.n_angles);
  v1es_stencils.SetGeom(5, 2, 3, 2, 2, v1s_specs.n_angles);

  for(int ang=0; ang < v1s_specs.n_angles; ang++) {
    for(int lpt=-v1c_specs.len_sum_len; lpt <= v1c_specs.len_sum_len; lpt++) {
      int lpdx = lpt + v1c_specs.len_sum_len;
      v1ls_stencils.FastEl3d(X, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(X, LINE, ang));
      v1ls_stencils.FastEl3d(Y, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(Y, LINE, ang));
    }

    float ls_off = (float)v1c_specs.len_sum_len;
    // center of length sum guy, "left" direction
    v1es_stencils.FastEl(X, 0, ON, LEFT, ang) =
      taMath_float::rint(-ls_off * v1s_ang_slopes.FastEl3d(X, LINE, ang));
    v1es_stencils.FastEl(Y, 0, ON, LEFT, ang) =
      taMath_float::rint(-ls_off * v1s_ang_slopes.FastEl3d(Y, LINE, ang));
    // center of length sum guy, "right" direction
    v1es_stencils.FastEl(X, 0, ON, RIGHT, ang) =
      taMath_float::rint(ls_off * v1s_ang_slopes.FastEl3d(X, LINE, ang));
    v1es_stencils.FastEl(Y, 0, ON, RIGHT, ang) =
      taMath_float::rint(ls_off * v1s_ang_slopes.FastEl3d(Y, LINE, ang));

    for(int orthdx=0; orthdx < 3; orthdx++) {
      int ortho = orthdx - 1;
      if(ang % 2 == 0) {        // vert, horiz
        // off guy, "left" direction
        v1es_stencils.FastEl(X, orthdx, OFF, LEFT, ang) =
          taMath_float::rint(v1s_ang_slopes.FastEl3d(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, LEFT, ang) =
          taMath_float::rint(v1s_ang_slopes.FastEl3d(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang));
        // off guy, "right" direction
        v1es_stencils.FastEl(X, orthdx, OFF, RIGHT, ang) =
          taMath_float::rint(-v1s_ang_slopes.FastEl3d(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, RIGHT, ang) =
          taMath_float::rint(-v1s_ang_slopes.FastEl3d(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang));
      }
      else {
        // this is an arcane formula that produces the "arrowhead" shape
        // off guy, "left" direction
        int off = (ortho == 0 ? 0 : 1);
        if(ortho < 0) ortho = 0;
        v1es_stencils.FastEl(X, orthdx, OFF, LEFT, ang) = (ang == 1 ? 0 : +off) +
          taMath_float::rint(v1s_ang_slopes.FastEl3d(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, LEFT, ang) = (ang == 1 ? -off : 0) +
          taMath_float::rint(v1s_ang_slopes.FastEl3d(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang));
        // off guy, "right" direction
        v1es_stencils.FastEl(X, orthdx, OFF, RIGHT, ang) = (ang == 1 ? +off : 0) +
          taMath_float::rint(-v1s_ang_slopes.FastEl3d(X, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(X, ORTHO, ang));
        v1es_stencils.FastEl(Y, orthdx, OFF, RIGHT, ang) = (ang == 1 ? 0 : +off) +
          taMath_float::rint(-v1s_ang_slopes.FastEl3d(Y, LINE, ang)) +
          taMath_float::rint((float)ortho * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang));
      }
    }
  }

  // config: x,y coords by tot_ni_len, by angles
  // todo: also include close-in down/up of diagonal!
  v1ls_ni_stencils.SetGeom(3, 2, v1ls_neigh_inhib.tot_ni_len, v1s_specs.n_angles);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int lpt=-v1ls_neigh_inhib.inhib_d; lpt <= v1ls_neigh_inhib.inhib_d; lpt++) {
      int lpdx = lpt + v1ls_neigh_inhib.inhib_d;
      v1ls_ni_stencils.FastEl3d(X, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(X, ORTHO, ang)); // ortho
      v1ls_ni_stencils.FastEl3d(Y, lpdx, ang) =
        taMath_float::rint((float)lpt * v1s_ang_slopes.FastEl3d(Y, ORTHO, ang));
    }
  }

  return true;
}

bool V1RegionSpec::InitFilters_V2() {
  v2tl_stencils.SetGeom(3, 3, 2, 4);

  // NOTE: first row in figures is LEFT, 2nd row is RIGHT

  // only specify the T, LEFT = L, RIGHT case -- just reverse the DIR for T, RIGHT and L, LEFT

  // T: ANG_0
  v2tl_stencils.FastEl3d(0,ANG,ANG_0) = ANG_90;
  v2tl_stencils.FastEl3d(0,DIR,ANG_0) = LEFT;
  v2tl_stencils.FastEl3d(1,ANG,ANG_0) = ANG_135;
  v2tl_stencils.FastEl3d(1,DIR,ANG_0) = LEFT;
  v2tl_stencils.FastEl3d(2,ANG,ANG_0) = ANG_45;
  v2tl_stencils.FastEl3d(2,DIR,ANG_0) = LEFT;

  // T: ANG_45
  v2tl_stencils.FastEl3d(0,ANG,ANG_45) = ANG_135;
  v2tl_stencils.FastEl3d(0,DIR,ANG_45) = LEFT;
  v2tl_stencils.FastEl3d(1,ANG,ANG_45) = ANG_0;
  v2tl_stencils.FastEl3d(1,DIR,ANG_45) = RIGHT;
  v2tl_stencils.FastEl3d(2,ANG,ANG_45) = ANG_90;
  v2tl_stencils.FastEl3d(2,DIR,ANG_45) = LEFT;

  // T: ANG_90
  v2tl_stencils.FastEl3d(0,ANG,ANG_90) = ANG_0;
  v2tl_stencils.FastEl3d(0,DIR,ANG_90) = RIGHT;
  v2tl_stencils.FastEl3d(1,ANG,ANG_90) = ANG_135;
  v2tl_stencils.FastEl3d(1,DIR,ANG_90) = LEFT;
  v2tl_stencils.FastEl3d(2,ANG,ANG_90) = ANG_45;
  v2tl_stencils.FastEl3d(2,DIR,ANG_90) = RIGHT;

  // T: ANG_135
  v2tl_stencils.FastEl3d(0,ANG,ANG_135) = ANG_45;
  v2tl_stencils.FastEl3d(0,DIR,ANG_135) = RIGHT;
  v2tl_stencils.FastEl3d(1,ANG,ANG_135) = ANG_0;
  v2tl_stencils.FastEl3d(1,DIR,ANG_135) = RIGHT;
  v2tl_stencils.FastEl3d(2,ANG,ANG_135) = ANG_90;
  v2tl_stencils.FastEl3d(2,DIR,ANG_135) = RIGHT;

  int max_cnt = 4 * v2_ffbo.radius * v2_ffbo.radius;
  v2ffbo_stencils.SetGeom(6, 2, max_cnt, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  v2ffbo_weights.SetGeom(5, max_cnt, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  v2ffbo_stencil_n.SetGeom(4, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  v2ffbo_norms.SetGeom(4, 2, v1s_specs.n_angles, 2, v1s_specs.n_angles);
  taVector2i suc;                        // send coords
  for(int rang_dx = 0; rang_dx < v1s_specs.n_angles; rang_dx++) {
    for(int rdir = 0; rdir < 2; rdir++) {
      for(int sang_dx = 0; sang_dx < v1s_specs.n_angles; sang_dx++) {
        for(int sdir = 0; sdir < 2; sdir++) { // integrate over sending directions
          int cnt = 0;
          for(suc.y = -v2_ffbo.radius; suc.y <= v2_ffbo.radius; suc.y++) {
            for(suc.x = -v2_ffbo.radius; suc.x <= v2_ffbo.radius; suc.x++) {
              float netwt = V2FFBoWt(suc, rang_dx, sang_dx, rdir, sdir);
              if(netwt < v2_ffbo.con_thr) continue;
              v2ffbo_stencils.FastEl(X, cnt, sdir, sang_dx, rdir, rang_dx) = suc.x;
              v2ffbo_stencils.FastEl(Y, cnt, sdir, sang_dx, rdir, rang_dx) = suc.y;
              v2ffbo_weights.FastEl(cnt, sdir, sang_dx, rdir, rang_dx) = netwt;
              cnt++;
              if(cnt >= max_cnt) {
                taMisc::Error("cnt >= max_cnt:", String(max_cnt),"in V2FFBo stencil alloc -- programmer error -- please submit bug report");
                return false;
              }
            }
          }
          v2ffbo_stencil_n.FastEl4d(sdir, sang_dx, rdir, rang_dx) = cnt;
          if(cnt > 0)
            v2ffbo_norms.FastEl4d(sdir, sang_dx, rdir, rang_dx) = 1.0f / (float)cnt;
        }
      }
    }
  }

  return true;
}

float V1RegionSpec::V2FFBoWt(taVector2i& suc, int rang_dx, int sang_dx, int rdir, int sdir) {
  float n_angles = v1s_specs.n_angles;

  // integer angles -- useful for precise conditionals..
  int rang_n = rang_dx + rdir * 4;
  int sang_n = sang_dx + sdir * 4;
  int dang_n;
  if(sang_n < rang_n)
    dang_n = (8 + sang_n) - rang_n;
  else
    dang_n = sang_n - rang_n;
  int dang_n_pi = dang_n;
  if(dang_n >= 4) dang_n_pi = 8 - dang_n;
  int abs_dang_n_pi = dang_n_pi < 0 ? -dang_n_pi : dang_n_pi;

//   if(dang_n == 0) return 0.0f;       // no self-line guys
  if(!v2_ffbo.opp_on && dang_n == 4) return 0.0f;       // no opposite angle cons

  taVector2i del = suc;          // assume 0,0 ruc
  float dst = del.Mag();
  if(dst > (float)v2_ffbo.radius) return 0.0f;
  if(dst == 0.0f) return 0.0f;  // no self con
  float nrmdst = dst / (float)v2_ffbo.radius;

  float gang = atan2f(del.y, del.x); // group angle -- 0..pi or -pi
  if(gang < 0.0f) gang += 2.0f * taMath_float::pi; // keep it positive

  // dir 0 = 0..pi, dir 1 = pi..2pi
  float rang = taMath_float::pi * ((float)rang_dx / n_angles) + taMath_float::pi * (float)rdir;
  float sang = taMath_float::pi * ((float)sang_dx / n_angles) + taMath_float::pi * (float)sdir;

  float dang;                   // delta-angle -- keep this positive too
  if(sang < rang)
    dang = (2.0f * taMath_float::pi + sang) - rang;
  else
    dang = sang - rang;
  float dang_pi = dang; // this determines type of projection -- equal fabs(dang_pi) are same type
  if(dang >= taMath_float::pi) dang_pi = (2.0f * taMath_float::pi) - dang;
  float abs_dang_pi = fabs(dang_pi);

  float op_mag = 0.0f;
  if(abs_dang_pi < 0.499f * taMath_float::pi)
    op_mag = ((0.5f * taMath_float::pi - abs_dang_pi) / (0.5f * taMath_float::pi)); // 1 for 0, .5 for 45

  float tang = rang + 0.5f * dang; // target angle

  float gtang = gang - tang;
  if(gtang > taMath_float::pi) gtang -= 2.0f * taMath_float::pi;
  if(gtang < -taMath_float::pi) gtang += 2.0f * taMath_float::pi;

  // make symmetric around half sphere
  bool op_side = false;
  if(gtang > taMath_float::pi * 0.5f) { gtang -= taMath_float::pi; op_side = true; }
  if(gtang < -taMath_float::pi * 0.5f){ gtang += taMath_float::pi; op_side = true; }

  float eff_mag = 1.0f;
  if(abs_dang_pi > 0.501f * taMath_float::pi) eff_mag = v2_ffbo.weak_mag;

  float netwt = eff_mag * taMath_float::gauss_den_nonorm(gtang, v2_ffbo.ang_sig) *
    taMath_float::gauss_den_nonorm(nrmdst, v2_ffbo.dist_sig);

  if(op_side)
    netwt *= op_mag;

  if(v2_ffbo.t_on && abs_dang_n_pi == 2 && dst <= 2.9f) {
    float grang = gang - rang;
    if(fabsf(grang - (1.5f * taMath_float::pi)) < .1f ||
       fabsf(grang - (-0.5f * taMath_float::pi)) < .1f) {
      netwt = 1.0f;
    }
  }

  return netwt;
}

bool V1RegionSpec::InitFilters_SpatInteg() {
  if(si_specs.spat_rf.MaxVal() > 1) {
    taMath_float::vec_kern2d_gauss(&si_weights, si_specs.spat_rf.x,
                                   si_specs.spat_rf.y, si_specs.gauss_sig,
                                   si_specs.gauss_sig);
    taMath_float::vec_norm_max(&si_weights, 1.0f); // max, not sum
  }
  else {
    si_weights.SetGeom(2, 1,1);
    si_weights.FastEl2d(0,0) = 1.0f;
  }
  return true;
}


bool V1RegionSpec::InitOutMatrix() {
  inherited::InitOutMatrix();

  ///////////////////  V1S Output ////////////////////////
  v1s_out_r.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    v1s_out_l.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
  else
    v1s_out_l.SetGeom(1,1);     // free memory
  v1s_out_r_raw.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    v1s_out_l_raw.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);

  v1pi_out_r.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    v1pi_out_l.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
  else
    v1pi_out_l.SetGeom(1,1);

  ///////////////////  V1M Output ////////////////////////
  if(motion_frames > 1) {
    v1m_out_r.SetGeom(4, v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_out_l.SetGeom(4, v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_out_l.SetGeom(1,1);   // free memory

    v1m_maxout_r.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_maxout_l.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_maxout_l.SetGeom(1,1);        // free memory

    // hist -- only saves on/off luminance
    v1m_hist_r.SetGeom(5, v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y,
                       motion_frames);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_hist_l.SetGeom(5, v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y,
                         motion_frames);
    else
      v1m_hist_l.SetGeom(1,1);  // free memory

    // still filters on top of history
    v1m_still_r.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      v1m_still_l.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_still_l.SetGeom(1,1); // free memory
  }
  else {
    v1m_out_r.SetGeom(1,1);
    v1m_out_l.SetGeom(1,1);
    v1m_hist_r.SetGeom(1,1);
    v1m_hist_l.SetGeom(1,1);
    v1m_still_r.SetGeom(1,1);
    v1m_still_l.SetGeom(1,1);
  }
  v1m_circ_r.matrix = &v1m_hist_r;
  v1m_circ_l.matrix = &v1m_hist_l;
  v1m_circ_r.Reset();
  v1m_circ_l.Reset();

  ///////////////////  V1B Output ////////////////////////
  if(region.ocularity == VisRegionParams::BINOCULAR) {
    v1b_dsp_out.SetGeom(4, v1b_feat_geom.x, v1b_feat_geom.y,
                        v1s_img_geom.x, v1s_img_geom.y);
    v1b_dsp_out_tmp.SetGeom(4, v1b_feat_geom.x, v1b_feat_geom.y,
                            v1s_img_geom.x, v1s_img_geom.y);


    v1b_dsp_horiz.SetGeom(3, DHZ_N, v1s_img_geom.x, v1s_img_geom.y);
  }
  else {
    v1b_dsp_out.SetGeom(1,1);
    v1b_dsp_out_tmp.SetGeom(1,1);
    v1b_dsp_horiz.SetGeom(1,1);
  }

  ///////////////////  V1C Output ////////////////////////
  if(v1c_filters & LEN_SUM) {
    v1sg_out.SetGeom(4, v1s_feat_geom.x, 1, v1sg_img_geom.x, v1sg_img_geom.y);
    v1ls_out.SetGeom(4, v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
    v1ls_out_raw.SetGeomN(v1ls_out.geom);
    if(v1c_filters & END_STOP) {
      v1es_out.SetGeom(4, v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
    }
    else {
      v1es_out.SetGeom(1,1);
    }
  }
  else {
    v1sg_out.SetGeom(1,1);
    v1ls_out.SetGeom(1,1);
    v1ls_out_raw.SetGeom(1,1);
  }

  ///////////////////  V2 Output ////////////////////////
  if(v2_filters & V2_TL) {
    v2tl_out.SetGeom(4, v1c_feat_geom.x, 4, v1c_img_geom.x, v1c_img_geom.y);
    v2tl_max.SetGeom(2, v1c_img_geom.x, v1c_img_geom.y);
  }
  else {
    v2tl_out.SetGeom(1,1);
    v2tl_max.SetGeom(1,1);
  }

  if(v2_filters & V2_BO) {
    v2bo_out.SetGeom(4, v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
    v2bo_lat.SetGeom(4, v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
    if(v2_save & SAVE_DEBUG && taMisc::gui_active)
      v2bos_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y*2, v1s_img_geom.x, v1s_img_geom.y);
    else
      v2bos_out.SetGeom(1,1);
  }
  else {
    v2bo_out.SetGeom(1,1);
    v2bo_lat.SetGeom(1,1);
    v2bos_out.SetGeom(1,1);
  }

  ////////////  Spat Integ

  if(spat_integ & SI_V1S) {
    si_v1s_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, si_v1s_geom.x, si_v1s_geom.y);
    si_v1s_out_raw.SetGeomN(si_v1s_out.geom);
  }
  else {
    si_v1s_out.SetGeom(1,1);
    si_v1s_out_raw.SetGeom(1,1);
  }
  if(spat_integ & SI_V1PI) {
    si_v1pi_out.SetGeom(4, v1s_feat_geom.x, 1, si_v1s_geom.x, si_v1s_geom.y);
    si_v1pi_out_raw.SetGeomN(si_v1pi_out.geom);
  }
  else {
    si_v1pi_out.SetGeom(1,1);
    si_v1pi_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V1PI_SG) {
    si_v1pi_sg_out.SetGeom(4, v1s_feat_geom.x, 1, si_v1sg_geom.x, si_v1sg_geom.y);
    si_v1pi_sg_out_raw.SetGeomN(si_v1pi_sg_out.geom);
  }
  else {
    si_v1pi_sg_out.SetGeom(1,1);
    si_v1pi_sg_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V1S_SG) {
    v1s_sg_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1sg_img_geom.x, v1sg_img_geom.y);
    si_v1s_sg_out.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, si_v1sg_geom.x, si_v1sg_geom.y);
    si_v1s_sg_out_raw.SetGeomN(si_v1s_sg_out.geom);
  }
  else {
    v1s_sg_out.SetGeom(1,1);
    si_v1s_sg_out.SetGeom(1,1);
    si_v1s_sg_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V1C) {
    if(spat_integ & SI_V1S_SG) { // special case -- combine both
      si_v1c_out.SetGeom(4, v1c_feat_geom.x, v1s_feat_geom.y + v1c_feat_geom.y, si_v1c_geom.x, si_v1c_geom.y);
      si_v1c_out_raw.SetGeomN(si_v1c_out.geom);
    }
    else {
      si_v1c_out.SetGeom(4, v1c_feat_geom.x, v1c_feat_geom.y, si_v1c_geom.x, si_v1c_geom.y);
      si_v1c_out_raw.SetGeomN(si_v1c_out.geom);
    }
  }
  else {
    si_v1c_out.SetGeom(1,1);
    si_v1c_out_raw.SetGeom(1,1);
  }

  if(spat_integ & SI_V2BO) {
    si_v2bo_out.SetGeom(4, v1c_feat_geom.x, 2, si_v1c_geom.x, si_v1c_geom.y);
    si_v2bo_out_raw.SetGeomN(si_v2bo_out.geom);
  }
  else {
    si_v2bo_out.SetGeom(1,1);
    si_v2bo_out_raw.SetGeom(1,1);
  }

  ///////////////////  OPT Output ////////////////////////
  if(opt_filters & ENERGY) {
    energy_out.SetGeom(2, v1s_img_geom.x, v1s_img_geom.y);
  }
  else {
    energy_out.SetGeom(1,1);
  }

  v1b_avgsum_out = 0.0f;

  return true;
}

void V1RegionSpec::IncrTime() {
  inherited::IncrTime();

  if(motion_frames <= 1) {
    return;             // nop
  }
  else {
    v1m_circ_r.CircAddLimit(motion_frames);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      v1m_circ_l.CircAddLimit(motion_frames);
    }
  }
}

////////////////////////////////////////////////////////////////////
//      V1Region        Filtering

bool V1RegionSpec::FilterImage_impl(bool motion_only) {
  inherited::FilterImage_impl(motion_only); // do dogs first

  // todo: maybe check rval for fail and bail -- not currently used..

  wrap = (region.edge_mode == VisRegionParams::WRAP);

  bool rval = V1SimpleFilter();

  if(!motion_only) {
    if(rval && v1b_filters != BF_NONE && region.ocularity == VisRegionParams::BINOCULAR) {
      rval &= V1BinocularFilter();
    }

    if(rval && v1c_filters != CF_NONE) {
      rval &= V1ComplexFilter();
    }

    if(rval && v2_filters != V2_NONE) {
      rval &= V2Filter();
    }

    if(rval && spat_integ != SI_NONE) {
      rval &= SpatIntegFilter();
    }

    if(rval && opt_filters != OF_NONE) {
      rval &= V1OptionalFilter();
    }
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table);
  }

  if(!motion_only) {
    if(v1b_save & SAVE_DATA && v1b_filters != BF_NONE &&
       !(taMisc::gui_active && v1b_save & ONLY_GUI) &&
       (region.ocularity == VisRegionParams::BINOCULAR)) {
      V1BOutputToTable(data_table);
    }
    if(v1c_filters != CF_NONE && v1c_save & SAVE_DATA &&
       !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
      V1COutputToTable(data_table);
    }
    if(v2_filters != V2_NONE && v2_save & SAVE_DATA &&
       !(!taMisc::gui_active && v2_save & ONLY_GUI)) {
      V2OutputToTable(data_table);
    }
    if(spat_integ != SI_NONE && si_save & SAVE_DATA &&
       !(!taMisc::gui_active && si_save & ONLY_GUI)) {
      SIOutputToTable(data_table);
    }
    if(opt_filters != OF_NONE && opt_save & SAVE_DATA &&
       !(!taMisc::gui_active && opt_save & ONLY_GUI)) {
      OptOutputToTable(data_table);
    }
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter() {
  bool rval = V1SimpleFilter_Static(cur_img_r, &v1s_out_r_raw, &v1s_out_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= V1SimpleFilter_Static(cur_img_l, &v1s_out_l_raw, &v1s_out_l);
  }

  rval &= V1SimpleFilter_PolInvar(&v1s_out_r, &v1pi_out_r);
  if(rval && region.ocularity == VisRegionParams::BINOCULAR) {
    rval &= V1SimpleFilter_PolInvar(&v1s_out_l, &v1pi_out_l);
  }

  if(motion_frames > 1) {
    rval &= V1SimpleFilter_Motion(&v1pi_out_r, &v1m_out_r, &v1m_maxout_r,
                                  &v1m_still_r, &v1m_hist_r, &v1m_circ_r);
    if(rval && !v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR) {
      rval &= V1SimpleFilter_Motion(&v1pi_out_l, &v1m_out_l, &v1m_maxout_l,
                                    &v1m_still_l, &v1m_hist_l, &v1m_circ_l);
    }
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter_Static(float_Matrix* image, float_Matrix* out_raw,
                                         float_Matrix* out) {
  cur_img = image;
  rgb_img = (cur_img->dims() == 3);

  if(rgb_img) {
    ColorRGBtoCMYK(*cur_img);   // precompute!
  }

  if(v1s_kwta.on) {
    cur_out = out_raw;
    cur_out_acts = out;
  }
  else {
    cur_out = out;
  }

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_thread);
  threads.Run(&ip_call, n_run);

  if(v1s_renorm != NO_RENORM) {            // always renorm prior to any kwta
    RenormOutput(v1s_renorm, cur_out);
  }

  if(v1s_kwta.on) {
//     v1s_kwta.Compute_Kwta(*out_raw, *out, v1s_gci);
    v1s_kwta.Compute_Inhib_IThr(*out_raw, v1s_gci, v1s_ithr);
    if(v1s_neigh_inhib.on) {
      ThreadImgProcCall ip_call_ni((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread);
      threads.Run(&ip_call_ni, n_run);
    }
    else {
      v1s_kwta.Compute_Act(*out_raw, *cur_out_acts, v1s_gci);
    }
  }

  return true;
}

void V1RegionSpec::V1SimpleFilter_Static_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);
  taVector2i icc = input_size.border + v1s_specs.spacing * sc; // image coords center
  float_Matrix* v1s_img = cur_img;

  int ctr_off;
  if(v1s_specs.filter_size % 2 == 0)
    ctr_off = v1s_specs.filter_size / 2;
  else
    ctr_off = (v1s_specs.filter_size-1) / 2;

  icc -= ctr_off;               // always offset

  taVector2i ic;         // image coord
  for(int chan = 0; chan < n_colors; chan++) {
    ColorChannel cchan = (ColorChannel)chan;
    if(rgb_img) {
      v1s_img = GetImageForChan(cchan);
    }

    int fcy = chan * n_polarities; // starting of y axis -- add 1 for off-polarity

    for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
      float cnv_sum = 0.0f;             // convolution sum
      if(chan == 0 || rgb_img) {                // only rgb images if chan > 0
        for(int yf = 0; yf < v1s_specs.filter_size; yf++) {
          for(int xf = 0; xf < v1s_specs.filter_size; xf++) {
            ic.y = icc.y + yf;
            ic.x = icc.x + xf;
            if(ic.WrapClip(wrap, input_size.retina_size)) {
              if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
            }
            cnv_sum += v1s_gabor_filters.FastEl3d(xf, yf, ang) * v1s_img->FastEl2d(ic.x, ic.y);
          }
        }
      }
      cnv_sum *= v1s_specs.gain;
      if(cnv_sum >= 0.0f) {
        cur_out->FastEl4d(ang, fcy, sc.x, sc.y) = cnv_sum; // on-polarity
        cur_out->FastEl4d(ang, fcy+1, sc.x, sc.y) = 0.0f;
      }
      else {
        cur_out->FastEl4d(ang, fcy, sc.x, sc.y) = 0.0f;
        cur_out->FastEl4d(ang, fcy+1, sc.x, sc.y) = -cnv_sum; // off-polarity
      }
    }
  }
}

void V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  float gi = v1s_gci.FastEl2d(sc.x, sc.y);

  taVector2i oc;         // other coord
  for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      float raw = cur_out->FastEl4d(ang, polclr, sc.x, sc.y);
      float feat_inhib_max = 0.0f;
      for(int lpdx=0; lpdx < v1s_neigh_inhib.tot_ni_len; lpdx++) { // go out to neighs
        if(lpdx == v1s_neigh_inhib.inhib_d) continue;              // skip self
        int xp = v1s_ni_stencils.FastEl3d(X,lpdx,ang);
        int yp = v1s_ni_stencils.FastEl3d(Y,lpdx,ang);
        oc.x = sc.x + xp;
        oc.y = sc.y + yp;
        if(oc.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float oth_ithr = v1s_ithr.FastEl4d(ang, polclr, oc.x, oc.y); // other guy
        // weights already have gain factor built in
        float ogi = v1s_neigh_inhib.inhib_g * oth_ithr; // note: directly on ithr!
        feat_inhib_max = MAX(feat_inhib_max, ogi);
      }

      float ge = v1s_kwta.g_bar_e * raw;
      float gi_eff = MAX(gi, feat_inhib_max);
      float act = v1s_kwta.Compute_ActFmIn(ge, gi_eff);
      cur_out_acts->FastEl4d(ang, polclr, sc.x,  sc.y) = act;
    }
  }
}

bool V1RegionSpec::V1SimpleFilter_PolInvar(float_Matrix* v1s_out_in, float_Matrix* v1pi_out) {
  cur_in = v1s_out_in;          // using img for this..
  cur_out = v1pi_out;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_PolInvar_thread);
  threads.Run(&ip_call, n_run);

  return true;
}

void V1RegionSpec::V1SimpleFilter_PolInvar_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_pi = 0.0f;
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float val = cur_in->FastEl4d(ang, polclr, sc.x, sc.y);
      max_pi = MAX(max_pi, val);
    }
    cur_out->FastEl4d(ang, 0, sc.x, sc.y) = max_pi;
  }
}


////////////////////////////////////////////////////////
//              Motion Filters

bool V1RegionSpec::V1SimpleFilter_Motion(float_Matrix* in, float_Matrix* out, float_Matrix* maxout,
                 float_Matrix* still, float_Matrix* hist, CircMatrix* circ) {
  cur_in = in;
  cur_out = out;
  cur_maxout = maxout;
  cur_still = still;
  cur_hist = hist;
  cur_circ = circ;

  int n_run = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  ThreadImgProcCall ip_cp_hist((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Motion_CpHist_thread);
  threads.Run(&ip_cp_hist, n_run);

  if(!cur_mot_only) {
    // if motion only, then really just load the history for later processing!
    ThreadImgProcCall ip_call_still((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Motion_Still_thread);
    threads.Run(&ip_call_still, n_run);

    ThreadImgProcCall ip_call_mot((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1SimpleFilter_Motion_thread);
    threads.Run(&ip_call_mot, n_run);

    if(v1m_renorm != NO_RENORM) {
      RenormOutput(v1m_renorm, out);
      RenormOutput(v1m_renorm, maxout);
    }
  }
  return true;
}

void V1RegionSpec::V1SimpleFilter_Motion_CpHist_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last();
  int mot_len = cur_circ->length;

  taVector2i mo;                 // motion offset
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float in_val = cur_in->FastEl4d(ang, 0, sc.x, sc.y);
    cur_hist->FastEl(ang, 0, sc.x, sc.y, cur_mot_idx) = in_val;
  }
}

void V1RegionSpec::V1SimpleFilter_Motion_Still_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last(); // e.g. 2
  int mot_len = cur_circ->length;             // e.g. 3

  taVector2i mo;                 // motion offset
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float cur_val = cur_hist->FastEl(ang, 0, sc.x, sc.y, cur_mot_idx);
    float min_mot = cur_val;
    if(cur_val >= v1s_motion.opt_thr) { // save time
      int mx_mot = mot_len-1; // don't go up to last value -- e.g., 2
      for(int mot = 0; mot < mx_mot; mot++) { // time steps back in time -- e.g., 0, 1
        float t_val = 0.0f;
        for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
          int twidx = v1s_motion.tuning_width+tw;
          int xp = v1m_still_stencils.FastEl4d(X, twidx, mot, ang);
          int yp = v1m_still_stencils.FastEl4d(Y, twidx, mot, ang);
          mo.x = sc.x + xp;
          mo.y = sc.y + yp;
          if(mo.WrapClip(wrap, v1s_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          int midx = cur_circ->CircIdx(mx_mot-1 - mot); // e.g., 1-0 = 1; 1-1 = 0,
          float val = cur_hist->FastEl(ang, 0, mo.x, mo.y, midx);
          val *= v1m_weights.FastEl1d(twidx);
          t_val = MAX(t_val, val);
        }
        min_mot = MIN(min_mot, t_val); // MIN = fast product
      }
    }
    cur_still->FastEl4d(ang, 0, sc.x, sc.y) = min_mot;
  }
}

void V1RegionSpec::V1SimpleFilter_Motion_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  int cur_mot_idx = cur_circ->CircIdx_Last(); // e.g., 2
  int mot_len = cur_circ->length;             // e.g., 3

  taVector2i mo;                 // motion offset
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_out = 0.0f;
    for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
      for(int dir = 0; dir < 2; dir++) { // directions
        int moty = (speed * 2 + dir);

        float cur_val = cur_hist->FastEl(ang, 0, sc.x, sc.y, cur_mot_idx);
        float still_val = cur_still->FastEl4d(ang, 0, sc.x, sc.y);
        cur_val -= still_val;   // subtract out still bg
        cur_val = MAX(cur_val, 0.0f);
        float min_mot = cur_val;
        if(cur_val >= v1s_motion.opt_thr) { // save time
          int mx_mot = mot_len-1; // don't go up to last value -- e.g., 2
          for(int mot = 0; mot < mx_mot; mot++) { // time steps back in time -- e.g., 0, 1
            float t_val = 0.0f;
            for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
              int twidx = v1s_motion.tuning_width+tw;
              int xp = v1m_stencils.FastEl(X, twidx, mot, dir, ang, speed);
              int yp = v1m_stencils.FastEl(Y, twidx, mot, dir, ang, speed);

              mo.x = sc.x + xp;
              mo.y = sc.y + yp;
              if(mo.WrapClip(wrap, v1s_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
              }
              int midx = cur_circ->CircIdx(mx_mot-1 - mot); // e.g., 1-0 = 1; 1-1 = 0,
              float val = cur_hist->FastEl(ang, 0, mo.x, mo.y, midx);
              float still_val = cur_still->FastEl4d(ang, 0, mo.x, mo.y);
              val -= still_val; // subtract out still bg
              val = MAX(val, 0.0f);
              val *= v1m_weights.FastEl1d(twidx);
              t_val = MAX(t_val, val);
            }
            min_mot = MIN(min_mot, t_val); // MIN = fast product
          }
        }
        cur_out->FastEl4d(ang, moty, sc.x, sc.y) = min_mot;
        max_out = MAX(max_out, min_mot);
      }
    }
    cur_maxout->FastEl4d(ang, 0, sc.x, sc.y) = max_out;
  }
}


////////////////////////////////////////////////////////
//              Binocular Filters

bool V1RegionSpec::V1BinocularFilter() {
  int n_run_s = v1s_img_geom.Product();
  int n_run_pre = v1sg_img_geom.Product();
  int n_run_c = v1c_img_geom.Product();

  threads.n_threads = MIN(n_run_s, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  // basic disparity matching computation -- MIN(Left, Right)
  if(v1b_specs.mot_in && motion_frames > 1) {
    cur_v1b_in_r = &v1m_maxout_r;
    cur_v1b_in_l = &v1m_maxout_l;
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_MinLr_thread);
    threads.Run(&ip_call, n_run_s);
  }
  else {
    cur_v1b_in_r = &v1pi_out_r;
    cur_v1b_in_l = &v1pi_out_l;
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_MinLr_thread);
    threads.Run(&ip_call, n_run_s);
  }

  if(v1b_specs.fix_horiz) {
    // first tag horiz line elements in parallel
    v1b_dsp_horiz.InitVals(-1);
    ThreadImgProcCall ip_call_horiz((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1BinocularFilter_HorizTag_thread);
    threads.Run(&ip_call_horiz, n_run_s);

    V1BinocularFilter_HorizAgg();       // then aggregate and correct disparity
  }

  if(v1b_renorm != NO_RENORM) {
    RenormOutput(v1b_renorm, &v1b_dsp_out);
  }

  // optional outputs
  if(v1b_filters & V1B_AVGSUM) {
    V1BinocularFilter_AvgSum();
  }
  return true;
}

void V1RegionSpec::V1BinocularFilter_MinLr_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  taVector2i bo;
  bo.y = sc.y;

  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    int dwd = v1b_widths.FastEl1d(didx);
    for(int ang = 0; ang < v1b_feat_geom.x; ang++) {
      float rv = cur_v1b_in_r->FastEl4d(ang, 0, sc.x, sc.y);
      float lval = 0.0f;
      for(int twidx = 0; twidx < dwd; twidx++) {
        int off = v1b_stencils.FastEl2d(twidx, didx);
        bo.x = sc.x - off;
        if(bo.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float lv = cur_v1b_in_l->FastEl4d(ang, 0, bo.x, bo.y);
        float lvwt = lv * v1b_weights.FastEl2d(twidx, didx);
        lval = MAX(lvwt, lval);                  // agg as max
      }
      float min_rl = MIN(rv, lval); // min = simple version of product..
      v1b_dsp_out.FastEl4d(ang, didx, sc.x, sc.y) = min_rl;
    }
  }
}

void V1RegionSpec::V1BinocularFilter_HorizTag_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  float sum_dist = 0.0f;
  int n_sum = 0;
  // horiz value is always first in row..  get it
  float hv = v1pi_out_r.FastEl4d(0, 0, sc.x, sc.y); // note: sc
  if(hv < v1b_specs.horiz_thr) return;          // note: v1b_dsp_horiz init -1 at start
  float max_rest = 0.0f;
  for(int ang = 1; ang < v1s_feat_geom.x; ang++) {              // rest of orients
    // response is anchored at corresponding location on the right (dominant) eye
    float rv = v1pi_out_r.FastEl4d(ang, 0, sc.x, sc.y); // note: sc
    if(rv > max_rest) {
      max_rest = rv;
    }
  }
  if(hv >= max_rest) {
    v1b_dsp_horiz.FastEl3d(DHZ_LEN, sc.x, sc.y) = 1;
    v1b_dsp_horiz.FastEl3d(DHZ_START, sc.x, sc.y) = sc.x;
  }
}

void V1RegionSpec::V1BinocularFilter_HorizAgg() {
  taVector2i sc;                 // simple coords
  for(sc.y=0; sc.y<v1s_img_geom.y; sc.y++) {
    int cur_st = -1;
    int cur_len = 0;
    int cur_mode = 0;           // 0 = nothing, 1 = completing line
    bool backprop = false;
    for(sc.x=0; sc.x<v1s_img_geom.x; sc.x++) {
      int ptlen = v1b_dsp_horiz.FastEl3d(DHZ_LEN, sc.x, sc.y);
      switch(cur_mode) {
      case 0: {                 // nothing
        if(ptlen > 0) {         // got something
          cur_len = 1;
          cur_st = sc.x;
          cur_mode = 1;
        }
        break;
      }
      case 1: {                 // completing line
        if(ptlen > 0) {         // keep going
          cur_len++;
        }
        else {                  // ended
          backprop = true;      // done!
        }
        break;
      }
      }

      if(backprop || (cur_mode > 0 && sc.x == v1s_img_geom.x-1)) {
        // propagate back to all the points along the horizontal line -- this is the key routine
        // just look at start and end cases -- this is only place where there is good data
        if(cur_len > 3) {
          for(int didx = 0; didx <v1b_specs.tot_disps; didx++) {
            float avg_dsp = 0.0f;
            for(int bxi=0; bxi < cur_len; bxi += cur_len-1) {
              int bx = cur_st + bxi;
              float dval = v1b_dsp_out.FastEl4d(0, didx, bx, sc.y); // 0 = horiz
              avg_dsp += dval;
            }
            avg_dsp *= 0.5f;    // 2 points, divide by 2

            // then copy out to the whole line
            for(int bxi=0; bxi < cur_len; bxi++) {
              int bx = cur_st + bxi;
              int& bptlen = v1b_dsp_horiz.FastEl3d(DHZ_LEN, bx, sc.y);
              int& bptst = v1b_dsp_horiz.FastEl3d(DHZ_START, bx, sc.y);
              bptlen = cur_len;
              bptst = cur_st;

              float& dval = v1b_dsp_out.FastEl4d(0, didx, bx, sc.y); // 0 = horiz
              dval = MIN(dval, avg_dsp);
            }
          }
        }
        // start over
        cur_st = -1;
        cur_len = 0;
        cur_mode = 0;
        backprop = false;
      }
    }
  }
}

void V1RegionSpec::V1BinocularFilter_AvgSum() {
  float norm_val = 0.0f;
  float* sums = new float[v1b_specs.tot_disps];
  for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
    sums[didx] = 0.0f;
  }

  taVector2i bc;         // binocular coords
  for(bc.y = 0; bc.y < v1s_img_geom.y; bc.y++) {
    for(bc.x = 0; bc.x < v1s_img_geom.x; bc.x++) {
      for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
        float dmax = 0.0f;
        for(int ang=0; ang < v1b_feat_geom.x; ang++) {
          float dval = v1b_dsp_out.FastEl4d(ang, didx, bc.x, bc.y);
          dmax = MAX(dval, dmax);
        }
        sums[didx] += dmax;
        norm_val += dmax;
      }
    }
  }
  // todo: could read out per didx for more info
  float dwt = 0.0f;
  if(norm_val > 0.0f) {
    for(int didx=0; didx < v1b_specs.tot_disps; didx++) {
      int disp = didx - v1b_specs.n_disps;
      sums[didx] /= norm_val;
      dwt += (float)disp * sums[didx];
    }
  }
  v1b_avgsum_out = dwt;
}


void V1RegionSpec::V1bDspCrossResMin(float extra_width, int max_extra,
                                        float pct_to_min) {
  // todo: make core routine threaded..
  RetinaProc* own = (RetinaProc*)GetOwner(&TA_RetinaProc);
  if(!own) return;
  for(int i=0; i<own->regions.size; i++) {
    V1RegionSpec* rsa = (V1RegionSpec*)own->regions.FastEl(i);
    for(int j=i+1; j<own->regions.size; j++) {
      V1RegionSpec* rsb = (V1RegionSpec*)own->regions.FastEl(j);
      V1RegionSpec* rs_sm;      // smaller
      V1RegionSpec* rs_lg;      // larger
      if(rsa->v1s_img_geom.x >= rsb->v1s_img_geom.x) {
        rs_sm = rsb; rs_lg = rsa;
      }
      else {
        rs_sm = rsa; rs_lg = rsb;
      }

      // testing
//       taVector2i sm_half = rs_sm->v1s_img_geom / 2;
//       taVector2i sm_0 = 0;

      taVector2i sm_to_lg;
      sm_to_lg = rs_lg->v1s_img_geom / rs_sm->v1s_img_geom;
      taVector2i extra;
      extra.x = (int)((float)sm_to_lg.x * extra_width + 0.5f);
      extra.y = (int)((float)sm_to_lg.y * extra_width + 0.5f);
      extra.x = MIN(max_extra, extra.x);
      extra.y = MIN(max_extra, extra.y);

      taVector2i tot_wd = sm_to_lg + extra;

      taVector2i lc;             // large coords
      taVector2i sc;             // small coords
      taVector2i xc;             // extra coords
      taVector2i alc;            // actual large coord
      for(sc.y = 0; sc.y < rs_sm->v1s_img_geom.y; sc.y++) {
        for(sc.x = 0; sc.x < rs_sm->v1s_img_geom.x; sc.x++) {
          lc = sc * sm_to_lg;
          for(int didx = 0; didx < v1b_specs.tot_disps; didx++) {
            for(int ang = 0; ang < v1b_feat_geom.x; ang++) {
              float smval = rs_sm->v1b_dsp_out.FastEl4d(ang, didx, sc.x, sc.y);
              float lmax = 0.0f;
              for(xc.y=-extra.y; xc.y<tot_wd.y; xc.y++) {
                for(xc.x=-extra.x; xc.x<tot_wd.x; xc.x++) {
                  alc = lc + xc;
                  if(alc.WrapClip(wrap, rs_lg->v1s_img_geom)) {
                    if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
                  }
                  float lval = rs_lg->v1b_dsp_out.FastEl4d(ang, didx, alc.x, alc.y);
                  lmax = MAX(lmax, lval);
                }
              }
              // soft-min function:
              float nw_val;
              if(lmax < smval) {
                nw_val = smval + pct_to_min * (lmax - smval);
              }
              else {
                nw_val = smval;
              }

//            if(sc == sm_half || sc == sm_0)
//              mn = 2.0f;      // test
              rs_sm->v1b_dsp_out_tmp.FastEl4d(ang, didx, sc.x, sc.y) = nw_val;
              // apply this result ONLY to the "core" large guys, not the extras..
              for(xc.y=0; xc.y<sm_to_lg.y; xc.y++) {
                for(xc.x=0; xc.x<sm_to_lg.x; xc.x++) {
                  alc = lc + xc;
                  float lval = rs_lg->v1b_dsp_out.FastEl4d(ang, didx, alc.x, alc.y);
                  // soft-min function:
                  float nw_val;
                  if(smval < lval) {
                    nw_val = lval + pct_to_min * (smval - lval);
                  }
                  else {
                    nw_val = lval;
                  }
                  rs_lg->v1b_dsp_out_tmp.FastEl4d(ang, didx, alc.x, alc.y) = nw_val;
                  // *never* create new feature activation beyond what is present in large guys
                }
              }
            }
          }
        }
      }
    }
  }

  for(int i=0; i<own->regions.size; i++) {
    V1RegionSpec* rsa = (V1RegionSpec*)own->regions.FastEl(i);
    rsa->v1b_dsp_out.CopyFrom(&rsa->v1b_dsp_out_tmp); // get tmp vals after all over
    // re-output v1b_dsp_out
    if(!rsa->data_table || rsa->save_mode == NONE_SAVE) // bail now
      continue;
    rsa->V1BOutputToTable(rsa->data_table);
  }
}


//////////////////////////////////////////////////////////////////////
//              Complex Filters

bool V1RegionSpec::V1ComplexFilter() {
  int n_run = v1c_img_geom.Product();
  int n_run_sg = v1sg_img_geom.Product();
  int n_run_v1s = v1s_img_geom.Product();

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  // first, square-group to optimize computation
  if(v1c_specs.sg4) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_SqGp4_thread);
    threads.Run(&ip_call, n_run_sg);
    cur_in = &v1sg_out;
  }
  else {
    cur_in = &v1pi_out_r;
  }

  if(v1c_filters & LEN_SUM) {
    if(v1ls_kwta.on)
      cur_out = &v1ls_out_raw;
    else
      cur_out = &v1ls_out;

    ThreadImgProcCall ip_call_ls((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_LenSum_thread);
    threads.Run(&ip_call_ls, n_run);

    // always renorm *prior* to any kwta
    if(v1c_renorm != NO_RENORM) {
      RenormOutput(v1c_renorm, cur_out);
    }

    if(v1ls_kwta.on) {
//       v1ls_kwta.Compute_Kwta(v1ls_out_raw, v1ls_out, v1ls_gci);
      v1ls_kwta.Compute_Inhib_IThr(v1ls_out_raw, v1ls_gci, v1ls_ithr);
      if(v1ls_neigh_inhib.on) {
        ThreadImgProcCall ip_call_ni((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_LenSum_neighinhib_thread);
        threads.Run(&ip_call_ni, n_run);
      }
      else {
        v1ls_kwta.Compute_Act(v1ls_out_raw, v1ls_out, v1ls_gci);
      }
    }

    if(v1c_filters & END_STOP) {
      ThreadImgProcCall ip_call_es((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1ComplexFilter_EndStop_thread);
      threads.Run(&ip_call_es, n_run);
    }
  }
  return true;
}

void V1RegionSpec::V1ComplexFilter_SqGp4_thread(int v1sg_idx, int thread_no) {
  taVector2i pc;                 // pre coords
  pc.SetFmIndex(v1sg_idx, v1sg_img_geom.x);
  taVector2i scs = v1c_specs.sg_spacing * pc; // v1s coords start
  scs += v1c_specs.sg_border;
  scs -= v1c_specs.sg_half; // convert to lower-left starting position, not center

  taVector2i sc;                 // simple coord
  taVector2i scc;                // simple coord, center
  for(int ang=0; ang<v1s_specs.n_angles; ang++) {
    float max_rf = 0.0f;   // max over spatial rfield
    int nctrs = v1sg_stencils.FastEl3d(2, 0, ang);         // length stored here
    for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
      int xp = v1sg_stencils.FastEl3d(X, ctrdx, ang);
      int yp = v1sg_stencils.FastEl3d(Y, ctrdx, ang);
      sc.y = scs.y + yp;
      sc.x = scs.x + xp;
      scc = sc; // center
      if(scc.WrapClip(wrap, v1s_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float ctr_val = v1pi_out_r.FastEl4d(ang, 0, scc.x, scc.y); // gets from polinvar
      max_rf = MAX(max_rf, ctr_val);
    }
    v1sg_out.FastEl4d(ang, 0, pc.x, pc.y) = max_rf;
  }
}

void V1RegionSpec::V1ComplexFilter_LenSum_thread(int v1c_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  taVector2i lc;         // line coord
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float line_sum = 0.0f;
    for(int lpdx=0; lpdx < v1c_specs.len_sum_width; lpdx++) {
      lc.x = cc.x + v1ls_stencils.FastEl3d(X,lpdx,ang);
      lc.y = cc.y + v1ls_stencils.FastEl3d(Y,lpdx,ang);
      if(lc.WrapClip(wrap, v1c_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float lval = cur_in->FastEl4d(ang, 0, lc.x, lc.y);
      line_sum += lval;
    }
    line_sum *= v1c_specs.len_sum_norm;
    cur_out->FastEl4d(ang, 0, cc.x, cc.y) = line_sum;
  }
}

void V1RegionSpec::V1ComplexFilter_LenSum_neighinhib_thread(int v1c_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  float gi = v1ls_gci.FastEl2d(cc.x, cc.y);

  taVector2i oc;         // other coord
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float raw = cur_out->FastEl4d(ang, 0, cc.x, cc.y);
    float feat_inhib_max = 0.0f;
    for(int lpdx=0; lpdx < v1ls_neigh_inhib.tot_ni_len; lpdx++) { // go out to neighs
      if(lpdx == v1ls_neigh_inhib.inhib_d) continue;               // skip self
      int xp = v1ls_ni_stencils.FastEl3d(X,lpdx,ang);
      int yp = v1ls_ni_stencils.FastEl3d(Y,lpdx,ang);
      oc.x = cc.x + xp;
      oc.y = cc.y + yp;
      if(oc.WrapClip(wrap, v1c_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float oth_ithr = v1ls_ithr.FastEl4d(ang, 0, oc.x, oc.y); // other guy
      // weights already have gain factor built in
      float ogi = v1ls_neigh_inhib.inhib_g * oth_ithr; // note: directly on ithr!
      feat_inhib_max = MAX(feat_inhib_max, ogi);
    }

    float ge = v1ls_kwta.g_bar_e * raw;
    float gi_eff = MAX(gi, feat_inhib_max);
    float act = v1ls_kwta.Compute_ActFmIn(ge, gi_eff);
    v1ls_out.FastEl4d(ang, 0, cc.x,  cc.y) = act;
  }
}

void V1RegionSpec::V1ComplexFilter_EndStop_thread(int v1c_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  taVector2i lc;         // line coord
  taVector2i oc;         // off coord
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int dir=0; dir < 2; dir++) {                  // direction
      // len sum point
      lc.x = cc.x + v1es_stencils.FastEl(X,0,ON,dir,ang);
      lc.y = cc.y + v1es_stencils.FastEl(Y,0,ON,dir,ang);
      if(lc.WrapClip(wrap, v1c_img_geom)) {
        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
      }
      float lsval = v1ls_out.FastEl4d(ang, 0, lc.x, lc.y); // len sum

      // off point
      float max_off = 0.0f;
      for(int orthdx=0; orthdx < 3; orthdx++) {
        oc.x = cc.x + v1es_stencils.FastEl(X,orthdx,OFF,dir,ang);
        oc.y = cc.y + v1es_stencils.FastEl(Y,orthdx,OFF,dir,ang);
        if(oc.WrapClip(wrap, v1c_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float offval = cur_in->FastEl4d(ang, 0, oc.x, oc.y); // single oriented line
        max_off = MAX(offval, max_off);
      }
      float esval = lsval - max_off;
      if(esval < v1c_specs.es_thr) esval = 0.0f; // keep it real
      v1es_out.FastEl4d(ang, dir, cc.x, cc.y) = esval;
    }
  }
}


//////////////////////////////////////////////////////////////////////
//              V2 Filters

bool V1RegionSpec::V2Filter() {
  int n_run = v1c_img_geom.Product();

//   if(v2_kwta.on)
//     cur_out = &v2tl_out_raw;
//   else
//     cur_out = &v2tl_out;

  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  if(v2_filters & V2_TL) {
    ThreadImgProcCall ip_call_v2tl((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_TL_thread);
    threads.Run(&ip_call_v2tl, n_run);
  }

  if(v2_filters & V2_BO) {
    if(v2_save & SAVE_DEBUG && taMisc::gui_active)
      v2bos_out.InitVals(0.0f);
    ThreadImgProcCall ip_call_v2ffbo((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_FFBO_thread);
    threads.Run(&ip_call_v2ffbo, n_run);
    ThreadImgProcCall ip_call_v2latbo((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_LatBO_thread);
    ThreadImgProcCall ip_call_v2latbointeg((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_LatBOinteg_thread);
    for(int li=0; li < v2_specs.lat_itrs; li++) {
      threads.Run(&ip_call_v2latbo, n_run);
      threads.Run(&ip_call_v2latbointeg, n_run);
    }
    ThreadImgProcCall ip_call_v2bofinal((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V2Filter_BOfinal_thread);
    threads.Run(&ip_call_v2bofinal, n_run);
  }

//   if(v2_kwta.on) {
//     v2_kwta.Compute_Kwta(v2tl_out_raw, v2tl_out, v2tl_gci);
//   }

  return true;
}

void V1RegionSpec::V2Filter_TL_thread(int v1c_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  taVector2i lc;         // line coord
  float max_lval = 0.0f;
  float max_tval = 0.0f;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1ls_out.FastEl4d(ang, 0, cc.x, cc.y);

    // first decrement lsedge for any end stopping along it
    float lsesmax = 0.0f;
    for(int dir=0; dir < 2; dir++) {                  // direction
      for(int lpdx=0; lpdx < v1c_specs.len_sum_width; lpdx++) {
        lc.x = cc.x + v1ls_stencils.FastEl3d(X,lpdx,ang);
        lc.y = cc.y + v1ls_stencils.FastEl3d(Y,lpdx,ang);
        if(lc.WrapClip(wrap, v1c_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float esv = v1es_out.FastEl4d(ang, dir, lc.x, lc.y);
        lsesmax = MAX(lsesmax, esv);
      }
    }
    lsedge -= lsesmax;
    if(lsedge < 0.0f) lsedge = 0.0f;

    for(int dir=0; dir < 2; dir++) {                  // direction

      float ang_es = v1es_out.FastEl4d(ang, dir, cc.x, cc.y);
      float op_ang_es = v1es_out.FastEl4d(ang, 1-dir, cc.x, cc.y);
      float max_ang_es = MAX(ang_es, op_ang_es);

      float op_t_max = 0.0f;
      float op_l_max = 0.0f;
      for(int opdx = 0; opdx < 3; opdx++) {
        int op_ang = v2tl_stencils.FastEl3d(opdx, ANG, ang);
        int op_dir = v2tl_stencils.FastEl3d(opdx, DIR, ang);

        // stencil is for T, LEFT -- opposite dirs for T, RIGHT
        float op_t = v1es_out.FastEl4d(op_ang, (dir == 0 ? op_dir : 1-op_dir), cc.x, cc.y);
        // stencil is for L, RIGHT -- opposite dirs for L, LEFT
        float op_l = v1es_out.FastEl4d(op_ang, (dir == 1 ? op_dir : 1-op_dir), cc.x, cc.y);

        op_t_max = MAX(op_t_max, op_t);
        op_l_max = MAX(op_l_max, op_l);
      }
      float tval = MIN(lsedge, op_t_max); // MIN = multiply
      v2tl_out.FastEl4d(ang, dir, cc.x, cc.y) = tval;
      float lval = MIN(max_ang_es, op_l_max); // MIN = multiply
      v2tl_out.FastEl4d(ang, 2+dir, cc.x, cc.y) = lval;
      max_lval = MAX(max_lval, lval);
      max_tval = MAX(max_tval, tval);
    }
  }
  if(max_lval > v2_specs.l_t_inhib_thr) {
    max_tval = 0.0f;
    // inhibit T's in same group!
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir=0; dir < 2; dir++) {                // direction
        float& tval = v2tl_out.FastEl4d(ang, dir, cc.x, cc.y);
        tval -= max_lval;
        if(tval < 0.0f) tval = 0.0f;
        max_tval = MAX(max_tval, tval);
      }
    }
  }
  v2tl_max.FastEl2d(cc.x, cc.y) = MAX(max_lval, max_tval);
}

// todo: seems like it could do more nonlinear MIN/MAX kind of stuff for integrating?
// * diff radii for straight vs. non-straight cases!
// * multiple integration for ffstep???
void V1RegionSpec::V2Filter_FFBO_thread(int v1s_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  taVector2i lc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl4d(ang, 0, cc.x, cc.y);
    float dirmax = 0.0f;
    int maxdir = -1;
    for(int dir=0; dir < 2; dir++) {                  // direction
      if(lsedge < v2_specs.act_thr) {
        v2bo_out.FastEl4d(ang, dir, cc.x, cc.y) = lsedge;
        continue;
      }
      float dirval = 0.0f;
      for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
        float sedge = v1s_out_r.FastEl4d(ang, polclr, cc.x, cc.y);
        int pol = polclr % 2;          // polarity
        if(sedge < v2_specs.act_thr) {
          if(v2_save & SAVE_DEBUG && taMisc::gui_active)
            v2bos_out.FastEl4d(ang, polclr*2 + dir, cc.x, cc.y) = sedge;
          continue;
        }
        // compute netinput from ffbo stencils
        float netin = 0.0f;
        for(int sang = 0; sang < v1s_specs.n_angles; sang++) { // sending angles
          for(int sdir = 0; sdir < 2; sdir++) { // sending dir
            int poloff = 0;
            if(sdir != dir)
              poloff = (pol == 1) ? -1 : 1;
            int spolclr = polclr + poloff;
            if(sang == ang && spolclr == polclr) continue;       // no info from same guy
            int cnt = v2ffbo_stencil_n.FastEl4d(sdir, sang, dir, ang);
            float snetin = 0.0f;
            for(int i=0; i<cnt; i++) {
              lc.x = cc.x + v2ffbo_stencils.FastEl(X, i, sdir, sang, dir, ang);
              lc.y = cc.y + v2ffbo_stencils.FastEl(Y, i, sdir, sang, dir, ang);
              if(lc.WrapClip(wrap, v1s_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
              }
              float lsv = v1s_out_r.FastEl4d(sang, spolclr, lc.x, lc.y);
              snetin += lsv * v2ffbo_weights.FastEl(i, sdir, sang, dir, ang);
            }
            snetin *= v2_ffbo.gain * v2ffbo_norms.FastEl4d(sdir, sang, dir, ang);
            netin += snetin;
          }
        }
        float net_gain = v2_specs.ambig_gain + v2_specs.ffbo_gain * netin;
        float boval = net_gain * sedge;
        if(boval > sedge) boval = sedge;
        if(v2_save & SAVE_DEBUG && taMisc::gui_active)
          v2bos_out.FastEl4d(ang, polclr*2 + dir, cc.x, cc.y) = boval;
        dirval = MAX(dirval, boval);
      }
      if(dirval > dirmax) {
        maxdir = dir;
        dirmax = dirval;
      }
    }
    if(lsedge < v2_specs.act_thr)
      continue;
    for(int dir=0; dir < 2; dir++) {                  // direction
      if(dir == maxdir) {
        float bo = dirmax;
        if(bo > lsedge) bo = lsedge;
        v2bo_out.FastEl4d(ang, dir, cc.x, cc.y) = bo;
      }
      else {
        v2bo_out.FastEl4d(ang, dir, cc.x, cc.y) = v2_specs.ambig_gain * lsedge;
      }
    }
  }
}

void V1RegionSpec::V2Filter_LatBO_thread(int v1c_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  taVector2i lc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl4d(ang, 0, cc.x, cc.y);
    if(lsedge < v2_specs.act_thr) {
      continue;
    }
    for(int dir=0; dir < 2; dir++) {                  // direction
      float netin = 0.0f;
      int cnt = v2ffbo_stencil_n.FastEl4d(dir, ang, dir, ang);
      float snetin = 0.0f;
      for(int i=0; i<cnt; i++) {
        lc.x = cc.x + v2ffbo_stencils.FastEl(X, i, dir, ang, dir, ang);
        lc.y = cc.y + v2ffbo_stencils.FastEl(Y, i, dir, ang, dir, ang);
        if(lc.WrapClip(wrap, v1c_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float obo = v2bo_out.FastEl4d(ang, dir, lc.x, lc.y);
        if(obo > 2.0f * v2_specs.ambig_gain) // non-ambig!
          snetin += obo * v2ffbo_weights.FastEl(i, dir, ang, dir, ang);
      }
      snetin *= v2_ffbo.gain * v2ffbo_norms.FastEl4d(dir, ang, dir, ang);
      v2bo_lat.FastEl4d(ang, dir, cc.x, cc.y) = snetin;
    }
  }
}

void V1RegionSpec::V2Filter_LatBOinteg_thread(int v1c_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  taVector2i lc;
  float dirvals[2];
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl4d(ang, 0, cc.x, cc.y);
    if(lsedge < v2_specs.act_thr) {
      continue;
    }
    float dirmax = 0.0f;
    int maxdir = -1;
    for(int dir=0; dir < 2; dir++) {                  // direction
      float dirval = v2bo_lat.FastEl4d(ang, dir, cc.x, cc.y);
      dirvals[dir] = dirval;
      if(dirval > dirmax) {
        maxdir = dir;
        dirmax = dirval;
      }
    }
    if(maxdir >= 0) {
      int othdir = (maxdir == 0) ? 1 : 0;
      if(dirmax - dirvals[othdir] < v2_specs.ambig_gain) {// close
        float inc = v2_specs.lat_dt * dirvals[othdir];
        float& bo = v2bo_out.FastEl4d(ang, othdir, cc.x, cc.y);
        bo += inc;
        if(bo > lsedge) bo = lsedge;
      }
      float inc = v2_specs.lat_dt * dirmax;
      float& bo = v2bo_out.FastEl4d(ang, maxdir, cc.x, cc.y);
      bo += inc;
      if(bo > lsedge) bo = lsedge;
    }
  }
}

void V1RegionSpec::V2Filter_BOfinal_thread(int v1c_idx, int thread_no) {
  taVector2i cc;                 // complex coords
  cc.SetFmIndex(v1c_idx, v1c_img_geom.x);

  taVector2i lc;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float lsedge = v1pi_out_r.FastEl4d(ang, 0, cc.x, cc.y);
    if(lsedge < v2_specs.act_thr) {
      continue;
    }
    float dirsum = 0.0f;
    for(int dir=0; dir < 2; dir++) {                  // direction
      float dirval = v2bo_out.FastEl4d(ang, dir, cc.x, cc.y);
      dirsum += dirval;
    }
    if(dirsum > lsedge) {
      // ensure that the sum never exceeds the raw val
      float dirnorm = lsedge / dirsum;
      for(int dir=0; dir < 2; dir++) {                // direction
        float& bo = v2bo_out.FastEl4d(ang, dir, cc.x, cc.y);
        bo *= dirnorm;
      }
    }
  }
}


/////////////////////////////////////////////////////////////////////
//              Spatial Integration

bool V1RegionSpec::SpatIntegFilter() {
  int n_run_s = si_v1s_geom.Product();
  int n_run_sg = si_v1sg_geom.Product();
  int n_run_c = si_v1c_geom.Product();

  threads.n_threads = MIN(n_run_c, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  if(spat_integ & SI_V1S) {
    if(si_kwta.on) cur_out = &si_v1s_out_raw;
    else           cur_out = &si_v1s_out;
    ThreadImgProcCall ip_call_v1s((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1S_thread);
    threads.Run(&ip_call_v1s, n_run_s);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1s_out_raw, si_v1s_out, si_gci);
  }

  if(spat_integ & SI_V1PI) {
    if(si_kwta.on) cur_out = &si_v1pi_out_raw;
    else           cur_out = &si_v1pi_out;
    ThreadImgProcCall ip_call_v1pi((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1PI_thread);
    threads.Run(&ip_call_v1pi, n_run_s);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1pi_out_raw, si_v1pi_out, si_gci);
  }

  if(spat_integ & SI_V1PI_SG) {
    if(si_kwta.on) cur_out = &si_v1pi_sg_out_raw;
    else           cur_out = &si_v1pi_sg_out;
    ThreadImgProcCall ip_call_v1sg((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1PI_SG_thread);
    threads.Run(&ip_call_v1sg, n_run_sg);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1pi_sg_out_raw, si_v1pi_sg_out, si_gci);
  }

  if(spat_integ & SI_V1S_SG) {
    int n_run_sg_gp4 = v1sg_img_geom.Product();

    ThreadImgProcCall ip_call_v1ssg_pre((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1S_SqGp4_thread);
    threads.Run(&ip_call_v1ssg_pre, n_run_sg_gp4);

    if(si_kwta.on && !(spat_integ & SI_V1C)) cur_out = &si_v1s_sg_out_raw;
    else           cur_out = &si_v1s_sg_out;
    ThreadImgProcCall ip_call_v1sg((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1S_SG_thread);
    threads.Run(&ip_call_v1sg, n_run_sg);

    if(!(spat_integ & SI_V1C)) {
      if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
      if(si_kwta.on) si_kwta.Compute_Kwta(si_v1s_sg_out_raw, si_v1s_sg_out, si_gci);
    }
  }

  if(spat_integ & SI_V1C) {
    if(si_kwta.on) cur_out = &si_v1c_out_raw;
    else           cur_out = &si_v1c_out;
    ThreadImgProcCall ip_call_v1c((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V1C_thread);
    threads.Run(&ip_call_v1c, n_run_c);

    if(spat_integ & SI_V1S_SG) { // both are on -- combine output into same table prior to kwta
      taVector2i cc;
      for(cc.y = 0; cc.y < si_v1c_geom.y; cc.y++) {
        for(cc.x = 0; cc.x < si_v1c_geom.x; cc.x++) {
          for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
            for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
              float lval = si_v1s_sg_out.FastEl4d(ang, polclr, cc.x, cc.y);
              cur_out->FastEl4d(ang, v1c_feat_geom.y + polclr, cc.x, cc.y) = lval;
            }
          }
        }
      }
    }

    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v1c_out_raw, si_v1c_out, si_gci);
  }

  if(spat_integ & SI_V2BO) {
    if(si_kwta.on) cur_out = &si_v2bo_out_raw;
    else           cur_out = &si_v2bo_out;
    ThreadImgProcCall ip_call_v2bo((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::SpatIntegFilter_V2BO_thread);
    threads.Run(&ip_call_v2bo, n_run_c);
    if(si_renorm != NO_RENORM) RenormOutput(si_renorm, cur_out);
    if(si_kwta.on) si_kwta.Compute_Kwta(si_v2bo_out_raw, si_v2bo_out, si_gci);
  }

  return true;
}

void V1RegionSpec::SpatIntegFilter_V1S_thread(int v1s_idx, int thread_no) {
  taVector2i sc;
  sc.SetFmIndex(v1s_idx, si_v1s_geom.x);
  taVector2i ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  taVector2i ic;                 // input coord
  taVector2i icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1s_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val = v1s_out_r.FastEl4d(ang, polclr, icc.x, icc.y);
          val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl4d(ang, polclr, sc.x, sc.y) = max_rf;
    } // for polclr
  }  // for ang
}

void V1RegionSpec::SpatIntegFilter_V1PI_thread(int v1s_idx, int thread_no) {
  taVector2i sc;
  sc.SetFmIndex(v1s_idx, si_v1s_geom.x);
  taVector2i ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  taVector2i ic;                 // input coord
  taVector2i icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_rf = 0.0f;   // max over spatial rfield
    for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
      ic.y = ics.y + ys;
      for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
        ic.x = ics.x + xs;
        icc = ic;       // center
        if(icc.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float val = v1pi_out_r.FastEl4d(ang, 0, icc.x, icc.y);
        val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
        max_rf = MAX(max_rf, val);
      }
    }
    cur_out->FastEl4d(ang, 0, sc.x, sc.y) = max_rf;
  } // for ang
}

void V1RegionSpec::SpatIntegFilter_V1PI_SG_thread(int v1sg_idx, int thread_no) {
  taVector2i sc;
  sc.SetFmIndex(v1sg_idx, si_v1sg_geom.x);
  taVector2i ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  taVector2i ic;                 // input coord
  taVector2i icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    float max_rf = 0.0f;   // max over spatial rfield
    for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
      ic.y = ics.y + ys;
      for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
        ic.x = ics.x + xs;
        icc = ic;       // center
        if(icc.WrapClip(wrap, v1sg_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float val = v1sg_out.FastEl4d(ang, 0, icc.x, icc.y);
        val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
        max_rf = MAX(max_rf, val);
      }
    }
    cur_out->FastEl4d(ang, 0, sc.x, sc.y) = max_rf;
  } // for ang
}

void V1RegionSpec::SpatIntegFilter_V1S_SqGp4_thread(int v1sg_idx, int thread_no) {
  taVector2i pc;                 // pre coords
  pc.SetFmIndex(v1sg_idx, v1sg_img_geom.x);
  taVector2i scs = v1c_specs.sg_spacing * pc; // v1s coords start
  scs += v1c_specs.sg_border;
  scs -= v1c_specs.sg_half; // convert to lower-left starting position, not center

  taVector2i sc;                 // simple coord
  taVector2i scc;                // simple coord, center
  for(int ang=0; ang<v1s_specs.n_angles; ang++) {
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float max_rf = 0.0f;   // max over spatial rfield
      int nctrs = v1sg_stencils.FastEl3d(2, 0, ang);       // length stored here
      for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
        int xp = v1sg_stencils.FastEl3d(X, ctrdx, ang);
        int yp = v1sg_stencils.FastEl3d(Y, ctrdx, ang);
        sc.y = scs.y + yp;
        sc.x = scs.x + xp;
        scc = sc;       // center
        if(scc.WrapClip(wrap, v1s_img_geom)) {
          if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
        }
        float ctr_val = v1s_out_r.FastEl4d(ang, polclr, scc.x, scc.y);
        max_rf = MAX(max_rf, ctr_val);
      }
      v1s_sg_out.FastEl4d(ang, polclr, pc.x, pc.y) = max_rf;
    }
  }
}

void V1RegionSpec::SpatIntegFilter_V1S_SG_thread(int v1sg_idx, int thread_no) {
  taVector2i sc;
  sc.SetFmIndex(v1sg_idx, si_v1sg_geom.x);
  taVector2i ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  taVector2i ic;                 // input coord
  taVector2i icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1sg_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val = v1s_sg_out.FastEl4d(ang, polclr, icc.x, icc.y);
          val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl4d(ang, polclr, sc.x, sc.y) = max_rf;
    } // for ang
  }
}

void V1RegionSpec::SpatIntegFilter_V1C_thread(int v1c_idx, int thread_no) {
  taVector2i sc;
  sc.SetFmIndex(v1c_idx, si_v1c_geom.x);
  taVector2i ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  taVector2i ic;                 // input coord
  taVector2i icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int cfdx = 0; cfdx < v1c_feat_geom.y; cfdx++) { // cfdx features
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1c_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val;
          if(cfdx == 0)         // length-sum = 0
            val = v1ls_out.FastEl4d(ang, 0, icc.x, icc.y);
          else
            val = v1es_out.FastEl4d(ang, cfdx-1, icc.x, icc.y);
          val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl4d(ang, cfdx, sc.x, sc.y) = max_rf;
    } // for cfdx
  }  // for ang
}


void V1RegionSpec::SpatIntegFilter_V2BO_thread(int v1c_idx, int thread_no) {
  taVector2i sc;
  sc.SetFmIndex(v1c_idx, si_v1c_geom.x);
  taVector2i ics = si_specs.spat_spacing * sc; // v1s coords start
  ics += si_specs.spat_border;
  ics -= si_specs.spat_half; // convert to lower-left starting position, not center

  taVector2i ic;                 // input coord
  taVector2i icc;                // input coord, center
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
    for(int dir = 0; dir < 2; dir++) { // dir
      float max_rf = 0.0f;   // max over spatial rfield
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
        ic.y = ics.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
          ic.x = ics.x + xs;
          icc = ic;     // center
          if(icc.WrapClip(wrap, v1c_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
          }
          float val = v2bo_out.FastEl4d(ang, dir, icc.x, icc.y);
          val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
          max_rf = MAX(max_rf, val);
        }
      }
      cur_out->FastEl4d(ang, dir, sc.x, sc.y) = max_rf;
    } // for dir
  }  // for ang
}

// void V1RegionSpec::V1ComplexFilter_Blob_thread(int v1c_idx, int thread_no) {
//   taVector2i cc;                      // complex coords
//   cc.SetFmIndex(v1c_idx, v1c_img_geom.x);
//   taVector2i pcs = v1c_specs.spat_spacing * cc; // v1sg_out coords start
//   pcs += v1c_specs.spat_border;
//   pcs -= v1c_specs.spat_half; // convert to lower-left starting position, not center

//   taVector2i pc;                      // pre coord
//   taVector2i pcc;             // pre coord, center
//   taVector2i sfc;             // v1s feature coords
//   taVector2i fc;                      // v1c feature coords
//   for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features -- includes b/w on/off
//     sfc.y = polclr;
//     fc.y = v1c_feat_blob_y + polclr / v1c_feat_geom.x;
//     fc.x = polclr % v1c_feat_geom.x;
//     float max_rf = 0.0f;   // max over spatial rfield
//     for(int ys = 0; ys < v1c_specs.spat_rf.y; ys++) { // yspat
//       pc.y = pcs.y + ys;
//       for(int xs = 0; xs < v1c_specs.spat_rf.x; xs++) { // xspat
//      pc.x = pcs.x + xs;
//      pcc = pc;       // center
//      if(pcc.WrapClip(wrap, v1sg_img_geom)) {
//        if(region.edge_mode == VisRegionParams::CLIP) continue; // bail on clipping only
//      }
//      // todo: could pre-compute this as a blob_raw guy in pre coords..
//      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
//        sfc.x = ang;
//        float ctr_val = cur_v1sg_out->FastEl4d(sfc.x, sfc.y, pcc.x, pcc.y);
//        ctr_val *= v1c_weights.FastEl2d(xs, ys); // spatial rf weighting
//        max_rf = MAX(max_rf, ctr_val);
//      }
//       }
//     }
//     cur_out->FastEl4d(fc.x, fc.y, cc.x, cc.y) = max_rf;
//   }
// }

/////////////////////////////////////////////////////////////////
//              Optional Filters

bool V1RegionSpec::V1OptionalFilter() {
  int n_run = v1s_img_geom.Product();
  threads.n_threads = MIN(n_run, taMisc::thread_defaults.n_threads); // keep in range..
  threads.min_units = 1;
  threads.nibble_chunk = 1;     // small chunks

  if(opt_filters & ENERGY) {
    ThreadImgProcCall ip_call((ThreadImgProcMethod)(V1RegionMethod)&V1RegionSpec::V1OptionalFilter_Energy_thread);
    threads.Run(&ip_call, n_run);
  }

  return true;
}

void V1RegionSpec::V1OptionalFilter_Energy_thread(int v1s_idx, int thread_no) {
  taVector2i sc;                 // simple coords
  sc.SetFmIndex(v1s_idx, v1s_img_geom.x);

  float max_feat = 0.0f;
  for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // just max over angles -- blobify!
    float v1sval = v1pi_out_r.FastEl4d(ang, 0, sc.x, sc.y);
    max_feat = MAX(max_feat, v1sval);
  }

  energy_out.FastEl2d(sc.x, sc.y) = max_feat;
}


////////////////////////////////////////////////////////////////////
//      V1Region        Data Table Output

bool V1RegionSpec::InitDataTable() {
  inherited::InitDataTable();
  if(!data_table) {
    return false;
  }

  bool fmt_only = true;
  if(v1s_save & SAVE_DATA && !(!taMisc::gui_active && v1s_save & ONLY_GUI)) {
    V1SOutputToTable(data_table, fmt_only);
  }

  if(v1b_save & SAVE_DATA && v1b_filters != BF_NONE &&
     !(taMisc::gui_active && v1b_save & ONLY_GUI)
     && (region.ocularity == VisRegionParams::BINOCULAR)) {
    V1BOutputToTable(data_table, fmt_only);
  }
  if(v1c_filters != CF_NONE && v1c_save & SAVE_DATA &&
     !(!taMisc::gui_active && v1c_save & ONLY_GUI)) {
    V1COutputToTable(data_table, fmt_only);
  }
  if(v2_filters != V2_NONE && v2_save & SAVE_DATA &&
     !(!taMisc::gui_active && v2_save & ONLY_GUI)) {
    V2OutputToTable(data_table, fmt_only);
  }
  if(spat_integ != SI_NONE && si_save & SAVE_DATA &&
     !(!taMisc::gui_active && si_save & ONLY_GUI)) {
    SIOutputToTable(data_table, fmt_only);
  }
  if(opt_filters != OF_NONE && opt_save & SAVE_DATA &&
     !(!taMisc::gui_active && opt_save & ONLY_GUI)) {
    OptOutputToTable(data_table, fmt_only);
  }

  return true;
}

bool V1RegionSpec::V1SOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  int idx;
  V1SOutputToTable_impl(dtab, &v1s_out_r, "_r", fmt_only);
  if(region.ocularity == VisRegionParams::BINOCULAR)
    V1SOutputToTable_impl(dtab, &v1s_out_l, "_l", fmt_only);

  { // polarinvar
    col = data_table->FindMakeColName(name + "_v1pi_r", idx, DataTable::VT_FLOAT, 4,
                                      v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&v1pi_out_r);
    }
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      col = data_table->FindMakeColName(name + "_v1pi_l", idx, DataTable::VT_FLOAT, 4,
                                        v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr doutl; doutl = (float_Matrix*)col->GetValAsMatrix(-1);
        doutl->CopyFrom(&v1pi_out_l);
      }
    }
  }

  if(motion_frames > 1) {
    V1MOutputToTable_impl(dtab, &v1m_out_r, &v1m_maxout_r, &v1m_still_r, &v1m_hist_r, &v1m_circ_r, "_r", fmt_only);
    if(!v1s_motion.r_only && region.ocularity == VisRegionParams::BINOCULAR)
      V1MOutputToTable_impl(dtab, &v1m_out_l, &v1m_maxout_l, &v1m_still_l, &v1m_hist_l, &v1m_circ_l, "_l", fmt_only);
  }

  return true;
}

bool V1RegionSpec::V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                         const String& col_sufx, bool fmt_only) {
  taVector2i sc;         // simple coords
  DataCol* col;
  int idx;
  if(v1s_save & SEP_MATRIX) {
    { // basic luminance b/w filters
      col = data_table->FindMakeColName(name + "_v1s_bw" + col_sufx, idx, DataTable::VT_FLOAT, 4,
                                        v1s_feat_geom.x, 2, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
          for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
            for(int polclr = 0; polclr < 2; polclr++) { // polclr features -- just first 2
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                float val = out->FastEl4d(ang, polclr, sc.x, sc.y);
                dout->FastEl4d(ang, polclr, sc.x, sc.y) = val;
              }
            }
          }
        }
      }
    }
    if(region.color == VisRegionParams::COLOR) {
      col = data_table->FindMakeColName(name + "_v1s_clr" + col_sufx, idx, DataTable::VT_FLOAT, 4,
                          v1s_feat_geom.x, 4, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        for(sc.y = 0; sc.y < v1s_img_geom.y; sc.y++) {
          for(sc.x = 0; sc.x < v1s_img_geom.x; sc.x++) {
            for(int polclr = 2; polclr < n_polclr; polclr++) { // polclr features -- just color
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                float val = out->FastEl4d(ang, polclr, sc.x, sc.y);
                dout->FastEl4d(ang, polclr-2, sc.x, sc.y) = val;
              }
            }
          }
        }
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1s" + col_sufx, idx, DataTable::VT_FLOAT, 4,
              v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(out);
    }
  }

  return true;
}


bool V1RegionSpec::V1MOutputToTable_impl(DataTable* dtab, float_Matrix* out,
     float_Matrix* maxout, float_Matrix* still, float_Matrix* hist, CircMatrix* circ,
     const String& col_sufx, bool fmt_only) {
  DataCol* col;
  int idx;

  {
    col = data_table->FindMakeColName(name + "_v1m" + col_sufx, idx, DataTable::VT_FLOAT, 4,
              v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(out);
    }
  }

  {
    col = data_table->FindMakeColName(name + "_v1m_max" + col_sufx, idx, DataTable::VT_FLOAT, 4,
              v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(maxout);
    }
  }

  if(v1s_save & SAVE_DEBUG) {
    col = data_table->FindMakeColName(name + "_v1m_still" + col_sufx, idx, DataTable::VT_FLOAT,
      4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(still);
    }

    int mmax = MIN(motion_frames, circ->length);
    for(int midx=0; midx < mmax; midx++) {
      col = data_table->FindMakeColName(name + "_v1m_hist" + col_sufx + "_m" + String(midx),
                        idx, DataTable::VT_FLOAT, 4,
                        v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        float_MatrixPtr lstfrm; lstfrm = (float_Matrix*)hist->GetFrameSlice(circ->CircIdx(midx));
        dout->CopyFrom(lstfrm);
      }
    }
  }
  return true;
}

bool V1RegionSpec::V1BOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  int idx;

  if(region.ocularity == VisRegionParams::BINOCULAR) {
    if(v1b_filters & V1B_DSP) {
      col = data_table->FindMakeColName(name + "_v1b_dsp", idx, DataTable::VT_FLOAT, 4,
                                        v1b_feat_geom.x, v1b_specs.tot_disps, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1b_dsp_out);
      }
    }
  }

  if(v1b_filters & V1B_AVGSUM) {
    col = data_table->FindMakeColName(name + "_v1b_avgsum", idx, DataTable::VT_FLOAT, 2,
                                      1, 1);
    if(!fmt_only) {
      col->SetMatrixVal(v1b_avgsum_out, -1, 0, 0);
    }
  }
  return true;
}

bool V1RegionSpec::V1COutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  taVector2i cc;         // complex coords
  int idx;

  if(v1c_save & SEP_MATRIX || !(v1c_filters & END_STOP)) {
    if(v1c_filters & LEN_SUM) {
      col = data_table->FindMakeColName(name + "_v1ls", idx, DataTable::VT_FLOAT, 4,
                                        v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1ls_out);
      }
    }

    if(v1c_filters & END_STOP) {
      col = data_table->FindMakeColName(name + "_v1es", idx, DataTable::VT_FLOAT, 4,
                                        v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1es_out);
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1c", idx, DataTable::VT_FLOAT, 4,
                      v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
        for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
          for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
            float lsval = v1ls_out.FastEl4d(ang, 0, cc.x, cc.y); // len sum
            dout->FastEl4d(ang, 0, cc.x, cc.y) = lsval;
            for(int dir=0; dir < 2; dir++) {                  // direction
              float esval = v1es_out.FastEl4d(ang, dir, cc.x, cc.y);
              dout->FastEl4d(ang, 1+dir, cc.x, cc.y) = esval;
            }
          }
        }
      }
    }
  }

  if(v1c_save & SAVE_DEBUG) {
    {
      col = data_table->FindMakeColName(name + "_v1sg_out", idx, DataTable::VT_FLOAT, 4,
                                        v1s_feat_geom.x, 1, v1sg_img_geom.x, v1sg_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v1sg_out);
      }
    }
  }
  return true;
}

bool V1RegionSpec::V2OutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  taVector2i cc;         // complex coords
  int idx;

  if(v2_filters & V2_TL) {
    if(v2_save & SEP_MATRIX) {
      {
        col = data_table->FindMakeColName(name + "_v2t", idx, DataTable::VT_FLOAT, 4,
                                          v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
        if(!fmt_only) {
          float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
          for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
            for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                for(int dir=0; dir < 2; dir++) {                      // direction
                  float tval = v2tl_out.FastEl4d(ang, dir, cc.x, cc.y);
                  dout->FastEl4d(ang, dir, cc.x, cc.y) = tval;
                }
              }
            }
          }
        }
      }
      {
        col = data_table->FindMakeColName(name + "_v2l", idx, DataTable::VT_FLOAT, 4,
                                          v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
        if(!fmt_only) {
          float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
          for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
            for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
              for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
                for(int dir=0; dir < 2; dir++) {                      // direction
                  float lval = v2tl_out.FastEl4d(ang, 2+dir, cc.x, cc.y);
                  dout->FastEl4d(ang, dir, cc.x, cc.y) = lval;
                }
              }
            }
          }
        }
      }
    }
    else {
      col = data_table->FindMakeColName(name + "_v2tl", idx, DataTable::VT_FLOAT, 4,
                                        v1c_feat_geom.x, 4, v1c_img_geom.x, v1c_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v2tl_out);
      }
    }
  }

  if(v2_filters & V2_BO) {
    col = data_table->FindMakeColName(name + "_v2bo", idx, DataTable::VT_FLOAT, 4,
                      v1c_feat_geom.x, v2_specs.depths_out * 2, v1c_img_geom.x, v1c_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      if(v2_specs.depths_out == 1) {
        dout->CopyFrom(&v2bo_out);
      }
      else {
        if(v2_specs.depth_idx >= 0 && v2_specs.depth_idx < v2_specs.depths_out) {
          dout->InitVals(0.0f); // got to clear it b/c not setting everything
        }
        for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
          for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
            for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
              for(int dir=0; dir < 2; dir++) {                // direction
                float tval = v2bo_out.FastEl4d(ang, dir, cc.x, cc.y);
                if(v2_specs.depth_idx >= 0 && v2_specs.depth_idx < v2_specs.depths_out) {
                  dout->FastEl4d(ang, v2_specs.depth_idx * 2 + dir, cc.x, cc.y) = tval;
                }
                else {          // all
                  for(int depth=0; depth < v2_specs.depths_out; depth++) {
                    dout->FastEl4d(ang, depth * 2 + dir, cc.x, cc.y) = tval;
                  }
                }
              }
            }
          }
        }
      }
    }
    if(v2_save & SAVE_DEBUG && taMisc::gui_active) { // save v2bos
      col = data_table->FindMakeColName(name + "_v2bos", idx, DataTable::VT_FLOAT, 4,
                v1s_feat_geom.x, v1s_feat_geom.y*2, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        dout->CopyFrom(&v2bos_out);
      }
    }
  }

  return true;
}

bool V1RegionSpec::SIOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  taVector2i cc;         // complex coords
  int idx;

  if(spat_integ & SI_V1S) {
    col = data_table->FindMakeColName(name + "_v1s_si", idx, DataTable::VT_FLOAT, 4,
                      v1s_feat_geom.x, v1s_feat_geom.y, si_v1s_geom.x, si_v1s_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1s_out);
    }
  }
  if(spat_integ & SI_V1PI) {
    col = data_table->FindMakeColName(name + "_v1pi_si", idx, DataTable::VT_FLOAT, 4,
                      v1s_feat_geom.x, 1, si_v1s_geom.x, si_v1s_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1pi_out);
    }
  }
  if(spat_integ & SI_V1PI_SG) {
    col = data_table->FindMakeColName(name + "_v1pi_sg_si", idx, DataTable::VT_FLOAT, 4,
                      v1s_feat_geom.x, 1, si_v1sg_geom.x, si_v1sg_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1pi_sg_out);
    }
  }

  if(spat_integ & SI_V1S_SG && !(spat_integ & SI_V1C)) {
    // if both are on, then they are combined
    col = data_table->FindMakeColName(name + "_v1s_sg_si", idx, DataTable::VT_FLOAT, 4,
                                      v1s_feat_geom.x, v1s_feat_geom.y, si_v1sg_geom.x, si_v1sg_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1s_sg_out);
    }
  }
  if(spat_integ & SI_V1C) {
    col = data_table->FindMakeColName(name + "_v1c_si", idx, DataTable::VT_FLOAT, 4,
                      v1c_feat_geom.x, si_v1c_out.dim(1), si_v1c_geom.x, si_v1c_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v1c_out);
    }
  }
  if(spat_integ & SI_V2BO) {
    col = data_table->FindMakeColName(name + "_v2bo_si", idx, DataTable::VT_FLOAT, 4,
                                      v1c_feat_geom.x, 2, si_v1c_geom.x, si_v1c_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&si_v2bo_out);
    }
  }

  return true;
}



bool V1RegionSpec::OptOutputToTable(DataTable* dtab, bool fmt_only) {
  DataCol* col;
  int idx;

  if(opt_filters & ENERGY) {
    col = data_table->FindMakeColName(name + "_energy", idx, DataTable::VT_FLOAT, 2,
                      v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&energy_out);
    }
  }

  return true;
}

void V1RegionSpec::V2BoDepthFmFg(V1RetinaProc* all_flat, float fg_thr) {
  DataCol* col;
  taVector2i cc;         // complex coords
  int idx;

  // todo: could thread this..
  RetinaProc* own = (RetinaProc*)GetOwner(&TA_RetinaProc);
  if(!own || !all_flat) return;
  int mxn = MIN(own->regions.size, all_flat->regions.size);
  for(int i=0; i<mxn; i++) {
    V1RegionSpec* fgrs = (V1RegionSpec*)own->regions.FastEl(i);
    V1RegionSpec* flatrs = (V1RegionSpec*)all_flat->regions.FastEl(i);
    col = fgrs->data_table->FindMakeColName(fgrs->name + "_v2bo_fgbg", idx, DataTable::VT_FLOAT,
            4, fgrs->v1c_feat_geom.x, fgrs->v2_specs.depths_out * 2,
            fgrs->v1c_img_geom.x, fgrs->v1c_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->InitVals(0.0f);       // got to clear it b/c not setting everything
    for(cc.y = 0; cc.y < fgrs->v1c_img_geom.y; cc.y++) {
      for(cc.x = 0; cc.x < fgrs->v1c_img_geom.x; cc.x++) {
        float fg_max = 0.0f;
        for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
          for(int dir=0; dir < 2; dir++) {                    // direction
            float fgval = fgrs->v2bo_out.FastEl4d(ang, dir, cc.x, cc.y);
            fg_max = MAX(fgval, fg_max);
          }
        }
        int off = 2;                                              // bg
        if(fg_max >= fg_thr) off = 0;                             // fg
        for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
          for(int dir=0; dir < 2; dir++) {                    // direction
            float flatval = flatrs->v2bo_out.FastEl4d(ang, dir, cc.x, cc.y);
            dout->FastEl4d(ang, off + dir, cc.x, cc.y) = flatval; // fg
          }
        }
      }
    }
  }
}

void V1RegionSpec::V2BoDepthFmImgMask(DataTable* img_mask, V1RetinaProc* all_flat, float fg_thr) {
  DataCol* col;
  taVector2i cc;         // complex coords
  int idx;

  // todo: could thread this..
  RetinaProc* own = (RetinaProc*)GetOwner(&TA_RetinaProc);
  if(!own || !all_flat || !img_mask) return;
  int mxn = MIN(own->regions.size, all_flat->regions.size);
  mxn = MIN(mxn, img_mask->cols());
  for(int i=0; i<mxn; i++) {
    V1RegionSpec* fgrs = (V1RegionSpec*)own->regions.FastEl(i);
    V1RegionSpec* flatrs = (V1RegionSpec*)all_flat->regions.FastEl(i);
    DataCol* mask = img_mask->data.FastEl(i);
    float_MatrixPtr mmat; mmat = (float_Matrix*)mask->GetValAsMatrix(-1);
    col = fgrs->data_table->FindMakeColName(fgrs->name + "_v2bo_fgbg", idx, DataTable::VT_FLOAT,
            4, fgrs->v1c_feat_geom.x, fgrs->v2_specs.depths_out * 2,
            fgrs->v1c_img_geom.x, fgrs->v1c_img_geom.y);
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    dout->InitVals(0.0f);       // got to clear it b/c not setting everything
    for(cc.y = 0; cc.y < fgrs->v1c_img_geom.y; cc.y++) {
      for(cc.x = 0; cc.x < fgrs->v1c_img_geom.x; cc.x++) {
        float mask_val = mmat->FastEl2d(cc.x, cc.y);
        float fg_max = mask_val;
        if(fg_max < fg_thr) {
          for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
            for(int dir=0; dir < 2; dir++) {                  // direction
              float fgval = fgrs->v2bo_out.FastEl4d(ang, dir, cc.x, cc.y);
              fg_max = MAX(fgval, fg_max);
            }
          }
        }
        int off = 2;                                              // bg
        if(fg_max >= fg_thr) off = 0;                             // fg
        for(int ang = 0; ang < fgrs->v1s_specs.n_angles; ang++) { // angles
          for(int dir=0; dir < 2; dir++) {                    // direction
            float flatval = flatrs->v2bo_out.FastEl4d(ang, dir, cc.x, cc.y);
            dout->FastEl4d(ang, off + dir, cc.x, cc.y) = flatval; // fg
          }
        }
      }
    }
  }
}


/////////////////////////////////////////////////////
//                      Graphing

int  V1RegionSpec::AngleDeg(int ang_no) {
  int ang_inc = 180 / v1s_specs.n_angles;
  return ang_no * ang_inc;
}

void V1RegionSpec::GridGaborFilters(DataTable* graph_data) {
  v1s_specs.GridFilters(v1s_gabor_filters, graph_data);
}

void V1RegionSpec::GridV1Stencils(DataTable* graph_data) {
  Init();                       // need to init stencils for sure!

  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridV1Stencils", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();

  graph_data->SetUserData("N_ROWS", 4);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 4.0f);
  //  graph_data->SetUserData("WIDTH", .5f + (float)input_size.retina_size.x / (float)input_size.retina_size.y);

  taVector2i max_sz(v1s_specs.filter_size, v1s_specs.filter_size);
  max_sz.Max(si_specs.spat_rf);

  int bin_rf_max = 5;;
  if(region.ocularity == VisRegionParams::BINOCULAR) {
    taVector2i bin_max(v1b_specs.tot_offs, v1b_specs.tot_disps + 2);
    max_sz.Max(bin_max);
  }

  int mot_rf_max = 5;
  if(motion_frames > 1) {
    mot_rf_max = motion_frames * (1 << v1s_motion.n_speeds) + v1s_motion.tuning_width;
    taVector2i mot_max(motion_frames * mot_rf_max, motion_frames * mot_rf_max);
    max_sz.Max(mot_max);
  }

  taVector2i brd(5,5);           // border
  max_sz += brd * 2;

  taVector2i half_sz = max_sz / 2;

  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  nmda->SetUserData("WIDTH", 24);
  DataCol* matda = graph_data->FindMakeColName("Stencil", idx, VT_FLOAT, 2,
                                              max_sz.x, max_sz.y);

  if(region.ocularity == VisRegionParams::BINOCULAR) { // v1b
    { // basic stencils
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1b Binoc", -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      taVector2i ic;
      taVector2i dc;
      for(int disp=-v1b_specs.n_disps; disp <= v1b_specs.n_disps; disp++) {
        int didx = disp + v1b_specs.n_disps;
        int dwd = v1b_widths.FastEl1d(didx);
        ic.y = half_sz.y + disp;
        ic.x = half_sz.x;

        if(ic.WrapHalf(max_sz)) continue;

        for(int twidx = 0; twidx < dwd; twidx++) {
          int off = v1b_stencils.FastEl2d(twidx, didx);
          dc = ic;
          dc.x += off;
          if(dc.WrapHalf(max_sz)) continue;
          mat->FastEl2d(dc.x,dc.y) = v1b_weights.FastEl2d(twidx, didx);
        }
      }
      mat->FastEl2d(half_sz.x,half_sz.y-v1b_specs.n_disps-1) = -0.5f;
    }
  }

  if(motion_frames > 1) { // v1simple, motion
    for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
        for(int dir = 0; dir < 2; dir++) { // directions
          float dirsign = (dir == 0) ? 1.0f : -1.0f; // direction sign for multiplying times slope values
          graph_data->AddBlankRow();
          nmda->SetValAsString("V1m sp:" + String(speed) + " ang:" + String(AngleDeg(ang)) +
                               " dir:" + String(dir == 0 ? "-" : "+"), -1);
          float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
          taVector2i ic;
          for(int mot = 0; mot < motion_frames; mot++) { // time steps back in time
            ic.y = half_sz.y;
            ic.x = brd.x + mot * mot_rf_max;

            // offset along line to prevent overwrite
            ic.x += taMath_float::rint(dirsign * v1s_ang_slopes.FastEl3d(X, LINE, ang));
            ic.y += taMath_float::rint(dirsign * v1s_ang_slopes.FastEl3d(Y, LINE, ang));

            if(ic.WrapHalf(max_sz)) continue;
            mat->FastEl2d(ic.x,ic.y) = -0.5f;

            for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
              int twidx = v1s_motion.tuning_width+tw;
              int xp = v1m_stencils.FastEl(X, twidx, mot, dir, ang, speed);
              int yp = v1m_stencils.FastEl(Y, twidx, mot, dir, ang, speed);
              ic.x = brd.x + xp + (motion_frames-1 -mot) * mot_rf_max;
              ic.y = half_sz.y + yp;
              if(ic.WrapHalf(max_sz)) continue;
              float mot_val = 1.0f; // color coding not necc: - (float)mot * (1.0f / (float)(motion_frames+2));
              mat->FastEl2d(ic.x,ic.y) = mot_val * v1m_weights.FastEl1d(twidx);
            }
          }
        }
      }
    }
  }

  if(v1c_specs.sg4) { // v1complex, sg4
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C PreGp 4x4 Ctrs: " + String(AngleDeg(ang)), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      taVector2i ic;
      // first draw a bounding box
      for(int ys = -1; ys <= 4; ys++) {
        ic.x = brd.x-1; ic.y = brd.y + ys;
        if(ic.WrapHalf(max_sz)) continue;
        mat->FastEl2d(ic.x,ic.y) = -.5;
        ic.x = brd.x+4; ic.y = brd.y + ys;
        if(ic.WrapHalf(max_sz)) continue;
        mat->FastEl2d(ic.x,ic.y) = -.5;
      }
      for(int xs = -1; xs <= 4; xs++) {
        ic.x = brd.x+xs; ic.y = brd.y -1;
        if(ic.WrapHalf(max_sz)) continue;
        mat->FastEl2d(ic.x,ic.y) = -.5;
        ic.x = brd.x+xs; ic.y = brd.y + 4;
        if(ic.WrapHalf(max_sz)) continue;
        mat->FastEl2d(ic.x,ic.y) = -.5;
      }
      int nctrs = v1sg_stencils.FastEl3d(2, 0, ang);       // length stored here
      for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
        int xp = v1sg_stencils.FastEl3d(X, ctrdx, ang);
        int yp = v1sg_stencils.FastEl3d(Y, ctrdx, ang);
        ic.x = brd.x + xp;
        ic.y = brd.y + yp;

        if(ic.WrapHalf(max_sz)) continue;
        mat->FastEl2d(ic.x,ic.y) = (ctrdx % 2 == 0) ? 1.0f: -1.0f;
      }
    }
  }
  { // spatial integ
    if(si_specs.spat_rf.MaxVal() > 1) {
      graph_data->AddBlankRow();
      nmda->SetValAsString("Spat Integ RF", -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      taVector2i sc;
      for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // ysimple
        sc.y = brd.y + ys;
        for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xsimple
          sc.x = brd.x + xs;
          if(sc.WrapHalf(max_sz)) continue;
          mat->FastEl2d(sc.x,sc.y) = si_weights.FastEl2d(xs, ys);
        }
      }
    }
  }

  { // v1complex, ls, es
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C Len Sum Ang: " + String(AngleDeg(ang)), -1);
      float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
      taVector2i ic;
      for(int lpdx=0; lpdx < v1c_specs.len_sum_width; lpdx++) {
        ic.x = brd.x + v1ls_stencils.FastEl3d(X,lpdx,ang);
        ic.y = brd.y + v1ls_stencils.FastEl3d(Y,lpdx,ang);
        if(ic.WrapHalf(max_sz)) continue;
        mat->FastEl2d(ic.x,ic.y) = 1.0f;
      }
    }
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir=0; dir < 2; dir++) {                // direction
        graph_data->AddBlankRow();
        nmda->SetValAsString("V1C End Stop Ang: " + String(AngleDeg(ang))
                             + " Dir: " + String(dir), -1);
        float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
        mat->FastEl2d(brd.x,brd.y) = 0.5f;
        taVector2i ic;
        ic.x = brd.x + v1es_stencils.FastEl(X,0,ON,dir,ang);
        ic.y = brd.y + v1es_stencils.FastEl(Y,0,ON,dir,ang);
        if(ic.WrapHalf(max_sz)) continue;
        mat->FastEl2d(ic.x,ic.y) = 1.0f;

        for(int orthdx=0; orthdx < 3; orthdx++) {
          ic.x = brd.x + v1es_stencils.FastEl(X,orthdx,OFF,dir,ang);
          ic.y = brd.y + v1es_stencils.FastEl(Y,orthdx,OFF,dir,ang);
          if(ic.WrapHalf(max_sz)) continue;
          mat->FastEl2d(ic.x,ic.y) = -1.0f;
        }
      }
    }
  }

  if(v2_filters & V2_BO) {
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      for(int dir=0; dir < 2; dir++) {                // direction
        for(int sang = 0; sang < v1s_specs.n_angles; sang++) { // angles
          for(int sdir=0; sdir < 2; sdir++) {                 // direction
            graph_data->AddBlankRow();
            nmda->SetValAsString("V2BO Ang: " + String(AngleDeg(ang))
                                 + " Dir: " + String(dir)
                                 + " SAng: " + String(AngleDeg(sang))
                                 + " SDir: " + String(sdir), -1);
            float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
            mat->FastEl2d(brd.x,brd.y) = -0.5f;
            taVector2i ic;
            int cnt = v2ffbo_stencil_n.FastEl4d(sdir, sang, dir, ang);
            for(int i=0; i<cnt; i++) {
              ic.x = brd.x + v2ffbo_stencils.FastEl(X, i, sdir, sang, dir, ang);
              ic.y = brd.y + v2ffbo_stencils.FastEl(Y, i, sdir, sang, dir, ang);
              if(ic.WrapHalf(max_sz)) continue;
              mat->FastEl2d(ic.x,ic.y) = v2ffbo_weights.FastEl(i, sdir, sang, dir, ang);
            }
          }
        }
      }
    }
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}


void V1RegionSpec::PlotSpacing(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_V1_PlotSpacing", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->Reset();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  nmda->SetUserData("WIDTH", 8);
  DataCol* matda = graph_data->FindMakeColName("Spacing", idx, VT_FLOAT, 2,
                                              input_size.retina_size.x, input_size.retina_size.y);
  graph_data->SetUserData("N_ROWS", 1);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 20.0f);
  graph_data->SetUserData("WIDTH", .5f + (float)input_size.retina_size.x / (float)input_size.retina_size.y);

//   taVector2i ic;
//   int x,y;
//   { // first do dogs
//     graph_data->AddBlankRow();
//     nmda->SetValAsString("DoG", -1);
//     float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
//     for(y=input_size.border.y; y<= input_size.retina_size.y-input_size.border.y; y+= dog_specs.spacing.y) {
//       for(x=input_size.border.x; x<= input_size.retina_size.x-input_size.border.x; x+=dog_specs.spacing.x) {
//      ic.y = y; ic.x = x;
//      ic.WrapHalf(input_size.retina_size);      mat->FastEl2d(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }

//   { // then v1 simple
//     graph_data->AddBlankRow();
//     nmda->SetValAsString("V1_Simple", -1);
//     float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
//     taVector2i brd(input_size.border.x+v1s_specs.border*dog_specs.spacing.x,
//                input_size.border.y+v1s_specs.border*dog_specs.spacing.y);
//     taVector2i spc(dog_specs.spacing.x * v1s_specs.spacing, dog_specs.spacing.y * v1s_specs.spacing);
//     // first render borders of RF's, every other
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= 2*spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+= 2*spc.x) {
//      ic.y = y; ic.x = x;
//      ic -= v1s_specs.rf_half*dog_specs.spacing; // lower left
//      taVector2i ec;
//      int ex,ey;
//      for(ey=0; ey < v1s_specs.rf_size; ey++) {
//        ec.y = ic.y + ey*dog_specs.spacing.y;  ec.x = ic.x;
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + ey*dog_specs.spacing.y;  ec.x = ic.x + dog_specs.spacing.x * (v1s_specs.rf_size-1);
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//      }
//      for(ex=0; ex < v1s_specs.rf_size; ex++) {
//        ec.y = ic.y;    ec.x = ic.x + ex*dog_specs.spacing.x;
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + dog_specs.spacing.y * (v1s_specs.rf_size-1); ec.x = ic.x + ex*dog_specs.spacing.x;
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//      }
//       }
//     }
//     // then centers
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+=spc.x) {
//      ic.y = y; ic.x = x;
//      ic.WrapHalf(input_size.retina_size);      mat->FastEl2d(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }

//   { // then v1 complex
//     taVector2i ic;
//     int x,y;
//     graph_data->AddBlankRow();
//     nmda->SetValAsString("V1_Complex", -1);
//     float_MatrixPtr mat; mat = (float_Matrix*)matda->GetValAsMatrix(-1);
//     taVector2i brd(input_size.border.x+v1s_specs.spacing*v1c_specs.net_border.x,
//                input_size.border.y+v1s_specs.spacing*v1c_specs.net_border.y);
//     taVector2i spc(v1s_specs.spacing * v1c_specs.net_spacing.x,
//                v1s_specs.spacing * v1c_specs.net_spacing.y);
//     taVector2i spcb(v1s_specs.spacing, v1s_specs.spacing);
//     // first render borders of RF's, every other
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= 2*spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+= 2*spc.x) {
//      ic.y = y; ic.x = x;
//      ic -= v1c_specs.net_half*spcb; // lower left
//      taVector2i ec;
//      int ex,ey;
//      for(ey=0; ey < v1c_specs.spat_rf.y; ey++) {
//        ec.y = ic.y + ey*spcb.y;  ec.x = ic.x;
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + ey*spcb.y;  ec.x = ic.x + spcb.x * (v1c_specs.spat_rf.x-1);
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//      }
//      for(ex=0; ex < v1c_specs.spat_rf.x; ex++) {
//        ec.y = ic.y;    ec.x = ic.x + ex*spcb.x;
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//        ec.y = ic.y + spcb.y * (v1c_specs.spat_rf.y-1); ec.x = ic.x + ex*spcb.x;
//        ec.WrapHalf(input_size.retina_size); mat->FastEl2d(ec.x,ec.y) = 0.2f;
//      }
//       }
//     }
//     for(y=brd.y; y<= input_size.retina_size.y-brd.y; y+= spc.y) {
//       for(x=brd.x; x<= input_size.retina_size.x-brd.x; x+=spc.x) {
//      ic.y = y; ic.x = x;
//      ic.WrapHalf(input_size.retina_size);      mat->FastEl2d(ic.x,ic.y) = 1.0f;
//       }
//     }
//   }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

