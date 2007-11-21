// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_qtgroup.cpp
#include "ta_qtgroup.h"


#include "ta_base.h"
#include "ta_qt.h"
#include "ta_qttype.h"
#include "ta_seledit.h"
#include "ta_dmem.h"
#include "ta_project.h" // for taDoc

#include "css_ta.h"

#include <qapplication.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenudata.h>
#include <QMenu>
#include <Q3VBox>
#include <QScrollArea> // for gpiGroupDialog
#include <QSplitter>
#include <qtooltip.h>
#include <QDesktopServices>

#include "icolor.h"
#include "ieditgrid.h"
#include "iscrollarea.h"
#include "itextbrowser.h"
#include "itextedit.h"


//////////////////////////
// 	Edit Buttons	//
//////////////////////////


gpiListEditButton::gpiListEditButton
(void* base, TypeDef* tp, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
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
(void* base, TypeDef* tp, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
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
(void* base, const char* nm, TypeDef* tp, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
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
(void* base, TypeDef* tp, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
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
    taiMethodData* mth_rep = md->im->GetMethodRep(cur_base, host, this, gui_parent);
    if(mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    mth_rep->AddToMenu(this);
  }
}

gpiListLinkEditButton::gpiListLinkEditButton
(void* base, TypeDef* tp, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
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
    taiMethodData* mth_rep = md->im->GetMethodRep(cur_base, host, this, gui_parent);
    if(mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    mth_rep->AddToMenu(this);
  }
}


//////////////////////////////////
//  	ArrayEditButton		//
//////////////////////////////////


gpiArrayEditButton::gpiArrayEditButton
(void* base, TypeDef* tp, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
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


//////////////////////////////////
//   taiListElsButtonBase		//
//////////////////////////////////

taiListElsButtonBase::taiListElsButtonBase(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
}

int taiListElsButtonBase::BuildChooser_0(taiItemChooser* ic, taList_impl* top_lst, 
  QTreeWidgetItem* top_item) 
{
  int rval = 0;
  
  for (int i = 0; i < top_lst->size; ++i) {
    TAPtr tab = (TAPtr)top_lst->FastEl_(i);
    if (!tab)  continue;
    QTreeWidgetItem* item = ic->AddItem(tab->GetColText(taBase::key_disp_name), top_item, tab); 
    item->setText(1, tab->GetColText(taBase::key_type));
    item->setText(2, tab->GetColText(taBase::key_desc));
    ++rval;
  }
  return rval;
}

int taiListElsButtonBase::columnCount(int view) const {
  switch (view) {
  case 0: return 3;
  default: return 0; // not supposed to happen
  }
}

const String taiListElsButtonBase::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Name"; 
    case 1: return "Type"; 
    case 2: return "Description"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiListElsButtonBase::labelNameNonNull() const {
  return item()->GetDisplayName();
}

const String taiListElsButtonBase::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List"; 
  default: return _nilString;
  }
}


//////////////////////////////////
//   taiListElsButton		//
//////////////////////////////////

taiListElsButton::taiListElsButton(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
  list = NULL;
}

void taiListElsButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!list) {
    taMisc::Error("taiListElsButtonBase::BuildChooser: list needed");
    return;
  }
  switch (view) {
  case 0: 
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(" NULL", NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, " ");
    }
    BuildChooser_0(ic, list, NULL); 
    break; 
  default: break; // shouldn't happen
  }
}

void taiListElsButton::GetImage(taList_impl* base_lst, TAPtr it) {
  list = base_lst;
  inherited::GetImage((void*)it, base_lst->el_base);
}


//////////////////////////////////
//   taiGroupElsButton		//
//////////////////////////////////

taiGroupElsButton::taiGroupElsButton(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
  grp = NULL;
}

void taiGroupElsButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!grp) {
    taMisc::Error("taiGroupElsButtonBase::BuildChooser: group needed");
    return;
  }
  switch (view) {
  case 0: 
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(" NULL", NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, " ");
    }
    BuildChooser_1(ic, grp, NULL); 
    break; 
  default: break; // shouldn't happen
  }
}

int taiGroupElsButton::BuildChooser_1(taiItemChooser* ic, taGroup_impl* top_grp, 
  QTreeWidgetItem* top_item) 
{
  int rval = 0;
  
  for (int i = 0; i < top_grp->gp.size; ++i) {
    taGroup_impl* tag = (taGroup_impl*)top_grp->gp.FastEl_(i);
    if (!tag)  continue;
    QTreeWidgetItem* item = ic->AddItem(tag->GetDisplayName(), top_item, tag); 
    item->setFlags(Qt::ItemIsEnabled); // not selectable
    //note: don't put text in the other columns, to keep items clean
    //TODO: put folder icon
    rval += BuildChooser_1(ic, tag, item);
    ++rval;
  }
  
  rval += BuildChooser_0(ic, top_grp, top_item); 
  return rval;
}

void taiGroupElsButton::GetImage(taGroup_impl* base_grp, TAPtr it) {
  grp = base_grp;
  inherited::GetImage((void*)it, NULL); // don't need a targ_typ
}


//////////////////////////
// 	gpiListEls	//
//////////////////////////


gpiListEls::gpiListEls(taiActions::RepType rt, int ft, TABLPtr lst, TypeDef* typ_, IDataHost* host_,
	taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ths = lst;
//  ta_actions = new taiMenu(rt, taiMenu::radio_update, ft, typ, (void*)ths, host_, this, gui_parent_);
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  over_max = false;
}

