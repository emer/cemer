// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiWidgetEnumStaticChooser.h"
#include <iHelpBrowser>
#include <MethodDef>
#include <EnumDef>
#include <iTextBrowser>
#include <String_Array>
#include <MemberDef>
#include <iDialogItemChooser>

#include <taMisc>
#include <taiMisc>

#include <iTreeWidget>
#include <QTreeWidgetItem>



taiWidgetEnumStaticChooser::taiWidgetEnumStaticChooser(TypeDef* typ_, IWidgetHost* host,
                                         taiWidget* par, QWidget* gui_parent_, int flags_,
                                         const String& flt_start_txt)
  : inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt)
{
}

void taiWidgetEnumStaticChooser::btnHelp_clicked() {
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
  EnumDef* ed = dynamic_cast<EnumDef*>(md());
  if (ed) {
//TODO    iHelpBrowser::StatLoadEnumDef(ed);
    return;
  }
}

void taiWidgetEnumStaticChooser::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;
  cats->AddUnique("Enums");

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
    cat = mth->OptionAfter("CAT_"); // note: could be empty for no category
    if(cat == "IGNORE") continue;
    cat = "method: " + cat;
    cats->AddUnique(cat);
  }
  cats->Sort(); // empty, if any, should sort to top
}

void taiWidgetEnumStaticChooser::BuildChooser(iDialogItemChooser* ic, int view) {
  inherited::BuildChooser(ic, view);
  if (!targ_typ) {
    taMisc::Error("taiWidgetEnumStaticChooser::BuildChooser: targ_type needed");
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
  case 4:
    BuildChooser_4(ic);
    ic->items->sortItems(0, Qt::AscendingOrder);
    break;
  default: break; // shouldn't happen
  }
}

void taiWidgetEnumStaticChooser::BuildChooser_0(iDialogItemChooser* ic) {
  String cat;
  cat = "Enum";
  for(int i=0; i < targ_typ->sub_types.size; i++) {
    TypeDef* td = targ_typ->sub_types.FastEl(i);
    if(td->IsEnum()) {
      for(int j=0;j< td->enum_vals.size; j++) {
        EnumDef* ed = td->enum_vals.FastEl(j);
        if(!ShowEnum(ed)) continue;
        QTreeWidgetItem* item = ic->AddItem(cat, ed->name + " (enum)", NULL, (void*)ed,
                                            ed->desc);
      }
    }
  }

  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if (!ShowMember(mbr)) continue;
    cat = mbr->OptionAfter("CAT_");
    if(cat == "IGNORE") continue;
    cat = "member: " + cat;
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name + " (member)", NULL, (void*)mbr,
                                        mbr->desc);
  }

  MethodSpace* mts = &targ_typ->methods;
  for (int i = 0; i < mts->size; ++i) {
    MethodDef* mth = mts->FastEl(i);
    if (!ShowMethod(mth)) continue;
    cat = mth->OptionAfter("CAT_");
    if(cat == "IGNORE") continue;
    cat = "method: " + cat;
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name + " (method)", NULL, (void*)mth,
                                        mth->desc);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
  }
}

void taiWidgetEnumStaticChooser::BuildChooser_1(iDialogItemChooser* ic) {
  String cat;
  cat = "Enum";
  for(int i=0; i < targ_typ->sub_types.size; i++) {
    TypeDef* td = targ_typ->sub_types.FastEl(i);
    if(td->IsEnum()) {
      for(int j=0;j< td->enum_vals.size; j++) {
        EnumDef* ed = td->enum_vals.FastEl(j);
        if(!ShowEnum(ed)) continue;
        QTreeWidgetItem* item = ic->AddItem(cat, ed->name, NULL, (void*)ed, ed->desc);
      }
    }
  }
}

