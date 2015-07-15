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

#ifndef taVector2f_h
#define taVector2f_h 1

// parent includes:
#include <taBase>

#include <cmath>

// member includes:

// declare all other types mentioned but not required to include:
class taMatrix; // 
class taVector2i; //

taTypeDef_Of(taVector2f);

class TA_API taVector2f : public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a value in 2D coordinate space
  INHERITED(taBase)
public:
  float         x;              // horizontal
  float         y;              // vertical

  inline void   SetXY(float xx, float yy)       { x = xx; y = yy; }
  inline void   GetXY(float& xx, float& yy)     { xx = x; yy = y; }

  void	ToMatrix(taMatrix& mat) const;
  // set values to a 1d matrix object (can be any type of matrix object)
  void	FromMatrix(taMatrix& mat);
  // set values from a matrix object (can be any type of matrix object)

  taVector2f(float xx)                      { SetXY(xx, xx); }
  taVector2f(float xx, float yy)            { SetXY(xx, yy); }
  taVector2f(int xx)                        { SetXY(xx, xx); }
  taVector2f(int xx, int yy)                { SetXY(xx, yy); }
  taVector2f(const taVector2i& cp);  // conversion constructor
  TA_BASEFUNS_LITE(taVector2f);

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

typedef taVector2f FloatTwoDCoord;	// #INSTANCE obsolete version of taVector2f

#endif // taVector2f_h
