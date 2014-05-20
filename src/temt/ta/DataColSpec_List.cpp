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

#include "DataColSpec_List.h"
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(DataColSpec_List);

const KeyString DataColSpec_List::GetListColKey(int col) const 
{
  return (KeyString)col;
}

String DataColSpec_List::GetColHeading(const KeyString& key) const {
  return GetColHeadingIdx(key.toInt());
}

String DataColSpec_List::GetColHeadingIdx(int col) const {
  static String hd_num("Chan #");
  static String hd_nm("Chan Name");
  static String hd_vt("Val Type");
  static String hd_mat("Is Mat");
  static String hd_gm("Geom");
  
  switch (col) {
  case 0: return hd_num;
  case 1: return hd_nm;
  case 2: return hd_vt;
  case 3: return hd_mat;
  case 4: return hd_gm;
  default: return _nilString; // compiler food
  }
}

bool DataColSpec_List::UpdateDataTableCols(DataTable* dt, bool remove_orphans) {
  if (!dt) return false;

  bool any_changes = false;
  DataCol* dc;
  DataColSpec* ds;
  dt->MarkCols(); // mark all
  for(int si = 0; si < size; ++si) {
    ds = FastEl(si);
    dc = dt->FindColFmSpec(ds);
    if(!dc) {
      any_changes = true;
      dc = dt->NewColFmSpec(ds);
      if(dc->col_idx != si) {
        dt->MoveCol(dc->col_idx, si); // move into place
        ds->col_num = si;
      }
    }
    else {
      if(dc->col_idx != si) {
        any_changes = true;
        dt->MoveCol(dc->col_idx, si); // move into place
        ds->col_num = si;
      }
    }
    dc->SetColFlag(DataCol::PIN); // indicates that it was used
  }

  if(remove_orphans) {
    int sz = dt->cols();
    dt->RemoveOrphanCols();
    if(sz != dt->cols())
      any_changes = true;
  }
  return any_changes;
}
