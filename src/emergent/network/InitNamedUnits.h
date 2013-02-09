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

#ifndef InitNamedUnits_h
#define InitNamedUnits_h 1

// parent includes:
#include <NetBaseProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:
class DataTable; //
class DataCol; //
class DynEnumType; //

eTypeDef_Of(InitNamedUnits);

class E_API InitNamedUnits: public NetBaseProgEl { 
  // Initialize named units system -- put this in the Init code of the program and it will configure everything based on the input_data datatable (which should be the first datatable in the args or vars -- Set Unit guys will look for it there)
INHERITED(NetBaseProgEl)
public:
  ProgVarRef	input_data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to the input data table -- finds the first one in the program by default (and makes one if not found)
  ProgVarRef	unit_names_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to the unit_names data table, which is created if it does not exist -- contains the name labels for each of the units
  ProgVarRef	network_var;	// #ITEM_FILTER_NetProgVarFilter variable that points to the network (optional; for labeling network units if desired)
  int		n_lay_name_chars; // number of layer-name chars to prepend to the enum values
  int		max_unit_chars; // max number of characters to use in unit label names (-1 = all)

  static bool	InitUnitNamesFmInputData(DataTable* unit_names, const DataTable* input_data);
  // intialize unit names data table from input data table
  static bool	InitDynEnumFmUnitNames(DynEnumType* dyn_enum, const DataCol* unit_names_col,
				       const String& prefix);
  // initialize a dynamic enum with names from unit names table colum (string matrix with one row)
  virtual bool	InitNamesTable();
  // #BUTTON #CONFIRM intialize (and update) the unit names table (will auto-create if not set) -- must have set the input_data_var to point to an input data table already!
  virtual bool	InitDynEnums();
  // #BUTTON #CONFIRM intialize the dynamic enums from names table -- do this after you have entered the names in the unit_names table, in order to then refer to the names using enum values (avoiding having to use quotes!)
  virtual bool	LabelNetwork(bool propagate_names = false);
  // #BUTTON label units in the network based on unit names table -- also sets the unit_names matrix in the layer so they are persistent -- network_var must be set -- if propagate_names is set, then names will be propagated along one-to-one projections to other layers that do not have names in the table
  virtual bool	ViewDataLegend();
  // #BUTTON #CONFIRM create a new grid view display of the input data with the unit names as alegend
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }
  override String	GetToolbarName() const { return "init nm units"; }

  PROGEL_SIMPLE_BASEFUNS(InitNamedUnits);
protected:
  override void	UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);

  virtual bool	GetInputDataVar();
  virtual bool	GetUnitNamesVar();
  virtual bool	GetNetworkVar();

  override void		GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy();
};

#endif // InitNamedUnits_h
