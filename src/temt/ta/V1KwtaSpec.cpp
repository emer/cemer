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


void V1KwtaSpec::Initialize() {
  on = false;
  gp_k = 1;
  gp_g = 0.1f;
  kwta_pt = 0.5f;
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

void V1KwtaSpec::Compute_Inhib(float_Matrix& inputs, float_Matrix& gc_i_mat) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);
  float_Matrix gpmat;
  gpmat.SetGeom(2, gxs, gys);
  gc_i_mat.SetGeom(2, ixs, iys);
  float max_gi = 0.0f;
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          gpmat.FastEl(gx, gy) = Compute_IThresh(g_bar_e * inputs.FastEl(gx, gy, ix, iy));
        }
      }
      float top_k_avg, bot_k_avg;
      taMath_float::vec_kwta_avg(top_k_avg, bot_k_avg, &gpmat, gp_k, true);
      float nw_gi = bot_k_avg + kwta_pt * (top_k_avg - bot_k_avg);
      gc_i_mat.FastEl(ix, iy) = nw_gi;
      max_gi = MAX(max_gi, nw_gi);
    }
  }
  if(gp_g > 0.0f) {
    float gpg_eff = gp_g * max_gi;
    for(int iy=0; iy < iys; iy++) {
      for(int ix=0; ix < ixs; ix++) {
        float gi = gc_i_mat.FastEl(ix, iy);
        gi = MAX(gi, gpg_eff);
        gc_i_mat.FastEl(ix, iy) = gi;
      }
    }
  }
}

bool V1KwtaSpec::Compute_Kwta(float_Matrix& inputs, float_Matrix& outputs,
                              float_Matrix& gc_i_mat) {
  if(TestError(inputs.dims() != 4, "Compute_Kwta",
               "input matrix must have 4 dimensions: gp x,y, outer (image) x,y"))
    return false;

  Compute_Inhib(inputs, gc_i_mat);
  Compute_Act(inputs, outputs, gc_i_mat);
  return true;
}

void V1KwtaSpec::Compute_Act(float_Matrix& inputs, float_Matrix& outputs,
                              float_Matrix& gc_i_mat) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);

  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      float gi = gc_i_mat.FastEl(ix, iy);
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          float raw = inputs.FastEl(gx, gy, ix, iy);
          float ge = g_bar_e * raw;
          float act = Compute_ActFmIn(ge, gi);
          outputs.FastEl(gx, gy, ix, iy) = act;
        }
      }
    }
  }
}

void V1KwtaSpec::Compute_Inhib_IThr(float_Matrix& inputs, float_Matrix& gc_i_mat,
                                    float_Matrix& ithrs) {
  Compute_All_IThr(inputs, ithrs);
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);
  float_Matrix gpmat;
  gpmat.SetGeom(2, gxs, gys);
  gc_i_mat.SetGeom(2, ixs, iys);
  float max_gi = 0.0f;
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          gpmat.FastEl(gx, gy) = ithrs.FastEl(gx, gy, ix, iy);
        }
      }
      float top_k_avg, bot_k_avg;
      taMath_float::vec_kwta_avg(top_k_avg, bot_k_avg, &gpmat, gp_k, true);
      float nw_gi = bot_k_avg + kwta_pt * (top_k_avg - bot_k_avg);
      gc_i_mat.FastEl(ix, iy) = nw_gi;
      max_gi = MAX(max_gi, nw_gi);
    }
  }
  if(gp_g > 0.0f) {
    float gpg_eff = gp_g * max_gi;
    for(int iy=0; iy < iys; iy++) {
      for(int ix=0; ix < ixs; ix++) {
        float gi = gc_i_mat.FastEl(ix, iy);
        gi = MAX(gi, gpg_eff);
        gc_i_mat.FastEl(ix, iy) = gi;
      }
    }
  }
}

void V1KwtaSpec::Compute_All_IThr(float_Matrix& inputs, float_Matrix& ithrs) {
  int gxs = inputs.dim(0);
  int gys = inputs.dim(1);
  int ixs = inputs.dim(2);
  int iys = inputs.dim(3);
  ithrs.SetGeom(4, gxs, gys, ixs, iys);
  for(int iy=0; iy < iys; iy++) {
    for(int ix=0; ix < ixs; ix++) {
      for(int gy=0; gy < gys; gy++) {
        for(int gx=0; gx < gxs; gx++) {
          ithrs.FastEl(gx, gy, ix, iy) = Compute_IThresh(g_bar_e * inputs.FastEl(gx, gy, ix, iy));
        }
      }
    }
  }
}
