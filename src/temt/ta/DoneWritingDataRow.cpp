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

TA_BASEFUNS_CTORS_DEFN(DoneWritingDataRow);


void DoneWritingDataRow::Initialize() {
}

String DoneWritingDataRow::GetDisplayName() const {
  String rval = "DoneWritingDataRow to: ";
  if(data_var) rval += data_var->name;
  else rval += "(ERROR: data_var not set!)";
  return rval;
}

void DoneWritingDataRow::GenCssBody_impl(Program* prog) {
  if(!data_var) {
    prog->AddLine(this, "// data_var not set!", ProgLine::MAIN_LINE);
    return;
  }
  prog->AddLine(this, data_var->name + ".WriteClose();");
  prog->AddVerboseLine(this);
}

