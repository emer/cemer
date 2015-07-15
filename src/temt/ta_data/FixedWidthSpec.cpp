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

#include "FixedWidthSpec.h"
#include <DataTable>

TA_BASEFUNS_CTORS_DEFN(FixedWidthSpec);
SMARTREF_OF_CPP(FixedWidthSpec); //



void FixedWidthSpec::Load_Init(DataTable* dat_) {
  dat = dat_;
  for (int i = 0; i < col_specs.size; ++i) {
    FixedWidthColSpec* fws = col_specs.FastEl(i);
    fws->col = dat->FindColName(fws->name);
    TestError(!fws->col, "Load_Init", "col name not in table:",
      fws->name);
  }
}

void FixedWidthSpec::Initialize() {
  n_skip_lines = 0;
  dat = NULL;
}

void FixedWidthSpec::AddRow(const String& ln) {
  dat->AddBlankRow();
  for (int i = 0; i < col_specs.size; ++i) {
    FixedWidthColSpec* fws = col_specs.FastEl(i);
    if (!fws) continue;
    String s = ln.from(fws->start_col - 1);
    if (fws->col_width >= 0)
      s.truncate(fws->col_width);
    fws->WriteData(s);
  }
}

