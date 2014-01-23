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

#ifndef taVector3i_h
#define taVector3i_h 1

// parent includes:
#include <taVector2i>

// member includes:

// declare all other types mentioned but not required to include:
class MatrixGeom; // 
class MatrixIndex; // 
class taMatrix; // 
class taVector3f; //

taTypeDef_Of(taVector3i);

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
  void CopyToMatrixGeom(MatrixGeom& geom) override;
  void CopyToMatrixIndex(MatrixIndex& idx) override;

  void	ToMatrix(taMatrix& mat) const;
  // set values to a 1d matrix object (can be any type of matrix object)
  void	FromMatrix(taMatrix& mat);
  // set values from a matrix object (can be any type of matrix object)

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

  inline void   Wrap(const taVector3i& max)
  { taVector2i::Wrap(max); WrapOne(z, max.z); }
  // wrap-around coordinates within 0,0,0 - max range

  inline bool    Clip(const taVector3i& max)
  { bool wcxy = taVector2i::Clip(max); bool wcz = ClipOne(z, max.z);
    return wcxy || wcz; }
  // clip coordinates within 0,0,0 - max range, true if out of range

  inline bool   WrapClip(bool wrap, const taVector3i& max)
  { if(wrap) { Wrap(max); return false; } return Clip(max); }
  // wrap-around or clip coordinates within 0,0,0 - max range, true if clipped out of range -- for performance, it is better to use separate code for wrap and clip cases

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

typedef taVector3i TDCoord;		// #INSTANCE obsolete version of Vector3i

#endif // taVector3i_h
