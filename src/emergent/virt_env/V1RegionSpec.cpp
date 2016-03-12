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
#include <DataTable>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(V1GaborSpec);
TA_BASEFUNS_CTORS_DEFN(V1sNeighInhib);
TA_BASEFUNS_CTORS_DEFN(V1MotionSpec);
TA_BASEFUNS_CTORS_DEFN(V1SquareGroup);
TA_BASEFUNS_CTORS_DEFN(V1ComplexSpec);
TA_BASEFUNS_CTORS_DEFN(VisSpatIntegSpec);
TA_BASEFUNS_CTORS_DEFN(V1RegionSpec);

void V1GaborSpec::Initialize() {
  on = true;
  wt = 1.0f;
  gain = 2.0f;
  n_angles = 4;
  size = 8;
  spacing = 1;
  wvlen = 6.0f;
  sig_len = 0.3f;
  sig_wd = 0.2f;
  phase_off = 0.0f;
  circle_edge = true;
}

void V1GaborSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void V1GaborSpec::RenderFilters(float_Matrix& fltrs) {
  fltrs.SetGeom(3, size, size, n_angles);

  float ctr = (float)(size-1) / 2.0f;
  float ang_inc = taMath_float::pi / (float)n_angles;

  float circ_radius = (float)(size) / 2.0f;

  float gs_len_eff = sig_len * (float)size;
  float gs_wd_eff = sig_wd * (float)size;

  float len_norm = 1.0f / (2.0f * gs_len_eff * gs_len_eff);
  float wd_norm = 1.0f / (2.0f * gs_wd_eff * gs_wd_eff);

  float twopinorm = (2.0f * taMath_float::pi) / wvlen;

  for(int ang = 0; ang < n_angles; ang++) {
    float angf = -(float)ang * ang_inc;

    float pos_sum = 0.0f;
    float neg_sum = 0.0f;
    for(int x = 0; x < size; x++) {
      for(int y = 0; y < size; y++) {
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
    for(int x = 0; x < size; x++) {
      for(int y = 0; y < size; y++) {
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

  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_V1Gabor_GridFilters", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
//   nmda->SetUserData("WIDTH", 10);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, size, size);

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

//////////////////////////////////////////
//              V1sNeighInhib

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


//////////////////////////////////////////
//              V1MotionSpec

void V1MotionSpec::Initialize() {
  r_only = true;
  n_speeds = 1;
  speed_inc = 1;
  tuning_width = 1;
  sig = 0.8f;
  opt_thr = 0.01f;

  tot_width = 1 + 2 * tuning_width;
}

void V1MotionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  tot_width = 1 + 2 * tuning_width;
}

//////////////////////////////////////////
//              V1SquareGroup

void V1SquareGroup::Initialize() {
  on = true;
  sg_rf = 2;
  sg_half = sg_rf / 2;
  sg_spc = 2;
  v1s_color = false;
}

void V1SquareGroup::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  TestWarning(sg_rf != 1 && sg_rf != 2 && sg_rf != 4, "UAE",
              "sg_rf must be either 1, 2, or 4");
  TestWarning(sg_spc != 1 && sg_spc != 2 && sg_spc != 4, "UAE",
              "sg_spc must be either 1, 2, or 4");
  
  sg_half = sg_rf / 2;
}

//////////////////////////////////////////
//              V1ComplexSpec

void V1ComplexSpec::Initialize() {
  on = true;
  end_stop = true;
  add_v1s = true;
  
  len_sum_len = 1;
  es_thr = 0.2f;

  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}

void V1ComplexSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  len_sum_width = 1 + 2 * len_sum_len;
  len_sum_norm = 1.0f / (float)(len_sum_width);
}

//////////////////////////////////////////
//              VisSpatIntegSpec

void VisSpatIntegSpec::Initialize() {
  on = false;
  v1s = false;
  v1pi = false;
  v1c = true;
  spat_rf = 6;
  sig = 0.8f;
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


//////////////////////////////////////////
//              V1RegionSpec

// for thread function calling:
typedef void (V1RegionSpec::*V1RegionMethod)(int, int);

void V1RegionSpec::Initialize() {
  v1s_specs_2.on = false;
  v1s_specs_3.on = false;
  v1s_renorm = NO_RENORM;
  v1s_save = (DataSave)(SAVE_DATA | ONLY_GUI);
  v1s_feat_geom.SetXYN(4, 2, 8);

  v1m_renorm = NO_RENORM;

  sg_save = NO_SAVE;
  
  v1c_save = SAVE_DATA;

  si_save = SAVE_DATA;

  opt_filters = OF_NONE;
  opt_save = SAVE_DATA;

  v1s_kwta.on = true;
  v1s_kwta.gi = 2.0f;
  v1s_kwta.lay_gi = 1.5f;
  v1s_neigh_inhib.inhib_g = 0.6f; // FFFB

  v1c_kwta.on = false;
  
  si_renorm = NO_RENORM;
  si_kwta.on = true;
  si_kwta.gi = 1.5f;
  si_kwta.lay_gi = 1.5f;

  n_colors = 1;
  n_polarities = 2;
  n_polclr = n_colors * n_polarities;
    
  cur_out_acts = NULL;
  cur_still = NULL;
  cur_maxout = NULL;
  cur_hist = NULL;
  cur_v1b_in_r = NULL;
  cur_v1b_in_l = NULL;
  cur_v1s_gabor_filter = NULL;
  cur_v1s_off = 0;
}

void V1RegionSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  v1s_specs.UpdateAfterEdit_NoGui();
  v1s_specs_2.UpdateAfterEdit_NoGui();
  v1s_specs_3.UpdateAfterEdit_NoGui();
  v1s_kwta.UpdateAfterEdit_NoGui();
  v1s_neigh_inhib.UpdateAfterEdit_NoGui();
  v1s_motion.UpdateAfterEdit_NoGui();
  v1c_specs.UpdateAfterEdit_NoGui();
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

  n_polarities = 2;             // justin case
  if(v1s_specs_2.on)
    n_polarities += 2;
  if(v1s_specs_3.on)
    n_polarities += 2;
  if(region.color == VisRegionParams::COLOR) {
    n_colors = 3;
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
  //       Square Group Geom

  if(square_group.on) {
    v1sg_feat_geom = v1s_feat_geom;
    if(!square_group.v1s_color) {
      v1sg_feat_geom.y = 2;
      v1sg_feat_geom.UpdateNfmXY();
    }

    if(region.edge_mode == VisRegionParams::WRAP) {
      square_group.sg_border = 0;
      v1sg_img_geom = v1s_img_geom / square_group.sg_spc;
    }
    else {
      square_group.sg_border = square_group.sg_spc;
      v1sg_img_geom = (((v1s_img_geom - 2 * square_group.sg_border)-1) /
                       square_group.sg_spc) + 1;
    }
  }
  else {
    square_group.sg_border = 0;
    v1sg_img_geom = v1s_img_geom;
  }

  ///////////////////////////////////////////////////////////////
  //                    V1C Complex

  if(v1c_specs.on) {
    int n_cmplx = 1;              // assume len sum
    if(v1c_specs.end_stop) {
      n_cmplx = 3;                                                // 2 es dirs
    }
    if(v1c_specs.add_v1s) {
      if(square_group.on) {
        n_cmplx += v1sg_feat_geom.y;
      }
      else {
        n_cmplx += v1s_feat_geom.y;
      }
    }

    v1c_feat_geom.x = v1s_specs.n_angles;
    v1c_feat_geom.y = n_cmplx;
    v1c_feat_geom.UpdateNfmXY();

    ///////       V1C spatial geom
    if(square_group.on) {
      v1c_img_geom = v1sg_img_geom;
    }
    else {
      v1c_img_geom = v1s_img_geom;
    }
  }

  ///////////////////////////////////////
  //  Spat Integ Geoms

  if(!v1c_specs.on) {
    si_specs.v1c = false;
  }

  if(si_specs.on) {
    if(region.edge_mode == VisRegionParams::WRAP) {
      si_specs.spat_border = 0;
      if(square_group.on) {
        si_v1s_geom = v1sg_img_geom / si_specs.spat_spacing;
      }
      else {
        si_v1s_geom = v1s_img_geom / si_specs.spat_spacing;
      }
      si_v1c_geom = v1c_img_geom / si_specs.spat_spacing;
    }
    else {
      si_specs.spat_border = si_specs.spat_spacing;
      if(square_group.on) {
        si_v1s_geom = (((v1sg_img_geom - 2 * si_specs.spat_border)-1) /
                        si_specs.spat_spacing) + 1;
      }
      else {
        si_v1s_geom = (((v1s_img_geom - 2 * si_specs.spat_border)-1) /
                       si_specs.spat_spacing) + 1;
      }
      si_v1c_geom = (((v1c_img_geom - 2 * si_specs.spat_border)-1) /
                     si_specs.spat_spacing) + 1;
    }
  }
}

bool V1RegionSpec::InitFilters() {
  inherited::InitFilters();
  InitFilters_V1Simple();
  if(motion_frames > 1)
    InitFilters_V1Motion();
  InitFilters_SquareGroup();
  InitFilters_V1Complex();
  InitFilters_SpatInteg();
  return true;
}

bool V1RegionSpec::InitFilters_V1Simple() {

  v1s_specs.RenderFilters(v1s_gabor_filters);
  if(v1s_specs_2.on)
    v1s_specs_2.RenderFilters(v1s_gabor_filters_2);
  if(v1s_specs_3.on)
    v1s_specs_3.RenderFilters(v1s_gabor_filters_3);

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
      v1m_weights.FastEl1d(idx) = taMath_float::gauss_den_sig(fx, v1s_motion.sig);
    }
  }
  taMath_float::vec_norm_max(&v1m_weights); // max norm to 1
  return true;
}

bool V1RegionSpec::InitFilters_SquareGroup() {
  // sg4 guys -- center points relative to lower-left corner of group
  v1sg4_stencils.SetGeom(3, 3, 10, 4);
  // lengths stored in position 2 of first point
  v1sg4_stencils.FastEl3d(2,0,0) = 8;
  v1sg4_stencils.FastEl3d(2,0,1) = 10;
  v1sg4_stencils.FastEl3d(2,0,2) = 8;
  v1sg4_stencils.FastEl3d(2,0,3) = 10;
  for(int lpdx=0; lpdx < 10; lpdx++) {
    // 0 = 0 deg
    v1sg4_stencils.FastEl3d(X, lpdx, 0) = 1 + lpdx / 4;
    v1sg4_stencils.FastEl3d(Y, lpdx, 0) = lpdx % 4;
    // 1 = 45 deg
    v1sg4_stencils.FastEl3d(X, lpdx, 1) = 2 + lpdx/5 - (lpdx % 5)/2;
    v1sg4_stencils.FastEl3d(Y, lpdx, 1) = lpdx/5 + ((lpdx%5)+1)/2;
    // 2 = 90 deg
    v1sg4_stencils.FastEl3d(X, lpdx, 2) = lpdx % 4;
    v1sg4_stencils.FastEl3d(Y, lpdx, 2) = 1 + lpdx / 4;
    // 3 = 135 deg
    v1sg4_stencils.FastEl3d(X, lpdx, 3) = lpdx/5 + (lpdx % 5)/2;
    v1sg4_stencils.FastEl3d(Y, lpdx, 3) = (1 - lpdx/5) + ((lpdx%5)+1)/2;
  }

  // sg2 guys -- center points relative to lower-left corner of group
  v1sg2_stencils.SetGeom(3, 3, 4, 4);
  // lengths stored in position 2 of first point
  v1sg2_stencils.FastEl3d(2,0,0) = 4;
  v1sg2_stencils.FastEl3d(2,0,1) = 4;
  v1sg2_stencils.FastEl3d(2,0,2) = 4;
  v1sg2_stencils.FastEl3d(2,0,3) = 4;
  for(int lpdx=0; lpdx < 4; lpdx++) {
    // 0 = 0 deg
    v1sg2_stencils.FastEl3d(X, lpdx, 0) = lpdx / 2;
    v1sg2_stencils.FastEl3d(Y, lpdx, 0) = lpdx % 2;
    // 1 = 45 deg
    v1sg2_stencils.FastEl3d(X, lpdx, 1) = lpdx / 2;
    v1sg2_stencils.FastEl3d(Y, lpdx, 1) = lpdx % 2;
    // 2 = 90 deg
    v1sg2_stencils.FastEl3d(X, lpdx, 2) = lpdx % 2;
    v1sg2_stencils.FastEl3d(Y, lpdx, 2) = lpdx / 2;
    // 3 = 135 deg
    v1sg2_stencils.FastEl3d(X, lpdx, 3) = lpdx % 2;
    v1sg2_stencils.FastEl3d(Y, lpdx, 3) = lpdx / 2;
  }
  return true;
}

bool V1RegionSpec::InitFilters_V1Complex() {
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

  return true;
}

bool V1RegionSpec::InitFilters_SpatInteg() {
  if(si_specs.spat_rf.MaxVal() > 1) {
    taMath_float::vec_kern2d_gauss(&si_weights, si_specs.spat_rf.x,
                                   si_specs.spat_rf.y, si_specs.sig, si_specs.sig);
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

  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);
  
  ///////////////////  V1S Output ////////////////////////
  v1s_out_r.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
  if(binoc)
    v1s_out_l.SetGeomN(v1s_out_r.geom);
  else
    v1s_out_l.SetGeom(1,1);     // free memory
  v1s_raw_r.SetGeomN(v1s_out_r.geom);
  if(v1s_adapt.on) {
    v1s_adapt_r.SetGeomN(v1s_out_r.geom);
  }
  else {
    v1s_adapt_r.SetGeom(1,1);
  }
  v1s_nimax.SetGeomN(v1s_out_r.geom);
  v1s_nimax.InitVals(0.0f);
  if(binoc) {
    v1s_raw_l.SetGeomN(v1s_out_r.geom);
    if(v1s_adapt.on) {
      v1s_adapt_l.SetGeomN(v1s_out_r.geom);
    }
    else {
      v1s_adapt_l.SetGeom(1,1);
    }
  }

  v1pi_out_r.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
  if(binoc)
    v1pi_out_l.SetGeom(4, v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
  else
    v1pi_out_l.SetGeom(1,1);

  ///////////////////  V1M Output ////////////////////////
  if(motion_frames > 1) {
    bool l_motion = !v1s_motion.r_only && binoc;
    v1m_out_r.SetGeom(4, v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    if(l_motion)
      v1m_out_l.SetGeom(4, v1m_feat_geom.x, v1m_feat_geom.y, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_out_l.SetGeom(1,1);   // free memory

    v1m_maxout_r.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(l_motion)
      v1m_maxout_l.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    else
      v1m_maxout_l.SetGeom(1,1);        // free memory

    // hist -- only saves on/off luminance
    v1m_hist_r.SetGeom(5, v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y,
                       motion_frames);
    if(l_motion)
      v1m_hist_l.SetGeom(5, v1s_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y,
                         motion_frames);
    else
      v1m_hist_l.SetGeom(1,1);  // free memory

    // still filters on top of history
    v1m_still_r.SetGeom(4, v1m_feat_geom.x, v1m_in_polarities, v1s_img_geom.x, v1s_img_geom.y);
    if(l_motion)
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

  ///////////////////  V1 Square Group ////////////////////////

  if(square_group.on) {
    v1s_sg_out_r.SetGeom(4, v1sg_feat_geom.x, v1sg_feat_geom.y,
                         v1sg_img_geom.x, v1sg_img_geom.y);
    if(binoc)
      v1s_sg_out_l.SetGeomN(v1s_sg_out_r.geom);
    else
      v1s_sg_out_l.SetGeom(1,1);

    v1pi_sg_out_r.SetGeom(4, v1s_feat_geom.x, 1, v1sg_img_geom.x, v1sg_img_geom.y);
    if(binoc)
      v1pi_sg_out_l.SetGeomN(v1pi_sg_out_r.geom);
    else
      v1pi_sg_out_l.SetGeom(1,1);

  }
  else {
    v1s_sg_out_r.SetGeom(1,1);
    v1s_sg_out_l.SetGeom(1,1);

    v1pi_sg_out_r.SetGeom(1,1);
    v1pi_sg_out_l.SetGeom(1,1);
  }

  
  ///////////////////  V1C Output ////////////////////////
  if(v1c_specs.on) {
    v1c_out_r.SetGeom(4, v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    if(binoc)
      v1c_out_l.SetGeomN(v1c_out_r.geom);
    else
      v1c_out_l.SetGeom(1,1);
  }
  else {
    v1c_out_r.SetGeom(1,1);
    v1c_out_l.SetGeom(1,1);
  }
  v1c_raw_r.SetGeomN(v1c_out_r.geom);
  v1c_raw_l.SetGeomN(v1c_out_l.geom);

  ////////////  Spat Integ

  if(si_specs.on) {
    if(si_specs.v1s) {
      if(square_group.on) {
        si_v1s_out_r.SetGeom(4, v1sg_feat_geom.x, v1sg_feat_geom.y,
                           si_v1s_geom.x, si_v1s_geom.y);
      }
      else {
        si_v1s_out_r.SetGeom(4, v1s_feat_geom.x, v1s_feat_geom.y,
                           si_v1s_geom.x, si_v1s_geom.y);
      }
      si_v1s_raw_r.SetGeomN(si_v1s_out_r.geom);
      if(binoc) {
        si_v1s_out_l.SetGeomN(si_v1s_out_r.geom);
        si_v1s_raw_l.SetGeomN(si_v1s_raw_r.geom);
      }
    }
    else {
      si_v1s_out_r.SetGeom(1,1);
      si_v1s_raw_r.SetGeom(1,1);
      si_v1s_out_l.SetGeom(1,1);
      si_v1s_raw_l.SetGeom(1,1);
    }

    if(si_specs.v1pi) {
      si_v1pi_out_r.SetGeom(4, v1s_feat_geom.x, 1, si_v1s_geom.x, si_v1s_geom.y);
      si_v1pi_raw_r.SetGeomN(si_v1pi_out_r.geom);
      if(binoc) {
        si_v1pi_out_l.SetGeomN(si_v1pi_out_r.geom);
        si_v1pi_raw_l.SetGeomN(si_v1pi_raw_r.geom);
      }
    }
    else {
      si_v1pi_out_r.SetGeom(1,1);
      si_v1pi_raw_r.SetGeom(1,1);
      si_v1pi_out_l.SetGeom(1,1);
      si_v1pi_raw_l.SetGeom(1,1);
    }
    
    if(si_specs.v1c) {
      si_v1c_out_r.SetGeom(4, v1c_feat_geom.x, v1c_feat_geom.y,
                           si_v1c_geom.x, si_v1c_geom.y);
      si_v1c_raw_r.SetGeomN(si_v1c_out_r.geom);
      if(binoc) {
        si_v1c_out_l.SetGeomN(si_v1c_out_r.geom);
        si_v1c_raw_l.SetGeomN(si_v1c_raw_r.geom);
      }
    }
    else {
      si_v1c_out_r.SetGeom(1,1);
      si_v1c_raw_r.SetGeom(1,1);
      si_v1c_out_l.SetGeom(1,1);
      si_v1c_raw_l.SetGeom(1,1);
    }
  }

  ///////////////////  OPT Output ////////////////////////
  if(opt_filters & ENERGY) {
    energy_out.SetGeom(2, v1s_img_geom.x, v1s_img_geom.y);
  }
  else {
    energy_out.SetGeom(1,1);
  }

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

void V1RegionSpec::ResetAdapt() {
  inherited::ResetAdapt();

  if(input_adapt.on) {
    v1s_adapt_r.InitVals(0.0f);
    if(region.ocularity == VisRegionParams::BINOCULAR) {
      v1s_adapt_l.InitVals(0.0f);
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
    if(rval && square_group.on) {
      rval &= DoV1SquareGroup();
    }
    
    if(rval && v1c_specs.on) {
      rval &= V1ComplexFilter();
    }

    if(rval && si_specs.on) {
      rval &= SpatIntegFilter();
    }

    if(rval && opt_filters != OF_NONE) {
      rval &= V1OptionalFilter();
    }
  }

  if(!data_table || save_mode == NONE_SAVE) // bail now
    return rval;

  if(OutSaveOk(v1s_save)) {
    V1SOutputToTable(data_table);
  }

  if(!motion_only) {
    if(square_group.on && OutSaveOk(sg_save)) {
      V1SqGpOutputToTable(data_table);
    }
    if(v1c_specs.on && OutSaveOk(v1c_save)) {
      V1COutputToTable(data_table);
    }
    if(si_specs.on && OutSaveOk(si_save)) {
      SIOutputToTable(data_table);
    }
    if(opt_filters != OF_NONE && OutSaveOk(opt_save)) {
      OptOutputToTable(data_table);
    }
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter() {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);
  
  bool rval = V1SimpleFilter_Static(cur_img_r, &v1s_raw_r, &v1s_out_r, &v1s_adapt_r);
  if(rval && binoc) {
    rval &= V1SimpleFilter_Static(cur_img_l, &v1s_raw_l, &v1s_out_l, &v1s_adapt_l);
  }

  rval &= V1SimpleFilter_PolInvar(&v1s_out_r, &v1pi_out_r);
  if(rval && binoc) {
    rval &= V1SimpleFilter_PolInvar(&v1s_out_l, &v1pi_out_l);
  }

  if(motion_frames > 1) {
    rval &= V1SimpleFilter_Motion(&v1pi_out_r, &v1m_out_r, &v1m_maxout_r,
                                  &v1m_still_r, &v1m_hist_r, &v1m_circ_r);
    if(rval && !v1s_motion.r_only && binoc) {
      rval &= V1SimpleFilter_Motion(&v1pi_out_l, &v1m_out_l, &v1m_maxout_l,
                                    &v1m_still_l, &v1m_hist_l, &v1m_circ_l);
    }
  }

  return rval;
}

bool V1RegionSpec::V1SimpleFilter_Static(float_Matrix* image, float_Matrix* raw,
                                         float_Matrix* out, float_Matrix* adapt) {
  cur_img = image;
  rgb_img = (cur_img->dims() == 3);

  if(rgb_img) {
    PrecomputeColor(cur_img);   // precompute!
  }

  if(v1s_kwta.On()) {
    cur_out = raw;
    cur_out_acts = out;
  }
  else {
    cur_out = out;
  }
  cur_adapt = adapt;

  cur_v1s_gabor_filter = &v1s_gabor_filters;
  cur_v1s_off = 0;
  IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_Static_thread);

  if(v1s_specs_2.on) {
    cur_v1s_gabor_filter = &v1s_gabor_filters_2;
    cur_v1s_off = n_colors * 2;
    IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_Static_thread);
  }
  if(v1s_specs_3.on) {
    cur_v1s_gabor_filter = &v1s_gabor_filters_3;
    cur_v1s_off = n_colors * 4;
    IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_Static_thread);
  }

  if(v1s_renorm != NO_RENORM) {            // always renorm prior to any kwta
    RenormOutput(v1s_renorm, cur_out);
  }

  if(v1s_kwta.On()) {
    if(v1s_neigh_inhib.on) { // pre-compute neigh inhib
      IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread);
      v1s_kwta.Compute_Inhib_Extra(*raw, *out, v1s_gci, v1s_nimax);
    }
    else {
      v1s_kwta.Compute_Inhib(*raw, *out, v1s_gci);
    }
  }

  return true;
}

void V1RegionSpec::V1SimpleFilter_Static_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1s_img_geom, st, ed);

  int flt_wdf = v1s_specs.size; // full-width
  int flt_wd;                          // half-width
  if(flt_wdf % 2 == 0)
    flt_wd = flt_wdf / 2;
  else
    flt_wd = (flt_wdf-1) / 2;

  int flt_vecw = flt_wdf / 4;
  flt_vecw *= 4;

  taVector2i in_off = input_size.border - flt_wd;
  
  taVector2i st_ne = -in_off; // skip over start..
  taVector2i ed_ne = v1s_img_geom + in_off;
  ed_ne -= flt_wdf;

  // taMisc::Info("st:", st.GetStr(), "ed:", ed.GetStr(),
  //              "st_ne:", st_ne.GetStr(), "ed_ne:", ed_ne.GetStr());
  
  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord

  float_Matrix* v1s_img = cur_img;

  // int ne_cnt = 0;
  
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    bool y_ne = (oc.y >= st_ne.y && oc.y < ed_ne.y); // y no edge
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      bool ne = y_ne && (oc.x >= st_ne.x && oc.x < ed_ne.x); // no edge

      // if(ne) ne_cnt++;
      
      taVector2i icc = in_off + v1s_specs.spacing * oc; // image coords center

      for(int chan = 0; chan < n_colors; chan++) {
        ColorChannel cchan = (ColorChannel)chan;
        if(rgb_img) {
          v1s_img = GetImageForChan(cchan);
        }

        int fcy = chan * 2 + cur_v1s_off; // starting of y axis -- add 1 for off-polarity

        for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
          const float* flt = (const float*)cur_v1s_gabor_filter->el +
            cur_v1s_gabor_filter->FastElIndex3d(0, 0, ang);
        
          float cnv_sum = 0.0f;               // convolution sum
          if(chan == 0 || rgb_img) {          // only rgb images if chan > 0
            int fi = 0;
            for(int yf = 0; yf < flt_wdf; yf++) {
              if(ne) {
                int img_st = v1s_img->FastElIndex2d(icc.x, icc.y + yf);
#ifdef TA_VEC_USE
                int xf;
                for(xf = 0; xf < flt_vecw; xf+= 4, fi+=4) {
                  Vec4f ivals;  ivals.load(v1s_img->el + img_st + xf);
                  Vec4f fvals;  fvals.load(flt + fi);
                  Vec4f prod = ivals * fvals;
                  cnv_sum += horizontal_add(prod);
                }
                for(; xf < flt_wdf; xf++, fi++) { // get the residuals
                  cnv_sum += v1s_img->FastEl_Flat(img_st + xf) * flt[fi];
                }
#else              
                for(int xf = 0; xf < flt_wdf; xf++, fi++) {
                  cnv_sum += v1s_img->FastEl_Flat(img_st + xf) * flt[fi];
                }
#endif              
              }
              else {
                for(int xf = 0; xf < flt_wdf; xf++, fi++) {
                  ic.y = icc.y + yf;
                  ic.x = icc.x + xf;
                  if(ic.WrapClip(wrap, input_size.retina_size)) {
                    if(region.edge_mode == VisRegionParams::CLIP) continue;
                  }
                  cnv_sum += v1s_img->FastEl2d(ic.x, ic.y) * flt[fi];
                }
              }
            }
          }
          cnv_sum *= v1s_specs.gain;
          if(cnv_sum >= 0.0f) {
            if(v1s_adapt.on) {
              float cs_orig = cnv_sum;
              float& adpt = cur_adapt->FastEl4d(ang, fcy, oc.x, oc.y);
              cnv_sum -= adpt;
              if(cnv_sum < 0.0f) cnv_sum = 0.0f;
              adpt += v1s_adapt.up_dt * cs_orig - v1s_adapt.dn_dt * adpt;
          
              float& adpto = cur_adapt->FastEl4d(ang, fcy+1, oc.x, oc.y);
              adpto -= v1s_adapt.dn_dt * adpto;
            }
            cur_out->FastEl4d(ang, fcy, oc.x, oc.y) = cnv_sum; // on-polarity
            cur_out->FastEl4d(ang, fcy+1, oc.x, oc.y) = 0.0f;
          }
          else {
            if(v1s_adapt.on) {
              float cs_orig = -cnv_sum;
              float& adpt = cur_adapt->FastEl4d(ang, fcy+1, oc.x, oc.y);
              cnv_sum += adpt;
              if(cnv_sum > 0.0f) cnv_sum = 0.0f;
              adpt += v1s_adapt.up_dt * cs_orig - v1s_adapt.dn_dt * adpt;
          
              float& adpto = cur_adapt->FastEl4d(ang, fcy, oc.x, oc.y);
              adpto -= v1s_adapt.dn_dt * adpto;
            }
            cur_out->FastEl4d(ang, fcy, oc.x, oc.y) = 0.0f;
            cur_out->FastEl4d(ang, fcy+1, oc.x, oc.y) = -cnv_sum; // off-polarity
          }
        }
      }
    }
  }

  // taVector2i tot = ed - st;
  // taMisc::Info("ne cnt:", String(ne_cnt),"out of:", String(tot.Product()));
}

void V1RegionSpec::V1SimpleFilter_Static_neighinhib_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1s_img_geom, st, ed);

  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
        for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
          float raw = cur_out->FastEl4d(ang, polclr, oc.x, oc.y);
          float feat_inhib_max = 0.0f;
          for(int lpdx=0; lpdx < v1s_neigh_inhib.tot_ni_len; lpdx++) { // go out to neighs
            if(lpdx == v1s_neigh_inhib.inhib_d) continue;              // skip self
            int xp = v1s_ni_stencils.FastEl3d(X,lpdx,ang);
            int yp = v1s_ni_stencils.FastEl3d(Y,lpdx,ang);
            ic.x = oc.x + xp;
            ic.y = oc.y + yp;
            if(ic.WrapClip(wrap, v1s_img_geom)) {
              if(region.edge_mode == VisRegionParams::CLIP) continue;
            }
            float oth_inp = cur_out->FastEl4d(ang, polclr, ic.x, ic.y); // other guy
            float ogi = v1s_neigh_inhib.inhib_g * oth_inp;
            feat_inhib_max = MAX(feat_inhib_max, ogi);
          }
          v1s_nimax.FastEl4d(ang, polclr, oc.x, oc.y) = feat_inhib_max;
        }
      }
    }
  }
}

