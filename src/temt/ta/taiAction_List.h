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

#ifndef taiAction_List_h
#define taiAction_List_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiAction_List : public taPtrList<taiAction> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  int                   count() {return size;} // for Qt-api compat

  taiAction*            PeekNonSep(); // returns last non-separator item, if any

  ~taiAction_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiAction*)it)->text()); }

protected:
  override void*        El_Ref_(void* it)  { ((taiAction*)it)->nref++; return it; }     // when pushed
  override void*        El_unRef_(void* it)  { ((taiAction*)it)->nref--; return it; }   // when popped
  override void         El_Done_(void* it);
};

#endif // taiAction_List_h
