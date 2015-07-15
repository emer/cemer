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

#ifndef VPUnref_h
#define VPUnref_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class MemberDef; //
class taBase; //


class TA_API VPUnref {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS stores a Value that couldn't be cashed out only works with taBase elements because they have paths..
public:
  String	name;		// just for looking up purposes
  MemberDef*	memb_def;	// memberdef of the pointer
  void* 	base;		// location of ptr to set, usually taBase**, but could be taSmartRef*
  taBase* 	parent;		// location's parent to update
  String 	path;

  taBase* 	Resolve();

  VPUnref(void* base, taBase* par, const String& p, MemberDef* md = NULL);
};

#endif // VPUnref_h
