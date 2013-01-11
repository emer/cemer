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

#ifndef taiType_List_h
#define taiType_List_h 1

// parent includes:
#include <taPtrList>
#include <taiType>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiType_List : public taPtrList<taiType> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void  El_Done_(void* it)      { delete (taiType*)it; }

public:
  ~taiType_List()              { Reset(); }
};

#endif // taiType_List_h
