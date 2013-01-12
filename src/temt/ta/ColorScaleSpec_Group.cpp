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

#include "ColorScaleSpec_Group.h"
#include <taMisc>


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
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ColdHot_HotPurple";
  cs->clr.Add(new RGBA(0.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ColdHot_ColdPurple";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueBlackRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.3f, 0.3f, 0.3f));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueGreyRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 0.5, 0.5));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueWhiteRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.9f, 0.9f, 0.9f));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_BlueGreenRed";
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.9f, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_Rainbow";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_ROYGBIV";
  cs->clr.Add(new RGBA(1.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.5));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_Jet";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.5));
  cs->clr.Add(new RGBA(0.0, 0.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.5, 1.0));
  cs->clr.Add(new RGBA(0.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.5, 1.0, 0.5));
  cs->clr.Add(new RGBA(1.0, 1.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.5, 0.0));
  cs->clr.Add(new RGBA(1.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(0.5, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_DarkLight";
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

  cs = (ColorScaleSpec*)NewEl(1, &TA_ColorScaleSpec);
  cs->name = "C_LightDark";
  cs->clr.Add(new RGBA(1.0, 1.0, 1.0));
  cs->clr.Add(new RGBA(0.0, 0.0, 0.0));
  cs->background.name = "grey80";
  cs->background.UpdateAfterEdit_NoGui();

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
