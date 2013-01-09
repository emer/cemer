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

#ifndef ColorScaleColor_h
#define ColorScaleColor_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:
class iColor; //
class RGBA; //

class TA_API ColorScaleColor : public taBase { // ##NO_TOKENS Color
INHERITED(taBase)
public:
  const iColor  color() {return color_;}                // #IGNORE
  const iColor  contrastcolor() {return contrastcolor_;}        // #IGNORE
  void SetColor(const iColor& c, RGBA* background = NULL){
    SetColor(c.redf(), c.greenf(), c.bluef(), c.alphaf(), background);
  }
  void SetColor(RGBA* c, RGBA* background = NULL) {
    SetColor(c->r, c->g, c->b, c->a, background);
  }
  void SetColor(float r,float g, float b, float a=1.0,  RGBA* background=NULL);
  // #USE_RVAL #ARGC=4
  TA_BASEFUNS_LITE(ColorScaleColor);
protected:
  iColor        color_;         // #IGNORE
  iColor        contrastcolor_; // #IGNORE
private:
  void  Copy_(const ColorScaleColor& cp)
  { color_ = cp.color_; contrastcolor_ = cp.contrastcolor_;}
  void Initialize()     {}
  void Destroy() {}
};


#endif // ColorScaleColor_h
