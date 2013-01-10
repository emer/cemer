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

#include "gpiInObjArgType.h"

int gpiInObjArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl) &&
     (argt->ptr == 1) && argt->DerivesFrom(TA_taBase) && (md->HasOption("ARG_ON_OBJ")))
    return gpiTAPtrArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* gpiInObjArgType::GetElFromArg(const char* nm, void* base) {
  taList_impl* lst = (taList_impl*)base;
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  if (lst != NULL)
    arg_val = new cssTA_Base(lst->DefaultEl_(), 1, npt, nm);
  else
    arg_val = new cssTA_Base(NULL, 1, npt, nm);

  arg_base = (void*)&(((cssTA_Base*)arg_val)->ptr);
  return arg_val;
}

taiData* gpiInObjArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  int new_flags = (flags_ & ~taiData::flgNoList); //note: exclude flgNoList
  if (GetHasOption("NULL_OK"))
    new_flags |= taiData::flgNullOk;
/*nn  if (GetHasOption("EDIT_OK"))
    new_flags |= taiData::flgEditOk; */
  if (typ->InheritsFrom(TA_taGroup_impl))
    return new taiGroupElsButton(typ, host_, par, gui_parent_,
      (new_flags | taiData::flgNoInGroup));
  else
    return new taiListElsButton(typ, host_, par, gui_parent_, new_flags);
}

void gpiInObjArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)
    return;
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      taBase::SetPointer((taBase**)arg_base, val.toBase());
    }
  }
  if (typ->InheritsFrom(TA_taGroup_impl)) {
    taiGroupElsButton* els = (taiGroupElsButton*)dat;
    els->GetImage((taGroup_impl*)base, *((taBase**)arg_base));
  } else {
    taiListElsButton* els = (taiListElsButton*)dat;
    els->GetImage((taList_impl*)base, *((taBase**)arg_base));
  }
}

void gpiInObjArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  //note: GetValue is not modal
  taiListElsButtonBase* els = (taiListElsButtonBase*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)els->GetValue());
}
