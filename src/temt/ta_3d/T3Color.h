// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef T3Color_h
#define T3Color_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <iColor>

#ifdef TA_QT3D

#else // TA_QT3D
#ifndef __MAKETA__
#include <Inventor/SbColor.h>
#endif

#endif // TA_QT3D

// declare all other types mentioned but not required to include:

// yet **another** color class!! but SbColor is not very convenient or intuitive, so
// we use this to facilitate api simplification

taTypeDef_Of(T3Color);

class TA_API T3Color { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS color for Coin 3d uses
public:
#ifdef __MAKETA__
  float         r;
  float         g;
  float         b;
#else
  union {
    struct {
      float     r;
      float     g;
      float     b;
    };
    float       rgb[3];
  };
#endif
  void          setValue(float r_, float g_, float b_) {r = r_; g = g_; b = b_;}
  T3Color() {r = g = b = 0.0f;}
  T3Color(float r_, float g_, float b_) {r = r_; g = g_; b = b_;}
  T3Color(float x_) {r = x_; g = x_; b = x_;}
  T3Color(const iColor* cp) {if (cp) cp->getRgb(r, g, b); else r = g = b = 0.0f;}
  T3Color(const iColor& cp) {cp.getRgb(r, g, b);}

#ifdef TA_QT3D
#else // TA_QT3D
#ifndef __MAKETA__
  T3Color(const SbColor& cp) {cp.getValue(r, g, b);}
  T3Color& operator =(const SbColor& cp) {cp.getValue(r, g, b); return *this;}
  operator SbColor() const {return SbColor(rgb);}
#endif
#endif // TA_QT3D
  T3Color& operator =(const iColor& cp) {cp.getRgb(r, g, b); return *this;}
  T3Color& operator =(float x_) {r = x_; g = x_; b = x_; return *this;}
  operator iColor() const { iColor rv; rv.setRgb(r,g,b); return rv; }
};


#endif // T3Color_h
