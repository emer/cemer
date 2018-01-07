// Copyright 2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// this must be included in a defined State_cpp/cuda/main context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#ifdef __MAKETA__
#pragma maketa_file_is_target ConSpec
#endif

class STATE_CLASS(WeightLimits) : public STATE_CLASS(SpecMemberBase) {
  // ##NO_TOKENS ##INLINE #NO_UPDATE_AFTER ##CAT_Network specifies weight limits for connections
INHERITED(SpecMemberBase)
public:
  enum LimitType {
    NONE,                       // no weight limitations
    GT_MIN,                     // constrain weights to be greater than min value
    LT_MAX,                     // constrain weights to be less than max value
    MIN_MAX                     // constrain weights to be within min and max values
  };
  LimitType     type;           // type of weight limitation to impose
  float         min;            // #CONDSHOW_OFF_type:NONE,LT_MAX minimum weight value (if applicable)
  float         max;            // #CONDSHOW_OFF_type:NONE,GT_MIN maximum weight value (if applicable)
  bool          sym;            // if true, also symmetrize weights with those in reciprocal connections, during weight initialization process -- this is automatically turned off if the random variance (rnd.var) in the weights is set to 0 (e.g., for fixed weight patterns)
  bool          sym_fm_top;     // #CONDSHOW_ON_sym if symmetrizing, and this is true, then top-down weights end up driving the symmetrized weights -- otherwise the bottom-up end of driving (default)

  INLINE void  ApplyMinLimit(float& wt)        { if(wt < min) wt = min; }
  INLINE void  ApplyMaxLimit(float& wt)        { if(wt > max) wt = max; }

  INLINE void  ApplyLimits(float& wt) {
    if(type == MIN_MAX)    { ApplyMinLimit(wt); ApplyMaxLimit(wt); }
    else if(type == GT_MIN)     ApplyMinLimit(wt);
    else if(type == LT_MAX)     ApplyMaxLimit(wt);
  }

  STATE_DECO_KEY("ConSpec");
  STATE_TA_STD_CODE_SPEC(WeightLimits);
private:
  void  Initialize() {
    type = NONE;
    min = -1.0f;
    max = 1.0f;
    sym = false;
    sym_fm_top = false;
  }
  void Defaults_init() { }
};
