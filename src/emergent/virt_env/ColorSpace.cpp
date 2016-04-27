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

#include "ColorSpace.h"

#include <float_Matrix>
#include <taVector2i>
#include <taMath_float>

TA_BASEFUNS_CTORS_DEFN(ColorSpace);

int ColorSpace::lookup_levels = 64;
int ColorSpace::lookup_levels_m1 = 63;
bool ColorSpace::use_lookup = true;
float_Matrix ColorSpace::srgb_to_oppo;

void ColorSpace::Initialize() {
  use_lookup = true;
  lookup_levels = 64;
  lookup_levels_m1 = 63;
}

// TODO: investigate this as a faster alternative at some point:
// https://graphics.stanford.edu/~boulos/papers/orgb_sig.pdf


void ColorSpace::sRGBtoGreyFastImg(float_Matrix& grey_img,
                                   const float_Matrix& srgb_img) {
  taVector2i img_size(srgb_img.dim(0), srgb_img.dim(1));
  grey_img.SetGeom(2, img_size.x, img_size.y);
  for(int yi = 0; yi < img_size.y; yi++) {
    for(int xi = 0; xi < img_size.y; xi++) {
      float r_s = srgb_img.FastEl3d(xi, yi, 0);
      float g_s = srgb_img.FastEl3d(xi, yi, 1);
      float b_s = srgb_img.FastEl3d(xi, yi, 2);
      float grey = (1.0f / 3.0f) * (r_s + g_s + b_s);
      grey_img.FastEl2d(xi, yi) = grey;
    }
  }
}

void ColorSpace::sRGBtoXYZImg(float_Matrix& xyz_img,
                              const float_Matrix& srgb_img) {
  taVector2i img_size(srgb_img.dim(0), srgb_img.dim(1));
  xyz_img.SetGeom(3, img_size.x, img_size.y, 3);
  for(int yi = 0; yi < img_size.y; yi++) {
    for(int xi = 0; xi < img_size.y; xi++) {
      float r_s = srgb_img.FastEl3d(xi, yi, 0);
      float g_s = srgb_img.FastEl3d(xi, yi, 1);
      float b_s = srgb_img.FastEl3d(xi, yi, 2);

      float X, Y, Z;
      sRGBtoXYZ(X, Y, Z, r_s, g_s, b_s);
      xyz_img.FastEl3d(xi, yi, 0) = X;
      xyz_img.FastEl3d(xi, yi, 1) = Y;
      xyz_img.FastEl3d(xi, yi, 2) = Z;
    }
  }
}

void ColorSpace::XYZtosRGBImg(float_Matrix& srgb_img,
                              const float_Matrix& xyz_img) {
  taVector2i img_size(xyz_img.dim(0), xyz_img.dim(1));
  srgb_img.SetGeom(3, img_size.x, img_size.y, 3);
  for(int yi = 0; yi < img_size.y; yi++) {
    for(int xi = 0; xi < img_size.x; xi++) {
      float X = xyz_img.FastEl3d(xi, yi, 0);
      float Y = xyz_img.FastEl3d(xi, yi, 1);
      float Z = xyz_img.FastEl3d(xi, yi, 2);

      float r_s, g_s, b_s;
      XYZtosRGB(r_s, g_s, b_s, X, Y, Z);
      srgb_img.FastEl3d(xi, yi, 0) = r_s;
      srgb_img.FastEl3d(xi, yi, 1) = g_s;
      srgb_img.FastEl3d(xi, yi, 2) = b_s;
    }
  }
}

