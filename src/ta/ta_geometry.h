// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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
#include "ta_TA_type.h"

#ifdef TA_GUI
#include "igeometry.h"
#endif

#include <math.h>

class TwoDCoord;
class PosTwoDCoord;
class TDCoord;
class PosTDCoord;
class FloatTwoDCoord;
class FloatTDCoord;

class TA_API TwoDCoord : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a value in 2D coordinate space
  INHERITED(taBase)
public:
  int 		x;  		// horizontal
  int 		y;  		// vertical

  bool		isZero() {return ((x == 0) && (y == 0));}
  inline void 	SetXY(int xx, int yy) { x = xx; y = yy;  }
  inline void 	SetXY(float xx, float yy) { x = (int)xx; y = (int)yy; }
  inline void 	GetXY(float& xx, float& yy) { xx = (float)x; yy = (float)y; }
  virtual void	CopyToMatrixGeom(MatrixGeom& geom);

  inline void 	Initialize() 		{ x = y = 0; }
  inline void 	Destroy()		{ };
  inline void 	Copy(const TwoDCoord& cp) { x = cp.x; y = cp.y; }

  TwoDCoord() 				{ Initialize(); }
  TwoDCoord(const TwoDCoord& cp) 	{ Copy(cp); }
  TwoDCoord(int xx) 			{ SetXY(xx, xx); }
  TwoDCoord(int xx, int yy) 		{ SetXY(xx, yy); }
  TwoDCoord(float xx, float yy) 	{ SetXY(xx, yy); }
  TwoDCoord(const FloatTwoDCoord& cp);	// conversion constructor
  ~TwoDCoord() 				{ };
  TAPtr Clone() 			{ return new TwoDCoord(*this); }
  void  UnSafeCopy(TAPtr cp);
  void  CastCopyTo(TAPtr cp) 	{ TwoDCoord& rf = *((TwoDCoord*)cp); rf.Copy(*this); }
  TAPtr MakeToken()			{ return (TAPtr)(new TwoDCoord); }
  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new TwoDCoord[no]); }
  TypeDef* GetTypeDef() const 		{ return StatTypeDef(0); }
  static TypeDef* StatTypeDef(int);

  inline TwoDCoord& operator=(const TwoDCoord& cp) 	{ Copy(cp); return *this;}
  TwoDCoord& operator=(const FloatTwoDCoord& cp);
  inline TwoDCoord& operator=(int cp) 		{ x = cp; y = cp; return *this;}
  inline TwoDCoord& operator=(float cp) 		{ x = (int)cp; y = (int)cp; return *this;}
  inline TwoDCoord& operator=(double cp) 		{ x = (int)cp; y = (int)cp; return *this;}

  inline TwoDCoord& 	operator += (const TwoDCoord& td)	{ x += td.x; y += td.y; return *this;}
  inline TwoDCoord& 	operator -= (const TwoDCoord& td)	{ x -= td.x; y -= td.y; return *this;}
  inline TwoDCoord& 	operator *= (const TwoDCoord& td)	{ x *= td.x; y *= td.y; return *this;}
  inline TwoDCoord& 	operator /= (const TwoDCoord& td)	{ x /= td.x; y /= td.y; return *this;}
  inline TwoDCoord& 	operator %= (const TwoDCoord& td)	{ x %= td.x; y %= td.y; return *this;}

  inline TwoDCoord& 	operator += (int td)	{ x += td; y += td; return *this;}
  inline TwoDCoord& 	operator -= (int td)	{ x -= td; y -= td; return *this;}
  inline TwoDCoord& 	operator *= (int td)	{ x *= td; y *= td; return *this;}
  inline TwoDCoord& 	operator /= (int td)	{ x /= td; y /= td; return *this;}
  inline TwoDCoord& 	operator %= (int td)	{ x %= td; y %= td; return *this;}

  inline TwoDCoord operator + (const TwoDCoord& td) const {
    TwoDCoord rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  inline TwoDCoord operator - (const TwoDCoord& td) const {
    TwoDCoord rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  inline TwoDCoord operator * (const TwoDCoord& td) const {
    TwoDCoord rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  inline TwoDCoord operator / (const TwoDCoord& td) const {
    TwoDCoord rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  inline TwoDCoord operator + (int td) const {
    TwoDCoord rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  inline TwoDCoord operator - (int td) const {
    TwoDCoord rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  inline TwoDCoord operator * (int td) const {
    TwoDCoord rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  inline TwoDCoord operator / (int td) const {
    TwoDCoord rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  inline TwoDCoord operator - () const {
    TwoDCoord rv; rv.x = -x; rv.y = -y; return rv;
  }
  inline TwoDCoord operator -- () const {
    TwoDCoord rv = *this; rv.x--; rv.y--; return rv;
  }
  inline TwoDCoord operator ++ () const {
    TwoDCoord rv = *this; rv.x++; rv.y++; return rv;
  }

  inline bool operator < (const TwoDCoord& td) const { return (x < td.x) && (y < td.y); }
  inline bool operator > (const TwoDCoord& td) const { return (x > td.x) && (y > td.y); }
  inline bool operator <= (const TwoDCoord& td) const { return (x <= td.x) && (y <= td.y); }
  inline bool operator >= (const TwoDCoord& td) const { return (x >= td.x) && (y >= td.y); }
  inline bool operator == (const TwoDCoord& td) const { return (x == td.x) && (y == td.y); }
  inline bool operator != (const TwoDCoord& td) const { return (x != td.x) || (y != td.y); }

  inline bool operator <  (int td) const { return (x < td) && (y < td); }
  inline bool operator >  (int td) const { return (x > td) && (y > td); }
  inline bool operator <= (int td) const { return (x <= td) && (y <= td); }
  inline bool operator >= (int td) const { return (x >= td) && (y >= td); }
  inline bool operator == (int td) const { return (x == td) && (y == td); }
  inline bool operator != (int td) const { return (x != td) || (y != td); }

  inline bool OrEq(int td) const { return (x == td) || (y == td); }
  inline bool OrEq(const TwoDCoord& td) const { return (x == td.x) || (y == td.y); }

  inline int SqMag() const { return x * x + y * y; }
  // squared magnitude of vector
  inline float Mag() const { return sqrt((float)SqMag()); }

  inline float	SqDist(const TwoDCoord& td) const { // squared distance between two vectors
    TwoDCoord dist = *this - td; return dist.SqMag();
  }
  inline float	Dist(const TwoDCoord& td) const { return sqrt(SqDist(td)); }
  inline int	Sum() const 	{ return x + y; }
  inline int 	Product() const	{ return x * y; }

  static inline int Sgn(int val) { return (val >= 0) ? 1 : -1; }
  static inline int Absv(int val) { return (val >= 0) ? val : -val; }

  inline void 	Invert()    	{ x = -x; y = -y; }
  inline void 	SumNorm() 	{ int mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void	Abs() 		{ x = Absv(x); y = Absv(y); }
  inline void	Min(TwoDCoord& td) { x = MIN(x,td.x); y = MIN(y,td.y); }
  inline void	Max(TwoDCoord& td) { x = MAX(x,td.x); y = MAX(y,td.y); }

  inline int MaxVal() const	{ int mx = MAX(x, y); return mx; }
  inline int MinVal() const	{ int mn = MIN(x, y); return mn; }

  inline String GetStr() { return String(x) + ", " + String(y); }

  bool		FitN(int n);		// adjust x and y to fit x total elements

  void		SetGtEq(int n)	{ x = MAX(n, x);  y = MAX(n, y); }
  // set each to be greater than or equal to n
  void		SetLtEq(int n)	{ x = MIN(n, x);  y = MIN(n, y); }
  // set each to be less than or equal to n

  static bool	WrapClipOne(bool wrap, int& c, int max);
  // wrap-around or clip one dimension, true if clipped (coord set to -1)
  bool		WrapClip(bool wrap, const TwoDCoord& max) {
    return WrapClipOne(wrap, x, max.x) || WrapClipOne(wrap, y, max.y);
  } // wrap-around or clip coordinates within 0,0 - max range, true if clipped
};

inline TwoDCoord operator + (int td, const TwoDCoord& v) {
  TwoDCoord rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
}
inline TwoDCoord operator - (int td, const TwoDCoord& v) {
  TwoDCoord rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
}
inline TwoDCoord operator * (int td, const TwoDCoord& v) {
  TwoDCoord rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
}
inline TwoDCoord operator / (int td, const TwoDCoord& v) {
  TwoDCoord rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
}

class TA_API PosTwoDCoord : public TwoDCoord {
  // #NO_UPDATE_AFTER #INLINE #INLINE_DUMP positive-only value in 2D coordinate space
  INHERITED(TwoDCoord)
public:
  void	Initialize()		{ }
  void	Destroy()		{ };
  TA_BASEFUNS(PosTwoDCoord);

  inline PosTwoDCoord& operator=(int cp) { x = cp; y = cp; return *this;}
  inline PosTwoDCoord& operator=(float cp) { x = (int)cp; y = (int)cp; return *this;}
  inline PosTwoDCoord& operator=(double cp) { x = (int)cp; y = (int)cp; return *this;}
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API XYNGeom : public PosTwoDCoord {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP two-dimensional X-Y geometry with possibility of total number n != x*y
  INHERITED(PosTwoDCoord)
public:
  bool	       	n_not_xy;	// #DEF_false total number of units is less than x * y
  int		n;		// #CONDEDIT_ON_n_not_xy:true total number of units (=x*y unless n_not_xy is true)

  int 		z;
  // #HIDDEN #READ_ONLY #NO_SAVE legacy v3 third dimension -- used for conversion only -- do not use!!  to be removed at some later date

  void operator=(const TwoDCoord& cp);

  void 	Copy(const XYNGeom& cp)
  { PosTwoDCoord::Copy(cp); n_not_xy = cp.n_not_xy; n = cp.n; z = cp.z; }
  void	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(XYNGeom);
protected:
  void	UpdateAfterEdit_impl();

};

class TA_API TDCoord : public TwoDCoord {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP a value in 3D coordinate space
  INHERITED(TwoDCoord)
public:
  int 		z;  		// depth

  bool		isZero() {return ((x == 0) && (y == 0) && (z = 0));}

  inline void 	SetXYZ(int xx, int yy, int zz) {
    x = xx; y = yy; z = zz;
  }
  inline void 	SetXYZ(float xx, float yy, float zz) {
    x = (int)xx; y = (int)yy; z = (int)zz;
  }
  inline void 	GetXYZ(float& xx, float& yy, float& zz) {
    xx = (float)x; yy = (float)y; zz = (float)z;
  }
  override void	CopyToMatrixGeom(MatrixGeom& geom);

  inline void 	Initialize() 		{ x = y = z = 0; }
  inline void 	Destroy()		{ };
  inline void 	Copy(const TDCoord& cp)	{ x = cp.x; y = cp.y; z = cp.z; }

  TDCoord() 				{ Initialize(); }
  TDCoord(const TDCoord& cp) 		{ Copy(cp); }
  TDCoord(int xx) 			{ SetXYZ(xx, xx, xx); }
  TDCoord(int xx, int yy, int zz) 	{ SetXYZ(xx, yy, zz); }
  TDCoord(float xx, float yy, float zz) { SetXYZ(xx, yy, zz); }
  TDCoord(const FloatTDCoord& cp);	// conversion constructor
  ~TDCoord() 				{ };
  TAPtr Clone() 			{ return new TDCoord(*this); }
  void  UnSafeCopy(TAPtr cp);
  void  CastCopyTo(TAPtr cp) 		{ TDCoord& rf = *((TDCoord*)cp); rf.Copy(*this); }
  TAPtr MakeToken()			{ return (TAPtr)(new TDCoord); }
  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new TDCoord[no]); }
  TypeDef* GetTypeDef() const 		{ return StatTypeDef(0); }
  static TypeDef* StatTypeDef(int);

  inline TDCoord& operator=(const TDCoord& cp) 	{ Copy(cp); return *this;}
  TDCoord& operator=(const FloatTDCoord& cp);
  inline TDCoord& operator=(int cp) 		{ x = cp; y = cp; z = cp; return *this;}
  inline TDCoord& operator=(float cp) 		{ x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
  inline TDCoord& operator=(double cp) 		{ x = (int)cp; y = (int)cp; z = (int)cp; return *this;}

  inline TDCoord& operator += (const TDCoord& td)	{ x += td.x; y += td.y; z += td.z; return *this;}
  inline TDCoord& operator -= (const TDCoord& td)	{ x -= td.x; y -= td.y; z -= td.z; return *this;}
  inline TDCoord& operator *= (const TDCoord& td)	{ x *= td.x; y *= td.y; z *= td.z; return *this;}
  inline TDCoord& operator /= (const TDCoord& td)	{ x /= td.x; y /= td.y; z /= td.z; return *this;}
  inline TDCoord& operator %= (const TDCoord& td)	{ x %= td.x; y %= td.y; z %= td.z; return *this;}

  inline TDCoord& operator += (int td)	{ x += td; y += td; z += td; return *this;}
  inline TDCoord& operator -= (int td)	{ x -= td; y -= td; z -= td; return *this;}
  inline TDCoord& operator *= (int td)	{ x *= td; y *= td; z *= td; return *this;}
  inline TDCoord& operator /= (int td)	{ x /= td; y /= td; z /= td; return *this;}
  inline TDCoord& operator %= (int td)	{ x %= td; y %= td; z %= td; return *this;}

  inline TDCoord operator + (const TDCoord& td) const {
    TDCoord rv; rv.x = x + td.x; rv.y = y + td.y; rv.z = z + td.z; return rv;
  }
  inline TDCoord operator - (const TDCoord& td) const {
    TDCoord rv; rv.x = x - td.x; rv.y = y - td.y; rv.z = z - td.z; return rv;
  }
  inline TDCoord operator * (const TDCoord& td) const {
    TDCoord rv; rv.x = x * td.x; rv.y = y * td.y; rv.z = z * td.z; return rv;
  }
  inline TDCoord operator / (const TDCoord& td) const {
    TDCoord rv; rv.x = x / td.x; rv.y = y / td.y; rv.z = z / td.z; return rv;
  }

  inline TDCoord operator + (int td) const {
    TDCoord rv; rv.x = x + td; rv.y = y + td; rv.z = z + td; return rv;
  }
  inline TDCoord operator - (int td) const {
    TDCoord rv; rv.x = x - td; rv.y = y - td; rv.z = z - td; return rv;
  }
  inline TDCoord operator * (int td) const {
    TDCoord rv; rv.x = x * td; rv.y = y * td; rv.z = z * td; return rv;
  }
  inline TDCoord operator / (int td) const {
    TDCoord rv; rv.x = x / td; rv.y = y / td; rv.z = z / td; return rv;
  }

  inline TDCoord operator - () const {
    TDCoord rv; rv.x = -x; rv.y = -y; rv.z = -z; return rv;
  }
  inline TDCoord operator -- () const {
    TDCoord rv = *this; rv.x--; rv.y--; rv.z--; return rv;
  }
  inline TDCoord operator ++ () const {
    TDCoord rv = *this; rv.x++; rv.y++; rv.z++; return rv;
  }

  inline bool operator < (const TDCoord& td) const { return (x < td.x) && (y < td.y) && (z < td.z); }
  inline bool operator > (const TDCoord& td) const { return (x > td.x) && (y > td.y) && (z > td.z); }
  inline bool operator <= (const TDCoord& td) const { return (x <= td.x) && (y <= td.y) && (z <= td.z); }
  inline bool operator >= (const TDCoord& td) const { return (x >= td.x) && (y >= td.y) && (z >= td.z); }
  inline bool operator == (const TDCoord& td) const { return (x == td.x) && (y == td.y) && (z == td.z); }
  inline bool operator != (const TDCoord& td) const { return (x != td.x) || (y != td.y) || (z != td.z); }

  inline bool operator <  (int td) const { return (x < td) && (y < td) && (z < td); }
  inline bool operator >  (int td) const { return (x > td) && (y > td) && (z > td); }
  inline bool operator <= (int td) const { return (x <= td) && (y <= td) && (z <= td); }
  inline bool operator >= (int td) const { return (x >= td) && (y >= td) && (z >= td); }
  inline bool operator == (int td) const { return (x == td) && (y == td) && (z == td); }
  inline bool operator != (int td) const { return (x != td) || (y != td) || (z != td); }

  inline bool OrEq(int td) const { return (x == td) || (y == td) || (z == td); }
  inline bool OrEq(const TDCoord& td) const { return (x == td.x) || (y == td.y) || (z == td.z); }

  inline int SqMag() const {		// squared magnitude of vector
    return x * x + y * y + z * z;
  }
  inline float Mag() const { return sqrt((float)SqMag()); }

  inline float	SqDist(const TDCoord& td) const { // squared distance between two vectors
    TDCoord dist = *this - td; return dist.SqMag();
  }
  inline float	Dist(const TDCoord& td) const { return sqrt(SqDist(td)); }
  inline int	Sum() const 	{ return x + y + z; }
  inline int 	Product() const	{ return x * y * z; }

  static inline int Sgn(int val) { return (val >= 0) ? 1 : -1; }
  static inline int Absv(int val) { return (val >= 0) ? val : -val; }

  inline void 	Invert()    	{ x = -x; y = -y; z = -z; }
  inline void 	SumNorm() 	{ int mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void	Abs() 		{ x = Absv(x); y = Absv(y); z = Absv(z); }
  inline void	Min(TDCoord& td) { x = MIN(x,td.x); y = MIN(y,td.y); z = MIN(z,td.z); }
  inline void	Max(TDCoord& td) { x = MAX(x,td.x); y = MAX(y,td.y); z = MAX(z,td.z); }

  inline int MaxVal() const	{ int mx = MAX(x, y); mx = MAX(mx, z); return mx; }
  inline int MinVal() const	{ int mn = MIN(x, y); mn = MIN(mn, z); return mn; }

  inline String GetStr() { return String(x) + ", " + String(y) + ", " + String(z); }

  bool		FitNinXY(int n);	// adjust x and y to fit x total elements

  void		SetGtEq(int n)	// set each to be greater than or equal to n
  { x = MAX(n, x);  y = MAX(n, y); z = MAX(n, z); }
  void		SetLtEq(int n)	// set each to be less than or equal to n
  { x = MIN(n, x);  y = MIN(n, y); z = MIN(n, z); }

  bool		WrapClip(bool wrap, const TDCoord& max) {
    return TwoDCoord::WrapClip(wrap, max) || WrapClipOne(wrap, z, max.z);
  } // wrap-around or clip coordinates within 0,0 - max range, -1 if clipped
};

inline TDCoord operator + (int td, const TDCoord& v) {
  TDCoord rv; rv.x = td + v.x; rv.y = td + v.y; rv.z = td + v.z; return rv;
}
inline TDCoord operator - (int td, const TDCoord& v) {
  TDCoord rv; rv.x = td - v.x; rv.y = td - v.y; rv.z = td - v.z; return rv;
}
inline TDCoord operator * (int td, const TDCoord& v) {
  TDCoord rv; rv.x = td * v.x; rv.y = td * v.y; rv.z = td * v.z; return rv;
}
inline TDCoord operator / (int td, const TDCoord& v) {
  TDCoord rv; rv.x = td / v.x; rv.y = td / v.y; rv.z = td / v.z; return rv;
}

class TA_API PosTDCoord : public TDCoord {
  // #NO_UPDATE_AFTER #INLINE #INLINE_DUMP positive-only value in 3D coordinate space
  INHERITED(TDCoord)
public:
  void	Initialize()		{ x = y = z = 0; }
  void	Destroy()		{ };
  TA_BASEFUNS(PosTDCoord);

  inline PosTDCoord& operator=(int cp) 	{ x = cp; y = cp; z = cp; return *this;}
  inline PosTDCoord& operator=(float cp) { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
  inline PosTDCoord& operator=(double cp) { x = (int)cp; y = (int)cp; z = (int)cp; return *this;}
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API FloatTwoDCoord : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a value in 2D coordinate space
  INHERITED(taBase)
public:
  float		x;  		// horizontal
  float		y;  		// vertical

  inline void 	SetXY(float xx, float yy) 	{ x = xx; y = yy; }
  inline void 	GetXY(float& xx, float& yy) 	{ xx = x; yy = y; }

  inline void 	Initialize() 			{ x = y = 0.0; }
  inline void 	Destroy()			{ };
  inline void 	Copy(const FloatTwoDCoord& cp)	{ x = cp.x; y = cp.y; }

  FloatTwoDCoord() 				{ Initialize(); }
  FloatTwoDCoord(const FloatTwoDCoord& cp) 	{ Copy(cp); }
  FloatTwoDCoord(float xx) 			{ SetXY(xx, xx); }
  FloatTwoDCoord(float xx, float yy) 		{ SetXY(xx, yy); }
  FloatTwoDCoord(int xx) 			{ SetXY(xx, xx); }
  FloatTwoDCoord(int xx, int yy) 		{ SetXY(xx, yy); }
  FloatTwoDCoord(const TwoDCoord& cp);	// conversion constructor
  ~FloatTwoDCoord() 				{ };
  TAPtr Clone() 				{ return new FloatTwoDCoord(*this); }
  void  UnSafeCopy(TAPtr cp);
  void  CastCopyTo(TAPtr cp) 	{ FloatTwoDCoord& rf = *((FloatTwoDCoord*)cp); rf.Copy(*this); }
  TAPtr MakeToken()			{ return (TAPtr)(new FloatTwoDCoord); }
  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new FloatTwoDCoord[no]); }
  TypeDef* GetTypeDef() const 		{ return StatTypeDef(0); }
  static TypeDef* StatTypeDef(int);

  inline FloatTwoDCoord& operator=(const FloatTwoDCoord& cp) { Copy(cp); return *this;}
  FloatTwoDCoord& operator=(const TwoDCoord& cp);
  inline FloatTwoDCoord& operator=(float cp) 		{ x = cp; y = cp; return *this;}
  inline FloatTwoDCoord& operator=(double cp) 		{ x = (float)cp; y = (float)cp; return *this;}

  inline FloatTwoDCoord& operator += (const FloatTwoDCoord& td)	{ x += td.x; y += td.y; return *this;}
  inline FloatTwoDCoord& operator -= (const FloatTwoDCoord& td)	{ x -= td.x; y -= td.y; return *this;}
  inline FloatTwoDCoord& operator *= (const FloatTwoDCoord& td)	{ x *= td.x; y *= td.y; return *this;}
  inline FloatTwoDCoord& operator /= (const FloatTwoDCoord& td)	{ x /= td.x; y /= td.y; return *this;}

  inline FloatTwoDCoord& operator += (float td)	{ x += td; y += td; return *this;}
  inline FloatTwoDCoord& operator -= (float td)	{ x -= td; y -= td; return *this;}
  inline FloatTwoDCoord& operator *= (float td)	{ x *= td; y *= td; return *this;}
  inline FloatTwoDCoord& operator /= (float td)	{ x /= td; y /= td; return *this;}

  inline FloatTwoDCoord operator + (const FloatTwoDCoord& td) const {
    FloatTwoDCoord rv; rv.x = x + td.x; rv.y = y + td.y; return rv;
  }
  inline FloatTwoDCoord operator - (const FloatTwoDCoord& td) const {
    FloatTwoDCoord rv; rv.x = x - td.x; rv.y = y - td.y; return rv;
  }
  inline FloatTwoDCoord operator * (const FloatTwoDCoord& td) const {
    FloatTwoDCoord rv; rv.x = x * td.x; rv.y = y * td.y; return rv;
  }
  inline FloatTwoDCoord operator / (const FloatTwoDCoord& td) const {
    FloatTwoDCoord rv; rv.x = x / td.x; rv.y = y / td.y; return rv;
  }

  inline FloatTwoDCoord operator + (float td) const {
    FloatTwoDCoord rv; rv.x = x + td; rv.y = y + td; return rv;
  }
  inline FloatTwoDCoord operator - (float td) const {
    FloatTwoDCoord rv; rv.x = x - td; rv.y = y - td; return rv;
  }
  inline FloatTwoDCoord operator * (float td) const {
    FloatTwoDCoord rv; rv.x = x * td; rv.y = y * td; return rv;
  }
  inline FloatTwoDCoord operator / (float td) const {
    FloatTwoDCoord rv; rv.x = x / td; rv.y = y / td; return rv;
  }

  inline FloatTwoDCoord operator - () const {
    FloatTwoDCoord rv; rv.x = -x; rv.y = -y; return rv;
  }

  inline bool operator < (const FloatTwoDCoord& td) const { return (x < td.x) && (y < td.y); }
  inline bool operator > (const FloatTwoDCoord& td) const { return (x > td.x) && (y > td.y); }
  inline bool operator <= (const FloatTwoDCoord& td) const { return (x <= td.x) && (y <= td.y); }
  inline bool operator >= (const FloatTwoDCoord& td) const { return (x >= td.x) && (y >= td.y); }
  inline bool operator == (const FloatTwoDCoord& td) const { return (x == td.x) && (y == td.y); }
  inline bool operator != (const FloatTwoDCoord& td) const { return (x != td.x) || (y != td.y); }

  inline bool operator <  (float td) const { return (x < td) && (y < td); }
  inline bool operator >  (float td) const { return (x > td) && (y > td); }
  inline bool operator <= (float td) const { return (x <= td) && (y <= td); }
  inline bool operator >= (float td) const { return (x >= td) && (y >= td); }
  inline bool operator == (float td) const { return (x == td) && (y == td); }
  inline bool operator != (float td) const { return (x != td) || (y != td); }

  inline float SqMag() const 	{ return x * x + y * y; }
  // squared magnitude of vector
  inline float Mag() const 	{ return sqrt(SqMag()); }

  inline float	SqDist(const FloatTwoDCoord& td) const { // squared distance between two vectors
    FloatTwoDCoord dist = *this - td; return dist.Mag();
  }
  inline float	Dist(const FloatTwoDCoord& td) const { return sqrt(SqDist(td)); }
  inline float	Sum() const 	{ return x + y; }
  inline float 	Product() const	{ return x * y; }

  inline void 	Invert()    	{ x = -x; y = -y; }
  inline void 	MagNorm() 	{ float mg = Mag(); if(mg > 0.0) *this /= mg; }
  inline void 	SumNorm() 	{ float mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void	Abs() 		{ x = fabs(x); y = fabs(y); }

  inline float MaxVal() const	{ float mx = MAX(x, y); return mx; }
  inline float MinVal() const	{ float mn = MIN(x, y); return mn; }

  inline String GetStr() { return String(x) + ", " + String(y); }
};

inline FloatTwoDCoord operator + (float td, const FloatTwoDCoord& v) {
  FloatTwoDCoord rv; rv.x = td + v.x; rv.y = td + v.y; return rv;
}
inline FloatTwoDCoord operator - (float td, const FloatTwoDCoord& v) {
  FloatTwoDCoord rv; rv.x = td - v.x; rv.y = td - v.y; return rv;
}
inline FloatTwoDCoord operator * (float td, const FloatTwoDCoord& v) {
  FloatTwoDCoord rv; rv.x = td * v.x; rv.y = td * v.y; return rv;
}
inline FloatTwoDCoord operator / (float td, const FloatTwoDCoord& v) {
  FloatTwoDCoord rv; rv.x = td / v.x; rv.y = td / v.y; return rv;
}

class TA_API FloatTDCoord : public FloatTwoDCoord {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP a real value in 3D coordinate space
  INHERITED(FloatTwoDCoord)
public:
  float 	z;  		// depth

  inline void 	SetXYZ(float xx, float yy, float zz) {
    x = xx; y = yy; z = zz;
  }
  inline void 	GetXYZ(float& xx, float& yy, float& zz) {
    xx = x; yy = y; zz = z;
  }

  inline void 	Initialize() 			{ x = y = z = 0.0; }
  inline void 	Destroy()			{ };
  inline void 	Copy(const FloatTDCoord& cp)	{ x = cp.x; y = cp.y; z = cp.z; }

  FloatTDCoord() 				{ Initialize(); }
  FloatTDCoord(const FloatTDCoord& cp) 		{ Copy(cp); }
  FloatTDCoord(float xx)			{ SetXYZ(xx, xx, xx); }
  FloatTDCoord(float xx, float yy, float zz)	{ SetXYZ(xx, yy, zz); }
  FloatTDCoord(int xx) 				{ SetXYZ(xx, xx, xx); }
  FloatTDCoord(int xx, int yy, int zz) 		{ SetXYZ(xx, yy, zz); }
  FloatTDCoord(const TDCoord& cp);	// conversion constructor
  ~FloatTDCoord() 				{ };
  TAPtr Clone() 			{ return new FloatTDCoord(*this); }
  void  UnSafeCopy(TAPtr cp);
  void  CastCopyTo(TAPtr cp) 		{ FloatTDCoord& rf = *((FloatTDCoord*)cp); rf.Copy(*this); }
  TAPtr MakeToken()			{ return (TAPtr)(new FloatTDCoord); }
  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new FloatTDCoord[no]); }
  TypeDef* GetTypeDef() const 		{ return StatTypeDef(0); }
  static TypeDef* StatTypeDef(int);
  inline FloatTDCoord& operator=(const FloatTDCoord& cp) { Copy(cp); return *this;}
  FloatTDCoord& operator=(const TDCoord& cp);
  inline FloatTDCoord& operator=(float cp) 		{ x = cp; y = cp; z = cp; return *this;}
  inline FloatTDCoord& operator=(double cp) 		{ x = (float)cp; y = (float)cp; z = (float)cp; return *this;}

  inline FloatTDCoord& 	operator += (const FloatTDCoord& td)	{ x += td.x; y += td.y; z += td.z; return *this;}
  inline FloatTDCoord& 	operator -= (const FloatTDCoord& td)	{ x -= td.x; y -= td.y; z -= td.z; return *this;}
  inline FloatTDCoord& 	operator *= (const FloatTDCoord& td)	{ x *= td.x; y *= td.y; z *= td.z; return *this;}
  inline FloatTDCoord& 	operator /= (const FloatTDCoord& td)	{ x /= td.x; y /= td.y; z /= td.z; return *this;}

  inline FloatTDCoord& 	operator += (float td)	{ x += td; y += td; z += td; return *this;}
  inline FloatTDCoord& 	operator -= (float td)	{ x -= td; y -= td; z -= td; return *this;}
  inline FloatTDCoord& 	operator *= (float td)	{ x *= td; y *= td; z *= td; return *this;}
  inline FloatTDCoord& 	operator /= (float td)	{ x /= td; y /= td; z /= td; return *this;}

  inline FloatTDCoord operator + (const FloatTDCoord& td) const {
    FloatTDCoord rv; rv.x = x + td.x; rv.y = y + td.y; rv.z = z + td.z; return rv;
  }
  inline FloatTDCoord operator - (const FloatTDCoord& td) const {
    FloatTDCoord rv; rv.x = x - td.x; rv.y = y - td.y; rv.z = z - td.z; return rv;
  }
  inline FloatTDCoord operator * (const FloatTDCoord& td) const {
    FloatTDCoord rv; rv.x = x * td.x; rv.y = y * td.y; rv.z = z * td.z; return rv;
  }
  inline FloatTDCoord operator / (const FloatTDCoord& td) const {
    FloatTDCoord rv; rv.x = x / td.x; rv.y = y / td.y; rv.z = z / td.z; return rv;
  }

  inline FloatTDCoord operator + (float td) const {
    FloatTDCoord rv; rv.x = x + td; rv.y = y + td; rv.z = z + td; return rv;
  }
  inline FloatTDCoord operator - (float td) const {
    FloatTDCoord rv; rv.x = x - td; rv.y = y - td; rv.z = z - td; return rv;
  }
  inline FloatTDCoord operator * (float td) const {
    FloatTDCoord rv; rv.x = x * td; rv.y = y * td; rv.z = z * td; return rv;
  }
  inline FloatTDCoord operator / (float td) const {
    FloatTDCoord rv; rv.x = x / td; rv.y = y / td; rv.z = z / td; return rv;
  }

  inline FloatTDCoord operator - () const {
    FloatTDCoord rv; rv.x = -x; rv.y = -y; rv.z = -z; return rv;
  }

  inline bool operator < (const FloatTDCoord& td) const { return (x < td.x) && (y < td.y) && (z < td.z); }
  inline bool operator > (const FloatTDCoord& td) const { return (x > td.x) && (y > td.y) && (z > td.z); }
  inline bool operator <= (const FloatTDCoord& td) const { return (x <= td.x) && (y <= td.y) && (z <= td.z); }
  inline bool operator >= (const FloatTDCoord& td) const { return (x >= td.x) && (y >= td.y) && (z >= td.z); }
  inline bool operator == (const FloatTDCoord& td) const { return (x == td.x) && (y == td.y) && (z == td.z); }
  inline bool operator != (const FloatTDCoord& td) const { return (x != td.x) || (y != td.y) || (z != td.z); }

  inline bool operator <  (float td) const { return (x < td) && (y < td) && (z < td); }
  inline bool operator >  (float td) const { return (x > td) && (y > td) && (z > td); }
  inline bool operator <= (float td) const { return (x <= td) && (y <= td) && (z <= td); }
  inline bool operator >= (float td) const { return (x >= td) && (y >= td) && (z >= td); }
  inline bool operator == (float td) const { return (x == td) && (y == td) && (z == td); }
  inline bool operator != (float td) const { return (x != td) || (y != td) || (z != td); }


  bool		Equals(float v)
    {return ((x == v) && (y == v) && (z == v));}
  bool		Equals(float xx, float yy, float zz)
    {return ((x == xx) && (y == yy) && (z == zz));}
  inline float SqMag() const {		// squared magnitude of vector
    return x * x + y * y + z * z;
  }
  inline float Mag() const { return sqrt(SqMag()); }

  inline float	SqDist(const FloatTDCoord& td) const { // squared distance between two vectors
    FloatTDCoord dist = *this - td; return dist.SqMag();
  }
  inline float	Dist(const FloatTDCoord& td) const {
    FloatTDCoord dist = *this - td; return dist.Mag();
  }
  inline float	Sum() const 	{ return x + y + z; }
  inline float	Product() const	{ return x * y * z; }

  inline void 	Invert()    	{ x = -x; y = -y; z = -z; }
  inline void 	MagNorm() 	{ float mg = Mag(); if(mg > 0.0) *this /= mg; }
  inline void 	SumNorm() 	{ float mg = Sum(); if(mg != 0.0) *this /= mg; }
  inline void	Abs() 		{ x = fabs(x); y = fabs(y); z = fabs(z); }

  inline float MaxVal() const	{ float mx = MAX(x, y); mx = MAX(mx, z); return mx; }
  inline float MinVal() const	{ float mn = MIN(x, y); mn = MIN(mn, z); return mn; }

  static inline float Sgn(float val) { return (val >= 0.0) ? 1.0 : -1.0; }

  inline String GetStr() { return String(x) + ", " + String(y) + ", " + String(z); }
#ifdef TA_GUI
  operator iVec3f() const {return iVec3f(x, y, z);}
#endif
};

inline FloatTDCoord operator + (float td, const FloatTDCoord& v) {
  FloatTDCoord rv; rv.x = td + v.x; rv.y = td + v.y; rv.z = td + v.z; return rv;
}
inline FloatTDCoord operator - (float td, const FloatTDCoord& v) {
  FloatTDCoord rv; rv.x = td - v.x; rv.y = td - v.y; rv.z = td - v.z; return rv;
}
inline FloatTDCoord operator * (float td, const FloatTDCoord& v) {
  FloatTDCoord rv; rv.x = td * v.x; rv.y = td * v.y; rv.z = td * v.z; return rv;
}
inline FloatTDCoord operator / (float td, const FloatTDCoord& v) {
  FloatTDCoord rv; rv.x = td / v.x; rv.y = td / v.y; rv.z = td / v.z; return rv;
}

class TA_API FloatRotation: public FloatTDCoord {
  //  ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP 3-d rotation data, xyz specify the rotation axis
  INHERITED(FloatTDCoord)
public:
  float		rot; // rotation angle, in radians

  inline void 	SetXYZR(float xx, float yy, float zz, float rr) {
    x = xx; y = yy; z = zz; rot = rr;
  }
  inline void 	GetXYZR(float& xx, float& yy, float& zz, float& rr) {
    xx = x; yy = y; zz = z; rr = rot;
  }

  String 		GetStr() {return FloatTDCoord::GetStr() + ", " + String(rot); }

  void 			Copy_(const FloatRotation& cp)	{x = cp.x; y = cp.y; z = cp.z; rot = cp.rot;}

  void 			Initialize() {x = y = 0.0f; z = 1.0f; rot = 0.0;}
  void 			Destroy() {}
  FloatRotation(float xx, float yy, float zz, float rr)	{SetXYZR(xx, yy, zz, rr); }
  FloatRotation(int xx, int yy, int zz, float rr) 	{SetXYZR(xx, yy, zz, rr); }
  TA_BASEFUNS(FloatRotation);
  COPY_FUNS(FloatRotation, FloatTDCoord);
};

class TA_API FloatTransform: public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math 3-d transformation data; applied in order: s, r, t
  INHERITED(taBase)
public:
  FloatTDCoord		scale; // scale factors, in x, y, and z
  FloatRotation		rotate; // rotation
  FloatTDCoord		translate; // translate, in x, y, and z

  void 			Copy_(const FloatTransform& cp)
    {scale.Copy(cp.scale); rotate.Copy(cp.rotate); translate.Copy(cp.translate);}
  TA_BASEFUNS(FloatTransform);
  COPY_FUNS(FloatTransform, taBase);
#ifdef TA_USE_INVENTOR
  void			CopyTo(SoTransform* txfm); // #IGNORE txfers values to an inventor txfm -- note, does a transfer, not an accumulate
#endif
private:
  void 			Initialize() {scale = 1.0f;}
  void 			Destroy() {}
};

// TODO: Brad -- you might need to update this with system-wide constructor changes, etc

class TA_API ValIdx : public taBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a float value and an index: very useful for sorting!
  INHERITED(taBase)
public:
  float		val;  		// value
  int		idx;		// index

  inline void 	SetValIdx(float v, int i) 	{ val = v; idx = i; }
  inline void 	GetValIdx(float& v, int& i) 	{ v = val; i = idx; }

  inline void 	Initialize() 			{ val = 0.0; idx = 0; }
  inline void 	Destroy()			{ };
  inline void 	Copy(const ValIdx& cp)	{ val = cp.val; idx = cp.idx; }

  ValIdx() 				{ Initialize(); }
  ValIdx(const ValIdx& cp) 		{ Copy(cp); }
  ValIdx(float v, int i) 		{ SetValIdx(v, i); }
  ValIdx(const String& str) 			{ val = (float)str; }
  ~ValIdx() 				{ };
  TAPtr Clone() 			{ return new ValIdx(*this); }
  void  UnSafeCopy(TAPtr cp) {
    if(cp->InheritsFrom(&TA_ValIdx)) Copy(*((ValIdx*)cp));
    if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);
  }
  void  CastCopyTo(TAPtr cp) 	{ ValIdx& rf = *((ValIdx*)cp); rf.Copy(*this); }
  TAPtr MakeToken()			{ return (TAPtr)(new ValIdx); }
  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new ValIdx[no]); }
  TypeDef* GetTypeDef() const 		{ return &TA_ValIdx; }
  static TypeDef* StatTypeDef(int) 	{ return &TA_ValIdx; }

  inline operator String () const { return String(val); }

  inline void operator=(const ValIdx& cp) { Copy(cp); }
  inline void operator=(float cp) 		{ val = cp; idx = -1; }

  inline void operator += (const ValIdx& td)	{ val += td.val; }
  inline void operator -= (const ValIdx& td)	{ val -= td.val; }
  inline void operator *= (const ValIdx& td)	{ val *= td.val; }
  inline void operator /= (const ValIdx& td)	{ val /= td.val; }

  inline void operator += (float td)	{ val += td; }
  inline void operator -= (float td)	{ val -= td; }
  inline void operator *= (float td)	{ val *= td; }
  inline void operator /= (float td)	{ val /= td; }

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

  inline String GetStr() { return String(val) + ":" + String(idx); }
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
public:
  STATIC_CONST ValIdx blank; // #HIDDEN #READ_ONLY 
  String	El_GetStr_(void* it) const { return (String)((ValIdx*)it); } // #IGNORE
  void		El_SetFmStr_(void* it, String& val)
  { ((ValIdx*)it)->val = (float)val; } // #IGNORE
  virtual void*		GetTA_Element(int i, TypeDef*& eltd)
  { eltd = &TA_ValIdx; if(InRange(i)) return FastEl_(i); return NULL; }
  void Initialize()	{ };
  void Destroy()	{ };
  TA_BASEFUNS(ValIdx_Array);
  TA_ARRAY_FUNS(ValIdx_Array,ValIdx);
};

#endif // tdgeometry_h
