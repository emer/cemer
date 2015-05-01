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

#ifndef BgPfcPrjnSpec_h
#define BgPfcPrjnSpec_h 1

// parent includes:
#include <GpCustomPrjnSpecBase>

// member includes:
#include <DataTable>

// declare all other types mentioned but not required to include:

eTypeDef_Of(BgPfcPrjnSpec);

class E_API BgPfcPrjnSpec : public GpCustomPrjnSpecBase {
  // for connecting BG and PFC layers, where there are separate PFC layers that interconnect with a single BG layer (Matrix, GPi, etc), allowing competition within the BG -- has a customizable data table of the different PFC layers that all map to the same BG layer -- also supports connections from a Patch layer with same name root as PFC
INHERITED(GpCustomPrjnSpecBase)
public:
  enum BgTableVals { // the different values stored in bg_table -- for rapid access
    BGT_NAME,
    BGT_SIZE_X,
    BGT_SIZE_Y,
    BGT_START_X,
    BGT_START_Y,
  };

  int           n_pfcs;        // number of different PFC layers that map into a common BG layer -- this is the number of rows that will be enforced in the bg_table data table
  DataTable     bg_table;      // #SHOW_TREE #EXPERT #HIDDEN_CHOOSER table of PFC layers that map into a common BG layer -- one row per PFC layer -- you specify the name of each PFC layer and its size in unit groups (x,y), and optionally a starting x,y unit group offset within the BG layer (-1 means use default horizontal layout of pfc's within bg) -- mouse over the column headers for important further details

  bool          cross_connect;       // if true, then this creates connections based on the PFC layer name specified in the connect_as field, instead of the actual name of the PFC layer (still needs to find the PFC layer name in bg_table to know which layer is the PFC layer and which is the BG layer)
  String        connect_as;          // #CONDSHOW_ON_cross_connect PFC layer name to connect as -- see cross_connect option for details

  inline int    FindBgTableRow(const String& name) {
    return bg_table.FindVal(name, BGT_NAME, 0, true);
  }
  // find table row for given pfc name -- emits error if not found
  inline Variant  GetBgTableVal(BgTableVals val, int row) {
    return bg_table.GetVal(val, row);
  }
  // get specific dyn value for given row
  inline void   SetBgTableVal(const Variant& vl, BgTableVals val, int row) {
    bg_table.SetVal(vl, val, row);
  }
  // set specific dyn value for given row
  
  virtual void  FormatBgTable();
  // #IGNORE format the bg table
  virtual void  InitBgTable();
  // default initial bg table
  virtual void  UpdtBgTable();
  // #BUTTON update the bg table to ensure consistency of everything

  void	Connect_impl(Projection* prjn, bool make_cons) override;

  TA_SIMPLE_BASEFUNS(BgPfcPrjnSpec);
protected:
  void UpdateAfterEdit_impl();
  
private:
  void Initialize();
  void Destroy()     { };
};

#endif // BgPfcPrjnSpec_h
