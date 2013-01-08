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

#ifndef DataColView_h
#define DataColView_h 1

// parent includes:
#include <T3DataView>

// member includes:

// declare all other types mentioned but not required to include:
class DataTableView; // 
class TypeDef; // 
class DataCol; // 


class TA_API DataColView: public T3DataView {
  // ##SCOPE_DataTableView base specification for the display of data columns
INHERITED(T3DataView)
friend class DataTableView;
public:
  bool          visible;        // is this column visible in display?
  //  bool      sticky;         // #DEF_false set this to retain this colspec even if its column deletes -- not supported by the views and nonsensical, right?

  DataCol*              dataCol() const {return (DataCol*)data();}
  void                  setDataCol(DataCol* value, bool first_time = false);

  DATAVIEW_PARENT(DataTableView)

  bool                  isVisible() const; // bakes in check for datacol

  virtual void          Hide();
  // #BUTTON #VIEWMENU set this column to be invisible

  override void         DataDestroying();

  void  SetDefaultName() {} // leave it blank
  void  Copy_(const DataColView& cp);
  TA_BASEFUNS(DataColView);
protected:
  override void         Unbind_impl(); // unbinds col
  virtual void          DataColUnlinked() {} // called if data set to NULL or destroys
  void                  UpdateFromDataCol(bool first_time = false);
  // called if data set to column, or we otherwise need to update
  virtual void          UpdateFromDataCol_impl(bool first_time);
  void  Initialize();
  void  Destroy();
};

#endif // DataColView_h
