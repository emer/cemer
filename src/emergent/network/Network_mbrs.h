// Copyright 2017-2018, Regents of the University of Colorado,
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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#ifdef __MAKETA__
#pragma maketa_file_is_target Network
#pragma maketa_file_is_target NetworkState
#endif


class STATE_CLASS(NetStatsSpecs) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Network parameters for how stats are computed
INHERITED(taOBase)
public:
  bool          sse_unit_avg;   // #CAT_Statistic compute sse as average sse over units (i.e., divide by total number of target units in layer)
  bool          sse_sqrt;       // #CAT_Statistic take the square root of the SSE, producing a Euclidian distance instead of raw sse (note this cannot technically be added across trials in a linear fashion, as raw sse can)
  float         cnt_err_tol;    // #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  bool          prerr;          // #CAT_Statistic compute precision and recall error values over units, at same time as computing sum squared error (SSE)
  bool          cos_err;          // #CAT_Statistic compute cosine error values over units, at same time as computing sum squared error (SSE)

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(NetStatsSpecs);
private:
  INLINE void   Initialize() {
    sse_unit_avg = false;
    sse_sqrt = false;
    cnt_err_tol = 0.0f;
    prerr = false;
    cos_err = true;
  }
};

class STATE_CLASS(NetworkCudaSpec) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Network parameters for NVIDA CUDA GPU implementation -- only applicable for CUDA_COMPILE binaries
  INHERITED(taOBase)
public:
  bool          on;             // #NO_SAVE is CUDA running?  this is true by default when running in an executable compiled with CUDA_COMPILE defined, and false otherwise -- can be turned off to fall back on regular C++ code, but cannot be turned on if CUDA has not been compiled
  bool          sync_units;     // #CONDSHOW_ON_on for debugging or visualization purposes -- keep the C++ host state Unit variables synchronized with the GPU device state (which is slow!) -- otherwise the GPU just runs, and updates a few key statistics (fastest)
  bool          sync_cons;      // #CONDSHOW_ON_on for debugging or visualization purposes -- keep the C++ host state Connection variables synchronized with the GPU device state (which is VERY slow!) -- otherwise the GPU just runs, and updates a few key statistics (fastest)
  int           min_threads;    // #CONDSHOW_ON_on #DEF_32 #MIN_32 minuimum number of CUDA threads to allocate per block -- each block works on all the cons in a single ConState, and the threads divide up the connections in turn -- must be a multiple of 32 (i.e., min of 32) -- actual size will be determined at run-time as function of max number of connections per connection group / cons_per_thread -- to specify an exact number of threads, just set min and max_threads to the same number
  int           max_threads;    // #CONDSHOW_ON_on #DEF_1024 #MAX_1024 maximum number of CUDA threads to allocate per block (sending group of connections) -- actual size will be determined at run-time as function of max number of connections per connection group / cons_per_thread -- for modern cards (compute capability 2.0 or higher) the max is 1024, but in general you might need to experiment to find the best performing number for your card and network, and interaction with cons_per_thread -- to specify an exact number of threads, just set min and max_threads to the same number
  int           cons_per_thread; // #CONDSHOW_ON_on #DEF_1:8 when computing number of threads to use, divide max number of connections per unit by this number, and then round to nearest multiple of 32, subject to the min and max_threads constraints
  bool          timers_on;      // #CONDSHOW_ON_on Accumulate timing information for each step of processing -- for debugging / optimizing threading
  int           n_threads;      // #READ_ONLY #SHOW computed number of threads it is actually using

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(NetworkCudaSpec);
#ifndef CUDA_COMPILE
  STATE_UAE(
    on = false;                   // can never be on!
            );
#endif
  
private:
  INLINE void  Initialize() {
#ifdef CUDA_COMPILE
    on = true;
#else
    on = false;
#endif
    sync_units = false;
    sync_cons = false;
    min_threads = 32;
    max_threads = 1024;
    cons_per_thread = 2;
    timers_on = false;
    n_threads = min_threads;
  }
};

// NOTE: this also contains any other misc State classes that could be widely used --
// everything includes Network so this is the widest scope, and will be compiled into proper
// TA classes..

#ifndef STATE_MAIN

#include <MTRnd>

#ifdef TAVECTOR2I
#undef TAVECTOR2I
#endif
#define TAVECTOR2I      STATE_CLASS(taVector2i)

#ifdef TAVECTOR2F
#undef TAVECTOR2F
#endif
#define TAVECTOR2F      STATE_CLASS(taVector2f)

class TAVECTOR2F;

class TAVECTOR2I : public STATE_CLASS(taBase) {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math an integer value in 2D coordinate space
INHERITED(taBase)
public:
  int           x;              // horizontal
  int           y;              // vertical

  INLINE bool   isZero() { return ((x == 0) && (y == 0)); }
  INLINE void   SetXY(int xx, int yy) { x = xx; y = yy;  }
  INLINE void   SetXY(float xx, float yy) { x = (int)xx; y = (int)yy; }
  INLINE void   GetXY(float& xx, float& yy) { xx = (float)x; yy = (float)y; }

  TAVECTOR2I()                           { SetXY(0, 0); }
  TAVECTOR2I(int xx)                     { SetXY(xx, xx); }
  TAVECTOR2I(int xx, int yy)             { SetXY(xx, yy); }
  TAVECTOR2I(float xx, float yy)         { SetXY(xx, yy); }
  INLINE TAVECTOR2I(const TAVECTOR2F& cp);

  INLINE TAVECTOR2I& operator=(const TAVECTOR2F& cp);

  INLINE TAVECTOR2I& operator=(int cp)           { x = cp; y = cp; return *this;}
  INLINE TAVECTOR2I& operator=(float cp)         { x = (int)cp; y = (int)cp; return *this;}
  INLINE TAVECTOR2I& operator=(double cp)        { x = (int)cp; y = (int)cp; return *this;}

