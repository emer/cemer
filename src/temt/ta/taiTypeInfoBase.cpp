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

#include "taiTypeInfoBase.h"
#include <taiMenu>
#include <MemberDef>

#include <taMisc>

taiTypeInfoBase::taiTypeInfoBase(taiActions::RepType rt, int ft,
                                 MemberDef* memb_md_, TypeDef* typ_,
                                 IWidgetHost* host_, taiData* par,
                                 QWidget* gui_parent_, int flags_)
  : taiData(typ_, host_, par, gui_parent_, flags_)
{
  targ_typ = NULL; // gets set later
  memb_md = memb_md_;
  menubase = NULL;
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, NULL, host_, this, gui_parent_);
}

taiTypeInfoBase::~taiTypeInfoBase() {
  if (ta_actions != NULL) {
    delete ta_actions;
    ta_actions = NULL;
  }
}

void taiTypeInfoBase::GetImage(const void* base, bool get_menu, void* cur_sel){
  menubase = (void*)base; // ok
//??  if (!typ)  return;
  if (get_menu)
    GetMenu();
  if ((!cur_sel) || (!(ta_actions->GetImageByData(Variant(cur_sel)))))
    ta_actions->GetImageByIndex(0);
}

QWidget* taiTypeInfoBase::GetRep() {
  if (ta_actions) return ta_actions->GetRep();
  else return NULL;
}

void taiTypeInfoBase::GetTarget() {
  targ_typ = typ; // may get overridden by comment directives
  if (!memb_md)  return;
  // a XxxDef* can have one of these options to specify the
  // target type for its XxxDef menu.
  // 1) a TYPE_xxxx in its comment directives
  // 2) a TYPE_ON_this in comment directives, specifying that the
  //    type should be the same as the parent
  // 3) a TYPE_ON_xxx in comment directives, specifying the name
  //    of the member in the same object which is a TypeDef*
  // 4) Nothing, which defaults to the type of the object the memberdef
  //      is in.

  String mb_nm = memb_md->OptionAfter("TYPE_ON_");
  if (!mb_nm.empty()) {
//    taBase* base = (taBase*)host->cur_base; //TODO: highly unsafe cast -- should provide As_taBase() or such in taiDialog
    if(menubase) {
      if (mb_nm == "this") {
        targ_typ = ((taBase*)menubase)->GetTypeDef();
      }
      else {
        TypeDef* own_td = typ;
        ta_memb_ptr net_mbr_off = 0;      int net_base_off = 0;
        MemberDef* tdmd = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off,
                                                        mb_nm, false); // no warn
        if (tdmd && (tdmd->type->name == "TypeDef_ptr")) {
          targ_typ = *(TypeDef**)(MemberDef::GetOff_static(menubase, net_base_off, net_mbr_off));
        }
      }
    }
    return;
  }

  mb_nm = memb_md->OptionAfter("TYPE_");
  if (!mb_nm.empty()) {
    targ_typ = TypeDef::FindGlobalTypeName(mb_nm);
    return;
  }
}
