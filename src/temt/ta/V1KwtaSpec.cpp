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

#include "V1KwtaSpec.h"
#include <float_Matrix>
#include <taMath_float>

// #include <taMisc>

void V1KwtaSpec::Initialize() {
  on = -1;                      // detect old loads
  mode = OFF;
  gp_k = 1;
  gi = 2.0f;
  gp_g = 0.1f;
  kwta_pt = 0.5f;
  ff = 1.0f;
  fb = 0.5f;
  n_itr = 10;
  fb_dt = 0.3f;
  ff0 = 0.1f;
  gain = 40.0f;
  nvar = 0.01f;
  g_bar_l = 0.1f;

  // gelin defaults:
  g_bar_e = 0.5f;
  e_rev_e = 1.0f;
  e_rev_l = 0.3f;
  thr = 0.5f;

  noise_conv.x_range.min = -.05f;
  noise_conv.x_range.max = .05f;
  noise_conv.res = .001f;
  noise_conv.UpdateAfterEdit_NoGui();

  nxx1_fun.x_range.min = -.03f;
  nxx1_fun.x_range.max = 1.0f;
  nxx1_fun.res = .001f;
  nxx1_fun.UpdateAfterEdit_NoGui();

  gber_l = g_bar_l * e_rev_l;
  e_rev_sub_thr_e = e_rev_e - thr;
  e_rev_sub_thr_i = e_rev_l - thr;
  gbl_e_rev_sub_thr_l = g_bar_l * (e_rev_l - thr);
  thr_sub_e_rev_i = thr - e_rev_l;
  thr_sub_e_rev_e = thr - e_rev_e;

  CreateNXX1Fun();
}

void V1KwtaSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(on != -1) {
    if(on == 0) mode = OFF;
    else mode = KWTA;           // preserve old
  }

  // these are all gelin defaults
  g_bar_e = 0.5f;
  e_rev_e = 1.0f;
  e_rev_l = 0.3f;
  thr = 0.5f;

  gber_l = g_bar_l * e_rev_l;
  e_rev_sub_thr_e = e_rev_e - thr;
  e_rev_sub_thr_i = e_rev_l - thr;
  gbl_e_rev_sub_thr_l = g_bar_l * (e_rev_l - thr);
  thr_sub_e_rev_i = thr - e_rev_l;
  thr_sub_e_rev_e = thr - e_rev_e;

  CreateNXX1Fun();
}

void V1KwtaSpec::CreateNXX1Fun() {
  // first create the gaussian noise convolver
  nxx1_fun.x_range.max = 1.0f;
  nxx1_fun.res = .001f; // needs same fine res to get the noise transitions
  nxx1_fun.UpdateAfterEdit_NoGui();
  float ns_rng = 3.0f * nvar;   // range factor based on noise level -- 3 sd
  ns_rng = MAX(ns_rng, nxx1_fun.res);
  nxx1_fun.x_range.min = -ns_rng;

  noise_conv.x_range.min = -ns_rng;
  noise_conv.x_range.max = ns_rng;
  noise_conv.res = nxx1_fun.res;
  noise_conv.UpdateAfterEdit_NoGui();

  noise_conv.AllocForRange();
  int i;
  float eff_nvar = MAX(nvar, 1.0e-6f); // just too lazy to do proper conditional for 0..
  float var = eff_nvar * eff_nvar;
  for(i=0; i < noise_conv.size; i++) {
    float x = noise_conv.Xval(i);
    noise_conv[i] = expf(-((x * x) / var)); // shouldn't there be a factor of 1/2 here..?
  }

  // normalize it
  float sum = 0.0f;
  for(i=0; i < noise_conv.size; i++)
    sum += noise_conv[i];
  for(i=0; i < noise_conv.size; i++)
    noise_conv[i] /= sum;

  // then create the initial function
  FunLookup fun;
  fun.x_range.min = nxx1_fun.x_range.min + noise_conv.x_range.min;
  fun.x_range.max = nxx1_fun.x_range.max + noise_conv.x_range.max;
  fun.res = nxx1_fun.res;
  fun.UpdateAfterEdit_NoGui();
  fun.AllocForRange();

  for(i=0; i<fun.size; i++) {
    float x = fun.Xval(i);
    float val = 0.0f;
    if(x > 0.0f)
      val = (gain * x) / ((gain * x) + 1.0f);
    fun[i] = val;
  }

  nxx1_fun.Convolve(fun, noise_conv); // does alloc
}

void V1KwtaSpec::Compute_Kwta(float_Matrix& inputs, float_Matrix& gc_i_mat) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);
  gpmat.SetGeom(2, gxs, gys);
  gc_i_mat.SetGeom(2, ixs, iys);
  float max_gi = 0.0f;
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          gpmat.FastEl2d(gx, gy) = 
            Compute_IThresh(g_bar_e * inputs.FastEl4d(gx, gy, ix, iy));
        }
      }
      float top_k_avg, bot_k_avg;
      taMath_float::vec_kwta_avg(top_k_avg, bot_k_avg, &gpmat, gp_k, true);
      float nw_gi = bot_k_avg + kwta_pt * (top_k_avg - bot_k_avg);
      gc_i_mat.FastEl2d(ix, iy) = nw_gi;
      max_gi = MAX(max_gi, nw_gi);
    }
  }
  if(gp_g > 0.0f) {
    float gpg_eff = gp_g * max_gi;
    for(int iy=0; iy < iys; iy++) {
      for(int ix=0; ix < ixs; ix++) {
        float gig = gc_i_mat.FastEl2d(ix, iy);
        gig = MAX(gig, gpg_eff);
        gc_i_mat.FastEl2d(ix, iy) = gig;
      }
    }
  }
}

