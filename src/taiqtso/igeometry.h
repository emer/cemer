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

#ifndef IGEOMETRY_H
#define IGEOMETRY_H

#include "taiqtso_def.h"

/* Notes:
   1. all classes below are value-semantics, and use the implicit copy and = operators.
   2. MAKETA cannot boggle the : class(): x(), y() {} form of constructor initialialization,
      so they have been moved to the .cc file.
   3. DO NOT add any virtual methods to these value-semantic classes
*/

#ifdef TA_USE_INVENTOR
class SbVec2s;		// #IGNORE
class SbVec3s;		// #IGNORE
#endif


class TAIQTSO_API iPoint { // #NO_TOKENS #INSTANCE
public:
  int	x;
  int	y;

  int		getArrayIndex(int x_, int y_); // assumes x and y are array sizes -- gets the row-major array index, or -1 if out of bounds
  bool		isEqual(int x_, int y_) {return ((x == x_) && (y == y_));}
  void		getValue(int& x_, int& y_) {x_ = x; y_ = y;}
  void		setValue(int x_, int y_) {x = x_; y = y_;}

  iPoint() {x = 0; y = 0;}
  iPoint(int x_, int y_) {x = x_; y = y_;}
  iPoint(int v) {x = v; y = v;}
  iPoint(const iPoint& cp) {x = cp.x; y = cp.y;}

  iPoint& operator =(const iPoint& cp) {x = cp.x; y = cp.y; return *this;}
  iPoint& operator =(int v) {x = v; y = v; return *this;}

#ifdef TA_GUI
  iPoint(const QPoint& val);
  iPoint& operator=(const QPoint& val);
  operator QPoint() const;
#endif
#ifdef TA_USE_INVENTOR
  iPoint(const SbVec2s& src);
  iPoint&	operator=(const SbVec2s& src);
  operator SbVec2s() const; // note: will always be in range in context of gui
#endif
};

typedef iPoint iVec2i; // synonym

class TAIQTSO_API iVec3i: public iVec2i { // #NO_TOKENS #INSTANCE
public:
  int	z;

  bool		isEqual(int x_, int y_, int z_) {return ((x == x_) && (y == y_) && (z == z_));}
  void		getValue(int& x_, int& y_, int& z_) {x_ = x; y_ = y; z_ = z;}
  void		setValue(int x_, int y_, int z_) {x = x_; y = y_; z = z_;}

  iVec3i() {z = 0;}
  iVec3i(int x_, int y_, int z_): iVec2i(x_, y_) {z = z_;}
  iVec3i(int v): iVec2i(v) {z = v;}
  iVec3i(const iVec3i& cp): iVec2i(cp) {z = cp.z;}

  iVec3i& operator =(const iVec3i& cp) {x = cp.x; y = cp.y; z = cp.z; return *this;}
  iVec3i& operator =(int v) {x = v; y = v; z = v; return *this;}

#ifdef TA_USE_INVENTOR
  iVec3i(const SbVec3s& src);
  iVec3i& operator=(const SbVec3s& src);
  operator SbVec3s() const; // note: will always be in range in context of gui
#endif
};

class TAIQTSO_API iSize { // #NO_TOKENS #INSTANCE
public:
  int w;
  int h;

  iSize();
  iSize(int w_, int h_);

  int height() {return h;}
  int width() {return w;}

#ifdef TA_GUI
  iSize(const QSize& val);
  iSize& operator=(const QSize& val);
  operator QSize() const;
#endif
};


class TAIQTSO_API iRect { // #NO_TOKENS #INSTANCE
public:
  int x;
  int y;
  int w;
  int h;

  iRect();
  iRect(const iPoint& topLeft, const iPoint& bottomRight);
  iRect(const iPoint& topLeft, const iSize& size );
  iRect(int left, int top, int width, int height);

  iPoint topLeft() const {return iPoint(x, y);}
  iSize size() const {return iSize(w, h);}
  int left() const {return x;}
  int top() const {return y;}
  int width() const {return w;}
  int height() const {return h;}

#ifdef TA_GUI
  iRect(const QRect& val);
  iRect& operator=(const QRect& val);
  operator QRect() const;
#endif

};

// Following types are for the 3-d components in t3_gadget

#ifdef TA_USE_INVENTOR
class SbVec3f;		// #IGNORE
class SbBox3f;		// #IGNORE
#endif

