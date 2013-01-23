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

#ifndef Member_List_h
#define Member_List_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class MemberDef; //

TypeDef_Of(Member_List);

class TA_API Member_List: public taPtrList<MemberDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS simple list of members
public:
  Member_List() {}
};

#endif // Member_List_h
