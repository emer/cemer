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

#ifndef RGBA_h
#define RGBA_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class iColor; //


class TA_API RGBA : public taNBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Display Red Green Blue Alpha color specification
INHERITED(taNBase)
public:
#ifdef __MAKETA__
  String                name; // color name -- use this to lookup standard X11 color names, most of which are also web/html standard color names
#endif
  float                 r; // red
  float                 g; // green
  float                 b; // blue
  float                 a; // alpha (intensity, ratio of fg to bg)
  String                desc; // description of what this color is

  const iColor          color() const;
  // return the internal value-based color object -- common currency for color reps
  void                  setColor(const iColor& cp);
  // set from the internal value-based color object -- common currency for color reps
  void                  setColorName(const String& nm);
  // set from standard X11 color names, most of which are also web/html standard color names

  void                  Set(float r_, float g_, float b_, float a_ = 1)
     {r=r_; g=g_; b=b_; a=a_;}
  // set from rgb values

  override void         SetDefaultName() { };

  String ToString_RGBA() const;
  String        GetDesc() const                 { return desc; }
  TA_BASEFUNS_LITE(RGBA);
  RGBA(float rd, float gr, float bl, float al = 1.0); // for Iv compatibility
protected:
  override void         UpdateAfterEdit_impl(); // don't use C names
  //note: we handle both directions of copy to/from taColor
  override void         CanCopyCustom_impl(bool to, const taBase* cp,
    bool quiet, bool& allowed, bool& forbidden) const;
  override void         CopyFromCustom_impl(const taBase* cp);
  override void         CopyToCustom_impl(taBase* to) const;

private:
  void  Copy_(const RGBA& cp);
  void  Initialize();
  void  Destroy();
};


#endif // RGBA_h
