// Copyright 2015, Regents of the University of Colorado,
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

#include "taiWidgetSubGroupMenu.h"
#include <taGroup_impl>
#include <iDialogObjChooser>
#include <taiWidgetMenu>
#include <MemberDef>
#include <taMisc>


taiWidgetSubGroupMenu::taiWidgetSubGroupMenu(taiWidgetActions::RepType rt, int ft, taGroup_impl* gp, TypeDef* typ_, IWidgetHost* host_, 
  taiWidget* par,	QWidget* gui_parent_, int flags_)
: taiWidgetTaBaseMenu(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ths = gp;
//  ta_actions = new taiWidgetMenu(rt, taiWidgetMenu::radio_update, ft, (void*)ths, typ, host_, this, gui_parent_);
  ta_actions = taiWidgetActions::New(rt, taiWidgetMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  over_max = false;
}

taiWidgetSubGroupMenu::taiWidgetSubGroupMenu(taiWidgetMenu* existing_menu, taGroup_impl* gp, TypeDef* typ_,
	IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_)
: taiWidgetTaBaseMenu(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  ths = gp;
  ownflag = false;
  over_max = false;
}

QWidget* taiWidgetSubGroupMenu::GetRep() {
  return ta_actions->GetRep();
}

void taiWidgetSubGroupMenu::GetImage(taGroup_impl* base_gp, taGroup_impl* gp) {
  if (ths != base_gp) {
    ths = base_gp;
    GetMenu(ta_actions, NULL);
  }
  ta_actions->GetImageByData(Variant((void*)gp));
  setCur_obj(gp, false);
}

taGroup_impl* taiWidgetSubGroupMenu::GetValue() {
  iAction* cur = ta_actions->curSel();
  if (over_max) {
    if((cur == NULL) || (cur->text() == "<Over max, select...>"))
      return (taGroup_impl*)cur_obj;
    else
      return (taGroup_impl*)cur->usr_data.toPtr();
  }
  if (cur != NULL)
    return (taGroup_impl*)cur->usr_data.toPtr();
  return NULL;
}

void taiWidgetSubGroupMenu::Edit() {
  taGroup_impl* cur_base = GetValue();
  if (cur_base != NULL) {
/*TODO obs    if (host != NULL)
      cur_base->Edit(host->modal);
    else */
      cur_base->Edit();
  }
}

void taiWidgetSubGroupMenu::Choose() {
  taList_impl* chs_root = ths;
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select>") &&
//      (ta_actions->cur_sel->usr_data != NULL))
//     chs_root = (taList_impl*)ta_actions->cur_sel->usr_data;

  iDialogObjChooser* chs = iDialogObjChooser::createInstance(chs_root, "SubGroups", true);
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj(chs->sel_obj());
/*TODO: needed???    if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select>") &&
       (ta_actions->cur_sel->men_act != NULL)) {
      ta_actions->cur_sel->men_act->Select(ta_actions->cur_sel); // call it!
    }
    else
      ta_actions->SetMLabel(chs->sel_str); */
  }
  delete chs;
}

void taiWidgetSubGroupMenu::GetMenu(taiWidgetActions* menu, iMenuAction* actn) {
  if (HasFlag(flgNullOk))
    menu->AddItem("NULL", taiWidgetMenu::use_default, actn, (void*)NULL);
  if (HasFlag(flgEditOk))
    menu->AddItem("Edit", taiWidgetMenu::normal,
	iAction::action, this, SLOT(Edit()) );
  if (ths == NULL)	return;
  String nm;
  if(ths->owner == NULL)
    nm = ths->GetTypeDef()->name;
  else if(ths->owner->GetName() == "")
    nm = ths->owner->GetTypeDef()->name;
  else
    nm = ths->owner->GetName();
  menu->AddItem(nm, taiWidgetMenu::use_default, actn, (void*)ths);
  menu->AddSep();
  GetMenu_impl(ths, menu, actn);
}

