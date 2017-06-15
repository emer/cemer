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

#include "taiWidgetCompletionChooser.h"
#include <taList_impl>
#include <iDialogItemChooser>
#include <Completions>
#include <taMisc>

#include <QTreeWidgetItem>

taiWidgetCompletionChooser::taiWidgetCompletionChooser(TypeDef* typ_, IWidgetHost* host,
                                                         taiWidget* par, QWidget* gui_parent_, int flags_,
                                                         const String& flt_start_txt, int button_width)
:inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt, button_width)
{
}

void taiWidgetCompletionChooser::SetCompletions(Completions* completions_info) {
  completions = completions_info;
}

void taiWidgetCompletionChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!completions) {
    taMisc::Error("taiWidgetCompletionChooser::BuildChooser: completions object pointer not set");
    return;
  }
  switch (view) {
    case 0:
      if (HasFlag(flgNullOk)) {
        // note: ' ' makes it sort to the top
        QTreeWidgetItem* item = ic->AddItem(" NULL", NULL, (void*)NULL); //note: no desc
        item->setData(1, Qt::DisplayRole, " ");
      }
        BuildChooser_0(ic, completions, NULL);
      break;
    default: break; // shouldn't happen
  }
}

int taiWidgetCompletionChooser::BuildChooser_0(iDialogItemChooser* ic, Completions* the_completions,
                                                QTreeWidgetItem* top_item)
{
  int rval = 0;
  
  for (int i = 0; i < the_completions->object_completions.size; ++i) {
    taBase* tab = the_completions->object_completions.FastEl(i);
    if (!tab)  continue;
    if(filter_start_txt.nonempty()) {
      String nm = tab->GetName();
      if(nm.nonempty() && !nm.startsWith(filter_start_txt)) continue;
    }
    QTreeWidgetItem* item = ic->AddItem(tab->GetColText(taBase::key_disp_name), top_item, tab);
    item->setText(1, tab->GetColText(taBase::key_type));
    item->setText(2, tab->GetColText(taBase::key_desc));
    ++rval;
  }
  return rval;
}

int taiWidgetCompletionChooser::columnCount(int view) const {
  switch (view) {
    case 0: return 3;
    default: return 0; // not supposed to happen
  }
}

const String taiWidgetCompletionChooser::headerText(int index, int view) const {
  switch (view) {
    case 0: switch (index) {
      case 0: return "Name";
      case 1: return "Type";
      case 2: return "Description";
    } break;
    default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiWidgetCompletionChooser::labelNameNonNull() const {
  return item()->GetDisplayName();
}

const String taiWidgetCompletionChooser::viewText(int index) const {
  switch (index) {
    case 0: return "Flat List"; 
    default: return _nilString;
  }
}

void taiWidgetCompletionChooser::GetImage(taList_impl* base_lst, taBase* it) {
  list = base_lst;
  inherited::GetImage((void*)it, base_lst->el_base);
}


