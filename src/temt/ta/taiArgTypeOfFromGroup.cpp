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

#include "taiArgTypeOfFromGroup.h"
#include <taiWidgetGroupElChooser>
#include <taiWidgetListElChooser>
#include <taList_impl>

#include <css_ta.h>

TypeDef_Of(taGroup_impl);

int taiArgTypeOfFromGroup::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (!argt->IsPointer() || !argt->IsTaBase())
    return 0;
  String fmgp = GetOptionAfter("FROM_GROUP_", md, aidx);
  if (fmgp.empty()) return 0;
  return taiArgTypeOfTokenPtr::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfFromGroup::GetElFromArg(const char* nm, void* base) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)   return NULL;
  taList_impl* lst = GetList(from_md, base);
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  if (lst != NULL)
    arg_val = new cssTA_Base(lst->DefaultEl_(), 1, npt, nm);
  else
    arg_val = new cssTA_Base(NULL, 1, npt, nm);

  arg_base = (void*)&(((cssTA_Base*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfFromGroup::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)   return NULL;
  int new_flags = flags_;
  if (GetHasOption("NULL_OK"))
    new_flags |= taiWidget::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    new_flags |= taiWidget::flgEditOk;
  new_flags |= taiWidget::flgNoHelp; // help not avail on modal arg dialogs

  if (GetHasOption("NO_GROUP_OPT"))
    new_flags |= taiWidget::flgNoGroup; //aka flagNoList

  if (from_md->type->DerivesFrom(TA_taGroup_impl))
     return new taiWidgetGroupElChooser(typ, host_, par, gui_parent_,
                                  (new_flags | taiWidget::flgNoInGroup));
  else
    return new taiWidgetListElChooser(typ, host_, par, gui_parent_, new_flags);
}

void taiArgTypeOfFromGroup::GetImage_impl(taiWidget* dat, const void* base) {
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
  if (typ->InheritsFrom(TA_taGroup_impl)) {
    taiWidgetGroupElChooser* els = (taiWidgetGroupElChooser*)dat;
    els->GetImage((taGroup_impl*)lst, *((taBase**)arg_base));
  } else {
    taiWidgetListElChooser* els = (taiWidgetListElChooser*)dat;
    els->GetImage((taList_impl*)lst, *((taBase**)arg_base));
  }
}

void taiArgTypeOfFromGroup::GetValue_impl(taiWidget* dat, void*) {
  if (arg_base == NULL)
    return;
  taiWidgetListElChooser_base* els = (taiWidgetListElChooser_base*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)els->GetValue());
}

MemberDef* taiArgTypeOfFromGroup::GetFromMd() {
  MemberDef* from_md = NULL;
  String mb_nm = GetOptionAfter("FROM_GROUP_");
  if (!mb_nm.empty()) {
    from_md = typ->members.FindName(mb_nm);
  }
  return from_md;
}

taList_impl* taiArgTypeOfFromGroup::GetList(MemberDef* from_md, const void* base) {
  if (from_md == NULL)
    return NULL;
  if(from_md->type->InheritsFrom(&TA_taSmartRef))
    return (taList_impl*)((taSmartRef*)from_md->GetOff(base))->ptr();
  else if(from_md->type->IsPointer())
    return *((taList_impl**)from_md->GetOff(base));
  else
    return (taList_impl*)from_md->GetOff(base);
}