void taiWidgetSubGroupMenu::UpdateMenu(iMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(actn);
}

void taiWidgetSubGroupMenu::GetMenu_impl(taGroup_impl* gp, taiWidgetActions* menu, iMenuAction* actn) {
  if (gp == NULL) return;
  if (gp->gp.size >= taMisc::max_menu) {
    iAction* mnel = menu->AddItem
      ("<Over max, select...>", taiWidgetMenu::normal,
        iAction::action, this, SLOT(Choose()), gp );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
    return;
  }
  for (int i = 0; i < gp->size; ++i) {
    taBase* tmp = (taBase*)gp->FastEl_(i);
    if (tmp == NULL)	continue;
    String nm = tmp->GetName();
    if (nm.empty())
      nm = String("[") + String(i) + "]: (" + tmp->GetTypeDef()->name + ")";
    bool added_sub = false;	// if a IN_GPMENU sub was added for this item
    taiWidgetMenu* sub_menu = NULL;		// the submenu if added
    if (tmp->HasOption("MEMB_IN_GPMENU") && (tmp->GetOwner() == gp))  {
      TypeDef* ttd = tmp->GetTypeDef();
      for (int fm = 0; fm < ttd->members.size; ++fm) {
	MemberDef* md = ttd->members[fm];
	if (md->HasOption("IN_GPMENU")) {
	  taGroup_impl* tmp_grp = (taGroup_impl*)md->GetOff(tmp);
	  if (tmp_grp->leaves == 0)
	    continue;
	  if (!added_sub) {
//TODO: tmp_grp???	    sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
            sub_menu = menu->AddSubMenu(nm);
	    sub_menu->AddItem(nm, taiWidgetMenu::use_default, actn, tmp);
	  }
	  String subnm = String("::") + md->name;
	  sub_menu->AddItem(subnm, taiWidgetMenu::use_default, actn, tmp_grp);
          sub_menu->AddSep();
	  GetMenu_impl(tmp_grp, sub_menu, actn);
	  added_sub = true;
	}
      }
    }
  }
  for (int i = 0; i < gp->gp.size; ++i) {
    taGroup_impl* tmp_grp = (taGroup_impl*)gp->FastGp_(i);
    String nm;
    if (tmp_grp->GetName() == "")
      nm = "Group [" + String(i) + "]";
    else
      nm = tmp_grp->GetName();
    bool has_sub_stuff = false;
    if (tmp_grp->gp.size > 0)
      has_sub_stuff = true;
    if (!has_sub_stuff) {
      for(int j = 0; j < tmp_grp->size; ++j) {
	taBase* tmp = (taBase*)tmp_grp->FastEl_(j);
	if (tmp == NULL)	continue;
	if (tmp->HasOption("MEMB_IN_GPMENU") && (tmp->GetOwner() == tmp_grp))  {
	  TypeDef* ttd = tmp->GetTypeDef();
	  for (int fm = 0; fm < ttd->members.size; ++fm) {
	    MemberDef* md = ttd->members[fm];
	    if (md->HasOption("IN_GPMENU")) {
	      taGroup_impl* sbgrp = (taGroup_impl*)md->GetOff(tmp);
	      if(sbgrp->leaves == 0)
		continue;
	      has_sub_stuff = true;
	      break;
	    }
	  }
	  if (has_sub_stuff)  break;
	}
      } // for int j
    } // !has_sub_stuff
    if (has_sub_stuff) {
//TODO qt4      taiWidgetMenu* sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
      taiWidgetMenu* sub_menu = menu->AddSubMenu(nm);
      String subnm = nm + ": Group";
      sub_menu->AddItem(subnm, taiWidgetMenu::use_default, actn, tmp_grp);
      sub_menu->AddSep();
      GetMenu_impl(tmp_grp, sub_menu, actn);
    } else
      menu->AddItem(nm, taiWidgetMenu::use_default, actn, tmp_grp);
  }
}

