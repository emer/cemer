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

#include "ColorScaleSpec.h"
#include <ColorScaleColor>
#include <ColorScaleColor_List>

TA_BASEFUNS_CTORS_DEFN(ColorScaleSpec);


void ColorScaleSpec::Initialize() {
  clr.SetBaseType(&TA_RGBA);
}

void ColorScaleSpec::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(background, this);
  taBase::Own(clr, this);
}

void ColorScaleSpec::Copy_(const ColorScaleSpec& cp) {
  background = cp.background;
  clr = cp.clr;
}

void ColorScaleSpec::GenRanges(ColorScaleColor_List* cl, int chunks) {
  // for odd colors, assuming 5 clrs and 16 chunks:
  // chunk: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
  // color: 0        1           2           3           4
  // range: <-   0  -><-    1   -><-    2   -><-    3   ->

  // for even colors, assuming 4 clrs and 14 chunks:
  // chunk: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
  // color: 0           1              2              3
  // range: <-    0    -><-     1    -><-     2      ->

  int n_range = clr.size-1;		// number of ranges
  int n_per = (chunks-1) / n_range; 	// number of chunks per range
  if(n_per <= 1) {
    n_per = 1;
  }

  cl->RemoveAll();
  int i;
  for(i=0; i< n_range; i++) {
    RGBA* low_color = clr[i];
    RGBA* hi_color = clr[i+1];
    float dr = (hi_color->r - low_color->r) / ((float) n_per);
    float dg = (hi_color->g - low_color->g) / ((float) n_per);
    float db = (hi_color->b - low_color->b) / ((float) n_per);
    float da = (hi_color->a - low_color->a) / ((float) n_per);

    int j;
    for(j=0; j<n_per; j++) {
      ColorScaleColor* pc = (ColorScaleColor *) cl->New(1,&TA_ColorScaleColor);
      float pcr = low_color->r + (dr * (float)j);
      float pcg = low_color->g + (dg * (float)j);
      float pcb = low_color->b + (db * (float)j);
      float pca = low_color->a + (da * (float)j);
      pc->SetColor(pcr,pcg,pcb,pca,&background);
    }
  }
  ColorScaleColor* pc = (ColorScaleColor*)cl->New(1,&TA_ColorScaleColor);
  RGBA* hi_color = clr.Peek();
  pc->SetColor(hi_color, &background);
}

const KeyString ColorScaleSpec::key_bkclr("bkclr");
const KeyString ColorScaleSpec::key_clr0("clr0");
const KeyString ColorScaleSpec::key_clr1("clr1");
const KeyString ColorScaleSpec::key_clr2("clr2");
const KeyString ColorScaleSpec::key_clr3("clr3");
const KeyString ColorScaleSpec::key_clr4("clr4");

String ColorScaleSpec::GetColText(const KeyString& key, int itm_idx) const
{
  if (key == key_bkclr) return background.ToString_RGBA();
  int i = -1;
  if (key == key_clr0) i = 0;
  else if (key == key_clr1) i = 1;
  else if (key == key_clr2) i = 2;
  else if (key == key_clr3) i = 3;
  else if (key == key_clr4) i = 4;
  if (i >= 0) {
    if (i < clr.size)
      return clr[i]->ToString_RGBA();
    else return _nilString; // nothing for this item
  }
  return inherited::GetColText(key, itm_idx);
}
