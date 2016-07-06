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

#include "taiArgTypeOfProgLibEl.h"
#include <taiWidgetProgLibElChooser>

taTypeDef_Of(ProgLibEl);


int taiArgTypeOfProgLibEl::BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) {
  if (!argt->IsPointer() || !argt->DerivesFrom(TA_ProgLibEl))
    return 0;
  return taiArgTypeOfFromGroup::BidForArgType(aidx,argt,md,td)+1;
}

taiWidget* taiArgTypeOfProgLibEl::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_)
{
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)   return NULL;
  int new_flags = flags_;
  if (GetHasOption("NULL_OK"))
    new_flags |= taiWidget::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    new_flags |= taiWidget::flgEditOk;

  if (GetHasOption("NO_GROUP_OPT"))
    new_flags |= taiWidget::flgNoGroup; //aka flagNoList

  return new taiWidgetProgLibElChooser(typ, host_, par, gui_parent_, new_flags);
}

void taiArgTypeOfProgLibEl::GetImage_impl(taiWidget* dat, const void* base) {
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
  taiWidgetProgLibElChooser* els = (taiWidgetProgLibElChooser*)dat;
  els->GetImage((taList_impl*)lst, *((taBase**)arg_base));
}

void taiArgTypeOfProgLibEl::GetValue_impl(taiWidget* dat, void*) {
  if (arg_base == NULL)
    return;
  taiWidgetProgLibElChooser* els = (taiWidgetProgLibElChooser*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)els->GetValue());
}

