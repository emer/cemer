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

#ifndef ViewSpec_h
#define ViewSpec_h 1

// parent includes:
#include <taDataView>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(ViewSpec);

class TA_API ViewSpec: public taDataView {
  // ##CAT_Display generic specification for a view of some item
INHERITED(taDataView)
public:
  String                display_name; // name used in display IF DIFFERENT THAN name
  bool                  visible;        // visibility flag

  String GetDisplayName() const;
  TA_BASEFUNS(ViewSpec) //
private:
  void  Copy_(const ViewSpec& cp);
  void Initialize();
  void Destroy() {}
};

#endif // ViewSpec_h