void V1KwtaSpec::Compute_FFFB(float_Matrix& inputs, float_Matrix& outputs, 
                              float_Matrix& gc_i_mat) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);
  float normval = 1.0f / (gxs * gys);
  float max_gi = 0.0f;
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      float avg_netin = 0.0f;
      float avg_act = 0.0f;
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          avg_netin += inputs.FastEl4d(gx, gy, ix, iy);
          avg_act += outputs.FastEl4d(gx, gy, ix, iy);
        }
      }
      avg_netin *= normval;
      avg_act *= normval;
      float nw_ffi = FFInhib(avg_netin);
      float nw_fbi = FBInhib(avg_act);

      float& fbi = gc_i_mat.FastEl3d(ix, iy, 1);
      fbi = fb_dt * nw_fbi + (1.0f - fb_dt) * fbi;
      
      float nw_gi = gi * (nw_ffi + nw_fbi);
      gc_i_mat.FastEl3d(ix, iy, 0) = nw_gi;
      max_gi = MAX(max_gi, nw_gi);
    }
  }
  if(gp_g > 0.0f) {
    float gpg_eff = gp_g * max_gi;
    for(int iy=0; iy < iys; iy++) {
      for(int ix=0; ix < ixs; ix++) {
        float gig = gc_i_mat.FastEl3d(ix, iy, 0);
        gig = MAX(gig, gpg_eff);
        gc_i_mat.FastEl3d(ix, iy, 0) = gig;
      }
    }
  }
}

bool V1KwtaSpec::Compute_Inhib(float_Matrix& inputs, float_Matrix& outputs,
                               float_Matrix& gc_i_mat) {
  if(TestError(inputs.dims() != 4, "Compute_Kwta",
               "input matrix must have 4 dimensions: gp x,y, outer (image) x,y"))
    return false;

  if(mode == FFFB) {
    outputs.InitVals(0.0f);
    int ixs = inputs.dim(2);
    int iys = inputs.dim(3);
    gc_i_mat.SetGeom(3, ixs, iys, 2); // extra copy to hold onto fb inhib for temp integ
    gc_i_mat.InitVals(0.0f);
    for(int i=0; i<n_itr; i++) {
      Compute_FFFB(inputs, outputs, gc_i_mat);
      Compute_Act(inputs, outputs, gc_i_mat);
    }
  }
  else {
    Compute_Kwta(inputs, gc_i_mat);
    Compute_Act(inputs, outputs, gc_i_mat);
  }
  return true;
}

bool V1KwtaSpec::Compute_Inhib_Extra(float_Matrix& inputs, float_Matrix& outputs,
                                     float_Matrix& gc_i_mat, float_Matrix& extra_inh) {
  if(TestError(inputs.dims() != 4, "Compute_Kwta",
               "input matrix must have 4 dimensions: gp x,y, outer (image) x,y"))
    return false;
  
  if(mode == FFFB) {
    outputs.InitVals(0.0f);
    int ixs = inputs.dim(2);
    int iys = inputs.dim(3);
    gc_i_mat.SetGeom(3, ixs, iys, 2);
    gc_i_mat.InitVals(0.0f);
    for(int i=0; i<n_itr; i++) {
      Compute_FFFB(inputs, outputs, gc_i_mat);
      Compute_Act_Extra(inputs, outputs, gc_i_mat, extra_inh);
    }
  }
  else {
    Compute_Kwta(inputs, gc_i_mat);
    Compute_Act_Extra(inputs, outputs, gc_i_mat, extra_inh);
  }
  return true;
}

void V1KwtaSpec::Compute_Act(float_Matrix& inputs, float_Matrix& outputs,
                              float_Matrix& gc_i_mat) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);

  // float avg_da = 0.0f;
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      float gig = gc_i_mat.FastEl2d(ix, iy);
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          float raw = inputs.FastEl4d(gx, gy, ix, iy);
          float ge = g_bar_e * raw;
          float act = Compute_ActFmIn(ge, gig);
          float& out = outputs.FastEl4d(gx, gy, ix, iy);
          // float da = act - out;
          // avg_da += fabsf(da);
          out = act;
        }
      }
    }
  }
  // avg_da /= (float)inputs.size;
  // if(mode == FFFB) {
  //   taMisc::Info("avg_da:", String(avg_da));
  // }
}

void V1KwtaSpec::Compute_Act_Extra(float_Matrix& inputs, float_Matrix& outputs,
                                   float_Matrix& gc_i_mat, float_Matrix& extra_inh) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);

  // float avg_da = 0.0f;
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      float gig = gc_i_mat.FastEl2d(ix, iy);
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          float raw = inputs.FastEl4d(gx, gy, ix, iy);
          float ge = g_bar_e * raw;
          float ei =  extra_inh.FastEl4d(gx, gy, ix, iy);
          float gi_eff;
          if(mode == KWTA) {
            float eig = Compute_IThresh(g_bar_e * extra_inh.FastEl4d(gx, gy, ix, iy));
            gi_eff = MAX(gig, eig);
          }
          else { // FFFB
            float eig = gi * FFInhib(extra_inh.FastEl4d(gx, gy, ix, iy));
            gi_eff = MAX(gig, eig);
          }
          float act = Compute_ActFmIn(ge, gi_eff);
          float& out = outputs.FastEl4d(gx, gy, ix, iy);
          // float da = act - out;
          // avg_da += fabsf(da);
          out = act;
        }
      }
    }
  }
  // avg_da /= (float)inputs.size;
  // if(mode == FFFB) {
  //   taMisc::Info("avg_da:", String(avg_da));
  // }
}

