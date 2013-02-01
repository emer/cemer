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

#include "taiMemberOfFunPtr.h"
#include <taiWidget>
#include <taiWidgetMenuButton>
#include <MethodDef>
#include <taiWidgetMenu>

#include <taMisc>
#include <taiMisc>


int taiMemberOfFunPtr::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->fun_ptr)
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfFunPtr::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!
  taiWidgetMenuButton* rval = new taiWidgetMenuButton(taiWidgetMenu::radio_update, taiMisc::fonSmall,
      typ, host_, par, gui_parent_, flags_);
  rval->AddItem("NULL");
  rval->AddSep();
  for (int i = 0; i < taMisc::reg_funs.size; ++i) {
    TypeDef* rftp = taMisc::reg_funs.FastEl(i);
    if(!rftp->IsFunction() || rftp->methods.size != 1) continue; // shouldn't happen
    MethodDef* fun = rftp->methods[0];
    if (mbr->CheckList(fun->lists)) {
      rval->AddItem((char*)fun->name, (void*)fun->addr);
    }
  }
  return rval;
}

void taiMemberOfFunPtr::GetImage_impl(taiWidget* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiWidgetMenuButton* rval = (taiWidgetMenuButton*)dat;
  if(*((void**)new_base) == NULL) {
    rval->GetImageByData(Variant(0));
    return;
  }
  int cnt;
  MethodDef* fun = taMisc::FindRegFunListAddr(*((ta_void_fun*)new_base),
                                              mbr->lists, cnt);
  if (fun)
    rval->GetImageByIndex(cnt + 1); //1 for NULL item
  GetOrigVal(dat, base);
}

void taiMemberOfFunPtr::GetMbrValue_impl(taiWidget* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiWidgetMenuButton* rval = (taiWidgetMenuButton*)dat;
  iAction* cur = rval->curSel();
  if (cur != NULL)
    *((void**)new_base) = cur->usr_data.toPtr();
}
