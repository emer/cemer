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

#ifndef DataOneProg_h
#define DataOneProg_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:
class DataTable; // 


class TA_API DataOneProg : public ProgEl { 
  // #VIRT_BASE #NO_INSTANCE a program element for operations on one data table (virtual base class -- do not use)
INHERITED(ProgEl)
public:
  ProgVarRef	    data_var;	// #ITEM_FILTER_DataProgVarFilter program variable pointing to data table for operation

  virtual DataTable* GetData() const;
  // get actual data table pointer from variable

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  PROGEL_SIMPLE_BASEFUNS(DataOneProg);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // DataOneProg_h
