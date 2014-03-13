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

#include "taiWidgetMemberDefChooser.h"

#include <iHelpBrowser>
#include <String_Array>
#include <MemberDef>
#include <iDialogItemChooser>

#include <iTreeWidget>
#include <QTreeWidgetItem>

#include <taMisc>
#include <taiMisc>




taiWidgetMemberDefChooser::taiWidgetMemberDefChooser(TypeDef* typ_, IWidgetHost* host,
                                       taiWidget* par, QWidget* gui_parent_, int flags_,
                                       const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
}

void taiWidgetMemberDefChooser::btnHelp_clicked() {
  MemberDef* mbr = md();
  if (!mbr) return;
  iHelpBrowser::StatLoadMember(mbr);
}

void taiWidgetMemberDefChooser::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;

  MemberSpace* mbs = &targ_typ->members;
  String cat;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if (!ShowMember(mbr)) continue;
    cat = mbr->OptionAfter("CAT_"); // note: could be empty for no category
    cats->AddUnique(cat);
  }
  cats->Sort(); // empty, if any, should sort to top
}

void taiWidgetMemberDefChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  inherited::BuildChooser(ic, view);
  if (!targ_typ) {
    taMisc::Error("taiWidgetMemberDefChooser::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0:
    BuildChooser_0(ic);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  case 1:
    BuildChooser_1(ic);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  default: break; // shouldn't happen
  }
}

void taiWidgetMemberDefChooser::BuildChooser_0(iDialogItemChooser* ic) {
  MemberSpace* mbs = &targ_typ->members;
  String cat;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if (!ShowMember(mbr)) continue;
    cat = mbr->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name, NULL, (void*)mbr, mbr->desc);
  }
}

void taiWidgetMemberDefChooser::BuildChooser_1(iDialogItemChooser* ic) {
  MemberSpace* mbs = &targ_typ->members;
  String cat;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if(!mbr->HasOption("EXPERT")) continue;
    cat = mbr->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name, NULL, (void*)mbr, mbr->desc);
  }
}

int taiWidgetMemberDefChooser::columnCount(int view) const {
  return 2;                     // always 2
}

const String taiWidgetMemberDefChooser::headerText(int index, int view) const {
  switch (view) {
  case 1:
  case 0: {
    switch (index) {
    case 0: return "Member";
    case 1: return "Description";
    }
    break;
  }
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiWidgetMemberDefChooser::labelNameNonNull() const {
  return md()->name;
}

bool taiWidgetMemberDefChooser::ShowMember(MemberDef* mbr) {
  // showing all members here, because this is used primarily in program lookups
  // and often you need to see things that are otherwise hidden
  // TODO: should have a ShowContext::SC_CHOOSER or something like that context
  // where it adjudicates these things..
  return (ShowItemFilter(NULL, mbr, mbr->name)); // && mbr->ShowMember());
}

const String taiWidgetMemberDefChooser::viewText(int index) const {
  switch (index) {
  case 0: return "Members";
  case 1: return "Expert";
  default: return _nilString;
  }
}

