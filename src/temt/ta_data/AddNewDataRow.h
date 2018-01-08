// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef AddNewDataRow_h
#define AddNewDataRow_h 1

// parent includes:
#include <DataOneProg>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(AddNewDataRow);

class TA_API AddNewDataRow: public DataOneProg { 
  // add a new row to data table (just calls AddBlankRow() on data table var object)
INHERITED(DataOneProg)
public:
  String	GetDisplayName() const override;
  String	GetToolbarName() const override { return "new row"; }
  bool      CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool      CvtFmCode(const String& code) override;


  TA_BASEFUNS_NOCOPY(AddNewDataRow);
protected:
  bool		GenCssBody_impl(Program* prog) override;
private:
  void	Initialize();
  void	Destroy()	{ }
}; 

#endif // AddNewDataRow_h