gpiListEls::gpiListEls(taiMenu* existing_menu, TABLPtr lst, TypeDef* typ_,
	IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(existing_menu, typ_, host_, par, gui_parent_, flags_) {
  ths = lst;
  over_max = false;
}

QWidget* gpiListEls::GetRep() {
  return ta_actions->GetRep();
}

void gpiListEls::GetImage(TABLPtr base_lst, TAPtr it) {
  if (ths != base_lst) {
    ths = base_lst;
  }
  UpdateMenu();
  ta_actions->GetImageByData(Variant(it)); // get rid of (void*)!
  setCur_obj(it, false);
}

TAPtr gpiListEls::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (over_max) {
    if ((cur == NULL) || (cur->text() == "<Over max, select...>")
       || (cur->text() == "gp.<Over max, select...>"))
      return cur_obj;
    else
      return cur->usr_data.toBase();
  }
  if(cur != NULL)
    return cur->usr_data.toBase();
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
//   if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select>") &&
//      (ta_actions->cur_sel->usr_data != NULL))
//     chs_root = (TABLPtr)ta_actions->cur_sel->usr_data;

  taiObjChooser* chs = taiObjChooser::createInstance(chs_root, "List/Group Objects", true);
  //if ((chs_obj != NULL) && !chs_obj->GetName().empty())
    //obs chs->sel_str = chs_obj->GetName();
    chs->setSel_obj(cur_obj);
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj(chs->sel_obj());
/*TODO: even needed???    if((ta_actions->curSel() != NULL) && (ta_actions->curSel()->label == "<Over max, select...>") &&
       (ta_actions->curSel()->men_act != NULL)) {
      ta_actions->curSel()->usr_data = (void*)chs_obj;
      ta_actions->curSel()->men_act->Select(ta_actions->curSel()); // call it!
    }
    else */
   //   ta_actions->setLabel(chs->sel_str());
  }
  delete chs;
}

void gpiListEls::GetMenu(taiActions* menu, taiMenuAction* actn) {
  if (HasFlag(flgNullOk))
    menu->AddItem("NULL", taiMenu::radio_update, actn, (void*)NULL);
  if (HasFlag(flgEditOk))
    menu->AddItem("Edit", taiMenu::normal,
	taiAction::action, this, SLOT(Edit()));
  if (!HasFlag(flgNoList)) {
    String nm;
    if (ths->GetName() == "")
      nm = "All";
    else
      nm = ths->GetName();
    menu->AddItem(nm, taiMenu::use_default, actn, ths);
  }
  menu->AddSep(); //note: doesn't double add or add at beginning
  GetMenu_impl(ths, menu, actn);
}

void gpiListEls::UpdateMenu(taiMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(ta_actions, actn);
}

void gpiListEls::GetMenu_impl(TABLPtr cur_lst, taiActions* menu, taiMenuAction* actn) {
  if (cur_lst == NULL)	return;
  if (cur_lst->size >= taMisc::max_menu) {
    taiAction* mnel = menu->AddItem
      ("<Over max, select...>", taiMenu::normal,
	taiAction::action, this, SLOT(Choose()), cur_lst );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
    return;
  }
  TAPtr tmp;
  for (int i = 0; i < cur_lst->size; ++i) {
    tmp = (TAPtr)cur_lst->FastEl_(i);
    if (tmp == NULL) continue;
    String nm = tmp->GetName();
    if (nm == "")
      nm = String("[") + String(i) + "]: (" + tmp->GetTypeDef()->name + ")";
    menu->AddItem((char*)nm, taiMenu::radio_update, actn, tmp);
  }
}


//////////////////////////
// 	gpiGroupEls	//
//////////////////////////

gpiGroupEls::gpiGroupEls(taiActions::RepType rt, int ft, TABLPtr lst, TypeDef* typ_, IDataHost* host_, 
  taiData* par, QWidget* gui_parent_, int flags_)
: gpiListEls(rt,ft,lst, typ_, host_,par, gui_parent_, flags_)
{
}

gpiGroupEls::gpiGroupEls(taiMenu* existing_menu, TABLPtr gp, TypeDef* tp, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_) // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk
: gpiListEls(existing_menu, gp, tp, host_,par, gui_parent_, flags_)
{
}

