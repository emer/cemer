// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef FixedWidthColSpec_List_h
#define FixedWidthColSpec_List_h 1

// parent includes:
#include <FixedWidthColSpec>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(FixedWidthColSpec_List);

class TA_API FixedWidthColSpec_List: public taList<FixedWidthColSpec> {
  // ##CAT_Data
INHERITED(taList<FixedWidthColSpec>)
public:
  TA_BASEFUNS_NOCOPY(FixedWidthColSpec_List);
private:
  void  Initialize();
  void  Destroy() {}
};

#endif // FixedWidthColSpec_List_h