void ColorSpace::sRGBtoOpponentsImg(float_Matrix& opp_img,
                                    const float_Matrix& srgb_img) {
  sRGBtoOpponents_GenLookup();
  taVector2i img_size(srgb_img.dim(0), srgb_img.dim(1));
  opp_img.SetGeom(3, img_size.x, img_size.y, N_OP_C);
  for(int yi = 0; yi < img_size.y; yi++) {
    for(int xi = 0; xi < img_size.x; xi++) {
      float r_s = srgb_img.FastEl3d(xi, yi, 0);
      float g_s = srgb_img.FastEl3d(xi, yi, 1);
      float b_s = srgb_img.FastEl3d(xi, yi, 2);

      float L_c, M_c, S_c, LM_c, LvM, SvLM, grey;
      if(use_lookup) {
        sRGBtoOpponents_lkup(L_c, M_c, S_c, LM_c, LvM, SvLM, grey, r_s, g_s, b_s);
      }
      else {
        sRGBtoOpponents(L_c, M_c, S_c, LM_c, LvM, SvLM, grey, r_s, g_s, b_s);
      }
      opp_img.FastEl3d(xi, yi, L_C) = L_c;
      opp_img.FastEl3d(xi, yi, M_C) = M_c;
      opp_img.FastEl3d(xi, yi, S_C) = S_c;
      opp_img.FastEl3d(xi, yi, LM_C) = LM_c;
      opp_img.FastEl3d(xi, yi, LvM_C) = LvM;
      opp_img.FastEl3d(xi, yi, SvLM_C) = SvLM;
      opp_img.FastEl3d(xi, yi, GREY) = grey;
    }
  }
}

namespace {
  inline void get_interp_idx(int& li, int& hi, float& pcthi, float& pctlo, const float val) {
    float fi = val * ColorSpace::lookup_levels_m1;
    li = (int)taMath_float::floor(fi);
    hi = (int)taMath_float::ceil(fi);
    if(li == ColorSpace::lookup_levels_m1) {
      hi = li;
      li = li-1;
      pcthi = 1.0f;
      pctlo = 0.0f;
    }
    else if(hi == li) {
      hi = li + 1;
      pcthi = 0.0f;
      pctlo = 1.0f;
    }
    else {
      pcthi = (fi - (float)li);
      pctlo = 1.0f - pcthi;
    }
  }
}

void ColorSpace::sRGBtoOpponents_lkup(float& L_c, float& M_c, float& S_c, float& LM_c,
                                      float& LvM, float& SvLM, float& grey,
                                      const float r_s, const float g_s, const float b_s) {
  float optmp[N_OP_C];

  int r0,r1,g0,g1,b0,b1;
  float rd,gd,bd, rdc,gdc,bdc;
  get_interp_idx(r0, r1, rd, rdc, r_s);
  get_interp_idx(g0, g1, gd, gdc, g_s);
  get_interp_idx(b0, b1, bd, bdc, b_s);

  for(int i=0; i< N_OP_C; i++) {
    float c00 = srgb_to_oppo.FastEl4d(i,r0,g0,b0) * rdc +
      srgb_to_oppo.FastEl4d(i,r1,g0,b0) * rd;
    float c01 = srgb_to_oppo.FastEl4d(i,r0,g0,b1) * rdc +
      srgb_to_oppo.FastEl4d(i,r1,g0,b1) * rd;
    float c10 = srgb_to_oppo.FastEl4d(i,r0,g1,b0) * rdc +
      srgb_to_oppo.FastEl4d(i,r1,g1,b0) * rd;
    float c11 = srgb_to_oppo.FastEl4d(i,r0,g1,b1) * rdc +
      srgb_to_oppo.FastEl4d(i,r1,g1,b1) * rd;
    float c0 = c00 * gdc + c10 * gd;
    float c1 = c01 * gdc + c11 * gd;
    float c = c0 * bdc + c1 * bd;
    optmp[i] = c;
  }
  L_c = optmp[L_C];
  M_c = optmp[M_C];
  S_c = optmp[S_C];
  LM_c = optmp[LM_C];
  LvM = optmp[LvM_C];
  SvLM = optmp[SvLM_C];
  grey = optmp[GREY];
}

