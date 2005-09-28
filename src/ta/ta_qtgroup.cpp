// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ta_qtgroup.cc

#include "ta_qtgroup.h"
#include "ta_css.h"
#include "ta_type.h"
# include "ta_base.h"
// #include "ta_filer.h"
#include "ta_qt.h"
#include "ta_qttype.h"
#include "ta_seledit.h"
// #include "css_qt.h"
// #include "css_qtdialog.h"
// #include "css_basic_types.h"
// #include "ta_css.h"
#include "ta_TA_type.h"
//
#include "icolor.h"
#include "ieditgrid.h"
// #include "ispinbox.h"

#include <qapplication.h>
// #include <qcheckbox.h>
// #include <qcombobox.h>
#include <qframe.h>
#include <qheader.h>
#include <qlabel.h>
 #include <qlayout.h>
// #include <qlineedit.h>
// #include <qmenubar.h>
#include <qmenudata.h>
#include <qpopupmenu.h>
// #include <qpushbutton.h>
#include <qvbox.h>
#include <qscrollview.h> // for gpiGroupDialog
// #include <qstring.h>
#include <qtooltip.h>
#include <qtable.h>
// #include <qwidgetstack.h>


//////////////////////////
// 	Edit Buttons	//
//////////////////////////


gpiListEditButton::gpiListEditButton
(void* base, TypeDef* tp, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiEditButton(base, NULL, tp, host_, par, gui_parent_, flags_)
{
}

void gpiListEditButton::SetLabel() {
  TABLPtr lst = (TABLPtr)cur_base;
  if(lst == NULL) {
    taiEditButton::SetLabel();
    return;
  }
  String nm = " Size: ";
  nm += String(lst->size);
  nm += String(" (") + lst->el_typ->name + ")";
  setRepLabel(nm);
}

gpiGroupEditButton::gpiGroupEditButton
(void* base, TypeDef* tp, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiEditButton(base, NULL, tp, host_, par, gui_parent_, flags_)
{
}

void gpiGroupEditButton::SetLabel() {
  TAGPtr gp = (TAGPtr)cur_base;
  if (gp == NULL) {
    taiEditButton::SetLabel();
    return;
  }
  String nm = " Size: ";
  nm += String(gp->size);
  if(gp->gp.size > 0)
    nm += String(".") + String(gp->gp.size);
  if(gp->leaves != gp->size)
    nm += String(".") + String((int) gp->leaves);
  nm += String(" (") + gp->el_typ->name + ")";
  setRepLabel(nm);
}


gpiSubEditButton::gpiSubEditButton
(void* base, const char* nm, TypeDef* tp, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiEditButton(base, NULL, tp, host_, par, gui_parent_, flags_)
{
  label = nm;
}

void gpiSubEditButton::SetLabel() {
  TAGPtr gp = (TAGPtr)cur_base;
  if(gp == NULL) {
    setRepLabel(label);
    return;
  }
  String nm = label + ", Size: ";
  nm += String(gp->size);
  if(gp->gp.size > 0)
    nm += String(".") + String(gp->gp.size);
  if(gp->leaves != gp->size)
    nm += String(".") + String((int) gp->leaves);
  nm += String(" (") + gp->el_typ->name + ")";
  setRepLabel(nm);
}

//////////////////////////////////
//  	LinkEditButton		//
//////////////////////////////////

gpiLinkEditButton::gpiLinkEditButton
(void* base, TypeDef* tp, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: gpiGroupEditButton(base, tp, host_, par, gui_parent_, flags_)
{
}

void gpiLinkEditButton::GetMethMenus() {
  if(meth_el.size > 0)		// only do this once..
    return;
  String men_nm, lst_men_nm;
  for (int i = 0; i <typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Close"))
      continue;
    String cur_nm = md->OptionAfter("MENU_ON_");
    if(cur_nm != "")
      men_nm = cur_nm;
    // has to be on one of these two menus..
    if ((men_nm != "Object") && (men_nm != "Edit"))
      continue;
    if ((men_nm == "Object") && (md->name != "Edit"))
      continue;
    if((md->name == "DuplicateEl") || (md->name == "Transfer"))
      continue;
    lst_men_nm = men_nm;
    taiMethodData* mth_rep = md->im->GetMethodRep(cur_base, host, this, mgui_parent);
    if(mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    mth_rep->AddToMenu(menu());
  }
}

gpiListLinkEditButton::gpiListLinkEditButton
(void* base, TypeDef* tp, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: gpiListEditButton(base, tp, host_, par, gui_parent_, flags_)
{
}

void gpiListLinkEditButton::GetMethMenus() {
  if (meth_el.size > 0)		// only do this once..
    return;
  String men_nm, lst_men_nm;
  for (int i = 0; i <typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Close"))
      continue;
    String cur_nm = md->OptionAfter("MENU_ON_");
    if (cur_nm != "")
      men_nm = cur_nm;
    // has to be on one of these two menus..
    if ((men_nm != "Object") && (men_nm != "Edit"))
      continue;
    if ((men_nm == "Object") && (md->name != "Edit"))
      continue;
    if ((md->name == "DuplicateEl") || (md->name == "Transfer"))
      continue;
    lst_men_nm = men_nm;
//    taiMethodData* mth_rep = md->im->GetMethodRep(typ, cur_base, dialog, mgui_parent);
    taiMethodData* mth_rep = md->im->GetMethodRep(cur_base, host, this, mgui_parent);
    if(mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    mth_rep->AddToMenu(menu());
  }
}


//////////////////////////////////
//  	ArrayEditButton		//
//////////////////////////////////


gpiArrayEditButton::gpiArrayEditButton
(void* base, TypeDef* tp, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiEditButton(base, NULL, tp, host_, par, gui_parent_, flags_)
{
}

void gpiArrayEditButton::SetLabel() {
  taArray_base* gp = (taArray_base*)cur_base;
  if(gp == NULL) {
    taiEditButton::SetLabel();
    return;
  }
  String nm = " Size: " + String(gp->size);
  TypeDef* td = gp->GetTypeDef()->GetTemplInstType();
  if ((td != NULL) && (td->templ_pars.size > 0))
    nm += String(" (") + td->templ_pars.FastEl(0)->name + ")";
  setRepLabel(nm);
}

//////////////////////////
//     Element Menus 	//
//////////////////////////

//////////////////////////
// 	gpiListEls	//
//////////////////////////


gpiListEls::gpiListEls(int rt, int ft, TABLPtr lst, TypeDef* typ_, taiDataHost* host_,
	taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ths = lst;
//  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, typ, (void*)ths, host_, this, gui_parent_);
  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  over_max = false;
}

gpiListEls::gpiListEls(taiMenu* existing_menu, TABLPtr lst, TypeDef* typ_,
	taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(existing_menu, typ_, host_, par, gui_parent_, flags_) {
  ths = lst;
  over_max = false;
}

QWidget* gpiListEls::GetRep() {
  return ta_menu->GetRep();
}

void gpiListEls::GetImage(TABLPtr base_lst, TAPtr it) {
  if (ths != base_lst) {
    ths = base_lst;
  }
  UpdateMenu();
  ta_menu->GetImage((void*)it);
  setCur_obj(it, false);
}

TAPtr gpiListEls::GetValue() {
  taiMenuEl* cur = ta_menu->GetValue();
  if (over_max) {
    if ((cur == NULL) || (cur->label == "<Over max, select...>")
       || (cur->label == "gp.<Over max, select...>"))
      return cur_obj;
    else
      return (TAPtr)cur->usr_data;
  }
  if(cur != NULL)
    return (TAPtr)cur->usr_data;
  return NULL;
}

void gpiListEls::Edit() {
  TAPtr cur_base = GetValue();
  if (cur_base != NULL) {
    cur_base->Edit();
/*obs    if (host != NULL)
      cur_base->Edit(host->modal);
    else
      cur_base->Edit(); */
  }
}

void gpiListEls::Choose() {
  TABLPtr chs_root = ths;
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_menu->cur_sel != NULL) && (ta_menu->cur_sel->label == "<Over max, select>") &&
//      (ta_menu->cur_sel->usr_data != NULL))
//     chs_root = (TABLPtr)ta_menu->cur_sel->usr_data;

  taiObjChooser* chs = taiObjChooser::createInstance(chs_root, "List/Group Objects", true);
  //if ((chs_obj != NULL) && !chs_obj->GetName().empty())
    //obs chs->sel_str = chs_obj->GetName();
    chs->setSel_obj(cur_obj);
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj(chs->sel_obj());
/*TODO: even needed???    if((ta_menu->curSel() != NULL) && (ta_menu->curSel()->label == "<Over max, select...>") &&
       (ta_menu->curSel()->men_act != NULL)) {
      ta_menu->curSel()->usr_data = (void*)chs_obj;
      ta_menu->curSel()->men_act->Select(ta_menu->curSel()); // call it!
    }
    else */
   //   ta_menu->setLabel(chs->sel_str());
  }
  delete chs;
}

void gpiListEls::GetMenu(taiMenu* menu, taiMenuAction* actn) {
  if (HasFlag(flgNullOk))
    menu->AddItem("NULL", NULL, taiMenu::radio_update, actn);
  if (HasFlag(flgEditOk))
    menu->AddItem("Edit", NULL, taiMenu::normal,
	taiMenuEl::action, this, SLOT(Edit()));
  if (!HasFlag(flgNoList)) {
    String nm;
    if (ths->GetName() == "")
      nm = "All";
    else
      nm = ths->GetName();
    menu->AddItem(nm, (void*)ths, taiMenu::use_default, actn);
  }
  menu->AddSep(); //note: doesn't double add or add at beginning
  GetMenu_impl(ths, menu, actn);
}

void gpiListEls::UpdateMenu(taiMenuAction* actn) {
  ta_menu->ResetMenu();
  GetMenu(ta_menu, actn);
}

void gpiListEls::GetMenu_impl(TABLPtr cur_lst, taiMenu* menu, taiMenuAction* actn) {
  if (cur_lst == NULL)	return;
  if (cur_lst->size >= taMisc::max_menu) {
    taiMenuEl* mnel = menu->AddItem
      ("<Over max, select...>", cur_lst, taiMenu::normal,
	taiMenuEl::action, this, SLOT(Choose()) );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
    return;
  }
  for (int i = 0; i < cur_lst->size; ++i) {
    TAPtr tmp = (TAPtr)cur_lst->FastEl_(i);
    if (tmp == NULL) continue;
    String nm = tmp->GetName();
    if (nm == "")
      nm = String("[") + String(i) + "]: (" + tmp->GetTypeDef()->name + ")";
    menu->AddItem((char*)nm, (void*)tmp, taiMenu::radio_update, actn);
  }
}


//////////////////////////
// 	gpiGroupEls	//
//////////////////////////

gpiGroupEls::gpiGroupEls
(int rt, int ft, TABLPtr lst, TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: gpiListEls(rt,ft,lst, typ_, host_,par, gui_parent_, flags_)
{
}

gpiGroupEls::gpiGroupEls(taiMenu* existing_menu, TABLPtr gp, TypeDef* tp, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_) // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk
: gpiListEls(existing_menu, gp, tp, host_,par, gui_parent_, flags_)
{
}

void gpiGroupEls::ChooseGp() {
  TABLPtr chs_root = &(((TAGPtr)ths)->gp);
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_menu->cur_sel != NULL) && (ta_menu->cur_sel->label == "gp.<Over max, select...>") &&
//      (ta_menu->cur_sel->usr_data != NULL))
//     chs_root = (TABLPtr)ta_menu->cur_sel->usr_data;

  taiObjChooser* chs = taiObjChooser::createInstance(chs_root, "SubGroups", true);
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj(chs->sel_obj());
/*TODO: needed???    if((ta_menu->cur_sel != NULL) && (ta_menu->cur_sel->label == "gp.<Over max, select...>") &&
       (ta_menu->cur_sel->men_act != NULL)) {
      ta_menu->cur_sel->usr_data = (void*)chs_obj;
      ta_menu->cur_sel->men_act->Select(ta_menu->cur_sel); // call it!
    }
    else
      ta_menu->SetMLabel(chs->sel_str); */
  }
  delete chs;
}

void gpiGroupEls::GetMenu_impl(TABLPtr cur_lst, taiMenu* menu, taiMenuAction* actn) {
  if (cur_lst == NULL) return;
  TAGPtr cur_gp = (TAGPtr)cur_lst;
  if (cur_gp->size >= taMisc::max_menu) {
    taiMenuEl* mnel = menu->AddItem
      ("<Over max, select...>", cur_gp, taiMenu::normal,
       taiMenuEl::action, this, SLOT(Choose()) );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
  } else {
    for (int i = 0; i < cur_gp->size; ++i) {
      TAPtr tmp = (TAPtr)cur_gp->FastEl_(i);
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
	    TAGPtr tmp_grp = (TAGPtr)md->GetOff(tmp);
	    if (tmp_grp->leaves == 0)
	      continue;

            taiMenu* sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);

            sub_menu->AddItem(nm, (void*)tmp, taiMenu::use_default, actn);
	    String subnm = String("::") + md->name;
	    sub_menu->AddItem(subnm, (void*)tmp_grp, taiMenu::use_default, actn);
	    sub_menu->AddSep();
	    GetMenu_impl(tmp_grp, sub_menu, actn);
	    added_sub = true;
	  }
	}
      }
      if (!added_sub) {
	menu->AddItem((char*)nm, (void*)tmp, taiMenu::use_default, actn);
      }
    }
  }
  if (cur_gp->gp.size >= taMisc::max_menu) {
    taiMenuEl* mnel = menu->AddItem
      ("gp.<Over max, select...>", &(cur_gp->gp), taiMenu::normal,
        taiMenuEl::action, this, SLOT(ChooseGp()) );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
  } else {
    for (int i = 0; i < cur_gp->gp.size; ++i) {
      TAGPtr tmp_grp = (TAGPtr)cur_gp->FastGp_(i);
      String nm = tmp_grp->GetName();
      if (nm == "")
	nm = "Group [" + String(i) + "]";
      taiMenu* sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
      if (!HasFlag(flgNoList)) {
	String subnm = nm + ": All";
	sub_menu->AddItem(subnm, (void*)tmp_grp, taiMenu::use_default, actn);
	sub_menu->AddSep();
      }
      GetMenu_impl(tmp_grp, sub_menu, actn);
    }
  }
}


//////////////////////////
// 	gpiSubGroups	//
//////////////////////////


gpiSubGroups::gpiSubGroups(int rt, int ft, TAGPtr gp, TypeDef* typ_, taiDataHost* host_, taiData* par,
	QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ths = gp;
//  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, (void*)ths, typ, host_, this, gui_parent_);
  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  over_max = false;
}

gpiSubGroups::gpiSubGroups(taiMenu* existing_menu, TAGPtr gp, TypeDef* typ_,
	taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  ths = gp;
  ownflag = false;
  over_max = false;
}

QWidget* gpiSubGroups::GetRep() {
  return ta_menu->GetRep();
}

void gpiSubGroups::GetImage(TAGPtr base_gp, TAGPtr gp) {
  if (ths != base_gp) {
    ths = base_gp;
    GetMenu(ta_menu, NULL);
  }
  ta_menu->GetImage((void*)gp);
  setCur_obj(gp, false);
}

TAGPtr gpiSubGroups::GetValue() {
  taiMenuEl* cur = ta_menu->GetValue();
  if (over_max) {
    if((cur == NULL) || (cur->label == "<Over max, select...>"))
      return (TAGPtr)cur_obj;
    else
      return (TAGPtr)cur->usr_data;
  }
  if (cur != NULL)
    return (TAGPtr)cur->usr_data;
  return NULL;
}

void gpiSubGroups::Edit() {
  TAGPtr cur_base = GetValue();
  if (cur_base != NULL) {
/*TODO obs    if (host != NULL)
      cur_base->Edit(host->modal);
    else */
      cur_base->Edit();
  }
}

void gpiSubGroups::Choose() {
  TABLPtr chs_root = ths;
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_menu->cur_sel != NULL) && (ta_menu->cur_sel->label == "<Over max, select>") &&
//      (ta_menu->cur_sel->usr_data != NULL))
//     chs_root = (TABLPtr)ta_menu->cur_sel->usr_data;

  taiObjChooser* chs = taiObjChooser::createInstance(chs_root, "SubGroups", true);
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj(chs->sel_obj());
/*TODO: needed???    if((ta_menu->cur_sel != NULL) && (ta_menu->cur_sel->label == "<Over max, select>") &&
       (ta_menu->cur_sel->men_act != NULL)) {
      ta_menu->cur_sel->men_act->Select(ta_menu->cur_sel); // call it!
    }
    else
      ta_menu->SetMLabel(chs->sel_str); */
  }
  delete chs;
}

