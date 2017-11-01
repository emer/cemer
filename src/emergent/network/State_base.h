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

// TA-like base classes for State implementation code
// included in appropriate specific context-setting code: State_core.h, cuda.h

#pragma maketa_file_is_target NetworkState


// these are all the base State classes

class STATE_CLASS(NetworkState); //
class STATE_CLASS(LayerState); //
class STATE_CLASS(PrjnState); //
class STATE_CLASS(UnGpState); //
class STATE_CLASS(UnitState); //
class STATE_CLASS(ConState); // 

// and corresponding spec core classes that contain all the algorithms

class STATE_CLASS(LayerSpec); //
class STATE_CLASS(ProjectionSpec); //
class STATE_CLASS(UnitSpec); //
class STATE_CLASS(ConSpec); //

class STATE_CLASS(taBase) {
  // ##NO_TOKENS ##INLINE base class for core
public:
};

class STATE_CLASS(taOBase) : public STATE_CLASS(taBase) {
  // ##NO_TOKENS ##INLINE base class with owner for core
public:
};

class STATE_CLASS(taNBase) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS ##INLINE base class with owner and name for core
public:
};

class STATE_CLASS(BaseSpec) : public STATE_CLASS(taNBase) {
  // ##NO_TOKENS ##INLINE base class for all State versions of specs
public:
};

class STATE_CLASS(SpecMemberBase) : public STATE_CLASS(taNBase) {
  // ##NO_TOKENS ##INLINE base class for all core versions of spec members
public:
};

class STATE_CLASS(taMath_float) {
public:
#ifndef __MAKETA__  
  typedef union {
    double d;
    struct {
#ifdef TA_LITTLE_ENDIAN
      int j, i;
#else
      int i, j;
#endif
    } n;
  } _eco;
#endif

  static double exp_fast(double x) {
    _eco tmp;
    tmp.n.j = 0;
    tmp.n.i = (int)((1048576/M_LN2)*x + (1072693248 - 60801));
    return tmp.d;
  }
  // #CAT_ExpLog a fast approximation to the exponential function from Nicol Schraudolph Neural Computation, 1999
  static float exp_fastf(float x) {
    return (float)exp_fast(x);
  }
  // #CAT_ExpLog a fast approximation to the exponential function from Nicol Schraudolph Neural Computation, 1999
  
  static float  euc_dist_sq(float x1, float y1, float x2, float y2)
  { return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)); }
  // #CAT_Trigonometry the squared Euclidean distance between two coordinates ((x1-x2)^2 + (y1-y2)^2)
  static float  euc_dist(float x1, float y1, float x2, float y2)
  { return sqrt(euc_dist_sq(x1, y1, x2, y2)); }
  // #CAT_Trigonometry the Euclidean distance between two coordinates ((x1-x2)^2 + (y1-y2)^2)

  static float gauss_den(float x) { return 0.398942280 * exp(-0.5 * x * x); }
  // #CAT_Probability gaussian (normal) distribution with uniform standard deviation: 1 / sqrt(2 * PI) * exp(-x^2 / 2)
  static float gauss_den_sig(float x, float sigma) {
    x /= sigma;
    return 0.398942280 * exp(-0.5 * x * x) / sigma;
  }
  // #CAT_Probability gaussian (normal) distribution with explicit sigma: 1 / (sigma * sqrt(2 * PI)) * exp(-x^2 / (2 * sigma^2))
  static float gauss_den_sq_sig(float x_sq, float sigma) {
    return 0.398942280 * exp(-0.5 * x_sq / (sigma * sigma)) / sigma;
  }
  // #CAT_Probability gaussian (normal) distribution with x already squared and explicit sigma: 1 / (sigma * sqrt(2 * PI)) * exp(-x_sq / (2 * sigma^2))
  static float gauss_den_nonorm(float x, float sigma) {
    x /= sigma; return exp(-0.5 * x * x);
  }
  // #CAT_Probability non-normalized gaussian (normal) distribution with uniform standard deviation: exp(-x^2 / (2 * sigma^2))
  static float logistic(float x, float gain=1.0, float off=0.0)
  { return 1.0 / (1.0 + exp(-gain*(x-off))); }
  // #CAT_ExpLog logistic (sigmoid) function of x: 1/(1 + e^(-gain*(x-off)))
  static float logistic_fast(float x, float gain=1.0, float off=0.0)
  { return 1.0 / (1.0 + exp_fast(-gain*(x-off))); }
  // #CAT_ExpLog logistic (sigmoid) function of x: 1/(1 + e^(-gain*(x-off))) -- using exp_fast function
  
};


// see also Network_mbrs.h for taBase objects that are used in State code --
// must put them in there so the TA* objects get defined properly.
// Network is the most widely included so best place to put them
