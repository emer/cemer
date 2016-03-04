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

#include "taiMemberOfTokenPtrFromGroup.h"
#include <taiWidgetSubGroupMenu>
#include <taiWidgetGroupElMenu>
#include <taiWidgetListElMenu>
#include <taiWidgetMenu>

#include <taMisc>
#include <taiMisc>

taTypeDef_Of(taSmartPtr);
taTypeDef_Of(taGroup_impl);

int taiMemberOfTokenPtrFromGroup::BidForMember(MemberDef* md, TypeDef* td) {
  if(td->IsActualTaBase() && md->type->IsBasePointerType()
     && (md->OptionAfter("FROM_GROUP_").nonempty() ||
         md->OptionAfter("FROM_LIST_").nonempty()))
    return taiMemberOfTokenPtr::BidForMember(md,td)+1;
  return 0;
}

taiWidget* taiMemberOfTokenPtrFromGroup::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {

  // setting mode now is good for rest of life
  if(mbr->type->IsTaBase())
    mode = MD_BASE;
  else if (mbr->type->DerivesFrom(TA_taSmartPtr))
    mode = MD_SMART_PTR;
  else if (mbr->type->DerivesFrom(TA_taSmartRef))
    mode = MD_SMART_REF;

  bool is_group = false;
  String fga = mbr->OptionAfter("FROM_GROUP_");
  if(fga.nonempty()) {
    is_group = true;
  }
  // else list
  
  int new_flags = flags_;
  if(!mbr->HasOption("NO_NULL"))
    new_flags |= taiWidget::flgNullOk;
  if(!mbr->HasOption("NO_EDIT"))
    new_flags |= taiWidget::flgEditOk;
  if(!mbr->HasOption("GROUP_OPT_OK"))
    new_flags |= taiWidget::flgNoList;
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    new_flags |= taiWidget::flgAutoApply; // default is to auto-apply!

  if (mbr->type->DerivesFrom(&TA_taGroup_impl)) {
    dat_typ = "subgp";
    return new taiWidgetSubGroupMenu(taiWidgetMenu::buttonmenu, taiMisc::fonSmall, NULL, typ, host_, par, gui_parent_, new_flags);
  }
  else if (is_group) {
    dat_typ = "gp";
    return new taiWidgetGroupElMenu(taiWidgetMenu::buttonmenu, taiMisc::fonSmall, NULL,
                typ, host_, par, gui_parent_, (new_flags | taiWidget::flgNoInGroup));
  }
  else {
    dat_typ = "list";
    return new taiWidgetListElMenu(taiWidgetMenu::buttonmenu, taiMisc::fonSmall, NULL,
                typ, host_, par, gui_parent_, new_flags);
  }
}

void taiMemberOfTokenPtrFromGroup::GetImage_impl(taiWidget* dat, const void* base) {
  bool is_group = false;
  String mb_path;
  String fga = mbr->OptionAfter("FROM_GROUP_");
  if(fga.nonempty()) {
    mb_path = fga;
    is_group = true;
  }
  else {
    mb_path = mbr->OptionAfter("FROM_LIST_");
  }

  MemberDef* from_md = NULL;
  taBase* bs = ((taBase*)base)->FindFromPath(mb_path, from_md);
  if (bs == NULL) {
    taMisc::Error("taiMemberOfTokenPtrFromGroup::GetImage_impl is NULL -- please report");
    return;
  }
  if(from_md->type->InheritsFrom(&TA_taSmartRef))
    bs = ((taSmartRef*)bs)->ptr();
  else if(from_md->type->IsPointer())
    bs = *((taBase**)bs);

  taBase* tok_ptr = NULL; // this is the addr of the token, in the member
  switch (mode) {
  case MD_BASE:
  case MD_SMART_PTR:  // is binary compatible
  {
    tok_ptr = *((taBase**)mbr->GetOff(base));
  } break;
  case MD_SMART_REF: {
    taSmartRef& ref = *((taSmartRef*)(mbr->GetOff(base)));
    tok_ptr = ref.ptr();
  } break;
  }

  if(dat_typ == "subgp") {
    taiWidgetSubGroupMenu* rval = (taiWidgetSubGroupMenu*)dat;
    taGroup_impl* lst = (taGroup_impl*)bs;
    rval->GetImage(lst, (taGroup_impl*)tok_ptr);
  }
  else if(dat_typ == "gp") {
    taiWidgetGroupElMenu* rval = (taiWidgetGroupElMenu*)dat;
    taList_impl* lst = (taList_impl*)bs;
    rval->GetImage(lst, tok_ptr);
  }
  else {
    taiWidgetListElMenu* rval = (taiWidgetListElMenu*)dat;
    taList_impl* lst = (taList_impl*)bs;
    rval->GetImage(lst, tok_ptr);
  }
  GetOrigVal(dat, base);
}

void taiMemberOfTokenPtrFromGroup::GetMbrValue(taiWidget* dat, void* base, bool& first_diff) {
  taBase* tabval = NULL;
  if (dat_typ == "subgp") {
    taiWidgetSubGroupMenu* rval = (taiWidgetSubGroupMenu*)dat;
    tabval = (taBase*)rval->GetValue();
  }
  else if(dat_typ == "gp") {
    taiWidgetGroupElMenu* rval = (taiWidgetGroupElMenu*)dat;
    tabval = (taBase*)rval->GetValue();
  }
  else {
    taiWidgetListElMenu* rval = (taiWidgetListElMenu*)dat;
    tabval = (taBase*)rval->GetValue();
  }

  switch (mode) {
  case MD_BASE:
    if (no_setpointer)
      *((void**)mbr->GetOff(base)) = tabval;
    else
      taBase::SetPointer((taBase**)mbr->GetOff(base), tabval);
    break;
  case MD_SMART_PTR: //WARNING: use of no_setpointer on smartptrs is not defined!
    taBase::SetPointer((taBase**)mbr->GetOff(base), tabval);
    break;
  case MD_SMART_REF: {
    taSmartRef& ref = *((taSmartRef*)(mbr->GetOff(base)));
    ref.set(tabval);
  } break;
  }

  CmpOrigVal(dat, base, first_diff);
}

