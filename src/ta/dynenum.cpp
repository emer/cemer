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



#include "dynenum.h"

void DynEnum::Initialize() {
  value_idx = -1;
  SetBaseType(&TA_DynEnumItem);
}

void DynEnum::Copy_(const DynEnum& cp) {
  value_idx = cp.value_idx;
}

void DynEnum::DataChanged(int dcr, void*, void*) {
  cerr << "dyn enum: " << name << " invalidated due to type change" << endl;
  value_idx = -1;
  OrderItems();
}

int DynEnum::NumVal() const {
  if(value_idx < 0 || value_idx >= size) return -1;
  return FastEl(value_idx)->value;
}

const String DynEnum::NameVal() const {
  if(value_idx < 0 || value_idx >= size) return "";
  return FastEl(value_idx)->name;
}

int DynEnum::FindNumIdx(int val) const {
  for(int i=0;i<size;i++)
    if(FastEl(i)->value == val) return i;
  return -1;
}

void DynEnum::OrderItems() {
  if(size == 0) return;
  int prval = FastEl(0)->value;
  for(int i=1;i<size;i++) {
    DynEnumItem* it = FastEl(i);
    if(it->value <= prval) {
      it->value = prval + 1;
    }
    prval = it->value;
  }
}

ostream& DynEnum::OutputType(ostream& strm, int indent) const {
  for(int i=0;i<size;i++) {
    DynEnumItem* it = FastEl(i);
    taMisc::fmt_sep(strm, it->name, 0, indent);
    strm << " = " << it->value << ";";
    if(!it->desc.empty())
      strm << "  // " << it->desc;
    strm << "\n";
  }
  return strm;
}
