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

#ifndef ProgLibEl_List_h
#define ProgLibEl_List_h 1

// parent includes:
#include <ProgLibEl>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ProgLibEl_List);

class TA_API ProgLibEl_List : public taList<ProgLibEl> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of program library elements
INHERITED(taList<ProgLibEl>)
public:
  TA_SIMPLE_BASEFUNS(ProgLibEl_List);
protected:

private:
  void  Initialize();
  void  Destroy() { Reset(); CutLinks(); }
};

#endif // ProgLibEl_List_h