bool V1RegionSpec::V1SimpleFilter_PolInvar(float_Matrix* v1s_out_in, float_Matrix* v1pi_out) {
  cur_in = v1s_out_in;          // using img for this..
  cur_out = v1pi_out;

  IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_PolInvar_thread);

  return true;
}

void V1RegionSpec::V1SimpleFilter_PolInvar_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1s_img_geom, st, ed);

  taVector2i oc;         // current coord -- output space

  if(v1s_specs_3.on) {
    const float wt_sum = v1s_specs.wt + v1s_specs_2.wt + v1s_specs_3.wt;
    const float wt1 = v1s_specs.wt / wt_sum;
    const float wt2 = v1s_specs_2.wt / wt_sum;
    const float wt3 = v1s_specs_3.wt / wt_sum;
    const int nper = n_colors * 2;
    const int nper2 = nper * 2;
    const int nper3 = nper * 3;
    for(oc.y = st.y; oc.y < ed.y; oc.y++) {
      for(oc.x = st.x; oc.x < ed.x; oc.x++) {
        for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
          float max_pi = 0.0f;
          for(int polclr = 0; polclr < nper; polclr++) { // polclr features
            float val = cur_in->FastEl4d(ang, polclr, oc.x, oc.y);
            max_pi = MAX(max_pi, val);
          }
          float max_pi2 = 0.0f;
          for(int polclr = nper; polclr < nper2; polclr++) { // polclr features
            float val = cur_in->FastEl4d(ang, polclr, oc.x, oc.y);
            max_pi2 = MAX(max_pi2, val);
          }
          float max_pi3 = 0.0f;
          for(int polclr = nper2; polclr < nper3; polclr++) { // polclr features
            float val = cur_in->FastEl4d(ang, polclr, oc.x, oc.y);
            max_pi3 = MAX(max_pi3, val);
          }
          cur_out->FastEl4d(ang, 0, oc.x, oc.y) = wt1 * max_pi + wt2 * max_pi2 +
            wt3 * max_pi3;
        }
      }
    }
  }
  else if(v1s_specs_2.on) {
    const float wt_sum = v1s_specs.wt + v1s_specs_2.wt;
    const float wt1 = v1s_specs.wt / wt_sum;
    const float wt2 = v1s_specs_2.wt / wt_sum;
    const int nper = n_colors * 2;
    const int nper2 = nper * 2;
    for(oc.y = st.y; oc.y < ed.y; oc.y++) {
      for(oc.x = st.x; oc.x < ed.x; oc.x++) {
        for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
          float max_pi = 0.0f;
          for(int polclr = 0; polclr < nper; polclr++) { // polclr features
            float val = cur_in->FastEl4d(ang, polclr, oc.x, oc.y);
            max_pi = MAX(max_pi, val);
          }
          float max_pi2 = 0.0f;
          for(int polclr = nper; polclr < nper2; polclr++) { // polclr features
            float val = cur_in->FastEl4d(ang, polclr, oc.x, oc.y);
            max_pi2 = MAX(max_pi2, val);
          }
          cur_out->FastEl4d(ang, 0, oc.x, oc.y) = wt1 * max_pi + wt2 * max_pi2;
        }
      }
    }
  }
  else {
    for(oc.y = st.y; oc.y < ed.y; oc.y++) {
      for(oc.x = st.x; oc.x < ed.x; oc.x++) {
        for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
          float max_pi = 0.0f;
          for(int polclr = 0; polclr < n_polclr; polclr++) { // polclr features
            float val = cur_in->FastEl4d(ang, polclr, oc.x, oc.y);
            max_pi = MAX(max_pi, val);
          }
          cur_out->FastEl4d(ang, 0, oc.x, oc.y) = max_pi;
        }
      }
    }
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

  // todo: put all these into one _thread call!
  IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_Motion_CpHist_thread);

  if(!cur_mot_only) {
    // if motion only, then really just load the history for later processing!
    IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_Motion_Still_thread);

    IMG_THREAD_CALL(V1RegionSpec::V1SimpleFilter_Motion_thread);

    if(v1m_renorm != NO_RENORM) {
      RenormOutput(v1m_renorm, out);
      RenormOutput(v1m_renorm, maxout);
    }
  }
  return true;
}

