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

#ifndef taHashEl_h
#define taHashEl_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:

//typedef uintptr_t taHashVal;
typedef unsigned long taHashVal; // this is a hash value

class TA_API  taHashEl {
  // ##NO_TOKENS holds information for one entry of the hash table
public:
  taHashVal     hash_code;      // hash-code for looking up
  int           value;          // value associated with hash code (e.g., index of item in list)
  String        hashed_str;     // source value for the hash code if a string -- this is necessary because the hash code is not guaranteed to be unique..

  void  Initialize()    { hash_code = 0; value = -1; }
  taHashEl()            { Initialize(); }
  taHashEl(taHashVal hash, int val, const String& str = _nilString)
    { hash_code = hash; value = val; hashed_str = str; }
};

#endif // taHashEl_h
