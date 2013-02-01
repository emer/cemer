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

#ifndef taiMemberWidgets_h
#define taiMemberWidgets_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <Member_List>
#include <taiWidget_List>
#include <taString>

// declare all other types mentioned but not required to include:

class TA_API taiMemberWidgets { // #IGNORE
public:
  Member_List           memb_el; // member elements (1:1 with data_el), empty in inline mode
  taiWidget_List           data_el; // data elements (1:1 with memb_el WHEN section shown)
  String                text; // for non-default guys, the text in the label or checkbox
  String                desc; // for non-default guys, the tooltip text
  bool                  show; // flag to help by indicating whether to show or not
  bool                  modal; // flag to indicate that section is modal (checkbox, or default closed tree)

  taiMemberWidgets() {show = false; modal = false;}
private:
  taiMemberWidgets(const taiMemberWidgets& cp); // value semantics not allowed
  taiMemberWidgets& operator=(const taiMemberWidgets& cp);
};

class TA_API taiMemberWidgets_List : public taPtrList<taiMemberWidgets> { // #IGNORE -- note that 1st list is always the default (no parent) -- leave it empty to have no root items
public:
  int                   def_size; // set to how many you want to use default processing
  void                  SetMinSize(int n); // make sure there are at least n sets
  void                  ResetItems(bool data_only = false); // calls Reset on all lists
  bool                  GetFlatDataItem(int idx, MemberDef** mbr, taiWidget** dat = NULL);
   // get the dat and/or mbr (both optional) from a flat idx
  int                   GetFlatDataIndex(taiWidget* dat);
  int                   GetFlatDataIndex(MemberDef* mbr, taBase* base);
   // get the flat idx from a mbr/dat, -1 if not found
  int                   GetDataSize() const; // # data items

  taiMemberWidgets_List()  {def_size = 0;}
  ~taiMemberWidgets_List();
protected:
  void  El_Done_(void* it) { delete (taiMemberWidgets*)it; }
};


#endif // taiMemberWidgets_h
