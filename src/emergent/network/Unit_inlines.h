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

inline Network* Unit::own_net() const {
  Layer* ol = own_lay();
  if(!ol) return NULL;
  return ol->own_net;
}

inline bool Unit::lay_lesioned() const {
  return own_lay()->lesioned();
}

inline Unit_Group* Unit::own_subgp() const {
  if(!owner || !owner->GetOwner()) return NULL;
  if(owner->GetOwner()->InheritsFrom(&TA_Layer)) return NULL; // we're owned by the layer really
  return (Unit_Group*)owner;
}

inline int Unit::UnitGpIdx() const {
  return own_lay()->UnitGpIdx((Unit*)this);
}

#endif // Unit_inlines_h
