// Copyright 2017, Regents of the University of Colorado,
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

#ifndef taMatrix_PList_h
#define taMatrix_PList_h 1

// parent includes:
#include <taMatrix>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taMatrix_PList);

class TA_API taMatrix_PList: public taPtrList<taMatrix> {
  // simple list for keeping track of slices
INHERITED(taPtrList<taMatrix>)
public:

  taMatrix_PList() {}
};

#endif // taMatrix_PList_h
