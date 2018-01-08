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

#ifndef int_FixedArray_h
#define int_FixedArray_h 1

// parent includes:
#include <taFixedArray>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(int_FixedArray);

class TA_API int_FixedArray: public taFixedArray<int> {
  // #INLINE #NO_TOKENS
  TA_FIXED_ARRAY_FUNS(int_FixedArray, int)
};

#endif // int_FixedArray_h