void gpiSubGroups::GetMenu(taiMenu* menu, taiMenuAction* actn) {
  if (HasFlag(flgNullOk))
    menu->AddItem("NULL", NULL, taiMenu::use_default, actn);
  if (HasFlag(flgEditOk))
    menu->AddItem("Edit", NULL, taiMenu::normal,
	taiMenuEl::action, this, SLOT(Edit()) );
  if (ths == NULL)	return;
  String nm;
  if(ths->owner == NULL)
    nm = ths->GetTypeDef()->name;
  else if(ths->owner->GetName() == "")
    nm = ths->owner->GetTypeDef()->name;
  else
    nm = ths->owner->GetName();
  menu->AddItem(nm, (void*)ths, taiMenu::use_default, actn);
  menu->AddSep();
  GetMenu_impl(ths, menu, actn);
}

void gpiSubGroups::UpdateMenu(taiMenuAction* actn) {
  ta_menu->ResetMenu();
  GetMenu(actn);
}

void gpiSubGroups::GetMenu_impl(TAGPtr gp, taiMenu* menu, taiMenuAction* actn) {
  if (gp == NULL) return;
  if (gp->gp.size >= taMisc::max_menu) {
    taiMenuEl* mnel = menu->AddItem
      ("<Over max, select...>", gp, taiMenu::normal,
        taiMenuEl::action, this, SLOT(Choose()) );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
    return;
  }
  for (int i = 0; i < gp->size; ++i) {
    TAPtr tmp = (TAPtr)gp->FastEl_(i);
    if (tmp == NULL)	continue;
    String nm = tmp->GetName();
    if (nm.empty())
      nm = String("[") + String(i) + "]: (" + tmp->GetTypeDef()->name + ")";
    bool added_sub = false;	// if a IN_GPMENU sub was added for this item
    taiMenu* sub_menu = NULL;		// the submenu if added
    if (tmp->HasOption("MEMB_IN_GPMENU") && (tmp->GetOwner() == gp))  {
      TypeDef* ttd = tmp->GetTypeDef();
      for (int fm = 0; fm < ttd->members.size; ++fm) {
	MemberDef* md = ttd->members[fm];
	if (md->HasOption("IN_GPMENU")) {
	  TAGPtr tmp_grp = (TAGPtr)md->GetOff(tmp);
	  if (tmp_grp->leaves == 0)
	    continue;
	  if (!added_sub) {
	    sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
	    sub_menu->AddItem(nm, (void*)tmp, taiMenu::use_default, actn);
	  }
	  String subnm = String("::") + md->name;
	  sub_menu->AddItem(subnm, (void*)tmp_grp, taiMenu::use_default, actn);
          sub_menu->AddSep();
	  GetMenu_impl(tmp_grp, sub_menu, actn);
	  added_sub = true;
	}
      }
    }
  }
  for (int i = 0; i < gp->gp.size; ++i) {
    TAGPtr tmp_grp = (TAGPtr)gp->FastGp_(i);
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
	TAPtr tmp = (TAPtr)tmp_grp->FastEl_(j);
	if (tmp == NULL)	continue;
	if (tmp->HasOption("MEMB_IN_GPMENU") && (tmp->GetOwner() == tmp_grp))  {
	  TypeDef* ttd = tmp->GetTypeDef();
	  for (int fm = 0; fm < ttd->members.size; ++fm) {
	    MemberDef* md = ttd->members[fm];
	    if (md->HasOption("IN_GPMENU")) {
	      TAGPtr sbgrp = (TAGPtr)md->GetOff(tmp);
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
      taiMenu* sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
      String subnm = nm + ": Group";
      sub_menu->AddItem(subnm, (void*)tmp_grp, taiMenu::use_default, actn);
      sub_menu->AddSep();
      GetMenu_impl(tmp_grp, sub_menu, actn);
    } else
      menu->AddItem((char*)nm, (void*)tmp_grp, taiMenu::use_default, actn);
  }
}


//////////////////////////
// 	gpiElTypes	//
//////////////////////////

gpiElTypes::gpiElTypes
(int rt, int ft, TypeDef* lstd, TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiTypeHier(rt, ft, typ_, host_, par, gui_parent_, flags_)
{
  lst_typd = lstd;
}

gpiElTypes::gpiElTypes
(taiMenu* existing_menu, TypeDef* gtd, TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiTypeHier(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  lst_typd = gtd;
}

void gpiElTypes::GetMenu(taiMenu* menu, taiMenuAction* nact) {
  GetMenu_impl(menu, typ, nact);
  menu->AddSep(); //note: won't add a spurious separator if not needed
  GetMenu_impl(menu, lst_typd, nact);	// get group types for this type
}


//////////////////////////
//    gpiNewFuns	//
//////////////////////////

gpiNewFuns* gpiNewFuns::CondNew(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
{
  for (int i = 0; i < typ_->methods.size; ++i){
    MethodDef* md = typ_->methods.FastEl(i);
    if (md->HasOption("NEW_FUN")) {
      return new gpiNewFuns(typ_, host_, par, gui_parent_, flags_);
    }
  }
  return NULL;
}

gpiNewFuns::gpiNewFuns(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(new QVBox(gui_parent_));

  for (int i = 0;i < typ->methods.size; ++i){
    MethodDef* md = typ->methods.FastEl(i);
    if (md->HasOption("NEW_FUN")) {
      taiMethToggle* mt = new taiMethToggle(NULL, md, typ, host_, this, m_rep);
      funs.Add(mt);
    }
  }
}

void gpiNewFuns::CallFuns(void* base) {
  for (int i = 0; i < funs.size; ++i) {
    taiMethToggle* mt = (taiMethToggle*)funs.FastEl(i);
    mt->base = base;
    mt->CallFun();
  }
}


//////////////////////////////////
// 	gpiListNew		//
//////////////////////////////////

TAPtr gpiListNew::New(TABLPtr the_lst, int n_els, TypeDef* td, QObject* parent)
{
  if (parent == NULL) parent = qApp->mainWidget();
  gpiListNew* create = new gpiListNew(the_lst, n_els, td, false, true, parent); //read_only, modal
  create->Constr();
  bool ok_can = create->Edit(true);
  if (!ok_can || (create->num <= 0)) {
    delete create;
    return NULL;
  }

  TAPtr rval = NULL;
  DMEM_GUI_RUN_IF {
    rval = the_lst->New(create->num, create->typ);
  }
  if (taMisc::record_script != NULL) {
    *taMisc::record_script << the_lst->GetPath() << "->New(" << create->num
			     << ", " << create->typ->name << ");" << endl;
  }
  DMEM_GUI_RUN_IF {
    if (create->fun_list) {
      for (int i = (int) (the_lst->size - create->num); i < the_lst->size; i++) {
        create->fun_list->CallFuns(the_lst->FastEl_(i));
      }
    }
    bool auto_edit = taMisc::auto_edit;
    if ((Qt::MouseButtonMask & create->mouse_button) == Qt::MidButton) // reverse of default
      auto_edit = !auto_edit;
    if((Qt::MouseButtonMask & create->mouse_button) == Qt::RightButton) // always edit on rt
      auto_edit = true;
    if(auto_edit) {
      if (create->num == 1) {	// edit-after-create
	rval->Edit();
      } else {			// edit the group
	the_lst->Edit();
      }
    }
  }
  delete create;
  return rval;
}

gpiListNew::gpiListNew(TABLPtr lst, int n_els, TypeDef* td, bool read_only_, bool modal_, QObject* parent)
: taiDataHost(td, read_only_, modal_, parent)
{
  if (n_els == 0) n_els = 1;
  ths = lst;
  num = n_els;
  if (td == NULL)
    typ = ths->el_typ;
  else
    typ = td;
  num_rep = NULL;
  typ_rep = NULL;
  fun_list  = NULL;
}

gpiListNew::~gpiListNew() {
  delete num_rep;
  delete typ_rep;
  if (fun_list) delete fun_list;
}

void gpiListNew::Constr_Body() {
  taiDataHost::Constr_Body();
  num_rep = new taiIncrField(NULL, this, NULL, body);//, taiData::flgPosOnly);
  typ_rep = new gpiElTypes(taiMenu::buttonmenu, taiMisc::fonSmall,
			       ths->GetTypeDef(), ths->el_base, this, NULL, body);
  typ_rep->GetMenu();
//  typ_rep->GetImage(typ);

  fun_list = gpiNewFuns::CondNew(ths->el_base, this, NULL, body); // only creates if there are funcs

  // lay out controls
  QWidget* trep;
  int row;
  trep = num_rep->GetRep();
  row = AddData(0, trep);
  AddName(0, "number", "the number of new items to make", trep);

  trep = typ_rep->GetRep();
  row = AddData(1, trep);
  AddName(1, "of type", "the Type of new items to make", trep);

  Constr_SubGpList(); // hook for groups

  // if any callable functions, put them in a box for the user to select from
  if ((fun_list != NULL) && (fun_list->funs.size > 0)) {
    trep = fun_list->GetRep();
    row = AddData(-1, trep);
    AddName(row, "call funs", "optional functions to call on each instance", trep);
  }
}

void gpiListNew::ClearBody_impl() {
  if (fun_list) {delete fun_list; fun_list = NULL;}
  if (typ_rep) {delete typ_rep; typ_rep = NULL;}
  num_rep = NULL;
  taiDataHost::ClearBody_impl();
}

void gpiListNew::Constr_Strings(const char*, const char* win_title) {
  taiDataHost::Constr_Strings("New: Create new object(s)", win_title);
}

void gpiListNew::Constr_Final() {
  taiDataHost::Constr_Final();
  GetImage();
}

void gpiListNew::GetImage() {
  num_rep->GetImage(num);
  typ_rep->GetImage(typ);
//no!  taiDialog::GetImage();
}

void gpiListNew::GetValue() {
  num = num_rep->GetValue();
  typ = typ_rep->GetValue();
}


//////////////////////////////////
// 	taiGroupNew		//
//////////////////////////////////

TAPtr gpiGroupNew::New(TAGPtr the_gp, TAGPtr init_gp, int n_els,  TypeDef* td,
    QObject* parent)
{
  if (parent == NULL) parent = qApp->mainWidget();
  gpiGroupNew* create = new gpiGroupNew(the_gp, init_gp, n_els, td, false, true, parent);
  create->Constr();
  bool ok_can = create->Edit(true);
  if (!ok_can || (create->num <= 0)) {
    delete create;
    return NULL;
  }

  TAPtr rval;
  DMEM_GUI_RUN_IF {
    rval = create->in_gp->New(create->num, create->typ);
  }
  if(taMisc::record_script != NULL) {
    *taMisc::record_script << create->in_gp->GetPath() << "->New(" << create->num
			     << ", " << create->typ->name << ");" << endl;
  }
  DMEM_GUI_RUN_IF {
    if((!create->typ->InheritsFrom(TA_taGroup_impl)) && (create->fun_list != NULL)) {
      int i;
      for(i = (int) (create->in_gp->size - create->num); i < create->in_gp->size; i++){
	create->fun_list->CallFuns(create->in_gp->FastEl_(i));
      }
    }
    bool auto_edit = taMisc::auto_edit;
    if ((Qt::MouseButtonMask & create->mouse_button) == Qt::MidButton) // reverse of default
      auto_edit = !auto_edit;
    if((Qt::MouseButtonMask & create->mouse_button) == Qt::RightButton) // always edit on rt
      auto_edit = true;
    if(auto_edit) {
      if(create->num == 1) {	// edit-after-create
	if(!create->typ->InheritsFrom(TA_taGroup_impl))
	  rval->Edit();
      }
      else {			// edit the group
	create->in_gp->Edit();
      }
    }
  }
  delete create;
  return rval;
}

gpiGroupNew::gpiGroupNew(TAGPtr gp, TAGPtr init_gp, int n_els, TypeDef* td,
    bool read_only_, bool modal_, QObject* parent)
: gpiListNew(gp, n_els, td, read_only_, modal_, parent)
{
  in_gp = gp;
  if (init_gp != NULL)
    in_gp = init_gp;
  subgp_list = NULL;
}

gpiGroupNew::~gpiGroupNew() {
  if (subgp_list != NULL)
    delete subgp_list;
}

void gpiGroupNew::Constr_SubGpList() {
  // could have some members with groups to create things in..
  if ((in_gp->gp.size == 0) && !(in_gp->el_typ->HasOption("MEMB_IN_GPMENU") &&
				(in_gp->leaves > 0)))
    return;

  subgp_list = new gpiSubGroups(taiMenu::buttonmenu, taiMisc::fonSmall, (TAGPtr)ths,
	typ, this, NULL, body); //NOTE: need to confirm 'typ' is proper variable to pass as TypeDef
  subgp_list->GetMenu();

  QWidget* trep;
  trep = subgp_list->GetRep();
  int row = AddData(-1, trep);
  AddName(row, "in", "the subgroup in which to add items", trep);
}

void gpiGroupNew::ClearBody_impl() {
  if (subgp_list) {delete subgp_list; subgp_list = NULL;}
  gpiListNew::ClearBody_impl();
}

void gpiGroupNew::GetImage() {
  if (subgp_list != NULL)
    subgp_list->GetImage((TAGPtr)ths, in_gp);
  gpiListNew::GetImage();
}

void gpiGroupNew::GetValue() {
  if (subgp_list != NULL)
    in_gp = subgp_list->GetValue();
  gpiListNew::GetValue();
}


//////////////////////////////////
// 	gpiListEdit		//
//////////////////////////////////


gpiList_ElData::gpiList_ElData(TypeDef* tp, TAPtr base) {
  typ = tp; cur_base = base;
}

gpiList_ElData::~gpiList_ElData() {
  data_el.Reset();
}


//////////////////////////////////////////////////////////
// 		gpiMultiEditDataHost			//
//////////////////////////////////////////////////////////

gpiMultiEditDataHost::gpiMultiEditDataHost(void* base, TypeDef* typ_, bool read_only_,
      bool modal_, QObject* parent)
: taiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  multi = NULL;
  scrMulti = NULL;
  multi_body = NULL;
  lay_multi = NULL;
  multi_rows = 1;
  multi_cols = 1;
}

void gpiMultiEditDataHost::SetMultiSize(int rows, int cols) {
  if (rows < multi_rows) rows = multi_rows;
  if (cols < multi_cols) cols = multi_cols;
  if ((rows == multi_rows) && (cols == multi_cols)) return;
  multi_body->setDimensions(rows, cols);
  multi_rows = rows;
  multi_cols = cols;
}

void gpiMultiEditDataHost::ClearBody_impl() {
  multi_body->clearLater(); // clears items in event loop
  taiEditDataHost::ClearBody_impl();
}

void gpiMultiEditDataHost::Constr_Box() {
  // create the splitter before calling base, so scrbody gets put into the splitter
  splBody = new QSplitter(widget());
  splBody->setOrientation(QSplitter::Vertical);
  vblDialog->addWidget(splBody);

  taiEditDataHost::Constr_Box();

  scrMulti = new QScrollView(splBody);
  scrMulti->viewport()->setPaletteBackgroundColor(*bg_color);
  scrMulti->setResizePolicy(QScrollView::AutoOneFit);
  multi = new QWidget();
  scrMulti->addChild(multi);

  lay_multi = new QHBoxLayout(multi);
  multi_body = new iEditGrid(1, 2, 0, 1, 1, multi);  // margin, hspace, vspace, rows, cols, par
  multi_body->setPaletteBackgroundColor(*bg_color);
  if (bg_color_dark)
   multi_body->setHiLightColor(*bg_color_dark);
  multi_body->setRowHeight(row_height);
  lay_multi->addWidget(multi_body);
}


//////////////////////////////////////////////////////////
// 		gpiListDataHost				//
//////////////////////////////////////////////////////////

gpiListDataHost::gpiListDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: gpiMultiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  cur_lst = (TAGPtr)cur_base;
  num_lst_fields = 0;
}

gpiListDataHost::~gpiListDataHost() {
  lst_data_el.Reset();
  lst_membs.Reset();
}

void gpiListDataHost::ClearBody_impl() {
  lst_data_el.Reset();
  lst_membs.Reset();
  num_lst_fields = 0;
  gpiMultiEditDataHost::ClearBody_impl();
}

void gpiListDataHost::Constr_Strings(const char*, const char* win_title) {
  prompt_str = cur_lst->GetTypeDef()->name + ": ";
  if (cur_lst->GetTypeDef()->desc == taBase_List::StatTypeDef(0)->desc) {
    prompt_str += cur_lst->el_typ->name + "s: " + cur_lst->el_typ->desc;
  }
  else {
    prompt_str += cur_lst->GetTypeDef()->desc;
  }
  win_str = String(taiM->classname()) + ": " + win_title
     + " " + cur_lst->GetPath();
}

// don't check for null im ptr here
bool gpiListDataHost::ShowMember(MemberDef* md) {
  return md->ShowMember(show);
}

void gpiListDataHost::Constr_Final() {
  gpiMultiEditDataHost::Constr_Final();
  multi_body->resizeNames(); //temp: idatatable should do this automatically
}

void gpiListDataHost::Constr_Body() {
  gpiMultiEditDataHost::Constr_Body(); // reuse entire implementation for list members
  Constr_ListLabels();
  Constr_ListData();
}

void gpiListDataHost::Constr_ListData() {
  for (int lf = 0; lf < lst_data_el.size; ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    String nm = String("[") + String(lf) + "]: (" + lf_el->typ->name + ")";
    AddMultiColName(lf, nm, String(""));

    cur_row = 0;
    for (int i = 0; i < lf_el->typ->members.size; ++i) {
      MemberDef* md = lf_el->typ->members.FastEl(i);
      if(!ShowMember(md))
	continue;
      MemberDef* lst_md = lst_membs.FindName(md->name);
      if (lst_md == NULL)
	continue;
      for (int idx = cur_row; idx < lst_md->idx; ++idx) {	// align with other elements in List
	++cur_row;
      }
      taiData* mb_dat = md->im->GetDataRep(this, NULL, multi_body->dataGridWidget());
      lf_el->data_el.Add(mb_dat);
      AddMultiData(cur_row, lf, mb_dat->GetRep());
      ++cur_row;
    }
  }
}

void gpiListDataHost::Constr_ListLabels() {
  bool has_labels = false;
  if (lst_membs.size > 0)
    has_labels = true;
  int row = 0;
  for (int lf = 0; lf < cur_lst->size; ++lf) {
    TAPtr tmp_lf = (TAPtr)cur_lst->FastEl_(lf);
    if (tmp_lf == NULL)	continue;
    TypeDef* tmp_td = tmp_lf->GetTypeDef();
    lst_data_el.Add(new gpiList_ElData(tmp_td, tmp_lf));
    if (has_labels)
      continue;
    for (int i = 0; i < tmp_td->members.size; ++i) {
      MemberDef* md = tmp_td->members.FastEl(i);
      if (ShowMember(md) && !(lst_membs.FindName(md->name))) {
	MemberDef* nmd = md->Clone();
	lst_membs.Add(nmd);	// index now reflects position in list...
        String desc = "";
        GetMembDesc(md, desc, "");
        AddMultiRowName(row, md->name, desc);
        ++row;
      }
    }
  }
}

void gpiListDataHost::GetValue() {
  bool rebuild = false;
  if (lst_data_el.size != cur_lst->size) rebuild = true;
  if (!rebuild) {		// check that same elements are present!
    for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
      if (lst_data_el.FastEl(lf)->cur_base != (TAPtr)cur_lst->FastEl_(lf)) {
	rebuild = true;
	break;
      }
    }
  }
  if (rebuild) {
    taMisc::Error("Cannot apply changes: List size or elements have changed");
    return;
  }

  // first for the List-structure members
  GetValue_impl(typ->members, data_el, cur_base);
  // then the List elements
  for (int lf=0;  lf < lst_data_el.size;  ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    GetValue_impl(lf_el->typ->members, lf_el->data_el, lf_el->cur_base);
    ((TAPtr)lf_el->cur_base)->UpdateAfterEdit();
  }
  cur_lst->UpdateAfterEdit();	// call here too!
  taiMisc::Update((TAPtr)cur_lst);
  GetButtonImage();
  Unchanged();
}

void gpiListDataHost::GetImage() {
  bool rebuild = false;
  if (lst_data_el.size != cur_lst->size) rebuild = true;
  if (!rebuild) {		// check that same elements are present!
    for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
      if (lst_data_el.FastEl(lf)->cur_base != (TAPtr)cur_lst->FastEl_(lf)) {
	rebuild = true;
	break;
      }
    }
  }
if (rebuild) return; //TEMP
/*TODO  if (rebuild) {
    if (cur_lst->size == 0) {
      taMisc::Error("List has zero elements: canceling edit");
      Cancel();
      return;
    }
    lst_data_el.Reset();
    ivGlyphIndex i;
    for(i=lst_data_g->count()-1; i >= 0; i--)
      lst_data_g->remove(i);
    for(i=labels->count()-1; i >= 1; i--)
      labels->remove(i);
    lst_membs.Reset();
    Constr_ListMembs();
    Constr_Labels_impl(lst_membs);
    Constr_ElData();
  } */

  // first for the List-structure members
  GetImage_impl(typ->members, data_el, cur_base);

  // then the elements
  for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    GetImage_impl(lf_el->typ->members, lf_el->data_el, lf_el->cur_base);
  }
  Unchanged();
}
/* TODO
int gpiListDataHost::Edit() {
  if ((cur_lst != NULL) && (cur_lst->size > 100)) {
    int rval = taMisc::Choice("List contains more than 100 items (size = " +
			      String(cur_lst->size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if (rval == 1) return 0;
  }
  return gpiMultiEditDataHost::Edit();
} */


