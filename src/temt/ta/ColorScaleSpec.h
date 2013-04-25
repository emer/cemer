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

#ifndef ColorScaleSpec_h
#define ColorScaleSpec_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <RGBA>
#include <RGBA_List>

// declare all other types mentioned but not required to include:
class ColorScaleColor_List; //

taTypeDef_Of(ColorScaleSpec);

class TA_API ColorScaleSpec : public taNBase {
  // ##CAT_Display Color Spectrum Data #SCOPE_taRootBase
INHERITED(taNBase)
public:
  RGBA          background;     // background color
  RGBA_List     clr;            // #SHOW_TREE group of colors

  virtual void  GenRanges(ColorScaleColor_List* cl, int chunks);

  static const KeyString key_bkclr; // #IGNORE
  static const KeyString key_clr0; // #IGNORE
  static const KeyString key_clr1; // #IGNORE
  static const KeyString key_clr2; // #IGNORE
  static const KeyString key_clr3; // #IGNORE
  static const KeyString key_clr4; // #IGNORE
  override String GetColText(const KeyString& key, int itm_idx) const;  // #IGNORE

  void  InitLinks();
  TA_BASEFUNS(ColorScaleSpec);
private:
  void  Copy_(const ColorScaleSpec& cp);
  void  Initialize();
  void  Destroy()               { };
};

SmartRef_Of(ColorScaleSpec); // ColorScaleSpecRef

#endif // ColorScaleSpec_h