void V1RegionSpec::V1SimpleFilter_Motion_CpHist_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1s_img_geom, st, ed);

  int cur_mot_idx = cur_circ->CircIdx_Last();
  int mot_len = cur_circ->length;

  taVector2i oc;         // current coord -- output space
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
        float in_val = cur_in->FastEl4d(ang, 0, oc.x, oc.y);
        cur_hist->FastEl(ang, 0, oc.x, oc.y, cur_mot_idx) = in_val;
      }
    }
  }
}

void V1RegionSpec::V1SimpleFilter_Motion_Still_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1s_img_geom, st, ed);

  // todo: could optimize wrap clip out as function of v1s_motion.tuning_width
  // also, make this more binary / strongly sigmoidal so it is a more robust signal
  
  int cur_mot_idx = cur_circ->CircIdx_Last();
  int mot_len = cur_circ->length;

  taVector2i mo;                 // motion offset
  taVector2i oc;         // current coord -- output space
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {

      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
        float cur_val = cur_hist->FastEl(ang, 0, oc.x, oc.y, cur_mot_idx);
        float min_mot = cur_val;
        if(cur_val >= v1s_motion.opt_thr) { // save time
          int mx_mot = mot_len-1; // don't go up to last value -- e.g., 2
          for(int mot = 0; mot < mx_mot; mot++) { // time steps back in time -- e.g., 0, 1
            float t_val = 0.0f;
            for(int tw = -v1s_motion.tuning_width; tw <= v1s_motion.tuning_width; tw++) {
              int twidx = v1s_motion.tuning_width+tw;
              int xp = v1m_still_stencils.FastEl4d(X, twidx, mot, ang);
              int yp = v1m_still_stencils.FastEl4d(Y, twidx, mot, ang);
              mo.x = oc.x + xp;
              mo.y = oc.y + yp;
              if(mo.WrapClip(wrap, v1s_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; 
              }
              int midx = cur_circ->CircIdx(mx_mot-1 - mot); // e.g., 1-0 = 1; 1-1 = 0,
              float val = cur_hist->FastEl(ang, 0, mo.x, mo.y, midx);
              val *= v1m_weights.FastEl1d(twidx);
              t_val = MAX(t_val, val);
            }
            min_mot = MIN(min_mot, t_val); // MIN = fast product
          }
        }
        cur_still->FastEl4d(ang, 0, oc.x, oc.y) = min_mot;
      }
    }
  }
}

