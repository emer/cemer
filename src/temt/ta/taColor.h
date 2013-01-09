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

#ifndef taColor_h
#define taColor_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:
class iColor; //

class TA_API taColor : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Display Red Green Blue Alpha color value
INHERITED(taBase)
public:
  bool                  no_a; // #NO_SHOW #NO_SAVE control using a, by context
  float                 r; // red
  float                 g; // green
  float                 b; // blue
  float                 a; // #CONDSHOW_ON_no_a:false alpha (intensity, ratio of fg to bg)

  const iColor          color() const;
  // return the internal value-based color object -- common currency for color reps
  void                  setColor(const iColor& cp);
  // set from the internal value-based color object -- common currency for color reps
  void                  setColorName(const String& nm);
  // set from standard X11 color names, most of which are also web/html standard color names

  void                  Set(float r_, float g_, float b_, float a_ = 1)
  { r=r_; g=g_; b=b_; a=a_; }

  TA_BASEFUNS_LITE(taColor);

  operator iColor() const {return color();}

private:
  void  Copy_(const taColor& cp) {r=cp.r; g=cp.g; b=cp.b; a=cp.a;}// not no_a
  void  Initialize() {no_a = false; r = g = b = 0; a = 1;}
  void  Destroy() {}
};



#endif // taColor_h
