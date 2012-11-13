// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef ta_geometry_h
#define ta_geometry_h

#include "ta_base.h"
#include "ta_matrix.h"
#include "ta_def.h"
#include "ta_TA_type_WRAPPER.h"

#ifdef TA_GUI
#include "igeometry.h"
#endif

#ifndef __MAKETA__
# include <math.h>
# include <ode/ode.h>
#endif

class taVector2i;
class PosVector2i;
class taVector3i;
class PosVector3i;
class taVector2f;
class taVector3f;
class taAxisAngle;
class taQuaternion;
class taTransform; //

// these are the old obsolete class names, defined for posterity, but should be phased out

typedef taVector2i TwoDCoord;		// #INSTANCE obsolete version of taVector2i
typedef PosVector2i PosTwoDCoord;	// #INSTANCE obsolete version of PosVector2i
typedef taVector3i TDCoord;		// #INSTANCE obsolete version of taVector3i
typedef PosVector3i PosTDCoord;		// #INSTANCE obsolete version of PosVector3i
typedef taVector2f FloatTwoDCoord;	// #INSTANCE obsolete version of taVector2f
typedef taVector3f FloatTDCoord;	// #INSTANCE obsolete version of taVector3f
typedef taAxisAngle FloatRotation;	// #INSTANCE obsolete version of taAxisAngle
typedef taTransform FloatTransform;	// #INSTANCE obsolete version of taTransform

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

class TA_API PosVector2i : public taVector2i {
  // #NO_UPDATE_AFTER #INLINE #INLINE_DUMP positive-only value in 2D coordinate space
  INHERITED(taVector2i)
public:
  TA_BASEFUNS_LITE(PosVector2i);

  inline PosVector2i& operator=(int cp) { x = cp; y = cp; return *this;}
  inline PosVector2i& operator=(float cp) { x = (int)cp; y = (int)cp; return *this;}
  inline PosVector2i& operator=(double cp) { x = (int)cp; y = (int)cp; return *this;}
  inline PosVector2i& operator=(const taVector2i& cp)
    {x = cp.x; y = cp.y; SetGtEq(0); return *this;}
protected:
  void  UpdateAfterEdit_impl();
private:
  void  Copy_(const PosVector2i&) {}
  void  Initialize()            { }
  void  Destroy()               { }
};

class TA_API XYNGeom : public PosVector2i {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP two-dimensional X-Y geometry with possibility of total number n != x*y
  INHERITED(PosVector2i)
public:
  bool          n_not_xy;       // #DEF_false total number of units is less than x * y
  int           n;              // #CONDEDIT_ON_n_not_xy:true total number of units (=x*y unless n_not_xy is true)

  int           z;
  // #HIDDEN #READ_ONLY #NO_SAVE legacy v3 third dimension -- used for conversion only -- do not use!!  to be removed at some later date

  void operator=(const taVector2i& cp);

  inline void   SetXYN(int xx, int yy, int nn)
  { x = xx; y = yy; n = nn; UpdateFlag(); }
  // set x, y, and n in one step

  inline void   UpdateFlag()    { n_not_xy = (x*y != n); }
  // update the n_not_xy flag from the current x,y,n values
  inline void   UpdateXYfmN()   { FitN(n); }
  // update x,y from existing n, including updating the n_not_xy flag
  inline void   UpdateNfmXY()   { n = x * y; UpdateFlag(); }
  // update x,y from N

  override bool FitN(int no)
  { bool rval = inherited::FitN(no); n = no; UpdateFlag(); return rval; }

  TA_BASEFUNS_LITE(XYNGeom);
protected:
  void  UpdateAfterEdit_impl();
private:
  void  Copy_(const XYNGeom& cp)
  { n_not_xy = cp.n_not_xy; n = cp.n; z = cp.z; }
  void  Initialize();
  void  Destroy()               { };

};

class TA_API taVector3i : public taVector2i {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP a value in 3D coordinate space
  INHERITED(taVector2i)
public:
  int           z;              // depth

  bool          isZero() {return ((x == 0) && (y == 0) && (z == 0));}

  inline void   SetXYZ(int xx, int yy, int zz) {
    x = xx; y = yy; z = zz;
  }
  inline void   SetXYZ(float xx, float yy, float zz) {
    x = (int)xx; y = (int)yy; z = (int)zz;
  }
  inline void   GetXYZ(float& xx, float& yy, float& zz) {
    xx = (float)x; yy = (float)y; zz = (float)z;
  }
  override void CopyToMatrixGeom(MatrixGeom& geom);
  override void CopyToMatrixIndex(MatrixIndex& idx);

  taVector3i(int xx)                       { SetXYZ(xx, xx, xx); }
  taVector3i(int xx, int yy, int zz)       { SetXYZ(xx, yy, zz); }
  taVector3i(float xx, float yy, float zz) { SetXYZ(xx, yy, zz); }
  taVector3i(const taVector3f& cp);      // conversion constructor
  TA_BASEFUNS_LITE(taVector3i)