void V1RegionSpec::V1SimpleFilter_Motion_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1s_img_geom, st, ed);

  // todo: could optimize wrap clip out as function of v1s_motion.tuning_width
  // also, make this more binary / strongly sigmoidal so it is a more robust signal
  
  int cur_mot_idx = cur_circ->CircIdx_Last();
  int mot_len = cur_circ->length;

  taVector2i mo;                 // motion offset
  taVector2i oc;         // current coord -- output space
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {

      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
        float max_out = 0.0f;
        for(int speed = 0; speed < v1s_motion.n_speeds; speed++) { // speed
          for(int dir = 0; dir < 2; dir++) { // directions
            int moty = (speed * 2 + dir);

            float cur_val = cur_hist->FastEl(ang, 0, oc.x, oc.y, cur_mot_idx);
            float still_val = cur_still->FastEl4d(ang, 0, oc.x, oc.y);
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

                  mo.x = oc.x + xp;
                  mo.y = oc.y + yp;
                  if(mo.WrapClip(wrap, v1s_img_geom)) {
                    if(region.edge_mode == VisRegionParams::CLIP) continue; 
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
            cur_out->FastEl4d(ang, moty, oc.x, oc.y) = min_mot;
            max_out = MAX(max_out, min_mot);
          }
        }
        cur_maxout->FastEl4d(ang, 0, oc.x, oc.y) = max_out;
      }
    }
  }
}


