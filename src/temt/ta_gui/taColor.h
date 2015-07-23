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
#include <iColor>

// declare all other types mentioned but not required to include:

taTypeDef_Of(taColor);

class TA_API taColor : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Display Red Green Blue Alpha color value
INHERITED(taBase)
public:
  bool    no_a; // #NO_SHOW #NO_SAVE control using a, by context
  float   r; // red
  float   g; // green
  float   b; // blue
  float   a; // #CONDSHOW_ON_no_a:false alpha (intensity, ratio of fg to bg)

  const iColor          color() const;
  // #IGNORE return the internal value-based color object -- common currency for color reps
  void                  setColor(const iColor& cp);
  // #IGNORE set from the internal value-based color object -- common currency for color reps
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

taTypeDef_Of(taColorPhong);

class TA_API taColorPhong : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Display phong-type color parameters -- ambient downscaling of regular diffuse color, plus specular brightness and shininess
INHERITED(taBase)
public:
   float        ambient;        // how much to diminish the color values to create ambient color -- always based off of basic color -- a value of 0 means ambient is black
  float         specular;       // how bright is the specular (shiny reflection) component (always white)
  float         shininess;      // how shiny is the surface -- larger values create smaller specular highlight, and vice-versa -- default of 150 makes a fairly small spot

  void          GetAmbient(taColor& amb, const taColor& clr)
  { amb.Set(clr.r * ambient, clr.g * ambient, clr.b * ambient, clr.a); }
  // get the ambient color values from basic diffuse color
  void          GetSpecular(taColor& spec)
  { spec.Set(specular, specular, specular, 1.0f); }
  // get the specular color as a color
  
  TA_BASEFUNS_LITE(taColorPhong);
private:
  void  Copy_(const taColorPhong& cp)
  { ambient = cp.ambient; specular = cp.specular; shininess = cp.shininess; }
  void  Initialize() { ambient = 0.2f; specular = 0.95f; shininess = 150.0f; }
  void  Destroy() {}
};


#endif // taColor_h
