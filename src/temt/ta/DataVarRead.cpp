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
#include <NameVar_PArray>

TA_BASEFUNS_CTORS_DEFN(DataVarRead);

void DataVarRead::Initialize() {
  writeToDataTable = false;   // MUST SET
}

String DataVarRead::GetDisplayName() const {
  String row_var_name;
  if((bool)row_var)
    row_var_name = row_var->name;

  String rval = "From table: ";

  if(data_var)
    rval += " table=" + data_var->name + " ";
  else
    rval += " table=? ";

  if (!column_name.empty())
    rval += " column=" + column_name + " ";
  else
    rval += " column=? ";

  if(row_spec  == CUR_ROW) {
    rval += " row=cur_row ";
  }
  else if(row_spec == ROW_NUM) {
    if (row_var)
      rval += " row_num=" + row_var_name + " ";
    else
      rval += " row_num=? ";
  }
  else {
    if (row_var)
      rval += " row_val=" + row_var_name + " ";
    else
      rval += " row_val=? ";
  }

  if(var)
    rval += " var=" + var->name + " ";
  else
    rval += " var=? ";

  return rval;
}

//String DataVarRead::GetDisplayName() const {
//  String rval;
//  String row_var_name = "(ERROR: not set!)";
//  if((bool)row_var)
//    row_var_name = row_var->name;
//
//  rval = "From table: ";
//
//  if(data_var)
//    rval += data_var->name + ",";
//  else
//    rval += "(SELECT: data table),";
//
//  if (!column_name.empty())
//    rval += " column: " + column_name + ",";
//  else
//    rval += "(ENTER: column name),";
//
//  if(row_spec  == CUR_ROW)
//    rval += " cur_row,";
//  else if(row_spec == ROW_NUM)
//    rval += " row_num: " + row_var_name + ",";
//  else
//    rval += " row_val: " + row_var_name + ",";
//
//    rval += " To variable: ";
//
//  if(var)
//    rval += var->name + " ";
//
//  return rval;
//}

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

  String remainder = code.after(":");
  if(remainder.empty()) return true;

  NameVar_PArray nv_pairs;
  ToNameValuePairs(remainder, nv_pairs);

  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();

    if (name.startsWith("tab")) {
      data_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("var")) {
      var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("row")) {
      row_spec = StringToRowType(name);
      row_var = FindVarNameInScope(value, false); // don't make
    }
    else if (name.startsWith("col")) {
      column_name = value;
    }
  }

  SigEmitUpdated();
  return true;
}

//bool DataVarRead::CvtFmCode(const String& code) {
//  String dc = code;  dc.downcase();
//  String tbn = GetToolbarName(); tbn.downcase();
//  String tn = GetTypeDef()->name; tn.downcase();
//  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true; // nothing we can do
//
//  String remainder = code.after(": ");
//  if(remainder.empty()) return true;
//  String dtnm = remainder.before(" ");
//  if(!data_var || data_var->name != dtnm) {
//    data_var = FindVarNameInScope(dtnm, false); // don't make
//  }
//
//  remainder = remainder.after(dtnm.length());
//
//  if(remainder.empty()) return true;
//
//  String_Array strs;
//  strs.FmDelimString(remainder, " ");
//  if(strs.size > 0) {
//    column_name = strs[0]; // would be good to verify
//  }
//  if(strs.size > 1 && row_spec == DataVarBase::CUR_ROW) {
//    var = FindVarNameInScope(strs[1], false); // don't make
//  }
//  if(strs.size > 2) { // not CUR_ROW so there is the row_var to get
//    if (strs[1].contains(':')) {
//      String rwvr(strs[1].after(':'));
//      String rwsp(strs[1].before(':'));
//      row_var = FindVarNameInScope(rwvr, false); // don't make
//      rwsp.downcase();
//      row_spec = StringToRowType(rwsp);
//    }
//    else {
//      row_var = FindVarNameInScope(strs[1], false); // don't make
//    }
//    var = FindVarNameInScope(strs[2], false); // don't make
//  }
//  SigEmitUpdated();
//  return true;
//}



void DataVarRead::ToNameValuePairs(const String& str, NameVar_PArray& nv_array) {
  int pos;
  String strCopy(str);
  strCopy = trimr(strCopy); // remove any tailing white space

  bool parse = true;
  while (parse) {
    pos = 0; // start at beginning of string or what is remaining
    pos = strCopy.index('=', pos);
    if (pos == -1) {  // no more pairs
      parse = false;
      break;
    }
    String name = strCopy.before(pos);
    name = trim(name);

    strCopy = strCopy.after(pos); // lop off what we processed
    strCopy = triml(strCopy);  // trim leading on remaining

    String value;
    pos = 0;  // reset before each search
    pos = strCopy.index(' ', pos); // find white space between name/value pairs (none if last n/v pair)
    if (pos == -1) {  // no space
      value = strCopy;  // last n/v pair
    }
    else {
      value = strCopy.before(pos);  // get chars up to next space
    }
    value = trim(value);
    nv_array.Add(NameVar(name, value));

    if (pos != -1) {  // more pairs - lop off what we got
      strCopy = strCopy.after(pos);
    }
    else {
      parse = false;
    }
  }
}