  INLINE TAVECTOR2I&     operator += (const TAVECTOR2I& td)   { x += td.x; y += td.y; return *this;}
  INLINE TAVECTOR2I&     operator -= (const TAVECTOR2I& td)   { x -= td.x; y -= td.y; return *this;}
  INLINE TAVECTOR2I&     operator *= (const TAVECTOR2I& td)   { x *= td.x; y *= td.y; return *this;}
  INLINE TAVECTOR2I&     operator /= (const TAVECTOR2I& td)   { x /= td.x; y /= td.y; return *this;}
  INLINE TAVECTOR2I&     operator %= (const TAVECTOR2I& td)   { x %= td.x; y %= td.y; return *this;}

  INLINE TAVECTOR2I&     operator += (int td)    { x += td; y += td; return *this;}
  INLINE TAVECTOR2I&     operator -= (int td)    { x -= td; y -= td; return *this;}
  INLINE TAVECTOR2I&     operator *= (int td)    { x *= td; y *= td; return *this;}
  INLINE TAVECTOR2I&     operator /= (int td)    { x /= td; y /= td; return *this;}
  INLINE TAVECTOR2I&     operator %= (int td)    { x %= td; y %= td; return *this;}

  INLINE TAVECTOR2I operator + (const TAVECTOR2I& td) const {
    TAVECTOR2I rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  INLINE TAVECTOR2I operator - (const TAVECTOR2I& td) const {
    TAVECTOR2I rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  INLINE TAVECTOR2I operator * (const TAVECTOR2I& td) const {
    TAVECTOR2I rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  INLINE TAVECTOR2I operator / (const TAVECTOR2I& td) const {
    TAVECTOR2I rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  INLINE TAVECTOR2I operator + (int td) const {
    TAVECTOR2I rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  INLINE TAVECTOR2I operator - (int td) const {
    TAVECTOR2I rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  INLINE TAVECTOR2I operator * (int td) const {
    TAVECTOR2I rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  INLINE TAVECTOR2I operator / (int td) const {
    TAVECTOR2I rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  INLINE TAVECTOR2I operator - () const {
    TAVECTOR2I rv; rv.x = -x; rv.y = -y; return rv;
  }
  INLINE TAVECTOR2I operator -- () const {
    TAVECTOR2I rv = *this; rv.x--; rv.y--; return rv;
  }
  INLINE TAVECTOR2I operator ++ () const {
    TAVECTOR2I rv = *this; rv.x++; rv.y++; return rv;
  }

  INLINE bool operator < (const TAVECTOR2I& td) const { return (x < td.x) && (y < td.y); }
  INLINE bool operator > (const TAVECTOR2I& td) const { return (x > td.x) && (y > td.y); }
  INLINE bool operator <= (const TAVECTOR2I& td) const { return (x <= td.x) && (y <= td.y); }
  INLINE bool operator >= (const TAVECTOR2I& td) const { return (x >= td.x) && (y >= td.y); }
  INLINE bool operator == (const TAVECTOR2I& td) const { return (x == td.x) && (y == td.y); }
  INLINE bool operator != (const TAVECTOR2I& td) const { return (x != td.x) || (y != td.y); }

  INLINE bool operator <  (int td) const { return (x < td) && (y < td); }
  INLINE bool operator >  (int td) const { return (x > td) && (y > td); }
  INLINE bool operator <= (int td) const { return (x <= td) && (y <= td); }
  INLINE bool operator >= (int td) const { return (x >= td) && (y >= td); }
  INLINE bool operator == (int td) const { return (x == td) && (y == td); }
  INLINE bool operator != (int td) const { return (x != td) || (y != td); }

  INLINE bool OrEq(int td) const { return (x == td) || (y == td); }
  INLINE bool OrEq(const TAVECTOR2I& td) const { return (x == td.x) || (y == td.y); }

  INLINE int SqMag() const { return x * x + y * y; }
  // squared magnitude of vector
  INLINE float Mag() const { return sqrtf((float)SqMag()); }

  INLINE float  SqDist(const TAVECTOR2I& td) const { // squared distance between two vectors
    TAVECTOR2I dist = *this - td; return dist.SqMag();
  }
  INLINE float  Dist(const TAVECTOR2I& td) const { return sqrtf(SqDist(td)); }
  INLINE int    Sum() const     { return x + y; }
  INLINE int    Product() const { return x * y; }

  static INLINE int Sgn(int val) { return (val >= 0) ? 1 : -1; }
  static INLINE int Absv(int val) { return (val >= 0) ? val : -val; }

  INLINE void   Invert()        { x = -x; y = -y; }
  INLINE void   SumNorm()       { int mg = Sum(); if(mg != 0.0) *this /= mg; }
  INLINE void   Abs()           { x = Absv(x); y = Absv(y); }
  INLINE void   Min(TAVECTOR2I& td) { x = MIN(x,td.x); y = MIN(y,td.y); }
  INLINE void   Max(TAVECTOR2I& td) { x = MAX(x,td.x); y = MAX(y,td.y); }

  INLINE int MaxVal() const     { int mx = MAX(x, y); return mx; }
  INLINE int MinVal() const     { int mn = MIN(x, y); return mn; }

  INLINE bool  FitN(int n) {            // adjust x and y to fit x total elements
    if((x * y) == n)    return false;
    y = (int)sqrtf((float)n);
    if(y < 1)
      y = 1;
    x = n / y;
    if(x*y == n) return true; // got lucky

    // next try a range of y's to fit evenly
    int sqrty = y;
    int lwy = y/2;  int hiy = y*2;
    if(lwy == 0) lwy = 1;
    for(y = lwy; y<=hiy; y++) {
      x = n / y;
      if(x*y == n) return true; // got lucky
    }
    if(n < 20) {
      x = n;    y = 1;          // just go linear for small values
      return true;
    }
    // else just go with an imperfect fit
    y = sqrty;
    x = n / y;
    while((x * y) < n)
      x++;
    return true;
  }

  INLINE void   SetGtEq(int n)  { x = MAX(n, x);  y = MAX(n, y); }
  // set each to be greater than or equal to n
  INLINE void   SetLtEq(int n)  { x = MIN(n, x);  y = MIN(n, y); }
  // set each to be less than or equal to n

  static INLINE int    WrapMax(int c, int max) {
    int rval = c % max; if(rval < 0) rval += max;  return rval;
  }
  static INLINE void WrapMinDistOne(int& pos, int& dst, const int pos_max, const int cmp,
                               const int cmp_half) {
    if(cmp < cmp_half) {
      if(Absv((pos-pos_max) - cmp) < Absv(dst)) { pos -= pos_max; dst = pos - cmp; }
    }
    else {
      if(Absv((pos+pos_max) - cmp) < Absv(dst)) { pos += pos_max; dst = pos - cmp; }
    }
  }
  // in computing the distance between two coord vals: dst = pos-cmp, consider whether the distance is shorter if pos is wrapped around as a function of pos_max size (condition on which side of the half-way point of the range for cmp value, cmp_half, for which way to wrap) -- if it is shorter, then update pos to new extended value (beyond normal range either - or +) and also update the distance value

  INLINE void   WrapMinDist(TAVECTOR2I& dst, const TAVECTOR2I& max, const TAVECTOR2I& cmp,
                            const TAVECTOR2I& cmp_half) {
    WrapMinDistOne(x, dst.x, max.x, cmp.x, cmp_half.x);
    WrapMinDistOne(y, dst.y, max.y, cmp.y, cmp_half.y);
  }
  // in computing the distance between two coords: dst = this-cmp, consider whether the distance is shorter if this is wrapped around as a function of pos_max size (condition on which side of the half-way point of the range for cmp value, cmp_half, for which way to wrap) -- if it is shorter, then update this pos to new extended value (beyond normal range either - or +) and also update the distance value

  static INLINE void    WrapOne(int& c, int max)
  { if(c >= 0 && c < max) return; if(c < 0) c = max + (c % max); else c = c % max; }
  // wrap-around one dimension
  INLINE void   Wrap(const TAVECTOR2I& max)
  { WrapOne(x, max.x); WrapOne(y, max.y); }
  // wrap-around coordinates within 0,0 - max range

  static INLINE bool   WrapOneHalf(int& c, int max)
  { if(c >= 0 && c < max) return false;  bool out_of_range = false;
    if(c < 0) { if(c < -max/2) out_of_range = true; c = max + (c % max); }
    else      { if(c > max + max/2) out_of_range = true; c = c % max; }
    return out_of_range; }
  // wrap-around one dimension, return true if out of range (more than half way around other side)
  INLINE bool   WrapHalf(const TAVECTOR2I& max)
  { bool wcx = WrapOneHalf(x, max.x); bool wcy = WrapOneHalf(y, max.y);
    return wcx || wcy; }
  // wrap-around coordinates within 0,0 - max range, return true if out of range (more than half way around other side)

  static INLINE bool   ClipOne(int& c, int max)
  { if(c >= 0 && c < max) return false; if(c < 0) c = 0; else c = max-1; return true; }
  // clip one dimension, true if out of range
  INLINE bool    Clip(const TAVECTOR2I& max)
  { bool wcx = ClipOne(x, max.x); bool wcy = ClipOne(y, max.y);
    return wcx || wcy; }
  // clip coordinates within 0,0 - max range, true if out of range

  INLINE void   SetFmIndex(int idx, int x_size) {
    x = idx % x_size;
    y = idx / x_size;
  }
  // set x, y values from a "cell" index in a 2d matrix-like space organized with x as the inner loop and y as the outer loop, with a given x dimension size

  INLINE bool   WrapClip(bool wrap, const TAVECTOR2I& max)
  { if(wrap) { Wrap(max); return false; } return Clip(max); }
  // wrap-around or clip coordinates within 0,0 - max range, true if clipped out of range -- for performance, it is better to use separate code for wrap and clip cases
};

INLINE TAVECTOR2I operator + (int td, const TAVECTOR2I& v) {
  TAVECTOR2I rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
}
INLINE TAVECTOR2I operator - (int td, const TAVECTOR2I& v) {
  TAVECTOR2I rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
}
INLINE TAVECTOR2I operator * (int td, const TAVECTOR2I& v) {
  TAVECTOR2I rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
}
INLINE TAVECTOR2I operator / (int td, const TAVECTOR2I& v) {
  TAVECTOR2I rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
}


class TAVECTOR2F : public STATE_CLASS(taBase) {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math a value in 2D coordinate space
INHERITED(taBase)
public:
  float         x;              // horizontal
  float         y;              // vertical

  INLINE void   SetXY(float xx, float yy)       { x = xx; y = yy; }
  INLINE void   GetXY(float& xx, float& yy)     { xx = x; yy = y; }

  TAVECTOR2F()                     { SetXY(0, 0); }
  TAVECTOR2F(float xx)             { SetXY(xx, xx); }
  TAVECTOR2F(float xx, float yy)   { SetXY(xx, yy); }
  TAVECTOR2F(int xx)               { SetXY(xx, xx); }
  TAVECTOR2F(int xx, int yy)       { SetXY(xx, yy); }
  TAVECTOR2F(const TAVECTOR2I& cp) { SetXY(cp.x, cp.y); }

  INLINE TAVECTOR2F& operator=(const TAVECTOR2I& cp) {
    x = (float)cp.x; y = (float)cp.y;
    return *this;
  }
  INLINE TAVECTOR2F& operator=(float cp)            { x = cp; y = cp; return *this;}
  INLINE TAVECTOR2F& operator=(double cp)           { x = (float)cp; y = (float)cp; return *this;}

  INLINE TAVECTOR2F& operator += (const TAVECTOR2F& td) { x += td.x; y += td.y; return *this;}
  INLINE TAVECTOR2F& operator -= (const TAVECTOR2F& td) { x -= td.x; y -= td.y; return *this;}
  INLINE TAVECTOR2F& operator *= (const TAVECTOR2F& td) { x *= td.x; y *= td.y; return *this;}
  INLINE TAVECTOR2F& operator /= (const TAVECTOR2F& td) { x /= td.x; y /= td.y; return *this;}

  INLINE TAVECTOR2F& operator += (float td) { x += td; y += td; return *this;}
  INLINE TAVECTOR2F& operator -= (float td) { x -= td; y -= td; return *this;}
  INLINE TAVECTOR2F& operator *= (float td) { x *= td; y *= td; return *this;}
  INLINE TAVECTOR2F& operator /= (float td) { x /= td; y /= td; return *this;}

  INLINE TAVECTOR2F operator + (const TAVECTOR2F& td) const {
    TAVECTOR2F rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  INLINE TAVECTOR2F operator - (const TAVECTOR2F& td) const {
    TAVECTOR2F rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  INLINE TAVECTOR2F operator * (const TAVECTOR2F& td) const {
    TAVECTOR2F rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  INLINE TAVECTOR2F operator / (const TAVECTOR2F& td) const {
    TAVECTOR2F rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  INLINE TAVECTOR2F operator + (float td) const {
    TAVECTOR2F rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  INLINE TAVECTOR2F operator - (float td) const {
    TAVECTOR2F rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  INLINE TAVECTOR2F operator * (float td) const {
    TAVECTOR2F rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  INLINE TAVECTOR2F operator / (float td) const {
    TAVECTOR2F rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  INLINE TAVECTOR2F operator - () const {
    TAVECTOR2F rv; rv.x = -x; rv.y = -y; return rv;
  }

  INLINE bool operator < (const TAVECTOR2F& td) const { return (x < td.x) && (y < td.y); }
  INLINE bool operator > (const TAVECTOR2F& td) const { return (x > td.x) && (y > td.y); }
  INLINE bool operator <= (const TAVECTOR2F& td) const { return (x <= td.x) && (y <= td.y); }
  INLINE bool operator >= (const TAVECTOR2F& td) const { return (x >= td.x) && (y >= td.y); }
  INLINE bool operator == (const TAVECTOR2F& td) const { return (x == td.x) && (y == td.y); }
  INLINE bool operator != (const TAVECTOR2F& td) const { return (x != td.x) || (y != td.y); }

  INLINE bool operator <  (float td) const { return (x < td) && (y < td); }
  INLINE bool operator >  (float td) const { return (x > td) && (y > td); }
  INLINE bool operator <= (float td) const { return (x <= td) && (y <= td); }
  INLINE bool operator >= (float td) const { return (x >= td) && (y >= td); }
  INLINE bool operator == (float td) const { return (x == td) && (y == td); }
  INLINE bool operator != (float td) const { return (x != td) || (y != td); }

  INLINE float SqMag() const    { return x * x + y * y; }
  // squared magnitude of vector
  INLINE float Mag() const      { return sqrtf(SqMag()); }

  INLINE float  SqDist(const TAVECTOR2F& td) const { // squared distance between two vectors
    TAVECTOR2F dist = *this - td; return dist.Mag();
  }
  INLINE float  Dist(const TAVECTOR2F& td) const { return sqrtf(SqDist(td)); }
  INLINE float  Sum() const     { return x + y; }
  INLINE float  Product() const { return x * y; }

  INLINE void   Invert()        { x = -x; y = -y; }
  INLINE void   MagNorm()       { float mg = Mag(); if(mg > 0.0) *this /= mg; }
  INLINE void   SumNorm()       { float mg = Sum(); if(mg != 0.0) *this /= mg; }
  INLINE void   Abs()           { x = fabs(x); y = fabs(y); }

  INLINE float MaxVal() const   { float mx = fmaxf(x, y); return mx; }
  INLINE float MinVal() const   { float mn = fminf(x, y); return mn; }
};

 
TAVECTOR2I::TAVECTOR2I(const TAVECTOR2F& cp) { SetXY(cp.x, cp.y); }

TAVECTOR2I& TAVECTOR2I::operator=(const TAVECTOR2F& cp) {
  x = (int)cp.x; y = (int)cp.y;
  return *this;
}

INLINE TAVECTOR2F operator + (float td, const TAVECTOR2F& v) {
  TAVECTOR2F rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
}
INLINE TAVECTOR2F operator - (float td, const TAVECTOR2F& v) {
  TAVECTOR2F rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
}
INLINE TAVECTOR2F operator * (float td, const TAVECTOR2F& v) {
  TAVECTOR2F rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
}
INLINE TAVECTOR2F operator / (float td, const TAVECTOR2F& v) {
  TAVECTOR2F rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
}


class STATE_CLASS(taMath_double) {
  // double-precision floating point math
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

  static double pi;
  // #NO_SAVE #READ_ONLY the value of pi

#ifdef TA_OS_WIN
  static double exp_fast(double x) { return exp(x); }
#else  
  static double exp_fast(double x) {
    _eco tmp;
    tmp.n.j = 0;
    tmp.n.i = (int)((1048576/M_LN2)*x + (1072693248 - 60801));
    return tmp.d;
  }
#endif  
  // #CAT_ExpLog a fast approximation to the exponential function from Nicol Schraudolph Neural Computation, 1999

  static double fact_ln(int n);
  // #CAT_Probability natural log (ln) of n factorial (n!)
  static double bico_ln(int n, int j) {  return fact_ln(n)-fact_ln(j)-fact_ln(n-j); }
  // #CAT_Probability natural log (ln) of n choose j (binomial)
  static double gamma_ln(double z);
  // #CAT_Probability natural log (ln) of gamma function (not gamma distribution): generalization of (n-1)! to real values
  static double beta_i(double a, double b, double x);
  // #CAT_Probability incomplete beta function
  
  static double beta_den(double x, double a, double b);
  // #CAT_Probability beta probability density function evaluated at 0 < x < 1 for shape parameters a, b
  static double beta_dev(double a, double b, int thr_no = -1) {
    double x1 = gamma_dev(a, 1.0, thr_no);
    double x2 = gamma_dev(b, 1.0, thr_no);
    return x1 / (x1 + x2);
  }
  // #CAT_Probability return a beta distribution deviate, characterized by parameters a > 0, b > 0 -- uses gamma_dev -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double binom_den(int n, int j, double p) {
    if(j > n) { return 0; }
    return exp(bico_ln(n,j) + (double)j * log(p) + (double)(n-j) * log(1.0 - p));
  }
  // #CAT_Probability binomial probability function
  static double binom_dev(int n, double p, int thr_no = -1);
  // #CAT_Probability binomial deviate: p prob with n trials -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double poisson_den(int j, double l) {
    return exp((double)j * log(l) - fact_ln(j) - l);
  }
  // #CAT_Probability poisson distribution
  static double poisson_dev(double l, int thr_no = -1);
  // #CAT_Probability poisson deviate:  mean is l -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double gamma_den(int j, double l, double t) {
    if(t < 0) return 0;
    return exp((double)j * log(l) + (double)(j-1) * log(t) - gamma_ln(j) - (l * t));
  }
  // #CAT_Probability gamma probability distribution: j events, l lambda, t time
  static double gamma_dev(double k, double lambda = 1.0, int thr_no = -1);
  // #CAT_Probability gamma deviate: how long to wait until k events with given lambda variance -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double gauss_den(double x) {
    return 0.398942280 * exp(-0.5 * x * x);
  }
  // #CAT_Probability gaussian (normal) distribution with uniform standard deviation: 1 / sqrt(2 * PI) * exp(-x^2 / 2)
  static float gauss_den_sig(float x, float sigma) {
    x /= sigma;  return 0.398942280 * exp(-0.5 * x * x) / sigma;
  }
  // #CAT_Probability gaussian (normal) distribution with explicit sigma: 1 / (sigma * sqrt(2 * PI)) * exp(-x^2 / (2 * sigma^2))
  static double gauss_dev(int thr_no = -1) {
    return MTRnd::GenRandGaussDev(thr_no);
  }
  // #CAT_Probability gaussian deviate: normally distributed -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  
};



class STATE_CLASS(taMath_float) {
  // single-precision floating point math
public:

  static float exp_fast(float x) {
    return (float)STATE_CLASS(taMath_double)::exp_fast(x);
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




class STATE_CLASS(Random) : public STATE_CLASS(taNBase) {
  // #STEM_BASE #NO_UPDATE_AFTER ##INLINE ##CAT_Math ##NO_TOKENS ##STATIC_COMPLETION Random Number Generation
INHERITED(taNBase)
public:
  enum Type {
    UNIFORM,                    // uniform with var = range on either side of the mean
    BINOMIAL,                   // binomial with var = p, par = n
    POISSON,                    // poisson with lambda = var
    GAMMA,                      // gamma with var scaling parameter and par = k stages
    GAUSSIAN,                   // normal with var
    BETA,                       // beta with var = a and par = b shape parameters
    NONE,                       // just the mean
  };

  Type          type;           // type of random variable to generate
  double        mean;           // mean of random distribution
  double        var;            // #CONDSHOW_OFF_type:NONE 'varibility' parameter for the random numbers (gauss = standard deviation, not variance; uniform = half-range)
  double        par;            // #CONDSHOW_ON_type:GAMMA,BINOMIAL,BETA extra parameter for distribution (depends on each one)

  INLINE double Gen(int thr_no = -1) const {
    if(var == 0.0f) return mean;
    switch(type) {
    case NONE:
      return mean;
    case UNIFORM:
      return UniformMeanRange(mean, var, thr_no);
    case BINOMIAL:
      return mean + Binom((int)par, var, thr_no);
    case POISSON:
      return mean + Poisson(var, thr_no);
    case GAMMA:
      return mean + Gamma(var, (int)par, thr_no);
    case GAUSSIAN:
      return mean + Gauss(var, thr_no);
    case BETA:
      return mean + Beta(var, par, thr_no);
    }
    return 0.0f;
  }
  // generate a random variable according to current parameters -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  INLINE double  Density(double x) const {
    if(var == 0.0f) return 0.0f;
    switch(type) {
    case NONE:
      return 0.0f;
    case UNIFORM:
      return UniformDen(x - mean, var);
    case BINOMIAL:
      return BinomDen((int)par, (int)(x-mean), var);
    case POISSON:
      return PoissonDen((int)(x-mean), var);
    case GAMMA:
      return GammaDen((int)par, var, x - mean);
    case GAUSSIAN:
      return GaussDen(x-mean, var);
    case BETA:
      return BetaDen(x-mean, var, par);
    }
    return 0.0f;
  }
  // get density of random variable according to current params


  ////////////////////////////////////////////////////////////////////////
  // various handy static random number generation functions:

  static int    IntZeroN(int n, int thr_no = -1)
  { if(n > 0) return (int)(MTRnd::GenRandInt32(thr_no) % (uint)n); return 0; }
  // #CAT_Int uniform random integer in the range between 0 and n, exclusive of n: [0,n) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static int    IntMinMax(int min, int max, int thr_no = -1)
  { return min + IntZeroN(max - min, thr_no); }
  // #CAT_Int uniform random integer in range between min and max, exclusive of max: [min,max) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static int    IntMeanRange(int mean, int range, int thr_no = -1)
  { return mean + (IntZeroN(2 * range + 1, thr_no) - range); }
  // #CAT_Int uniform random integer with given range on either side of the mean: [mean - range, mean + range] -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double ZeroOne(int thr_no = -1)
  { return MTRnd::GenRandRes53(thr_no); }
  // #CAT_Float uniform random number between zero and one (inclusive of 1 due to rounding!) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double UniformMinMax(double min, double max, int thr_no = -1)
  { return min + (max - min) * ZeroOne(thr_no); }
  // #CAT_Float uniform random number between min and max values inclusive (Do not use for generating integers - will not include max!) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double UniformMeanRange(double mean, double range, int thr_no = -1)
  { return mean + range * 2.0 * (ZeroOne(thr_no) - 0.5); }
  // #CAT_Float uniform random number with given range on either size of the mean: [mean - range, mean + range] -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double Binom(int n, double p, int thr_no = -1)
  { return STATE_CLASS(taMath_double)::binom_dev(n,p,thr_no); }
  // #CAT_Float binomial with n trials (par) each of probability p (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Poisson(double l, int thr_no = -1)
  { return STATE_CLASS(taMath_double)::poisson_dev(l,thr_no); }
  // #CAT_Float poisson with parameter l (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Gamma(double var, double j, int thr_no = -1)
  { return STATE_CLASS(taMath_double)::gamma_dev(j, var, thr_no); }
  // #CAT_Float gamma with given variance, number of exponential stages (par) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Gauss(double stdev, int thr_no = -1)
  { return stdev * STATE_CLASS(taMath_double)::gauss_dev(thr_no); }
  // #CAT_Float gaussian (normal) random number with given standard deviation -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Beta(double a, double b, int thr_no = -1)
  { return STATE_CLASS(taMath_double)::beta_dev(a, b, thr_no); }
  // #CAT_Float beta random number with two shape parameters a > 0 and b > 0 -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static bool   BoolProb(double p, int thr_no = -1)
  { return (ZeroOne(thr_no) < p); }
  // #CAT_Bool boolean true/false with given probability -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  
  static double UniformDen(double x, double range)
  { double rval = 0.0; if(fabs(x) <= range) rval = 1.0 / (2.0 * range); return rval; }
  // #CAT_Float uniform density at x with given range on either size of 0 (subtr mean from x before)
  static double BinomDen(int n, int j, double p) {
    return STATE_CLASS(taMath_double)::binom_den(n,j,p);
  }
  // #CAT_Float binomial density at j with n trials (par) each of probability p (var)
  static double PoissonDen(int j, double l) {
    return STATE_CLASS(taMath_double)::poisson_den(j,l);
  }
  // #CAT_Float poisson density with parameter l (var)
  static double GammaDen(int j, double l, double t) {
    return STATE_CLASS(taMath_double)::gamma_den(j,l,t);
  }
  // #CAT_Float gamma density at time t with given number of stages (par), lambda (var)
  static double GaussDen(double x, double stdev) {
    return STATE_CLASS(taMath_double)::gauss_den_sig(x, stdev);
  }
  // #CAT_Float gaussian (normal) density for given standard deviation (0 mean)
  static double BetaDen(double x, double a, double b) {
    return STATE_CLASS(taMath_double)::beta_den(x, a, b);
  }
  // #CAT_Float beta density at value 0 < x < 1 for shape parameters a, b

  STATE_CLASS(Random)() { Initialize(); }
private:
  void Initialize() {
    type = UNIFORM;
    mean = 0.0f;
    var = 1.0f;
    par = 1.0f;
  }

  void Copy_(const STATE_CLASS(Random)& cp){
    type = cp.type;
    mean = cp.mean;
    var = cp.var;
    par = cp.par;
  }
};

class STATE_CLASS(RndSeed) : public STATE_CLASS(taNBase) {
  // #STEM_BASE ##CAT_Math ##NO_TOKEN_CHOOSER random seeds: can control the random number generator to restart with the same pseudo-random sequence or get a new one
INHERITED(taNBase)
public:
  uint32_t       rnd_seed;   // #READ_ONLY the random seed

  virtual void   NewSeed() {
    rnd_seed = MTRnd::GetTimePidSeed();
    MTRnd::InitSeeds(rnd_seed);
  }
  // #BUTTON #CONFIRM set the seed to a new random value (based on time and process id)
  
  virtual void   OldSeed() {
    if(rnd_seed == 0) {
      NewSeed();
    }
    else {
      MTRnd::InitSeeds(rnd_seed);
    }
  }
  // #BUTTON #CONFIRM restore current seed to random num generator
  
  virtual void   Init(uint32_t i) {
    rnd_seed = i;
    MTRnd::InitSeeds(rnd_seed);
  }
  // initialize the seed based on given initializer

//   virtual void  DMem_Sync(MPI_Comm comm) {
// #ifdef DMEM_COMPILE
//     if(taMisc::dmem_nprocs <= 1)
//       return;
//     // just blast the first guy to all members of the same communicator
//     DMEM_MPICALL(MPI_Bcast(&rnd_seed, 1, MPI_INT, 0, comm),
//                  "Process::SyncAllSeeds", "Bcast");
//     OldSeed();            // then get my seed!
// #endif
//   }
  // #IGNORE synchronize seeds across all procs -- uses the first proc's seed

  STATE_CLASS(RndSeed)() { Initialize(); }
private:    
  void  Initialize() { rnd_seed = 0; }

};

class STATE_CLASS(Average) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math encapsulates the sum and count values for computing an average value in an incremental fashion over time
public:
  float         avg;    // the computed average value that was last computed, as sum / n -- this may not reflect current sum, n values -- just depends on when GetAvg was computed -- see UpdtAvg versions of increment methods
  float         sum;    // #DMEM_AGG_SUM overall sum of values accumulated since last reset
  int           n;      // #DMEM_AGG_SUM the number of values accumulated since last reset


  INLINE void   ResetSum()
  { sum = 0.0f; n = 0; }
  // #CAT_Average reset the sum and n accumulation variables
  INLINE void   ResetAvg()
  { ResetSum(); avg = 0.0f; }
  // #CAT_Average reset the sum and n accumulation variables, and the computed average value

  INLINE float  GetAvg()
  { if(n > 0) avg = sum / (float)n; return avg; }
  // #CAT_Average compute the average as sum / n, update the avg member to store this value, and return it

  INLINE float  GetAvg_Reset()
  { float rval = GetAvg(); ResetSum(); return rval; }
  // #CAT_Average compute the average as sum / n, update the avg member to store this value, and return it -- also reset sum, n counters for next time

  INLINE void   Increment(float val)
  { sum += val; n++; }
  // #CAT_Average increment the sum by given value, and n by one count
  INLINE float  IncrementAvg(float val)
  { Increment(val); return GetAvg(); }
  // #CAT_Average increment the sum by given value, and n by one count, and return new average value (updates avg field)

  INLINE void   IncrementN(float val, int n_new)
  { sum += val; n += n_new; }
  // #CAT_Average increment the sum by given value, and n by given count -- for a batch update of multiple items
  INLINE float  IncrementNAvg(float val, int n_new)
  { IncrementN(val, n_new); return GetAvg(); }
  // #CAT_Average increment the sum by given value, and n by given count, and return new average value (updates avg field)

  STATE_CLASS(Average)()      { ResetAvg(); }
};

 

class STATE_CLASS(MinMax) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math minimum-maximum values
  INHERITED(taOBase)
public:
  float         min;    // minimum value
  float         max;    // maximum value

  INLINE bool  RangeTest(float val) const { return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  INLINE bool  RangeTestEq(float val) const { return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)

  INLINE bool  operator < (const float val) const   { return (val < min); }
  INLINE bool  operator <= (const float val) const  { return (val <= min); }
  INLINE bool  operator > (const float val) const   { return (val > max); }
  INLINE bool  operator >= (const float val) const  { return (val >= max); }

  INLINE void  Init(float it)  { min = max = it; }  // initializes the max and min to this value
  INLINE void  Set(float mn, float mx) { min = mn; max = mx; }  // set values

  INLINE float  Range() const  { return (max - min); }
  INLINE float  Scale() const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }
  // scale is the inverse of range
  INLINE float  MidPoint() const { return 0.5f * (min + max); }
  // returns the range between the min and the max

  INLINE void  UpdateRange(STATE_CLASS(MinMax)& it)
  { min = fminf(it.min, min); max = fmaxf(it.max, max); }

  INLINE void  UpdateRange(float it)
  { min = fminf(it, min); max = fmaxf(it, max); }  // updates the range

  INLINE void  MaxLT(float it)  { max = fminf(it, max); }
  // max less than (or equal)

  INLINE void  MinGT(float it)  { min = fmaxf(it, min); }
  // min greater than (or equal)

  INLINE void  WithinRange(STATE_CLASS(MinMax)& it)         // put my range within given one
  { min = fmaxf(it.min, min); max = fminf(it.max, max); }
  INLINE void  WithinRange(float min_, float max_) // #IGNORE put my range within given one
  { min = fmaxf(min_, min); max = fminf(max_, max); }
  INLINE void  SymRange() // symmetrize my range around zero, with max abs value of current min, max
  { float mxabs = fmaxf(fabsf(min), fabsf(max)); min = -mxabs; max = mxabs; }

  INLINE float Normalize(float val) const { return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  INLINE float Project(float val) const   { return min + (val * Range()); }
  // project a normalized value into the current min-max range

  INLINE float Clip(float val) const
  { val = fminf(max,val); val = fmaxf(min,val); return val; }
  // clip given value within current range

  STATE_CLASS(MinMax)() { Initialize(); }
  
private:
  void  Initialize()            { min = max = 0.0f; }
  void  Copy_(const STATE_CLASS(MinMax)& cp) { min = cp.min; max = cp.max; }
};


class STATE_CLASS(MinMaxRange) : public STATE_CLASS(MinMax) {
  // ##UAE_IN_PROGRAM min-max values plus scale and range
public:
  float         range;          // #HIDDEN distance between min and max
  float         scale;          // #HIDDEN scale (1.0 / range)

  INLINE float Normalize(float val) const { return (val - min) * scale; }
  // normalize given value to 0-1 range given current in max

  INLINE float Project(float val) const   { return min + (val * range); }
  // project a normalized value into the current min-max range

  INLINE void  UpdateRange()
  { range = Range(); if(range != 0.0f) scale = 1.0f / range; }
  // update the saved range value from current min / max

  STATE_CLASS(MinMaxRange)() { Initialize(); }
  
private:
  void  Initialize()            { range = scale = 0.0f; }
  void  Copy_(const STATE_CLASS(MinMaxRange)& cp) { range = cp.range; scale = cp.scale; }
};


class STATE_CLASS(PRerrVals) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network contains precision and recall error values
public:
  float         true_pos;       // #DMEM_AGG_SUM true positive values -- e.g., how many active targets were correctly activated by the network (actually a sum over graded activations -- if(targ > act) act else targ)
  float         false_pos;      // #DMEM_AGG_SUM false positive values -- e.g., how many inactive targets were incorrectly activated by the network (actually a sum over graded activations -- if(act > targ) act - targ)
  float         false_neg;      // #DMEM_AGG_SUM false negative values -- e.g., how many active targets were incorrectly not activated by the network (actually a sum over graded activations -- if(targ > act) targ - act)
  float         true_neg;      // #DMEM_AGG_SUM true negative values -- e.g., to what extent was the unit correctly off
  float         precision;      // precision = true_pos / (true_pos + false_pos) -- how many of the positive responses were true positives -- i.e., of the responses the network made, how many were correct -- does not include any of the false negatives, so the network could be under-responding (see recall)
  float         recall;         // recall = true_pos / (true_pos + false_neg) -- how many true positive responses were there relative to the total number of positive targets -- if it did not respond to a number of cases where it should have, then recall will be low
  float         specificity;    // specificity = true_neg / (false_pos + true_neg) -- proportion of true negatives correctly identified as such
  float         fallout;       // fallout = fp / (fp + tn) -- fallout - false positive rate
  float         fdr;           // false discovery rate = fp / (fp + tp)
  float         fmeasure;       // fmeasure = 2 * precision * recall / (precision + recall) -- harmonic mean of precision and recall -- is 1 when network is performing optimally
  float         mcc;            // mcc = ((tp*tn) - (fp*fn)) / sqrt((tp+fp)*(tp+fn)*(tn+fp)*(tn+fn)) -- Matthews Correlation Coefficient. balanced statistic that reflects all of the kinds of errors the network could make. ranges from -1 to 1. this is the best overall value to look at for summary performance

  INLINE void   InitVals() { true_pos = false_pos = false_neg = true_neg = specificity = mcc = precision = recall = fallout = fdr = fmeasure = 0.0f; }
  // initialize all values to 0

  INLINE void   IncrVals(const STATE_CLASS(PRerrVals)& vls)
  { true_pos += vls.true_pos; false_pos += vls.false_pos; false_neg += vls.false_neg; true_neg += vls.true_neg; }
  // increment values from another set of values

  INLINE void   ComputePR() {
    precision = recall = specificity = fdr = fallout = fmeasure = mcc = 0.0f;
    float tp = true_pos; float fp = false_pos; float tn = true_neg; float fn = false_neg;

    precision   = tp > 0 && fp > 0  ? tp/(tp+fp):0;
    recall      = tp > 0 && fn > 0  ? tp/(tp+fn):0;
    specificity = fp > 0 && tn > 0  ? tn/(fp+tn):0;
    fdr         = tp > 0 && fp > 0  ? fp/(fp+tp):0;
    fallout     = fp > 0 && tn > 0  ? fp/(fp+tn):0;
    fmeasure    = tp > 0 && fp > 0 && fn > 0 ? 2*tp/(2*tp+fp+fn):0;
    mcc         = tp > 0 && fp > 0 && tn > 0 && fn > 0 ?
      (tp*tn-fp*fn)/sqrtf((tp+fp)*(tp+fn)*(tn+fp)*(tn+fn)):0;
  }
  // compute the precision, recall, and fmeasure values based on current raw stats values

  STATE_CLASS(PRerrVals)() { InitVals(); }
};


class STATE_CLASS(CircBufferIndex) {
  // Static-only collection of functions that implement circular buffer indexing logic, for using a fixed amount of storage as a wrap-around circular buffer -- very efficient for first-in-first-out moving-window buffers of recent values -- requires a starting index (st_idx), length of buffer (number of items currently stored), and an overall buffer size (maximum number that can be stored), above which the items fall off the end of the buffer
public:

  static inline int   CircIdx(const int cidx, const int st_idx, const int size)
  { int rval = cidx+st_idx; if(rval >= size) rval -= size; return rval; }
  // #CAT_CircAccess gets physical index from logical circular index -- no range checking

  static inline bool  CircIdxInRange(const int cidx, const int length)
  { if(cidx < 0) return false; if(cidx >= length) return false; return true; }
  // #CAT_CircAccess is the given logical circular index within range

  static inline void  CircShiftLeft(const int nshift, int& st_idx, int& length,
                                    const int size)
  { st_idx = CircIdx(nshift, st_idx, size); length -= nshift; }
  // #CAT_CircModify shift the buffer to the left -- shift the first elements off the start of the list, making room at the end for more elements (decreasing length)

  static inline void Reset(int& st_idx, int& length)
  { st_idx = 0; length = 0; }
  // #CAT_CircModify reset the buffer (starting index and length to 0)

  static inline float  CircEl_float(const int cidx, float* buf, int st_idx,
                                   const int size) {
    return buf[CircIdx(cidx, st_idx, size)];
  }
  // #CAT_CircAccess get a float value from a circular buffer
  static inline int  CircEl_int(const int cidx, int* buf, const int st_idx,
                                 const int size) {
    return buf[CircIdx(cidx, st_idx, size)];
  }
  // #CAT_CircAccess get a int value from a circular buffer
  static inline double CircEl_double(const int cidx, double* buf, const int st_idx,
                                   const int size) {
    return buf[CircIdx(cidx, st_idx, size)];
  }
  // #CAT_CircAccess get a int value from a circular buffer

  static inline void  CircAddShift_float(const float& item, float* buf, int& st_idx,
                                         int& length, const int size) {
    if(length >= size) {
      CircShiftLeft((1 + length) - size, st_idx, length, size); // make room
    }
    buf[CircIdx(length++, st_idx, size)] = item;      // set to the element at the end
  }
  // #CAT_CircModify add a new float item to a circular buffer, shifting it left if length is at or above size to ensure a fixed overall length list (otherwise expanding list up to size)

  static inline void  CircAddShift_double(const double& item, double* buf, int& st_idx,
                                         int& length, const int size) {
    if(length >= size) {
      CircShiftLeft((1 + length) - size, st_idx, length, size); // make room
    }
    buf[CircIdx(length++, st_idx, size)] = item;      // set to the element at the end
  }
  // #CAT_CircModify add a new double item to a circular buffer, shifting it left if length is at or above size to ensure a fixed overall length list (otherwise expanding list up to size)

  static inline void  CircAddShift_int(const int& item, int* buf, int& st_idx,
                                         int& length, const int size) {
    if(length >= size) {
      CircShiftLeft((1 + length) - size, st_idx, length, size); // make room
    }
    buf[CircIdx(length++, st_idx, size)] = item;      // set to the element at the end
  }
  // #CAT_CircModify add a new int item to a circular buffer, shifting it left if length is at or above size to ensure a fixed overall length list (otherwise expanding list up to size)

  
};


class STATE_CLASS(TimeUsedHRd); // #IGNORE

class STATE_CLASS(TimeUsedHR) : public STATE_CLASS(taNBase) {
  // #STEM_BASE #INLINE ##CAT_Program ##CHOOSE_AS_MEMBER computes amount of time used (in high resolution) for a given process: start the timer at the start, then do EndTimer and it computes the amount used
public:
  double        s_used;         // #SHOW #GUI_READ_ONLY total number of seconds used
  int           n_used;         // #SHOW #GUI_READ_ONLY number of individual times the timer has been used without resetting accumulation
  STATE_CLASS(Average)       avg_used;       // #SHOW #GUI_READ_ONLY running average of s_used, updated whenever UpdtAvg is called -- useful for keeping track of averages over time

  INIMPL void  StartTimer(bool reset_used = true);
  // #CAT_TimeUsed record the current time as the starting time, and optionally reset the time used information
  INIMPL void  EndTimer();
  // #CAT_TimeUsed record the current time as the ending time, and compute difference as the time used
  INIMPL void  ResetUsed();
  // #CAT_TimeUsed reset time used information -- does NOT reset the avg_used accumulator

  INLINE float IncrAvg()
  { return avg_used.IncrementAvg(s_used); }
  // #CAT_TimeUsed increment the avg_used running average with the current s_used data -- note this is the total accumulated s_used, not the average of s_used / n_used -- see IncrAvgFmAvg
  INLINE float IncrAvgFmAvg()
  { if(n_used > 0) return avg_used.IncrementAvg(s_used / (float)n_used); return 0.0f; }
  // #CAT_TimeUsed increment the avg_used running average with the current s_used / n_used average -- see also IncrAvg
  INLINE void  ResetAvg()
  { avg_used.ResetAvg(); }
  // #CAT_TimeUsed reset the avg_used running average accumulator

  INLINE void  EndIncrAvg() { EndTimer(); IncrAvg(); }
  // #CAT_TimeUsed end timing recording, and then increment this run into the ongoing average

  STATE_CLASS(TimeUsedHR)()  { Initialize(); }
  ~STATE_CLASS(TimeUsedHR)() { Destroy(); }
protected:
  STATE_CLASS(TimeUsedHRd)*   d; // private impl data (depends on platform)
private:
  INIMPL void  Initialize();
  INIMPL void  Destroy();
};


#ifdef TIME_USED
#undef TIME_USED
#endif
#define TIME_USED      STATE_CLASS(TimeUsedHR)

#endif // STATE_MAIN