//////////////////////////////////////////////////////////////////////
//              Square Grouping

bool V1RegionSpec::DoV1SquareGroup() {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);

  V1SquareGroup_V1S_SqGp(&v1s_out_r, &v1s_sg_out_r);
  if(binoc) {
    V1SquareGroup_V1S_SqGp(&v1s_out_l, &v1s_sg_out_l);
  }

  V1SquareGroup_V1PI_SqGp(&v1pi_out_r, &v1pi_sg_out_r);
  if(binoc) {
    V1SquareGroup_V1PI_SqGp(&v1pi_out_l, &v1pi_sg_out_l);
  }

  return true;
}

void V1RegionSpec::V1SquareGroup_V1S_SqGp(float_Matrix* v1s_in, float_Matrix* sg_out) {
  cur_in = v1s_in;
  cur_out = sg_out;

  IMG_THREAD_CALL(V1RegionSpec::V1SquareGroup_V1S_SqGp_thread);
}

void V1RegionSpec::V1SquareGroup_V1S_SqGp_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1sg_img_geom, st, ed);

  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input = simple coord
  taVector2i icc;        // input = simple coord, center

  if(square_group.sg_rf == 2) {
    for(oc.y = st.y; oc.y < ed.y; oc.y++) {
      for(oc.x = st.x; oc.x < ed.x; oc.x++) {
        taVector2i scs = square_group.sg_spc * oc; // v1s coords start
        scs += square_group.sg_border;
        scs -= square_group.sg_half; // convert to lower-left starting position, not center

        for(int ang=0; ang<v1s_specs.n_angles; ang++) {
          for(int polclr = 0; polclr < v1sg_feat_geom.y; polclr++) {
            float max_rf = 0.0f;   // max over spatial rfield
            // int nctrs = v1sg2_stencils.FastEl3d(2, 0, ang);       // length stored here
            for(int ctrdx = 0; ctrdx < 4; ctrdx++) {
              int xp = v1sg2_stencils.FastEl3d(X, ctrdx, ang);
              int yp = v1sg2_stencils.FastEl3d(Y, ctrdx, ang);
              ic.y = scs.y + yp;
              ic.x = scs.x + xp;
              icc = ic;       // center
              if(icc.WrapClip(wrap, v1s_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue;
              }
              float ctr_val = cur_in->FastEl4d(ang, polclr, icc.x, icc.y);
              max_rf = MAX(max_rf, ctr_val);
            }
            cur_out->FastEl4d(ang, polclr, oc.x, oc.y) = max_rf;
          }
        }
      }
    }
  }
  else {
    for(oc.y = st.y; oc.y < ed.y; oc.y++) {
      for(oc.x = st.x; oc.x < ed.x; oc.x++) {
        taVector2i scs = square_group.sg_spc * oc; // v1s coords start
        scs += square_group.sg_border;
        scs -= square_group.sg_half; // convert to lower-left starting position, not center

        for(int ang=0; ang<v1s_specs.n_angles; ang++) {
          for(int polclr = 0; polclr < v1sg_feat_geom.y; polclr++) { // polclr features
            float max_rf = 0.0f;   // max over spatial rfield
            int nctrs = v1sg4_stencils.FastEl3d(2, 0, ang);       // length stored here
            for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
              int xp = v1sg4_stencils.FastEl3d(X, ctrdx, ang);
              int yp = v1sg4_stencils.FastEl3d(Y, ctrdx, ang);
              ic.y = scs.y + yp;
              ic.x = scs.x + xp;
              icc = ic;       // center
              if(icc.WrapClip(wrap, v1s_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; 
              }
              float ctr_val = cur_in->FastEl4d(ang, polclr, icc.x, icc.y);
              max_rf = MAX(max_rf, ctr_val);
            }
            cur_out->FastEl4d(ang, polclr, oc.x, oc.y) = max_rf;
          }
        }
      }
    }
  }
}

void V1RegionSpec::V1SquareGroup_V1PI_SqGp(float_Matrix* pi_in, float_Matrix* sg_out) {
  cur_in = pi_in;
  cur_out = sg_out;
  IMG_THREAD_CALL(V1RegionSpec::V1SquareGroup_V1PI_SqGp_thread);
}

