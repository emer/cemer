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

#ifndef DataColSpec_h
#define DataColSpec_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <MatrixGeom>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataColSpec);

class TA_API DataColSpec: public taNBase {
  // ##CAT_Data describes a column of data in a DataTable
INHERITED(taNBase)
public:
  int                   col_num; // #SHOW #READ_ONLY #NO_SAVE the column number (-1=at end)
  ValType               val_type; // the type of data the channel uses
  bool                  is_matrix; // is this a matrix cell
  MatrixGeom            cell_geom; // #CONDSHOW_ON_is_matrix the geometry of a matrix cell

  virtual void          SetCellGeom(int dims,
                                    int d0, int d1=0, int d2=0, int d3=0, int d4=0);
  // set matrix geom -- also sets is_matrix = true
  virtual void          SetCellGeomN(const MatrixGeom& geom);
  // set matrix geom -- also sets is_matrix = true

  TA_SIMPLE_BASEFUNS(DataColSpec);
private:
  void  Initialize();
  void  Destroy() {}
};

#endif // DataColSpec_h
