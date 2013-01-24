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

#include "taiTokenPtrArgType.h"
#include <taiTokenPtrButton>
#include <IDataHost>

#include <taMisc>

#include <css_ta.h>

TypeDef_Of(TypeDef_ptr);

int taiTokenPtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (td->InheritsFrom(TA_taBase) &&
     (argt->IsPointer()) && argt->DerivesFrom(TA_taBase))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiTokenPtrArgType::GetElFromArg(const char* nm, void*) {
  // arg_val is for the function
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  arg_val = new cssTA_Base(NULL, 1, npt, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiTokenPtrArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  int token_flags = 0;
  if (GetHasOption("NULL_OK"))
    token_flags |= taiData::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    token_flags |= taiData::flgEditOk;
  token_flags |= taiData::flgNoHelp; // help not avail on modal arg dialogs
  taiTokenPtrButton* rval = new taiTokenPtrButton(npt, host_, par, gui_parent_, token_flags);
  return rval;
//   taiToken* rval = new taiToken(taiMenu::buttonmenu, taiMisc::fonSmall, npt, host_, par, gui_parent_,
//      token_flags);
  return rval;
}

void taiTokenPtrArgType::GetImage_impl(taiData* dat, const void* base){
  if(arg_base == NULL)
    return;
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  String mb_nm = GetOptionAfter("TYPE_ON_");
  if(!mb_nm.empty()) {
    if(mb_nm == "this") {
      npt = ((taBase*)base)->GetTypeDef(); // use object type
    }
    else {
      TypeDef* own_td = typ;
      ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
      MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                    mb_nm, false); // no warn
      if (md && (md->type == &TA_TypeDef_ptr)) {
        TypeDef* mbr_typ = *(TypeDef**)(MemberDef::GetOff_static(base, net_base_off, net_mbr_off));
        if(mbr_typ->InheritsFrom(npt) || npt->InheritsFrom(mbr_typ))
          npt = mbr_typ;                // make sure this applies to this argument..
      }
    }
  }
  else {
    mb_nm = GetOptionAfter("TYPE_");
    if(!mb_nm.empty()) {        // check again..
      TypeDef* tmptd = taMisc::types.FindName(mb_nm);
      if(tmptd) npt = tmptd;
    }
  }
  taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
  //  taiToken* rval = (taiToken*)dat;
  taBase* scope = NULL;
  if((rval->host != NULL) && (rval->host->GetRootTypeDef() != NULL) &&
          (rval->host->GetRootTypeDef()->InheritsFrom(TA_taBase)))
    scope = (rval->host)->Base();
  else
    scope = (taBase*)base;
  TypeDef* scope_type = NULL;
  if(GetHasOption("NO_SCOPE")) {
    scope = NULL;
    scope_type = NULL; // really no scope!
  }
  else if(GetHasOption("PROJ_SCOPE"))
    scope_type = taMisc::default_scope; // default is project
  else {
    String sctyp = GetOptionAfter("SCOPE_");
    if(!sctyp.empty()) {
      scope_type = taMisc::types.FindName(sctyp);
    }
  }
  String nulltxt = GetOptionAfter("NULL_TEXT_");
  if(nulltxt.nonempty()) {
    taMisc::SpaceLabel(nulltxt);
    rval->setNullText(nulltxt);
  }
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      taBase::SetPointer((taBase**)arg_base, val.toBase());
    }
  }
  rval->GetImageScoped(*((taBase**)arg_base), npt, scope, scope_type);
}

void taiTokenPtrArgType::GetValue_impl(taiData* dat, void*) {
  if(arg_base == NULL)
    return;
  taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)rval->GetValue());
}