void V1RegionSpec::V1SquareGroup_V1PI_SqGp_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1sg_img_geom, st, ed);

  taVector2i sc;                 // simple coord
  taVector2i scc;                // simple coord, center
  taVector2i oc;         // current coord -- output space

  if(square_group.sg_rf == 2) {
    for(oc.y = st.y; oc.y < ed.y; oc.y++) {
      for(oc.x = st.x; oc.x < ed.x; oc.x++) {
        taVector2i scs = square_group.sg_spc * oc; // v1s coords start
        scs += square_group.sg_border;
        scs -= square_group.sg_half; // convert to lower-left starting position, not center

        for(int ang=0; ang<v1s_specs.n_angles; ang++) {
          float max_rf = 0.0f;   // max over spatial rfield
          // int nctrs = v1sg2_stencils.FastEl3d(2, 0, ang);         // length stored here
          for(int ctrdx = 0; ctrdx < 4; ctrdx++) {
            int xp = v1sg2_stencils.FastEl3d(X, ctrdx, ang);
            int yp = v1sg2_stencils.FastEl3d(Y, ctrdx, ang);
            sc.y = scs.y + yp;
            sc.x = scs.x + xp;
            scc = sc; // center
            if(scc.WrapClip(wrap, v1s_img_geom)) {
              if(region.edge_mode == VisRegionParams::CLIP) continue; 
            }
            float ctr_val = cur_in->FastEl4d(ang, 0, scc.x, scc.y); // gets from polinvar
            max_rf = MAX(max_rf, ctr_val);
          }
          cur_out->FastEl4d(ang, 0, oc.x, oc.y) = max_rf;
        }
      }
    }
  }
  else {                        // must be 4
    for(oc.y = st.y; oc.y < ed.y; oc.y++) {
      for(oc.x = st.x; oc.x < ed.x; oc.x++) {
        taVector2i scs = square_group.sg_spc * oc; // v1s coords start
        scs += square_group.sg_border;
        scs -= square_group.sg_half; // convert to lower-left starting position, not center

        for(int ang=0; ang<v1s_specs.n_angles; ang++) {
          float max_rf = 0.0f;   // max over spatial rfield
          int nctrs = v1sg4_stencils.FastEl3d(2, 0, ang);         // length stored here
          for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
            int xp = v1sg4_stencils.FastEl3d(X, ctrdx, ang);
            int yp = v1sg4_stencils.FastEl3d(Y, ctrdx, ang);
            sc.y = scs.y + yp;
            sc.x = scs.x + xp;
            scc = sc; // center
            if(scc.WrapClip(wrap, v1s_img_geom)) {
              if(region.edge_mode == VisRegionParams::CLIP) continue; 
            }
            float ctr_val = cur_in->FastEl4d(ang, 0, scc.x, scc.y); // gets from polinvar
            max_rf = MAX(max_rf, ctr_val);
          }
          cur_out->FastEl4d(ang, 0, oc.x, oc.y) = max_rf;
        }
      }
    }
  }
}


//////////////////////////////////////////////////////////////////////
//              Complex Filters

bool V1RegionSpec::V1ComplexFilter() {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);

  if(square_group.on) {
    cur_in_r = &v1pi_sg_out_r;
    cur_in_l = &v1pi_sg_out_l;
  }
  else {
    cur_in_r = &v1pi_out_r;
    cur_in_l = &v1pi_out_l;
  }

  if(v1c_kwta.On()) {
    cur_out_r = &v1c_raw_r;
    cur_out_l = &v1c_raw_l;
  }
  else {
    cur_out_r = &v1c_out_r;
    cur_out_l = &v1c_out_l;
  }
    

  V1ComplexFilter_LenSum(cur_in_r, cur_out_r);
  if(binoc)
    V1ComplexFilter_LenSum(cur_in_l, cur_out_l);

  if(v1c_specs.end_stop) {
    V1ComplexFilter_EndStop(cur_in_r, cur_out_r);
    if(binoc)
      V1ComplexFilter_EndStop(cur_in_l, cur_out_l);
  }

  if(v1c_specs.add_v1s) {
    if(square_group.on) {
      cur_in_r = &v1s_sg_out_r;
      cur_in_l = &v1s_sg_out_l;
    }
    else {
      cur_in_r = &v1s_out_r;
      cur_in_l = &v1s_out_l;
    }
    V1ComplexFilter_AddV1s(cur_in_r, cur_out_r);
    if(binoc)
      V1ComplexFilter_AddV1s(cur_in_l, cur_out_l);
  }
  
  if(v1c_kwta.On()) {
    v1c_kwta.Compute_Inhib(v1c_raw_r, v1c_out_r, v1c_gci);
    if(binoc) {
      v1c_kwta.Compute_Inhib(v1c_raw_l, v1c_out_l, v1c_gci);
    }
  }

  return true;
}

void V1RegionSpec::V1ComplexFilter_LenSum(float_Matrix* ls_in, float_Matrix* v1c_out) {
  cur_in = ls_in;
  cur_out = v1c_out;

  IMG_THREAD_CALL(V1RegionSpec::V1ComplexFilter_LenSum_thread);
}

void V1RegionSpec::V1ComplexFilter_LenSum_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1c_img_geom, st, ed);

  taVector2i ic;         // input = line coord
  taVector2i oc;         // current coord -- output space
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {

      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
        float line_sum = 0.0f;
        for(int lpdx=0; lpdx < v1c_specs.len_sum_width; lpdx++) {
          ic.x = oc.x + v1ls_stencils.FastEl3d(X,lpdx,ang);
          ic.y = oc.y + v1ls_stencils.FastEl3d(Y,lpdx,ang);
          if(ic.WrapClip(wrap, v1c_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue; 
          }
          float lval = cur_in->FastEl4d(ang, 0, ic.x, ic.y);
          line_sum += lval;
        }
        line_sum *= v1c_specs.len_sum_norm;
        cur_out->FastEl4d(ang, 0, oc.x, oc.y) = line_sum;
      }
    }
  }
}

void V1RegionSpec::V1ComplexFilter_EndStop(float_Matrix* pi_in, float_Matrix* v1c_out) {
  cur_in = pi_in;
  cur_out = v1c_out;

  IMG_THREAD_CALL(V1RegionSpec::V1ComplexFilter_EndStop_thread);
}

void V1RegionSpec::V1ComplexFilter_EndStop_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1c_img_geom, st, ed);

  taVector2i ic;         // input = line coord
  taVector2i sc;         // second input = stop / off coord
  taVector2i oc;         // current coord -- output space
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
        for(int dir=0; dir < 2; dir++) {                  // direction
          // len sum point
          ic.x = oc.x + v1es_stencils.FastEl(X,0,ON,dir,ang);
          ic.y = oc.y + v1es_stencils.FastEl(Y,0,ON,dir,ang);
          if(ic.WrapClip(wrap, v1c_img_geom)) {
            if(region.edge_mode == VisRegionParams::CLIP) continue;
          }
          float lsval = cur_out->FastEl4d(ang, 0, ic.x, ic.y); // len sum

          // off point
          float max_off = 0.0f;
          for(int orthdx=0; orthdx < 3; orthdx++) {
            sc.x = oc.x + v1es_stencils.FastEl(X,orthdx,OFF,dir,ang);
            sc.y = oc.y + v1es_stencils.FastEl(Y,orthdx,OFF,dir,ang);
            if(sc.WrapClip(wrap, v1c_img_geom)) {
              if(region.edge_mode == VisRegionParams::CLIP) continue;
            }
            float offval = cur_in->FastEl4d(ang, 0, sc.x, sc.y); // single oriented line
            max_off = MAX(offval, max_off);
          }
          float esval = lsval - max_off;
          if(esval < v1c_specs.es_thr) esval = 0.0f; // keep it real
          cur_out->FastEl4d(ang, 1+dir, oc.x, oc.y) = esval;
        }
      }
    }
  }
}


void V1RegionSpec::V1ComplexFilter_AddV1s(float_Matrix* v1s_in, float_Matrix* v1c_out) {
  cur_in = v1s_in;
  cur_out = v1c_out;

  IMG_THREAD_CALL(V1RegionSpec::V1ComplexFilter_AddV1s_thread);
}

void V1RegionSpec::V1ComplexFilter_AddV1s_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1c_img_geom, st, ed);

  int v1s_off = 1;
  if(v1c_specs.end_stop)
    v1s_off += 2;

  int ymax = v1c_feat_geom.y - v1s_off;
  
  taVector2i oc;         // current coord -- output space
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      for(int polclr = 0; polclr < ymax; polclr++) { // polclr features
        for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
          float val = cur_in->FastEl4d(ang, polclr, oc.x, oc.y);
          cur_out->FastEl4d(ang, v1s_off + polclr, oc.x, oc.y) = val;
        }
      }
    }
  }
}

  
/////////////////////////////////////////////////////////////////////
//              Spatial Integration

