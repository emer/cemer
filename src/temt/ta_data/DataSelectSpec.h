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

#ifndef DataSelectSpec_h
#define DataSelectSpec_h 1

// parent includes:
#include <DataOpBaseSpec>
#include <Relation>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; //

taTypeDef_Of(DataSelectSpec);

class TA_API DataSelectSpec : public DataOpBaseSpec {
  // #CAT_Data a datatable select specification (list of select elements)
  INHERITED(DataOpBaseSpec)
public:
  Relation::CombOp	comb_op;	// how to combine individual expressions for each column

  virtual void 	UpdateEnabled(); // update the act_enabled flags based on variables etc

  void GetColumns(DataTable* dt) override { inherited::GetColumns(dt); UpdateEnabled(); }

  String GetDisplayName() const override;
  TA_SIMPLE_BASEFUNS(DataSelectSpec);
private:
  void	Initialize();
  void 	Destroy()		{ };
};

#endif // DataSelectSpec_h
