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

#ifndef taiMemberWidgets_h
#define taiMemberWidgets_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taiWidget_List>
#include <Member_List>

// declare all other types mentioned but not required to include:


class TA_API taiMemberWidgets {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS contains data_els for one member of List
public:
  TypeDef*       typ;
  taBase*        cur_base;
  Member_List    memb_el; // members
  taiWidget_List data_el; // data elements for members

  taiMemberWidgets(TypeDef* tp, taBase* base);
  virtual ~taiMemberWidgets();
};

#endif // taiMemberWidgets_h
