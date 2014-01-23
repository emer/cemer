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

#ifndef GraphAxisView_h
#define GraphAxisView_h 1

// parent includes:
#include <GraphAxisBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(GraphAxisView);

class TA_API GraphAxisView : public GraphAxisBase {
  // a non-Y axis (X, Z, etc)
INHERITED(GraphAxisBase)
public:

  bool          row_num;        // display row number instead of column value for this axis

  void         ComputeRange() override;
  bool         UpdateRange() override;
  void         UpdateOnFlag() override;

  void          CopyFromView(GraphAxisView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

  SIMPLE_COPY(GraphAxisView);
  T3_DATAVIEWFUNS(GraphAxisView, GraphAxisBase)
protected:
  void         UpdateAfterEdit_impl() override;

private:
  void                  Initialize();
  void                  Destroy() { };
};

#endif // GraphAxisView_h
