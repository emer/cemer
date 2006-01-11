// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

// ta_variant.cpp -- variant datatype

#include "ta_variant.h"

#include "ta_matrix.h"

Variant::Variant(const Variant &cp)
  :type(cp.type), is_null(cp.is_null)
{
  switch (cp.type) {
  case T_String: new(&d.str)String(cp.getString()); break;
  case T_taBase:
  case T_taMatrix: d.tab = NULL; taBase::SetPointer(&d.tab, cp.d.tab); break;
  default: d.i64 = cp.d.i64; // just copy 64 bits, valid for all other types
  }
}

Variant::~Variant() { 
  switch (type) {
  case T_String: getString().~String(); break;
  case T_taBase:
  case T_taMatrix: taBase::DelPointer(&d.tab); break;
  default: break; // compiler food
  }
  type = T_Invalid; is_null = true; // helps avoid hard-to-find zombie problems
}

