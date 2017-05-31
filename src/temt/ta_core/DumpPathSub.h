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

#ifndef DumpPathSub_h
#define DumpPathSub_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class taBase; //
class TypeDef; //

class TA_API DumpPathSub {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Path element substitution
public:
  TypeDef*	type;
  taBase*	parent;		// for relative paths
  String	old_path;
  String	new_path;

  DumpPathSub(TypeDef* td, taBase* par, const String& o, const String& n);
};

#endif // DumpPathSub_h
