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

#ifndef MatrixChannelSpec_h
#define MatrixChannelSpec_h 1

// parent includes:
#include <ChannelSpec>

// member includes:
#include <MatrixGeom>
#include <String_Matrix>

// declare all other types mentioned but not required to include:


TypeDef_Of(MatrixChannelSpec);

class TA_API MatrixChannelSpec: public ChannelSpec {
  // describes a matrix channel of data in a DataBlock
INHERITED(ChannelSpec)
public:
  MatrixGeom            cell_geom; //  the geom of each cell
  bool                  uses_cell_names; // set 'true' if there are cell names associated
  String_Matrix         cell_names; //  #COND_EDIT_ON_uses_cell_names:true (optional) the names associated with each cell

  override bool         isMatrix() const {return true;}
  override const MatrixGeom& cellGeom() const {return cell_geom;}
  override bool         usesCellNames() const {return uses_cell_names;}
  override const String_Matrix& cellNames() const {return cell_names;}

  virtual void          SetCellGeom(bool uses_names, int dims,
    int d0, int d1=0, int d2=0, int d3=0, int d4=0);
  virtual void          SetCellGeomN(bool uses_names, const MatrixGeom& geom);

  void  UpdateAfterEdit();
  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(MatrixChannelSpec);
protected:
  void                  InitCellNames();
private:
  void  Copy_(const MatrixChannelSpec& cp);
  void          Initialize();
  void          Destroy() {CutLinks();}
};

#endif // MatrixChannelSpec_h