  taVector3i& operator=(const taVector3f& cp);
  inline taVector3i& operator=(int cp)             { x = cp; y = cp; z = cp; return *this;}
  inline taVector3i& operator=(float cp)           { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
  inline taVector3i& operator=(double cp)          { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}

  inline taVector3i& operator += (const taVector3i& td)       { x += td.x; y += td.y; z += td.z; return *this;}
  inline taVector3i& operator -= (const taVector3i& td)       { x -= td.x; y -= td.y; z -= td.z; return *this;}
  inline taVector3i& operator *= (const taVector3i& td)       { x *= td.x; y *= td.y; z *= td.z; return *this;}
  inline taVector3i& operator /= (const taVector3i& td)       { x /= td.x; y /= td.y; z /= td.z; return *this;}
  inline taVector3i& operator %= (const taVector3i& td)       { x %= td.x; y %= td.y; z %= td.z; return *this;}

  inline taVector3i& operator += (int td)  { x += td; y += td; z += td; return *this;}
  inline taVector3i& operator -= (int td)  { x -= td; y -= td; z -= td; return *this;}
  inline taVector3i& operator *= (int td)  { x *= td; y *= td; z *= td; return *this;}
  inline taVector3i& operator /= (int td)  { x /= td; y /= td; z /= td; return *this;}
  inline taVector3i& operator %= (int td)  { x %= td; y %= td; z %= td; return *this;}

  inline taVector3i operator + (const taVector3i& td) const {
    taVector3i rv; rv.x = x + td.x; rv.y = y + td.y; rv.z = z + td.z; return rv;
  }
  inline taVector3i operator - (const taVector3i& td) const {
    taVector3i rv; rv.x = x - td.x; rv.y = y - td.y; rv.z = z - td.z; return rv;
  }
  inline taVector3i operator * (const taVector3i& td) const {
    taVector3i rv; rv.x = x * td.x; rv.y = y * td.y; rv.z = z * td.z; return rv;
  }
  inline taVector3i operator / (const taVector3i& td) const {
    taVector3i rv; rv.x = x / td.x; rv.y = y / td.y; rv.z = z / td.z; return rv;
  }

  inline taVector3i operator + (int td) const {
    taVector3i rv; rv.x = x + td; rv.y = y + td; rv.z = z + td; return rv;
  }
  inline taVector3i operator - (int td) const {
    taVector3i rv; rv.x = x - td; rv.y = y - td; rv.z = z - td; return rv;
  }
  inline taVector3i operator * (int td) const {
    taVector3i rv; rv.x = x * td; rv.y = y * td; rv.z = z * td; return rv;
  }
  inline taVector3i operator / (int td) const {
    taVector3i rv; rv.x = x / td; rv.y = y / td; rv.z = z / td; return rv;
  }

  inline taVector3i operator - () const {
    taVector3i rv; rv.x = -x; rv.y = -y; rv.z = -z; return rv;
  }
  inline taVector3i operator -- () const {
    taVector3i rv = *this; rv.x--; rv.y--; rv.z--; return rv;
  }
  inline taVector3i operator ++ () const {
    taVector3i rv = *this; rv.x++; rv.y++; rv.z++; return rv;
  }

  inline bool operator < (const taVector3i& td) const { return (x < td.x) && (y < td.y) && (z < td.z); }
  inline bool operator > (const taVector3i& td) const { return (x > td.x) && (y > td.y) && (z > td.z); }
  inline bool operator <= (const taVector3i& td) const { return (x <= td.x) && (y <= td.y) && (z <= td.z); }
  inline bool operator >= (const taVector3i& td) const { return (x >= td.x) && (y >= td.y) && (z >= td.z); }
  inline bool operator == (const taVector3i& td) const { return (x == td.x) && (y == td.y) && (z == td.z); }
  inline bool operator != (const taVector3i& td) const { return (x != td.x) || (y != td.y) || (z != td.z); }

  inline bool operator <  (int td) const { return (x < td) && (y < td) && (z < td); }
  inline bool operator >  (int td) const { return (x > td) && (y > td) && (z > td); }
  inline bool operator <= (int td) const { return (x <= td) && (y <= td) && (z <= td); }
  inline bool operator >= (int td) const { return (x >= td) && (y >= td) && (z >= td); }
  inline bool operator == (int td) const { return (x == td) && (y == td) && (z == td); }
  inline bool operator != (int td) const { return (x != td) || (y != td) || (z != td); }

  inline bool OrEq(int td) const { return (x == td) || (y == td) || (z == td); }
  inline bool OrEq(const taVector3i& td) const { return (x == td.x) || (y == td.y) || (z == td.z); }

  inline int SqMag() const {            // squared magnitude of vector
    return x * x + y * y + z * z;
  }
  inline float Mag() const { return sqrt((float)SqMag()); }

  inline float  SqDist(const taVector3i& td) const { // squared distance between two vectors
    taVector3i dist = *this - td; return dist.SqMag();
  }
  inline float  Dist(const taVector3i& td) const { return sqrt(SqDist(td)); }
  inline int    Sum() const     { return x + y + z; }
  inline int    Product() const { return x * y * z; }

  static inline int Sgn(int val) { return (val >= 0) ? 1 : -1; }
  static inline int Absv(int val) { return (val >= 0) ? val : -val; }

  inline void   Invert()        { x = -x; y = -y; z = -z; }
  inline void   SumNorm()       { int mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void   Abs()           { x = Absv(x); y = Absv(y); z = Absv(z); }
  inline void   Min(taVector3i& td) { x = MIN(x,td.x); y = MIN(y,td.y); z = MIN(z,td.z); }
  inline void   Max(taVector3i& td) { x = MAX(x,td.x); y = MAX(y,td.y); z = MAX(z,td.z); }

  inline int MaxVal() const     { int mx = MAX(x, y); mx = MAX(mx, z); return mx; }
  inline int MinVal() const     { int mn = MIN(x, y); mn = MIN(mn, z); return mn; }

  inline String GetStr() const { return String(x) + ", " + String(y) + ", " + String(z); }

  bool          FitNinXY(int n);        // adjust x and y to fit x total elements

  void          SetGtEq(int n)  // set each to be greater than or equal to n
  { x = MAX(n, x);  y = MAX(n, y); z = MAX(n, z); }
  void          SetLtEq(int n)  // set each to be less than or equal to n
  { x = MIN(n, x);  y = MIN(n, y); z = MIN(n, z); }

  bool          WrapClip(bool wrap, const taVector3i& max) {
    bool wcxy = taVector2i::WrapClip(wrap, max); bool wcz = WrapClipOne(wrap, z, max.z);
    return wcxy || wcz;
  }  // wrap-around or clip coordinates within 0,0 - max range, true if out of range (clipped or more than half way around other side for wrap)
private:
  inline void   Copy_(const taVector3i& cp) { x = cp.x; y = cp.y; z = cp.z; }
  inline void   Initialize()            { x = y = z = 0; }
  inline void   Destroy()               { };
};

inline taVector3i operator + (int td, const taVector3i& v) {
  taVector3i rv; rv.x = td + v.x; rv.y = td + v.y; rv.z = td + v.z; return rv;
}
inline taVector3i operator - (int td, const taVector3i& v) {
  taVector3i rv; rv.x = td - v.x; rv.y = td - v.y; rv.z = td - v.z; return rv;
}
inline taVector3i operator * (int td, const taVector3i& v) {
  taVector3i rv; rv.x = td * v.x; rv.y = td * v.y; rv.z = td * v.z; return rv;
}
inline taVector3i operator / (int td, const taVector3i& v) {
  taVector3i rv; rv.x = td / v.x; rv.y = td / v.y; rv.z = td / v.z; return rv;
}

class TA_API PosVector3i : public taVector3i {
  // #NO_UPDATE_AFTER #INLINE #INLINE_DUMP positive-only value in 3D coordinate space
  INHERITED(taVector3i)
public:
  TA_BASEFUNS_LITE(PosVector3i);

  inline PosVector3i& operator=(const taVector3i& cp) { x = cp.x; y = cp.y; z = cp.z; UpdateAfterEdit(); return *this;}
  inline PosVector3i& operator=(int cp)  { x = cp; y = cp; z = cp; return *this;}
  inline PosVector3i& operator=(float cp) { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
  inline PosVector3i& operator=(double cp) { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
protected:
  void  UpdateAfterEdit_impl();
private:
  NOCOPY(PosVector3i)
  void  Initialize()    { }
  void  Destroy()       { };
};

class TA_API taVector2f : public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a value in 2D coordinate space
  INHERITED(taBase)
public:
  float         x;              // horizontal
  float         y;              // vertical

  inline void   SetXY(float xx, float yy)       { x = xx; y = yy; }
  inline void   GetXY(float& xx, float& yy)     { xx = x; yy = y; }

  taVector2f(float xx)                      { SetXY(xx, xx); }
  taVector2f(float xx, float yy)            { SetXY(xx, yy); }
  taVector2f(int xx)                        { SetXY(xx, xx); }
  taVector2f(int xx, int yy)                { SetXY(xx, yy); }
  taVector2f(const taVector2i& cp);  // conversion constructor
  TA_BASEFUNS_LITE(taVector2f)

  taVector2f& operator=(const taVector2i& cp);
  inline taVector2f& operator=(float cp)            { x = cp; y = cp; return *this;}
  inline taVector2f& operator=(double cp)           { x = (float)cp; y = (float)cp; return *this;}

  inline taVector2f& operator += (const taVector2f& td) { x += td.x; y += td.y; return *this;}
  inline taVector2f& operator -= (const taVector2f& td) { x -= td.x; y -= td.y; return *this;}
  inline taVector2f& operator *= (const taVector2f& td) { x *= td.x; y *= td.y; return *this;}
  inline taVector2f& operator /= (const taVector2f& td) { x /= td.x; y /= td.y; return *this;}

  inline taVector2f& operator += (float td) { x += td; y += td; return *this;}
  inline taVector2f& operator -= (float td) { x -= td; y -= td; return *this;}
  inline taVector2f& operator *= (float td) { x *= td; y *= td; return *this;}
  inline taVector2f& operator /= (float td) { x /= td; y /= td; return *this;}

  inline taVector2f operator + (const taVector2f& td) const {
    taVector2f rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  inline taVector2f operator - (const taVector2f& td) const {
    taVector2f rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  inline taVector2f operator * (const taVector2f& td) const {
    taVector2f rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  inline taVector2f operator / (const taVector2f& td) const {
    taVector2f rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  inline taVector2f operator + (float td) const {
    taVector2f rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  inline taVector2f operator - (float td) const {
    taVector2f rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  inline taVector2f operator * (float td) const {
    taVector2f rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  inline taVector2f operator / (float td) const {
    taVector2f rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  inline taVector2f operator - () const {
    taVector2f rv; rv.x = -x; rv.y = -y; return rv;
  }

  inline bool operator < (const taVector2f& td) const { return (x < td.x) && (y < td.y); }
  inline bool operator > (const taVector2f& td) const { return (x > td.x) && (y > td.y); }
  inline bool operator <= (const taVector2f& td) const { return (x <= td.x) && (y <= td.y); }
  inline bool operator >= (const taVector2f& td) const { return (x >= td.x) && (y >= td.y); }
  inline bool operator == (const taVector2f& td) const { return (x == td.x) && (y == td.y); }
  inline bool operator != (const taVector2f& td) const { return (x != td.x) || (y != td.y); }

  inline bool operator <  (float td) const { return (x < td) && (y < td); }
  inline bool operator >  (float td) const { return (x > td) && (y > td); }
  inline bool operator <= (float td) const { return (x <= td) && (y <= td); }
  inline bool operator >= (float td) const { return (x >= td) && (y >= td); }
  inline bool operator == (float td) const { return (x == td) && (y == td); }
  inline bool operator != (float td) const { return (x != td) || (y != td); }

  inline float SqMag() const    { return x * x + y * y; }
  // squared magnitude of vector
  inline float Mag() const      { return sqrt(SqMag()); }

  inline float  SqDist(const taVector2f& td) const { // squared distance between two vectors
    taVector2f dist = *this - td; return dist.Mag();
  }
  inline float  Dist(const taVector2f& td) const { return sqrt(SqDist(td)); }
  inline float  Sum() const     { return x + y; }
  inline float  Product() const { return x * y; }

  inline void   Invert()        { x = -x; y = -y; }
  inline void   MagNorm()       { float mg = Mag(); if(mg > 0.0) *this /= mg; }
  inline void   SumNorm()       { float mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void   Abs()           { x = fabs(x); y = fabs(y); }

  inline float MaxVal() const   { float mx = MAX(x, y); return mx; }
  inline float MinVal() const   { float mn = MIN(x, y); return mn; }

  inline String GetStr() const { return String(x) + ", " + String(y); }
private:
  inline void   Copy_(const taVector2f& cp) { x = cp.x; y = cp.y; }
  inline void   Initialize()                    { x = y = 0.0; }
  inline void   Destroy()                       { };
};

inline taVector2f operator + (float td, const taVector2f& v) {
  taVector2f rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
}
inline taVector2f operator - (float td, const taVector2f& v) {
  taVector2f rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
}
inline taVector2f operator * (float td, const taVector2f& v) {
  taVector2f rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
}
inline taVector2f operator / (float td, const taVector2f& v) {
  taVector2f rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
}

class TA_API taVector3f : public taVector2f {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP a real value in 3D coordinate space
  INHERITED(taVector2f)
public:
  float         z;              // depth

  inline void   SetXYZ(float xx, float yy, float zz) {
    x = xx; y = yy; z = zz;
  }
  inline void   GetXYZ(float& xx, float& yy, float& zz) {
    xx = x; yy = y; zz = z;
  }

  TA_BASEFUNS_LITE(taVector3f)
  taVector3f(float xx)                        { SetXYZ(xx, xx, xx); }
  taVector3f(float xx, float yy, float zz)    { SetXYZ(xx, yy, zz); }
  taVector3f(int xx)                          { SetXYZ(xx, xx, xx); }
  taVector3f(int xx, int yy, int zz)          { SetXYZ(xx, yy, zz); }
  taVector3f(const taVector3i& cp);      // conversion constructor

  taVector3f& operator=(const taVector3i& cp);
  inline taVector3f& operator=(float cp)              { x = cp; y = cp; z = cp; return *this;}
  inline taVector3f& operator=(double cp)             { x = (float)cp; y = (float)cp; z = (float)cp; return *this;}

  inline taVector3f&  operator += (const taVector3f& td)    { x += td.x; y += td.y; z += td.z; return *this;}
  inline taVector3f&  operator -= (const taVector3f& td)    { x -= td.x; y -= td.y; z -= td.z; return *this;}
  inline taVector3f&  operator *= (const taVector3f& td)    { x *= td.x; y *= td.y; z *= td.z; return *this;}
  inline taVector3f&  operator /= (const taVector3f& td)    { x /= td.x; y /= td.y; z /= td.z; return *this;}

  inline taVector3f&  operator += (float td)  { x += td; y += td; z += td; return *this;}
  inline taVector3f&  operator -= (float td)  { x -= td; y -= td; z -= td; return *this;}
  inline taVector3f&  operator *= (float td)  { x *= td; y *= td; z *= td; return *this;}
  inline taVector3f&  operator /= (float td)  { x /= td; y /= td; z /= td; return *this;}

  inline taVector3f operator + (const taVector3f& td) const {
    taVector3f rv; rv.x = x + td.x; rv.y = y + td.y; rv.z = z + td.z; return rv;
  }
  inline taVector3f operator - (const taVector3f& td) const {
    taVector3f rv; rv.x = x - td.x; rv.y = y - td.y; rv.z = z - td.z; return rv;
  }
  inline taVector3f operator * (const taVector3f& td) const {
    taVector3f rv; rv.x = x * td.x; rv.y = y * td.y; rv.z = z * td.z; return rv;
  }
  inline taVector3f operator / (const taVector3f& td) const {
    taVector3f rv; rv.x = x / td.x; rv.y = y / td.y; rv.z = z / td.z; return rv;
  }

  inline taVector3f operator + (float td) const {
    taVector3f rv; rv.x = x + td; rv.y = y + td; rv.z = z + td; return rv;
  }
  inline taVector3f operator - (float td) const {
    taVector3f rv; rv.x = x - td; rv.y = y - td; rv.z = z - td; return rv;
  }
  inline taVector3f operator * (float td) const {
    taVector3f rv; rv.x = x * td; rv.y = y * td; rv.z = z * td; return rv;
  }
  inline taVector3f operator / (float td) const {
    taVector3f rv; rv.x = x / td; rv.y = y / td; rv.z = z / td; return rv;
  }

  inline taVector3f operator - () const {
    taVector3f rv; rv.x = -x; rv.y = -y; rv.z = -z; return rv;
  }

  inline bool operator < (const taVector3f& td) const { return (x < td.x) && (y < td.y) && (z < td.z); }
  inline bool operator > (const taVector3f& td) const { return (x > td.x) && (y > td.y) && (z > td.z); }
  inline bool operator <= (const taVector3f& td) const { return (x <= td.x) && (y <= td.y) && (z <= td.z); }
  inline bool operator >= (const taVector3f& td) const { return (x >= td.x) && (y >= td.y) && (z >= td.z); }
  inline bool operator == (const taVector3f& td) const { return (x == td.x) && (y == td.y) && (z == td.z); }
  inline bool operator != (const taVector3f& td) const { return (x != td.x) || (y != td.y) || (z != td.z); }

  inline bool operator <  (float td) const { return (x < td) && (y < td) && (z < td); }
  inline bool operator >  (float td) const { return (x > td) && (y > td) && (z > td); }
  inline bool operator <= (float td) const { return (x <= td) && (y <= td) && (z <= td); }
  inline bool operator >= (float td) const { return (x >= td) && (y >= td) && (z >= td); }
  inline bool operator == (float td) const { return (x == td) && (y == td) && (z == td); }
  inline bool operator != (float td) const { return (x != td) || (y != td) || (z != td); }


  bool          Equals(float v)
    {return ((x == v) && (y == v) && (z == v));}
  bool          Equals(float xx, float yy, float zz)
    {return ((x == xx) && (y == yy) && (z == zz));}
  inline float SqMag() const
  { return x * x + y * y + z * z; }
  // #CAT_Math squared magnitude of vector
  inline float Mag() const { return sqrt(SqMag()); }

  inline float  SqDist(const taVector3f& td) const { // squared distance between two vectors
    taVector3f dist = *this - td; return dist.SqMag();
  }
  inline float  Dist(const taVector3f& td) const {
    taVector3f dist = *this - td; return dist.Mag();
  }
  inline float  Sum() const     { return x + y + z; }
  inline float  Product() const { return x * y * z; }

  inline void   Invert()        { x = -x; y = -y; z = -z; }
  inline void   MagNorm()       { float mg = Mag(); if(mg > 0.0) *this /= mg; }
  inline void   SumNorm()       { float mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void   Abs()           { x = fabs(x); y = fabs(y); z = fabs(z); }

  inline float MaxVal() const   { float mx = MAX(x, y); mx = MAX(mx, z); return mx; }
  inline float MinVal() const   { float mn = MIN(x, y); mn = MIN(mn, z); return mn; }

  static inline float Sgn(float val) { return (val >= 0.0) ? 1.0 : -1.0; }

  inline String GetStr() const { return String(x) + ", " + String(y) + ", " + String(z); }
#ifdef TA_GUI
  operator iVec3f() const {return iVec3f(x, y, z);}
#endif
private:
  inline void   Initialize()                    { z = 0.0; }
  inline void   Destroy()                       { };
  inline void   Copy_(const taVector3f& cp)   { z = cp.z; }
};

inline taVector3f operator + (float td, const taVector3f& v) {
  taVector3f rv; rv.x = td + v.x; rv.y = td + v.y; rv.z = td + v.z; return rv;
}
inline taVector3f operator - (float td, const taVector3f& v) {
  taVector3f rv; rv.x = td - v.x; rv.y = td - v.y; rv.z = td - v.z; return rv;
}
inline taVector3f operator * (float td, const taVector3f& v) {
  taVector3f rv; rv.x = td * v.x; rv.y = td * v.y; rv.z = td * v.z; return rv;
}
inline taVector3f operator / (float td, const taVector3f& v) {
  taVector3f rv; rv.x = td / v.x; rv.y = td / v.y; rv.z = td / v.z; return rv;
}

class TA_API taAxisAngle: public taVector3f {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP 3-d rotation data, xyz specify the rotation axis
  INHERITED(taVector3f)
public:
  float         rot; // rotation angle, in radians

  inline void   SetXYZR(float xx, float yy, float zz, float rr) {
    x = xx; y = yy; z = zz; rot = rr;
  }
  inline void   GetXYZR(float& xx, float& yy, float& zz, float& rr) {
    xx = x; yy = y; zz = z; rr = rot;
  }

  void	FromEuler(float theta_x, float theta_y, float theta_z);
  // #CAT_Rotation #BUTTON convert from Euler angles

  void	RotateAxis(float x_axis, float y_axis, float z_axis, float rot_ang);
  // #CAT_Rotation #BUTTON rotate this rotation by given axis rotation parameters
  void	RotateEuler(float theta_x, float theta_y, float theta_z);
  // #CAT_Rotation #BUTTON rotate this rotation by given rotation parameters
  void	RotateXYZ(float& x, float& y, float& z);
  // #CAT_Rotation rotate x,y,z vector according to the current rotation parameters
  void	RotateVec(taVector3f& vec);
  // #CAT_Rotation rotate vector according to the current rotation parameters

  String        GetStr() const {return taVector3f::GetStr() + ", " + String(rot); }

  taAxisAngle(float xx, float yy, float zz, float rr) {SetXYZR(xx, yy, zz, rr); }
  taAxisAngle(int xx, int yy, int zz, float rr)       {SetXYZR(xx, yy, zz, rr); }

  taAxisAngle& operator=(const taQuaternion& cp);

  TA_BASEFUNS_LITE(taAxisAngle);
private:
  void                  Copy_(const taAxisAngle& cp)  {rot = cp.rot;}
  void                  Initialize() { z = 1.0f; rot = 0.0f;}
  void                  Destroy() {}
};

// note: following is informed by http://willperone.net/Code/quaternion.php and ODE code

class TA_API taQuaternion: public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a quaternion representation of 3D rotation 
  INHERITED(taBase)
public:
  float         s; // scalar, real component
  float         x; // imaginary x component
  float         y; // imaginary y component
  float         z; // imaginary z component

  inline void   SetSXYZ(float ss, float xx, float yy, float zz)
  { s = ss; x = xx; y = yy; z = zz; }
  // #CAT_Quaternion set scalar and xyz imaginary values
  inline void   GetSXYZ(float& ss, float& xx, float& yy, float& zz)
  { ss = s; xx = x; yy = y; zz = z; }
  // #CAT_Quaternion get scalar and xyz imaginary values

  inline void	FromEuler(float theta_x, float theta_y, float theta_z) {
    float cos_z_2 = cosf(0.5f*theta_z);
    float cos_y_2 = cosf(0.5f*theta_y);
    float cos_x_2 = cosf(0.5f*theta_x);

    float sin_z_2 = sinf(0.5f*theta_z);
    float sin_y_2 = sinf(0.5f*theta_y);
    float sin_x_2 = sinf(0.5f*theta_x);

    s = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
    x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
    y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
    z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
  }
  // #CAT_Quaternion #BUTTON set quaternion from three Euler angles
  inline void	FromEulerVec(const taVector3f& euler) {
    FromEuler(euler.x, euler.y, euler.z);
  }
  // #CAT_Quaternion #BUTTON set quaternion from three Euler angles in vector form

  inline void	ToEuler(float& theta_z, float& theta_y, float& theta_x, bool homogenous=true) const {
    float sqs = s*s;    
    float sqx = x*x;    
    float sqy = y*y;    
    float sqz = z*z;    
    if(homogenous) {
      theta_x = atan2f(2.0f * (x*y + z*s), sqx - sqy - sqz + sqs);    		
      theta_y = asinf(-2.0f * (x*z - y*s));
      theta_z = atan2f(2.0f * (y*z + x*s), -sqx - sqy + sqz + sqs);    
    }
    else {
      theta_x = atan2f(2.0f * (z*y + x*s), 1.0f - 2.0f*(sqx + sqy));
      theta_y = asinf(-2.0f * (x*z - y*s));
      theta_z = atan2f(2.0f * (x*y + z*s), 1.0f - 2.0f*(sqy + sqz));
    }
  }
  // #CAT_Quaternion return three Euler angles from quaternion
  inline void	ToEulerVec(taVector3f& euler, bool homogenous=true) const {
    ToEuler(euler.x, euler.y, euler.z);
  }
  // #CAT_Quaternion return three Euler angles from quaternion into a 3D vector

  inline void	FromAxisAngle(const taAxisAngle& axa) {
    float ang2 = axa.rot*0.5f; float sinang2 = sinf(ang2);
    s = cosf(ang2); x = axa.x * sinang2; y = axa.y * sinang2; z = axa.z * sinang2;
  }
  // #CAT_Quaternion set quaternion from taAxisAngle (axis + angle) value
  inline void	FromAxisAngle(float x_axis, float y_axis, float z_axis, float rot_ang) {
    float ang2 = rot_ang*0.5f; float sinang2 = sinf(ang2);
    s = cosf(ang2); x = x_axis * sinang2; y = y_axis * sinang2; z = z_axis * sinang2;
  }
  // #CAT_Quaternion set quaternion from taAxisAngle (axis + angle) value
  inline void	ToAxisAngle(taAxisAngle& axa) const {
    axa.rot = acosf(s);
    float sinangi = sinf(axa.rot);
    if(sinangi == 0.0f) {	// can't convert
      axa.rot = 0.0f;
      axa.x = 0.0f; axa.y = 0.0f; axa.z = 1.0f;
      return;
    }
    sinangi = 1.0f / sinangi;
    axa.x = x * sinangi; axa.y = y * sinangi; axa.z = z * sinangi;  axa.rot *= 2.0f;
  }
  // #CAT_Quaternion set taAxisAngle from this quaternion

  inline void	ToMatrix(float_Matrix& mat) const {
    float mag = Mag();
    if(mag < 0.9999 || mag > 1.0001) { taMisc::Error("taQuaternion::ToMatrix -- must be normalized (Mag == 1.0), mag is:", 
						     String(mag)); return; }
    mat.SetGeom(2,3,3);

    // note: not 100% sure about r,c = x,y coordinates on this matrix -- might need to flip..

    mat.Set(1.0f - 2.0f*(y*y+z*z), 0,0);
    mat.Set(2.0f*(x*y-s*z),0,1);
    mat.Set(2.0f*(x*z+s*y),0,2);

    mat.Set(2.0f*(x*y+s*z),1,0);
    mat.Set(1.0f-2.0f*(x*x+z*z), 1,1);
    mat.Set(2.0f*(y*z-s*x),1,2);
    
    mat.Set(2.0f*(x*z-s*y),2,0);
    mat.Set(2.0f*(y*z+s*x),2,1);
    mat.Set(1.0f-2.0f*(x*x+y*y),2,2);
  }

  void	RotateAxis(float x_axis, float y_axis, float z_axis, float rot_ang) {
    taQuaternion q; q.FromAxisAngle(x_axis, y_axis, z_axis, rot_ang);
    q.Normalize();
    *this *= q;
  }
  // #CAT_Quaternion #BUTTON rotate this rotation by given axis rotation parameters
  void	RotateEuler(float theta_x, float theta_y, float theta_z) {
    taQuaternion q; q.FromEuler(theta_x, theta_y, theta_z);
    q.Normalize();
    *this *= q;
  }
  // #CAT_Quaternion #BUTTON rotate this rotation by given rotation parameters

  void	RotateXYZ(float& x, float& y, float& z) {
    taQuaternion vecq(0, x, y, z); // convert vec to quat
    taQuaternion conj = this->Conjugated();
    taQuaternion rotv = *this * vecq * conj;
    x = rotv.x; y = rotv.y; z = rotv.z;
  }
  // #CAT_Quaternion #BUTTON rotate x,y,z vector according to current rotation parameters
  void	RotateVec(taVector3f& vec) {
    RotateXYZ(vec.x, vec.y, vec.z);
  }
  // #CAT_Quaternion #BUTTON rotate vector according to current rotation parameters

  inline float SqMag() const {
    return s*s + x*x + y*y + z*z;
  }
  // #CAT_Quaternion squared magnitude (length) of quaternion -- sum of squared components
  inline float Mag() const {
    return sqrtf(SqMag());
  }
  // #CAT_Quaternion magnitude (length) of quaternion -- square root of sum of squared components
  inline void Conjugate() {
    x = -x; y = -y; z = -z;
  }
  // #CAT_Quaternion convert this quaternion into its conjugate (imaginary x,y,z values flipped to their negatives)
  inline taQuaternion Conjugated() const {
    taQuaternion rv(*this); rv.Conjugate(); return rv;
  }
  // #CAT_Quaternion return conjugate version of this quaternion (imaginary x,y,z values flipped to their negatives)
  inline void Invert() {
    Conjugate(); *this /= SqMag(); 
  }
  // #CAT_Quaternion invert this quaternion (conjugate and divide by squared magnitude)
  inline taQuaternion Inverted() const {
    taQuaternion rv(*this); rv.Invert(); return rv;
  }
  // #CAT_Quaternion convert this quaternion into its conjugate (imaginary x,y,z values flipped to their negatives)
  inline void Normalize() {
    *this /= Mag(); 
  }
  // #CAT_Quaternion normalize this quaternion (divide by magnitude or length)
  inline taQuaternion Normalized() const {
    taQuaternion rv(*this); rv.Normalize(); return rv;
  }
  // #CAT_Quaternion return normalized version of this quaternion (divide by magnitude or length)

  inline taQuaternion operator + (const taQuaternion& q) const {
    taQuaternion rv; rv.s = s + q.s; rv.x = x + q.x; rv.y = y + q.y; rv.z = z + q.z; return rv;
  }
  inline taQuaternion operator - (const taQuaternion& q) const {
    taQuaternion rv; rv.s = s - q.s; rv.x = x - q.x; rv.y = y - q.y; rv.z = z - q.z; return rv;
  }
  inline taQuaternion operator * (const taQuaternion& q) const {
    taQuaternion rv; rv.s = s * q.s - (x * q.x + y * q.y + z * q.z);
    rv.x = y*q.z - z*q.y + s*q.x + x*q.s;
    rv.y = z*q.x - x*q.z + s*q.y + y*q.s;
    rv.z = x*q.y - y*q.x + s*q.z + z*q.s; return rv;
  }
  inline taQuaternion operator / (const taQuaternion& q) const {
    taQuaternion inv(q);
    inv.Invert(); // 1/q
    return *this * inv;
  }

  inline taQuaternion operator * (float scale) const {
    taQuaternion rv; rv.s = s * scale; rv.x = x * scale; rv.y = y * scale; rv.z = z * scale; return rv;
  }
  inline taQuaternion operator / (float scale) const {
    taQuaternion rv;
    if(scale != 0.0f) { rv.s = s / scale; rv.x = x / scale; rv.y = y / scale; rv.z = z / scale; }
    else	      { taMisc::Error("Quaternion -- division by 0 scalar"); }
    return rv;
  }

  inline taQuaternion operator - () const {
    taQuaternion rv; rv.s = -s; rv.x = -x; rv.y = -y; rv.z = -z; return rv;
  }

  inline taQuaternion& operator += (const taQuaternion& q) {
    s += q.s; x += q.x; y += q.y; z += q.z; return *this;
  }
  inline taQuaternion& operator -= (const taQuaternion& q) {
    s -= q.s; x -= q.x; y -= q.y; z -= q.z; return *this;
  }
  inline taQuaternion& operator *= (const taQuaternion& q) {
    float ox = x; float oy = y; float oz = z;
    x = oy*q.z - oz*q.y + s*q.x + ox*q.s;
    y = oz*q.x - ox*q.z + s*q.y + oy*q.s;
    z = ox*q.y - oy*q.x + s*q.z + oz*q.s;
    s = s*q.s - (ox * q.x + oy * q.y + oz * q.z); return *this;
  }
  inline taQuaternion& operator /= (const taQuaternion& q) {
    taQuaternion inv(q);
    inv.Invert(); // 1/q
    return *this *= inv;
  }

  inline taQuaternion& operator *= (float scale) {
    s *= scale; x *= scale; y *= scale; z *= scale; return *this;
  }
  inline taQuaternion& operator /= (float scale) {
    if(scale != 0.0f) { s /= scale; x /= scale; y /= scale; z /= scale; }
    else	      { taMisc::Error("Quaternion -- division by 0 scalar"); }
    return *this;
  }

#ifndef __MAKETA__
  void		ToODE(dQuaternion dq) const
  { dq[0] = s; dq[1] = x; dq[2] = y; dq[3] = z; }
  void		FromODE(const dQuaternion dq)
  { s = dq[0]; x = dq[1]; y = dq[2]; z = dq[3]; }
#endif

  String        GetStr() const { return String(s) + ", " + String(x) + ", " + String(y) + ", " + String(z);  }

  taQuaternion(float ss, float xx, float yy, float zz)
  { SetSXYZ(ss, xx, yy, zz); }
  taQuaternion(float theta_x, float theta_y, float theta_z)
  { FromEuler(theta_x, theta_y, theta_z); }
  taQuaternion(const taVector3f& euler)
  { FromEulerVec(euler); }

  taQuaternion& operator=(const taAxisAngle& cp)
  { FromAxisAngle(cp); return *this; }
  taQuaternion& operator=(const taVector3f& cp)
  { FromEulerVec(cp); return *this; }

  TA_BASEFUNS_LITE(taQuaternion);
private:
  void                  Copy_(const taQuaternion& cp)  { s = cp.s; x = cp.x; y = cp.y; z = cp.z; }
  void                  Initialize() { s = 1.0f; x = y = z = 0.0f; }
  void                  Destroy() {}
};

class TA_API taTransform: public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math 3-d transformation data; applied in order: s, r, t
  INHERITED(taBase)
public:
  taVector3f          scale; // scale factors, in x, y, and z
  taAxisAngle         rotate; // rotation
  taVector3f          translate; // translate, in x, y, and z

  TA_BASEFUNS_LITE(taTransform);
#ifdef TA_USE_INVENTOR
  void                  CopyTo(SoTransform* txfm); // #IGNORE txfers values to an inventor txfm -- note, does a transfer, not an accumulate
#endif
private:
  void                  Copy_(const taTransform& cp)
    {scale.Copy(cp.scale); rotate.Copy(cp.rotate); translate.Copy(cp.translate);}
  void                  Initialize() {scale = 1.0f;}
  void                  Destroy() {}
};

class TA_API ValIdx : public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a float value and an index: very useful for sorting!
  INHERITED(taBase)
public:
  float         val;            // value
  int           idx;            // index

  inline void   SetValIdx(float v, int i)       { val = v; idx = i; }
  inline void   GetValIdx(float& v, int& i)     { v = val; i = idx; }

  inline void   Initialize()                    { val = 0.0; idx = 0; }
  inline void   Destroy()                       { };
  void  Copy(const ValIdx& cp)  { val = cp.val; idx = cp.idx; }
  inline bool Copy(const taBase* cp) {return taBase::Copy(cp);}

  ValIdx()                              { Initialize(); }
  ValIdx(const ValIdx& cp)              { Copy(cp); }
  ValIdx(float v, int i)                { SetValIdx(v, i); }
  ValIdx(const String& str)             { val = (float)str; }
  ~ValIdx()                             { };
  taBase* Clone() const                 { return new ValIdx(*this); }
  void  UnSafeCopy(const taBase* cp) {
    if(cp->InheritsFrom(&TA_ValIdx)) Copy(*((ValIdx*)cp));
    if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);
  }
  void  CastCopyTo(taBase* cp) const    { ValIdx& rf = *((ValIdx*)cp); rf.Copy(*this); }
  taBase* MakeToken() const             { return (taBase*)(new ValIdx); }
  taBase* MakeTokenAry(int no) const    { return (taBase*)(new ValIdx[no]); }
  TypeDef* GetTypeDef() const           { return &TA_ValIdx; }
  static TypeDef* StatTypeDef(int)      { return &TA_ValIdx; }

  inline operator String () const { return String(val); }

  inline void operator=(const ValIdx& cp) { Copy(cp); }
  inline void operator=(float cp)               { val = cp; idx = -1; }

  inline void operator += (const ValIdx& td)    { val += td.val; }
  inline void operator -= (const ValIdx& td)    { val -= td.val; }
  inline void operator *= (const ValIdx& td)    { val *= td.val; }
  inline void operator /= (const ValIdx& td)    { val /= td.val; }

  inline void operator += (float td)    { val += td; }
  inline void operator -= (float td)    { val -= td; }
  inline void operator *= (float td)    { val *= td; }
  inline void operator /= (float td)    { val /= td; }

  inline ValIdx operator + (const ValIdx& td) const {
    ValIdx rv; rv.val = val + td.val; rv.idx = idx; return rv;
  }
  inline ValIdx operator - (const ValIdx& td) const {
    ValIdx rv; rv.val = val - td.val; rv.idx = idx; return rv;
  }
  inline ValIdx operator * (const ValIdx& td) const {
    ValIdx rv; rv.val = val * td.val; rv.idx = idx; return rv;
  }
  inline ValIdx operator / (const ValIdx& td) const {
    ValIdx rv; rv.val = val / td.val; rv.idx = idx; return rv;
  }

  inline ValIdx operator + (float td) const {
    ValIdx rv; rv.val = val + td; rv.idx = idx; return rv;
  }
  inline ValIdx operator - (float td) const {
    ValIdx rv; rv.val = val - td; rv.idx = idx; return rv;
  }
  inline ValIdx operator * (float td) const {
    ValIdx rv; rv.val = val * td; rv.idx = idx; return rv;
  }
  inline ValIdx operator / (float td) const {
    ValIdx rv; rv.val = val / td; rv.idx = idx; return rv;
  }

  inline ValIdx operator - () const {
    ValIdx rv; rv.val = -val; rv.idx = idx; return rv;
  }

  inline bool operator <  (const ValIdx& td) const { return (val <  td.val); }
  inline bool operator >  (const ValIdx& td) const { return (val >  td.val); }
  inline bool operator <= (const ValIdx& td) const { return (val <= td.val); }
  inline bool operator >= (const ValIdx& td) const { return (val >= td.val); }
  inline bool operator == (const ValIdx& td) const { return (val == td.val); }
  inline bool operator != (const ValIdx& td) const { return (val != td.val); }

  inline bool operator <  (float td) const { return (val <  td); }
  inline bool operator >  (float td) const { return (val >  td); }
  inline bool operator <= (float td) const { return (val <= td); }
  inline bool operator >= (float td) const { return (val >= td); }
  inline bool operator == (float td) const { return (val == td); }
  inline bool operator != (float td) const { return (val != td); }

  inline String GetStr() const { return String(val) + ":" + String(idx); }
};

inline ValIdx operator + (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td + v.val; rv.idx = v.idx; return rv;
}
inline ValIdx operator - (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td - v.val; rv.idx = v.idx; return rv;
}
inline ValIdx operator * (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td * v.val; rv.idx = v.idx; return rv;
}
inline ValIdx operator / (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td / v.val; rv.idx = v.idx; return rv;
}

class TA_API ValIdx_Array : public taArray<ValIdx> {
  // #NO_UPDATE_AFTER ##CAT_Math array of value & index items
INHERITED(taArray<ValIdx>)
public:
  STATIC_CONST ValIdx blank; // #HIDDEN #READ_ONLY
                                                                                 // NULL; }
  TA_BASEFUNS_NOCOPY(ValIdx_Array);
  TA_ARRAY_FUNS(ValIdx_Array,ValIdx);
protected:
  override Variant      El_GetVar_(const void* itm) const
  { return (Variant)(((ValIdx*)itm)->val); }
  String        El_GetStr_(const void* it) const { return (String)((ValIdx*)it); } // #IGNORE
  void          El_SetFmStr_(void* it, const String& val)
  { ((ValIdx*)it)->val = (float)val; } // #IGNORE
private:
  void Initialize()     { };
  void Destroy()        { };
};

#endif // tdgeometry_h