void gpiGroupEls::ChooseGp() {
  TABLPtr chs_root = &(((TAGPtr)ths)->gp);
  // this is not a good idea: it makes the prior selection the root, so you can't actuall
  // choose anything!
//   if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "gp.<Over max, select...>") &&
//      (ta_actions->cur_sel->usr_data != NULL))
//     chs_root = (TABLPtr)ta_actions->cur_sel->usr_data;

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

void gpiGroupEls::GetMenu_impl(TABLPtr cur_lst, taiActions* menu, taiMenuAction* actn) {
  if (cur_lst == NULL) return;
  TAGPtr cur_gp = (TAGPtr)cur_lst;
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
      TAGPtr tmp_grp = (TAGPtr)cur_gp->FastGp_(i);
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


//////////////////////////
// 	gpiSubGroups	//
//////////////////////////


gpiSubGroups::gpiSubGroups(taiActions::RepType rt, int ft, TAGPtr gp, TypeDef* typ_, IDataHost* host_, 
  taiData* par,	QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ths = gp;
//  ta_actions = new taiMenu(rt, taiMenu::radio_update, ft, (void*)ths, typ, host_, this, gui_parent_);
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  over_max = false;
}

gpiSubGroups::gpiSubGroups(taiMenu* existing_menu, TAGPtr gp, TypeDef* typ_,
	IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  ths = gp;
  ownflag = false;
  over_max = false;
}

QWidget* gpiSubGroups::GetRep() {
  return ta_actions->GetRep();
}

void gpiSubGroups::GetImage(TAGPtr base_gp, TAGPtr gp) {
  if (ths != base_gp) {
    ths = base_gp;
    GetMenu(ta_actions, NULL);
  }
  ta_actions->GetImageByData(Variant((void*)gp));
  setCur_obj(gp, false);
}

TAGPtr gpiSubGroups::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (over_max) {
    if((cur == NULL) || (cur->text() == "<Over max, select...>"))
      return (TAGPtr)cur_obj;
    else
      return (TAGPtr)cur->usr_data.toPtr();
  }
  if (cur != NULL)
    return (TAGPtr)cur->usr_data.toPtr();
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
//   if((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select>") &&
//      (ta_actions->cur_sel->usr_data != NULL))
//     chs_root = (TABLPtr)ta_actions->cur_sel->usr_data;

  taiObjChooser* chs = taiObjChooser::createInstance(chs_root, "SubGroups", true);
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

void gpiSubGroups::GetMenu(taiActions* menu, taiMenuAction* actn) {
  if (HasFlag(flgNullOk))
    menu->AddItem("NULL", taiMenu::use_default, actn, (void*)NULL);
  if (HasFlag(flgEditOk))
    menu->AddItem("Edit", taiMenu::normal,
	taiAction::action, this, SLOT(Edit()) );
  if (ths == NULL)	return;
  String nm;
  if(ths->owner == NULL)
    nm = ths->GetTypeDef()->name;
  else if(ths->owner->GetName() == "")
    nm = ths->owner->GetTypeDef()->name;
  else
    nm = ths->owner->GetName();
  menu->AddItem(nm, taiMenu::use_default, actn, (void*)ths);
  menu->AddSep();
  GetMenu_impl(ths, menu, actn);
}

void gpiSubGroups::UpdateMenu(taiMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(actn);
}

void gpiSubGroups::GetMenu_impl(TAGPtr gp, taiActions* menu, taiMenuAction* actn) {
  if (gp == NULL) return;
  if (gp->gp.size >= taMisc::max_menu) {
    taiAction* mnel = menu->AddItem
      ("<Over max, select...>", taiMenu::normal,
        taiAction::action, this, SLOT(Choose()), gp );
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
//TODO: tmp_grp???	    sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
            sub_menu = menu->AddSubMenu(nm);
	    sub_menu->AddItem(nm, taiMenu::use_default, actn, tmp);
	  }
	  String subnm = String("::") + md->name;
	  sub_menu->AddItem(subnm, taiMenu::use_default, actn, tmp_grp);
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
//TODO qt4      taiMenu* sub_menu = menu->AddSubMenu(nm, (void*)tmp_grp);
      taiMenu* sub_menu = menu->AddSubMenu(nm);
      String subnm = nm + ": Group";
      sub_menu->AddItem(subnm, taiMenu::use_default, actn, tmp_grp);
      sub_menu->AddSep();
      GetMenu_impl(tmp_grp, sub_menu, actn);
    } else
      menu->AddItem(nm, taiMenu::use_default, actn, tmp_grp);
  }
}


//////////////////////////
// 	gpiElTypes	//
//////////////////////////

gpiElTypes::gpiElTypes(taiActions::RepType rt, int ft, TypeDef* lstd, TypeDef* typ_, IDataHost* host_, 
  taiData* par, QWidget* gui_parent_, int flags_)
: taiTypeHier(rt, ft, typ_, host_, par, gui_parent_, flags_)
{
  lst_typd = lstd;
}

