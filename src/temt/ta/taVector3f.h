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

#ifndef taVector3f_h
#define taVector3f_h 1

// parent includes:
#include <taVector2f>

// member includes:
#ifdef TA_GUI
#include "igeometry.h"
#endif

// declare all other types mentioned but not required to include:
class taMatrix; // 
class taVector3i; //


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

  void	ToMatrix(taMatrix& mat) const;
  // set values to a 1d matrix object (can be any type of matrix object)
  void	FromMatrix(taMatrix& mat);
  // set values from a matrix object (can be any type of matrix object)

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

typedef taVector3f FloatTDCoord;	// #INSTANCE obsolete version of taVector3f

#endif // taVector3f_h
