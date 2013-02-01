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

#include "taiMethodDefButton.h"
#include <String_Array>
#include <iHelpBrowser>
#include <MethodDef>
#include <taiItemChooser>

#include <taMisc>
#include <taiMisc>

#include <QTreeWidget>
#include <QTreeWidgetItem>


taiMethodDefButton::taiMethodDefButton(TypeDef* typ_, IWidgetHost* host,
                                       taiWidget* par, QWidget* gui_parent_, int flags_,
                                       const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
}

void taiMethodDefButton::btnHelp_clicked() {
  MethodDef* mth = md();
  if (!mth) return;
  iHelpBrowser::StatLoadMethod(mth);
}

void taiMethodDefButton::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;

  MethodSpace* mbs = &targ_typ->methods;
  String cat;
  for (int i = 0; i < mbs->size; ++i) {
    MethodDef* mth = mbs->FastEl(i);
    if (!ShowMethod(mth)) continue;
    cat = mth->OptionAfter("CAT_"); // note: could be empty for no category
    cats->AddUnique(cat);
  }
  cats->Sort(); // empty, if any, should sort to top
}

void taiMethodDefButton::BuildChooser(taiItemChooser* ic, int view) {
  inherited::BuildChooser(ic, view);
  if (!targ_typ) {
    taMisc::Error("taiMethodDefButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0:
    BuildChooser_0(ic);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  case 1:
    ic->items->sortItems(1, Qt::AscendingOrder); // so items aren't sorted by 0
    BuildChooser_1(ic, targ_typ, NULL);
    ic->items->sortItems(1, Qt::AscendingOrder);
    break;
  case 2:
    BuildChooser_2(ic);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  default: break; // shouldn't happen
  }
}

void taiMethodDefButton::BuildChooser_0(taiItemChooser* ic) {
  MethodSpace* mbs = &targ_typ->methods;
  String cat;
  for (int i = 0; i < mbs->size; ++i) {
    MethodDef* mth = mbs->FastEl(i);
    if (!ShowMethod(mth)) continue;
    cat = mth->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name, NULL, (void*)mth);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
    item->setData(1, Qt::DisplayRole, mth->desc);
  }
}

int taiMethodDefButton::BuildChooser_1(taiItemChooser* ic, TypeDef* top_typ,
  QTreeWidgetItem* top_item)
{
  int rval = 0;
  String cat;
  MethodSpace* mbs = &top_typ->methods;
  QString typ_nm = top_typ->name; // let Qt share the rep
  // do methods at this level -- basically, anything living here, or not a virt override
  for (int i = 0; i < mbs->size; ++i) {
    MethodDef* mth = mbs->FastEl(i);
    if ((mth->owner != mbs) || mth->is_override) continue;
    if (!ShowMethod(mth)) continue;
    ++rval;
    cat = mth->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(typ_nm, top_item, (void*)mth);
    QVariant proto = mth->prototype(); // share
    item->setData(0, Qt::ToolTipRole, proto);
    item->setData(1, Qt::DisplayRole, mth->name);
    item->setData(1, Qt::ToolTipRole, proto);
    item->setData(2, Qt::DisplayRole, mth->desc);
  }
  // do parent type(s) as non-selectable folders
  for (int i = 0; i < top_typ->parents.size; ++i) {
    TypeDef* par_typ = top_typ->parents.FastEl(i);

    QTreeWidgetItem* item = ic->AddItem(par_typ->name, top_item, (void*)NULL);
    item->setFlags(Qt::ItemIsEnabled); // but not selectable
    //NOTE: no other column data, to keep the display clean
    // render the methods for this item -- if none, we'll just delete it
    int num = BuildChooser_1(ic, par_typ, item);
    //TODO: delete this item if num==0
    rval += num; // the result needs to include deeply nested methods
  }
  return rval;
}

void taiMethodDefButton::BuildChooser_2(taiItemChooser* ic) {
  MethodSpace* mbs = &targ_typ->methods;
  String cat;
  for (int i = 0; i < mbs->size; ++i) {
    MethodDef* mth = mbs->FastEl(i);
    if(!mth->HasOption("EXPERT")) continue;
    cat = mth->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name, NULL, (void*)mth);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
    item->setData(1, Qt::DisplayRole, mth->desc);
  }
}

int taiMethodDefButton::columnCount(int view) const {
  switch (view) {
  case 2:
  case 0: return 2;
  case 1: return 3;
  default: return 0; // not supposed to happen
  }
}

const String taiMethodDefButton::headerText(int index, int view) const {
  switch (view) {
  case 2:
  case 0: {
    switch (index) {
    case 0: return "Method";
    case 1: return "Description";
    }
    break;
  }
  case 1: {
    switch (index) {
    case 0: return "Class";
    case 1: return "Method";
    case 2: return "Description";
    }
    break;
  }
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiMethodDefButton::labelNameNonNull() const {
  return md()->name;
}

bool taiMethodDefButton::ShowMethod(MethodDef* mth) {
  return (ShowItemFilter(NULL, mth, mth->name) &&  mth->ShowMethod());
}

const String taiMethodDefButton::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List";
  case 1: return "Grouped By Class";
  case 2: return "Expert";
  default: return _nilString;
  }
}

