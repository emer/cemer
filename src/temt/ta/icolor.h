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


// icolor.h: taqt color object

#ifndef ICOLOR_H
#define ICOLOR_H

#include "ta_def.h"
#include "ta_global.h"
#include "ta_string.h"

#ifdef TA_GUI
class QColor; // 
#endif

#ifdef TA_USE_INVENTOR
class SoMFColor; // #IGNORE
#endif

class TA_API rgb_t { // very low-level class, esp provided for rgb_Matrix and raw image ops;\nwe use default copy constructor and assignment operator;\nDO NOT CHANGE BYTE ORDER -- these are compatible (on all endians) with the jpeg decode library, and enable superfast copying to our matrixes
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;
  
  int		toInt() const {return (r << 16) | (g << 8) | b;} // returns in RRGGBB web format
  void		setInt(int i); // set from RRGGBB web format
  const String	toString() const; // returns in hex RRGGBB web format
  void		setString(const String& s); // set from hex RRGGBB web format
  
  inline operator String() const {return toString();}
  
  rgb_t() {r = 0; g = 0; b = 0;}
};

class TA_API iColor { // value-type class used for specifying color values in a Qt and So-compatible way
public:
  static const iColor	black_; // convenience -- can take its address to pass an iColor* for black

  static const iColor*	black() {return &black_;} // convenience -- can pass an iColor* for black

  static int		fc2ic(float value) {return (int) (255.0f * value);} // #IGNORE float color to int color
  static float		ic2fc(int value) {return ((float) value) / 255.0f ;} // #IGNORE int color to float color
  static bool  		find (const char* name, float& r, float& g, float& b); // for Iv compat

  iColor() {setRgba(0,0,0,255);} // black
  iColor(int r_, int g_, int b_) {setRgb(r_, g_, b_);}
  iColor(float r_, float g_, float b_) {setRgb(r_, g_, b_);}
  iColor(int r_, int g_, int b_, int a_) {setRgba(r_, g_, b_, a_);}
  iColor(float r_, float g_, float b_, float a_) {setRgba(r_, g_, b_, a_);}
  explicit iColor(float x) {setRgba(x, x, x, 1.0f);}
  explicit iColor(int rgb_) {setRgb(rgb_);} // hex Internet color value, b is lowest byte
  iColor(const iColor& src, float a_ ) {setRgba(src.r, src.g, src.b, fc2ic(a_));} 
   // for Iv compat -- this insane api is used a lot in ex. colorscale.*
  iColor(const iColor& src) {c = src.c;}

  void		getRgb(float& r_, float& g_, float& b_) const {
    r_ = ic2fc(r); g_ = ic2fc(g); b_ = ic2fc(b); }
  void		setRgb(int r, int g, int b);
  void		setRgb(float r_, float g_, float b_)
  		  {setRgb(fc2ic(r_), fc2ic(g_), fc2ic(b_));}
  void		setRgb(int rgb_);
  void		setRgba(int r, int g, int b, int a);
  void		setRgba(float r_, float g_, float b_, float a_)
  		  {setRgba(fc2ic(r_), fc2ic(g_), fc2ic(b_), fc2ic(a_));}
  int		red() const {return r;}
  int		green() const {return g;}
  int		blue() const {return b;}
  float		redf() const {return ic2fc(r);}
  float		greenf() const {return ic2fc(g);}
  float		bluef() const {return ic2fc(b);}
  float		alphaf() const {return ic2fc(a);}
  int		rgb() const { return (r << 16) | (g << 8) | b; } // can't assume byte order

  void		clear() {setRgba(0,0,0,255);} // black
  void		intensities(float& r, float& g, float& b); // Iv->Qt compat


  iColor& operator =(const iColor& cp) {c = cp.c; return *this;}
  iColor& operator =(float x) {setRgba(x, x, x, 1.0f); return *this;}

#ifdef TA_GUI
  iColor(const QColor& src); // conversion constructor
  void	 	set(const QColor& src);
  void	 	set(const QColor* src);
  iColor& 	operator=(const QColor&  src); // conversion assignment operator
  		operator QColor() const;
#endif

#ifdef TA_USE_INVENTOR
  void		copyTo(SoMFColor& col) const; //
#endif

#ifndef __MAKETA__
union {
  uint32_t	c; // for uber-efficient copying
struct {
#endif // MAKETA

#if (TA_BYTE_ORDER == TA_BIG_ENDIAN)
  uint8_t	a; 
  uint8_t 	r;
  uint8_t 	g;
  uint8_t 	b;
#elif (TA_BYTE_ORDER == TA_LITTLE_ENDIAN)
  uint8_t 	b;
  uint8_t 	g;
  uint8_t 	r;
  uint8_t	a; 
#else
# error "Undefined byte order"
#endif

#ifndef __MAKETA__
};};
#endif // MAKETA
};


#endif // ICOLOR_H