//////////////////////////////////
// 	taiGroupDataHost		//
//////////////////////////////////

gpiGroupDataHost::gpiGroupDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: gpiListDataHost(base, typ_, read_only_, modal_, parent)
{
  subData = NULL;
  scrSubData = NULL;
}

gpiGroupDataHost::~gpiGroupDataHost() {
  sub_data_el.Reset();
}

void gpiGroupDataHost::ClearBody_impl() {
  sub_data_el.Reset();
  DeleteChildrenLater(subData);
  gpiListDataHost::ClearBody_impl();
}

void gpiGroupDataHost::Constr_Strings(const char*, const char* win_title) {
  prompt_str = cur_lst->GetTypeDef()->name + ": ";
  if(cur_lst->GetTypeDef()->desc == TA_taBase_Group.desc) {
    prompt_str += cur_lst->el_typ->name + "s: " + cur_lst->el_typ->desc;
  }
  else {
    prompt_str += cur_lst->GetTypeDef()->desc;
  }
  win_str = String(taiM->classname()) + ": " +
    win_title + " " + cur_lst->GetPath();
}


void gpiGroupDataHost::Constr_Box() {
  gpiListDataHost::Constr_Box();
  TAGPtr cur_gp = (TAGPtr)cur_lst;
  //TODO: maybe should always create -- may not even show if empty
  if (cur_gp->gp.size > 0) {
    scrSubData = new QScrollView(multi);
    scrSubData->setHScrollBarMode(QScrollView::AlwaysOff); // no h scrolling
    subData = new QWidget();
    subData->setPaletteBackgroundColor(*bg_color_dark);
    scrSubData->viewport()->setPaletteBackgroundColor(*bg_color_dark);
    scrSubData->addChild(subData);
    scrSubData->viewport()->setPaletteBackgroundColor(*bg_color_dark);
    lay_multi->addWidget(scrSubData);

  }
}

