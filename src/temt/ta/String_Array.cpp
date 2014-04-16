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

#include "String_Array.h"

#include <QStringList>

TA_BASEFUNS_CTORS_DEFN(String_Array);
SMARTREF_OF_CPP(String_Array)

const String String_Array::blank = "";

#ifdef TA_USE_QT
void String_Array::ToQStringList(QStringList& sl) {
  sl.clear();
  for (int i = 0; i < size; ++i) {
    sl.append(FastEl(i).toQString());
  }
}
#endif // TA_USE_QT

String String_Array::ToDelimString(const String& delim) {
  String rval;
  for (int i = 0; i < size; ++i) {
    rval += FastEl(i);
    if(i < size-1) rval += delim;
  }
  return rval;
}

void String_Array::FmDelimString(const String& str, const String& delim, bool reset_first) {
  if(reset_first) Reset();
  if (delim == "") {
    for (int i = 0; i < str.length(); i++) {
      Add(str.at(i,1));
    }
  }
  else {
    String remainder = str;
    while(remainder.nonempty()) {
      if(remainder.contains(delim)) {
  	Add(remainder.before(delim));
  	remainder = remainder.after(delim);
      }
      else {
  	Add(remainder);
  	remainder = _nilString;
      }
    }
  }
}

void String_Array::Split(const String& str, const String& delim) {
  FmDelimString(str, delim);
}
