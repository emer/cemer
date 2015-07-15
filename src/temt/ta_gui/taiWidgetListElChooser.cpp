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

#include "taiWidgetListElChooser.h"
#include <taList_impl>
#include <iDialogItemChooser>

#include <taMisc>
#include <taiMisc>

#include <QTreeWidgetItem>


taiWidgetListElChooser::taiWidgetListElChooser(TypeDef* typ_, IWidgetHost* host,
				   taiWidget* par, QWidget* gui_parent_, int flags_,
				   const String& flt_start_txt)
 :inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
  list = NULL;
}

const String taiWidgetListElChooser::titleText() {
  String chs_title = "Choose " + itemTag();
  if(list) chs_title += " from list: " + list->GetDisplayName();
  return chs_title;
}

void taiWidgetListElChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!list) {
    taMisc::Error("taiWidgetListElChooser_base::BuildChooser: list needed");
    return;
  }
  switch (view) {
  case 0: 
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(" NULL", NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, " ");
    }
    BuildChooser_0(ic, list, NULL); 
    break; 
  default: break; // shouldn't happen
  }
}

void taiWidgetListElChooser::GetImage(taList_impl* base_lst, taBase* it) {
  list = base_lst;
  inherited::GetImage((void*)it, base_lst->el_base);
}

