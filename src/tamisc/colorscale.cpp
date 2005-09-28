// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// colorscale.cc

#include "colorscale.h"

#include "icolor.h"

#ifdef TA_GUI
#include "ta_qt.h"

#include <qapplication.h> // default palette
#include <qpalette.h>
#endif

//////////////////////////
//	RGBA		//
//////////////////////////

RGBA::RGBA(float rd, float gr, float bl, float al) {
  Register(); Initialize(); SetDefaultName();
  r = rd; g = gr; b = bl; a = al;
  color_.setRgb(r, g, b, a);
}

void RGBA::Initialize() {
  r = g = b = 0.0f;
  a = 1.0f;
  name = "";
}

void RGBA::Destroy() {
}

void RGBA::UpdateAfterEdit(){
  inherited::UpdateAfterEdit();
  color_.setRgb(r, g, b, a);
  if(!taMisc::gui_active)    return;
  if (!name.empty()){
    if(!(iColor::find
	 (/*ivSession::instance()->default_display(),*/(char *)name,r,g,b))){
      taMisc::Error("Color: " , name , " not found for this display");
    }

  }
}

void RGBA::Copy_(const RGBA& cp) {
  name = cp.name;		// copy name because it goes with r,g,b
  r = cp.r;
  g = cp.g;
  b = cp.b;
  a = cp.a;
  color_.setRgb(r, g, b, a);
}

const iColor* RGBA::color() const {
  //note: updating every time is slower, but WAY WAY more reliable than trying
  // to track every possible case of manual updating
  ((iColor&)color_).setRgb(r, g, b, a); // note: cast away constness is ok for an internal cache variable
  return &color_;
}

String RGBA::ToString_RGBA() {
  return String("R:") + String(r) +
    String("; G:") + String(g) +
    String("; B:") + String(b) +
    String("; A:") + String(a);
}


//////////////////////////
//	TAColor		//
//////////////////////////


// set the color to a new color based on the values given
void TAColor::SetColor(float r, float g, float b, float a, RGBA* background){
  color_.setRgb(r,g,b,a);


  // compute contrast color

  // first get background color from specified background and its
  // relationship to the gui's background

  float bgc = 0.0;
  if (background !=  NULL)
    bgc = ((background->r + background->g + background->b)/3.0f) *  background->a;
  float 	dr=0.0f,dg=0.0f,db=0.0f;
#ifdef TA_GUI
  ((iColor)(QApplication::palette().color(QPalette::Active, QColorGroup::Background))).intensities(dr, dg, db);
#endif
  bgc += ((dr+dg+db)/3.0f) * ((background == NULL) ? 1.0 :
                              (1.0 - background->a));

  // now get the intensity of the foreground color

  float		nr=1.0,ng=1.0,nb=1.0,na=1.0;
  color_.intensities(nr,ng,nb);
  na = color_.alpha();
  // compute the ratio of the background color to the foreground color
  float bw = ((nr+ng+nb)/3.0f) * na + (bgc * (1.0f-na));

  if(bw >= 0.5f)        // color is mostly white
    nr = ng = nb = 0.0; // make contrast black
  else                  // color is mostly black
    nr = ng = nb = 1.0; // make contrast white
  contrastcolor_.setRgb(nr,ng,nb,1.0);

}



//////////////////////////
//   ColorScaleSpec	//
//////////////////////////

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

void ColorScaleSpec::GenRanges(TAColor_List* cl, int chunks) {
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
      TAColor* pc = (TAColor *) cl->New(1,&TA_TAColor);
      float pcr = low_color->r + (dr * (float)j);
      float pcg = low_color->g + (dg * (float)j);
      float pcb = low_color->b + (db * (float)j);
      float pca = low_color->a + (da * (float)j);
      pc->SetColor(pcr,pcg,pcb,pca,&background);
    }
  }
  TAColor* pc = (TAColor*)cl->New(1,&TA_TAColor);
  RGBA* hi_color = clr.Peek();
  pc->SetColor(hi_color, &background);
}


//////////////////////////////////
// 	ColorScaleSpec_MGroup	//
//////////////////////////////////

void ColorScaleSpec_MGroup::NewDefaults() {
  if(size != 0)
    return;

  ColorScaleSpec* cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ColdHot";
  cs->clr.Add(new RGBA(0.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ColdHotPurple";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueBlackRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.3f, 0.3f, 0.3f));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueGreyRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueWhiteRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.9f, 0.9f, 0.9f));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueGreenRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.9f, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_Rainbow";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ROYGBIV";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.5));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_DarkLight";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_LightDark";
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->background.name = "grey64";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "M_DarkLight";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0, 0.0));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;
  cs->background.a = 1.0f;

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "M_LightDark";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0, 1.0));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;
  cs->background.a = 1.0f;

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "M_LightDarkLight";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0, 0.0));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;
  cs->background.a = 1.0f;

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "P_DarkLight";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "P_DarkLight_brite";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(.3f, .3f, .3f));
  cs->clr.Add(new RGBA(.6f, .6f, .6f));
  cs->clr.Add(new RGBA(.8f, .8f, .8f));
  cs->clr.Add(new RGBA(1, 1, 1));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "P_LightDark";
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "P_DarkLightDark";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "P_LightDarkLight";
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->background.r = 1.0f;
  cs->background.g = 1.0f;
  cs->background.b = 1.0f;

}

#define CSSMG_COLS 5

int ColorScaleSpec_MGroup::NumListCols() const {
  return taList_impl::NumListCols() + CSSMG_COLS;
}

