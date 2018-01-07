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

// this contains all full classes that appear as members of _core specs
// it must be included directly in BpUnitSpec.h, _cpp.h, _cuda.h
// the STATE_CLASS macro will define a _cpp _cuda or plain (no suffix) version

// this pragma ensures that maketa properly grabs this type information even though
// this file is included in the other files -- we get ta info for main and _cpp, not cuda

#ifdef __MAKETA__
#pragma maketa_file_is_target BpUnitSpec
#pragma maketa_file_is_target BpUnitSpec_cpp
#endif

class E_API STATE_CLASS(NLXX1ActSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra noisy linear XX1 activation function specifications
INHERITED(SpecMemberBase)
public:
  float         ramp_start;     // value of negative net input where the linear ramping function starts -- this should be a negative number, typically between -2 and -5
  float         ramp_max;       // maximum value, achieved at 0, of the linear ramp that started at ramp_start -- typically .05 to .1
  float         ramp_gain;      // #READ_ONLY = ramp_max / -ramp_start
  
  INLINE float  NLXX1Fun(float netin)
  { if(netin < ramp_start) return 0.0f;
    if(netin > 0.0f) { return ramp_max + (netin / (netin + 1.0f)); }
    return ramp_gain * (ramp_start - netin); }
  // compute the noisy-linear XX1 function

  INLINE float  NLXX1Deriv(float netin) {
    if(netin > 0.0f) {
      float denom = (1.0f + netin); return 1.0f / (denom * denom); }
    if(netin < ramp_start) return 0.0f;
    return -ramp_gain;
  }
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(NLXX1ActSpec);
  STATE_UAE(  ramp_gain = ramp_max / ramp_start; );

private:
  INLINE void        Initialize() {
    Defaults_init();
  }
  INLINE void        Defaults_init() {
    ramp_start = -3.0f;
    ramp_max = 0.1f;
    ramp_gain = ramp_max / ramp_start;
  }
};

class E_API STATE_CLASS(GaussActSpec) : public STATE_CLASS(SpecMemberBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Leabra gaussian function
INHERITED(SpecMemberBase)
public:
  float         mean;           // mean of Gaussian
  float         std_dev;        // std deviation of Gaussian
  float         std_dev_r;      // #HIDDEN reciprocal of std_dev
  
  INLINE float  GaussActFun(float netin)
  { float val = std_dev_r * (netin - mean); return expf(- (val * val)); }
  // compute the gaussian function

  INLINE float  GaussActDeriv(float act, float net)
  { return -act * 2.0f * (net - mean) * std_dev_r;  }
  
  STATE_DECO_KEY("UnitSpec");
  STATE_TA_STD_CODE_SPEC(GaussActSpec);
  STATE_UAE(    std_dev_r = 1.0f / std_dev; );
private:
  INLINE void        Initialize() {
    Defaults_init();
  }
  INLINE void        Defaults_init() {
    mean = 0.0f;
    std_dev = 1.0f;
    std_dev_r = 1.0f;
  }
};

