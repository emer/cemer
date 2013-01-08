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

#ifndef VEObjCarousel_h
#define VEObjCarousel_h 1

// parent includes:
#include <VEBody>

// member includes:
#include <DataTableRef>

// declare all other types mentioned but not required to include:


class TA_API VEObjCarousel : public VEBody {
  // virtual environment object carousel -- contains a number of different objects that are pre-loaded and then selectable by name or number (info is contained in a DataTable)
INHERITED(VEBody)
public:
#ifdef __MAKETA__
  String        obj_fname;      // #READ_ONLY #HIDDEN object FileName corresponding to given object number
#endif
  DataTableRef  obj_table;      // the data table containing FileName, FilePath columns (other columns can be present but are ignored) -- objects are loaded from FilePath and can be selected by FileName or row number
  int           cur_obj_no;     // #READ_ONLY #SHOW current object number to view -- select using ViewObjNo button/function, which drives the view update as well
  SoSwitch*     obj_switch;     // #IGNORE saved switch, to minimize reloading

  virtual bool  ViewObjNo(int obj_no);
  // #BUTTON select object to view by number, corresponding to the rows of the obj_table data table
  virtual bool  ViewObjName(const String& obj_name);
  // #BUTTON select object to view by FileName value in the obj_table data table
  virtual bool  LoadObjs(bool force = false);
  // #BUTTON load the objects listed in the obj_table -- if not forcing, checks to see if count is the same as number of rows in obj_table and does not reload if so (and returns false)

  virtual void  MakeSwitch();   // #IGNORE make the switch obj if it doesn't yet exist

  TA_SIMPLE_BASEFUNS(VEObjCarousel);
private:
  void  Initialize();
  void  Destroy();
};

#endif // VEObjCarousel_h
