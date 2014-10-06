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

#ifndef GpRowColPrjnSpec_h
#define GpRowColPrjnSpec_h 1

// parent includes:
#include <GpCustomPrjnSpecBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(GpRowColPrjnSpec);

class E_API GpRowColPrjnSpec : public GpCustomPrjnSpecBase {
  // projections involving a range of rows or columns of unit groups within sending and/or receiving layer -- also if a layer 
INHERITED(GpCustomPrjnSpecBase)
public:
  enum RowCol {
    ROWS,                       // connect a range of rows of unit groups
    COLS,                       // connect a range of cols of unit groups
    ALL,                        // all units in the layer
  };

  RowCol        recv_row_col;   // how to connect the receiving layer: by rows or columns of unit groups, or to all units
  int           recv_start;     // #CONDSHOW_OFF_recv_row_col:ALL starting row or column for receiving layer (0..n-1)-- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)
  int           recv_end;       // #CONDSHOW_OFF_recv_row_col:ALL ending row or column for receiving layer (0..n-1) -- to just do one, should be same as start -- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)

  RowCol        send_row_col;   // how to connect the sending layer: by rows or columns of unit groups, or to all units
  int           send_start;     // #CONDSHOW_OFF_send_row_col:ALL starting row or column for sending laeyr (0..n-1) -- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)
  int           send_end;       // #CONDSHOW_OFF_send_row_col:ALL ending row or column for sending layer (0..n-1) -- to just do one, should be same as start -- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)

  void	Connect_impl(Projection* prjn, bool make_cons) override;


  TA_SIMPLE_BASEFUNS(GpRowColPrjnSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // GpRowColPrjnSpec_h
