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

// this must be included in a defined State_core/main/cuda context with appropriate
// #ifndef multiple-include protection AT THAT LEVEL not here..

#pragma maketa_file_is_target Network
#pragma maketa_file_is_target NetworkState


class STATE_CLASS(NetStatsSpecs) : public STATE_CLASS(taOBase) {
  // ##INLINE ##NO_TOKENS ##CAT_Network parameters for how stats are computed
INHERITED(taOBase)
public:
  bool          sse_unit_avg;   // #CAT_Statistic compute sse as average sse over units (i.e., divide by total number of target units in layer)
  bool          sse_sqrt;       // #CAT_Statistic take the square root of the SSE, producing a Euclidian distance instead of raw sse (note this cannot technically be added across trials in a linear fashion, as raw sse can)
  float         cnt_err_tol;    // #CAT_Statistic tolerance for computing the count of number of errors over current epoch
  bool          prerr;          // #CAT_Statistic compute precision and recall error values over units, at same time as computing sum squared error (SSE)

  STATE_DECO_KEY("Network");
  STATE_TA_STD_CODE(NetStatsSpecs);
private:
  INLINE void	Initialize() {
    sse_unit_avg = false;
    sse_sqrt = false;
    cnt_err_tol = 0.0f;
    prerr = false;
  }
};

// NOTE: this also contains any other misc State classes that could be widely used --
// everything includes Network so this is the widest scope, and will be compiled into proper
// TA classes..

#ifndef STATE_MAIN

#include <MTRnd>

// TODO: need to eliminate taMath_double depend
#include <taMath_double>

class STATE_CLASS(taVector2i) : public STATE_CLASS(taBase) {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math an integer value in 2D coordinate space
INHERITED(taBase)
public:
  int           x;              // horizontal
  int           y;              // vertical

  INLINE bool   isZero() { return ((x == 0) && (y == 0)); }
  INLINE void   SetXY(int xx, int yy) { x = xx; y = yy;  }
  INLINE void   SetXY(float xx, float yy) { x = (int)xx; y = (int)yy; }
  INLINE void   GetXY(float& xx, float& yy) { xx = (float)x; yy = (float)y; }

  STATE_CLASS(taVector2i)()                           { SetXY(0, 0); }
  STATE_CLASS(taVector2i)(int xx)                     { SetXY(xx, xx); }
  STATE_CLASS(taVector2i)(int xx, int yy)             { SetXY(xx, yy); }
  STATE_CLASS(taVector2i)(float xx, float yy)         { SetXY(xx, yy); }

  // INLINE taVector2i& operator=(const taVector2f& cp) {
  //   x = (int)cp.x; y = (int)cp.y;
  //   return *this;
  // }

  INLINE STATE_CLASS(taVector2i)& operator=(int cp)           { x = cp; y = cp; return *this;}
  INLINE STATE_CLASS(taVector2i)& operator=(float cp)         { x = (int)cp; y = (int)cp; return *this;}
  INLINE STATE_CLASS(taVector2i)& operator=(double cp)        { x = (int)cp; y = (int)cp; return *this;}

