// Copyright, 1995-2005, Regents of the University of Colorado,
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
#include "taglobal.h"

#ifdef TA_GUI
class QColor; // #IGNORE
#endif

#ifdef TA_USE_INVENTOR
class SoMFColor; // #IGNORE
#endif

//NOTE: floating point operations are provided to simplify the Iv->Qt port
// forwards
//class iBrush;

class TA_API iColor {
public:
  static const iColor	black_; // convenience -- can take its address to pass an iColor* for black

  static const iColor*	black() {return &black_;} // convenience -- can pass an iColor* for black

  static int		fc2ic(float value) {return (int) (255.0f * value);} // #IGNORE float color to int color
  static float		ic2fc(int value) {return ((float) value) / 255.0f ;} // #IGNORE int color to float color
  static bool  		find (const char* name, float& r, float& g, float& b); // for Iv compat

  iColor() {setRgb(0,0,0,1.0);} // black
//use implicit  iColor(const iColor& src) {c = src;} // copy constructor
  iColor(const iColor* src) {set(src);} // NULL converted to black
  iColor(int r_, int g_, int b_, float a_ = 1.0) {setRgb(r_, g_, b_, a_);}
  iColor(float r_, float g_, float b_, float a_ = 1.0) {setRgb(r_, g_, b_, a_);}
  iColor(float x) {setRgb(x, x, x, 1.0f);}
  iColor(int rgb_) {setRgb(rgb_);} // hex Internet color value, b is lowest byte
  iColor(const iColor& src, float a_) {setRgb(src.r, src.g, src.b, a_);} // for Iv compat

  void		getRgb(float& r_, float& g_, float& b_) const {
    r_ = ic2fc(r); g_ = ic2fc(g); b_ = ic2fc(b); }
  void		setRgb(int r, int g, int b, float a = 1.0);
  void		setRgb(float r_, float g_, float b_, float a_ = 1.0)
  		  {setRgb(fc2ic(r_), fc2ic(g_), fc2ic(b_), a_);}
  void		setRgb(int rgb_);
  void	 	set(const iColor* src);
  int		red() const {return r;}
  int		green() const {return g;}
  int		blue() const {return b;}
  float		redf() const {return ic2fc(r);}
  float		greenf() const {return ic2fc(g);}
  float		bluef() const {return ic2fc(b);}
  float		alpha() const {return ic2fc(a);}
  int		rgb() const { return (r << 16) | (g << 8) | b; } // can't assume byte order

  void		clear() {setRgb(0,0,0,1.0);} // black
  void		intensities(float& r, float& g, float& b); // Iv->Qt compat


//use implicit  iColor& operator =(const iColor& src);
  iColor& operator =(float x) {setRgb(x, x, x, 1.0f); return *this;}
  iColor& operator =(const iColor* cp) {set(cp); return *this;} //note: works for NULL too
//  		operator iBrush() const; //implicit conversion, similar to how Qt allows QColor for QBrush

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

protected:
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a; // for Iv/Qt porting
};


#endif // ICOLOR_H


