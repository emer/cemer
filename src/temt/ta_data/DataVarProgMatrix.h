// Copyright 2015, Regents of the University of Colorado,
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

#ifndef DataVarProgMatrix_h
#define DataVarProgMatrix_h 1

// parent includes:
#include <DataVarProg>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataVarProgMatrix);

class TA_API DataVarProgMatrix : public DataVarProg { 
  // A program element for exchanging information between program variables and data table values in columns with the same names as the variables -- for matrix columns where you want to access up to 4 cells (var_1 is first cell, var_2 is second cell..), name of column is var name before last '_' -- regular DataVarProg can manage copying Matrix column to matrix object variables, or enums
INHERITED(DataVarProg)
public:
  String	GetToolbarName() const override { return "data mtx=\nvars"; }

  TA_BASEFUNS_NOCOPY(DataVarProgMatrix);
protected:
  bool GenCss_OneVar(Program* prog, ProgVar* var, const String& idnm, int var_no) override;
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataVarProgMatrix_h
