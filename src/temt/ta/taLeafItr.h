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

#ifndef taLeafItr_h
#define taLeafItr_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class taGroup_impl; // 


class   TA_API taLeafItr {              // contains the indicies for iterating over leafs
public:
  taGroup_impl*        cgp;            // pointer to current group
  int           g;              // index of current group
  int           i;              // index of current leaf element
};

#endif // taLeafItr_h
