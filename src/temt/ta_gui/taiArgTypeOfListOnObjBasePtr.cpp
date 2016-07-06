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

#include "taiArgTypeOfListOnObjBasePtr.h"
#include <taList_impl>
#include <taiWidgetGroupElChooser>
#include <taiWidgetListElChooser>

#include <css_ta.h>

taTypeDef_Of(taGroup_impl);

int taiArgTypeOfListOnObjBasePtr::BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl) &&
      (argt->IsPointer()) && argt->IsTaBase() && (md->HasOption("ARG_ON_OBJ")))
    return taiArgTypeOfListBasePtr::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfListOnObjBasePtr::GetElFromArg(const char* nm, void* base) {
  taList_impl* lst = (taList_impl*)base;
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  if (lst != NULL)
    arg_val = new cssTA_Base(lst->DefaultEl_(), 1, npt, nm);
  else
    arg_val = new cssTA_Base(NULL, 1, npt, nm);

  arg_base = (void*)&(((cssTA_Base*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfListOnObjBasePtr::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  int new_flags = (flags_ & ~taiWidget::flgNoList); //note: exclude flgNoList
  if (GetHasOption("NULL_OK"))
    new_flags |= taiWidget::flgNullOk;
/*nn  if (GetHasOption("EDIT_OK"))
    new_flags |= taiWidget::flgEditOk; */
  if (typ->InheritsFrom(TA_taGroup_impl))
    return new taiWidgetGroupElChooser(typ, host_, par, gui_parent_,
      (new_flags | taiWidget::flgNoInGroup));
  else
    return new taiWidgetListElChooser(typ, host_, par, gui_parent_, new_flags);
}

void taiArgTypeOfListOnObjBasePtr::GetImage_impl(taiWidget* dat, const void* base) {
  if (arg_base == NULL)
    return;
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      taBase::SetPointer((taBase**)arg_base, val.toBase());
    }
  }
  if (typ->InheritsFrom(TA_taGroup_impl)) {
    taiWidgetGroupElChooser* els = (taiWidgetGroupElChooser*)dat;
    els->GetImage((taGroup_impl*)base, *((taBase**)arg_base));
  } else {
    taiWidgetListElChooser* els = (taiWidgetListElChooser*)dat;
    els->GetImage((taList_impl*)base, *((taBase**)arg_base));
  }
}

void taiArgTypeOfListOnObjBasePtr::GetValue_impl(taiWidget* dat, void*) {
  if (arg_base == NULL)
    return;
  //note: GetValue is not modal
  taiWidgetListElChooser_base* els = (taiWidgetListElChooser_base*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)els->GetValue());
}
