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

#ifndef taiWidgetActions_List_h
#define taiWidgetActions_List_h 1

// parent includes:
#include <taPtrList>
#include <taiWidgetActions>

// member includes:
#include <taiWidgetMenu>

// declare all other types mentioned but not required to include:


class TA_API taiWidgetActions_List : public taPtrList<taiWidgetActions> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void  El_Done_(void* it)      { if (own_items) delete (taiWidgetMenu*)it; }

public:
  bool own_items; //generally false
  taiWidgetActions_List()            { own_items = false; }
  ~taiWidgetActions_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiWidgetActions*)it)->mlabel); }
};

#endif // taiWidgetActions_List_h
