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

#include "gpiGroupEls.h"
#include <taGroup_impl>
#include <taiObjChooser>
#include <taiMenu>
#include <MemberDef>

#include <taMisc>




gpiGroupEls::gpiGroupEls(taiActions::RepType rt, int ft, taList_impl* lst, TypeDef* typ_, IWidgetHost* host_, 
  taiData* par, QWidget* gui_parent_, int flags_)
: gpiListEls(rt,ft,lst, typ_, host_,par, gui_parent_, flags_)
{
}

gpiGroupEls::gpiGroupEls(taiMenu* existing_menu, taList_impl* gp, TypeDef* tp, IWidgetHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_) // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk
: gpiListEls(existing_menu, gp, tp, host_,par, gui_parent_, flags_)
{
}

void gpiGroupEls::ChooseGp() {
  taList_impl* chs_root = &(((taGroup_impl*)ths)->gp);
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "gp.<Over max, select...>") &&
//      (ta_actions->cur_sel->usr_data != NULL))
//     chs_root = (taList_impl*)ta_actions->cur_sel->usr_data;

  taiObjChooser* chs = taiObjChooser::createInstance(chs_root, "SubGroups", true);
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj(chs->sel_obj());
/*TODO: needed???    if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "gp.<Over max, select...>") &&
       (ta_actions->cur_sel->men_act != NULL)) {
      ta_actions->cur_sel->usr_data = (void*)chs_obj;
      ta_actions->cur_sel->men_act->Select(ta_actions->cur_sel); // call it!
    }
    else
      ta_actions->SetMLabel(chs->sel_str); */
  }
  delete chs;
}

void gpiGroupEls::GetMenu_impl(taList_impl* cur_lst, taiActions* menu, taiMenuAction* actn) {
  if (cur_lst == NULL) return;
  taGroup_impl* cur_gp = (taGroup_impl*)cur_lst;
  if (cur_gp->size >= taMisc::max_menu) {
    taiAction* mnel = menu->AddItem
      ("<Over max, select...>", taiMenu::normal,
       taiAction::action, this, SLOT(Choose()), cur_gp );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
  } else {
    for (int i = 0; i < cur_gp->size; ++i) {
      taBase* tmp = (taBase*)cur_gp->FastEl_(i);
      if (tmp == NULL) continue;
      String nm = tmp->GetName();
      if (nm.empty())
	nm = String("[") + String(i) + "]: (" + tmp->GetTypeDef()->name + ")";

      bool added_sub = false;	// if a IN_GPMENU sub was added for this item
      if (!HasFlag(flgNoInGroup) && tmp->HasOption("MEMB_IN_GPMENU") &&
	 (tmp->GetOwner() == cur_gp)) // only get for owned items
      {
	TypeDef* ttd = tmp->GetTypeDef();
	for (int fm = 0; fm < ttd->members.size; ++fm) {
	  MemberDef* md = ttd->members[fm];
	  if (md->HasOption("IN_GPMENU")) {
	    taGroup_impl* tmp_grp = (taGroup_impl*)md->GetOff(tmp);
	    if (tmp_grp->leaves == 0)
	      continue;

//TODO Qt4            taiMenu* sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
            taiMenu* sub_menu = menu->AddSubMenu(nm);

            sub_menu->AddItem(nm, taiMenu::use_default, actn, tmp);
	    String subnm = String("::") + md->name;
	    sub_menu->AddItem(subnm, taiMenu::use_default, actn, tmp_grp);
	    sub_menu->AddSep();
	    GetMenu_impl(tmp_grp, sub_menu, actn);
	    added_sub = true;
	  }
	}
      }
      if (!added_sub) {
	menu->AddItem((char*)nm, taiMenu::use_default, actn, tmp);
      }
    }
  }
  if (cur_gp->gp.size >= taMisc::max_menu) {
    taiAction* mnel = menu->AddItem
      ("gp.<Over max, select...>", taiMenu::normal,
        taiAction::action, this, SLOT(ChooseGp()), &(cur_gp->gp) );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
  } else {
    for (int i = 0; i < cur_gp->gp.size; ++i) {
      taGroup_impl* tmp_grp = (taGroup_impl*)cur_gp->FastGp_(i);
      String nm = tmp_grp->GetName();
      if (nm == "")
	nm = "Group [" + String(i) + "]";
//TODO Qt4      taiMenu* sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
      taiMenu* sub_menu = menu->AddSubMenu(nm);
      if (!HasFlag(flgNoList)) {
	String subnm = nm + ": All";
	sub_menu->AddItem(subnm, taiMenu::use_default, actn, tmp_grp);
	sub_menu->AddSep();
      }
      GetMenu_impl(tmp_grp, sub_menu, actn);
    }
  }
}

