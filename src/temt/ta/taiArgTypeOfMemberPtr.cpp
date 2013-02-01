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

#include "taiArgTypeOfMemberPtr.h"
#include <taiWidgetMemberDefPtr>

#include <css_ta.h>

#include <taMisc>


int taiArgTypeOfMemberPtr::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (argt->DerivesFrom(TA_MemberDef) && (argt->IsPointer()))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfMemberPtr::GetElFromArg(const char* nm, void*) {
  arg_val = new cssMemberDef(NULL, 1, &TA_MemberDef, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfMemberPtr::GetDataRep_impl(IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_, int flags_, MemberDef*)
{
  flags_ |= taiWidget::flgNoHelp; // help not avail on modal arg dialogs

  taiWidgetMemberDefPtr* rval = new taiWidgetMemberDefPtr(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiArgTypeOfMemberPtr::GetImage_impl(taiWidget* dat, const void* base) {
  if(arg_base == NULL)
    return;
  taiWidgetMemberDefPtr* rval = (taiWidgetMemberDefPtr*)dat;
  MemberDef* md = (MemberDef*)*((void**)arg_base);
  rval->GetImage(md, typ);
}

void taiArgTypeOfMemberPtr::GetValue_impl(taiWidget* dat, void*) {
  if (arg_base == NULL)
    return;
  taiWidgetMemberDefPtr* rval = (taiWidgetMemberDefPtr*)dat;
  *((MemberDef**)arg_base) = rval->GetValue();
}
