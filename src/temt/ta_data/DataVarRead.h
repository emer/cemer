// Copyright 2014-2018, Regents of the University of Colorado,
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

#ifndef DataVarRead_h
#define DataVarRead_h 1

// parent includes:
#include <DataVarSimple>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(DataVarRead);

class TA_API DataVarRead : public DataVarSimple {
  // A program element for reading from a data table -- scalar var/col and matrix var/col supported, and enum to/from Matrix localist code also supported (see DataVarReadMatrix for accessing individual matrix cells)
INHERITED(DataVarSimple)
public:
  String        GetDisplayName() const override;
  String  GetToolbarName() const override { return "read from\ndata table"; }
  bool    CanCvtFmCode(const String& code, ProgEl* scope_el) const override;

  PROGEL_SIMPLE_BASEFUNS(DataVarRead);
protected:

private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

#endif // DataVarRead_h
