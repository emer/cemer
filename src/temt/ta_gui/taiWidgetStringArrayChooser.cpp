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

#include "taiWidgetStringArrayChooser.h"
#include <iDialogItemChooser>
#include <BuiltinTypeDefs>
#include <QTreeWidgetItem>

#include <taMisc>

taiWidgetStringArrayChooser::taiWidgetStringArrayChooser(TypeDef* typ_, IWidgetHost* host,
				   taiWidget* par, QWidget* gui_parent_, int flags_,
				   const String& flt_start_txt)
 :inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
  str_ary = NULL;
}

const String taiWidgetStringArrayChooser::titleText() {
  String chs_title = "Choose " + itemTag();
  if(str_ary) chs_title += " from String Array: " + str_ary->GetDisplayName();
  return chs_title;
}

void taiWidgetStringArrayChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!str_ary) {
    taMisc::Error("taiWidgetStringArrayChooser::BuildChooser: str_ary needed");
    return;
  }
  switch (view) {
  case 0: 
    BuildChooser_0(ic); 
    break; 
  default: break; // shouldn't happen
  }
}

void taiWidgetStringArrayChooser::GetImage(String_Array* ary, String* it) {
  str_ary = ary;
  inherited::GetImage((void*)it, &TA_taString);
}

int taiWidgetStringArrayChooser::BuildChooser_0(iDialogItemChooser* ic) {
  int rval = 0;
  
  for (int i = 0; i < str_ary->size; ++i) {
    String* st = &(str_ary->FastEl(i));
    if(filter_start_txt.nonempty()) {
      if(!st->startsWith(filter_start_txt)) continue;
    }
    // QTreeWidgetItem* item = ic->AddItem(taMisc::LeadingZeros(i,2), NULL, st); 
    // item->setText(1, *st);
    QTreeWidgetItem* item = ic->AddItem(*st, NULL, st); 
    ++rval;
  }
  return rval;
}

int taiWidgetStringArrayChooser::columnCount(int view) const {
  switch (view) {
  case 0: return 1;
  default: return 0; // not supposed to happen
  }
}

const String taiWidgetStringArrayChooser::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    // case 0: return "No."; 
    case 0: return "String"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiWidgetStringArrayChooser::viewText(int index) const {
  switch (index) {
  case 0: return "Array Items"; 
  default: return _nilString;
  }
}

const String taiWidgetStringArrayChooser::labelNameNonNull() const {
  return "Array Items";
}


