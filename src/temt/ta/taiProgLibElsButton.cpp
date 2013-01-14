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

#include "taiProgLibElsButton.h"
#include <String_Array>
#include <ProgLib>
#include <taiItemChooser>

#include <QTreeWidgetItem>


taiProgLibElsButton::taiProgLibElsButton(TypeDef* typ_, IDataHost* host, taiData* par,
                                         QWidget* gui_parent_, int flags_)
 :inherited(typ_, host, par, gui_parent_, flags_)
{
  // nop
}

int taiProgLibElsButton::columnCount(int view) const {
  switch (view) {
  case 0: return 6;
  default: return 0; // not supposed to happen
  }
}

const String taiProgLibElsButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Name";
    case 1: return "Type";
    case 2: return "Tags";
    case 3: return "Description";
    case 4: return "Date Modified";
    case 5: return "URL/filename";
    } break;
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiProgLibElsButton::titleText() {
  return "Please choose a program from the following in the available libraries";
}

void taiProgLibElsButton::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;

  if(!list) return;             // shouldn't happen

  ProgLib* plib = (ProgLib*)list;
  for(int i=0;i<plib->size;i++) {
    ProgLibEl* pel = plib->FastEl(i);
    for(int j=0;j<pel->tags_array.size;j++) {
      cats->AddUnique(pel->tags_array[j]);
    }
  }
  cats->Sort(); // empty, if any, should sort to top
}

int taiProgLibElsButton::BuildChooser_0(taiItemChooser* ic, taList_impl* top_lst,
                                        QTreeWidgetItem* top_item)
{
  int rval = 0;

  ic->multi_cats = true;        // multiple categories

  ProgLib* plib = (ProgLib*)top_lst;
  for (int i = 0; i < plib->size; ++i) {
    ProgLibEl* pel = plib->FastEl(i);
    QTreeWidgetItem* item = ic->AddItem(pel->tags, pel->GetDisplayName(),
                                        top_item, pel);
    item->setText(1, pel->lib_name); // GetColText(taBase::key_type));
    item->setText(2, pel->tags);
    item->setText(3, pel->desc); // GetColText(taBase::key_desc));
    item->setText(4, pel->date);
    if(pel->URL.nonempty())
      item->setText(5, pel->URL);
    else
      item->setText(5, pel->filename);
    ++rval;
  }
  return rval;
}
