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

#include "gpiFromGpTokenPtrMember.h"
#include <gpiSubGroups>
#include <gpiGroupEls>
#include <gpiListEls>
#include <taiMenu>

#include <taiMisc>

TypeDef_Of(taSmartPtr);
TypeDef_Of(taGroup_impl);

int gpiFromGpTokenPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if(td->InheritsFrom(&TA_taBase) && md->type->IsBasePointerType()
     && md->OptionAfter("FROM_GROUP_").nonempty())
    return taiTokenPtrMember::BidForMember(md,td)+1;
  return 0;
}

taiData* gpiFromGpTokenPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)   return NULL;

  // setting mode now is good for rest of life
  if(mbr->type->DerivesFrom(TA_taBase))
    mode = MD_BASE;
  else if (mbr->type->DerivesFrom(TA_taSmartPtr))
    mode = MD_SMART_PTR;
  else if (mbr->type->DerivesFrom(TA_taSmartRef))
    mode = MD_SMART_REF;

  int new_flags = flags_;
  if(!mbr->HasOption("NO_NULL"))
    new_flags |= taiData::flgNullOk;
  if(!mbr->HasOption("NO_EDIT"))
    new_flags |= taiData::flgEditOk;
  if(!mbr->HasOption("GROUP_OPT_OK"))
    new_flags |= taiData::flgNoList;
  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    new_flags |= taiData::flgAutoApply; // default is to auto-apply!

  if (mbr->type->DerivesFrom(&TA_taGroup_impl))
    return new gpiSubGroups(taiMenu::buttonmenu, taiMisc::fonSmall, NULL, typ, host_, par, gui_parent_, new_flags);
  else if (from_md->type->DerivesFrom(TA_taGroup_impl))
    return new gpiGroupEls(taiMenu::buttonmenu, taiMisc::fonSmall, NULL,
                typ, host_, par, gui_parent_, (new_flags | taiData::flgNoInGroup));
  else
    return new gpiListEls(taiMenu::buttonmenu, taiMisc::fonSmall, NULL,
                typ, host_, par, gui_parent_, new_flags);
}

void gpiFromGpTokenPtrMember::GetImage_impl(taiData* dat, const void* base) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)   return;

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

  if (mbr->type->DerivesFrom(TA_taGroup_impl)) {
    gpiSubGroups* rval = (gpiSubGroups*)dat;
    taGroup_impl* lst = (taGroup_impl*)GetList(from_md, base);
    rval->GetImage(lst, (taGroup_impl*)tok_ptr);
  } else {
    gpiListEls* rval = (gpiListEls*)dat;
    taList_impl* lst = GetList(from_md, base);
    rval->GetImage(lst, tok_ptr);
  }
  GetOrigVal(dat, base);
}

void gpiFromGpTokenPtrMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  taBase* tabval = NULL;
  if (mbr->type->DerivesFrom(&TA_taGroup_impl)) {
    gpiSubGroups* rval = (gpiSubGroups*)dat;
    tabval = (taBase*)rval->GetValue();
  } else {
    gpiListEls* rval = (gpiListEls*)dat;
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

MemberDef* gpiFromGpTokenPtrMember::GetFromMd() {
  String mb_nm = mbr->OptionAfter("FROM_GROUP_");
  MemberDef* from_md = NULL;
  if(mb_nm != "")
    from_md = typ->members.FindName(mb_nm);
  return from_md;
}

taList_impl* gpiFromGpTokenPtrMember::GetList(MemberDef* from_md, const void* base) {
  if (from_md == NULL)
    return NULL;
  if(from_md->type->InheritsFrom(&TA_taSmartRef))
    return (taList_impl*)((taSmartRef*)from_md->GetOff(base))->ptr();
  else if(from_md->type->IsPointer())
    return *((taList_impl**)from_md->GetOff(base));
  else
    return (taList_impl*)from_md->GetOff(base);
}
