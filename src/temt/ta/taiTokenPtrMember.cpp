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

#include "taiTokenPtrMember.h"
#include <MethodDef>
#include <taiData>
#include <taiTokenPtrButton>
#include <taSmartPtr>

#include <taMisc>

int taiTokenPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if(td->IsActualTaBase() && md->type->IsBasePointerType())
    return inherited::BidForMember(md,td) + 1;
  return 0;
}

taiData* taiTokenPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par,
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
    flags_ |= taiData::flgNullOk;
  // options that require non-readonly
  if (!(flags_ & taiData::flgReadOnly)) {
    if (!mbr->HasOption("NO_EDIT")) //note: #EDIT is the default
      flags_ |= taiData::flgEditOk;

    if (flags_ & taiData::flgEditOk)
      flags_ |= taiData::flgEditDialog;
  }

  if(!mbr->HasOption(TypeItem::opt_NO_APPLY_IMMED))
    flags_ |= taiData::flgAutoApply; // default is to auto-apply!

/*TODO: prob can't have disabling for no keep tokens, because sometimes
  we don't know the type
  if (!npt->tokens.keep)
    flags_ |= taiData::flgNoTokenDlg; // no dialog */
  taiTokenPtrButton* rval = new taiTokenPtrButton(npt, host_, par, gui_parent_,
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

TypeDef* taiTokenPtrMember::GetMinType(const void* base) {
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
      ta_memb_ptr net_mbr_off = 0;
      int net_base_off = 0;
      MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                    tmp, false); // no warn
      if (md && (md->type->name == "TypeDef_ptr")) {
        dir_type = *(TypeDef**)(MemberDef::GetOff_static(base, net_base_off, net_mbr_off));
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

taBase* taiTokenPtrMember::GetTokenPtr(const void* base) const {
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

void taiTokenPtrMember::GetImage_impl(taiData* dat, const void* base) {
  taBase* tok_ptr = GetTokenPtr(base); // this is the addr of the token, in the member
  TypeDef* targ_typ = GetMinType(base);

  taBase* scope = (taBase*)base;
  TypeDef* scope_type = NULL;
  if(mbr->HasOption("NO_SCOPE")) {
    scope = NULL;
    scope_type = NULL;          // really no scope
  }
  else if(mbr->HasOption("PROJ_SCOPE"))
    scope_type = taMisc::default_scope; // default is project
  else {
    String sctyp = mbr->OptionAfter("SCOPE_");
    if(!sctyp.empty()) {
      scope_type = TypeDef::FindGlobalTypeName(sctyp);
    }
  }

  taiTokenPtrButton* tpb = (taiTokenPtrButton*)dat;
  tpb->GetImageScoped(tok_ptr, targ_typ, scope, scope_type);
  GetOrigVal(dat, base);
}

void taiTokenPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
//note: in 3.2 we bailed if not keeping tokens, but that is complicated to test
// and could modally depend on dynamic type directives, so we just always set

  taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
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
