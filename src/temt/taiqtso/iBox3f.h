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

#ifndef iBox3f_h
#define iBox3f_h 1

#include "taiqtso_def.h"

#include <iVec3f>

#ifdef TA_OS_WIN
// following for msvc
# ifdef near
#   undef near
# endif
# ifdef far
#   undef far
# endif
#endif

#ifdef TA_USE_INVENTOR
class SbBox3f;          // #IGNORE
#endif

/* Notes:
   1. all classes below are value-semantics, and use the implicit copy and = operators.
   2. MAKETA cannot boggle the : class(): x(), y() {} form of constructor initialialization,
      so they have been moved to the .cc file.
   3. DO NOT add any virtual methods to these value-semantic classes
*/


/* Note iBox3f is used as a bounding box in a normally rotated coordinate system, such that the box planes
   are coincident with the coordinate planes
*/

class TAIQTSO_API iBox3f { // #NO_TOKENS #INSTANCE #EDIT_INLINE 3d box, typically used for a bounding box in a normalized coordinate system
public:
  iVec3f        min; // minimum point
  iVec3f        max;  // maximum point

  iVec3f        center() const {return iVec3f((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);}
    // center point of the box
  float         height() const {return (max.y - min.y);}
  float         width() const {return (max.x - min.x);}
  float         depth() const {return (max.z - min.z);}
  float         bottom() const {return min.y;}
  float         top() const {return max.y;}
  float         left() const {return min.x;}
  float         right() const {return max.x;}
  float         far() const {return min.z;} //#IGNORE
  float         near() const {return max.z;} //#IGNORE
  iVec3f        size() const {return iVec3f(max.x - min.x, max.y - min.y, max.z - min.z);}  // size of the box in each dimension
  bool          isZero() const { return (min.isZero() && max.isZero());}

  void          setValue(float min_x, float min_y, float min_z,  float max_x, float max_y, float max_z)
    {min.x = min_x; min.y = min_y; min.z = min_z;  max.x = max_x; max.y = max_y; max.z = max_z;}
  void          setValue(const iVec3f& min_, const iVec3f& max_)
    {min = min_;   max = max_;}
  void          setValueCS(float center_x, float center_y, float center_z, float size_x, float size_y, float size_z)
  {min.x = center_x - (size_x / 2); min.y = center_y - (size_y / 2); min.z = center_z - (size_z / 2);
  max.x = min.x + size_x; max.y =  min.y + size_y; max.z =  min.z + size_z; }
  // set the value by specifying the center and the size -- can only specify boxes with planes parallel to the coord axes
  void          setValueCS(const iVec3f& center_, const iVec3f& size_)
  {min.x = center_.x - (size_.x / 2); min.y = center_.y - (size_.y / 2); min.z = center_.z - (size_.z / 2);
   max.x = min.x + size_.x; max.y =  min.y + size_.y; max.z =  min.z + size_.z; }
  // set the value by specifying the center and the size -- can only specify boxes with planes parallel to the coord axes

  void          MaxOf(const iBox3f& val) {MaxOf(*this, val);} // #IGNORE sets this instance to most expansive of itself and val
  void          MaxOf(const iBox3f& val1, const iBox3f& val2); //#IGNORE sets this instance to most expansive of inputs

  iBox3f() {}
  iBox3f(float val) {min = val; max = val;}
  iBox3f(float min_x, float min_y, float min_z,  float max_x, float max_y, float max_z)
    {min.x = min_x; min.y = min_y; min.z = min_z;  max.x = max_x; max.y = max_y; max.z = max_z;}
  iBox3f(const iVec3f& min_, const iVec3f& max_) {min = min_;   max = max_;}
  iBox3f(const iBox3f& val) {min = val.min;   max = val.max;}

  iBox3f&       operator=(float val) {min = val; max = val; return *this;}
  iBox3f&       operator=(const iBox3f& val) {min = val.min;   max = val.max; return *this;}

#ifdef TA_USE_INVENTOR
  operator SbBox3f() const; // we can convert to a SbBox3f, but can't always do the converse
#endif
};

#endif // iBox3f_h
