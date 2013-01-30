// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef Unit_inlines_h
#define Unit_inlines_h 1

// parent includes:
#include <Unit>

// member includes:
#include <Layer>

// declare all other types mentioned but not required to include:


inline Layer* Unit::own_lay() const {
  return ((Unit_Group*)owner)->own_lay;
}

inline bool Unit::lay_lesioned() const {
  return own_lay()->lesioned();
}

#endif // Unit_inlines_h
