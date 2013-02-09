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

#ifndef FunCallItem_h
#define FunCallItem_h 1

// parent includes:
#include "ta_def.h"
#include <taString>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 


taTypeDef_Of(FunCallItem);

class TA_API FunCallItem {
// #NO_INSTANCE
public:
  taBase*               it;
  String                fun_name;
  FunCallItem(taBase* it_, const String& fn) {it = it_; fun_name = fn;}
};

#endif // FunCallItem_h