bool V1RegionSpec::SpatIntegFilter() {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);

  if(si_specs.v1s) {
    if(si_kwta.On()) {
      cur_out_r = &si_v1s_raw_r;
      cur_out_l = &si_v1s_raw_l;
    }
    else {
      cur_out_r = &si_v1s_out_r;
      cur_out_l = &si_v1s_out_l;
    }

    if(square_group.on) {
      cur_in_r = &v1s_sg_out_r;
      cur_in_l = &v1s_sg_out_l;
    }
    else {
      cur_in_r = &v1s_out_r;
      cur_in_l = &v1s_out_l;
    }

    SpatIntegFilter_V1S(cur_in_r, cur_out_r);
    if(si_kwta.On())
      si_kwta.Compute_Inhib(si_v1s_raw_r, si_v1s_out_r, si_gci);
    if(binoc) {
      SpatIntegFilter_V1S(cur_in_l, cur_out_l);
      if(si_kwta.On())
        si_kwta.Compute_Inhib(si_v1s_raw_l, si_v1s_out_l, si_gci);
    }
  }

  if(si_specs.v1pi) {
    if(si_kwta.On()) {
      cur_out_r = &si_v1pi_raw_r;
      cur_out_l = &si_v1pi_raw_l;
    }
    else {
      cur_out_r = &si_v1pi_out_r;
      cur_out_l = &si_v1pi_out_l;
    }

    if(square_group.on) {
      cur_in_r = &v1pi_sg_out_r;
      cur_in_l = &v1pi_sg_out_l;
    }
    else {
      cur_in_r = &v1pi_out_r;
      cur_in_l = &v1pi_out_l;
    }

    SpatIntegFilter_V1PI(cur_in_r, cur_out_r);
    if(si_kwta.On())
      si_kwta.Compute_Inhib(si_v1pi_raw_r, si_v1pi_out_r, si_gci);
    if(binoc) {
      SpatIntegFilter_V1PI(cur_in_l, cur_out_l);
      if(si_kwta.On())
        si_kwta.Compute_Inhib(si_v1pi_raw_l, si_v1pi_out_l, si_gci);
    }
  }

  if(si_specs.v1c) {
    if(si_kwta.On()) {
      cur_out_r = &si_v1c_raw_r;
      cur_out_l = &si_v1c_raw_l;
    }
    else {
      cur_out_r = &si_v1c_out_r;
      cur_out_l = &si_v1c_out_l;
    }
    cur_in_r = &v1c_out_r;
    cur_in_l = &v1c_out_l;

    SpatIntegFilter_V1C(cur_in_r, cur_out_r);
    if(si_kwta.On())
      si_kwta.Compute_Inhib(si_v1c_raw_r, si_v1c_out_r, si_gci);
    if(binoc) {
      SpatIntegFilter_V1C(cur_in_l, cur_out_l);
      if(si_kwta.On())
        si_kwta.Compute_Inhib(si_v1c_raw_l, si_v1c_out_l, si_gci);
    }
  }

  return true;
}

void V1RegionSpec::SpatIntegFilter_V1S(float_Matrix* v1s_in, float_Matrix* si_out) {
  cur_in = v1s_in;
  cur_out = si_out;
  IMG_THREAD_CALL(V1RegionSpec::SpatIntegFilter_V1S_thread);

  if(si_renorm != NO_RENORM)
    RenormOutput(si_renorm, cur_out);
}

void V1RegionSpec::SpatIntegFilter_V1S_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, si_v1s_geom, st, ed);

  int ymax = cur_in->dim(1);
  taVector2i in_g(cur_in->dim(2), cur_in->dim(3));
  
  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord
  taVector2i icc;        // input coord, center
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {

      taVector2i ics = si_specs.spat_spacing * oc; // v1s coords start
      ics += si_specs.spat_border;
      ics -= si_specs.spat_half; // convert to lower-left starting position, not center
      
      for(int polclr = 0; polclr < ymax; polclr++) { // polclr features
        for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
          float max_rf = 0.0f;   // max over spatial rfield
          for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
            ic.y = ics.y + ys;
            for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
              ic.x = ics.x + xs;
              icc = ic;     // center
              if(icc.WrapClip(wrap, in_g)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; 
              }
              float val = cur_in->FastEl4d(ang, polclr, icc.x, icc.y);
              val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
              max_rf = MAX(max_rf, val);
            }
          }
          cur_out->FastEl4d(ang, polclr, oc.x, oc.y) = max_rf;
        } // for polclr
      }  // for ang
    }
  }
}

void V1RegionSpec::SpatIntegFilter_V1PI(float_Matrix* v1pi_in, float_Matrix* si_out) {
  cur_in = v1pi_in;
  cur_out = si_out;
  IMG_THREAD_CALL(V1RegionSpec::SpatIntegFilter_V1PI_thread);
  
  if(si_renorm != NO_RENORM)
    RenormOutput(si_renorm, cur_out);
}

void V1RegionSpec::SpatIntegFilter_V1PI_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, si_v1s_geom, st, ed);

  taVector2i in_g(cur_in->dim(2), cur_in->dim(3));

  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord
  taVector2i icc;        // input coord, center
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      taVector2i ics = si_specs.spat_spacing * oc; // v1s coords start
      ics += si_specs.spat_border;
      ics -= si_specs.spat_half; // convert to lower-left starting position, not center

      for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
        float max_rf = 0.0f;   // max over spatial rfield
        for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
          ic.y = ics.y + ys;
          for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
            ic.x = ics.x + xs;
            icc = ic;       // center
            if(icc.WrapClip(wrap, in_g)) {
              if(region.edge_mode == VisRegionParams::CLIP) continue; 
            }
            float val = cur_in->FastEl4d(ang, 0, icc.x, icc.y);
            val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
            max_rf = MAX(max_rf, val);
          }
        }
        cur_out->FastEl4d(ang, 0, oc.x, oc.y) = max_rf;
      }
    }
  }
}

void V1RegionSpec::SpatIntegFilter_V1C(float_Matrix* v1c_in, float_Matrix* si_out) {
  cur_in = v1c_in;
  cur_out = si_out;
  IMG_THREAD_CALL(V1RegionSpec::SpatIntegFilter_V1C_thread);
  
  if(si_renorm != NO_RENORM)
    RenormOutput(si_renorm, cur_out);
}

void V1RegionSpec::SpatIntegFilter_V1C_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, si_v1c_geom, st, ed);

  taVector2i oc;         // current coord -- output space
  taVector2i ic;         // input coord
  taVector2i icc;        // input coord, center
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      taVector2i ics = si_specs.spat_spacing * oc; // v1s coords start
      ics += si_specs.spat_border;
      ics -= si_specs.spat_half; // convert to lower-left starting position, not center

      for(int cfdx = 0; cfdx < v1c_feat_geom.y; cfdx++) { // cfdx features
        for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
          float max_rf = 0.0f;   // max over spatial rfield
          for(int ys = 0; ys < si_specs.spat_rf.y; ys++) { // yspat
            ic.y = ics.y + ys;
            for(int xs = 0; xs < si_specs.spat_rf.x; xs++) { // xspat
              ic.x = ics.x + xs;
              icc = ic;     // center
              if(icc.WrapClip(wrap, v1c_img_geom)) {
                if(region.edge_mode == VisRegionParams::CLIP) continue; 
              }
              float val = cur_in->FastEl4d(ang, cfdx, icc.x, icc.y);
              val *= si_weights.FastEl2d(xs, ys); // spatial rf weighting
              max_rf = MAX(max_rf, val);
            }
          }
          cur_out->FastEl4d(ang, cfdx, oc.x, oc.y) = max_rf;
        }
      }
    }
  }
}


/////////////////////////////////////////////////////////////////
//              Optional Filters

bool V1RegionSpec::V1OptionalFilter() {
  if(opt_filters & ENERGY) {
    IMG_THREAD_CALL(V1RegionSpec::V1OptionalFilter_Energy_thread);
  }

  return true;
}

void V1RegionSpec::V1OptionalFilter_Energy_thread(int thr_no) {
  taVector2i st;
  taVector2i ed;
  GetThread2DGeom(thr_no, v1s_img_geom, st, ed);

  taVector2i oc;         // current coord -- output space
  for(oc.y = st.y; oc.y < ed.y; oc.y++) {
    for(oc.x = st.x; oc.x < ed.x; oc.x++) {
      float max_feat = 0.0f;
      // just max over angles -- blobify!
      for(int ang = 0; ang < v1s_specs.n_angles; ang++) {
        float v1sval = v1pi_out_r.FastEl4d(ang, 0, oc.x, oc.y);
        max_feat = MAX(max_feat, v1sval);
      }

      energy_out.FastEl2d(oc.x, oc.y) = max_feat;
    }
  }
}


////////////////////////////////////////////////////////////////////
//      V1Region        Data Table Output

bool V1RegionSpec::InitDataTable() {
  inherited::InitDataTable();
  if(!data_table) {
    return false;
  }

  bool fmt_only = true;
  if(OutSaveOk(v1s_save)) {
    V1SOutputToTable(data_table, fmt_only);
  }
  if(square_group.on && OutSaveOk(sg_save)) {
    V1SqGpOutputToTable(data_table, fmt_only);
  }
  if(v1c_specs.on && OutSaveOk(v1c_save)) {
    V1COutputToTable(data_table, fmt_only);
  }
  if(si_specs.on && OutSaveOk(si_save)) {
    SIOutputToTable(data_table, fmt_only);
  }
  if(opt_filters != OF_NONE && OutSaveOk(opt_save)) {
    OptOutputToTable(data_table, fmt_only);
  }

  return true;
}