String ColorScaleSpec_MGroup::GetColHeading(int col) {
  switch (col - taList_impl::NumListCols()) {
  case 0: return String("BkClr");
  case 1: return String("clr0");
  case 2: return String("clr1");
  case 3: return String("clr2");
  case 4: return String("clr3");
  case 5: return String("clr4");
  default: return taList_impl::GetColHeading(col);
  }
}

String ColorScaleSpec_MGroup::ChildGetColText_impl(taBase* child, int col, int itm_idx) {
  if (child->GetTypeDef()->InheritsFrom(&TA_ColorScaleSpec)) {
    ColorScaleSpec* css = (ColorScaleSpec*)child;
    int i = col - taList_impl::NumListCols();
    if (i == 0) {
      return css->background.ToString_RGBA();
    } else  if ((i >= 1) && (i <= 4)) {
      i = i - 1;
      if (i < css->clr.size)
        return css->clr[i]->ToString_RGBA();
      else return String(); // nothing for this item
    }
  }
  return taList_impl::ChildGetColText_impl(child, col, itm_idx);
}

void ColorScaleSpec_MGroup::SetDefaultColor() {
  NewDefaults();
  if(!taMisc::gui_active) return;
/*2004...  ivWidgetKit* wkit = ivWidgetKit::instance();
  String guiname = wkit->gui();
  if(guiname == "monochrome")
    SetDefaultEl("M_DarkLight");
  else */
    SetDefaultEl("C_ColdHot");
}

//////////////////////////
//   ScaleRange		//
//////////////////////////

void ScaleRange::SetFromScale(ColorScale& cs) {
  auto_scale = cs.auto_scale;
  min = cs.min;
  max = cs.max;
}


//////////////////////////
//	ColorScale	//
//////////////////////////

void ColorScale::Initialize() {
  chunks = 0;
  spec = NULL;
  owner = NULL;
  min = 0.0f;
  max = 0.0f;
  range = 0.0f;
  zero = 0.0f;
  auto_scale = true;
  colors.SetBaseType(&TA_TAColor);
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
  taBase::DelPointer((TAPtr*)&spec);
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

void ColorScale::MapColors() {
  if(spec == NULL)
    return;
  background.setRgb(spec->background.r, spec->background.g,
		      spec->background.b, spec->background.a);
  // ensure that chunks evenly fit with number of colors
  int n_range = spec->clr.size-1;
  if(chunks % n_range != 1) {
    chunks += (n_range - (chunks % n_range)) + 1;
  }
  spec->GenRanges(&colors, chunks);

  maxout.SetColor(new iColor(*GetColor(colors.size-1),.25), &spec->background);
  minout.SetColor(new iColor(*GetColor(0),.25), &spec->background);
#ifdef CYGWIN
  // do not set alpha level on colors because it behaves differently than solid
  // colors -- does not appear to be reset or something, causing wrong units to
  // be highlighted in network viewer.
  nocolor.SetColor(new iColor(*GetColor((colors.size+1)/2)),&spec->background);
#else
  nocolor.SetColor(new iColor(*GetColor((colors.size+1)/2),.25),&spec->background);
#endif
}

void ColorScale::DefaultChunks(){
/*this is 2004... what is "monochrome"???  ivWidgetKit* wkit = ivWidgetKit::instance();
  String guiname = wkit->gui();
  if(guiname == "monochrome")
    chunks = taMisc::mono_scale_size;
  else */
    chunks = taMisc::color_scale_size;
}

float ColorScale::GetAbsPercent(float val){
  if (val >= zero)
    return ((max - zero) == 0.0f) ? 0.0f : fabs((val - zero) / (max - zero));
  else
    return ((zero - min) == 0.0f) ? 0.0f : fabs((zero - val) / (zero - min));
}

const iColor* ColorScale::GetColor(float val, const iColor** maincolor, const iColor** contrast) {
  int idx;
  const iColor* m;
  const iColor* c;
  if (range == 0) {
    m = GetColor((int) ((.5 * (float)(chunks-1)) + .5));
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
  if (maincolor!= NULL) *maincolor = m;
  if (contrast != NULL) *contrast = c;
  return m;
}

const iColor* ColorScale::Get_Background(){
  return &background;
}

const iColor* ColorScale::GetColor(int idx) {
  if((idx >= 0) && (idx < colors.size)) {
    return ((TAColor *) colors[idx])->color();
  }
  else {
    return NULL;
  }
}

const iColor* ColorScale::GetContrastColor(int idx) {
  if((idx >= 0) && (idx < colors.size)) {
    return ((TAColor *) colors[idx])->contrastcolor();
  }
  else {
    return NULL;
  }
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
  String tmpath = ".ColorScaleSpec_MGroup";
  ColorScaleSpec_MGroup* gp =
    (ColorScaleSpec_MGroup*)tabMisc::root->FindFromPath(tmpath, md);
  if((gp == NULL) || (md == NULL)) {
    taMisc::Error("ColorScaleSpec_MGroup not found from root in NewDefaults");
    return;
  }
  gp->NewDefaults();
  taBase::SetPointer((TAPtr*)&spec, (ColorScaleSpec*)gp->DefaultEl());
  DefaultChunks();
  MapColors();
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

void ColorScale::SetFromScaleRange(ScaleRange* sr) {
  if (!sr) return;
  auto_scale = sr->auto_scale;
  SetMinMax(sr->min, sr->max);
}

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





