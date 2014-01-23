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

#ifndef DataCalcSetSrcRow_h
#define DataCalcSetSrcRow_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataCalcSetSrcRow);

class TA_API DataCalcSetSrcRow : public DataSrcDestProg { 
  // set all the current values into the src data table (used ONLY within a DataCalcLoop -- automatically gets src_data from outer DataCalcLoop object)
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter source table to set values in -- automatically updated from DataCalcLoop
  ProgVarRef	    dest_data_var;	// #READ_ONLY #HIDDEN #ITEM_FILTER_DataProgVarFilter destination table -- automatically updated from DataCalcLoop
#endif

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  String GetDisplayName() const CPP11_OVERRIDE;
  String	GetToolbarName() const CPP11_OVERRIDE { return "=src row"; }

  void 	InitLinks();
  TA_BASEFUNS(DataCalcSetSrcRow);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void	CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE; 

private:
  void	Copy_(const DataCalcSetSrcRow& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataCalcSetSrcRow_h