class TAIQTSO_API iVec3f { // #NO_TOKENS #INSTANCE 3d vector, for things like points and sizes
public:
  float 	x;
  float 	y;
  float 	z;

  bool		isEqual(float x_, float y_, float z_) const {return ((x == x_) && (y == y_) && (z == z_));}
  bool		isUnity() const { return ((x == 1.0f) && (y == 1.0f) && (z == 1.0f));}
  bool		isZero() const { return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f));}
  void		setValue(float x_, float y_, float z_)  {x = x_; y = y_; z = z_;}

  iVec3f() {x = 0; y = 0; z = 0;}
  iVec3f(float val) {x = val; y = val; z = val;}
  iVec3f(float x_, float y_, float z_) {x = x_; y = y_; z = z_;}
  iVec3f(const iVec3f& val) {x = val.x; y = val.y; z = val.z;}

  iVec3f& 	operator=(float val) {x = val; y = val; z = val; return *this;}
  iVec3f& 	operator=(const iVec3f& val) {x = val.x; y = val.y; z = val.z; return *this;}

#ifdef TA_USE_INVENTOR
  iVec3f(const SbVec3f& src);
  iVec3f&	operator=(const SbVec3f& src);
  operator SbVec3f() const;
#endif
};

/* Note iBox3f is used as a bounding box in a normally rotated coordinate system, such that the box planes
   are coincident with the coordinate planes
*/


class TAIQTSO_API iBox3f { // #NO_TOKENS #INSTANCE 3d box, typically used for a bounding box in a normalized coordinate system
public:
  iVec3f 	min; // minimum point
  iVec3f	max;  // maximum point
  iVec3f 	center() const {return iVec3f((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);}
    // center point of the box
  iVec3f	size() const {return iVec3f(max.x - min.x, max.y - min.y, max.z - min.z);}  // size of the box in each dimension
  bool		isZero() const { return (min.isZero() && max.isZero());}

  void		setValue(float min_x, float min_y, float min_z,  float max_x, float max_y, float max_z)
    {min.x = min_x; min.y = min_y; min.z = min_z;  max.x = max_x; max.y = max_y; max.z = max_z;}
  void		setValue(const iVec3f& min_, const iVec3f& max_)
    {min = min_;   max = max_;}
  void		setValueCS(float center_x, float center_y, float center_z, float size_x, float size_y, float size_z)
  {min.x = center_x - (size_x / 2); min.y = center_y - (size_y / 2); min.z = center_z - (size_z / 2);
  max.x = min.x + size_x; max.y =  min.y + size_y; max.z =  min.z + size_z; }
  // set the value by specifying the center and the size -- can only specify boxes with planes parallel to the coord axes
  void		setValueCS(const iVec3f& center_, const iVec3f& size_)
  {min.x = center_.x - (size_.x / 2); min.y = center_.y - (size_.y / 2); min.z = center_.z - (size_.z / 2);
   max.x = min.x + size_.x; max.y =  min.y + size_.y; max.z =  min.z + size_.z; }
  // set the value by specifying the center and the size -- can only specify boxes with planes parallel to the coord axes

  void		MaxOf(const iBox3f& val) {MaxOf(*this, val);} // #IGNORE sets this instance to most expansive of itself and val
  void		MaxOf(const iBox3f& val1, const iBox3f& val2); //#IGNORE sets this instance to most expansive of inputs

  iBox3f() {}
  iBox3f(float val) {min = val; max = val;}
  iBox3f(float min_x, float min_y, float min_z,  float max_x, float max_y, float max_z)
    {min.x = min_x; min.y = min_y; min.z = min_z;  max.x = max_x; max.y = max_y; max.z = max_z;}
  iBox3f(const iVec3f& min_, const iVec3f& max_) {min = min_;   max = max_;}
  iBox3f(const iBox3f& val) {min = val.min;   max = val.max;}

  iBox3f&	operator=(float val) {min = val; max = val; return *this;}
  iBox3f&	operator=(const iBox3f& val) {min = val.min;   max = val.max; return *this;}

#ifdef TA_USE_INVENTOR
//  iBox3f(const SbBox3f& src);
//  iBox3f&	operator=(const SbBox3f& src);
  operator SbBox3f() const; // we can convert to a SbBox3f, but can't always do the converse
#endif
};

#endif
