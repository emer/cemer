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

#ifndef taStringDiffItem_h
#define taStringDiffItem_h 1

// parent includes:

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:

///////////////////////////////////////////////
// String-Based Text Diff Algorithm

// C++ version of C# code by Matthias Hertel:
// http://www.mathertel.de/Diff
// see .cpp file for more information

class TA_API taStringDiffItem {
  // ##NO_TOKENS one item of difference between the strings
public:
  int           start_a;        // start line number in string A
  int           start_b;        // start line number in string B
  int           delete_a;       // number of lines deleted in string A
  int           insert_b;       // number of lines inserted in string B
  String        insert_b_str;   // actual string value of lines inserted into string B

  taStringDiffItem() { start_a = start_b = delete_a = insert_b = 0; }

  bool operator==(taStringDiffItem& cmp) { return start_a == cmp.start_a; }
  bool operator>(taStringDiffItem& cmp) { return start_a > cmp.start_a; }
  bool operator<(taStringDiffItem& cmp) { return start_a < cmp.start_a; }
};

#endif // taStringDiffItem_h
