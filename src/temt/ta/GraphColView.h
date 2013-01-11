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

#ifndef GraphColView_h
#define GraphColView_h 1

// parent includes:
#include <DataColView>

// member includes:
#include <FixedMinMax>
#include <MinMax>

// declare all other types mentioned but not required to include:
class GraphTableView; // 
class TypeDef; // 

/*
  User Data for Columns:

  MIN=x (x: float) -- forces min range to be x
  MAX=x (x: float) -- forces max range to be x

  Axis options:
  X_AXIS -- set as X Axis
  Z_AXIS -- set as Z Axis
  PLOT_1 -- set as plot_1 data (first data to be plotted)
  PLOT_2 -- set as plot_2 data
  PLOT_n -- set as plot_n data
  ERR_1 -- set as err_1 data
  ERR_2 -- set as err_2 data
  ERR_n -- set as err_n data
  COLOR_AXIS -- set as color_axis
  RASTER_AXIS -- set as raster_axis
*/

class TA_API GraphColView : public DataColView {
  // information for graph display of a column: note that the axis handles all the key display, so not much happens with this guy
INHERITED(DataColView)
public:
  FixedMinMax           fixed_range;    // fixed min/max range values for display (if not fixed, automatically set to min/max of data)
  MinMax                data_range;     // #READ_ONLY actual min and max of data (including fixed range)

  override bool         hasViewProperties() const { return true; }
  override String       GetDisplayName() const;

  DATAVIEW_PARENT(GraphTableView)

  void                  CopyFromView(GraphColView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  void InitLinks();
  SIMPLE_COPY(GraphColView);
  TA_BASEFUNS(GraphColView);
protected:

private:
  void  Initialize();
  void  Destroy();
};

#endif // GraphColView_h
