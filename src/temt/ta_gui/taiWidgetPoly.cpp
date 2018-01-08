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

#include "taiWidgetPoly.h"
#include <MemberDef>
#include <taiMember>

#include <taMisc>
#include <taiMisc>



taiWidgetPoly* taiWidgetPoly::New(bool add_members, TypeDef* typ_, IWidgetHost* host_,
  taiWidget* par, QWidget* gui_parent_, int flags)
{
  taiWidgetPoly*  rval = new taiWidgetPoly(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  if (add_members)
    rval->AddTypeMembers();
  return rval;
}

taiWidgetPoly::taiWidgetPoly(TypeDef* typ_, IWidgetHost* host_, taiWidget* par,
                         QWidget* gui_parent_, int flags_)
  : inherited(typ_, host_, par, gui_parent_, flags_)
{
  if (HasFlag(flgFlowLayout))
    lay_type = LT_Flow;
}

taiWidgetPoly::~taiWidgetPoly() {
  widget_el.Reset();
}

void taiWidgetPoly::AddChildMember(MemberDef* md, int column, bool mbr_type_only) {
  memb_el.Add(md);
  if(mbr_type_only) {
    taMisc::Warning("taiWidgetPoly::AddChildMember: we don't support mbr_type_only -- if needed impl as in Mashup!");
  }
  inherited::AddChildMember(md, column, false); // doesn't support mbr_type_only!
}

void taiWidgetPoly::AddTypeMembers() {
  InitLayout();
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    // todo: could add an option for seeing expert, but really, not..
    if(md->IsEditorHidden() || md->HasHiddenInline() || md->HasExpert())
      continue;
    AddChildMember(md);
  }
  EndLayout();
}

void taiWidgetPoly::Constr(QWidget* gui_parent_) {
  SetRep(MakeLayoutWidget(gui_parent_));
  if (host != NULL) {
    QPalette pal = rep()->palette();
    pal.setColor(QPalette::Background, host->colorOfCurRow());
    rep()->setPalette(pal); 
  }
}

void taiWidgetPoly::ChildRemove(taiWidget* child) {
  int i = widget_el.FindEl(child);
  if (i > 0)
    memb_el.RemoveIdx(i);
  inherited::ChildRemove(child);
}

void taiWidgetPoly::GetImage_impl(const void* base_) {
  if (typ && typ->IsActualTaBase()) {
    m_child_base = (taBase*)base_; // used for Seledit ctxt menus, and similar
  }
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taiWidget* mb_dat = widget_el.FastEl(i);
    md->im->GetImage(mb_dat, base_);
  }
}

void taiWidgetPoly::GetValue_impl(void* base_) const {
  bool rec_on = taMisc::record_on; // don't record script stuff now
  taMisc::record_on = false;
  bool first_diff = true;
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taiWidget* mb_dat = widget_el.FastEl(i);
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

