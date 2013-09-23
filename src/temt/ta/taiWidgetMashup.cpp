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

#include "taiWidgetMashup.h"
#include <MemberDef>
#include <taiMember>

#include <taMisc>
#include <taiMisc>


taiWidgetMashup* taiWidgetMashup::New(bool add_members, TypeDef* typ_, IWidgetHost* host_,
  taiWidget* par, QWidget* gui_parent_, int flags)
{
  taiWidgetMashup*  rval = new taiWidgetMashup(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  if (add_members)
    rval->AddTypeMembers();
  return rval;
}

bool taiWidgetMashup::ShowMemberStat(MemberDef* md, int show) {
  if (md->HasOption("HIDDEN_INLINE") ||
    (md->type->HasOption("HIDDEN_INLINE") && !md->HasOption("SHOW_INLINE"))
    )
    return false;
  else
    return md->ShowMember((TypeItem::ShowMembs)show);
}

taiWidgetMashup::taiWidgetMashup(TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                         QWidget* gui_parent_, int flags_)
  : inherited(typ_, host_, par, gui_parent_, flags_)
{
  if (HasFlag(flgFlowLayout))
    lay_type = LT_Flow;
  if (host_) {
    show = host_->show();
  }
  else {
    show = taMisc::show_gui;
  }
}

taiWidgetMashup::~taiWidgetMashup() {
  data_el.Reset();
}

bool taiWidgetMashup::ShowMember(MemberDef* md) const {
  return ShowMemberStat(md, show);
}

void taiWidgetMashup::AddChildMember(MemberDef* md) {
  memb_el.Add(md);
  inherited::AddChildMember(md);
}

void taiWidgetMashup::AddBase(taBase* b) {
  memb_bases.Add(b);
}

void taiWidgetMashup::SetBases(taBase* b1, taBase* b2, taBase* b3,
                               taBase* b4, taBase* b5, taBase* b6) {
  memb_bases.Reset();
  if(b1 != NULL) memb_bases.Add(b1);
  if(b2 != NULL) memb_bases.Add(b2);
  if(b3 != NULL) memb_bases.Add(b3);
  if(b4 != NULL) memb_bases.Add(b4);
  if(b5 != NULL) memb_bases.Add(b5);
  if(b6 != NULL) memb_bases.Add(b6);
}

void taiWidgetMashup::AddTypeMembers() {
  InitLayout();
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md))
      continue;
    AddChildMember(md);
  }
  EndLayout();
}

void taiWidgetMashup::Constr(QWidget* gui_parent_) {
  SetRep(MakeLayoutWidget(gui_parent_));
  if (host != NULL) {
    QPalette pal = rep()->palette();
    pal.setColor(QPalette::Background, host->colorOfCurRow());
    rep()->setPalette(pal); 
  }
}

void taiWidgetMashup::ChildRemove(taiWidget* child) {
  int i = data_el.FindEl(child);
  if (i > 0)
    memb_el.RemoveIdx(i);
  inherited::ChildRemove(child);
}

void taiWidgetMashup::GetImage_impl(const void* base_) {
  if(memb_bases.size != memb_el.size) {
    taMisc::Error("taiWidgetMashup: programmer error -- must call SetBases or AddBase so that memb_bases is same size as memb_el");
    return;
  }
  // if (typ && typ->IsActualTaBase()) {
  //   m_child_base = (taBase*)base_; // used for Seledit ctxt menus, and similar
  // }
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taBase* bs = memb_bases.FastEl(i);
    taiWidget* mb_dat = data_el.FastEl(i);
    md->im->GetImage(mb_dat, bs);
  }
}

void taiWidgetMashup::GetValue_impl(void* base_) const {
  if(memb_bases.size != memb_el.size) {
    taMisc::Error("taiWidgetMashup: programmer error -- must call SetBases or AddBase so that memb_bases is same size as memb_el");
    return;
  }
  bool rec_on = taMisc::record_on; // don't record script stuff now
  taMisc::record_on = false;
  bool first_diff = true;
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taBase* bs = memb_bases.FastEl(i);
    taiWidget* mb_dat = data_el.FastEl(i);
    md->im->GetMbrValue(mb_dat, bs, first_diff);
    if(bs && !HasFlag(flgNoUAE)) {
      bs->MemberUpdateAfterEdit(md, true); // edit dialog context
    }
  }
  if (bs && !HasFlag(flgNoUAE)) {
    bs->UpdateAfterEdit();   // hook to update the contents after an edit..
  }
  taMisc::record_on = rec_on;
}