gpiElTypes::gpiElTypes
(taiMenu* existing_menu, TypeDef* gtd, TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiTypeHier(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  lst_typd = gtd;
}

void gpiElTypes::GetMenu(taiActions* menu, taiMenuAction* nact) {
  GetMenu_impl(menu, typ, nact);
  menu->AddSep(); //note: won't add a spurious separator if not needed
  GetMenu_impl(menu, lst_typd, nact);	// get group types for this type
}


//////////////////////////
//    gpiNewFuns	//
//////////////////////////

gpiNewFuns* gpiNewFuns::CondNew(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
{
  for (int i = 0; i < typ_->methods.size; ++i){
    MethodDef* md = typ_->methods.FastEl(i);
    if (md->HasOption("NEW_FUN")) {
      return new gpiNewFuns(typ_, host_, par, gui_parent_, flags_);
    }
  }
  return NULL;
}

gpiNewFuns::gpiNewFuns(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(new Q3VBox(gui_parent_));

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
  if (parent == NULL) parent = taiMisc::main_window;
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
  inherited::Constr_Body();
  num_rep = new taiIncrField(NULL, this, NULL, body);//, taiData::flgPosOnly);
//   typ_rep = new gpiElTypes(taiMenu::buttonmenu, taiMisc::fonSmall,
// 			       ths->GetTypeDef(), ths->el_base, this, NULL, body);
//   typ_rep->GetMenu();
//  typ_rep->GetImage(typ);
  typ_rep = new taiTypeDefButton(ths->el_base, this, NULL, body);
  typ_rep->GetImage(ths->el_typ, ths->el_base);

  fun_list = gpiNewFuns::CondNew(ths->el_base, this, NULL, body); // only creates if there are funcs

  // lay out controls
  QWidget* trep;
  int row;
  trep = num_rep->GetRep();
  row = AddData(0, trep);
  AddName(0, "number", "the number of new items to make", num_rep);

  trep = typ_rep->GetRep();
  row = AddData(1, trep);
  AddName(1, "of type", "the Type of new items to make", typ_rep);

  Constr_SubGpList(); // hook for groups

  // if any callable functions, put them in a box for the user to select from
  if ((fun_list != NULL) && (fun_list->funs.size > 0)) {
    trep = fun_list->GetRep();
    row = AddData(-1, trep);
    AddName(row, "call funs", "optional functions to call on each instance", fun_list);
  }
}

void gpiListNew::ClearBody_impl() {
  if (fun_list) {delete fun_list; fun_list = NULL;}
  if (typ_rep) {delete typ_rep; typ_rep = NULL;}
  num_rep = NULL;
  inherited::ClearBody_impl();
}

void gpiListNew::Constr_Strings(const char*, const char* win_title) {
  inherited::Constr_Strings("New: Create new object(s)", win_title);
}

void gpiListNew::Constr_Final() {
  inherited::Constr_Final();
  GetImage();
}

void gpiListNew::GetImage() {
  num_rep->GetImage(num);
  typ_rep->GetImage(typ, ths->el_base);
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
  if (parent == NULL) parent = taiMisc::main_window;
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
  AddName(row, "in", "the subgroup in which to add items", subgp_list);
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


//////////////////////////////////
//  gpiMultiEditDataHost	//
//////////////////////////////////

gpiMultiEditDataHost::gpiMultiEditDataHost(void* base, TypeDef* typ_, bool read_only_,
      bool modal_, QObject* parent)
: inherited(base, typ_, read_only_, modal_, parent)
{
  multi = NULL;
  scrMulti = NULL;
  multi_body = NULL;
  lay_multi = NULL;
  multi_rows = 1;
  multi_cols = 1;
  header_row = true; // most compatible choice
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
  ClearMultiBody_impl();
  taiEditDataHost::ClearBody_impl();
}

void gpiMultiEditDataHost::ClearMultiBody_impl() {
  multi_body->clearLater(); // clears items in event loop
}

void gpiMultiEditDataHost::Constr_Body() {
  inherited::Constr_Body(); // reuse entire implementation for list members
  Constr_MultiBody();
}

void gpiMultiEditDataHost::Constr_Box() {
  // create the splitter before calling base, so scrbody gets put into the splitter
  splBody = new QSplitter(widget());
  splBody->setOrientation(Qt::Vertical);
  vblDialog->addWidget(splBody, 1); // gets all the space

  taiEditDataHost::Constr_Box();

  scrMulti = new iScrollArea(splBody);
  SET_PALETTE_BACKGROUND_COLOR(scrMulti->viewport(), bg_color);
  scrMulti->setWidgetResizable(true);
  multi = new QWidget();
  scrMulti->setWidget(multi);

  lay_multi = new QHBoxLayout(multi);
  lay_multi->setMargin(0);
  multi_body = new iEditGrid(header_row, 2, 1, 1, 1, multi);  // , hmargins, vmargins, rows, cols, par
  SET_PALETTE_BACKGROUND_COLOR(multi_body, bg_color);
  multi_body->setHiLightColor(bg_color_dark);
  multi_body->setRowHeight(row_height);
  lay_multi->addWidget(multi_body);
}

void gpiMultiEditDataHost::Constr_MultiBody() {
  // nothing
}

void gpiMultiEditDataHost::RebuildMultiBody() {
//note: don't disable updates before clear, because it really doesn't help,
// and just requires recursive descent into everything to be nuked
  ClearMultiBody_impl();
  multi->setUpdatesEnabled(false);
    Constr_MultiBody();
  multi->setUpdatesEnabled(true);
}


//////////////////////////////////
//  gpiListDataHost		//
//////////////////////////////////

gpiListDataHost::gpiListDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: gpiMultiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  cur_lst = (TABLPtr)cur_base;
  num_lst_fields = 0;
}

gpiListDataHost::~gpiListDataHost() {
  lst_data_el.Reset();
  lst_membs.Reset();
}

void gpiListDataHost::ClearMultiBody_impl() {
  lst_data_el.Reset();
  lst_membs.Reset();
  num_lst_fields = 0;
  inherited::ClearMultiBody_impl();
}


void gpiListDataHost::Constr_Strings(const char*, const char* win_title) {
  prompt_str = cur_lst->GetTypeDef()->name + ": ";
  if (cur_lst->GetTypeDef()->desc == taBase_List::StatTypeDef(0)->desc) {
    prompt_str += cur_lst->el_typ->name + "s: " + cur_lst->el_typ->desc;
  }
  else {
    prompt_str += cur_lst->GetTypeDef()->desc;
  }
  win_str = String(win_title)
     + " " + cur_lst->GetPath();
}

void gpiListDataHost::Constr_Final() {
  gpiMultiEditDataHost::Constr_Final();
  multi_body->resizeNames(); //temp: idatatable should do this automatically
}

void gpiListDataHost::Constr_MultiBody() {
  inherited::Constr_MultiBody(); 
  Constr_ElData();
  Constr_ListLabels();
  Constr_ListData();
}

void gpiListDataHost::Constr_ElData() {
  for (int lf = 0; lf < cur_lst->size; ++lf) {
    TAPtr tmp_lf = (TAPtr)cur_lst->FastEl_(lf);
    if (tmp_lf == NULL)	continue; // note: not supposed to have NULL values in lists
    TypeDef* tmp_td = tmp_lf->GetTypeDef();
    lst_data_el.Add(new gpiList_ElData(tmp_td, tmp_lf));
    // add to the unique list of all showable members
    for (int i = 0; i < tmp_td->members.size; ++i) {
      MemberDef* md = tmp_td->members.FastEl(i);
      if (ShowMember(md)) {
        lst_membs.AddUnique(md->name);
      }
    }
  }
} 

void gpiListDataHost::Constr_ListData() {
  for (int lf = 0; lf < lst_data_el.size; ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    String nm = String("[") + String(lf) + "]: (" + lf_el->typ->name + ")";
    AddMultiColName(lf, nm, String(""));

    for (int i = 0; i < lf_el->typ->members.size; ++i) {
      MemberDef* md = lf_el->typ->members.FastEl(i);
      if (!ShowMember(md)) continue;
      int lst_idx = lst_membs.FindEl(md->name);
      if (lst_idx < 0) continue; //note: shouldn't happen!!!
      cur_row = lst_idx; 
      taiData* mb_dat = md->im->GetDataRep(this, NULL, multi_body->dataGridWidget());
      lf_el->memb_el.Add(md);
      lf_el->data_el.Add(mb_dat);
      //TODO: should get desc for this member, to add to tooltip for rep
      AddMultiData(cur_row, lf, mb_dat->GetRep());
    }
  }
}

void gpiListDataHost::Constr_ListLabels() {
  for (int lf = 0; lf < lst_membs.size; ++lf) {
    //NOTE: no desc's because same name'd member could conflict
    AddMultiRowName(lf, lst_membs.FastEl(lf), "");
  }
}

void gpiListDataHost::GetValue_Membs() {
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
 // NOTE: we should always be able to do a GetValue, because we always rebuild
 // when data changes (ie, in program, or from another gui panel)
  if (rebuild) {
    taMisc::Error("Cannot apply changes: List size or elements have changed");
    return;
  }

  // first for the List-structure members
  GetValue_Membs_def();
  // then the List elements
  for (int lf=0;  lf < lst_data_el.size;  ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    GetValue_impl(&lf_el->memb_el, lf_el->data_el, lf_el->cur_base);
    ((TAPtr)lf_el->cur_base)->UpdateAfterEdit();
  }
  cur_lst->UpdateAfterEdit();	// call here too!
  taiMisc::Update((TAPtr)cur_lst);
}

void gpiListDataHost::GetImage_Membs() {
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
    RebuildMultiBody(); 
  /*obs lst_data_el.Reset();
    ivGlyphIndex i;
    for(i=lst_data_g->count()-1; i >= 0; i--)
      lst_data_g->remove(i);
    for(i=labels->count()-1; i >= 1; i--)
      labels->remove(i);
    lst_membs.Reset();
    Constr_ListMembs();
    Constr_Labels_impl(lst_membs);
    Constr_ElData(); */
  } 

  // first for the List-structure members
  GetImage_Membs_def();

  // then the elements
  for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
    gpiList_ElData* lf_el = lst_data_el.FastEl(lf);
    GetImage_impl(&lf_el->memb_el, lf_el->data_el, lf_el->cur_base);
  }
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


gpiCompactList_ElData::gpiCompactList_ElData(TypeDef* typ_, TAPtr base, taiData* data_el_) {
  typ = typ_;
  cur_base = base;
  data_el = data_el_;
}

gpiCompactList_ElData::~gpiCompactList_ElData() {}


//////////////////////////////////
//  gpiCompactListDataHost	//
//////////////////////////////////

gpiCompactListDataHost::gpiCompactListDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: gpiMultiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  header_row = false; 
  cur_lst = (TABLPtr)cur_base;
}

