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

#include "taiArgTypeOfTokenPtr.h"
#include <taiWidgetTokenChooser>
#include <IWidgetHost>

#include <taMisc>

#include <css_ta.h>


int taiArgTypeOfTokenPtr::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (td->IsActualTaBase() &&
     (argt->IsPointer()) && argt->IsTaBase())
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfTokenPtr::GetElFromArg(const char* nm, void*) {
  // arg_val is for the function
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  arg_val = new cssTA_Base(NULL, 1, npt, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfTokenPtr::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  int token_flags = 0;
  if (GetHasOption("NULL_OK"))
    token_flags |= taiWidget::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    token_flags |= taiWidget::flgEditOk;
  token_flags |= taiWidget::flgNoHelp; // help not avail on modal arg dialogs
  taiWidgetTokenChooser* rval = new taiWidgetTokenChooser(npt, host_, par, gui_parent_, token_flags);
  return rval;
//   taiWidgetTokenPtrMenu* rval = new taiWidgetTokenPtrMenu(taiWidgetMenu::buttonmenu, taiMisc::fonSmall, npt, host_, par, gui_parent_,
//      token_flags);
  return rval;
}

void taiArgTypeOfTokenPtr::GetImage_impl(taiWidget* dat, const void* base){
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
      if (md && (md->type->name == "TypeDef_ptr")) {
        TypeDef* mbr_typ = *(TypeDef**)(MemberDef::GetOff_static(base, net_base_off, net_mbr_off));
        if(mbr_typ->InheritsFrom(npt) || npt->InheritsFrom(mbr_typ))
          npt = mbr_typ;                // make sure this applies to this argument..
      }
    }
  }
  else {
    mb_nm = GetOptionAfter("TYPE_");
    if(!mb_nm.empty()) {        // check again..
      TypeDef* tmptd = TypeDef::FindGlobalTypeName(mb_nm);
      if(tmptd) npt = tmptd;
    }
  }
  taiWidgetTokenChooser* rval = (taiWidgetTokenChooser*)dat;
  //  taiWidgetTokenPtrMenu* rval = (taiWidgetTokenPtrMenu*)dat;
  taBase* scope = NULL;
  if((rval->host != NULL) && (rval->host->GetRootTypeDef() != NULL) &&
          (rval->host->GetRootTypeDef()->IsActualTaBase()))
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
      scope_type = TypeDef::FindGlobalTypeName(sctyp);
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

void taiArgTypeOfTokenPtr::GetValue_impl(taiWidget* dat, void*) {
  if(arg_base == NULL)
    return;
  taiWidgetTokenChooser* rval = (taiWidgetTokenChooser*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)rval->GetValue());
}