void taiWidgetEnumStaticChooser::BuildChooser_2(iDialogItemChooser* ic) {
  String cat;
  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if (!ShowMember(mbr)) continue;
    cat = mbr->OptionAfter("CAT_");
    if(cat == "IGNORE") continue;
    cat = "member: " + cat;
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name, NULL, (void*)mbr, mbr->desc);
  }
}

void taiWidgetEnumStaticChooser::BuildChooser_3(iDialogItemChooser* ic) {
  String cat;
  MethodSpace* mts = &targ_typ->methods;
  for (int i = 0; i < mts->size; ++i) {
    MethodDef* mth = mts->FastEl(i);
    if (!ShowMethod(mth)) continue;
    cat = mth->OptionAfter("CAT_");
    if(cat == "IGNORE") continue;
    cat = "method: " + cat;
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name, NULL, (void*)mth, mth->desc);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
  }
}

void taiWidgetEnumStaticChooser::BuildChooser_4(iDialogItemChooser* ic) {
  String cat;
  cat = "Enum";
  for(int i=0; i < targ_typ->sub_types.size; i++) {
    TypeDef* td = targ_typ->sub_types.FastEl(i);
    if(td->IsEnum()) {
      for(int j=0;j< td->enum_vals.size; j++) {
        EnumDef* ed = td->enum_vals.FastEl(j);
        if(!ed->HasOption("EXPERT")) continue;
        QTreeWidgetItem* item = ic->AddItem(cat, ed->name + " (enum)", NULL, (void*)ed,
                                            ed->desc);
      }
    }
  }

  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    if(!mbr->HasExpert()) continue;
    cat = mbr->OptionAfter("CAT_");
    if(cat == "IGNORE") continue;
    cat = "member: " + cat;
    QTreeWidgetItem* item = ic->AddItem(cat, mbr->name + " (member)", NULL, (void*)mbr,
                                        mbr->desc);
  }

  MethodSpace* mts = &targ_typ->methods;
  for (int i = 0; i < mts->size; ++i) {
    MethodDef* mth = mts->FastEl(i);
    if(!mth->HasOption("EXPERT")) continue;
    cat = mth->OptionAfter("CAT_");
    if(cat == "IGNORE") continue;
    cat = "method: " + cat;
    QTreeWidgetItem* item = ic->AddItem(cat, mth->name + " (method)", NULL, (void*)mth,
                                        mth->desc);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
  }
}

int taiWidgetEnumStaticChooser::columnCount(int view) const {
  return 2;                     // always 2
}

const String taiWidgetEnumStaticChooser::headerText(int index, int view) const {
  if(index == 1) return "Description";
  if(index == 0) {
    switch (view) {
    case 0: return "Item";
    case 1: return "Enum";
    case 2: return "Static Member";
    case 3: return "Static Method";
    case 4: return "Item";
    default: break; // compiler food
    }
  }
  return _nilString; // shouldn't happen
}

const String taiWidgetEnumStaticChooser::labelNameNonNull() const {
  return md()->name;
}

bool taiWidgetEnumStaticChooser::ShowEnum(EnumDef* enm) {
  if(enm->HasOption("EXPERT"))
    return false;
  return (ShowItemFilter(NULL, enm, enm->name));
}

bool taiWidgetEnumStaticChooser::ShowMethod(MethodDef* mth) {
  if(!mth->is_static) return false;
  return (ShowItemFilter(NULL, mth, mth->name)  &&  mth->ShowMethod());
}

bool taiWidgetEnumStaticChooser::ShowMember(MemberDef* mbr) {
  if(!mbr->is_static) return false;
  return (ShowItemFilter(NULL, mbr, mbr->name) &&  !mbr->IsEditorHidden());
}

const String taiWidgetEnumStaticChooser::viewText(int index) const {
  switch (index) {
  case 0: return "Enums & statics";
  case 1: return "Enums";
  case 2: return "Static Members";
  case 3: return "Static Methods";
  case 4: return "Expert Items";
  default: return _nilString;
  }
}

