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

#include "taiArgTypeOfMemberPtr.h"
#include <taiWidgetMemberDefChooser>

#include <css_ta.h>

#include <taMisc>


int taiArgTypeOfMemberPtr::BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) {
  if (argt->DerivesFrom(TA_MemberDef) && (argt->IsPointer()))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfMemberPtr::GetElFromArg(const char* nm, void*) {
  arg_val = new cssMemberDef(NULL, 1, &TA_MemberDef, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfMemberPtr::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  flags_ |= taiWidget::flgNoHelp; // help not avail on modal arg dialogs

  taiWidgetMemberDefChooser* rval = new taiWidgetMemberDefChooser(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiArgTypeOfMemberPtr::GetImage_impl(taiWidget* dat, const void* base) {
  if(arg_base == NULL)
    return;
  TypeDef* eff_td = typ;
  if(base && typ->IsTaBase()) {
    eff_td = ((taBase*)base)->GetTypeDef();
  }
  taiWidgetMemberDefChooser* rval = (taiWidgetMemberDefChooser*)dat;
  MemberDef* md = (MemberDef*)*((void**)arg_base);
  rval->GetImage(md, eff_td);
}

void taiArgTypeOfMemberPtr::GetValue_impl(taiWidget* dat, void*) {
  if (arg_base == NULL)
    return;
  taiWidgetMemberDefChooser* rval = (taiWidgetMemberDefChooser*)dat;
  *((MemberDef**)arg_base) = rval->GetValue();
}
