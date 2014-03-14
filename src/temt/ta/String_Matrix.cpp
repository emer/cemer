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

#include "String_Matrix.h"
#include <taMisc>
#include <String_Array>

TA_BASEFUNS_CTORS_DEFN(String_Matrix);
SMARTREF_OF_CPP(String_Matrix);


using namespace std;

const String String_Matrix::blank;

int String_Matrix::defAlignment() const {
  return Qt::AlignLeft;
}

int String_Matrix::Dump_Load_Item(istream& strm, int idx) {
  int c = taMisc::skip_till_start_quote_or_semi(strm);
  if (c == '\"') {
    c = taMisc::read_till_end_quote_semi(strm);
  }
  if (c != EOF) {
    FastEl_Flat(idx) = taMisc::LexBuf;
  }
  return c;
}

void String_Matrix::Dump_Save_Item(ostream& strm, int idx) {
// note: we don't write "" for empty
  taMisc::write_quoted_string(strm, FastEl_Flat(idx));
}

void String_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilString;
  }
}

String String_Matrix::ToDelimString(const String& delim) {
  String rval;
  for (int i = 0; i < size; ++i) {
    rval += FastEl_Flat(i);
    if(i < size-1) rval += delim;
  }
  return rval;
}

void String_Matrix::FmDelimString(const String& str, const String& delim) {
  Reset();
  String_Array ar;
  ar.FmDelimString(str, delim);
  if(dims() == 0) {
    SetGeom(1, ar.size);
  }
  else {
    if(size < ar.size) {
      int extra = ((ar.size - size) / FrameSize());
      AddFrames(extra);
      while(size < ar.size)
        AddFrames(1);
    }
  }
  int idx = 0;
  String remainder = str;
  while(remainder.nonempty()) {
    if(remainder.contains(delim)) {
      Set_Flat(remainder.before(delim), idx++);
      remainder = remainder.after(delim);
    }
    else {
      Set_Flat(remainder, idx++);
      remainder = _nilString;
    }
  }
}
