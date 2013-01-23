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

#ifndef MinMaxRange_h
#define MinMaxRange_h 1

// parent includes:
#include <MinMax>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(MinMaxRange);

class TA_API MinMaxRange : public MinMax {
  // min-max values plus scale and range #INLINE #INLINE_DUMP
  INHERITED(MinMax)
public:
  float         range;          // #HIDDEN distance between min and max
  float         scale;          // #HIDDEN scale (1.0 / range)

  float Normalize(float val) const      { return (val - min) * scale; }
  // normalize given value to 0-1 range given current in max

  float Project(float val) const        { return min + (val * range); }
  // project a normalized value into the current min-max range

  TA_BASEFUNS_LITE(MinMaxRange);
protected:
  void  UpdateAfterEdit_impl()
  { inherited::UpdateAfterEdit_impl();
    range = Range(); if(range != 0.0f) scale = 1.0f / range; }
private:
  void  Initialize()            { range = scale = 0.0f; }
  void  Destroy()               { };
  void  Copy_(const MinMaxRange& cp)    { range = cp.range; scale = cp.scale; }
};

#endif // MinMaxRange_h