void gpiGroupDataHost::Constr_Body() {
  gpiListDataHost::Constr_Body();
  laySubData = new QVBoxLayout(subData);
  //TODO:if there are subgroups, only show 2 of maingroup
  Constr_SubGpData();
}

void gpiGroupDataHost::GetImage() {
  gpiListDataHost::GetImage();
  TAGPtr cur_gp = (TAGPtr)cur_lst;
  if (sub_data_el.size != cur_gp->gp.size) {
return;
/*TODO    if (subData == NULL)
      return;
    sub_data_el.Reset();
    for (int i = sub_data_g->count() - 1; i >= 0; --i)
      sub_data_g->remove(i);
    Constr_SubGpData(); */
  }

  // and the sub-Lists
  for (int lf = 0; lf < cur_gp->gp.size; lf++) {
    TAGPtr sub = cur_gp->gp.FastEl(lf);
    gpiSubEditButton* sub_dat = (gpiSubEditButton*)sub_data_el.FastEl(lf);
    String nm = sub->name;
    if(nm == "")
      nm = String("[") + String(lf) + "]";
    sub_dat->label = nm;
    sub_dat->GetImage((void*)sub);
  }
}

void gpiGroupDataHost::Constr_SubGpData() {
  if(subData == NULL)
    return;
  TAGPtr cur_gp = (TAGPtr)cur_lst;
  bool widgetAdded = false;
  for (int lf = 0; lf < cur_gp->gp.size; ++lf) {
    TAGPtr sub = cur_gp->gp.FastEl(lf);
    String nm = sub->name;
    if (nm == "")
      nm = String("[") + String(lf) + "]";
    taiData* mb_dat =
      new gpiSubEditButton((void*)sub, nm, sub->GetTypeDef(), this, NULL, subData);
    QWidget* rep = mb_dat->GetRep();
    if (rep != NULL) {
      laySubData->addWidget(rep);
      widgetAdded = true;
    }
    rep->show(); // needed when rebuilding
    sub_data_el.Add(mb_dat);
  }
  if (widgetAdded)
    laySubData->addStretch();
}
/*TODO
int gpiGroupDataHost::Edit() {
  TAGPtr cur_gp = (TAGPtr)cur_lst;
  if((cur_gp != NULL) && ((cur_gp->size > 100) || (cur_gp->gp.size > 100))) {
    int rval = taMisc::Choice("Group or sub-groups contain more than 100 items (size = " +
			      String(cur_gp->size) + ", gp.size = "
			      + String(cur_gp->gp.size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if(rval == 1) return 0;
  }
  return taiEditDataHost::Edit();
} */

