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

#ifndef DataGroupSpec_h
#define DataGroupSpec_h 1

// parent includes:
#include <DataOpBaseSpec>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataGroupSpec);

class TA_API DataGroupSpec : public DataOpBaseSpec {
  // #CAT_Data a datatable grouping specification (list of group elements)
INHERITED(DataOpBaseSpec)
public:
  bool  append_agg_name;        // if true, append aggregation operator name (e.g., group, mean, last, etc) to column names of destination data table (otherwise, just keep same names as source)

  TA_SIMPLE_BASEFUNS(DataGroupSpec);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // DataGroupSpec_h
