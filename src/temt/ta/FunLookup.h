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

#ifndef FunLookup_h
#define FunLookup_h 1

// parent includes:
#include <float_Array>

// member includes:
#include <MinMaxRange>

// declare all other types mentioned but not required to include:

TypeDef_Of(FunLookup);

class TA_API FunLookup : public float_Array {
  // ##CAT_Math function lookup for non-computable functions and optimization
  INHERITED(float_Array)
public:
  MinMaxRange   x_range;        // range of the x axis
  float         res;            // resolution of the function
  float         res_inv;        // #READ_ONLY #NO_SAVE 1/res: speeds computation because multiplies are faster than divides

  inline float  Yval(float x) const
    // get y value at given x value (no interpolation)
    { return SafeEl( (int) ((x - x_range.min) * res_inv)); }

  inline float  Xval(int idx)   // get x value for given index position within list
  { return x_range.min + ((float)idx * res); }

  inline float  Eval(float x)   {
    // get value via linear interpolation between points..
    int idx = (int) floor((x - x_range.min) * res_inv);
    if(idx < 0) return FastEl(0); if(idx >= size-1) return FastEl(size-1);
    float x_0 = x_range.min + (res * (float)idx);
    float y_0 = FastEl(idx);
    float y_1 = FastEl(idx+1);
    return y_0 + (y_1 - y_0) * ((x - x_0) * res_inv);
  }

  virtual void  AllocForRange(); // allocate values for given range and resolution

#ifndef __MAKETA__
  virtual void  Plot(std::ostream& strm); // #BUTTON generate file for plotting function
#else
  virtual void  Plot(ostream& strm); // #BUTTON generate file for plotting function
#endif

  virtual void  Convolve(const FunLookup& src, const FunLookup& con);
  // convolve source array with convolve array and put result here

  void  Initialize();
  void  Destroy()               { };
  void  InitLinks();
  void  Copy_(const FunLookup& cp);
  TA_BASEFUNS(FunLookup);
 protected:
  void  UpdateAfterEdit_impl();
};

#endif // FunLookup_h