bool V1RegionSpec::V1SOutputToTable(DataTable* dtab, bool fmt_only) {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);
  DataCol* col;
  int idx;
  V1SOutputToTable_impl(dtab, &v1s_out_r, "_r", fmt_only);
  if(binoc) {
    V1SOutputToTable_impl(dtab, &v1s_out_l, "_l", fmt_only);
  }

  { // polarinvar
    col = data_table->FindMakeColName(name + "_v1pi_r", idx, DataTable::VT_FLOAT, 4,
                                      v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(&v1pi_out_r);
    }
    if(binoc) {
      col = data_table->FindMakeColName(name + "_v1pi_l", idx, DataTable::VT_FLOAT, 4,
                                        v1s_feat_geom.x, 1, v1s_img_geom.x, v1s_img_geom.y);
      if(!fmt_only) {
        float_MatrixPtr doutl; doutl = (float_Matrix*)col->GetValAsMatrix(-1);
        doutl->CopyFrom(&v1pi_out_l);
      }
    }
  }

  if(motion_frames > 1) {
    V1MOutputToTable_impl(dtab, &v1m_out_r, &v1m_maxout_r, &v1m_still_r, &v1m_hist_r,
                          &v1m_circ_r, "_r", fmt_only);
    if(!v1s_motion.r_only && binoc)
      V1MOutputToTable_impl(dtab, &v1m_out_l, &v1m_maxout_l, &v1m_still_l,
                            &v1m_hist_l, &v1m_circ_l, "_l", fmt_only);
  }

  return true;
}

bool V1RegionSpec::V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                         const String& col_sufx, bool fmt_only) {
  taVector2i sc;         // simple coords
  DataCol* col;
  int idx;
  if(v1s_save & SEP_MATRIX && region.color == VisRegionParams::COLOR) {
    { // basic luminance b/w filters
      col = data_table->FindMakeColName
        (name + "_v1s_bw" + col_sufx, idx, DataTable::VT_FLOAT, 4,
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
    {
      col = data_table->FindMakeColName
        (name + "_v1s_clr" + col_sufx, idx, DataTable::VT_FLOAT, 4,
         v1s_feat_geom.x, n_polclr-2, v1s_img_geom.x, v1s_img_geom.y);
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


bool V1RegionSpec::V1SqGpOutputToTable(DataTable* dtab, bool fmt_only) {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);
  FourDimMatrixToTable(dtab, &v1s_sg_out_r, "_v1s_sg_r", fmt_only);
  if(binoc) {
    FourDimMatrixToTable(dtab, &v1s_sg_out_l, "_v1s_sg_l", fmt_only);
  }

  FourDimMatrixToTable(dtab, &v1pi_sg_out_r, "_v1pi_sg_r", fmt_only);
  if(binoc) {
    FourDimMatrixToTable(dtab, &v1pi_sg_out_l, "_v1pi_sg_l", fmt_only);
  }

  return true;
}

bool V1RegionSpec::V1COutputToTable(DataTable* dtab, bool fmt_only) {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);
  V1COutputToTable_impl(dtab, &v1c_out_r, "_r", fmt_only);
  if(binoc) {
    V1COutputToTable_impl(dtab, &v1c_out_l, "_l", fmt_only);
  }
  return true;
}

bool V1RegionSpec::V1COutputToTable_impl(DataTable* dtab, float_Matrix* v1c_out,  
                                         const String& col_sufx, bool fmt_only) {
  DataCol* col;
  taVector2i cc;         // complex coords
  int idx;

  if((v1c_save & SEP_MATRIX) && v1c_specs.end_stop) {
    col = data_table->FindMakeColName
      (name + "_v1ls" + col_sufx, idx, DataTable::VT_FLOAT, 4,
       v1c_feat_geom.x, 1, v1c_img_geom.x, v1c_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
        for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
          for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
            float lsval = v1c_out->FastEl4d(ang, 0, cc.x, cc.y); // len sum
            dout->FastEl4d(ang, 0, cc.x, cc.y) = lsval;
          }
        }
      }
    }

    col = data_table->FindMakeColName
      (name + "_v1es" + col_sufx, idx, DataTable::VT_FLOAT, 4,
       v1c_feat_geom.x, 2, v1c_img_geom.x, v1c_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(cc.y = 0; cc.y < v1c_img_geom.y; cc.y++) {
        for(cc.x = 0; cc.x < v1c_img_geom.x; cc.x++) {
          for(int dir=0; dir < 2; dir++) {                  // direction
            for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
              float esval = v1c_out->FastEl4d(ang, 1+dir, cc.x, cc.y); // len sum
              dout->FastEl4d(ang, dir, cc.x, cc.y) = esval;
            }
          }
        }
      }
    }
  }
  else {
    col = data_table->FindMakeColName(name + "_v1c" + col_sufx, idx,
                                      DataTable::VT_FLOAT, 4,
                      v1c_feat_geom.x, v1c_feat_geom.y, v1c_img_geom.x, v1c_img_geom.y);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      dout->CopyFrom(v1c_out);
    }
  }
        
  return true;
}

bool V1RegionSpec::SIOutputToTable(DataTable* dtab, bool fmt_only) {
  bool binoc = (region.ocularity == VisRegionParams::BINOCULAR);

  if(si_specs.v1s) {
    FourDimMatrixToTable(dtab, &si_v1s_out_r, "_si_v1s_r", fmt_only);
    if(binoc) {
      FourDimMatrixToTable(dtab, &si_v1s_out_l, "_si_v1s_l", fmt_only);
    }
  }

  if(si_specs.v1pi) {
    FourDimMatrixToTable(dtab, &si_v1pi_out_r, "_si_v1pi_r", fmt_only);
    if(binoc) {
      FourDimMatrixToTable(dtab, &si_v1pi_out_l, "_si_v1pi_l", fmt_only);
    }
  }

  if(si_specs.v1c) {
    FourDimMatrixToTable(dtab, &si_v1c_out_r, "_si_v1c_r", fmt_only);
    if(binoc) {
      FourDimMatrixToTable(dtab, &si_v1c_out_l, "_si_v1c_l", fmt_only);
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


/////////////////////////////////////////////////////
//                      Graphing

int  V1RegionSpec::AngleDeg(int ang_no) {
  int ang_inc = 180 / v1s_specs.n_angles;
  return ang_no * ang_inc;
}

void V1RegionSpec::GridGaborFilters(DataTable* graph_data, int which_filter) {
  if(which_filter == 2)
    v1s_specs_2.GridFilters(v1s_gabor_filters_2, graph_data);
  else if(which_filter == 3)
    v1s_specs_3.GridFilters(v1s_gabor_filters_3, graph_data);
  else
    v1s_specs.GridFilters(v1s_gabor_filters, graph_data);
}

void V1RegionSpec::GridV1Stencils(DataTable* graph_data) {
  Init();                       // need to init stencils for sure!

  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridV1Stencils", true);
  }
  graph_data->StructUpdate(true);
  graph_data->Reset();

  graph_data->SetUserData("N_ROWS", 4);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);
  graph_data->SetUserData("BLOCK_SPACE", 4.0f);
  //  graph_data->SetUserData("WIDTH", .5f + (float)input_size.retina_size.x / (float)input_size.retina_size.y);

  taVector2i max_sz(v1s_specs.size, v1s_specs.size);
  max_sz.Max(si_specs.spat_rf);

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

  if(square_group.sg_rf > 1) { // v1complex, sg
    for(int ang = 0; ang < v1s_specs.n_angles; ang++) { // angles
      graph_data->AddBlankRow();
      nmda->SetValAsString("V1C SubGp Ctrs: " + String(AngleDeg(ang)), -1);
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
      if(square_group.sg_rf == 2) {
        int nctrs = v1sg2_stencils.FastEl3d(2, 0, ang);       // length stored here
        for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
          int xp = v1sg2_stencils.FastEl3d(X, ctrdx, ang);
          int yp = v1sg2_stencils.FastEl3d(Y, ctrdx, ang);
          ic.x = brd.x + xp;
          ic.y = brd.y + yp;

          if(ic.WrapHalf(max_sz)) continue;
          mat->FastEl2d(ic.x,ic.y) = (ctrdx % 2 == 0) ? 1.0f: -1.0f;
        }
      }
      else {                                           // 4
        int nctrs = v1sg4_stencils.FastEl3d(2, 0, ang);       // length stored here
        for(int ctrdx = 0; ctrdx < nctrs; ctrdx++) {
          int xp = v1sg4_stencils.FastEl3d(X, ctrdx, ang);
          int yp = v1sg4_stencils.FastEl3d(Y, ctrdx, ang);
          ic.x = brd.x + xp;
          ic.y = brd.y + yp;

          if(ic.WrapHalf(max_sz)) continue;
          mat->FastEl2d(ic.x,ic.y) = (ctrdx % 2 == 0) ? 1.0f: -1.0f;
        }
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

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}


void V1RegionSpec::PlotSpacing(DataTable* graph_data, bool reset) {
  taProject* proj = GetMyProj();
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

