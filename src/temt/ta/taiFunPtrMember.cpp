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

#include "taiFunPtrMember.h"
#include <taiData>
#include <taiButtonMenu>
#include <MethodDef>
#include <taiMenu>

#include <taiMisc>


int taiFunPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->fun_ptr)
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiFunPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiData::flgAutoApply; // default is to auto-apply!
  taiButtonMenu* rval = new taiButtonMenu(taiMenu::radio_update, taiMisc::fonSmall,
      typ, host_, par, gui_parent_, flags_);
  rval->AddItem("NULL");
  rval->AddSep();
  MethodDef* fun;
  // for (int i = 0; i < TA_taRegFun.methods.size; ++i) {
  //   fun = TA_taRegFun.methods.FastEl(i);
  //   if (mbr->CheckList(fun->lists))
  //     rval->AddItem((char*)fun->name, (void*)fun->addr);
  // }
  return rval;
}

void taiFunPtrMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiButtonMenu* rval = (taiButtonMenu*)dat;
  if(*((void**)new_base) == NULL) {
    rval->GetImageByData(Variant(0));
    return;
  }
  int cnt;
  MethodDef* fun = NULL; //TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)new_base),
  //                                                        mbr->lists, cnt);
  if (fun)
    rval->GetImageByIndex(cnt + 1); //1 for NULL item
  GetOrigVal(dat, base);
}

void taiFunPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiButtonMenu* rval = (taiButtonMenu*)dat;
  taiAction* cur = rval->curSel();
  if (cur != NULL)
    *((void**)new_base) = cur->usr_data.toPtr();
}
