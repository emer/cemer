// Copyright, 1995-2007, Regents of the University of Colorado,
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



// colorscale.cc

#include "colorscale.h"

#include "icolor.h"
#include "ta_project.h"

#ifdef TA_GUI
# include "ta_qt.h"
# include <qapplication.h> // default palette
# include <qpalette.h>
#endif


//////////////////////////
//  taColor		//
//////////////////////////

const iColor taColor::color() const {
  iColor rval(r,g,b,a);
  return rval;
}

void taColor::setColor(const iColor& cp) {
  Set(cp.redf(), cp.greenf(), cp.bluef(), cp.alphaf());
}

//////////////////////////
//	RGBA		//
//////////////////////////

RGBA::RGBA(float rd, float gr, float bl, float al) {
  Initialize(); SetDefaultName();
  r = rd; g = gr; b = bl; a = al;
}

void RGBA::Initialize() {
  r = g = b = 0.0f;
  a = 1.0f;
}

void RGBA::Destroy() {
}

void RGBA::UpdateAfterEdit_impl(){
  taOBase::UpdateAfterEdit_impl(); // skip nBase guy
  //  if(!taMisc::gui_active)    return;
  if (!name.empty()) {
    if(!(iColor::find
	 ((char*)name,r,g,b))){
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
}

void RGBA::CanCopyCustom_impl(bool to, const taBase* cp,
    bool quiet, bool& allowed, bool& forbidden) const
{
  // we handle both dirs, so don't check
  if (cp->InheritsFrom(&TA_taColor)) {
    allowed = true;
  } else 
    inherited::CanCopyCustom_impl(to, cp, quiet, allowed, forbidden);
}

void RGBA::CopyFromCustom_impl(const taBase* cp_)
{
  const taColor* cp = dynamic_cast<const taColor*>(cp_);
  if (cp) {
    r = cp->r;
    g = cp->g;
    b = cp->b;
    a = cp->a;
  } else
    inherited::CopyFromCustom_impl(cp_);
}

void RGBA::CopyToCustom_impl(taBase* to_) const
{
  taColor* to = dynamic_cast<taColor*>(to_);
  if (to) {
    to->r = r;
    to->g = g;
    to->b = b;
    to->a = a;
  } else
    inherited::CopyToCustom_impl(to_);
}


const iColor RGBA::color() const {
  return iColor(r, g, b, a);
}

String RGBA::ToString_RGBA() const {
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
  color_.setRgba(r,g,b,a);


  // compute contrast color

  // first get background color from specified background and its
  // relationship to the gui's background

  float bgc = 0.0;
  if (background !=  NULL)
    bgc = ((background->r + background->g + background->b)/3.0f) *  background->a;
  float 	dr=0.0f,dg=0.0f,db=0.0f;
#ifdef TA_GUI
  if(taMisc::gui_active)
    ((iColor)(QApplication::palette().color(QPalette::Active, QPalette::Background))).intensities(dr, dg, db);
#endif
  bgc += ((dr+dg+db)/3.0f) * ((background == NULL) ? 1.0 :
                              (1.0 - background->a));

  // now get the intensity of the foreground color

  float		nr=1.0,ng=1.0,nb=1.0;
  color_.intensities(nr,ng,nb);
  float na = color_.alphaf();
  // compute the ratio of the background color to the foreground color
  float bw = ((nr+ng+nb)/3.0f) * na + (bgc * (1.0f-na));

  if(bw >= 0.5f)        // color is mostly white
    nr = ng = nb = 0.0; // make contrast black
  else                  // color is mostly black
    nr = ng = nb = 1.0; // make contrast white
  contrastcolor_.setRgba(nr,ng,nb,1.0);

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


//////////////////////////////////
// 	ColorScaleSpec_Group	//
//////////////////////////////////

void ColorScaleSpec_Group::NewDefaults() {
  if(size != 0)
    return;

  ColorScaleSpec* cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ColdHot";
  cs->clr.Add(new RGBA(0.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ColdHotPurple";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueBlackRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.3f, 0.3f, 0.3f));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueGreyRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueWhiteRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.9f, 0.9f, 0.9f));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueGreenRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.9f, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_Rainbow";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ROYGBIV";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.5));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_DarkLight";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_LightDark";
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit();

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

int ColorScaleSpec_Group::NumListCols() const {
  return inherited::NumListCols() + CSSMG_COLS;
}

String ColorScaleSpec_Group::GetColHeading(const KeyString& key) const {
  if (key == ColorScaleSpec::key_bkclr) return String("BkClr");
  else if (key == ColorScaleSpec::key_clr0) return String("clr0");
  else if (key == ColorScaleSpec::key_clr1) return String("clr1");
  else if (key == ColorScaleSpec::key_clr2) return String("clr2");
  else if (key == ColorScaleSpec::key_clr3) return String("clr3");
  else if (key == ColorScaleSpec::key_clr4) return String("clr4");
  else return inherited::GetColHeading(key);
}

const KeyString ColorScaleSpec_Group::GetListColKey(int col) const {
  int i = col - inherited::NumListCols();
  if (i >= 0) switch (i) {
  case 0: return ColorScaleSpec::key_bkclr;
  case 1: return ColorScaleSpec::key_clr0;
  case 2: return ColorScaleSpec::key_clr1;
  case 3: return ColorScaleSpec::key_clr2;
  case 4: return ColorScaleSpec::key_clr3;
  case 5: return ColorScaleSpec::key_clr4;
  default: return _nilKeyString;
  }
  return inherited::GetListColKey(col);
}

void ColorScaleSpec_Group::SetDefaultColor() {
  NewDefaults();
  if(!taMisc::gui_active) return;
/*2004...  ivWidgetKit* wkit = ivWidgetKit::instance();
  String guiname = wkit->gui();
  if(guiname == "monochrome")
    SetDefaultEl("M_DarkLight");
  else */
    SetDefaultElName("C_ColdHot");
}

//////////////////////////
//   ScaleRange		//
//////////////////////////

void ScaleRange::SetFromScale(ColorScale& cs) {
  auto_scale = cs.auto_scale;
  min = cs.min;
  max = cs.max;
}

void ScaleRange::UpdateAfterEdit_impl() {
  taOBase::UpdateAfterEdit_impl(); // skip over taNbase to avoid c_name thing!
}

//////////////////////////
//	ColorScale	//
//////////////////////////

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

void ColorScale::SetColorSpec(ColorScaleSpec* color_spec) {
  taBase::SetPointer((TAPtr*)&spec, color_spec);
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
  if (isnan(val) || isinf(val)) {
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
    return ((TAColor *) colors[idx])->color();
  }
  if (ok) *ok = false;
  return def_color;
}

const iColor ColorScale::GetContrastColor(int idx, bool* ok) {
  if((idx >= 0) && (idx < colors.size)) {
    if (ok) *ok = true;
    return ((TAColor *) colors[idx])->contrastcolor();
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

void ColorScale::SymRange() {
  float mxabs = MAX(fabsf(min), fabsf(max));
  min = -mxabs; max = mxabs;
  FixRangeZero();
}


//////////////////////////
//	ViewColors	//
//////////////////////////

void ViewColor::InitLinks() {
  inherited::InitLinks();
  taBase::Own(fg_color, this);
  taBase::Own(bg_color, this);
}

void ViewColor::Initialize() {
  use_fg = false;
  use_bg = false;
}

void ViewColor::Destroy() {
}


bool ViewColor_List::FindMakeViewColor(const String& nm, const String& dsc,
				       bool fg, const String& fg_name,
				       bool bg, const String& bg_name) {
  ViewColor* vc = FindName(nm);
  bool rval = true;
  if(!vc) {
    vc = new ViewColor;
    vc->name = nm;		// note: this list has a hash table typicaly, so make sure name is set first before adding!
    Add(vc);
    rval = false;
  }
  vc->desc = dsc;
  vc->use_fg = fg;
  vc->use_bg = bg;
  if(fg) {
    vc->fg_color.name = fg_name;
    vc->fg_color.UpdateAfterEdit();
  }
  if(bg) {
    vc->bg_color.name = bg_name;
    vc->bg_color.UpdateAfterEdit();
  }
  return rval;
}


