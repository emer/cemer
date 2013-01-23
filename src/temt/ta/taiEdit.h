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

#ifndef taiEdit_h
#define taiEdit_h 1

// parent includes:
#include <taiType>

// member includes:

// declare all other types mentioned but not required to include:
class EditDataPanel; //
class taiEditDataHost; //
class iMainWindowViewer; //
class taiDataLink; //

TypeDef_Of(taiEdit);

class TA_API taiEdit : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiEdit, taiType);
public:
  taiEdit*              LowerBidder() { return static_cast<taiEdit*>(next_lower_bidder); }

  virtual const iColor  GetBackgroundColor(void* base, bool& ok); // gets for taBase
  virtual int           Edit(void* base=NULL, bool read_only=false, const iColor& bgcol = def_color); //edit wherever found (note: rarely overridden)
  virtual int           EditDialog(void* base, bool read_only = false,
                                   bool modal = false, const iColor& bgcol = def_color,
                                   int min_width=-1, int min_height=-1);
  // edit in a Dialog (note: rarely overridden) -- finds existing if non-modal else makes new

  virtual EditDataPanel* EditNewPanel(taiDataLink* link, void* base=NULL,
    bool read_only = false, const iColor& bgcol = def_color);
  //edit in a new panel (note: rarely overridden)

  virtual EditDataPanel* EditPanel(taiDataLink* link, void* base=NULL,
    bool read_only = false, iMainWindowViewer* not_in_win = NULL,
    const iColor& bgcol = def_color);
  //edit in a panel, prefer existing, else created new (note: rarely overridden)

  virtual int           BidForType(TypeDef*) { return 0;}
  virtual int           BidForEdit(TypeDef*) { return 1;}
  virtual void          AddEdit(TypeDef* td); // add an edit to a type

  void                  Initialize() {}
  void                  Destroy() {}

protected:
  virtual taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance, overridden by subclasses
};

#endif // taiEdit_h
