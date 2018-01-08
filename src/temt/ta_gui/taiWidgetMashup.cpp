// Copyright 2013-2018, Regents of the University of Colorado,
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

taiWidgetMashup::taiWidgetMashup(TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                         QWidget* gui_parent_, int flags_)
  : inherited(typ_, host_, par, gui_parent_, flags_)
{
  if (HasFlag(flgFlowLayout))
    lay_type = LT_Flow;
}

taiWidgetMashup::~taiWidgetMashup() {
  widget_el.Reset();
}

void taiWidgetMashup::AddChildMember(MemberDef* md, int column, bool mbr_type_only) {
  memb_el.Add(md);
  type_only.Add(mbr_type_only);
  inherited::AddChildMember(md, column, mbr_type_only);
}

void taiWidgetMashup::AddBase(taBase* b) {
  memb_bases.Add(b);
}

void taiWidgetMashup::GetImage() {
  GetImage_impl(NULL);
}
  
void taiWidgetMashup::GetValue() {
  GetValue_impl(NULL);
}

void taiWidgetMashup::ResetBases() {
  memb_bases.Reset();
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
    if(md->IsEditorHidden() || md->HasHiddenInline())
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
  int i = widget_el.FindEl(child);
  if (i > 0)
    memb_el.RemoveIdx(i);
  inherited::ChildRemove(child);
}

void taiWidgetMashup::GetImage_impl(const void* base_) {
  if(memb_bases.size != memb_el.size) {
    taMisc::Error("taiWidgetMashup: programmer error -- must call SetBases or AddBase so that memb_bases is same size as memb_el");
    return;
  }
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taBase* bs = memb_bases.FastEl(i);
    taiWidget* mb_dat = widget_el.FastEl(i);
    m_child_base = bs;
    if(type_only[i]) {
      md->type->it->GetImage(mb_dat, bs);
    }
    else {
      md->im->GetImage(mb_dat, bs);
    }
  }
  m_child_base = NULL;
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
    m_child_base = bs;
    taiWidget* mb_dat = widget_el.FastEl(i);
    if(type_only[i]) {
      md->type->it->GetValue(mb_dat, bs);
    }
    else {
      md->im->GetMbrValue(mb_dat, bs, first_diff);
      if(bs && !HasFlag(flgNoUAE)) {
        bs->MemberUpdateAfterEdit(md, true); // edit dialog context
      }
    }
    if (bs && !HasFlag(flgNoUAE)) {
      bs->UpdateAfterEdit();   // hook to update the contents after an edit..
    }
  }
  m_child_base = NULL;
  taMisc::record_on = rec_on;
}


