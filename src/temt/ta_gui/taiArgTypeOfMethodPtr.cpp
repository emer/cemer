// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "taiArgTypeOfMethodPtr.h"
#include <taiWidgetMethodDefMenu>
#include <taiWidgetActions>
#include <taiWidgetMenu>
#include <taiMisc>

#include <css_ta.h>



int taiArgTypeOfMethodPtr::BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) {
  if(argt->DerivesFrom(TA_MethodDef) && (argt->IsPointer()))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfMethodPtr::GetElFromArg(const char* nm, void*) {
  arg_val = new cssMethodDef(NULL, 1, &TA_MethodDef, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfMethodPtr::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  MethodDef* init_md = typ->methods.FindName("Load");
  if (*((MethodDef**)arg_base) != NULL)
    init_md = *((MethodDef**)arg_base);
  flags_ |= taiWidget::flgNoHelp; // help not avail on modal arg dialogs
  taiWidgetMethodDefMenu* rval = new taiWidgetMethodDefMenu(taiWidgetMenu::buttonmenu, taiMisc::fonSmall,
        init_md, NULL, typ, host_, par, gui_parent_, flags_);
  rval->GetMenu();
  return rval;
}

void taiArgTypeOfMethodPtr::GetImage_impl(taiWidget* dat, const void* base) {
  if(arg_base == NULL)
    return;
  taiWidgetMethodDefMenu* rval = (taiWidgetMethodDefMenu*)dat;
  rval->md = (MethodDef*)*((void**)arg_base);
  TypeDef* eff_td = typ;
  if(base && typ->IsTaBase()) {
    eff_td = ((taBase*)base)->GetTypeDef();
  }
  rval->menubase = eff_td;
  rval->ta_actions->Reset();
  MethodSpace* mbs = &(eff_td->methods);
  for (int i = 0; i < mbs->size; ++i){
    MethodDef* mbd = mbs->FastEl(i);
    if (mbd->im == NULL) continue;
    if ((mbd->name == "Close") || (mbd->name == "DuplicateMe") || (mbd->name == "ChangeMyType")
       || (mbd->name == "AddToControlPanel") || (mbd->name == "AddControlFunForEdit")
       || (mbd->name == "Help"))
      continue;
    rval->ta_actions->AddItem(mbd->GetLabel(), mbd);
  }
  MethodDef* initmd = (MethodDef*)*((void**)arg_base);
  if ((initmd != NULL) && eff_td->InheritsFrom(initmd->GetOwnerType()))
    rval->ta_actions->GetImageByData(Variant((void*)initmd));
  else
    rval->ta_actions->GetImageByIndex(0);       // just get first on list
}

void taiArgTypeOfMethodPtr::GetValue_impl(taiWidget* dat, void*) {
  if(arg_base == NULL)
    return;
  taiWidgetMethodDefMenu* rval = (taiWidgetMethodDefMenu*)dat;
  *((void**)arg_base) = rval->GetValue();
}
