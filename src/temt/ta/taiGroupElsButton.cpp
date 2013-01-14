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

#include "taiGroupElsButton.h"
#include <taGroup_impl>
#include <taiItemChooser>

#include <taMisc>

#include <QTreeWidgetItem>

taiGroupElsButton::taiGroupElsButton(TypeDef* typ_, IDataHost* host,
				     taiData* par, QWidget* gui_parent_, int flags_,
				     const String& flt_start_txt)
 :inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
  grp = NULL;
}

const String taiGroupElsButton::titleText() {
  String chs_title = "Choose " + itemTag();
  if(grp) chs_title += " from group: " + grp->GetDisplayName();
  return chs_title;
}

void taiGroupElsButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!grp) {
    taMisc::Error("taiGroupElsButtonBase::BuildChooser: group needed");
    return;
  }
  switch (view) {
  case 0: 
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(" NULL", NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, " ");
    }
    BuildChooser_1(ic, grp, NULL); 
    break; 
  default: break; // shouldn't happen
  }
}

int taiGroupElsButton::BuildChooser_1(taiItemChooser* ic, taGroup_impl* top_grp, 
  QTreeWidgetItem* top_item) 
{
  int rval = 0;
  
  for (int i = 0; i < top_grp->gp.size; ++i) {
    taGroup_impl* tag = (taGroup_impl*)top_grp->gp.FastEl_(i);
    if (!tag)  continue;
    QTreeWidgetItem* item = ic->AddItem(tag->GetDisplayName(), top_item, tag); 
    item->setFlags(Qt::ItemIsEnabled); // not selectable
    //note: don't put text in the other columns, to keep items clean
    //TODO: put folder icon
    rval += BuildChooser_1(ic, tag, item);
    ++rval;
  }
  
  rval += BuildChooser_0(ic, top_grp, top_item); 
  return rval;
}

void taiGroupElsButton::GetImage(taGroup_impl* base_grp, taBase* it) {
  grp = base_grp;
  inherited::GetImage((void*)it, NULL); // don't need a targ_typ
}