//////////////////////////////////
//	gpiArrayEditDataHost	//
//////////////////////////////////


gpiArrayEditDataHost::gpiArrayEditDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: taiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  n_ary_membs = 0;
}

gpiArrayEditDataHost::~gpiArrayEditDataHost() {
  //anything?
}

void gpiArrayEditDataHost::ClearBody_impl() {
  //???
  taiEditDataHost::ClearBody_impl();
}

bool gpiArrayEditDataHost::ShowMember(MemberDef* md) {
  if(md->name == "size")
    return true;
  else
    return taiEditDataHost::ShowMember(md);
}

void gpiArrayEditDataHost::GetImage() {
  taiEditDataHost::GetImage();
  taArray_base* cur_ary = (taArray_base*)cur_base;
  if (data_el.size != cur_ary->size + n_ary_membs) {
return; //TEMP
/*TODO:    ivGlyphIndex i;
    for(i=ary_data_g->count()-1; i >= 0; i--)
      ary_data_g->remove(i);
    int j;
    for(j=data_el.size-1; j >= n_ary_membs; j--)
      data_el.Remove(j);
    Constr_AryData(); */
  }
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i) {
    taiData* mb_dat = data_el.FastEl(i + n_ary_membs);
    tit->GetImage(mb_dat, cur_ary->FastEl_(i));
  }
}

