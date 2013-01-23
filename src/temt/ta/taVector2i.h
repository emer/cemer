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

#ifndef taVector2i_h
#define taVector2i_h 1

// parent includes:
#include <taBase>

#include <cmath>

// member includes:

// declare all other types mentioned but not required to include:
class MatrixGeom; // 
class MatrixIndex; // 
class taMatrix; // 
class taVector2f; //

TypeDef_Of(taVector2i);

class TA_API taVector2i : public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a value in 2D coordinate space
INHERITED(taBase)
public:
  int           x;              // horizontal
  int           y;              // vertical

  bool          isZero() {return ((x == 0) && (y == 0));}
  inline void   SetXY(int xx, int yy) { x = xx; y = yy;  }
  inline void   SetXY(float xx, float yy) { x = (int)xx; y = (int)yy; }
  inline void   GetXY(float& xx, float& yy) { xx = (float)x; yy = (float)y; }
  virtual void  CopyToMatrixGeom(MatrixGeom& geom);
  virtual void  CopyToMatrixIndex(MatrixIndex& idx);

  void	        ToMatrix(taMatrix& mat) const;
  // set values to a 1d matrix object (can be any type of matrix object)
  void	        FromMatrix(taMatrix& mat);
  // set values from a matrix object (can be any type of matrix object)

  taVector2i(int xx)                     { SetXY(xx, xx); }
  taVector2i(int xx, int yy)             { SetXY(xx, yy); }
  taVector2i(float xx, float yy)         { SetXY(xx, yy); }
  taVector2i(const taVector2f& cp);  // conversion constructor

  TA_BASEFUNS_LITE(taVector2i)

  taVector2i& operator=(const taVector2f& cp);
  inline taVector2i& operator=(int cp)           { x = cp; y = cp; return *this;}
  inline taVector2i& operator=(float cp)                 { x = (int)cp; y = (int)cp; return *this;}
  inline taVector2i& operator=(double cp)                { x = (int)cp; y = (int)cp; return *this;}

  inline taVector2i&     operator += (const taVector2i& td)       { x += td.x; y += td.y; return *this;}
  inline taVector2i&     operator -= (const taVector2i& td)       { x -= td.x; y -= td.y; return *this;}
  inline taVector2i&     operator *= (const taVector2i& td)       { x *= td.x; y *= td.y; return *this;}
  inline taVector2i&     operator /= (const taVector2i& td)       { x /= td.x; y /= td.y; return *this;}
  inline taVector2i&     operator %= (const taVector2i& td)       { x %= td.x; y %= td.y; return *this;}

  inline taVector2i&     operator += (int td)    { x += td; y += td; return *this;}
  inline taVector2i&     operator -= (int td)    { x -= td; y -= td; return *this;}
  inline taVector2i&     operator *= (int td)    { x *= td; y *= td; return *this;}
  inline taVector2i&     operator /= (int td)    { x /= td; y /= td; return *this;}
  inline taVector2i&     operator %= (int td)    { x %= td; y %= td; return *this;}

  inline taVector2i operator + (const taVector2i& td) const {
    taVector2i rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  inline taVector2i operator - (const taVector2i& td) const {
    taVector2i rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  inline taVector2i operator * (const taVector2i& td) const {
    taVector2i rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  inline taVector2i operator / (const taVector2i& td) const {
    taVector2i rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  inline taVector2i operator + (int td) const {
    taVector2i rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  inline taVector2i operator - (int td) const {
    taVector2i rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  inline taVector2i operator * (int td) const {
    taVector2i rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  inline taVector2i operator / (int td) const {
    taVector2i rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  inline taVector2i operator - () const {
    taVector2i rv; rv.x = -x; rv.y = -y; return rv;
  }
  inline taVector2i operator -- () const {
    taVector2i rv = *this; rv.x--; rv.y--; return rv;
  }
  inline taVector2i operator ++ () const {
    taVector2i rv = *this; rv.x++; rv.y++; return rv;
  }

  inline bool operator < (const taVector2i& td) const { return (x < td.x) && (y < td.y); }
  inline bool operator > (const taVector2i& td) const { return (x > td.x) && (y > td.y); }
  inline bool operator <= (const taVector2i& td) const { return (x <= td.x) && (y <= td.y); }
  inline bool operator >= (const taVector2i& td) const { return (x >= td.x) && (y >= td.y); }
  inline bool operator == (const taVector2i& td) const { return (x == td.x) && (y == td.y); }
  inline bool operator != (const taVector2i& td) const { return (x != td.x) || (y != td.y); }

  inline bool operator <  (int td) const { return (x < td) && (y < td); }
  inline bool operator >  (int td) const { return (x > td) && (y > td); }
  inline bool operator <= (int td) const { return (x <= td) && (y <= td); }
  inline bool operator >= (int td) const { return (x >= td) && (y >= td); }
  inline bool operator == (int td) const { return (x == td) && (y == td); }
  inline bool operator != (int td) const { return (x != td) || (y != td); }

  inline bool OrEq(int td) const { return (x == td) || (y == td); }
  inline bool OrEq(const taVector2i& td) const { return (x == td.x) || (y == td.y); }

  inline int SqMag() const { return x * x + y * y; }
  // squared magnitude of vector
  inline float Mag() const { return sqrt((float)SqMag()); }

  inline float  SqDist(const taVector2i& td) const { // squared distance between two vectors
    taVector2i dist = *this - td; return dist.SqMag();
  }
  inline float  Dist(const taVector2i& td) const { return sqrt(SqDist(td)); }
  inline int    Sum() const     { return x + y; }
  inline int    Product() const { return x * y; }

  static inline int Sgn(int val) { return (val >= 0) ? 1 : -1; }
  static inline int Absv(int val) { return (val >= 0) ? val : -val; }

  inline void   Invert()        { x = -x; y = -y; }
  inline void   SumNorm()       { int mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void   Abs()           { x = Absv(x); y = Absv(y); }
  inline void   Min(taVector2i& td) { x = MIN(x,td.x); y = MIN(y,td.y); }
  inline void   Max(taVector2i& td) { x = MAX(x,td.x); y = MAX(y,td.y); }

  inline int MaxVal() const     { int mx = MAX(x, y); return mx; }
  inline int MinVal() const     { int mn = MIN(x, y); return mn; }

  inline String GetStr() const { return String(x) + ", " + String(y); }

  virtual bool  FitN(int n);            // adjust x and y to fit x total elements

  void          SetGtEq(int n)  { x = MAX(n, x);  y = MAX(n, y); }
  // set each to be greater than or equal to n
  void          SetLtEq(int n)  { x = MIN(n, x);  y = MIN(n, y); }
  // set each to be less than or equal to n

  static int    WrapMax(int c, int max) {
    int rval = c % max; if(rval < 0) rval += max;  return rval;
  }
  static void   WrapMinDistOne(int& pos, int& dst, const int pos_max, const int cmp,
                               const int cmp_half) {
    if(cmp < cmp_half) {
      if(Absv((pos-pos_max) - cmp) < Absv(dst)) { pos -= pos_max; dst = pos - cmp; }
    }
    else {
      if(Absv((pos+pos_max) - cmp) < Absv(dst)) { pos += pos_max; dst = pos - cmp; }
    }
  }
  // in computing the distance between two coord vals: dst = pos-cmp, consider whether the distance is shorter if pos is wrapped around as a function of pos_max size (condition on which side of the half-way point of the range for cmp value, cmp_half, for which way to wrap) -- if it is shorter, then update pos to new extended value (beyond normal range either - or +) and also update the distance value

  void          WrapMinDist(taVector2i& dst, const taVector2i& max, const taVector2i& cmp,
                            const taVector2i& cmp_half) {
    WrapMinDistOne(x, dst.x, max.x, cmp.x, cmp_half.x);
    WrapMinDistOne(y, dst.y, max.y, cmp.y, cmp_half.y);
  }
  // in computing the distance between two coords: dst = this-cmp, consider whether the distance is shorter if this is wrapped around as a function of pos_max size (condition on which side of the half-way point of the range for cmp value, cmp_half, for which way to wrap) -- if it is shorter, then update this pos to new extended value (beyond normal range either - or +) and also update the distance value

  static bool   WrapClipOne(bool wrap, int& c, int max);
  // wrap-around or clip one dimension, true if out of range (clipped or more than half way around other side for wrap)
  bool          WrapClip(bool wrap, const taVector2i& max) {
    bool wcx = WrapClipOne(wrap, x, max.x); bool wcy = WrapClipOne(wrap, y, max.y);
    return wcx || wcy;          // have to explicitly call else cond eval will avoid clip!
  } // wrap-around or clip coordinates within 0,0 - max range, true if out of range (clipped or more than half way around other side for wrap)


  inline void   SetFmIndex(int idx, int x_size) {
    x = idx % x_size;
    y = idx / x_size;
  }
  // set x, y values from a "cell" index in a 2d matrix-like space organized with x as the inner loop and y as the outer loop, with a given x dimension size

private:
  inline void   Copy_(const taVector2i& cp) { x = cp.x; y = cp.y; }
  inline void   Initialize()            { x = y = 0; }
  inline void   Destroy()               { };
};

inline taVector2i operator + (int td, const taVector2i& v) {
  taVector2i rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
}
inline taVector2i operator - (int td, const taVector2i& v) {
  taVector2i rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
}
inline taVector2i operator * (int td, const taVector2i& v) {
  taVector2i rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
}
inline taVector2i operator / (int td, const taVector2i& v) {
  taVector2i rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
}

typedef taVector2i TwoDCoord;		// #INSTANCE obsolete version of taVector2i

#endif // taVector2i_h
