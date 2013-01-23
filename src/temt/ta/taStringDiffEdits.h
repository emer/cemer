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

#ifndef taStringDiffEdits_h
#define taStringDiffEdits_h 1

// parent includes:

// member includes:
#include <taStringDiffItem_PArray>
#include <int_PArray>

// declare all other types mentioned but not required to include:

TypeDef_Of(taStringDiffEdits);

class TA_API taStringDiffEdits {
  // #NO_TOKENS a set of diffs between string A and string B -- can be used to convert string A into string B, given only string A and these edits
public:
  taStringDiffItem_PArray       diffs;  // the raw diffs
  int_PArray                    line_st; // line starting positions for string A

  String      GetLine(const String& str, int st_ln, int ed_ln=-1)
  { int st = line_st[st_ln];  if(ed_ln < 0) ed_ln = st_ln;
    int ed; if(ed_ln == line_st.size-1) ed = str.length()-1; else ed = line_st[ed_ln+1]-1;
    return str.at(st, ed-st);
  }

  String        GenerateB(const String& str_a);
  // generate (return value) from str_a and the stored diff information

  String        GetDiffStr(const String& str_a);
  // get a string representation of the diffs -- if str_a is passed, then it produces normal diff format, otherwise a bit more minimal

  int GetLinesChanged();
  // get total count of lines changed (inserts + deletes) in the diffs

};

#endif // taStringDiffEdits_h
