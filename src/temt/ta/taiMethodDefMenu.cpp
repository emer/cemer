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

#include "taiMethodDefMenu.h"
#include <MethodDef>
#include <taiMenu>



/*
//////////////////////////////////
//      taiMemberDefMenu        //
//////////////////////////////////

taiMemberDefMenu::taiMemberDefMenu(taiActions::RepType rt, int ft, MemberDef* md_,
  MemberDef* memb_md_, TypeDef* typ_, IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_)
: inherited(rt, ft, memb_md_, typ_, host_, par, gui_parent_, flags_)
{
  md = md_;
}

MemberDef* taiMemberDefMenu::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (cur == NULL)
    return NULL;
  else
    return (MemberDef*)(cur->usr_data.toPtr());
}

void taiMemberDefMenu::GetMenu() {
  ta_actions->Reset();
  //always get target, because it could be dynamic
  GetTarget();

  if (targ_typ == NULL) {
    ta_actions->AddItem("!!!TypeSpace Error!!!");
    return;
  }

  // determine list filter
  taMisc::ShowMembs show = taMisc::NO_HID_DET;

  // if the target type is member-containing type, then optionally filter members by visibility of its host
  if (targ_typ == typ) {
    if ((md != NULL)  && (md->HasOption("TYPESHOW_VISIBLE"))) {
      taiEditDataHost* dlg = taiM->FindEdit(menubase, typ); //NOTE: finds any, in any window or viewer
      if (dlg != NULL) {
        show = dlg->show;
      }
    }
  }

  MemberSpace& mbs = targ_typ->members;
  for (int i = 0; i < mbs.size; ++i) {
    MemberDef* mbd = mbs.FastEl(i);
    if (!mbd->ShowMember(show)) continue;
    ta_actions->AddItem(mbd->GetLabel(), mbd);
  }
}
*/

//////////////////////////////////
//      taiMethodDefMenu        //
//////////////////////////////////

taiMethodDefMenu::taiMethodDefMenu(taiActions::RepType rt, int ft, MethodDef* md_,
                                   MemberDef* memb_md_, TypeDef* typ_,
                                   IDataHost* host_, taiData* par,
                                   QWidget* gui_parent_, int flags_)
  : inherited(rt, ft, memb_md_, typ_, host_, par, gui_parent_, flags_)
{
  md = md_;
  sp =  NULL;
}

MethodDef* taiMethodDefMenu::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (cur != NULL)
    return (MethodDef*)cur->usr_data.toPtr();
  return NULL;
}

void taiMethodDefMenu::UpdateMenu(const taiMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(actn);
}

void taiMethodDefMenu::GetMenu(const taiMenuAction* actn) {
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
    ta_actions->AddItem(mbd->GetLabel(),taiMenu::use_default, actn,mbd);
  }
}
