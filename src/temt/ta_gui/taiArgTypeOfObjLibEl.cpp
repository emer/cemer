// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taiArgTypeOfObjLibEl.h"
#include <taiWidgetObjLibElChooser>

taTypeDef_Of(ObjLibEl);


int taiArgTypeOfObjLibEl::BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) {
  if (!argt->IsPointer() || !argt->DerivesFrom(TA_ObjLibEl))
    return 0;
  return taiArgTypeOfFromGroup::BidForArgType(aidx,argt,md,td)+1;
}

taiWidget* taiArgTypeOfObjLibEl::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_)
{
  int new_flags = flags_;
  if (GetHasOption("NULL_OK"))
    new_flags |= taiWidget::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    new_flags |= taiWidget::flgEditOk;

  if (GetHasOption("NO_GROUP_OPT"))
    new_flags |= taiWidget::flgNoGroup; //aka flagNoList

  return new taiWidgetObjLibElChooser(typ, host_, par, gui_parent_, new_flags);
}

void taiArgTypeOfObjLibEl::GetImage_impl(taiWidget* dat, const void* base) {
  if (arg_base == NULL)  return;
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      taBase::SetPointer((taBase**)arg_base, val.toBase());
    }
  }
  taList_impl* lst = GetList(base);
  taiWidgetObjLibElChooser* els = (taiWidgetObjLibElChooser*)dat;
  els->GetImage((taList_impl*)lst, *((taBase**)arg_base));
}

void taiArgTypeOfObjLibEl::GetValue_impl(taiWidget* dat, void*) {
  if (arg_base == NULL)
    return;
  taiWidgetObjLibElChooser* els = (taiWidgetObjLibElChooser*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)els->GetValue());
}

