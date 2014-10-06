// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef ToGpRowColPrjnSpec_h
#define ToGpRowColPrjnSpec_h 1

// parent includes:
#include <GpCustomPrjnSpecBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ToGpRowColPrjnSpec);

class E_API ToGpRowColPrjnSpec  : public GpCustomPrjnSpecBase {
  // send projection to a range of rows or columns of unit groups within receiving layer -- projects from entire receiving layer
INHERITED(GpCustomPrjnSpecBase)
public:
  enum RowCol {
    ROWS,                       // connect a range of rows of unit groups
    COLS,                       // connect a range of cols of unit groups
  };
  RowCol        row_col;        // connect by rows or columns of unit groups
  int           start;          // starting row or column (0..n-1)-- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)
  int           end;            // ending row or column (0..n-1) -- to just do one, should be same as start -- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)

  void	Connect_impl(Projection* prjn, bool make_cons) override;

  TA_SIMPLE_BASEFUNS(ToGpRowColPrjnSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ToGpRowColPrjnSpec_h
