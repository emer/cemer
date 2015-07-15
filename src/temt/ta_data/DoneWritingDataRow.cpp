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

#include "DoneWritingDataRow.h"
#include <Program>
#include <NameVar_PArray>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(DoneWritingDataRow);


void DoneWritingDataRow::Initialize() {
}

String DoneWritingDataRow::GetDisplayName() const {
  String rval = "DoneWritingDataRow: ";
  if(data_var)
    rval += " table = " + data_var->name + " ";
  else
    rval += " table = ? ";
  return rval;
}

bool DoneWritingDataRow::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  String dc = code;  dc.downcase();
  String tbn = GetToolbarName(); tbn.downcase();
  String tn = GetTypeDef()->name; tn.downcase();
  if(dc.startsWith(tbn) || dc.startsWith(tn)) return true;
  if(dc.startsWith("donewriting") || dc.startsWith("done writing")) return true;
  return false;
}

bool DoneWritingDataRow::CvtFmCode(const String& code) {
  String dc = code;  dc.downcase();
  String remainder = code.after(":");
  if(remainder.empty()) return true;
  
  NameVar_PArray nv_pairs;
  taMisc::ToNameValuePairs(remainder, nv_pairs);
  
  for (int i=0; i<nv_pairs.size; i++) {
    String name = nv_pairs.FastEl(i).name;
    name.downcase();
    String value = nv_pairs.FastEl(i).value.toString();
    
    if (name.startsWith("tab")) {
      data_var = FindVarNameInScope(value, false); // don't make
    }
  }
  
  SigEmitUpdated();
  return true;
}

void DoneWritingDataRow::GenCssBody_impl(Program* prog) {
  if(!data_var) {
    prog->AddLine(this, "// data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, data_var->name + ".WriteClose();");
  prog->AddVerboseLine(this);
}