void gpiArrayEditDataHost::GetValue() {
  taiEditDataHost::GetValue();
  taArray_base* cur_ary = (taArray_base*)cur_base;
  if (data_el.size != cur_ary->size + n_ary_membs) {
    taMisc::Error("Cannot apply changes: Array size has changed");
    return;
  }
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i){
    taiData* mb_dat = data_el.FastEl(i + n_ary_membs);
    tit->GetValue(mb_dat, cur_ary->FastEl_(i));
  }
}

void gpiArrayEditDataHost::Constr_AryData() {
  //NOTE: constructs names and labels
  QWidget* rep;
  taArray_base* cur_ary = (taArray_base*)cur_base;
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i) {
    taiData* mb_dat = tit->GetDataRep(this, NULL, body);
    data_el.Add(mb_dat);
    rep = mb_dat->GetRep();
    String nm = String("[") + String(i) + "]";
    AddData(i + n_ary_membs, rep);
    AddName(i + n_ary_membs, nm, String(""), rep);
  }
}

void gpiArrayEditDataHost::Constr_Data() {
//  data_g = layout->vbox();
//  ary_data_g = new lrScrollBox;
//  ary_data_g->naturalnum = 5;
//  Constr_Data_impl(typ->members, data_el);
  taiEditDataHost::Constr_Data();
  n_ary_membs = data_el.size;
  Constr_AryData();
//  FocusOnFirst();
//  GetImage();
}
/* TODO
int gpiArrayEditDataHost::Edit() {
  taArray_base* cur_ary = (taArray_base*)cur_base;
  if((cur_ary != NULL) && (cur_ary->size > 100)) {
    int rval = taMisc::Choice("Array contains more than 100 items (size = " +
			      String(cur_ary->size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if(rval == 1) return 0;
  }
  return taiEditDataHost::Edit();
} */


