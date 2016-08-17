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

#include "taiWidgetWikiChooser.h"

#include <ProjTemplates>
#include <String_Array>
#include <iDialogItemChooser>
#include <QTreeWidgetItem>
#include <taMisc>

taiWidgetWikiChooser::taiWidgetWikiChooser(TypeDef* typ_, IWidgetHost* host, taiWidget* par,
					 QWidget* gui_parent_, int flags_)
 :inherited(typ_, host, par, gui_parent_, flags_)
{
  // nop
}

int taiWidgetWikiChooser::columnCount(int view) const {
  switch (view) {
  case 0: return 2;
  default: return 0; // not supposed to happen
  }
}

const String taiWidgetWikiChooser::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Name"; 
    case 1: return "URL"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiWidgetWikiChooser::titleText() {
  return "Please choose a wiki from the choices below";
}

void taiWidgetWikiChooser::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;
  cats->Add("Wikis");
}

int taiWidgetWikiChooser::BuildChooser_0(iDialogItemChooser* ic, taList_impl* top_lst, 
					QTreeWidgetItem* top_item) 
{
  int rval = 0;

  ic->multi_cats = false;	// multiple categories
  
  for(int i=0; i<taMisc::wikis.size; i++) {
    NameVar& nv = taMisc::wikis.FastEl(i);
    String url = nv.value.toString();
    QTreeWidgetItem* item = ic->AddItem("Wikis", nv.name,
					top_item, &nv, url, 1); 
    ++rval;
  }
  return rval;
}