gpiCompactListDataHost::~gpiCompactListDataHost() {
  lst_data_el.Reset();
}

void gpiCompactListDataHost::ClearMultiBody_impl() {
  lst_data_el.Reset();
  inherited::ClearMultiBody_impl();
}


void gpiCompactListDataHost::Constr_Strings(const char*, const char* win_title) {
  prompt_str = cur_lst->GetTypeDef()->name + ": ";
  if (cur_lst->GetTypeDef()->desc == taBase_List::StatTypeDef(0)->desc) {
    prompt_str += cur_lst->el_typ->name + "s: " + cur_lst->el_typ->desc;
  }
  else {
    prompt_str += cur_lst->GetTypeDef()->desc;
  }
  win_str = String(win_title)
     + " " + cur_lst->GetPath();
}

// don't check for null im ptr here
bool gpiCompactListDataHost::ShowMember(MemberDef* md) const {
  return md->ShowMember(show());
}

void gpiCompactListDataHost::Constr_ElData() {
  for (int lf = 0; lf < cur_lst->size; ++lf) {
    TAPtr tmp_lf = (TAPtr)cur_lst->FastEl_(lf);
    if (tmp_lf == NULL)	continue; // note: not supposed to have NULL values in lists
    TypeDef* tmp_td = tmp_lf->GetTypeDef();
    lst_data_el.Add(new gpiCompactList_ElData(tmp_td, tmp_lf));
  }
} 

void gpiCompactListDataHost::Constr_Final() {
  gpiMultiEditDataHost::Constr_Final();
  multi_body->resizeNames(); //temp: idatatable should do this automatically
}

void gpiCompactListDataHost::Constr_MultiBody() {
  inherited::Constr_MultiBody(); 
  Constr_ElData();
  Constr_ListData();
}


void gpiCompactListDataHost::Constr_ListData() {
  for (int i = 0; i < lst_data_el.size; ++i) {
    gpiCompactList_ElData* lf_el = lst_data_el.FastEl(i);
    String nm = String("[") + String(i) + "]: (" + lf_el->typ->name + ")";
    AddMultiRowName(i, nm, String(""));
    // note: the type better grok INLINE!!!!
    taiData* mb_dat = lf_el->typ->it->GetDataRep(this, NULL, multi_body->dataGridWidget(), NULL, taiData::flgInline);
    lf_el->data_el = mb_dat;
    AddMultiData(i, 1, mb_dat->GetRep());
  }
}

void gpiCompactListDataHost::GetValue_Membs() {
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
 // NOTE: we should always be able to do a GetValue, because we always rebuild
 // when data changes (ie, in program, or from another gui panel)
  if (rebuild) {
    taMisc::Error("Cannot apply changes: List size or elements have changed");
    return;
  }

  // first for the List-structure members
  GetValue_Membs_def();
  // then the List elements
  for (int lf=0;  lf < lst_data_el.size;  ++lf) {
    gpiCompactList_ElData* lf_el = lst_data_el.FastEl(lf);
    lf_el->typ->it->GetValue(lf_el->data_el, lf_el->cur_base);
    ((TAPtr)lf_el->cur_base)->UpdateAfterEdit();
  }
  cur_lst->UpdateAfterEdit();	// call here too!
  taiMisc::Update((TAPtr)cur_lst);
}

