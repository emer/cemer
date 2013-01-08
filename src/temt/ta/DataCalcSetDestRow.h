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

#ifndef DataCalcSetDestRow_h
#define DataCalcSetDestRow_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <ProgVarRef>

// declare all other types mentioned but not required to include:


class TA_API DataCalcSetDestRow : public DataSrcDestProg { 
  // set all the current values into the dest data table (used ONLY within a DataCalcLoop -- automatically gets dest_data from outer DataCalcLoop object)
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #HIDDEN #ITEM_FILTER_DataProgVarFilter source data for operation
  ProgVarRef	    dest_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter destination table to add row in -- automatically updated from DataCalcLoop
#endif

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  override String GetDisplayName() const;
  override String	GetToolbarName() const { return "=dest row"; }

  void 	InitLinks();
  TA_BASEFUNS(DataCalcSetDestRow);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog); 

private:
  void	Copy_(const DataCalcSetDestRow& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataCalcSetDestRow_h
