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

#ifndef DataCalcAddDestRow_h
#define DataCalcAddDestRow_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataCalcAddDestRow);

class TA_API DataCalcAddDestRow : public DataSrcDestProg { 
  // add a new blank row into the dest data table (used ONLY within a DataCalcLoop to add new data -- automatically gets dest_data from outer DataCalcLoop object) -- MUST put within a CodeBlock if multiple are used per loop, otherwise duplicate variable definition warnings will occur
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #HIDDEN #ITEM_FILTER_DataProgVarFilter source data for operation
  ProgVarRef	    dest_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter destination table to add row in -- automatically updated from DataCalcLoop
#endif

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  override String GetDisplayName() const;
  override String	GetToolbarName() const { return "+dest row"; }

  void	InitLinks();
  TA_BASEFUNS(DataCalcAddDestRow);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog); 

private:
  void	Copy_(const DataCalcAddDestRow& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataCalcAddDestRow_h
