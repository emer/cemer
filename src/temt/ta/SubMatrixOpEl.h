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

#ifndef SubMatrixOpEl_h
#define SubMatrixOpEl_h 1

// parent includes:
#include <DataOpEl>

// member includes:
#include <MatrixGeom>

// declare all other types mentioned but not required to include:


taTypeDef_Of(SubMatrixOpEl);

class TA_API SubMatrixOpEl : public DataOpEl {
  // sub matrix operations element -- specifies a sub-matrix column and offset of that sub-matrix into larger matrix to operate on
  INHERITED(DataOpEl)
public:
  MatrixGeom		offset;

  String GetDisplayName() const override;
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(SubMatrixOpEl);
protected:
  void	 CheckThisConfig_impl(bool quiet, bool& rval) override;
};

#endif // SubMatrixOpEl_h
