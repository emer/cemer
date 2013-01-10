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

#include "SArg_Array.h"

using namespace std;

void SArg_Array::Initialize() {
}

void SArg_Array::InitLinks() {
  String_Array::InitLinks();
  taBase::Own(labels, this);
}

void SArg_Array::UpdateAfterEdit() {
  labels.SetSize(size);
  inherited::UpdateAfterEdit();
}

void SArg_Array::Copy_(const SArg_Array& cp) {
  labels = cp.labels;
}

int SArg_Array::Dump_Save_Value(ostream& strm, taBase* par, int indent) {
  int rval = String_Array::Dump_Save_Value(strm, par, indent);
  strm << "};" << endl;
  int rv2 = labels.Dump_Save_Value(strm, this, indent+1);
  return (rval && rv2);
}

int SArg_Array::Dump_Load_Value(istream& strm, taBase* par) {
  int rval = String_Array::Dump_Load_Value(strm, par);
  int c = taMisc::skip_white(strm, true); // peek
  if(c == '{') {
    labels.Dump_Load_Value(strm, this);
  }
  return rval;
}

bool SArg_Array::HasValue(const String& key) const {
  return (labels.FindEl(key) >= 0);
}

String SArg_Array::GetValue(const String& key) const {
  int idx = labels.FindEl(key);
  if (idx >= 0) return SafeEl(idx);
  else return _nilString;
}

void SArg_Array::SetValue(const String& key, const String& value) {
  int idx = labels.FindEl(key);
  if (idx >= 0) {
    if(TestWarning((idx >= size), "SetValue",
                   "Consistency error: SArg_Array used as key/values has missing entry for key:", key)) {//nop
    }
    else {
      FastEl(idx) = value;
    }
  } else {
    labels.Add(key);
    Add(value);
  }
#ifdef DEBUG // helpful when viewing stuff in gui
  DataChanged(DCR_ITEM_UPDATED);
#endif
}
