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

#include "NameVar.h"

bool NameVar::Parse(const String& raw, String& name, String& val) {
  String namet = trim(raw.before('='));
  if (namet.length() == 0) return false;
  name = namet;
  val = trim(raw.after('='));
  return true;
}

void NameVar::SetFmStr(const String& val) {
  String valt;
  if (Parse(val, name, valt)) {
    if(value.type() == Variant::T_Invalid) {
      value = valt;             // sets as string
    }
    else {
      value.updateFromString(valt);
    }
  }
  // else error!  todo.
}

