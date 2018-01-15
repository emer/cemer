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

#ifndef DataGroupEl_h
#define DataGroupEl_h 1

// parent includes:
#include <DataOpEl>

// member includes:
#include <AggregateSpec>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataGroupEl);

class TA_API DataGroupEl : public DataOpEl {
  // one element of a data grouping specification
  INHERITED(DataOpEl)
public:
  String        name;           // #HIDDEN #READ_ONLY cached name value
  AggregateSpec agg;            // how to aggregate this information

  String        GetName() const override { return name; }
  bool          SetName(const String& nm) override;
  void          UpdateName() override;
  String        GetDisplayName() const override;
  bool          BrowserEditSet(const String& new_val_str, int move_after = 0) override;

  void  Initialize();
  void  Destroy()               { };
  TA_SIMPLE_BASEFUNS(DataGroupEl);
protected:
  void   CheckThisConfig_impl(bool quiet, bool& rval) override;
};

#endif // DataGroupEl_h
