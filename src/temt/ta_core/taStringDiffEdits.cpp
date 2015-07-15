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

#include "taStringDiffEdits.h"
#include <taStringDiff>


String taStringDiffEdits::GenerateB(const String& str_a) {
  String rval(0, str_a.length(), '\0'); // pre-alloc buffer
  int last_a = 0;
//  int last_b = 0;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    String alns = GetLine(str_a, last_a, df.start_a-1);
    rval.cat(alns).cat("\n");
    last_a = df.start_a + df.delete_a; // skip over deleted lines
    if(df.insert_b > 0)
      rval.cat(df.insert_b_str).cat("\n");
  }
  if(last_a < line_st.size-1) {
    String alns = GetLine(str_a, last_a, line_st.size-1);
    rval.cat(alns).cat("\n");
  }
  return rval;
}

String taStringDiffEdits::GetDiffStr(const String& str_a) {
  String rval;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    bool chg = false;
    if(df.delete_a == df.insert_b) {
      rval += String(df.start_a+1) + "c" + String(df.start_b+1) +
        taStringDiff::GetDiffRange(df.start_b, df.insert_b) + "\n";
      chg = true;
    }
    if(df.delete_a > 0) {
      if(!chg) {
        rval += String(df.start_a+1) +
          taStringDiff::GetDiffRange(df.start_a, df.delete_a)
          + "d" + String(df.start_b+1) + "\n";
      }
      if(str_a.nonempty()) {
        for(int l=df.start_a; l<df.start_a + df.delete_a; l++)
          rval += "< " + GetLine(str_a, l) + "\n";
        if(chg) {
          rval += "---\n";
        }
      }
    }
    if(df.insert_b > 0) {
      if(!chg) {
        rval += String(df.start_a+1) + "a" + String(df.start_b+1) +
          taStringDiff::GetDiffRange(df.start_b, df.insert_b) + "\n";
      }
      for(int l=df.start_b; l<df.start_b + df.insert_b; l++)
        rval += "> " + df.insert_b_str + "\n"; // todo: need to line-a-fy with >
    }
  }
  return rval;
}

int taStringDiffEdits::GetLinesChanged() {
  int tot = 0;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    tot += df.insert_b + df.delete_a;
  }
  return tot;
}

