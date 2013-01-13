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

#include "taiPolyData.h"
#include <MemberDef>
#include <taiMember>

#include <taMisc>
#include <taiMisc>



taiPolyData* taiPolyData::New(bool add_members, TypeDef* typ_, IDataHost* host_,
  taiData* par, QWidget* gui_parent_, int flags)
{
  taiPolyData*  rval = new taiPolyData(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  if (add_members)
    rval->AddTypeMembers();
  return rval;
}

bool taiPolyData::ShowMemberStat(MemberDef* md, int show) {
  if (md->HasOption("HIDDEN_INLINE") ||
    (md->type->HasOption("HIDDEN_INLINE") && !md->HasOption("SHOW_INLINE"))
    )
    return false;
  else
    return md->ShowMember((TypeItem::ShowMembs)show);
}

taiPolyData::taiPolyData(TypeDef* typ_, IDataHost* host_, taiData* par,
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

taiPolyData::~taiPolyData() {
  data_el.Reset();
}

bool taiPolyData::ShowMember(MemberDef* md) const {
  return ShowMemberStat(md, show);
}

void taiPolyData::AddChildMember(MemberDef* md) {
  memb_el.Add(md);
  inherited::AddChildMember(md);
}

void taiPolyData::AddTypeMembers() {
  InitLayout();
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md))
      continue;
    AddChildMember(md);
  }
  EndLayout();
}

void taiPolyData::Constr(QWidget* gui_parent_) {
  SetRep(MakeLayoutWidget(gui_parent_));
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep(), host->colorOfCurRow());
  }
}

void taiPolyData::ChildRemove(taiData* child) {
  int i = data_el.FindEl(child);
  if (i > 0)
    memb_el.RemoveIdx(i);
  inherited::ChildRemove(child);
}

void taiPolyData::GetImage_impl(const void* base_) {
  if (typ && typ->InheritsFrom(&TA_taBase)) {
    m_child_base = (taBase*)base_; // used for Seledit ctxt menus, and similar
  }
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    md->im->GetImage(mb_dat, base_);
  }
}

void taiPolyData::GetValue_impl(void* base_) const {
  bool rec_on = taMisc::record_on; // don't record script stuff now
  taMisc::record_on = false;
  bool first_diff = true;
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    md->im->GetMbrValue(mb_dat, base_, first_diff);
    if(m_child_base && !HasFlag(flgNoUAE)) {
      m_child_base->MemberUpdateAfterEdit(md, true); // edit dialog context
    }
  }
  if (m_child_base && !HasFlag(flgNoUAE)) {
     m_child_base->UpdateAfterEdit();   // hook to update the contents after an edit..
  }
  taMisc::record_on = rec_on;
}

