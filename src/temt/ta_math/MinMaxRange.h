// Copyright 2013-2017, Regents of the University of Colorado,
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

taTypeDef_Of(MinMaxRange);

class TA_API MinMaxRange : public MinMax {
  // ##UAE_IN_PROGRAM min-max values plus scale and range
INHERITED(MinMax)
public:
  float         range;          // #HIDDEN distance between min and max
  float         scale;          // #HIDDEN scale (1.0 / range)

  inline float Normalize(float val) const      { return (val - min) * scale; }
  // normalize given value to 0-1 range given current in max

  inline float Project(float val) const        { return min + (val * range); }
  // project a normalized value into the current min-max range

  inline void  UpdateRange()
  { range = Range(); if(range != 0.0f) scale = 1.0f / range; }
  // update the saved range value from current min / max

  TA_BASEFUNS_LITE(MinMaxRange);
protected:
  void  UpdateAfterEdit_impl() override
  { inherited::UpdateAfterEdit_impl(); UpdateRange(); }
private:
  void  Initialize()            { range = scale = 0.0f; }
  void  Destroy()               { };
  void  Copy_(const MinMaxRange& cp)    { range = cp.range; scale = cp.scale; }
};

#endif // MinMaxRange_h
