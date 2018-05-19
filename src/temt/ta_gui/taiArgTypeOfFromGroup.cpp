// Copyright 2013-2018, Regents of the University of Colorado,
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

#include <taMisc>

taTypeDef_Of(taGroup_impl);

int taiArgTypeOfFromGroup::BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) {
  if (!argt->IsPointer() || !argt->IsTaBase())
    return 0;
  String fmgp = GetOptionAfter("FROM_GROUP_", md, aidx);
  if (fmgp.empty())
    fmgp = GetOptionAfter("FROM_LIST_", md, aidx);
  if (fmgp.empty()) return 0;
  return taiArgTypeOfTokenPtr::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfFromGroup::GetElFromArg(const char* nm, void* base) {
  taList_impl* lst = GetList(base);
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  if (lst != NULL)
    if (GetHasOption("PREFER_NULL")) {
      arg_val = new cssTA_Base(NULL, 1, npt, nm);
    }
    else {
      arg_val = new cssTA_Base(lst->DefaultEl_(), 1, npt, nm);
    }
  else
    arg_val = new cssTA_Base(NULL, 1, npt, nm);

  arg_base = (void*)&(((cssTA_Base*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfFromGroup::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  int new_flags = flags_;
  if (GetHasOption("NULL_OK"))
    new_flags |= taiWidget::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    new_flags |= taiWidget::flgEditOk;
  new_flags |= taiWidget::flgNoHelp; // help not avail on modal arg dialogs

  if (GetHasOption("NO_GROUP_OPT"))
    new_flags |= taiWidget::flgNoGroup; //aka flagNoList

  bool is_gp = false;
  String fmgp = GetOptionAfter("FROM_GROUP_");
  if(fmgp.nonempty())
    is_gp = true;
  
  if (is_gp)
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
  taList_impl* lst = GetList(base);
  if(!lst) return;

  bool is_gp = false;
  String fmgp = GetOptionAfter("FROM_GROUP_");
  if(fmgp.nonempty())
    is_gp = true;
  
  if (is_gp) {
    if(!lst->InheritsFrom(&TA_taGroup_impl)) {
      taMisc::Warning("taiArgTypeOfFromGroup: member is not a group type -- change from FROM_GROUP_ to FROM_LIST_", fmgp);
    }
    taiWidgetGroupElChooser* els = (taiWidgetGroupElChooser*)dat;
    els->GetImage((taGroup_impl*)lst, *((taBase**)arg_base));
  }
  else {
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

taList_impl* taiArgTypeOfFromGroup::GetList(const void* base) {
  MemberDef* from_md = NULL;
  String mb_nm = GetOptionAfter("FROM_GROUP_");
  if(mb_nm.empty())
    mb_nm = GetOptionAfter("FROM_LIST_");
  if (mb_nm.empty()) {
    taMisc::Warning("taiArgTypeOfFromGroup: no path after FROM_GROUP_ or FROM_LIST_ for method arg -- must fix!  will likely crash");
    return NULL;
  }
  String submb_nm;
  if(mb_nm.contains('.')) {
    submb_nm = mb_nm.after('.');
    mb_nm = mb_nm.before('.');
  }
  from_md = typ->members.FindName(mb_nm);
  if(!from_md) {
    from_md = typ->static_members.FindName(mb_nm);
  }
  if (!from_md) {
    taMisc::Warning("taiArgTypeOfFromGroup: member name not found -- must fix!  will likely crash:", mb_nm);
    return NULL;
  }
  taBase* mb_base;
  if(from_md->type->InheritsFrom(&TA_taSmartRef))
    mb_base = ((taSmartRef*)from_md->GetOff(base))->ptr();
  else if(from_md->type->IsPointer())
    mb_base = *((taBase**)from_md->GetOff(base));
  else
    mb_base = (taBase*)from_md->GetOff(base);
  if(!mb_base) {
    taMisc::Warning("taiArgTypeOfFromGroup: list is null at this member!", mb_nm);
    return NULL;
  }
  if(submb_nm.nonempty()) {
    TypeDef* td = mb_base->GetTypeDef();
    int base_off = 0;
    MemberDef* mbr_def = TypeDef::FindMemberPathStatic(td, base_off, submb_nm, true); // warn
    if(mbr_def) {
      void* address = mbr_def->GetOff(mb_base, base_off);
      mb_base = (taBase*)address;
    }
  }
  if(!mb_base->InheritsFrom(&TA_taList_impl)) {
    taMisc::Warning("taiArgTypeOfFromGroup: resulting object is not a taList_impl",
                    mb_base->GetTypeDef()->name);
    return NULL;
  }
  taList_impl* rval = (taList_impl*)mb_base;
  return rval;
}
