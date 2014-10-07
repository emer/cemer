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

#ifndef RowColPrjnSpec_h
#define RowColPrjnSpec_h 1

// parent includes:
#include "network_def.h"
#include <ProjectionSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(RowColPrjnSpec);

class E_API RowColPrjnSpec : public ProjectionSpec {
  // projections involving a range of rows or columns of units or unit groups within sending and/or receiving layer -- connectivity can be one-to-one or all-to-all for selected rows/cols
INHERITED(ProjectionSpec)
public:
  enum RowCol {
    ROWS,                       // connect a range of rows of unit groups
    COLS,                       // connect a range of cols of unit groups
    ALL,                        // all units in the layer
  };

  RowCol        recv_row_col;   // how to connect the receiving layer: by rows or columns of unit groups, or to all units
  bool          recv_gp;        // #CONDSHOW_OFF_recv_row_col:ALL use unit groups instead of units for the recv layer row/col structure -- if not using groups, uses logical display layout of units for rows and columns, even if there is underlying group structure
  int           recv_start;     // #CONDSHOW_OFF_recv_row_col:ALL starting row or column for receiving layer (0..n-1)-- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)
  int           recv_end;       // #CONDSHOW_OFF_recv_row_col:ALL ending row or column for receiving layer (0..n-1) -- to just do one, should be same as start -- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)

  RowCol        send_row_col;   // how to connect the sending layer: by rows or columns of unit groups, or to all units
  bool          send_gp;        // #CONDSHOW_OFF_send_row_col:ALL use unit groups instead of units for the send layer row/col structure -- if not using groups, uses logical display layout of units for rows and columns, even if there is underlying group structure
  int           send_start;     // #CONDSHOW_OFF_send_row_col:ALL starting row or column for sending laeyr (0..n-1) -- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)
  int           send_end;       // #CONDSHOW_OFF_send_row_col:ALL ending row or column for sending layer (0..n-1) -- to just do one, should be same as start -- can use negative numbers to start from the back (e.g., -1 is last one, -2 is second-to-last, etc)

  bool          one_to_one;     // if true, then the same total number of units or groups must be selected for recv and send sides, and they are connected in a one-to-one fashion -- otherwise, the connection is all-to-all for all of the selected recv and sending units

  void	Connect_impl(Projection* prjn, bool make_cons) override;


  TA_SIMPLE_BASEFUNS(RowColPrjnSpec);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // RowColPrjnSpec_h
