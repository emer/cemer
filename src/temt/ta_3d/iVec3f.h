// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef iVec3f_h
#define iVec3f_h 1

#include "ta_def.h"

/* Notes:
   1. all classes below are value-semantics, and use the implicit copy and = operators.
   2. MAKETA cannot boggle the : class(): x(), y() {} form of constructor initialialization,
      so they have been moved to the .cc file.
   3. DO NOT add any virtual methods to these value-semantic classes
*/

#ifdef TA_QT3D
class QVector3D;                // #IGNORE
#else // TA_QT3D
class SbVec3f;          // #IGNORE
#endif // TA_QT3D

taTypeDef_Of(iVec3f);

class TA_API iVec3f { // #NO_TOKENS #INSTANCE #EDIT_INLINE 3d vector, for things like points and sizes
public:
  float         x;
  float         y;
  float         z;

  bool          isEqual(float x_, float y_, float z_) const {return ((x == x_) && (y == y_) && (z == z_));}
  bool          isUnity() const { return ((x == 1.0f) && (y == 1.0f) && (z == 1.0f));}
  bool          isZero() const { return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f));}
  void          setValue(float x_, float y_, float z_)  {x = x_; y = y_; z = z_;}

  iVec3f() {x = 0; y = 0; z = 0;}
  iVec3f(float val) {x = val; y = val; z = val;}
  iVec3f(float x_, float y_, float z_) {x = x_; y = y_; z = z_;}
  iVec3f(const iVec3f& val) {x = val.x; y = val.y; z = val.z;}

  iVec3f&       operator=(float val) {x = val; y = val; z = val; return *this;}
  iVec3f&       operator=(const iVec3f& val) {x = val.x; y = val.y; z = val.z; return *this;}

  inline iVec3f&  operator += (const iVec3f& td)    { x += td.x; y += td.y; z += td.z; return *this;}
  inline iVec3f&  operator -= (const iVec3f& td)    { x -= td.x; y -= td.y; z -= td.z; return *this;}
  inline iVec3f&  operator *= (const iVec3f& td)    { x *= td.x; y *= td.y; z *= td.z; return *this;}
  inline iVec3f&  operator /= (const iVec3f& td)    { x /= td.x; y /= td.y; z /= td.z; return *this;}

  inline iVec3f&  operator += (float td)  { x += td; y += td; z += td; return *this;}
  inline iVec3f&  operator -= (float td)  { x -= td; y -= td; z -= td; return *this;}
  inline iVec3f&  operator *= (float td)  { x *= td; y *= td; z *= td; return *this;}
  inline iVec3f&  operator /= (float td)  { x /= td; y /= td; z /= td; return *this;}

  inline iVec3f operator + (const iVec3f& td) const {
    iVec3f rv; rv.x = x + td.x; rv.y = y + td.y; rv.z = z + td.z; return rv;
  }
  inline iVec3f operator - (const iVec3f& td) const {
    iVec3f rv; rv.x = x - td.x; rv.y = y - td.y; rv.z = z - td.z; return rv;
  }
  inline iVec3f operator * (const iVec3f& td) const {
    iVec3f rv; rv.x = x * td.x; rv.y = y * td.y; rv.z = z * td.z; return rv;
  }
  inline iVec3f operator / (const iVec3f& td) const {
    iVec3f rv; rv.x = x / td.x; rv.y = y / td.y; rv.z = z / td.z; return rv;
  }

  inline iVec3f operator + (float td) const {
    iVec3f rv; rv.x = x + td; rv.y = y + td; rv.z = z + td; return rv;
  }
  inline iVec3f operator - (float td) const {
    iVec3f rv; rv.x = x - td; rv.y = y - td; rv.z = z - td; return rv;
  }
  inline iVec3f operator * (float td) const {
    iVec3f rv; rv.x = x * td; rv.y = y * td; rv.z = z * td; return rv;
  }
  inline iVec3f operator / (float td) const {
    iVec3f rv; rv.x = x / td; rv.y = y / td; rv.z = z / td; return rv;
  }

  inline iVec3f operator - () const {
    iVec3f rv; rv.x = -x; rv.y = -y; rv.z = -z; return rv;
  }

#ifdef TA_QT3D
  iVec3f(const QVector3D& src);
  iVec3f&       operator=(const QVector3D& src);
  operator QVector3D() const;
#else // TA_QT3D
  iVec3f(const SbVec3f& src);
  iVec3f&       operator=(const SbVec3f& src);
  operator SbVec3f() const;
#endif // TA_QT3D
};

inline iVec3f operator + (float td, const iVec3f& v) {
  iVec3f rv; rv.x = td + v.x; rv.y = td + v.y; rv.z = td + v.z; return rv;
}
inline iVec3f operator - (float td, const iVec3f& v) {
  iVec3f rv; rv.x = td - v.x; rv.y = td - v.y; rv.z = td - v.z; return rv;
}
inline iVec3f operator * (float td, const iVec3f& v) {
  iVec3f rv; rv.x = td * v.x; rv.y = td * v.y; rv.z = td * v.z; return rv;
}
inline iVec3f operator / (float td, const iVec3f& v) {
  iVec3f rv; rv.x = td / v.x; rv.y = td / v.y; rv.z = td / v.z; return rv;
}

#endif // iVec3f_h