bool ColorSpace::sRGBtoOpponents_GenLookup() {
  if(srgb_to_oppo.dims() == 4 && srgb_to_oppo.dim(1) == lookup_levels)
    return false;
  lookup_levels_m1 = lookup_levels - 1;
  srgb_to_oppo.SetGeom(4, N_OP_C, lookup_levels, lookup_levels, lookup_levels);
  float L_c, M_c, S_c, LM_c, LvM, SvLM, grey;
  for(int bi = 0; bi < lookup_levels; bi++) {
    float bf = (float)bi / (float)lookup_levels;
    for(int gi = 0; gi < lookup_levels; gi++) {
      float gf = (float)gi / (float)lookup_levels;
      for(int ri = 0; ri < lookup_levels; ri++) {
        float rf = (float)ri / (float)lookup_levels;
        sRGBtoOpponents(L_c, M_c, S_c, LM_c, LvM, SvLM, grey, rf, gf, bf);
        srgb_to_oppo.FastEl4d(L_C, ri, gi, bi) = L_c;
        srgb_to_oppo.FastEl4d(M_C, ri, gi, bi) = M_c;
        srgb_to_oppo.FastEl4d(S_C, ri, gi, bi) = S_c;
        srgb_to_oppo.FastEl4d(LM_C, ri, gi, bi) = LM_c;
        srgb_to_oppo.FastEl4d(LvM_C, ri, gi, bi) = LvM;
        srgb_to_oppo.FastEl4d(SvLM_C, ri, gi, bi) = SvLM;
        srgb_to_oppo.FastEl4d(GREY, ri, gi, bi) = grey;
      }
    }
  }
  return true;
}


void ColorSpace::RenderColorChecker_sRGB(float_Matrix& img, int width, int height) {

  int sRGB_vals[] =       
    { 115,82,68,          // 'Dark Skin';     
      194,150,130,        // 'Light Skin';    
      98,122,157,         // 'Blue Sky';      
      87,108,67,          // 'Foliage';       
      133,128,177,        // 'Blue Flower';   
      103,189,170,        // 'Bluish Green';  
      214,126,44,         // 'Orange';        
      80,91,166,          // 'Purple Red';    
      193,90,99,          // 'Moderate Red';  
      94,60,108,          // 'Purple';        
      157,188,64,         // 'Yellow Green';  
      224,163,46,         // 'Orange Yellow'; 
      56,61,150,          // 'Blue';          
      70,148,73,          // 'Green';         
      175,54,60,          // 'Red';           
      231,199,31,         // 'Yellow';        
      187,86,149,         // 'Magenta';       
      8,133,161,          // 'Cyan';          
      255,255,255,        // 'White';         
      200,200,200,        // 'Neutral 8';     
      160,160,160,        // 'Neutral 65';    
      122,122,121,        // 'Neutral 5';     
      85,85,85,           // 'Neutral 35';    
      52,52,52 };         // 'Black'};        

  taVector2i n_sq;  n_sq.SetXY(6,4);
  int num_sq = n_sq.Product();
  taVector2i img_size(width, height);
  taVector2i bord = 16;
  taVector2i marg = 8;
  taVector2i up_bord = img_size - bord;

  taVector2i net_size = img_size - 2*bord;
  taVector2i sq_size = net_size / n_sq;
  
  img.SetGeom(3, img_size.x, img_size.y, 3);
  img.InitVals(0.0f);
              
  taVector2i ic;
  for(ic.y = bord.y; ic.y < up_bord.y; ic.y++) {
    for(ic.x = bord.x; ic.x < up_bord.x; ic.x++) {
      float r_s = 0.0f; float g_s = 0.0f; float b_s = 0.0f;
      taVector2i nc = ic - bord;
      taVector2i sq_c = nc / sq_size;
      taVector2i sq_st = sq_c * sq_size + bord;
      if(ic - sq_st > marg) {
        int clr_idx = (n_sq.y-1 - sq_c.y) * n_sq.x + sq_c.x;
        if(clr_idx < num_sq) {
          r_s = sRGB_vals[clr_idx * 3] / 255.0f;
          g_s = sRGB_vals[clr_idx * 3 + 1] / 255.0f;
          b_s = sRGB_vals[clr_idx * 3 + 2] / 255.0f;

          img.FastEl3d(ic.x, ic.y, 0) = r_s;
          img.FastEl3d(ic.x, ic.y, 1) = g_s;
          img.FastEl3d(ic.x, ic.y, 2) = b_s;
        }
      }
    }
  }
}

