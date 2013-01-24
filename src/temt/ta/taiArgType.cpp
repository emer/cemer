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

#include "taiArgType.h"
#include <taBase>
#include <Variant>
#include <taiBoolType>

#include <css_machine.h>
#include <css_basic_types.h>
#include <css_ta.h>

taiArgType::taiArgType(int aidx, TypeDef* argt, MethodDef* mb, TypeDef* td)
: taiType(td) {
  meth = mb;
  arg_idx = aidx;
  arg_typ = argt;
  err_flag = false;

  arg_base = NULL;
  arg_val = NULL;
  use_it = NULL;
  obj_inst = NULL;              // note: no longer used..
}

taiArgType::taiArgType() {
  meth = NULL;
  arg_idx = -1;
  arg_typ = NULL;
  err_flag = false;

  arg_base = NULL;
  arg_val = NULL;
  use_it = NULL;
  obj_inst = NULL;
}

taiArgType::~taiArgType() {
  if (use_it != NULL) {
    delete use_it;
    use_it = NULL;
  }
  if (obj_inst != NULL) {       // note: no longer used..
//     delete obj_inst;   // this is now done automatically by deref from taBase var pointer!!
    obj_inst = NULL;
  }
}

void taiArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)  return;

  if (arg_val && GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      *arg_val = val;           // use css conversion code!
    }
  }

  if(use_it != NULL)
    use_it->GetImage(dat, arg_base);
  else
    arg_typ->it->GetImage(dat, arg_base);
}

void taiArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL) return;

  if (use_it)
    use_it->GetValue(dat, arg_base);
  else
    arg_typ->it->GetValue(dat, arg_base);
}

taiData* taiArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  if (arg_base == NULL) return NULL;

  taiData* rval;
  if (use_it)
    rval = use_it->GetDataRep(host_, par, gui_parent_, NULL, flags_);
  else
    rval = arg_typ->it->GetDataRep(host_, par, gui_parent_, NULL, flags_);
  return rval;
}

cssEl* taiArgType::GetElFromArg(const char* nm, void*) {
  if(arg_typ->IsNotPtr()) {
    /* type notes:
      explicitly signed/unsigned chars are treated as numbers, whereas char is a char
      current gui stuff can't handle uints well, so we lump them with variants
    */
    if (arg_typ->DerivesFrom(TA_int)  ||
      arg_typ->DerivesFrom(TA_short) || arg_typ->DerivesFrom(TA_unsigned_short) ||
      arg_typ->DerivesFrom(TA_signed_char) || arg_typ->DerivesFrom(TA_unsigned_char)
    ) {
      arg_typ = &TA_int;
      arg_val = new cssInt(0, nm);
      arg_base = (void*)&(((cssInt*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_int64_t) || arg_typ->DerivesFrom(TA_uint64_t)
      || arg_typ->DerivesFrom(TA_unsigned_int)
    ) {
      arg_typ = &TA_Variant;
      arg_val = new cssVariant(Variant(0LL), nm);
      arg_base = (void*)&(((cssVariant*)arg_val)->val);
      use_it = new taiType(arg_typ); // note: only use the vanilla string field gui, not the Variant gui
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_bool)) {
      arg_val = new cssBool(false, nm);
      arg_base = (void*)&(((cssBool*)arg_val)->val);
      use_it = new taiBoolType(arg_typ); // make an it for it...
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_char)) {
      arg_typ = &TA_char;
      arg_val = new cssChar(0, nm);
      arg_base = (void*)&(((cssChar*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_float) || arg_typ->DerivesFrom(TA_double)) {
      arg_typ = &TA_double;
      arg_val = new cssReal(0, nm);
      arg_base = (void*)&(((cssReal*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_taString)) {
      arg_typ = &TA_taString;
      arg_val = new cssString("", nm);
      arg_base = (void*)&(((cssString*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_Variant)) {
      arg_typ = &TA_Variant;
      arg_val = new cssVariant(_nilVariant, nm);
      arg_base = (void*)&(((cssVariant*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_taBase)) {
      arg_typ = arg_typ->GetNonRefType()->GetNonConstType();
      if(arg_typ->GetInstance() == NULL) return NULL;
      arg_val = new cssTA_Base(NULL, 0, arg_typ, nm); // it will create token for us!
      arg_base = (void*)((cssTA_Base*)arg_val)->ptr;
      return arg_val;
    } else if (arg_typ->DerivesFormal(TA_enum)) {
      arg_val = new cssEnum(0, nm);
      arg_base = (void*)&(((cssEnum*)arg_val)->val);
      return arg_val;
    }
    return NULL;
  }
  // ptr > 0 (probably 1)

  if(arg_typ->DerivesFrom(TA_char)) {
    arg_typ = &TA_taString;
    arg_val = new cssString("", nm);
    arg_base = (void*)&(((cssString*)arg_val)->val);
    return arg_val;
  }
  else if(arg_typ->DerivesFrom(TA_taBase)) {
    TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
    arg_val = new cssTA_Base(NULL, 1, npt, nm);
    arg_base = (void*)&(((cssTA*)arg_val)->ptr);
    return arg_val;
  }

  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  arg_val = new cssTA(NULL, 1, npt, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

bool taiArgType::GetHasOption(const String& opt, MethodDef* md, int aidx) {
  if(!md) md = meth;
  if(!md) return false;
  if(aidx < 0) aidx = arg_idx;
  // first search for digit explicitly -- if multiple opts present, this is key
  String digopt = opt + "_" + String(aidx);
  String mb_nm = md->OptionAfter(digopt);
  if(!mb_nm.empty())
    return true;
  // then check for option with another digit before going to generic
  mb_nm = md->OptionAfter(opt + "_");
  if(!mb_nm.empty()) {
    if(isdigit(mb_nm.firstchar())) { // arg position indicator
      int ai = (int)String((char)mb_nm.firstchar());
      if(ai == aidx) return true; // should not happen due to above check
      else return false;
    }
  }
  return md->HasOption(opt);
}

String taiArgType::GetOptionAfter(const String& opt, MethodDef* md, int aidx) {
  if(!md) md = meth;
  if(!md) return _nilString;
  if(aidx < 0) aidx = arg_idx;
  // first search for digit explicitly -- if multiple opts present, this is key
  String digopt = opt + String(aidx) + "_";
  String mb_nm = md->OptionAfter(digopt);
  if(!mb_nm.empty())
    return mb_nm;
  // then check for option with another digit before going to generic
  mb_nm = md->OptionAfter(opt);
  if(!mb_nm.empty()) {
    if(isdigit(mb_nm.firstchar()) && (mb_nm[1] == '_')) { // arg position indicator
      int ai = (int)String((char)mb_nm.firstchar());
      if(ai == aidx) mb_nm = mb_nm.after(1);      // should not happen due to above check
      else mb_nm = _nilString;                    // bail
    }
  }
  return mb_nm;                 // otherwise must be generic
}
