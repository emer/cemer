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

#ifndef DataCalcCopyCommonCols_h
#define DataCalcCopyCommonCols_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


class TA_API DataCalcCopyCommonCols : public DataSrcDestProg { 
  // copy all of the columns from source to dest that have the same name and type (used ONLY within a DataCalcLoop -- automatically gets src_data from outer DataCalcLoop object) -- must do a + dest row *before* this step (copies into this new row)
INHERITED(DataSrcDestProg)
public:
#ifdef __MAKETA__
  ProgVarRef	    src_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter source data for copying -- automatically updated from DataCalcLoop
  ProgVarRef	    dest_data_var;	// #READ_ONLY #SHOW #ITEM_FILTER_DataProgVarFilter destination table for copying -- automatically updated from DataCalcLoop
#endif
  bool		only_named_cols;
  // only copy columns that are named in src_cols and dest_cols (otherwise just operates on all the datatable columns)

  virtual void	GetDataPtrsFmLoop();
  // get my data table ptrs from parent calc loop obj

  override String GetDisplayName() const;
  override String	GetToolbarName() const { return "cpy cols"; }

  void 	InitLinks();
  SIMPLE_COPY_EX(DataCalcCopyCommonCols,CopyInner_);
  TA_BASEFUNS(DataCalcCopyCommonCols);
protected:
  override void UpdateAfterEdit_impl();
  override void	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog); 

private:
  void	Copy_(const DataCalcCopyCommonCols& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataCalcCopyCommonCols_h
