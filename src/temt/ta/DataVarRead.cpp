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

#include "DataVarRead.h"
#include <DataCol>
#include <DataTable>
#include <Program>
#include <taProject>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DataVarRead);

void DataVarRead::Initialize() {
  writeToDataTable = false;   // MUST SET
}

String DataVarRead::GetDisplayName() const {
  String rval;
  String row_var_name = "(ERROR: not set!)";
  if((bool)row_var)
    row_var_name = row_var->name;

  rval = "From table: ";

  if(data_var)
    rval += data_var->name + ",";
  else
    rval += "(SELECT: data table),";

  if (!column_name.empty())
    rval += " column: " + column_name + ",";
  else
    rval += "(ENTER: column name),";

  if(row_spec  == CUR_ROW)
    rval += " cur_row,";
  else if(row_spec == ROW_NUM)
    rval += " row_num: " + row_var_name + ",";
  else
    rval += " row_val: " + row_var_name + ",";

    rval += " To variable: ";

  if(var)
    rval += var->name + " ";

  return rval;
}

bool DataVarRead::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  if(dc.startsWith("from table:")) return true;
  return false;
}

bool DataVarRead::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true; // nothing we can do

  String remainder = code.after(": ");
  if(remainder.empty()) return true;
  String dtnm = remainder.before(" ");
  if(!data_var || data_var->name != dtnm) {
    data_var = FindVarNameInScope(dtnm, false); // don't make
  }

  remainder = remainder.after(dtnm.length());

  if(remainder.empty()) return true;

  String_Array strs;
  strs.FmDelimString(remainder, " ");
  if(strs.size > 0) {
    column_name = strs[0]; // would be good to verify
  }
  if(strs.size > 1 && row_spec == DataVarBase::CUR_ROW) {
    var = FindVarNameInScope(strs[1], false); // don't make
  }
  if(strs.size > 2) { // not CUR_ROW so there is the row_var to get
    if (strs[1].contains(':')) {
      String rwvr(strs[1].after(':'));
      String rwsp(strs[1].before(':'));
      row_var = FindVarNameInScope(rwvr, false); // don't make
      rwsp.downcase();
      row_spec = StringToRowType(rwsp);
    }
    else {
      row_var = FindVarNameInScope(strs[1], false); // don't make
    }
    var = FindVarNameInScope(strs[2], false); // don't make
  }
  SigEmitUpdated();
  return true;
}