  INLINE STATE_CLASS(taVector2i)&     operator += (const STATE_CLASS(taVector2i)& td)   { x += td.x; y += td.y; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator -= (const STATE_CLASS(taVector2i)& td)   { x -= td.x; y -= td.y; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator *= (const STATE_CLASS(taVector2i)& td)   { x *= td.x; y *= td.y; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator /= (const STATE_CLASS(taVector2i)& td)   { x /= td.x; y /= td.y; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator %= (const STATE_CLASS(taVector2i)& td)   { x %= td.x; y %= td.y; return *this;}

  INLINE STATE_CLASS(taVector2i)&     operator += (int td)    { x += td; y += td; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator -= (int td)    { x -= td; y -= td; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator *= (int td)    { x *= td; y *= td; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator /= (int td)    { x /= td; y /= td; return *this;}
  INLINE STATE_CLASS(taVector2i)&     operator %= (int td)    { x %= td; y %= td; return *this;}

  INLINE STATE_CLASS(taVector2i) operator + (const STATE_CLASS(taVector2i)& td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator - (const STATE_CLASS(taVector2i)& td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator * (const STATE_CLASS(taVector2i)& td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator / (const STATE_CLASS(taVector2i)& td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  INLINE STATE_CLASS(taVector2i) operator + (int td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator - (int td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator * (int td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator / (int td) const {
    STATE_CLASS(taVector2i) rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  INLINE STATE_CLASS(taVector2i) operator - () const {
    STATE_CLASS(taVector2i) rv; rv.x = -x; rv.y = -y; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator -- () const {
    STATE_CLASS(taVector2i) rv = *this; rv.x--; rv.y--; return rv;
  }
  INLINE STATE_CLASS(taVector2i) operator ++ () const {
    STATE_CLASS(taVector2i) rv = *this; rv.x++; rv.y++; return rv;
  }

  INLINE bool operator < (const STATE_CLASS(taVector2i)& td) const { return (x < td.x) && (y < td.y); }
  INLINE bool operator > (const STATE_CLASS(taVector2i)& td) const { return (x > td.x) && (y > td.y); }
  INLINE bool operator <= (const STATE_CLASS(taVector2i)& td) const { return (x <= td.x) && (y <= td.y); }
  INLINE bool operator >= (const STATE_CLASS(taVector2i)& td) const { return (x >= td.x) && (y >= td.y); }
  INLINE bool operator == (const STATE_CLASS(taVector2i)& td) const { return (x == td.x) && (y == td.y); }
  INLINE bool operator != (const STATE_CLASS(taVector2i)& td) const { return (x != td.x) || (y != td.y); }

  INLINE bool operator <  (int td) const { return (x < td) && (y < td); }
  INLINE bool operator >  (int td) const { return (x > td) && (y > td); }
  INLINE bool operator <= (int td) const { return (x <= td) && (y <= td); }
  INLINE bool operator >= (int td) const { return (x >= td) && (y >= td); }
  INLINE bool operator == (int td) const { return (x == td) && (y == td); }
  INLINE bool operator != (int td) const { return (x != td) || (y != td); }

  INLINE bool OrEq(int td) const { return (x == td) || (y == td); }
  INLINE bool OrEq(const STATE_CLASS(taVector2i)& td) const { return (x == td.x) || (y == td.y); }

  INLINE int SqMag() const { return x * x + y * y; }
  // squared magnitude of vector
  INLINE float Mag() const { return sqrtf((float)SqMag()); }

  INLINE float  SqDist(const STATE_CLASS(taVector2i)& td) const { // squared distance between two vectors
    STATE_CLASS(taVector2i) dist = *this - td; return dist.SqMag();
  }
  INLINE float  Dist(const STATE_CLASS(taVector2i)& td) const { return sqrtf(SqDist(td)); }
  INLINE int    Sum() const     { return x + y; }
  INLINE int    Product() const { return x * y; }

  static INLINE int Sgn(int val) { return (val >= 0) ? 1 : -1; }
  static INLINE int Absv(int val) { return (val >= 0) ? val : -val; }

  INLINE void   Invert()        { x = -x; y = -y; }
  INLINE void   SumNorm()       { int mg = Sum(); if(mg != 0.0) *this /= mg; }
  INLINE void   Abs()           { x = Absv(x); y = Absv(y); }
  INLINE void   Min(STATE_CLASS(taVector2i)& td) { x = MIN(x,td.x); y = MIN(y,td.y); }
  INLINE void   Max(STATE_CLASS(taVector2i)& td) { x = MAX(x,td.x); y = MAX(y,td.y); }

  INLINE int MaxVal() const     { int mx = MAX(x, y); return mx; }
  INLINE int MinVal() const     { int mn = MIN(x, y); return mn; }

  INLINE bool  FitN(int n) {            // adjust x and y to fit x total elements
    if((x * y) == n)	return false;
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
      x = n;    y = 1;		// just go linear for small values
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

  INLINE void   WrapMinDist(STATE_CLASS(taVector2i)& dst, const STATE_CLASS(taVector2i)& max, const STATE_CLASS(taVector2i)& cmp,
                            const STATE_CLASS(taVector2i)& cmp_half) {
    WrapMinDistOne(x, dst.x, max.x, cmp.x, cmp_half.x);
    WrapMinDistOne(y, dst.y, max.y, cmp.y, cmp_half.y);
  }
  // in computing the distance between two coords: dst = this-cmp, consider whether the distance is shorter if this is wrapped around as a function of pos_max size (condition on which side of the half-way point of the range for cmp value, cmp_half, for which way to wrap) -- if it is shorter, then update this pos to new extended value (beyond normal range either - or +) and also update the distance value

  static INLINE void    WrapOne(int& c, int max)
  { if(c >= 0 && c < max) return; if(c < 0) c = max + (c % max); else c = c % max; }
  // wrap-around one dimension
  INLINE void   Wrap(const STATE_CLASS(taVector2i)& max)
  { WrapOne(x, max.x); WrapOne(y, max.y); }
  // wrap-around coordinates within 0,0 - max range

  static INLINE bool   WrapOneHalf(int& c, int max)
  { if(c >= 0 && c < max) return false;  bool out_of_range = false;
    if(c < 0) { if(c < -max/2) out_of_range = true; c = max + (c % max); }
    else      { if(c > max + max/2) out_of_range = true; c = c % max; }
    return out_of_range; }
  // wrap-around one dimension, return true if out of range (more than half way around other side)
  INLINE bool   WrapHalf(const STATE_CLASS(taVector2i)& max)
  { bool wcx = WrapOneHalf(x, max.x); bool wcy = WrapOneHalf(y, max.y);
    return wcx || wcy; }
  // wrap-around coordinates within 0,0 - max range, return true if out of range (more than half way around other side)

  static INLINE bool   ClipOne(int& c, int max)
  { if(c >= 0 && c < max) return false; if(c < 0) c = 0; else c = max-1; return true; }
  // clip one dimension, true if out of range
  INLINE bool    Clip(const STATE_CLASS(taVector2i)& max)
  { bool wcx = ClipOne(x, max.x); bool wcy = ClipOne(y, max.y);
    return wcx || wcy; }
  // clip coordinates within 0,0 - max range, true if out of range

  INLINE void   SetFmIndex(int idx, int x_size) {
    x = idx % x_size;
    y = idx / x_size;
  }
  // set x, y values from a "cell" index in a 2d matrix-like space organized with x as the inner loop and y as the outer loop, with a given x dimension size

  INLINE bool   WrapClip(bool wrap, const STATE_CLASS(taVector2i)& max)
  { if(wrap) { Wrap(max); return false; } return Clip(max); }
  // wrap-around or clip coordinates within 0,0 - max range, true if clipped out of range -- for performance, it is better to use separate code for wrap and clip cases
};

// INLINE STATE_CLASS(taVector2i) operator + (int td, const STATE_CLASS(taVector2i)& v) {
//   STATE_CLASS(taVector2i) rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
// }
// INLINE STATE_CLASS(taVector2i) operator - (int td, const STATE_CLASS(taVector2i)& v) {
//   STATE_CLASS(taVector2i) rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
// }
// INLINE STATE_CLASS(taVector2i) operator * (int td, const STATE_CLASS(taVector2i)& v) {
//   STATE_CLASS(taVector2i) rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
// }
// INLINE STATE_CLASS(taVector2i) operator / (int td, const STATE_CLASS(taVector2i)& v) {
//   STATE_CLASS(taVector2i) rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
// }


class STATE_CLASS(taVector2f) : public STATE_CLASS(taBase) {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math a value in 2D coordinate space
INHERITED(taBase)
public:
  float         x;              // horizontal
  float         y;              // vertical

  INLINE void   SetXY(float xx, float yy)       { x = xx; y = yy; }
  INLINE void   GetXY(float& xx, float& yy)     { xx = x; yy = y; }

  STATE_CLASS(taVector2f)()                     { SetXY(0, 0); }
  STATE_CLASS(taVector2f)(float xx)             { SetXY(xx, xx); }
  STATE_CLASS(taVector2f)(float xx, float yy)   { SetXY(xx, yy); }
  STATE_CLASS(taVector2f)(int xx)               { SetXY(xx, xx); }
  STATE_CLASS(taVector2f)(int xx, int yy)       { SetXY(xx, yy); }
  STATE_CLASS(taVector2f)(const STATE_CLASS(taVector2i)& cp) { SetXY(cp.x, cp.y); }

  INLINE STATE_CLASS(taVector2f)& operator=(const STATE_CLASS(taVector2i)& cp) {
    x = (float)cp.x; y = (float)cp.y;
    return *this;
  }
  INLINE STATE_CLASS(taVector2f)& operator=(float cp)            { x = cp; y = cp; return *this;}
  INLINE STATE_CLASS(taVector2f)& operator=(double cp)           { x = (float)cp; y = (float)cp; return *this;}

  INLINE STATE_CLASS(taVector2f)& operator += (const STATE_CLASS(taVector2f)& td) { x += td.x; y += td.y; return *this;}
  INLINE STATE_CLASS(taVector2f)& operator -= (const STATE_CLASS(taVector2f)& td) { x -= td.x; y -= td.y; return *this;}
  INLINE STATE_CLASS(taVector2f)& operator *= (const STATE_CLASS(taVector2f)& td) { x *= td.x; y *= td.y; return *this;}
  INLINE STATE_CLASS(taVector2f)& operator /= (const STATE_CLASS(taVector2f)& td) { x /= td.x; y /= td.y; return *this;}

  INLINE STATE_CLASS(taVector2f)& operator += (float td) { x += td; y += td; return *this;}
  INLINE STATE_CLASS(taVector2f)& operator -= (float td) { x -= td; y -= td; return *this;}
  INLINE STATE_CLASS(taVector2f)& operator *= (float td) { x *= td; y *= td; return *this;}
  INLINE STATE_CLASS(taVector2f)& operator /= (float td) { x /= td; y /= td; return *this;}

  INLINE STATE_CLASS(taVector2f) operator + (const STATE_CLASS(taVector2f)& td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  INLINE STATE_CLASS(taVector2f) operator - (const STATE_CLASS(taVector2f)& td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  INLINE STATE_CLASS(taVector2f) operator * (const STATE_CLASS(taVector2f)& td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  INLINE STATE_CLASS(taVector2f) operator / (const STATE_CLASS(taVector2f)& td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  INLINE STATE_CLASS(taVector2f) operator + (float td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  INLINE STATE_CLASS(taVector2f) operator - (float td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  INLINE STATE_CLASS(taVector2f) operator * (float td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  INLINE STATE_CLASS(taVector2f) operator / (float td) const {
    STATE_CLASS(taVector2f) rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  INLINE STATE_CLASS(taVector2f) operator - () const {
    STATE_CLASS(taVector2f) rv; rv.x = -x; rv.y = -y; return rv;
  }

  INLINE bool operator < (const STATE_CLASS(taVector2f)& td) const { return (x < td.x) && (y < td.y); }
  INLINE bool operator > (const STATE_CLASS(taVector2f)& td) const { return (x > td.x) && (y > td.y); }
  INLINE bool operator <= (const STATE_CLASS(taVector2f)& td) const { return (x <= td.x) && (y <= td.y); }
  INLINE bool operator >= (const STATE_CLASS(taVector2f)& td) const { return (x >= td.x) && (y >= td.y); }
  INLINE bool operator == (const STATE_CLASS(taVector2f)& td) const { return (x == td.x) && (y == td.y); }
  INLINE bool operator != (const STATE_CLASS(taVector2f)& td) const { return (x != td.x) || (y != td.y); }

  INLINE bool operator <  (float td) const { return (x < td) && (y < td); }
  INLINE bool operator >  (float td) const { return (x > td) && (y > td); }
  INLINE bool operator <= (float td) const { return (x <= td) && (y <= td); }
  INLINE bool operator >= (float td) const { return (x >= td) && (y >= td); }
  INLINE bool operator == (float td) const { return (x == td) && (y == td); }
  INLINE bool operator != (float td) const { return (x != td) || (y != td); }

  INLINE float SqMag() const    { return x * x + y * y; }
  // squared magnitude of vector
  INLINE float Mag() const      { return sqrtf(SqMag()); }

  INLINE float  SqDist(const STATE_CLASS(taVector2f)& td) const { // squared distance between two vectors
    STATE_CLASS(taVector2f) dist = *this - td; return dist.Mag();
  }
  INLINE float  Dist(const STATE_CLASS(taVector2f)& td) const { return sqrtf(SqDist(td)); }
  INLINE float  Sum() const     { return x + y; }
  INLINE float  Product() const { return x * y; }

  INLINE void   Invert()        { x = -x; y = -y; }
  INLINE void   MagNorm()       { float mg = Mag(); if(mg > 0.0) *this /= mg; }
  INLINE void   SumNorm()       { float mg = Sum(); if(mg != 0.0) *this /= mg; }
  INLINE void   Abs()           { x = fabs(x); y = fabs(y); }

  INLINE float MaxVal() const   { float mx = fmaxf(x, y); return mx; }
  INLINE float MinVal() const   { float mn = fminf(x, y); return mn; }
};


// INLINE STATE_CLASS(taVector2f) operator + (float td, const STATE_CLASS(taVector2f)& v) {
//   STATE_CLASS(taVector2f) rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
// }
// INLINE STATE_CLASS(taVector2f) operator - (float td, const STATE_CLASS(taVector2f)& v) {
//   STATE_CLASS(taVector2f) rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
// }
// INLINE STATE_CLASS(taVector2f) operator * (float td, const STATE_CLASS(taVector2f)& v) {
//   STATE_CLASS(taVector2f) rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
// }
// INLINE STATE_CLASS(taVector2f) operator / (float td, const STATE_CLASS(taVector2f)& v) {
//   STATE_CLASS(taVector2f) rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
// }


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
  { return taMath_double::binom_dev(n,p,thr_no); }
  // #CAT_Float binomial with n trials (par) each of probability p (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Poisson(double l, int thr_no = -1)
  { return taMath_double::poisson_dev(l,thr_no); }
  // #CAT_Float poisson with parameter l (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Gamma(double var, double j, int thr_no = -1)
  { return taMath_double::gamma_dev(j, var, thr_no); }
  // #CAT_Float gamma with given variance, number of exponential stages (par) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Gauss(double stdev, int thr_no = -1)
  { return stdev * taMath_double::gauss_dev(thr_no); }
  // #CAT_Float gaussian (normal) random number with given standard deviation -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Beta(double a, double b, int thr_no = -1)
  { return taMath_double::beta_dev(a, b, thr_no); }
  // #CAT_Float beta random number with two shape parameters a > 0 and b > 0 -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static bool   BoolProb(double p, int thr_no = -1)
  { return (ZeroOne(thr_no) < p); }
  // #CAT_Bool boolean true/false with given probability -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

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

  virtual void  DMem_Sync(MPI_Comm comm) {
#ifdef DMEM_COMPILE
    if(taMisc::dmem_nprocs <= 1)
      return;
    // just blast the first guy to all members of the same communicator
    DMEM_MPICALL(MPI_Bcast(&rnd_seed, 1, MPI_INT, 0, comm),
                 "Process::SyncAllSeeds", "Bcast");
    OldSeed();            // then get my seed!
#endif
  }
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


#endif // STATE_MAIN
