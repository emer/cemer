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

#ifndef DoneWritingDataRow_h
#define DoneWritingDataRow_h 1

// parent includes:
#include <DataOneProg>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DoneWritingDataRow);

class TA_API DoneWritingDataRow: public DataOneProg { 
  // add this after you are done writing everything to the current row of the data table, and it will update displays and write to log files, etc (just calls WriteClose() on data table var object)
INHERITED(DataOneProg)
public:
  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "row done"; }

  TA_BASEFUNS_NOCOPY(DoneWritingDataRow);
protected:
  override void		GenCssBody_impl(Program* prog);
private:
  void	Initialize();
  void	Destroy()	{ }
}; 

#endif // DoneWritingDataRow_h
