// Co2018ght 2016-2017, Regents of the University of Colorado,
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

#include "taiWidgetNameVarArrayChooser.h"

#include <iDialogItemChooser>
#include <BuiltinTypeDefs>
#include <QTreeWidgetItem>

#include <taMisc>

taiWidgetNameVarArrayChooser::taiWidgetNameVarArrayChooser(TypeDef* typ_, IWidgetHost* host,
				   taiWidget* par, QWidget* gui_parent_, int flags_,
				   const String& flt_start_txt)
 :inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
  str_ary = NULL;
}

const String taiWidgetNameVarArrayChooser::titleText() {
  String chs_title = "Choose " + itemTag();
  if(str_ary) chs_title += " from options";
  return chs_title;
}

void taiWidgetNameVarArrayChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!str_ary) {
    taMisc::Error("taiWidgetNameVarArrayChooser::BuildChooser: str_ary needed");
    return;
  }
  switch (view) {
  case 0: 
    BuildChooser_0(ic); 
    break; 
  default: break; // shouldn't happen
  }
}

void taiWidgetNameVarArrayChooser::GetImage(NameVar_PArray* ary, NameVar* it) {
  str_ary = ary;
  inherited::GetImage((void*)it, &TA_NameVar);
}

int taiWidgetNameVarArrayChooser::BuildChooser_0(iDialogItemChooser* ic) {
  int rval = 0;
  
  for (int i = 0; i < str_ary->size; ++i) {
    NameVar* st = &(str_ary->FastEl(i));
    if(filter_start_txt.nonempty()) {
      if(!st->name.startsWith(filter_start_txt)) continue;
    }
    QTreeWidgetItem* item = ic->AddItem(st->name, NULL, st);
    item->setText(1, st->value.toString());
    ++rval;
  }
  return rval;
}

int taiWidgetNameVarArrayChooser::columnCount(int view) const {
  switch (view) {
  case 0: return 2;
  default: return 0; // not supposed to happen
  }
}

const String taiWidgetNameVarArrayChooser::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    // case 0: return "No."; 
    case 0: return "Name"; 
    case 1: return "Value"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiWidgetNameVarArrayChooser::viewText(int index) const {
  switch (index) {
  case 0: return "Array Items"; 
  default: return _nilString;
  }
}

const String taiWidgetNameVarArrayChooser::labelNameNonNull() const {
  return "Array Items";
}