void gpiCompactListDataHost::GetImage_Membs() {
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
    RebuildMultiBody(); 
  } 

  // first for the List-structure members
  GetImage_Membs_def();

  // then the elements
  for (int lf = 0;  lf < lst_data_el.size;  ++lf) {
    gpiCompactList_ElData* lf_el = lst_data_el.FastEl(lf);
    lf_el->typ->it->GetImage(lf_el->data_el, lf_el->cur_base);
  }
}
/* TODO
int gpiCompactListDataHost::Edit() {
  if ((cur_lst != NULL) && (cur_lst->size > 100)) {
    int rval = taMisc::Choice("List contains more than 100 items (size = " +
			      String(cur_lst->size) + "), continue with Edit?",
			      "Ok", "Cancel");
    if (rval == 1) return 0;
  }
  return gpiMultiEditDataHost::Edit();
} */



//////////////////////////////////
//	gpiArrayEditDataHost	//
//////////////////////////////////


gpiArrayEditDataHost::gpiArrayEditDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: taiEditDataHost(base, typ_, read_only_, modal_, parent)
{
  // array items get their own memb, but they are handled by us
  membs.SetMinSize(membs.size + 1);
  array_set = membs.size - 1;
}

gpiArrayEditDataHost::~gpiArrayEditDataHost() {
  //anything?
}

void gpiArrayEditDataHost::ClearBody_impl() {
  taiEditDataHost::ClearBody_impl();
}

bool gpiArrayEditDataHost::ShowMember(MemberDef* md) const {
  if (md->name == "size")
    return true;
  else
    return inherited::ShowMember(md);
}

void gpiArrayEditDataHost::Constr_Data_Labels() {
  inherited::Constr_Data_Labels();
  Constr_AryData_Labels();
}

void gpiArrayEditDataHost::Constr_AryData_Labels() {
  iLabel* lbl = new iLabel("Array items", body);
  AddSectionLabel(-1, lbl,
    "the data items in the array, one per line");
  
  taArray_base* cur_ary = (taArray_base*)cur_base;
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i) {
    taiData* mb_dat = tit->GetDataRep(this, NULL, body);
    data_el(array_set).Add(mb_dat);
    QWidget* rep = mb_dat->GetRep();
    bool fill_hor = mb_dat->fillHor();
    String nm = String("[") + String(i) + "]";
    int idx = AddData(-1, rep, fill_hor);
    AddName(idx, nm, String(""), mb_dat);
  }
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

void gpiArrayEditDataHost::GetImage_Membs() {
  inherited::GetImage_Membs();
  taArray_base* cur_ary = (taArray_base*)cur_base;
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i) {
    taiData* mb_dat = data_el(array_set).SafeEl(i);
    if (mb_dat == NULL) return; // unexpected end
    tit->GetImage(mb_dat, cur_ary->FastEl_(i));
  }
}

void gpiArrayEditDataHost::GetValue_Membs() {
  inherited::GetValue_Membs();
  taArray_base* cur_ary = (taArray_base*)cur_base;
  MemberDef* eldm = typ->members.FindName("el");
  taiType* tit = eldm->type->GetNonPtrType()->it;
  for (int i = 0; i < cur_ary->size; ++i){
    taiData* mb_dat = data_el(array_set).SafeEl(i);
    if (mb_dat == NULL) return; // unexpected
    tit->GetValue(mb_dat, cur_ary->FastEl_(i));
  }
}


/////////////////////////////////
//	SArgEditDataHost	//
//////////////////////////////////

SArgEditDataHost::SArgEditDataHost(void* base, TypeDef* tp,  bool read_only_,
  bool modal_, QObject* parent)
:inherited(base, tp, read_only_, modal_, parent) 
{
}

bool SArgEditDataHost::ShowMember(MemberDef* md) const {
  if (md->name == "size")
    return true;
  else
    return inherited::ShowMember(md);
}

void SArgEditDataHost::Constr_AryData() {
  SArg_Array* cur_ary = (SArg_Array*)cur_base;
  cur_ary->UpdateAfterEdit();
  MemberDef* eldm = typ->members.FindName("el");
  taiType* it = eldm->type->GetNonPtrType()->it;
  QWidget* rep;
  for (int i=0; i < cur_ary->size; ++i) {
    taiData* mb_dat = it->GetDataRep(this, NULL, body);
    data_el(array_set).Add(mb_dat);
    rep = mb_dat->GetRep();
    bool fill_hor = mb_dat->fillHor();
    String nm = String("[") + String(i) + "]";
    String lbl = cur_ary->labels[i];
    if (!lbl.empty())
      nm = lbl + nm;
    int idx = AddData(-1, rep, fill_hor);
    AddName(idx, nm, String(""), mb_dat);
  }
}


//////////////////////////////////
//	gpiSelectEdit		//
//////////////////////////////////

gpiSelectEditDataHost::gpiSelectEditDataHost(void* base, TypeDef* td, bool read_only_,
  	bool modal_, QObject* parent)
: taiEditDataHost(base, td, read_only_, modal_, parent)
{
  sele = (SelectEdit*)base;
  // we use the default membs, and add one for the sele guys
  sele_set = membs.size; // index of new guy we add:
  membs.SetMinSize(membs.size + 1);
  mnuRemoveMember = NULL;

}

gpiSelectEditDataHost::~gpiSelectEditDataHost() {
}

bool gpiSelectEditDataHost::ShowMember(MemberDef* md) const {
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
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  mnuRemoveMember = NULL;
  if (menu) {
    menu->Reset();
  }
  inherited::ClearBody_impl();
}

