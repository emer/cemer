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

#include "taiEnumTypeDefButton.h"
#include <iHelpBrowser>
#include <MethodDef>
#include <iTextBrowser>
#include <String_Array>
#include <MemberDef>
#include <taiItemChooser>

#include <taMisc>
#include <taiMisc>

#include <QTreeWidget>
#include <QTreeWidgetItem>


taiEnumTypeDefButton::taiEnumTypeDefButton(TypeDef* typ_, IDataHost* host,
                                           taiData* par, QWidget* gui_parent_, int flags_,
                                           const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
}

bool taiEnumTypeDefButton::AddType_Enum(TypeDef* typ_, TypeDef* par_typ) {
//TODO: doesn't seem to work for eliminating ex taBase::Orientation
//from showing up in template types
  //note: we already determined typ_ is an enum
  if (typ_->HasOption("HIDDEN")) return false;
  // because enums are inherited, only show in the type itself
  // we will show all base and inherited from top, but then only
  // base for all inherited from top

  // easiest to determine is when this is directly in parent
  TypeDef* ot = typ_->GetOwnerType(); //cache
  if (ot == par_typ) return true;
  // ok, so not in parent, only other case we allow is when it
  // is inherited in the top type, then we only show in that type
  if ((par_typ == targ_typ) && (ot == targ_typ)) return true;
  return false;
}

void taiEnumTypeDefButton::btnHelp_clicked() {
  TypeDef* typ = td();
  if (!typ) return;
  iHelpBrowser::StatLoadEnum(typ);
}

void taiEnumTypeDefButton::BuildChooser(taiItemChooser* ic, int view) {
  inherited::BuildChooser(ic, view);
  if (!targ_typ) {
    taMisc::Error("taiEnumTypeDefButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0:
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(" ", NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, nullText());
    }
    BuildChooser_0(ic, targ_typ, NULL);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  default: break; // shouldn't happen
  }
}

int taiEnumTypeDefButton::BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ,
  QTreeWidgetItem* top_item)
{
  int rval = 0;

  // add Enums of this type
  for (int i = 0; i < top_typ->sub_types.size; ++i) {
    TypeDef* chld = top_typ->sub_types.FastEl(i);
    if (!chld->IsEnum()) continue;
    if (AddType_Enum(chld, top_typ)) {
      QTreeWidgetItem* item = ic->AddItem(top_typ->name, top_item, (void*)chld);
      item->setData(1, Qt::DisplayRole, chld->name);
      item->setData(2, Qt::DisplayRole, chld->desc);
      ++rval;
    }
  }
  // add entries for the subclasses (but only full class types)
  for (int i = 0; i < top_typ->children.size; ++i) {
    TypeDef* chld = top_typ->children.FastEl(i);
    if (chld->IsAnyPtr() || !chld->IsClass())
      continue;
    //note: we are recursive, but aren't making a tree
    int num = BuildChooser_0(ic, chld, top_item);
    rval += num;
  }
  return rval;
}

int taiEnumTypeDefButton::columnCount(int view) const {
  switch (view) {
  case 0: return 3;
  default: return 0; // not supposed to happen
  }
}

const String taiEnumTypeDefButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Type";
    case 1: return "Enum";
    case 2: return "Description";
    } break;
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}
