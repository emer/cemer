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

#include "taiTypePtrArgType.h"
#include <taBase>
#include <MemberDef>
#include <taiTypeDefButton>

#include <taMisc>

#include <css_ta.h>

TypeDef_Of(TypeDef_ptr);

int taiTypePtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (argt->DerivesFrom(TA_TypeDef) && (argt->IsPointer()))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiTypePtrArgType::GetElFromArg(const char* nm, void* base) {
  String mb_nm = GetOptionAfter("TYPE_ON_");
  if (mb_nm.nonempty()) {
    TypeDef* tpdf = NULL;
    if (mb_nm == "this") {
      tpdf = typ;
      if (typ->IsTaBase() && (base != NULL))
        tpdf = ((taBase*)base)->GetTypeDef();
    } else {
      TypeDef* own_td = typ;
      ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
      MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                    mb_nm, false); // no warn
      if (md && (md->type == &TA_TypeDef_ptr)) {
        tpdf = *(TypeDef**)(MemberDef::GetOff_static(base, net_base_off, net_mbr_off));
      }
    }
    if(tpdf) {
      base_type = tpdf;
      arg_val = new cssTypeDef(tpdf, 1, &TA_TypeDef, nm);
      arg_base = (void*)&(((cssTA*)arg_val)->ptr);
      return arg_val;
    }
  } else {
    mb_nm = GetOptionAfter("TYPE_");
    if(mb_nm != "") {
      TypeDef* tpdf;
      if (mb_nm == "this") {
        tpdf = typ;
        if (typ->IsTaBase() && (base != NULL))
          tpdf = ((taBase*)base)->GetTypeDef();
      } else {
        tpdf = taMisc::types.FindName(mb_nm);
      }
      if (tpdf == NULL)
        tpdf = &TA_taBase;
      base_type = tpdf;
      arg_val = new cssTypeDef(tpdf, 1, &TA_TypeDef, nm);
      arg_base = (void*)&(((cssTA*)arg_val)->ptr);
      return arg_val;
    }
  }
  base_type = &TA_taBase;;
  arg_val = new cssTypeDef(&TA_taBase, 1, &TA_TypeDef, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiTypePtrArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  int flags = flags_;
  if (GetHasOption("NULL_OK"))
    flags |= taiData::flgNullOk;
  flags |= taiData::flgNoHelp; // help not avail on modal arg dialogs

//   TypeDef* init_typ = &TA_taBase;
//   if (*((TypeDef**)arg_base) != NULL)
//     init_typ = *((TypeDef**)arg_base);
  taiTypeDefButton* rval = new taiTypeDefButton(base_type, host_, par,
                                                gui_parent_, flags);
  return rval;
}

void taiTypePtrArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)
    return;
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  String nulltxt = GetOptionAfter("NULL_TEXT_");
  if(nulltxt.nonempty()) {
    taMisc::SpaceLabel(nulltxt);
    rval->setNullText(nulltxt);
  }
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {// must be a string..
      TypeDef* tdlkup = taMisc::types.FindName(val.toString());
      if(tdlkup && !tdlkup->InheritsFrom(base_type)) {
        base_type = tdlkup;     // reset base type to accommodate current value
      }
      *((TypeDef**)arg_base) = tdlkup;
    }
  }
  TypeDef* typ_ = (TypeDef*)*((void**)arg_base);
  rval->GetImage(typ_, base_type);
}

void taiTypePtrArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  *((void**)arg_base) = rval->GetValue();
}
