// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiWidgetMethodDefMenu.h"
#include <MethodDef>
#include <taiWidgetMenu>

//////////////////////////////////
//   taiWidgetMethodDefMenu     //
//////////////////////////////////

taiWidgetMethodDefMenu::taiWidgetMethodDefMenu(taiWidgetActions::RepType rt, int ft, MethodDef* md_,
                                   MemberDef* memb_md_, TypeDef* typ_,
                                   IWidgetHost* host_, taiWidget* par,
                                   QWidget* gui_parent_, int flags_)
  : inherited(rt, ft, memb_md_, typ_, host_, par, gui_parent_, flags_)
{
  md = md_;
  sp =  NULL;
}

MethodDef* taiWidgetMethodDefMenu::GetValue() {
  iAction* cur = ta_actions->curSel();
  if (cur != NULL)
    return (MethodDef*)cur->usr_data.toPtr();
  return NULL;
}

void taiWidgetMethodDefMenu::UpdateMenu(const iMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(actn);
}

void taiWidgetMethodDefMenu::GetMenu(const iMenuAction* actn) {
  ta_actions->Reset();
  //always get target, because it could be dynamic
  GetTarget();
  if (!targ_typ) {
    ta_actions->AddItem("TypeSpace Error");
    return;
  }
  MethodDef* mbd;
  MethodSpace* mbs = sp;
  if (!mbs)
    mbs = &targ_typ->methods;
  for (int i = 0; i < mbs->size; ++i){
    mbd = mbs->FastEl(i);
    ta_actions->AddItem(mbd->GetLabel(),taiWidgetMenu::use_default, actn,mbd);
  }
}
