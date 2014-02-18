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

#ifndef DataVarSimple_h
#define DataVarSimple_h 1

// parent includes:
#include <DataVarBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataVarSimple);

class TA_API DataVarSimple : public DataVarBase {
  // A program element for exchanging information between program variables and data table values in columns with the same names as the variables -- scalar var/col and matrix var/col supported, and enum to/from Matrix localist code also supported (see DataVarSimpleMatrix for accessing individual matrix cells)
INHERITED(DataVarBase)
public:
  ProgVarRef	var;		// #VIRT_BASE #ITEM_FILTER_StdProgVarFilter program variable to operate on -- name must match name of column in data table!

  PROGEL_SIMPLE_BASEFUNS(DataVarSimple);
protected:

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataVarSimple_h
