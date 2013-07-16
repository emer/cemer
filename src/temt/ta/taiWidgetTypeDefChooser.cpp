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

#include "taiWidgetTypeDefChooser.h"
#include <iHelpBrowser>
#include <iTextBrowser>
#include <iDialogItemChooser>

#include <String_Array>

#include <taMisc>
#include <taiMisc>

#include <iTreeWidget>
#include <QTreeWidgetItem>


taiWidgetTypeDefChooser::taiWidgetTypeDefChooser(TypeDef* typ_, IWidgetHost* host,
                                   taiWidget* par, QWidget* gui_parent_, int flags_,
                                   const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
}

taiWidgetTypeDefChooser::TypeCat taiWidgetTypeDefChooser::AddType_Class(TypeDef* typ_) {
  if ((typ_->IsAnyPtr()) || (typ_->HasOption("HIDDEN"))) return TC_NoAdd;
  if (!typ_->IsActualClassNoEff()) // only type classes please..
    return TC_NoAdd;
  // no nested typedefs TODO: find a better way to identify nested typedefs
  if (typ_->name == "inherited") return TC_NoAdd;

  // we don't add templates, but we do add their children
  if (typ_->IsTemplInst())
    return TC_NoAddCheckChildren;

  if(typ_->HasOption("VIRT_BASE")) {
    return TC_NoAddCheckChildren;
  }

  // don't clutter list with these..
  if((typ_->members.size==0) && (typ_->methods.size==0) && !(typ_ == &TA_taBase))
    return TC_NoAdd;
  return TC_Add;
}

void taiWidgetTypeDefChooser::btnHelp_clicked() {
  TypeDef* typ = td();
  if (!typ) return;
  iHelpBrowser::StatLoadType(typ);
}

void taiWidgetTypeDefChooser::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;
  BuildCategoriesR_impl(targ_typ);
  cats->Sort(); // empty, if any, should sort to top
}

void taiWidgetTypeDefChooser::BuildCategoriesR_impl(TypeDef* top_typ) {
  TypeCat tc = AddType_Class(top_typ);
  switch (tc) {
  case TC_NoAdd : return;
  case TC_NoAddCheckChildren: break;
  case TC_Add: {
    String cat = top_typ->OptionAfter("CAT_"); // note: could be empty for no category
    cats->AddUnique(cat);
    } break;
  }
  for (int i = 0; i < top_typ->children.size; ++i) {
    TypeDef* chld = top_typ->children.FastEl(i);
    BuildCategoriesR_impl(chld);
  }
}

bool taiWidgetTypeDefChooser::hasNoItems() {
  // always false for this -- targ_typ is snould not be null!
  return false;
}

bool taiWidgetTypeDefChooser::hasOnlyOneItem() {
  if(HasFlag(flgNullOk)) return false; // we now have 2 -- targ_typ and null
  if(!targ_typ) return false;          // shouldn't happen
  return (targ_typ->children.size == 0); // if we have no children, then there is only 1!
}

void taiWidgetTypeDefChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  inherited::BuildChooser(ic, view);
  if (!targ_typ) {
    taMisc::Error("taiWidgetTypeDefChooser::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0:
    if (HasFlag(flgNullOk)) {
      QTreeWidgetItem* item = ic->AddItem(nullText(), NULL, (void*)NULL);
      item->setData(1, Qt::DisplayRole, " "); //note: no desc
    }
    BuildChooser_0(ic, targ_typ, NULL);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  default: break; // shouldn't happen
  }
}

int taiWidgetTypeDefChooser::BuildChooser_0(iDialogItemChooser* ic, TypeDef* top_typ,
                                     QTreeWidgetItem* top_item)
{
  int rval = 0;
  QTreeWidgetItem* item = NULL; // used for intermediate items
  TypeCat tc = AddType_Class(top_typ);
  switch (tc) {
  case TC_NoAdd : return rval;
  case TC_NoAddCheckChildren: break;
  //TODO: maybe we should create a node for these, since they are typically templates
  // but then again, usually there will be a _impl just prior that will get a node
  case TC_Add: {
    TypeDef* par = top_typ->parents.SafeEl(0); // NULL for root types, ex. taBase
    String par_name;
    if (par) par_name = par->name;
    String cat = top_typ->OptionAfter("CAT_"); // note: could be empty for no category
    item = ic->AddItem(cat, top_typ->name, top_item, (void*)top_typ);
    item->setData(1, Qt::DisplayRole, par_name);
    item->setData(2, Qt::DisplayRole, top_typ->desc);
    ++rval;
    } break;
  }
  for (int i = 0; i < top_typ->children.size; ++i) {
    TypeDef* chld = top_typ->children.FastEl(i);
    rval += BuildChooser_0(ic, chld, top_item); // note, we just build a list
  }
  //TODO: if a NoAddCheckChildren didn't have items, delete it
  // do initial sort
  return rval;
}

int taiWidgetTypeDefChooser::columnCount(int view) const {
  switch (view) {
  case 0: return 3;
  default: return 0; // not supposed to happen
  }
}

int taiWidgetTypeDefChooser::CountChildren(TypeDef* td) {
  int rval = 0;
  TypeDef* chld;
  for (int i = 0; i < td->children.size; ++i) {
    chld = td->children[i];
    if (chld->IsAnyPtr())
      continue;
    ++rval;
  }
  return rval;
}

const String taiWidgetTypeDefChooser::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Type";
    case 1: return "Par Type";
    case 2: return "Description";
    } break;
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiWidgetTypeDefChooser::labelNameNonNull() const {
  return td()->name;
}

const String taiWidgetTypeDefChooser::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List";
//case 1: return "By Class Hierarchy";
  default: return _nilString;
  }
}
