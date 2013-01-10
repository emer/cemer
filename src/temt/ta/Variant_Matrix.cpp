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

#include "Variant_Matrix.h"

using namespace std;

const Variant Variant_Matrix::blank;

int Variant_Matrix::Dump_Load_Item(istream& strm, int idx) {
  int c;
  Variant& val = FastEl_Flat(idx);
  if (!val.Dump_Load_Type(strm, c)) goto end;
  if (c == EOF) return c;

  switch (val.type()) {
  case Variant::T_Invalid: // no content
    c = taMisc::skip_white(strm); // should read ;
    break;
  case Variant::T_Bool:
  case Variant::T_Int:
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
    c = taMisc::read_till_semi(strm);
    val.updateFromString(taMisc::LexBuf);
    break;
  case Variant::T_Char: {// reads chars as ints
    c = taMisc::read_till_semi(strm);
    int sc = taMisc::LexBuf.toInt();
    val.setChar((char)sc, val.isNull());
    }
    break;

  case Variant::T_String:
    //note: an empty string was not written
    c = taMisc::skip_till_start_quote_or_semi(strm);
    if (c == '\"') {
      c = taMisc::read_till_end_quote_semi(strm);
      val.updateFromString(taMisc::LexBuf);
    }
    break;
//  case Variant::T_Ptr:  //not streamable
//TODO: maybe we should issue a warning???
//    break;

  case Variant::T_Base:
  case Variant::T_Matrix:
    //note: an empty string was not written
    c = taMisc::skip_till_start_quote_or_semi(strm);
    if (c == '\"') {
      c = taMisc::read_till_end_quote_semi(strm);
      val.updateFromString(taMisc::LexBuf);
    }
    break;
  default:
    c = taMisc::read_till_semi(strm);
    break;
  }

end:
  return c;
}

void Variant_Matrix::Dump_Save_Item(ostream& strm, int idx) {
  const Variant& val = FastEl_Flat(idx);
  val.Dump_Save_Type(strm);
  switch (val.type()) {
  case Variant::T_Invalid: break; // no content
  case Variant::T_Bool:
  case Variant::T_Int:
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
    strm << ' ' << val.toString();
    break;
  case Variant::T_Double:
    strm << ' ' << String(val.toDouble(), "%.16lg");
    break;
  case Variant::T_Char: // write chars as ints
    strm << ' ' << val.toInt();
    break;

  case Variant::T_String:
    strm << ' ';
    //note: doesn't write empty strings
    taMisc::write_quoted_string(strm, val.toString());
    break;
  case Variant::T_Ptr:  //not streamable
//TODO: maybe we should issue a warning???
    break;

  case Variant::T_Base:
  case Variant::T_Matrix:
    strm << ' ';
    //note: doesn't write empty strings
    taMisc::write_quoted_string(strm, val.toString());
    break;

  default:
    break;
  }
}

void Variant_Matrix::ReclaimOrphans_(int from, int to) {
  for (int i = from; i <= to; ++i) {
    el[i] = _nilVariant;
  }
}