/////////////////////////////////
//	SArgEditDataHost	//
//////////////////////////////////

SArgEditDataHost::SArgEditDataHost(void* base, TypeDef* tp,  bool read_only_,
  	bool modal_, QObject* parent)
: gpiArrayEditDataHost(base, tp, read_only_, modal_, parent) {
  n_ary_membs = 0;
//  ary_data_g = NULL;
}

bool SArgEditDataHost::ShowMember(MemberDef* md) {
  if(md->name == "size")
    return true;
  else
    return gpiArrayEditDataHost::ShowMember(md);
}

void SArgEditDataHost::Constr_AryData() {
  SArg_Array* cur_ary = (SArg_Array*)cur_base;
  cur_ary->UpdateAfterEdit();
  MemberDef* eldm = typ->members.FindName("el");
  taiType* it = eldm->type->GetNonPtrType()->it;
  QWidget* rep;
  for (int i=0; i < cur_ary->size; ++i) {
    taiData* mb_dat = it->GetDataRep(this, NULL, body);
    data_el.Add(mb_dat);
    rep = mb_dat->GetRep();
    String nm = String("[") + String(i) + "]";
    String lbl = cur_ary->labels[i];
    if (!lbl.empty())
      nm = lbl + nm;
    AddData(i + n_ary_membs, rep);
    AddName(i + n_ary_membs, nm, String(""), rep);
  }
}


//////////////////////////////////
//	gpiSelectEdit		//
//////////////////////////////////

gpiSelectEditDataHost::gpiSelectEditDataHost(void* base, TypeDef* td, bool read_only_,
  	bool modal_, QObject* parent)
: taiEditDataHost(base, td, read_only_, modal_, parent)
{
  use_show = false;
  sele = (SelectEdit*)base;
  base_items = 0;
  mnuRemoveMember = NULL;

}

gpiSelectEditDataHost::~gpiSelectEditDataHost() {
}

bool gpiSelectEditDataHost::ShowMember(MemberDef* md) {
  if (md->im == NULL) return false;
  if((md->name == "config")) return true;
  return false;
}

void gpiSelectEditDataHost::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodData objects are used in menu calls, so can't be
  layMethButtons = NULL;
  DeleteChildrenLater(frmMethButtons);
  showMethButtons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  mnuRemoveMember = NULL;
  if (menu) {
    menu->ResetMenu();
  }
  taiEditDataHost::ClearBody_impl();
}

void gpiSelectEditDataHost::Constr_Body() {
  if (rebuild_body) {
    meth_el.Reset();
  }
  taiEditDataHost::Constr_Body();
  mnuRemoveMember = new QPopupMenu();

  base_items = data_el.size;
  String nm;
  String help_text;
  for (int i = 0; i < sele->members.size; ++i) {
    MemberDef* md = sele->members.FastEl(i);
    taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
    data_el.Add(mb_dat);
    QWidget* data = mb_dat->GetRep();
    int row = AddData(-1, data);

    help_text = "";
    String new_lbl = sele->config.mbr_labels.FastEl(i);
//    String new_lbl = sele->GetMbrLabel(i);
    GetName(md, nm, help_text); //note: we just call this to get the help text
    if (!new_lbl.empty())
      nm = new_lbl;
    AddName(row, nm, help_text, data);
    MakeMenuItem(mnuRemoveMember, nm, i, i, SLOT(mnuRemoveMember_select(int)));
  }
  // we deleted the normally not-deleted methods, so redo them here
  if (rebuild_body) {
    Constr_Methods();
    frmMethButtons->setHidden(!showMethButtons);
  }
}

