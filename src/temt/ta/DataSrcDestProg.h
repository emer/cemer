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

#ifndef DataSrcDestProg_h
#define DataSrcDestProg_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:
class DataTable; // 


class TA_API DataSrcDestProg : public ProgEl { 
  // #VIRT_BASE #NO_INSTANCE a program element for data operations involving a source and destination (virtual base class -- do not use)
INHERITED(ProgEl)
public:
  ProgVarRef	    src_data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to source data for operation
  ProgVarRef	    dest_data_var;	// #NULL_OK #ITEM_FILTER_DataProgVarFilter program variable pointing to destination (result) data for operation (if NULL, a new one will be automatically created)

  virtual DataTable* GetSrcData(); // get source data table pointer from src_data_var (or NULL)
  virtual DataTable* GetDestData(); // get dsource data table pointer from dest_data_var (or NULL)

  virtual void	UpdateSpecDataTable() { };
  // #CAT_Data update the data table pointer(s) for the spec in this prog (so the user can choose columns from the appropriate data table)

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  PROGEL_SIMPLE_BASEFUNS(DataSrcDestProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataSrcDestProg_h
