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

#ifndef ColorScale_h
#define ColorScale_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:
#include <ColorScaleColor_List>

// declare all other types mentioned but not required to include:
class iColor; //
class ColorScaleSpec; //

taTypeDef_Of(ColorScale);

class TA_API ColorScale : public taNBase {
  // ##NO_TOKENS ##CAT_Display defines a range of colors to code data values with
INHERITED(taNBase)
public:
  static const iColor   def_color;      // #NO_COPY #NO_SAVE ignore me

  int                   chunks;         // number of chunks to divide scale into
  float                 min;
  float                 max;
  float                 last_min;
  float                 last_max;
  float                 range;
  float                 zero;
  ColorScaleSpec*       spec;           // specifies the color ranges
  bool                  auto_scale;     // #DEF_true

  ColorScaleColor_List  colors;         // #IGNORE the actual colors
  iColor                background;     // #IGNORE background color

  ColorScaleColor       maxout;         // #IGNORE
  ColorScaleColor       minout;         // #IGNORE
  ColorScaleColor       nocolor;        // #IGNORE

  virtual void          SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON #INIT_ARGVAL_ON_spec set the color scale spec to determine the palette of colors representing values

  virtual const iColor  Get_Background(); // #IGNORE
  float                 GetAbsPercent(float val);
  virtual const iColor  GetColor(int idx, bool* ok = NULL);
  virtual const iColor  GetColor(float val, float& sc_val, iColor* maincolor=NULL,
                                 iColor* contrast=NULL);
  // #IGNORE
  virtual void          GetColorRGB(float val, RGBA& rgba);  // use this method in emergent projects for coloring data values
  virtual const iColor  GetContrastColor(int idx, bool* ok = NULL); // #IGNORE
  int                   GetIdx(float val);
  void                  DefaultChunks();

  virtual void          MapColors();    // generates the colors from spec
  virtual void          NewDefaults(); //

  // funcs used to be in bar:
  virtual void          ModRange(float val);
  virtual void          ModRoundRange(float val);
  virtual void          FixRangeZero();
  virtual void          SetMinMax(float mn,float mx);
  virtual void          UpdateMinMax(float mn, float mx); // maybe expand bounds
  virtual bool          UpdateMinMax(float val); // maybe expand bounds, returning true if expanded
  virtual void          RestoreMinMax(); // restore min max to pre auto_scale values
  virtual void          SymRange();              // symmetrize min/max values around zero

  virtual void          SetScaleData(bool auto_scale_, float min_, float max_);

  void  InitLinks() override;
  void  CutLinks() override;
  void  UpdateAfterEdit() override;
  TA_BASEFUNS(ColorScale);
  ColorScale(int chunk);
private:
  SIMPLE_COPY(ColorScale) //note: added 4/12/07 for consistency, but may not be good
  void  Initialize();
  void  Destroy();
};

TA_SMART_PTRS(TA_API, ColorScale); // ColorScaleRef

#endif // ColorScale_h
