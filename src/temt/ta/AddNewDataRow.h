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

#ifndef AddNewDataRow_h
#define AddNewDataRow_h 1

// parent includes:
#include <DataOneProg>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API AddNewDataRow: public DataOneProg { 
  // add a new row to data table (just calls AddBlankRow() on data table var object)
INHERITED(DataOneProg)
public:
  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "new row"; }

  TA_BASEFUNS_NOCOPY(AddNewDataRow);
protected:
  override void		GenCssBody_impl(Program* prog);
private:
  void	Initialize();
  void	Destroy()	{ }
}; 

#endif // AddNewDataRow_h