void gpiSelectEditDataHost::Constr_Body() {
  if (rebuild_body) {
    meth_el.Reset();
  }
  inherited::Constr_Body();
  mnuRemoveMember = new QMenu();
  
  iLabel* lbl = new iLabel("Select Edit items", body);
  AddSectionLabel(-1, lbl,
    "the members that have been added from other objects");

  // delete any previous sele members
  memb_el(sele_set).Reset();
  String nm;
  String help_text;
  for (int i = 0; i < sele->members.size; ++i) {
    MemberDef* md = sele->members.FastEl(i);
    if (md->im == NULL) continue; // shouldn't happen
    taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
    memb_el(sele_set).Add(md);
    data_el(sele_set).Add(mb_dat);
    QWidget* data = mb_dat->GetRep();
    int row = AddData(-1, data);

    help_text = "";
    String new_lbl = sele->config.mbr_labels.FastEl(i);
//    String new_lbl = sele->GetMbrLabel(i);
    GetName(md, nm, help_text); //note: we just call this to get the help text
    if (!new_lbl.empty())
      nm = new_lbl + " " + nm;
    AddName(row, nm, help_text, mb_dat, md);
    MakeMenuItem(mnuRemoveMember, nm, i, i, SLOT(mnuRemoveMember_select(int)));
  }
  // we deleted the normally not-deleted methods, so redo them here
  if (rebuild_body) {
    Constr_Methods();
    frmMethButtons->setHidden(!showMethButtons());
  }
}

void gpiSelectEditDataHost::Constr_Methods() {
  inherited::Constr_Methods();
  QMenu* mnuRemoveMethod_menu = new QMenu();
  QMenu* mnuRemoveMethod_menu_but = new QMenu();
  QMenu* mnuRemoveMethod_but = new QMenu();
  if (cur_menu != NULL) {// for safety... cur_menu should be the SelectEdit menu
    cur_menu->AddSep();
/*TODO Qt4    QMenu* menu_tmp = cur_menu->rep_popup();
    menu_tmp->insertItem("Remove field", mnuRemoveMember);
    taiMenu* taimen_tmp = cur_menu->AddSubMenu("Remove function");
    menu_tmp = taimen_tmp->rep_popup();
    menu_tmp->insertItem("Main menu", mnuRemoveMethod_menu);
    menu_tmp->insertItem("Menu buttons", mnuRemoveMethod_menu_but);
    menu_tmp->insertItem("Buttons", mnuRemoveMethod_but); */
  }


  for (int i = 0; i < sele->methods.size; ++i) {
    MethodDef* md = sele->methods.FastEl(i);
    if(md->im == NULL) continue;
    taiMethodData* mth_rep = md->im->GetMethodRep(sele->meth_bases.FastEl(i), this, NULL, frmMethButtons);
    if (mth_rep == NULL) continue;
    meth_el.Add(mth_rep);
    QMenu* rem_men = NULL; // remove menu

    String men_nm = sele->config.meth_labels.FastEl(i);
    if (mth_rep->is_menu_item) {
      //NOTE: for seledit functions, we never place them on the last menu or button, because that may
      // make no sense -- the label specifies the place, or Actions if no label
      if (md->HasOption("MENU_BUTTON")) { // menu button item
        if (men_nm.empty())
          men_nm = "Actions";
        cur_menu_but = ta_menu_buttons.FindName(men_nm);
        if (cur_menu_but == NULL) {
          cur_menu_but = taiActions::New
            (taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
                    NULL, this, NULL, widget());
          cur_menu_but->setLabel(men_nm);
          DoAddMethButton((QPushButton*)cur_menu_but->GetRep()); // rep is the button for buttonmenu
          ta_menu_buttons.Add(cur_menu_but);
          rem_men = new QMenu();
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
          rem_men = new QMenu();
          mnuRemoveMethod_menu->insertItem(men_nm, rem_men);
        } else {
          rem_men = FindMenuItem(mnuRemoveMethod_menu, men_nm);
        }
        mth_rep->AddToMenu(cur_menu);
      }
      //note: we assume mth_rep methods will use GetLabel() to provide the menu text
      if (rem_men) MakeMenuItem(rem_men, md->GetLabel(), i, i, SLOT(mnuRemoveMethod_select(int)));
    }
    else {			// BUTTON
      if (men_nm.nonempty())
        men_nm += " " + md->GetLabel();
      else
	men_nm = md->GetLabel();
      AddMethButton(mth_rep, men_nm);
      MakeMenuItem(mnuRemoveMethod_but, men_nm, i, i, SLOT(mnuRemoveMethod_select(int)));
    }
  }
}

void gpiSelectEditDataHost::DoRemoveSelEdit() {
   // removes the sel_item_index item -- need to reduce by 1 because of pre-existing items on seledit dialog
  int sel_item_index = memb_el(sele_set).FindEl(sel_item_md);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
#ifdef DEBUG
  else
    taMisc::Error("gpiSelectEditDataHost::DoRemoveSelEdit: could not find item index from MethodDef");
#endif
}

