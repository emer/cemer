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

#ifndef SetUnitsVar_h
#define SetUnitsVar_h 1

// parent includes:
#include "network_def.h"
#include <ProgEl>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(SetUnitsVar);

class EMERGENT_API SetUnitsVar: public ProgEl { 
  // set units in input_data table to present to the network based on dynamic enum variables where the type name of the dynamic enum corresponds to the layer name in the input data: values supplied as variables
INHERITED(ProgEl)
public:
  ProgVarRef	input_data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to the input data table
  
  bool		set_nm;		// set trial name based on unit names here
  ProgVarRef	offset;		// #ITEM_FILTER_StdProgVarFilter add this additional offset to unit indicies -- useful for unit groups with same sets of units
  ProgVarRef	unit_1;		// #ITEM_FILTER_DynEnumProgVarFilter unit to activate -- order doesn't matter -- can be any unit
  ProgVarRef	unit_2;		// #ITEM_FILTER_DynEnumProgVarFilter unit to activate -- order doesn't matter -- can be any unit
  ProgVarRef	unit_3;		// #ITEM_FILTER_DynEnumProgVarFilter unit to activate -- order doesn't matter -- can be any unit
  ProgVarRef	unit_4;		// #ITEM_FILTER_DynEnumProgVarFilter unit to activate -- order doesn't matter -- can be any unit
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "Function"; }
  override String	GetToolbarName() const { return "set units var"; }

  PROGEL_SIMPLE_BASEFUNS(SetUnitsVar);
protected:
  override void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  virtual bool	GetInputDataVar();

  override void		GenCssBody_impl(Program* prog);
  virtual bool	GenCss_OneUnit(Program* prog, ProgVarRef& un, const String& idnm, DataTable* idat);

private:
  void	Initialize();
  void	Destroy();
};

#endif // SetUnitsVar_h
