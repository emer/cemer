// Copyright 2017, Regents of the University of Colorado,
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

#include "DataOpEl.h"
#include <ProgEl>
#include <DataTable>
#include <taProject>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataOpEl);


void DataOpEl::Initialize() {
  col_lookup = NULL;
  col_idx = -1;
}

void DataOpEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(col_lookup) {
    col_name = col_lookup->name;
    taBase::SetPointer((taBase**)&col_lookup, NULL); // reset as soon as used -- just a temp guy!
  }
  UpdateName();
}

void DataOpEl::MakeNameUnique() {
  if(owner && owner->InheritsFrom(&TA_taList_impl)) {
    ((taList_impl*)owner)->MakeElNameUnique(this); // just this guy
  }
}

String DataOpEl::GetListIdxSuffix() {
  if(owner && owner->InheritsFrom(&TA_taList_impl)) {
    return String("_") + (String)((taList_impl*)owner)->FindEl_(this);
  }
  return _nilString;
}

void DataOpEl::SetColName(const String& nm) {
  col_name = nm;
  UpdateName();
}

String DataOpEl::GetDisplayName() const {
  return GetName();
}

String DataOpEl::GetName() const {
  return col_name;
}

bool DataOpEl::SetName(const String& nm) {
  // Ensure name is a legal C-language identifier.
  String new_name = taMisc::StringCVar(nm);
  if (col_name == new_name) return true;
  col_name = new_name;
  return true;
}

bool DataOpEl::BrowserEditSet(const String& new_val_str, int move_after) {
  if(move_after != -11) {
    taProject* proj = GetMyProj();
    if(proj) {
      proj->undo_mgr.SaveUndo(this, "BrowserEditSet", this);
    }
  }
  bool rval = SetName(new_val_str);
  UpdateAfterEdit();
  return rval;
}

void DataOpEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(col_name.empty(), quiet, rval,"col_name is empty");
  //  CheckError(col_idx < 0, quiet, rval,"could not find", col_name,"in datatable");
  // note: an error can be too strong and prevent transitional code from running -- sometimes
  // at compile time the names aren't right, but they later end up being ok..
  ProgEl* pel = GET_MY_OWNER(ProgEl);
  if(pel && pel->HasProgFlag(ProgEl::QUIET))
    return;
  // TestWarning(col_idx < 0, "CheckConfig", "could not find", col_name,"in datatable");
  // see above -- this is not useful..
}

void DataOpEl::SetDataTable(DataTable* dt) {
  data_table = dt;
}

void DataOpEl::GetColumns(DataTable* dt) {
  if(!dt) return;
  col_idx = dt->FindColNameIdx(col_name);
  DataCol* da = NULL;
  if(col_idx >= 0)
    da = dt->data[col_idx];
  taBase::SetPointer((taBase**)&col_lookup, da);
}

void DataOpEl::ClearColumns() {
  taBase::SetPointer((taBase**)&col_lookup, NULL);
}

void DataOpEl::GetListForCompletion(const MemberDef* md, String_Array& list) {
  if (data_table) {
    FOREACH_ELEM_IN_LIST(DataCol, col, data_table->data) {
      list.Add(col->name);
    }
  }
}
