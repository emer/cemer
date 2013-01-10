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

#include "VEObjCarousel.h"

void VEObjCarousel::Initialize() {
  cur_obj_no = -1;
  SetBodyFlag(FM_FILE);
  obj_switch = NULL;
}

// Destroy, LoadObjs are in in ta_virtenv_qtso

bool VEObjCarousel::ViewObjNo(int obj_no) {
  if(TestError(!(bool)obj_table, "ViewObjNo", "no obj_table data table set -- must set this first -- falling back on basic body render for now!"))
    return false;
  if(TestError(obj_no >= obj_table->rows, "ViewObjNo", "obj_no out of range -- only:",
               String(obj_table->rows), "rows in the obj_table data table"))
    return false;
  cur_obj_no = obj_no;
  obj_fname = obj_table->GetVal("FileName", cur_obj_no);
  DataChanged(DCR_ITEM_UPDATED); // update displays..
  return true;
}

bool VEObjCarousel::ViewObjName(const String& obj_nm) {
  if(TestError(!(bool)obj_table, "ViewObjName", "no obj_table data table set -- must set this first -- falling back on basic body render for now!"))
    return false;
  int obj_no = obj_table->FindVal(obj_nm, "FileName");
  if(TestError(obj_no < 0, "ViewObjName", "object file named:", obj_nm,
               "not found in the obj_table data table"))
    return false;
  cur_obj_no = obj_no;
  obj_fname = obj_nm;
  DataChanged(DCR_ITEM_UPDATED); // update displays..
  return true;
}

