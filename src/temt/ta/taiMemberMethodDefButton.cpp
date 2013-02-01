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

#include "taiMemberMethodDefButton.h"
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



taiMemberMethodDefButton::taiMemberMethodDefButton(TypeDef* typ_, IWidgetHost* host,
                   taiData* par, QWidget* gui_parent_, int flags_,
                                                   const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
}

void taiMemberMethodDefButton::btnHelp_clicked() {
  MethodDef* mth = dynamic_cast<MethodDef*>(md());
  if (mth) {
    iHelpBrowser::StatLoadMethod(mth);
    return;
  }
  MemberDef* mbr = dynamic_cast<MemberDef*>(md());
  if (mbr) {
    iHelpBrowser::StatLoadMember(mbr);
    return;
  }
}

void taiMemberMethodDefButton::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;
  String cat;

  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if (!ShowMember(mbr)) continue;
    cat = "member: " + mbr->OptionAfter("CAT_"); // note: could be empty for no category
    cats->AddUnique(cat);
  }

  MethodSpace* mts = &targ_typ->methods;
  for (int i = 0; i < mts->size; ++i) {
    MethodDef* mth = mts->FastEl(i);
    if (!ShowMethod(mth)) continue;
    cat = "method: " + mth->OptionAfter("CAT_"); // note: could be empty for no category
    cats->AddUnique(cat);
  }
  cats->Sort(); // empty, if any, should sort to top
}

void taiMemberMethodDefButton::BuildChooser(taiItemChooser* ic, int view) {
  inherited::BuildChooser(ic, view);

  if (!targ_typ) {
    taMisc::Error("taiMemberMethodDefButton::BuildChooser: targ_type needed");
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
  case 2:
    BuildChooser_2(ic);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  case 3:
    BuildChooser_3(ic);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  default: break; // shouldn't happen
  }
}

void taiMemberMethodDefButton::BuildChooser_0(taiItemChooser* ic) {
  String cat;
  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if (!ShowMember(mbr)) continue;
    cat = "member: " + mbr->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name + " (member)", NULL, (void*)mbr);
    item->setData(1, Qt::DisplayRole, mbr->desc);
  }

  MethodSpace* mts = &targ_typ->methods;
  for (int i = 0; i < mts->size; ++i) {
    MethodDef* mth = mts->FastEl(i);
    if (!ShowMethod(mth)) continue;
    cat = "method: " + mth->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name + " (method)", NULL, (void*)mth);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
    item->setData(1, Qt::DisplayRole, mth->desc);
  }
}

void taiMemberMethodDefButton::BuildChooser_1(taiItemChooser* ic) {
  String cat;
  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if (!ShowMember(mbr)) continue;
    cat = "member: " + mbr->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name, NULL, (void*)mbr);
    item->setData(1, Qt::DisplayRole, mbr->desc);
  }
}

void taiMemberMethodDefButton::BuildChooser_2(taiItemChooser* ic) {
  String cat;
  MethodSpace* mts = &targ_typ->methods;
  for (int i = 0; i < mts->size; ++i) {
    MethodDef* mth = mts->FastEl(i);
    if (!ShowMethod(mth)) continue;
    cat = "method: " + mth->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name, NULL, (void*)mth);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
    item->setData(1, Qt::DisplayRole, mth->desc);
  }
}

void taiMemberMethodDefButton::BuildChooser_3(taiItemChooser* ic) {
  String cat;
  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if(!mbr->HasOption("EXPERT")) continue;
    cat = "member: " + mbr->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name + " (member)", NULL, (void*)mbr);
    item->setData(1, Qt::DisplayRole, mbr->desc);
  }

  MethodSpace* mts = &targ_typ->methods;
  for (int i = 0; i < mts->size; ++i) {
    MethodDef* mth = mts->FastEl(i);
    if(!mth->HasOption("EXPERT")) continue;
    cat = "method: " + mth->OptionAfter("CAT_");
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name + " (method)", NULL, (void*)mth);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
    item->setData(1, Qt::DisplayRole, mth->desc);
  }
}

int taiMemberMethodDefButton::columnCount(int view) const {
  return 2;                     // always 2
}

const String taiMemberMethodDefButton::headerText(int index, int view) const {
  if(index == 1) return "Description";
  if(index == 0) {
    switch (view) {
    case 0: return "Member/Method";
    case 1: return "Member";
    case 2: return "Method";
    case 3: return "Member/Method";
    default: break; // compiler food
    }
  }
  return _nilString; // shouldn't happen
}

const String taiMemberMethodDefButton::labelNameNonNull() const {
  return md()->name;
}

bool taiMemberMethodDefButton::ShowMethod(MethodDef* mth) {
  return (ShowItemFilter(NULL, mth, mth->name) &&  mth->ShowMethod());
}

bool taiMemberMethodDefButton::ShowMember(MemberDef* mbr) {
  // showing all members here, because this is used primarily in program lookups
  // and often you need to see things that are otherwise hidden
  // TODO: should have a ShowContext::SC_CHOOSER or something like that context
  // where it adjudicates these things..
  return (ShowItemFilter(NULL, mbr, mbr->name)); //  &&  mbr->ShowMember());
}

const String taiMemberMethodDefButton::viewText(int index) const {
  switch (index) {
  case 0: return "Members & Methods";
  case 1: return "Members";
  case 2: return "Methods";
  case 3: return "Expert Items";
  default: return _nilString;
  }
}

