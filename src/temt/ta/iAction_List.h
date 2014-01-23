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

#ifndef iAction_List_h
#define iAction_List_h 1

// parent includes:
#include <taPtrList>
#include <iAction>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API iAction_List : public taPtrList<iAction> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  int                   count() {return size;} // for Qt-api compat

  iAction*            PeekNonSep(); // returns last non-separator item, if any

  ~iAction_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((iAction*)it)->text()); }

protected:
  void*        El_Ref_(void* it)  override { ((iAction*)it)->nref++; return it; }     // when pushed
  void*        El_unRef_(void* it)  override { ((iAction*)it)->nref--; return it; }   // when popped
  void         El_Done_(void* it) override;
};

#endif // iAction_List_h
