// Co2018ght 2013-2017, Regents of the University of Colorado,
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
#include <ProgVar>

// declare all other types mentioned but not required to include:


taTypeDef_Of(DataCalcSetDestRow);

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

  String    GetDisplayName() const override;
  String    GetToolbarName() const override { return "=dest row"; }
  bool      CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool      CvtFmCode(const String& code) override;

  void 	InitLinks() override;
  TA_BASEFUNS(DataCalcSetDestRow);
protected:
  void      UpdateAfterEdit_impl() override;
  void	    CheckThisConfig_impl(bool quiet, bool& rval) override;
  bool	    GenCssBody_impl(Program* prog) override; 

private:
  void	Copy_(const DataCalcSetDestRow& cp);
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataCalcSetDestRow_h
