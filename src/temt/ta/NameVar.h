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

#ifndef NameVar_h
#define NameVar_h 1

// parent includes:

// member includes:
#include <Variant>
#include <taString>

// declare all other types mentioned but not required to include:


TypeDef_Of(NameVar);

class TA_API NameVar {
  // a name-value representation, using a String and a Variant
public:
  static bool	Parse(const String& raw, String& name, String& value);
  // pulls the nm and val out of name=value (ignores ws around the =); true if = found
  
  String	name;		// the name
  Variant	value;		// the value

  String	GetStr() const	{ return name + "=" + value.toString(); }
  void		SetFmStr(const String& val);

  bool	operator>(const NameVar& cm) { return value > cm.value; }
  bool	operator<(const NameVar& cm) { return value < cm.value; }
  bool	operator>=(const NameVar& cm) { return value >= cm.value; }
  bool	operator<=(const NameVar& cm) { return value <= cm.value; }
  bool	operator==(const NameVar& cm) { return value == cm.value; }

  NameVar(const NameVar& cp) { name = cp.name; value = cp.value; }
  NameVar(const String& nm, const Variant& vl) { name = nm; value = vl; }
  NameVar(const char* nm, const Variant& vl) { name = nm; value = vl; }
  NameVar()	{ };
  ~NameVar()	{ };
};

#endif // NameVar_h
