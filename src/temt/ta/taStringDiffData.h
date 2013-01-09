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

#ifndef taStringDiffData_h
#define taStringDiffData_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <int_PArray>

// declare all other types mentioned but not required to include:


class TA_API taStringDiffData {
  // ##NO_TOKENS #IGNORE one data record for each String item being compared in taStringDiff
public:
  int           lines;          // Number of elements (lines)
  int_PArray    data;           // Buffer of numbers that will be compared (hash codes of items).
  int_PArray    modified;       // bit-mapped flags for modification, which is the basic result of the diff -- if set for dataA it means deleted and for dataB it means inserted -- has 2 extra bits in it.
  int_PArray    line_st;        // starting position within data string for each line

  inline void SetModified(int idx, bool flag) {
    int aidx = idx / sizeof(int);
    int bit = idx % sizeof(int);
    int mask = 1 << bit;
    int& curval = modified.FastEl(aidx);
    if(flag) {
      curval |= mask;
    }
    else {
      curval &= ~mask;
    }
  }

  inline bool GetModified(int idx) {
    int aidx = idx / sizeof(int);
    int bit = idx % sizeof(int);
    int mask = 1 << bit;
    int curval = modified.FastEl(aidx);
    return 0 != (curval & mask);
  }

  inline void AllocModified() {
    int trg_n = (lines+2) / sizeof(int);
    modified.SetSize(trg_n+1);
  }

  inline void InitFmData() {
    lines = data.size;
    AllocModified();
    modified.InitVals(0);
  }

  String GetLine(const String& str, int st_ln, int ed_ln=-1) {
    int st = line_st[st_ln];
    if (ed_ln < 0) {
      ed_ln = st_ln;
    }
    int ed;
    if (ed_ln == line_st.size-1) {
      ed = str.length() - 1;
    }
    else {
      ed = line_st[ed_ln + 1] - 1;
    }
    return str.at(st, ed - st);
  }

  void  Reset() {
    lines = 0;
    data.Reset();
    modified.Reset();
    line_st.Reset();
  }

  taStringDiffData() {
    lines = 0;
  }
};

#endif // taStringDiffData_h
