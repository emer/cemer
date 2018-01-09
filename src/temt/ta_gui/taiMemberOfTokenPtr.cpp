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

#include "taiMemberOfTokenPtr.h"
#include <MethodDef>
#include <taiWidget>
#include <taiWidgetTokenChooser>
#include <taSmartPtr>

#include <taMisc>

int taiMemberOfTokenPtr::BidForMember(MemberDef* md, TypeDef* td) {
  if(td->IsActualTaBase() && md->type->IsBasePointerType())
    return inherited::BidForMember(md,td) + 1;
  return 0;
}

taiWidget* taiMemberOfTokenPtr::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  // setting mode now is good for rest of life
  if (mbr->type->IsTaBase())
    mode = MD_BASE;
  else if (mbr->type->DerivesFrom(TA_taSmartPtr))
    mode = MD_SMART_PTR;
  else if (mbr->type->DerivesFrom(TA_taSmartRef))
    mode = MD_SMART_REF;

  TypeDef* npt = GetMinType(NULL); // note: this will only be a min type
  if (!mbr->HasOption("NO_NULL"))
    flags_ |= taiWidget::flgNullOk;
  if (mbr->HasOption("PREFER_ITEM")) // default is to prefer null
    flags_ |= taiWidget::flgPreferItem;
  // options that require non-readonly
  if (!(flags_ & taiWidget::flgReadOnly)) {
    if (!mbr->HasOption("NO_EDIT")) //note: #EDIT is the default
      flags_ |= taiWidget::flgEditOk;

    if (flags_ & taiWidget::flgEditOk)
      flags_ |= taiWidget::flgEditDialog;
  }

  flags_ |= taiWidget::flgAutoApply; // default is to auto-apply!

/*TODO: prob can't have disabling for no keep tokens, because sometimes
  we don't know the type
  if (!npt->tokens.keep)
    flags_ |= taiWidget::flgNoTokenDlg; // no dialog */
  taiWidgetTokenChooser* rval = new taiWidgetTokenChooser(npt, host_, par, gui_parent_,
        flags_);
  String filt_nm = mbr->OptionAfter("ITEM_FILTER_");
  if (filt_nm.nonempty()) {
    TypeDef* par_typ = mbr->owner->owner;
    if (par_typ) {
      MethodDef* md = par_typ->methods.FindName(filt_nm);
      if (md && md->is_static)
        rval->item_filter = (item_filter_fun)(md->addr);
    }
  }
  String choos_nm = mbr->OptionAfter("CUST_CHOOSER_");
  if (choos_nm.nonempty()) {
    TypeDef* par_typ = mbr->owner->owner;
    if (par_typ) {
      MethodDef* md = par_typ->methods.FindName(choos_nm);
      if (md && md->is_static)
        rval->cust_chooser = (cust_chooser_fun)(md->addr);
    }
  }
  return rval;
}

TypeDef* taiMemberOfTokenPtr::GetMinType(const void* base) {
  // the min type is at least the type of the member, but can be more derived
  TypeDef* rval = NULL;
  // first, we'll try to get a bare minimum type, from the member type itself
  switch (mode) {
  case MD_BASE: {
    rval = mbr->type->GetNonPtrType();
  } break;
  case MD_SMART_PTR: {
    rval = taSmartPtr::GetBaseType(mbr->type);
  } break;
  case MD_SMART_REF: {
    //note: don't know anything about the type w/o an instance
    if (base) {
      taSmartRef& ref = *((taSmartRef*)(mbr->GetOff(base)));
      rval = ref.GetBaseType();
    } else {
      rval = &TA_taBase;
    }
  } break;
  }

  // now, further restrict according to type directives
  TypeDef* dir_type = NULL;
  // dynamic (member-based) type scoping
  String tmp = mbr->OptionAfter("TYPE_ON_");
  if (tmp.nonempty()) {
    if (base) {
      TypeDef* own_td = typ;
      int net_base_off = 0;
      MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, tmp, false); // no warn
      if (md && (md->type->name == "TypeDef_ptr")) {
        dir_type = *(TypeDef**)(md->GetOff(base, net_base_off));
      }
    }
  }
  else {
    // static type scoping
    tmp = mbr->OptionAfter("TYPE_");
    if (tmp.nonempty()) {
      dir_type = TypeDef::FindGlobalTypeName(tmp);
    }
  }
  if (dir_type && dir_type->InheritsFrom(rval))
    rval = dir_type;

  return rval;
}

