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

#ifndef FixedMinMax_h
#define FixedMinMax_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class MinMax;

TypeDef_Of(FixedMinMax);

class TA_API FixedMinMax : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math minimum-maximum values with toggles for usage
INHERITED(taBase)
public:
  bool          fix_min;        // use fixed minimum value?
  float         min;            // minimum value
  bool          fix_max;        // use fixed maximum value?
  float         max;            // maximum value

  void  Init(float it)  { min = max = it; }  // initializes the max and min to this value
  void  InitFix(bool fx = false) { fix_min = fix_max = fx; }
  void  Set(float mn, float mx) { min = mn; max = mx; }
  void  SetMin(float mn) { min = mn; fix_min = true; }
  void  SetMax(float mx) { max = mx; fix_max = true; }
  void  FixRange(MinMax& mm);

  TA_BASEFUNS_LITE(FixedMinMax);
private:
  void  Initialize();
  void  Destroy()               { };
  void  Copy_(const FixedMinMax& cp);
};

#endif // FixedMinMax_h
