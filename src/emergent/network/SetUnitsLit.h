// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef SetUnitsLit_h
#define SetUnitsLit_h 1

// parent includes:
#include "network_def.h"
#include <ProgEl>

// member includes:
#include <DynEnum>
#include <ProgVar>

// declare all other types mentioned but not required to include:
class DataTable; //

eTypeDef_Of(SetUnitsLit);

class E_API SetUnitsLit: public ProgEl { 
  // set units in input_data table to present to the network based on dynamic enum values where the type name of the dynamic enum corresponds to the layer name in the input data: values supplied as literal items
INHERITED(ProgEl)
public:
  ProgVarRef	input_data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to the input data table
  bool		set_nm;		// set trial name based on unit names here
  int		offset;		// add this additional offset to unit indicies -- useful for unit groups with same sets of units
  DynEnum	unit_1; 	// unit to activate -- order doesn't matter -- can be any unit
  DynEnum	unit_2; 	// unit to activate -- order doesn't matter -- can be any unit
  DynEnum	unit_3; 	// unit to activate -- order doesn't matter -- can be any unit
  DynEnum	unit_4; 	// unit to activate -- order doesn't matter -- can be any unit

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "Function"; }
  String	GetToolbarName() const override { return "set units lit"; }

  PROGEL_SIMPLE_BASEFUNS(SetUnitsLit);
protected:
  void	UpdateAfterEdit_impl() override;
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;
  virtual bool	GetInputDataVar();

  void		GenCssBody_impl(Program* prog) override;
  virtual bool	GenCss_OneUnit(Program* prog, DynEnum& un, const String& idnm, DataTable* idat);

private:
  void	Initialize();
  void	Destroy();
};

#endif // SetUnitsLit_h
