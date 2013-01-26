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

#include "taiProgLibElArgType.h"
#include <taiProgLibElsButton>

TypeDef_Of(ProgLibEl);


int taiProgLibElArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (!argt->IsPointer() || !argt->DerivesFrom(TA_ProgLibEl))
    return 0;
  return gpiFromGpArgType::BidForArgType(aidx,argt,md,td)+1;
}

taiData* taiProgLibElArgType::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_)
{
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)   return NULL;
  int new_flags = flags_;
  if (GetHasOption("NULL_OK"))
    new_flags |= taiData::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    new_flags |= taiData::flgEditOk;

  if (GetHasOption("NO_GROUP_OPT"))
    new_flags |= taiData::flgNoGroup; //aka flagNoList

  return new taiProgLibElsButton(typ, host_, par, gui_parent_, new_flags);
}

void taiProgLibElArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)  return;
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      taBase::SetPointer((taBase**)arg_base, val.toBase());
    }
  }
  MemberDef* from_md = GetFromMd();
  if (from_md == NULL)  return;
  taList_impl* lst = GetList(from_md, base);
  taiProgLibElsButton* els = (taiProgLibElsButton*)dat;
  els->GetImage((taList_impl*)lst, *((taBase**)arg_base));
}

void taiProgLibElArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  taiProgLibElsButton* els = (taiProgLibElsButton*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)els->GetValue());
}