void gpiSelectEditDataHost::FillLabelContextMenu_SelEdit(iLabel* sender,
  QMenu* menu, int& last_id)
{
  MemberDef* md = (MemberDef*)qvariant_cast<ta_intptr_t>(sender->userData());
  int sel_item_index = memb_el(sele_set).FindEl(md);
  if (sel_item_index < 0) return; // only add for user-added items
  menu->insertItem("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()), 0, ++last_id);
}

QMenu* gpiSelectEditDataHost::FindMenuItem(QMenu* par_menu, const char* label) {
  int id = 0;
  for (uint i = 0; i < par_menu->count(); ++i) {
    id = par_menu->idAt(i);
    if (par_menu->text(id) == label)
      return (QMenu*)par_menu->findItem(id);
  }
  return NULL;
}

void gpiSelectEditDataHost::GetValue_Membs_def() {
  inherited::GetValue_Membs_def(); // does defaults
  GetValue_SeleMembs();
  sele->UpdateAllBases();
}

void gpiSelectEditDataHost::GetValue_SeleMembs() {
  bool first_diff = true;
  for (int i=0; i < memb_el(sele_set).size; i++) {
    MemberDef* md = memb_el(sele_set).FastEl(i);
    TAPtr bs = sele->mbr_bases.SafeEl(i);
    if(bs == NULL) continue;
    taiData* mb_dat = data_el(sele_set).SafeEl(i);
    if (mb_dat == NULL) break; // prob shouldn't happen!
    md->im->GetMbrValue(mb_dat, (void*)bs, first_diff);
    if(!first_diff) {		// always reset!
      taiMember::EndScript((void*)bs);
      first_diff = true;
    }
  }
}

void gpiSelectEditDataHost::GetImage_Membs_def() {
  inherited::GetImage_Membs_def(); // does defaults
  GetImage_SeleMembs();
}

void gpiSelectEditDataHost::GetImage_SeleMembs()
{
  for (int i = 0; i < memb_el(sele_set).size; ++i) {
    MemberDef* md = memb_el(sele_set).FastEl(i);
    TAPtr bs = sele->mbr_bases.SafeEl(i);
    if (bs == NULL) continue;
    taiData* mb_dat = data_el(sele_set).SafeEl(i);
    if (mb_dat == NULL) break; // prob shouldn't happen!
    md->im->GetImage(mb_dat, (void*)bs);
  }
}

void gpiSelectEditDataHost::MakeMenuItem(QMenu* menu, const char* name, int index, int param, const char* slot) {
    menu->insertItem(name, this, slot, 0, index);
    menu->setItemParameter(index, param);
}


void gpiSelectEditDataHost::mnuRemoveMember_select(int idx) {
  sele->RemoveField(idx);
}

void gpiSelectEditDataHost::mnuRemoveMethod_select(int idx) {
  sele->RemoveFun(idx);
}


//////////////////////////////////
//  DocEditDataHost		//
//////////////////////////////////

DocEditDataHost::DocEditDataHost(void* base, TypeDef* typ_, bool read_only_,
      bool modal_, QObject* parent)
: inherited(base, typ_, read_only_, modal_, parent)
{
  init();
}

void DocEditDataHost::init() {
  tedHtml = NULL;
}

taDoc* DocEditDataHost::doc() const {
  return static_cast<taDoc*>(cur_base);
}

void DocEditDataHost::Constr_Body() {
  //  taiDataHost::Constr_Body();
}

void DocEditDataHost::Constr_Box() {
  // Html tab
  tedHtml = new iTextEdit;
  tedHtml->setAcceptRichText(false); // is the raw html as text
  if (read_only) {
    tedHtml->setReadOnly(true);
  } else { // r/w
    connect(tedHtml, SIGNAL(textChanged()), 
	    this, SLOT(Changed()) );
  }
  vblDialog->addWidget(tedHtml, 1); // gets all the space

  //  inherited::Constr_Box();
}

void DocEditDataHost::GetImage_Membs() {
  //  inherited::GetImage_Membs();
  taDoc* doc = this->doc();
  if (!doc) return; // ex. for zombies
  
  QString text = doc->text; 
  // to avoid the guy always jumping to the top after edit
  // we compare, and don't update if the same
  if (text == tedHtml->toPlainText()) return; 
  tedHtml->clear();
  tedHtml->insertPlainText(text); // we set the html as text
}

void DocEditDataHost::GetValue_Membs() {
  //  inherited::GetValue_Membs();
  taDoc* doc = this->doc();
  if (!doc) return; // ex. for zombies
  
  doc->text = tedHtml->toPlainText();
  doc->UpdateAfterEdit();
  taiMisc::Update(doc);
}


//////////////////////////
//    iDocEditDataPanel	//
//////////////////////////

iDocEditDataPanel::iDocEditDataPanel(taiDataLink* dl_)
:inherited(dl_)
{
  taDoc* doc_ = doc();
  de = NULL;
  if (doc_) {
    de = new DocEditDataHost(doc_, doc_->GetTypeDef());
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = doc_->GetEditColorInherit(ok);
      if (ok) de->setBgColor(bgcol);
    }
    de->ConstrEditControl();
    setCentralWidget(de->widget()); //sets parent
    setButtonsWidget(de->widButtons);
  }
}

iDocEditDataPanel::~iDocEditDataPanel() {
  if (de) {
    delete de;
    de = NULL;
  }
}

bool iDocEditDataPanel::ignoreDataChanged() const {
  return !isVisible();
}

void iDocEditDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
  // not in this case!
}

bool iDocEditDataPanel::HasChanged() {
  if (de) return de->HasChanged();
  else return false;
}

void iDocEditDataPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iDocEditDataPanel::UpdatePanel_impl() {
  if (de) de->ReShow_Async();
}

void iDocEditDataPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (de && de->HasChanged()) {
    de->Apply();
  }
}


//////////////////////////
// tabDocViewType	//
//////////////////////////

int tabDocViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taDoc))
    return (inherited::BidForView(td) +1);
  return 0;
}

void tabDocViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // doc view is default
  iDocDataPanel* cp = new iDocDataPanel();
  cp->setDoc((taDoc*)dl_->data());
  DataPanelCreated(cp);

  // then source editor
  iDocEditDataPanel* dp = new iDocEditDataPanel(dl_);
  DataPanelCreated(dp);

  // then standard properties
  inherited::CreateDataPanel_impl(dl_);
}
