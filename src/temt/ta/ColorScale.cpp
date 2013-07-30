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

#include "ColorScale.h"
#include <iColor>
#include <ColorScaleSpec>
#include <ColorScaleSpec_Group>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>

#include <math.h>

const iColor ColorScale::def_color;

void ColorScale::Initialize() {
  chunks = 0;
  spec = NULL;
  owner = NULL;
  min = 0.0f;
  max = 0.0f;
  range = 0.0f;
  zero = 0.0f;
  auto_scale = true;
  colors.SetBaseType(&TA_ColorScaleColor);
}

void ColorScale::Destroy() {
  CutLinks();
}

void ColorScale::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(colors, this);
  NewDefaults();
}

void ColorScale::CutLinks() {
  // don't want to close edits and purge dialogs, cause flush is generated at bad times..
//  taNBase::CutLinks();
  taBase::DelPointer((taBase**)&spec);
}

ColorScale::ColorScale(int chunk) {
  Register();
  chunks = chunk;
  spec = NULL;
  SetDefaultName();
}

void ColorScale::UpdateAfterEdit() {
  taNBase::UpdateAfterEdit();
  MapColors();
  if(owner != NULL)
    owner->UpdateAfterEdit();
}

void ColorScale::SetColorSpec(ColorScaleSpec* color_spec) {
  taBase::SetPointer((taBase**)&spec, color_spec);
  DefaultChunks();
  MapColors();
}

void ColorScale::MapColors() {
  if(spec == NULL)
    return;
  background.setRgba(spec->background.r, spec->background.g,
		      spec->background.b, spec->background.a);
  // ensure that chunks evenly fit with number of colors
  int n_range = spec->clr.size-1;
  if(chunks % n_range != 1) {
    chunks += (n_range - (chunks % n_range)) + 1;
  }
  spec->GenRanges(&colors, chunks);

  maxout.SetColor(iColor(GetColor(colors.size-1),.25), &spec->background);
  minout.SetColor(iColor(GetColor(0),.25), &spec->background);
  nocolor.SetColor(iColor(GetColor((colors.size+1)/2),.25), &spec->background);
}

void ColorScale::DefaultChunks(){
  chunks = taMisc::color_scale_size;
}

float ColorScale::GetAbsPercent(float val){
  if (val >= zero)
    return ((max - zero) == 0.0f) ? 0.0f : fabs((val - zero) / (max - zero));
  else
    return ((zero - min) == 0.0f) ? 0.0f : fabs((zero - val) / (zero - min));
}

const iColor ColorScale::GetColor(float val, float& sc_val, iColor* maincolor,
    iColor* contrast) 
{
  iColor m;
  iColor c;
  if (std::isnan(val) || std::isinf(val)) {
    val = 0.0f;
    m = iColor::black_;
    c.setRgb(255,255,255);
  } else {
    int idx;
    if (range == 0.0f) {
      m = GetColor((int) ((.5f * (float)(chunks-1)) + .5f));
      c = GetContrastColor(chunks-1);
    } else if(val > max) {
      m = maxout.color();
      c = maxout.contrastcolor(); // GetContrastColor(bar->scale->chunks -1);
    } else if(val < min) {
      m = minout.color();
      c = minout.contrastcolor(); // GetContrastColor(0);
    } else {
      idx = GetIdx(val);
      m = GetColor(idx);
      c = GetContrastColor(idx);
    }
  }
  if (maincolor!= NULL) *maincolor = m;
  if (contrast != NULL) *contrast = c;
  if(range > 0.0f) {
    sc_val = (val - zero) / range;
    if(sc_val > 1.0f) sc_val = 1.0f;
    else if(sc_val < -1.0f) sc_val = -1.0f;
  }
  else {
    sc_val = 0.0f;
  }
  return m;
}

const iColor ColorScale::Get_Background(){
  return background;
}

const iColor ColorScale::GetColor(int idx, bool* ok) {
  if((idx >= 0) && (idx < colors.size)) {
    if (ok) *ok = true;
    return ((ColorScaleColor *) colors[idx])->color();
  }
  if (ok) *ok = false;
  return def_color;
}

const iColor ColorScale::GetContrastColor(int idx, bool* ok) {
  if((idx >= 0) && (idx < colors.size)) {
    if (ok) *ok = true;
    return ((ColorScaleColor *) colors[idx])->contrastcolor();
  }
  if (ok) *ok = false;
  return def_color;
}

int ColorScale::GetIdx(float val) {
  int chnk = chunks-1;
  float rval = ((max - min) == 0.0f) ? 0.0f : (val-min)/(max-min);
  int idx = (int) ((rval * (float)chnk) + .5);
  idx = MAX(0, idx);
  idx = MIN(chnk, idx);
  return idx;
}

void ColorScale::NewDefaults() {
  MemberDef* md;
  String tmpath = ".ColorScaleSpec_Group";
  ColorScaleSpec_Group* gp =
    (ColorScaleSpec_Group*)tabMisc::root->FindFromPath(tmpath, md);
  if(!gp) {
    taMisc::Error("ColorScaleSpec_Group not found from root in NewDefaults");
    return;
  }
  gp->NewDefaults();
  SetColorSpec((ColorScaleSpec*)gp->DefaultEl());
}

void ColorScale::ModRange(float val){
  range = fabs(val);
  min = zero - range;
  max = zero + range;
}

void ColorScale::ModRoundRange(float val) {
  val = fabs(val);
  int val_order = (val <= 0) ? 0 : (int)log10(fabs(val)); // order of the value
  double val_units;
  if(val > 1)
    val_units = powf(10.0f, (float)val_order); // units
  else
    val_units = powf(10.0f, (float)(val_order-1));
  float val_rat = (val / val_units);
  int rng_val = 0;
  if(val_rat <= 1.0)
    rng_val = 1;
  if(val_rat <= 2.0)
    rng_val = 2;
  else if(val_rat <= 5.0)
    rng_val = 5;
  else
    rng_val = 10;

  range = (double)rng_val * val_units;
  if(range == 0)	range = 1.0; // avoid div by 0
  min = zero - range;
  max = zero + range;
}

void ColorScale::FixRangeZero() {
  range = (max - min) /2.0;
  zero = max - range;
};

void ColorScale::SetMinMax(float mn,float mx){
  if((min == mn) && (max == mx)) return;
  min = mn; max = mx;
  FixRangeZero();
}

bool ColorScale::UpdateMinMax(float val) {
  bool rval = false;
  if (val < min) {min = val; rval = true;}
  if (val > max) {max = val; rval = true;}
  if (rval) FixRangeZero();
  return rval;
}

void ColorScale::UpdateMinMax(float mn, float mx) {
  if(mn < min) min = mn;
  if(mx > max) max = mx;
  FixRangeZero();
}

void ColorScale::SymRange() {
  float mxabs = MAX(fabsf(min), fabsf(max));
  min = -mxabs; max = mxabs;
  FixRangeZero();
}