void gpiSelectEditDataHost::Constr_Methods() {
  taiEditDataHost::Constr_Methods();
  QPopupMenu* mnuRemoveMethod_menu = new QPopupMenu();
  QPopupMenu* mnuRemoveMethod_menu_but = new QPopupMenu();
  QPopupMenu* mnuRemoveMethod_but = new QPopupMenu();
  if (cur_menu != NULL) {// for safety... cur_menu should be the SelectEdit menu
    cur_menu->AddSep();
    QPopupMenu* menu_tmp = cur_menu->rep_popup();
    menu_tmp->insertItem("Remove field", mnuRemoveMember);
    taiMenu* taimen_tmp = cur_menu->AddSubMenu("Remove function");
    menu_tmp = taimen_tmp->rep_popup();
    menu_tmp->insertItem("Main menu", mnuRemoveMethod_menu);
    menu_tmp->insertItem("Menu buttons", mnuRemoveMethod_menu_but);
    menu_tmp->insertItem("Buttons", mnuRemoveMethod_but);
  }


  for (int i = 0; i < sele->methods.size; ++i) {
    MethodDef* md = sele->methods.FastEl(i);
    if(md->im == NULL) continue;
    taiMethodData* mth_rep = md->im->GetMethodRep(sele->meth_bases.FastEl(i), this, NULL, frmMethButtons);
    if (mth_rep == NULL) continue;
    meth_el.Add(mth_rep);
    QPopupMenu* rem_men = NULL; // remove menu

    String men_nm = sele->config.meth_labels.FastEl(i);
    if (mth_rep->is_menu_item) {
      //NOTE: for seledit functions, we never place them on the last menu or button, because that may
      // make no sense -- the label specifies the place, or Actions if no label
      if (md->HasOption("MENU_BUTTON")) { // menu button item
        if (men_nm.empty())
          men_nm = "Actions";
        cur_menu_but = ta_menu_buttons.FindName(men_nm);
        if (cur_menu_but == NULL) {
          cur_menu_but = new
            taiMenu(taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
                    NULL, this, NULL, widget());
          cur_menu_but->setLabel(men_nm);
          DoAddMethButton((QPushButton*)cur_menu_but->GetRep()); // rep is the button for buttonmenu
          ta_menu_buttons.Add(cur_menu_but);
          rem_men = new QPopupMenu();
          mnuRemoveMethod_menu_but->insertItem(men_nm, rem_men);
        } else {
          rem_men = FindMenuItem(mnuRemoveMethod_menu_but, men_nm);
        }
        mth_rep->AddToMenu(cur_menu_but);
      } else { // menubar item
        if (men_nm.empty())
          men_nm = "Actions";
        cur_menu = ta_menus.FindName(men_nm);
        if (cur_menu == NULL) {
          cur_menu = menu->AddSubMenu(men_nm);
          ta_menus.Add(cur_menu);
          rem_men = new QPopupMenu();
          mnuRemoveMethod_menu->insertItem(men_nm, rem_men);
        } else {
          rem_men = FindMenuItem(mnuRemoveMethod_menu, men_nm);
        }
        mth_rep->AddToMenu(cur_menu);
      }
      //note: we assume mth_rep methods will use GetLabel() to provide the menu text
      if (rem_men) MakeMenuItem(rem_men, md->GetLabel(), i, i, SLOT(mnuRemoveMethod_select(int)));
   } else {
      if (men_nm.empty())
        men_nm = md->GetLabel();
      AddMethButton(mth_rep, men_nm);
      MakeMenuItem(mnuRemoveMethod_but, men_nm, i, i, SLOT(mnuRemoveMethod_select(int)));
    }
  }
}

void gpiSelectEditDataHost::DoRemoveSelEdit() {
   // removes the sel_item_index item -- need to reduce by 1 because of pre-existing items on seledit dialog
  sele->RemoveField(sel_item_index - base_items);
}

void gpiSelectEditDataHost::FillLabelContextMenu_SelEdit(iContextLabel* sender, QPopupMenu* menu, int& last_id) {
  if (sender->index() < base_items) return; // only add for user-added items
  menu->insertItem("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()), 0, ++last_id);
}

QPopupMenu* gpiSelectEditDataHost::FindMenuItem(QPopupMenu* par_menu, const char* label) {
  int id = 0;
  for (uint i = 0; i < par_menu->count(); ++i) {
    id = par_menu->idAt(i);
    if (par_menu->text(id) == label)
      return (QPopupMenu*)par_menu->findItem(id);
  }
  return NULL;
}

void gpiSelectEditDataHost::GetValue() {
  GetValue_impl(typ->members, data_el, cur_base);
  sele->UpdateAllBases();
  Unchanged();
}

void gpiSelectEditDataHost::GetValue_impl(const MemberSpace& ms, const taiDataList& dl,
	void* base)
{
  int i, cnt = 0;
  bool first_diff = true;
  for(i=0; i<ms.size; i++) {
    MemberDef* md = ms.FastEl(i);
    if(!ShowMember(md)) continue;
    taiData* mb_dat = dl.FastEl(cnt++);
    md->im->GetMbrValue(mb_dat, base, first_diff);
  }
  if(!first_diff) {		// end the basic guy
    taiMember::EndScript(base);
    first_diff = true;
  }

  for(i=0; i<sele->members.size; i++) {
    MemberDef* md = sele->members.FastEl(i);
    TAPtr bs = sele->mbr_bases.FastEl(i);
    if(bs == NULL) continue;
    taiData* mb_dat = dl.FastEl(cnt++);
    md->im->GetMbrValue(mb_dat, (void*)bs, first_diff);
    if(!first_diff) {		// always reset!
      taiMember::EndScript((void*)bs);
      first_diff = true;
    }
  }
}

void gpiSelectEditDataHost::GetImage_impl(const MemberSpace& ms, const taiDataList& dl,
	void* base)
{
  int cnt = 0;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (!ShowMember(md))
      continue;
    taiData* mb_dat = dl.FastEl(cnt++);
    md->im->GetImage(mb_dat, base);
  }

  for (int i = 0; i < sele->members.size; ++i) {
    MemberDef* md = sele->members.FastEl(i);
    TAPtr bs = sele->mbr_bases.FastEl(i);
    if (bs == NULL) continue;
    taiData* mb_dat = dl.FastEl(cnt++);
    md->im->GetImage(mb_dat, (void*)bs);
  }
}

void gpiSelectEditDataHost::MakeMenuItem(QPopupMenu* menu, const char* name, int index, int param, const char* slot) {
    menu->insertItem(name, this, slot, 0, index);
    menu->setItemParameter(index, param);
}


void gpiSelectEditDataHost::mnuRemoveMember_select(int idx) {
  sele->RemoveField(idx);
}

void gpiSelectEditDataHost::mnuRemoveMethod_select(int idx) {
  sele->RemoveFun(idx);
}