taBase* taiMemberOfTokenPtr::GetTokenPtr(const void* base) const {
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
  return tok_ptr;
}

void taiMemberOfTokenPtr::GetImage_impl(taiWidget* dat, const void* base) {
  taBase* tok_ptr = GetTokenPtr(base); // this is the addr of the token, in the member
  TypeDef* targ_typ = GetMinType(base);

  taBase* scope = (taBase*)base;
  TypeDef* scope_type = NULL;
  taBase* scope_on_obj = NULL;
  String scope_on_str = mbr->OptionAfter("SCOPEOBJ_ON_");
  if(base && scope_on_str.nonempty()) {
    TypeDef* own_td = typ;
    int net_base_off = 0;
    MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, scope_on_str, false); // no warn
    if (md && md->type->IsBasePointerType()) {
      if (md->type->IsTaBase() || md->type->DerivesFrom(TA_taSmartPtr)) {
        scope_on_obj = *((taBase**)(md->GetOff(base, net_base_off)));
      }
      else if(md->type->DerivesFrom(TA_taSmartRef)) {
        taSmartRef& ref = *((taSmartRef*)(md->GetOff(base, net_base_off)));
        scope_on_obj = ref.ptr();
      }
    }
  }
  
  scope_on_str = mbr->OptionAfter("SCOPETYPE_ON_");
  if(base && scope_on_str.nonempty()) {
    TypeDef* own_td = typ;
    int net_base_off = 0;
    MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, scope_on_str, false); // no warn
    if(md && md->type->name == "TypeDef_ptr") {
      scope_type = *((TypeDef**)(md->GetOff(base, net_base_off)));
    }
  }
  
  if(mbr->HasOption("NO_SCOPE")) {
    scope = NULL;
    scope_type = NULL;          // really no scope
  }
  else if(!scope_type && mbr->HasOption("PROJ_SCOPE")) {
    scope_type = taMisc::default_scope; // default is project
  }
  else if(!scope_type) {
    String sctyp = mbr->OptionAfter("SCOPE_");
    if(!sctyp.empty()) {
      scope_type = TypeDef::FindGlobalTypeName(sctyp);
    }
  }

  taiWidgetTokenChooser* tpb = (taiWidgetTokenChooser*)dat;

  if(scope_on_obj) {
    tpb->GetImageScopeObj(tok_ptr, targ_typ, scope_on_obj);
  }
  else {
    tpb->GetImageScoped(tok_ptr, targ_typ, scope, scope_type);
  }
  GetOrigVal(dat, base);
}

void taiMemberOfTokenPtr::GetMbrValue_impl(taiWidget* dat, void* base) {
//note: in 3.2 we bailed if not keeping tokens, but that is complicated to test
// and could modally depend on dynamic type directives, so we just always set

  taiWidgetTokenChooser* rval = (taiWidgetTokenChooser*)dat;
  switch (mode) {
  case MD_BASE:
    if (no_setpointer)
      *((void**)mbr->GetOff(base)) = rval->GetValue();
    else
      taBase::SetPointer((taBase**)mbr->GetOff(base), (taBase*)rval->GetValue());
    break;
  case MD_SMART_PTR: //WARNING: use of no_setpointer on smartptrs is not defined!
    taBase::SetPointer((taBase**)mbr->GetOff(base), (taBase*)rval->GetValue());
    break;
  case MD_SMART_REF: {
    taSmartRef& ref = *((taSmartRef*)(mbr->GetOff(base)));
    ref = (taBase*)rval->GetValue();
  } break;
  }
}
