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

#ifndef taiMenu_List_h
#define taiMenu_List_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiMenu_List : public taPtrList<taiActions> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void  El_Done_(void* it)      { if (own_items) delete (taiMenu*)it; }

public:
  bool own_items; //generally false
  taiMenu_List()            { own_items = false; }
  ~taiMenu_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiActions*)it)->mlabel); }
};

#endif // taiMenu_List_h
