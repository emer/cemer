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

#ifndef DataSortEl_h
#define DataSortEl_h 1

// parent includes:
#include <DataOpEl>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataSortEl);

class TA_API DataSortEl : public DataOpEl {
  // one element of a data sorting specification
  INHERITED(DataOpEl)
public:
  enum SortOrder {
    ASCENDING,
    DESCENDING,
  };

  SortOrder		order;		// order to sort this in

  String GetDisplayName() const override;
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataSortEl);
protected:
  void	 CheckThisConfig_impl(bool quiet, bool& rval) override;
};

#endif // DataSortEl_h
